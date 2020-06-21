
// MessageSpeedTestDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "MessageSpeedTest.h"
#include "MessageSpeedTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_USER_COMPLETE (WM_USER+1)

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


// CMessageSpeedTestDlg ダイアログ



CMessageSpeedTestDlg::CMessageSpeedTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MESSAGESPEEDTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMessageSpeedTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_COUNT, m_CtrlEditCount);
	DDX_Control(pDX, IDC_EDIT_TIME, m_CtrlEditTime);
	DDX_Control(pDX, IDC_EDIT_AVERAGE, m_CtrlEditAverage);
	DDX_Control(pDX, IDC_EDIT_TARGET, m_CtrlEditTarget);
}

BEGIN_MESSAGE_MAP(CMessageSpeedTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CMessageSpeedTestDlg::OnBnClickedButtonStart)
END_MESSAGE_MAP()


// CMessageSpeedTestDlg メッセージ ハンドラー

BOOL CMessageSpeedTestDlg::OnInitDialog()
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

void CMessageSpeedTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMessageSpeedTestDlg::OnPaint()
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
HCURSOR CMessageSpeedTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



UINT __cdecl TestThread(LPVOID pData)
{
	CMessageSpeedTestDlg* p = static_cast<CMessageSpeedTestDlg*>(pData);
	QueryPerformanceCounter(&p->m_Begin);
	p->SendMessage(WM_USER_COMPLETE);
	return 0;
}


void CMessageSpeedTestDlg::OnBnClickedButtonStart()
{
	QueryPerformanceFrequency(&m_Freq);
	m_Count = 0;
	m_Sum = 0;
	CString buffer;
	m_CtrlEditTarget.GetWindowText(buffer);
	m_Target = _ttoi(buffer);
	AfxBeginThread(TestThread, this);
}

LRESULT CMessageSpeedTestDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	float time;
	CString buff;
	switch (message) {
	case WM_USER_COMPLETE:
		QueryPerformanceCounter(&m_End);
		m_Sum += m_End.QuadPart - m_Begin.QuadPart;
		time = static_cast<float>(m_Sum) / m_Freq.QuadPart;

		if (m_Count < m_Target) {
			m_Count++;
			AfxBeginThread(TestThread, this);
		}
		else {
			buff.Format(_T("%d"), m_Count);
			m_CtrlEditCount.SetWindowText(buff);
			buff.Format(_T("%f"), time);
			m_CtrlEditTime.SetWindowText(buff);
			buff.Format(_T("%f"), time * 1000000 / m_Count);
			m_CtrlEditAverage.SetWindowText(buff);

		}
		break;
	default:
		break;
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}
