#pragma once

#include <forward_list>
#include <vector>
#include "InfoProgressCtrl.h"
#include "CSemaphoreWithCounter.h"

class CMozJpegGUIDlg;

// CProgressDlg ダイアログ

class CProgressDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CProgressDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROGRESS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	// Original functions

	void Start();
	void Abort();
	void CloseDlg();
	// Completed thread
	bool ThreadEnd(CWinThread* pThread);
	bool AddThread();
	// Thread for waiting abort completion
	static UINT __cdecl AbortThread(LPVOID pData);
	// Thread to wait completion
	static UINT __cdecl CompleteThread(LPVOID pData);

	// CDialog functions

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();

	// GUI functions

	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CButton m_CtrlButtonPause;
	CButton m_CtrlButtonStop;
	CInfoProgressCtrl m_CtrlProgressProgress;
	CInfoProgressCtrl m_CtrlProgressReadBuffer;
	CInfoProgressCtrl m_CtrlProgressCPU;

	// Sync objects

	CSemaphoreWithCounter* m_pSyncHDD;
	CSemaphoreWithCounter* m_pSyncCPU;
	CSemaphoreWithCounter* m_pSyncReadBuff;
	CEvent m_SyncAbort;
	CCriticalSection m_CCSThreadList;

	// Public members

	// List of files to be saved
	std::vector<CString> m_FileList;
	// Output Folder. This is ignored if m_fSaveToOriginalDir is true
	CString m_OutputDir;
	// Saves the file to the same folder as original input if true
	bool m_fSaveToOriginalDir;
	bool m_fOverwrite;
	CString m_Options;
	unsigned int m_MaxCPU;
	unsigned int m_MaxHDD;
	unsigned int m_MaxReadBuff;
	bool m_Abort;
	bool m_Paused;
	CMozJpegGUIDlg* m_pParent;
	bool m_fCopyIfSmaller;

	// internal members
private:
	std::forward_list<CWinThread*> m_ThreadList;
	UINT m_StartCount;
	UINT m_CompletedCount;
	bool m_fComplete;
	UINT m_EndCount;
	LARGE_INTEGER m_StartTime;
	LARGE_INTEGER m_EndTime;
	HANDLE m_hCompleteThread;
};
