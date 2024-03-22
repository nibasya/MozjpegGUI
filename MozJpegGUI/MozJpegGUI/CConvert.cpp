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
	m_pThread(NULL), m_InData(NULL), m_OutData(NULL), m_pMetadata(NULL)
{
	size_t len = fileName.GetLength() + 5;
	TCHAR* buff = new TCHAR[len];
	_tcscpy_s(buff, len, fileName);
	PathCchRemoveExtension(buff, len);
	_tcscat_s(buff, len, _T(".jpg"));

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

		// check if the source file is jpeg file
		bool isSrcJpg = false;
		CString ext = PathFindExtension(m_Filename);
		ext = ext.MakeUpper();
		if (ext == _T(".JPEG") || ext == _T(".JPG")) {
			isSrcJpg = true;
		}

		if (!isSrcJpg) {
			if (!ReadMetadata()) {
				_RPTFT0(_T("Aborting from ReadMetadata()\n"));
				throw EConvertError::EIgnore;
			}
		}

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
		if(!isSrcJpg){
			if (!WriteMetadata()) {
				_RPTFT0(_T("Aborting from WriteMetadata()\n"));
				throw EConvertError::EIgnore;
			}
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
	catch (CJpegException e) {
		m_pParent->m_Paused = true;
		CString str;
		str.Format(_T("Error code %d: %s"), e.e, e.str);
		MessageBox(m_pParent->m_hWnd, str, _T("Error"), MB_OK);
		if (m_InData) {
			free(m_InData);
			m_InData = NULL;
		}
		if (m_OutData) {
			free(m_OutData);
			m_OutData = NULL;
		}
		m_pParent->Abort();
	}
	free(m_pMetadata);
	m_pMetadata = NULL;

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
			Pause();
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
			Pause();
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

void CConvert::Pause()
{
	while (m_pParent->m_Paused) {
		Sleep(500);
		if (WaitForSingleObject(m_pParent->m_SyncAbort.m_hObject, 0) == WAIT_OBJECT_0) {
			break;
		}
	}
}

void CConvert::ErrorHelper(const TCHAR* _rptft0, const UINT ID, const TCHAR* resourceName, CString& additionalStr)
{
	CString str;
	_RPTFT0(_rptft0);
	if (!str.LoadString(ID)) {
		OutputDebugString(CString(_T("Failed to load resource: ")) + resourceName);
	}
	m_pParent->MessageBox(str + additionalStr);
}

bool CConvert::ReadMetadata()
{
	using namespace Gdiplus;

	UINT size = 0;
	const int maxPropTypeSize = 100;
	WCHAR strPropType[maxPropTypeSize] = L"";
	Status stat;
	Bitmap* pBmp = NULL;
	CString str;

	try {
		// open file
		pBmp = new Bitmap(m_Filename);
		if (pBmp == NULL) {
			ErrorHelper(_T("Failed to load file for GDI+\n"), IDS_ERR_FAILED_TO_LOAD_FILE_FOR_GDIPLUS, _T("IDS_ERR_FAILED_TO_LOAD_FILE_FOR_GDIPLUS"), m_Outputname);
			throw EConvertError::EIgnore;
		}

		// read props
		stat = pBmp->GetPropertySize(&size, &m_MetaCount);
		if (stat != Status::Ok) {
			ErrorHelper(_T("Failed to get metadata size\n"), IDS_ERR_FAILED_TO_GET_METADATA_SIZE, _T("IDS_ERR_FAILED_TO_GET_METADATA_SIZE"), m_Outputname);
			throw EConvertError::EIgnore;
		}
		m_pMetadata = (PropertyItem*)malloc(size);
		if (m_pMetadata == NULL) {
			ErrorHelper(_T("Not enough memory for metadata\n"), IDS_ERR_NOT_ENOUGH_MEMORY_FOR_METADATA, _T("IDS_ERR_NOT_ENOUGH_MEMORY_FOR_METADATA"), m_Outputname);
			throw EConvertError::EIgnore;
		}
		stat = pBmp->GetAllPropertyItems(size, m_MetaCount, m_pMetadata);
		if (stat != Status::Ok) {
			ErrorHelper(_T("Failed to read metadata\n"), IDS_ERR_FAILED_TO_READ_METADATA, _T("IDS_ERR_FAILED_TO_READ_METADATA"), m_Outputname);
			throw EConvertError::EIgnore;
		}
	}
	catch (EConvertError) {
		delete pBmp;
		pBmp = NULL;
		free(m_pMetadata);
		m_pMetadata = NULL;
		return false;
	}

	delete pBmp;
	return true;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	if(GetImageEncoders(num, size, pImageCodecInfo) != Gdiplus::Status::Ok)
		return -1;	// Failure

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

bool CConvert::WriteMetadata()
{
	using namespace Gdiplus;

	UINT size = 0;
	UINT count = 0;
	const int maxPropTypeSize = 100;
	WCHAR strPropType[maxPropTypeSize] = L"";
	Status stat;
	IStream* input = NULL;
	Bitmap* pBmp = NULL;
	IStream* output = NULL;

	try {
		// open input memory
		input = SHCreateMemStream(m_OutData, static_cast<UINT>(m_OutSize));
		if (input == NULL) {
			ErrorHelper(_T("Failed to create memory stream for input\n"), IDS_ERR_FAILED_TO_CREATE_MEMORY_STREAM_FOR_INPUT, _T("IDS_ERR_FAILED_TO_CREATE_MEMORY_STREAM_FOR_INPUT"), m_Outputname);
			throw EConvertError::EIgnore;
		}
		// open image
		pBmp = new Bitmap(input);
		if (pBmp == NULL) {
			ErrorHelper(_T("Failed to load file for GDI+\n"), IDS_ERR_FAILED_TO_LOAD_FILE_FOR_GDIPLUS, _T("IDS_ERR_FAILED_TO_LOAD_FILE_FOR_GDIPLUS"), m_Outputname);
			throw EConvertError::EIgnore;
		}

		// write props
		for (UINT count = 0; count < m_MetaCount; count++) {
			stat = pBmp->SetPropertyItem(m_pMetadata + count);
			if (stat != Status::Ok) {
				ErrorHelper(_T("Failed to set metadata\n"), IDS_ERR_FAILED_TO_SET_METADATA, _T("IDS_ERR_FAILED_TO_SET_METADATA"), m_Outputname);
				throw EConvertError::EIgnore;
			}
		}

		// save file
		output = SHCreateMemStream(NULL, 0);
		if (output == NULL) {
			ErrorHelper(_T("Failed to create memory stream for output\n"), IDS_ERR_FAILED_TO_CREATE_MEMORY_STREAM_FOR_OUTPUT, _T("IDS_ERR_FAILED_TO_CREATE_MEMORY_STREAM_FOR_OUTPUT"), m_Outputname);
			throw EConvertError::EIgnore;
		}
		CLSID  encoderClsid;
		if (GetEncoderClsid(_T("image/jpeg"), &encoderClsid) < 0) {
			ErrorHelper(_T("image/jpeg encoder not installed for GDI+\n"), IDS_ERR_IMAGE_JPEG_ENCODER_NOT_INSTALLED_FOR_GDIPLUS, _T("IDS_ERR_IMAGE_JPEG_ENCODER_NOT_INSTALLED_FOR_GDIPLUS"), CString());
			throw EConvertError::EIgnore;
		};
		if (pBmp->Save(output, &encoderClsid) != Gdiplus::Status::Ok) {
			ErrorHelper(_T("Failed to write metadata\n"), IDS_ERR_FAILED_TO_WRITE_METADATA, _T("IDS_ERR_FAILED_TO_WRITE_METADATA"), m_Outputname);
			throw EConvertError::EIgnore;
		}

		// copy output data into m_Outdata
		free(m_OutData);
		STATSTG statStg;
		if (output->Stat(&statStg, STATFLAG::STATFLAG_NONAME) != S_OK) {
			ErrorHelper(_T("Failed to get status of the output stream"), IDS_ERR_FAILED_TO_GET_STATUS_OF_OUTPUT_STREAM, _T("IDS_ERR_FAILED_TO_GET_STATUS_OF_OUTPUT_STREAM"), m_Outputname);
			throw EConvertError::EIgnore;
		};
		m_OutSize = statStg.cbSize.QuadPart;
		m_OutData = reinterpret_cast<BYTE*>(malloc((size_t)m_OutSize));
		if (m_OutData == NULL) {
			ErrorHelper(_T("Not enough memory for metadata\n"), IDS_ERR_NOT_ENOUGH_MEMORY_FOR_METADATA, _T("IDS_ERR_NOT_ENOUGH_MEMORY_FOR_METADATA"), m_Outputname);
			throw EConvertError::EIgnore;
		}
		ULONG readSize;
		LARGE_INTEGER beginning = { 0 };
		if (output->Seek(beginning, STREAM_SEEK_SET, NULL) != S_OK) {
			ErrorHelper(_T("Failed to seek output stream"), IDS_ERR_FAILED_TO_SEEK_OUTPUT_STREAM, _T("IDS_ERR_FAILED_TO_SEEK_OUTPUT_STREAM"), m_Outputname);
			throw EConvertError::EIgnore;
		}
		HRESULT hRes;
		if ((hRes = output->Read(m_OutData, (ULONG)m_OutSize, &readSize)) != S_OK) {
			ErrorHelper(_T("Failed to read from output stream"), IDS_ERR_FAILED_TO_READ_FROM_OUTPUT_STREAM, _T("IDS_ERR_FAILED_TO_READ_FROM_OUTPUT_STREAM"), m_Outputname);
			throw EConvertError::EIgnore;
		}
		if (readSize != m_OutSize) {
			ErrorHelper(_T("Read size is not equal to stream size"), IDS_ERR_READ_SIZE_IS_NOT_EQUAL_TO_STREAM_SIZE, _T("IDS_ERR_READ_SIZE_IS_NOT_EQUAL_TO_STREAM_SIZE"), m_Outputname);
			throw EConvertError::EIgnore;
		}
	}
	catch (EConvertError) {
		input->Release();
		delete pBmp;
		output->Release();
		return false;
	}

	input->Release();
	delete pBmp;
	output->Release();
	return true;
}
