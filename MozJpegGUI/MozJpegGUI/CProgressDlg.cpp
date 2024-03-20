// CProgressDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "MozJpegGUI.h"
#include "MozJpegGUIDlg.h"
#include "CConvert.h"
#include "RPTT.h"
#include "afxdialogex.h"
#include "CProgressDlg.h"
#include <PathCch.h>
#include <gdiplus.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#define WM_USER_ABORT (WM_USER+101)
#define WM_USER_COMPLETE (WM_USER+102)

const UINT PROGRESS_UPDATE_TIMER = 10623;

// CProgressDlg ダイアログ

IMPLEMENT_DYNAMIC(CProgressDlg, CDialogEx)

CProgressDlg::CProgressDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROGRESS_DIALOG, pParent), m_pParent(NULL), m_Paused(false), m_fComplete(false), m_CompletedCount(0), m_StartCount(0), m_EndCount(0),
	m_SyncAbort(0, TRUE), m_pSyncHDD(NULL), m_pSyncReadBuff(NULL), m_pSyncCPU(NULL),m_MaxCPU(1), m_MaxHDD(1), m_MaxReadBuff(1),
	m_fOverwrite(false), m_fSaveToOriginalDir(false), m_Abort(false), m_hCompleteThread(NULL), m_fKeepMetadata(false), m_GdiPlusToken(NULL)
{
}

CProgressDlg::~CProgressDlg()
{
	if (m_GdiPlusToken != NULL) {
		Gdiplus::GdiplusShutdown(m_GdiPlusToken);
	}
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_PAUSE, m_CtrlButtonPause);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_CtrlButtonStop);
	DDX_Control(pDX, IDC_PROGRESS_PROGRESS, m_CtrlProgressProgress);
	DDX_Control(pDX, IDC_PROGRESS_READBUFFER, m_CtrlProgressReadBuffer);
	DDX_Control(pDX, IDC_PROGRESS_CPU, m_CtrlProgressCPU);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CProgressDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CProgressDlg::OnBnClickedButtonPause)
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CProgressDlg メッセージ ハンドラー


void CProgressDlg::Start()
{
	ASSERT(m_pSyncCPU == NULL);
	ASSERT(m_pSyncHDD == NULL);
	ASSERT(m_pSyncReadBuff == NULL);
	m_pSyncCPU = new CSemaphoreWithCounter(m_MaxCPU, m_MaxCPU);
	m_pSyncHDD = new CSemaphoreWithCounter(m_MaxHDD, m_MaxHDD);
	m_pSyncReadBuff = new CSemaphoreWithCounter(m_MaxReadBuff, m_MaxReadBuff);
	
	m_CtrlProgressCPU.SetSmoothMove(false);
	m_CtrlProgressReadBuffer.SetSmoothMove(false);
	m_CtrlProgressProgress.SetSmoothMove(false);

	m_EndCount = static_cast<UINT>(m_FileList.size());
	m_CtrlProgressCPU.SetRange(0, m_MaxCPU);
	m_CtrlProgressReadBuffer.SetRange(0, m_MaxReadBuff);
	m_CtrlProgressProgress.SetRange(0, m_EndCount);

	if (m_fKeepMetadata) {
		Gdiplus::GdiplusStartupInput gdiinput;
		Gdiplus::Status status;
		status = Gdiplus::GdiplusStartup(&m_GdiPlusToken, &gdiinput, NULL);
		if (status != Gdiplus::Status::Ok) {
			CString str;
			if (!str.LoadString(IDS_ERR_FAILED_TO_START_GDIPLUS)) {
				OutputDebugString(_T("Failed to load resource: IDS_ERR_FAILED_TO_START_GDIPLUS"));
			}
			MessageBox(str);
			CloseDlg();
			return;
		}
	}

	SetTimer(PROGRESS_UPDATE_TIMER, 100, NULL);
	QueryPerformanceCounter(&m_StartTime);

	AddThread();
}


bool CProgressDlg::AddThread()
{
	static CCriticalSection ccs;
	CSingleLock lock(&ccs, TRUE);

	if (m_StartCount >= m_FileList.size()) {
		if (!m_fComplete) {
			CWinThread* pThread = AfxBeginThread(CompleteThread, this);
			m_hCompleteThread = pThread->m_hThread;
			m_fComplete = true;
		}
		return true;
	}

	CString outputDir = m_OutputDir;
	if (m_fSaveToOriginalDir) {
		outputDir = m_FileList[m_StartCount];
		PathCchRemoveFileSpec(outputDir.GetBuffer(), outputDir.GetAllocLength());
		outputDir.ReleaseBuffer();
	}

	CConvert* pConv = new CConvert(this, m_FileList[m_StartCount], outputDir, m_Options);
	_RPTTN(_T("Starting %s\n"), m_FileList[m_StartCount]);
	CWinThread* pThread;
	pThread = AfxBeginThread(CConvert::MainThread, pConv);
	if (pThread == NULL) {
		delete pConv;
		CString str;
		if (!str.LoadString(IDS_ERR_FAILED_TO_CREATE_CONVERT_THREAD)) {
			OutputDebugString(_T("Failed to load resource: IDS_ERR_FAILED_TO_CREATE_CONVERT_THREAD"));
		}
		MessageBox(str);
		CloseDlg();
		return false;
	}

	CSingleLock lockThreadList(&m_CCSThreadList, TRUE);
	pConv->m_pThread = pThread;
	m_ThreadList.push_front(pThread);
	lockThreadList.Unlock();

	m_StartCount++;
	return true;
}

