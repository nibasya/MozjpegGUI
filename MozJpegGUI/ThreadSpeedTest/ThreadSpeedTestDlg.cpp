
// ThreadSpeedTestDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "ThreadSpeedTest.h"
#include "ThreadSpeedTestDlg.h"
#include "afxdialogex.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CThreadSpeedTestDlg ダイアログ



CThreadSpeedTestDlg::CThreadSpeedTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_THREADSPEEDTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CThreadSpeedTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NUMTHREADS, m_CtrlEditNum);
	DDX_Control(pDX, IDC_EDIT_TIME, m_CtrlEditTime);
	DDX_Control(pDX, IDC_EDIT_CREATED_THREADS, m_CtrlEditCreated);
	DDX_Control(pDX, IDC_EDIT_TIMEPERTHREAD, m_CtrlEditTimePerThread);
}

BEGIN_MESSAGE_MAP(CThreadSpeedTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CThreadSpeedTestDlg::OnBnClickedButtonStart)
END_MESSAGE_MAP()


// CThreadSpeedTestDlg メッセージ ハンドラー

BOOL CThreadSpeedTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CThreadSpeedTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CThreadSpeedTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CThreadSpeedTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

struct SData {
	volatile bool complete;
	LARGE_INTEGER time;
};

static UINT Thread(LPVOID pParam) {
	SData* pData = static_cast<SData*>(pParam);
	QueryPerformanceCounter(&pData->time);
	pData->complete = true;
	return 0;
}

void CThreadSpeedTestDlg::OnBnClickedButtonStart()
{
	int tgt;
	CString str;
	m_CtrlEditNum.GetWindowText(str);
	tgt = _ttoi(str);
	if (tgt < 0) {
		MessageBox(_T("Set positive number"));
		return;
	}

	LARGE_INTEGER freq, begin, start, end;

	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&begin);

	SData data;
	LONGLONG totalTime = 0;

	for (int i = 0; i < tgt; i++) {
		data.complete = false;
		QueryPerformanceCounter(&start);
		AfxBeginThread(Thread, &data);
//		Sleep(1);
		while (!data.complete);
		end = data.time;
		totalTime += (data.time.QuadPart - start.QuadPart);
		str.Format(_T("%d"), i);
		m_CtrlEditCreated.SetWindowText(str);
		str.Format(_T("%f"), ((float)(data.time.QuadPart - begin.QuadPart) / freq.QuadPart));
		m_CtrlEditTime.SetWindowText(str);
		str.Format(_T("%f"), ((float)totalTime *1000000/ freq.QuadPart / i));
		m_CtrlEditTimePerThread.SetWindowText(str);
	}
}
