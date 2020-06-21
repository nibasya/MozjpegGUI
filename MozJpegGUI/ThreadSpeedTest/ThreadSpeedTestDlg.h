
// ThreadSpeedTestDlg.h : ヘッダー ファイル
//

#pragma once


// CThreadSpeedTestDlg ダイアログ
class CThreadSpeedTestDlg : public CDialogEx
{
// コンストラクション
public:
	CThreadSpeedTestDlg(CWnd* pParent = nullptr);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_THREADSPEEDTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_CtrlEditNum;
	CEdit m_CtrlEditTime;
	CEdit m_CtrlEditCreated;
	CEdit m_CtrlEditTimePerThread;
	afx_msg void OnBnClickedButtonStart();
};
