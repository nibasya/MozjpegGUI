#include "pch.h"
#include "CManager.h"
#include "CReadFile.h"

CReadFile::CReadFile() {
	m_State = INITIAL;
}


// If name does not exists, return false and do nothing
bool CReadFile::SetFilename(CString name)
{
	if (!PathFileExists(name))
		return false;
	m_Filename = name;
	return true;
}


// Start the thread. When process is completed, a message will be thrown to pWnd.
void CReadFile::Start(CWnd* pWnd)
{
	m_pWnd = pWnd;
	m_State = RUNNING;
	AfxBeginThread(Thread, this);
}


UINT __cdecl CReadFile::Thread(LPVOID pData)
{
	CReadFile* p = static_cast<CReadFile*>(pData);
	WCHAR tempDir[MAX_PATH], tempFile[MAX_PATH];
	::GetTempPathW(MAX_PATH, tempDir);
	::GetTempFileNameW(tempDir, _T("MJG"), 0, tempFile);
	p->m_TmpFilename = tempFile;

	HANDLE hFileRead = CreateFile(p->m_Filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileRead == INVALID_HANDLE_VALUE) {
		p->m_State = FAIL;
		SendMessage(p->m_pWnd->m_hWnd, WM_USER_READFILE, reinterpret_cast<LPARAM>(p), FAIL_OPEN_READ);
	}

	HANDLE hFileWrite = CreateFile(
		p->m_TmpFilename, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_TEMPORARY |
		FILE_ATTRIBUTE_HIDDEN |
		FILE_ATTRIBUTE_NOT_CONTENT_INDEXED |
		FILE_FLAG_RANDOM_ACCESS,
		NULL);

	if (hFileWrite == INVALID_HANDLE_VALUE) {
		p->m_State = FAIL;
		SendMessage(p->m_pWnd->m_hWnd, WM_USER_READFILE, reinterpret_cast<LPARAM>(p), FAIL_OPEN_WRITE);
	}

	p->m_State = COMPLETE;
	SendMessage(p->m_pWnd->m_hWnd, WM_USER_READFILE, NULL, reinterpret_cast<LPARAM>(p));
	return 0;
}