// Completed thread
bool CProgressDlg::ThreadEnd(CWinThread* pThread)
{
	CSingleLock lock(&m_CCSThreadList, TRUE);
	m_ThreadList.remove(pThread);
	m_CompletedCount++;
	return true;
}

void CProgressDlg::OnBnClickedButtonStop()
{
	OnClose();
}


void CProgressDlg::OnBnClickedButtonPause()
{
	CString str;
	if (!m_Paused) {
		if (!str.LoadString(IDS_BUTTON_RESUME)) {
			OutputDebugString(_T("Failed to load resource: IDS_BUTTON_RESUME\n"));
		}
		m_CtrlButtonPause.SetWindowText(str);
//		m_CtrlButtonStop.EnableWindow(false);
		m_Paused = true;
	}
	else {
		if (!str.LoadString(IDS_BUTTON_PAUSE)) {
			OutputDebugString(_T("Failed to load resource: IDS_BUTTON_PAUSE\n"));
		}
		m_CtrlButtonPause.SetWindowText(str);
//		m_CtrlButtonStop.EnableWindow(true);
		m_Paused = false;
	}
}


void CProgressDlg::OnClose()
{
	bool paused = m_Paused;
	if(!paused)
		OnBnClickedButtonPause();
	CString str;
	if (!str.LoadString(IDS_MSG_EXIT_COMPRESS)) {
		OutputDebugString(_T("Failed to load resource: IDS_MSG_EXIT_COMPRESS\n"));
	}
	if (MessageBox(str, 0, MB_YESNO) != IDYES) {
		if(!paused)
			OnBnClickedButtonPause();
		return;
	}
	Abort();
}


void CProgressDlg::Abort()
{
	m_Abort = true;
	m_SyncAbort.SetEvent();
	m_CtrlButtonPause.EnableWindow(FALSE);
	m_CtrlButtonStop.EnableWindow(FALSE);
	for each (auto pThread in m_ThreadList)
	{
		CancelSynchronousIo(pThread);
	}
	m_Paused = false;
	SetWindowText(_T("Aborting..."));
	if (m_hCompleteThread != NULL) {
		if (TerminateThread(m_hCompleteThread, 0) != 0) {
		}
		else {
			_RPTT0(_T("Failed to terminate complete_thread\n"));
		}
		m_hCompleteThread = NULL;
	}
	AfxBeginThread(AbortThread, this);
}


// Thread for waiting abort completion
UINT __cdecl CProgressDlg::AbortThread(LPVOID pData)
{
	CProgressDlg* pDlg = static_cast<CProgressDlg*>(pData);
	while (!pDlg->m_ThreadList.empty()) {
		Sleep(100);
	}
	pDlg->PostMessage(WM_USER_ABORT);
	return 0;
}


LRESULT CProgressDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	CString str;
	switch (message) {
	case WM_USER_ABORT:
		CloseDlg();
		return 0;
	case WM_USER_COMPLETE:
		if (!str.LoadString(IDS_MSG_COMPLETE)) {
			OutputDebugString(_T("Failed to load resource: IDS_MSG_COMPLETE"));
		}
		{
			CString buff;
			QueryPerformanceCounter(&m_EndTime);
			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			buff.Format(_T("%f sec."), ((float)(m_EndTime.QuadPart - m_StartTime.QuadPart)) / freq.QuadPart);
			MessageBox(str+buff);
			CloseDlg();
		}
		return 0;
	}
	return CDialogEx::WindowProc(message, wParam, lParam);
}


void CProgressDlg::OnCancel()
{
	OnClose();
}


void CProgressDlg::CloseDlg()
{
	CDialogEx::OnClose();
	KillTimer(PROGRESS_UPDATE_TIMER);
	DestroyWindow();
	m_pParent->m_CtrlButtonConvert.EnableWindow(TRUE);
	m_pParent->m_pProgressDlg = NULL;

	delete m_pSyncCPU;
	m_pSyncCPU = NULL;
	delete m_pSyncHDD;
	m_pSyncHDD = NULL;
	delete m_pSyncReadBuff;
	m_pSyncReadBuff = NULL;

	delete this;
}


// Thread to wait completion
UINT __cdecl CProgressDlg::CompleteThread(LPVOID pData)
{
	CProgressDlg* pDlg = static_cast<CProgressDlg*>(pData);
	while (!pDlg->m_ThreadList.empty()) {
		Sleep(100);
	}
	pDlg->PostMessage(WM_USER_COMPLETE);
	return 0;
}


void CProgressDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == PROGRESS_UPDATE_TIMER) {
		CString str;
		str.Format(_T("%d / %d"), m_pSyncCPU->GetCount(), m_MaxCPU);
		m_CtrlProgressCPU.SetPos(m_pSyncCPU->GetCount());
		m_CtrlProgressCPU.SetWindowText(str);
		str.Format(_T("%d / %d"), m_pSyncReadBuff->GetCount(), m_MaxReadBuff);
		m_CtrlProgressReadBuffer.SetPos(m_pSyncReadBuff->GetCount());
		m_CtrlProgressReadBuffer.SetWindowText(str);
		str.Format(_T("%d / %d"), m_CompletedCount, m_EndCount);
		m_CtrlProgressProgress.SetPos(m_CompletedCount);
		m_CtrlProgressProgress.SetWindowText(str);
	}

	CDialogEx::OnTimer(nIDEvent);
}
