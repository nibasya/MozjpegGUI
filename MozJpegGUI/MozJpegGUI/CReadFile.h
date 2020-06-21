#pragma once
class CReadFile
{
public:
	bool SetFilename(CString name);
private:
	// Filename to be read
	CString m_Filename;
	// Temporary file name
	CString m_TmpFilename;
	// pointer to an object which message will be sent
	CWnd* m_pWnd;
public:
	enum EState{INITIAL, RUNNING, COMPLETE, FAIL} m_State;
	enum EError{FAIL_OPEN_READ, FAIL_OPEN_WRITE};
public:
	CReadFile();
	// Start the thread. When process is completed, a message will be thrown to pWnd.
	void Start(CWnd* pWnd);
	static UINT __cdecl Thread(LPVOID pData);
};

