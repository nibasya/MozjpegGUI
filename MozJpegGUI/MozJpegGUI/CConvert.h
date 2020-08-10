#pragma once

class CSemaphoreWithCounter;
class CProgressDlg;

class CConvert
{
public:
	CConvert(CProgressDlg* pParent, CString &fileName, CString &outDir, CString &options) ;
	enum class EConvertError {EAbort, EIgnore} ;
	static UINT __cdecl MainThread(LPVOID pData);
	CProgressDlg* m_pParent;
	CString m_Filename;
	CString m_CommandOptions;
	CString m_Outputname;
	CString m_OutDir;
	CWinThread* m_pThread;
private:
	BYTE* m_InData;
	LONGLONG m_InSize;
	BYTE* m_OutData;
	LONGLONG m_OutSize;
	UINT Main();
	void Pause();
public:
	bool ReadFile();
	bool Convert();
	bool WriteFile();
	void CreateArgs(int* argc, void*** argv);
};

class CConvertLock
{
public:
	CConvertLock(CSyncObject& syncAbort, CSemaphoreWithCounter* pSyncObj);
	~CConvertLock();
	DWORD Lock();
	DWORD Unlock();
private:
	CSyncObject* m_SyncObj[2];
	CMultiLock* m_pLock;
	bool m_Locked;
};