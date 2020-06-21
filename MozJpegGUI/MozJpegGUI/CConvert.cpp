#include "pch.h"
#include "resource.h"
#include "CProgressDlg.h"
#include "RPTT.h"
#include <PathCch.h>
#include <vector>
#include "mozJpeg/cjpeg.h"
#include "CSemaphoreWithCounter.h"
#include "GetLastErrorToString.h"
#include "CConvert.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

CConvert::CConvert(CProgressDlg* pParent, CString& fileName, CString& outDir, CString& options) :
	m_pParent(pParent), m_Filename(fileName), m_CommandOptions(options), m_OutDir(outDir), 
	m_pThread(NULL), m_InData(NULL), m_OutData(NULL)
{
	size_t len = fileName.GetLength() + 5;
	TCHAR* buff = new TCHAR[len];
	_tcscpy_s(buff, len, fileName);
	PathCchRemoveExtension(buff, len);
	PathCchAddExtension(buff, len, _T(".jpg"));

	CString name = PathFindFileName(buff);
	size_t outlen = len + outDir.GetLength();
	TCHAR* out = new TCHAR[outlen];
	_tcscpy_s(out, outlen, outDir);
	PathCchAppend(out, outlen, name);
	m_Outputname = out;
	delete[] out;
	delete[] buff;
};


UINT __cdecl CConvert::MainThread(LPVOID pData)
{
	UINT ret;
	ret = static_cast<CConvert*>(pData)->Main();
	delete static_cast<CConvert*>(pData);
	return ret;
};


UINT CConvert::Main()
{
	try {
		if (!ReadFile()) {
			_RPTFT0(_T("Aborting from ReadFile()\n"));
			throw EConvertError::EIgnore;
		}

		CConvertLock lockReadBuff(m_pParent->m_SyncAbort, m_pParent->m_pSyncReadBuff);

		if(!m_pParent->AddThread())
			throw EConvertError::EIgnore;

		CConvertLock lockCPU(m_pParent->m_SyncAbort, m_pParent->m_pSyncCPU);

		if (!Convert()) {
			_RPTFT0(_T("Aborting from Convert()\n"));
			throw EConvertError::EIgnore;
		}

		lockCPU.Unlock();

		if (m_pParent->m_fCopyIfSmaller && m_OutSize > m_InSize) {
			size_t outlen = m_Filename.GetLength() + 5 + m_OutDir.GetLength();
			TCHAR* out = new TCHAR[outlen];
			_tcscpy_s(out, outlen, m_OutDir);
			PathCchAppend(out, outlen, PathFindFileName(m_Filename));
			m_Outputname = out;
			delete[] out;

			free(m_OutData);
			m_OutData = m_InData;
			m_InData = NULL;
			m_OutSize = m_InSize;
		}
		if (!WriteFile()) {
			_RPTFT0(_T("Aborting from WriteFile()\n"));
			throw EConvertError::EIgnore;
		}
		if (m_InData) {
			free(m_InData);
			m_InData = NULL;
		}

		lockReadBuff.Unlock();
	}
	catch (EConvertError e) {
		if (e == EConvertError::EAbort) {
			_RPTFT0(_T("Aborting\n"));
		}
		if (m_InData) {
			free(m_InData);
			m_InData = NULL;
		}
		if (m_OutData) {
			free(m_OutData);
			m_OutData = NULL;
		}
	}
	m_pParent->ThreadEnd(m_pThread);
	return 0;
}


