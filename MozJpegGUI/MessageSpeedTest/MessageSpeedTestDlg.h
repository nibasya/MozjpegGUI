
// MessageSpeedTestDlg.h : ヘッダー ファイル
//

#pragma once


// CMessageSpeedTestDlg ダイアログ
class CMessageSpeedTestDlg : public CDialogEx
{
// コンストラクション
public:
	CMessageSpeedTestDlg(CWnd* pParent = nullptr);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MESSAGESPEEDTEST_DIALOG };
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
	CEdit m_CtrlEditCount;
	CEdit m_CtrlEditTime;
	CEdit m_CtrlEditAverage;
	afx_msg void OnBnClickedButtonStart();
	LARGE_INTEGER m_Freq, m_Begin, m_End;
	LONGLONG m_Sum;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	int m_Count;
	int m_Target;
	CEdit m_CtrlEditTarget;
};