bool CConvert::ReadFile()
{
	CString str;
	bool retry;

	HANDLE hFileRead = INVALID_HANDLE_VALUE;

	CConvertLock lock(m_pParent->m_SyncAbort, m_pParent->m_pSyncHDD);

	try {
		retry = true;
		while (retry) {
			hFileRead = CreateFile(m_Filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL & FILE_FLAG_SEQUENTIAL_SCAN, NULL);
			if (hFileRead != INVALID_HANDLE_VALUE) {
				break;
			}
			else {
				_RPTFT0(_T("Failed to open input file\n"));
				if (!str.LoadString(IDS_ERR_FAILED_TO_READ_FILE)) {
					OutputDebugString(_T("Failed to load resource: IDS_ERR_FAILED_TO_READ_FILE"));
				}
				// TODO: show error file name
				int ret = m_pParent->MessageBox(str, 0, MB_ABORTRETRYIGNORE);
				switch (ret) {
				case IDABORT:
					throw EConvertError::EAbort;
					break;
				case 0:
				case IDRETRY:
					break;
				case IDIGNORE:
					throw EConvertError::EIgnore;
					break;
				}
			}
		}
//		_RPTTN(_T("Opened input file: %s\n"), m_Filename);

		LARGE_INTEGER filesize;
		GetFileSizeEx(hFileRead, &filesize);
#ifdef _M_IX86
		if (filesize.HighPart > 0 || filesize.LowPart>_HEAP_MAXREQ) {
			_RPTFT0(_T("Input file is too large for win32 system\n"));
			if (!str.LoadString(IDS_ERR_READFILE_TOO_LARGE)) {
				OutputDebugString(_T("Failed to load resource: IDS_ERR_READFILE_TOO_LARGE"));
			}
			int ret = m_pParent->MessageBox(str, 0, MB_YESNO);
			if (ret == IDYES) {
				throw EConvertError::EAbort;
			}
			else {
				throw EConvertError::EIgnore;
			}
		}
		m_InSize = filesize.LowPart;
		m_InData = static_cast<BYTE*>(malloc(filesize.LowPart));
#else
		m_InSize = filesize.QuadPart;
		m_InData = new BYTE[filesize.QuadPart];
#endif
		if (m_InData == NULL) {
			_RPTFT0(_T("Failed to malloc buffer for reading input file\n"));
			if (!str.LoadString(IDS_ERR_NOT_ENOUGH_MEMORY)) {
				OutputDebugString(_T("Failed to load resource: IDS_ERR_NOT_ENOUGH_MEMORY"));
			}
			m_pParent->MessageBox(str);
			throw EConvertError::EAbort;
		}
		DWORD readfilesize;
		LONGLONG readPos = 0;
		while (readPos < m_InSize) {
			DWORD sizePerOneRead = (m_InSize - readPos) > DWORD_MAX ? DWORD_MAX : static_cast<DWORD>(m_InSize - readPos);
			if (::ReadFile(hFileRead, m_InData + readPos, sizePerOneRead, &readfilesize, NULL) == FALSE || readfilesize != sizePerOneRead) {
				_RPTFT0(_T("Failed to read input file\n"));
				if (!str.LoadString(IDS_ERR_FAILED_TO_READ_FILE)) {
					OutputDebugString(_T("Failed to load resource: IDS_ERR_FAILED_TO_READ_FILE"));
				}
				int ret = m_pParent->MessageBox(str, 0, MB_ABORTRETRYIGNORE);
				switch (ret) {
				case IDABORT:
					throw EConvertError::EAbort;
					break;
				case 0:
				case IDRETRY:
					break;
				case IDIGNORE:
					throw EConvertError::EIgnore;
					break;
				}
			}
			readPos += readfilesize;
		}
		CloseHandle(hFileRead);
		hFileRead = INVALID_HANDLE_VALUE;
//		_RPTTN(_T("Closed input file: %s\n"), m_Filename);

	}
	catch (EConvertError e){
		if (hFileRead != INVALID_HANDLE_VALUE) {
			CloseHandle(hFileRead);
			_RPTTN(_T("Closed input file: %s\n"), m_Filename);
		}
		if (m_InData != NULL) {
			free(m_InData);
			m_InData = NULL;
		}

		switch (e) {
		case EConvertError::EAbort:
			m_pParent->Abort();
			return false;
			break;
		case EConvertError::EIgnore:
			return false;
			break;
		}
	}

	return true;
}


bool CConvert::Convert()
{
	int argc;
	void** argv;
	int ret;
	CreateArgs(&argc, &argv);
	CJpeg jpeg;
	jpeg.m_pSyncAbort = &m_pParent->m_SyncAbort;
	jpeg.m_Paused = &m_pParent->m_Paused;
	ret = jpeg.cjpeg_main(argc, (char**)argv);
	int i;
	for (i = 0; i < argc; i++) {
		delete[] argv[i];
	}
	delete[] argv;
	m_OutData = jpeg.m_outbuffer;
	m_OutSize = jpeg.m_outsize;
	return ret;
}


bool CConvert::WriteFile()
{
	CConvertLock lock(m_pParent->m_SyncAbort, m_pParent->m_pSyncHDD);

	HANDLE hFileWrite = INVALID_HANDLE_VALUE;
	CString str;
	bool retry;

	try {
		retry = true;
		if (m_pParent->m_fOverwrite) {
			while (retry) {
				hFileWrite = CreateFile(m_Outputname, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFileWrite != INVALID_HANDLE_VALUE) {
					break;
				}
				else {
					_RPTTN(_T("Failed to create/overwrite output file: %s\n"), m_Outputname);
					if (!str.LoadString(IDS_ERR_FAILED_TO_OVERWRITE_OUTPUT_FILE)) {
						OutputDebugString(_T("Failed to load resource: IDS_ERR_FAILED_TO_OVERWRITE_OUTPUT_FILE"));
					}
					str += _T("\n") + m_Outputname;
					int ret = m_pParent->MessageBox(str, 0, MB_RETRYCANCEL);
					switch (ret) {
					case IDCANCEL:
						throw EConvertError::EAbort;
						break;
					case 0:
					case IDRETRY:
						break;
					}
				}
			}
		}
		else {
			while (retry) {
				hFileWrite = CreateFile(m_Outputname, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFileWrite != INVALID_HANDLE_VALUE) {
					break;
				}
				else {
					_RPTTN(_T("Failed to create output file: %s\n"), m_Outputname);
					if (!str.LoadString(IDS_ERR_FAILED_TO_CREATE_OUTPUT_FILE)) {
						OutputDebugString(_T("Failed to load resource: IDS_ERR_FAILED_TO_CREATE_OUTPUT_FILE"));
					}
					str += _T("\n") + m_Outputname;
					int ret = m_pParent->MessageBox(str, 0, MB_RETRYCANCEL);
					switch (ret) {
					case IDCANCEL:
						throw EConvertError::EAbort;
						break;
					case 0:
					case IDRETRY:
						break;
					}
				}
			}
		}

		DWORD writefilesize=0;
		LONGLONG writePos=0;
		do {
			DWORD writeSize = (m_OutSize - writePos) > DWORD_MAX ? DWORD_MAX : static_cast<DWORD>(m_OutSize - writePos);
			if (::WriteFile(hFileWrite, m_OutData + writePos, writeSize, &writefilesize, NULL) != TRUE) {
				_RPTFT0(_T("Failed to write output file\n"));
				if (!str.LoadString(IDS_ERR_FAILED_TO_WRITE_FILE)) {
					OutputDebugString(_T("Failed to load resource: IDS_ERR_FAILED_TO_WRITE_FILE"));
				}
				m_pParent->MessageBox(str+m_Outputname);
				throw EConvertError::EAbort;
			}
			writePos += writefilesize;
		} while (writePos != m_OutSize);

		CloseHandle(hFileWrite);
		hFileWrite = INVALID_HANDLE_VALUE;
		free(m_OutData);
		m_OutData = NULL;
		_RPTTN(_T("Closed write file: %s\n"), m_Outputname);
	}
	catch (EConvertError e) {
		if (m_OutData != NULL) {
			free(m_OutData);
			m_OutData = NULL;
		}
		if (hFileWrite != INVALID_HANDLE_VALUE) {
			CloseHandle(hFileWrite);
			_RPTTN(_T("Closed write file: %s\n"), m_Outputname);
		}

		switch (e) {
		case EConvertError::EAbort:
			m_pParent->Abort();
			return false;
			break;
		case EConvertError::EIgnore:
			return false;
			break;
		}
	}

	return true;
}

const unsigned int SYNC_ABORT = 0;
const unsigned int SYNC_OBJ = 1;

CConvertLock::CConvertLock(CSyncObject& syncAbort, CSemaphoreWithCounter* pSyncObj):
	m_pLock(NULL), m_Locked(false)
{
	m_SyncObj[SYNC_ABORT] = &syncAbort;
	m_SyncObj[SYNC_OBJ] = pSyncObj;

	m_pLock = new CMultiLock(m_SyncObj, 2);
	Lock();
}

CConvertLock::~CConvertLock()
{
	delete m_pLock;
}

DWORD CConvertLock::Lock()
{
	DWORD ret;

	ret = m_pLock->Lock(INFINITE, FALSE);
	if (ret == SYNC_ABORT) {
		_RPTFT0(_T("Aborting from Lock\n"));
		throw CConvert::EConvertError::EAbort;
	}
	static_cast<CSemaphoreWithCounter*>(m_SyncObj[SYNC_OBJ])->AddCount();
	m_Locked = true;
	return ret;
}

DWORD CConvertLock::Unlock()
{
	if (!m_Locked) {
		return FALSE;
	}
	BOOL ret;
	ret = m_pLock->Unlock();
	if (ret != TRUE) {
		GetLastErrorToString().CreateMsg();
	}
	else {
		m_Locked = false;
	}
	return ret;
}


void CConvert::CreateArgs(int* argc, void*** argv)
{
	CString cmd = m_CommandOptions;
	int curpos = 0;
	CString token;
	std::vector<CString> vec;

	vec.push_back(CString(_T("mozjpegGUI.exe")));
	vec.push_back(CString(_T("-outfile")));
	vec.push_back(m_Outputname);

	token = cmd.Tokenize(_T(" "), curpos);
	while (token != _T("")) {
		vec.push_back(token);
		token = cmd.Tokenize(_T(" "), curpos);
	}

	vec.push_back(m_Filename);

	*argc = static_cast<int>(vec.size());
	*argv = new void*[*argc];
	int i;

	for (i = 0; i < *argc; i++) {
		CStringA aBuff = CT2A(vec[i]);
		(*argv)[i] = new char[aBuff.GetLength() + 1];
		strcpy_s(static_cast<char *>((*argv)[i]), aBuff.GetLength()+1, aBuff);
	}
}
