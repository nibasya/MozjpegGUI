
// MozJpegGUIDlg.h : ヘッダー ファイル
//

#pragma once
#include <vector>

class CProgressDlg;

// CMozJpegGUIDlg ダイアログ
class CMozJpegGUIDlg : public CDialogEx
{
// コンストラクション
public:
	CMozJpegGUIDlg(CWnd* pParent = nullptr);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOZJPEGGUI_DIALOG };
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
	CListBox m_CtrlListFileList;
	CButton m_CtrlButtonAddFolder;
	CButton m_CtrlButtonConvert;
	CButton m_CtrlButtonClear;
	CComboBox m_CtrlComboSetting;
	CEdit m_CtrlEditQuality;
	CComboBox m_CtrlComboColor;
	CButton m_CtrlCheckOptimize;
	CComboBox m_CtrlComboCoding;
	CButton m_CtrlCheckOptimizeProgressiveScan;
	CComboBox m_CtrlComboDCScanOpt;
	CButton m_CtrlCheckTrellisOptimization;
	CButton m_CtrlCheckTrellisOptimizationOfDCCoefficients;
	CComboBox m_CtrlComboTuneTrellisOptimization;
	// Initialize convertion setting
	CButton m_CtrlButtonInitialize;
	CToolTipCtrl m_CtrlToolTip;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	// Returns true if the file can be handled by this software. filename can include its path.
	bool CheckFileExtern(CString filename);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonAddfolder();
	CEdit m_CtrlEditSaveTo;
	afx_msg void OnBnClickedButtonSaveto();
	afx_msg void OnBnClickedButtonConvert();
	CProgressDlg* m_pProgressDlg;
	CButton m_CtrlCheckOverwrite;
	void ReadSetting();
	void SaveSetting();
	void ReadConvertSetting(int num);
	void SaveConvertSetting(int num);
	int InitSettingCombo();
	void InitGUI();
	void CreateInitialSetting();
	afx_msg void OnBnClickedButtonSettingLoad();
	afx_msg void OnBnClickedButtonSettingSave();
	afx_msg void OnBnClickedButtonInitialize();
	void CreateInitialSettingSub(int num);
	CString CreateOptions();
	afx_msg void OnClose();
	// Wait for prgress control to complete abort
	static UINT WaitForAbort(LPVOID pData);
	CButton m_CtrlCheckCopyIfSmaller;
	afx_msg void OnCbnCloseupComboSetting();
	// Current setting number used for mozJpegGUI setting
	int m_CurrentSetting;
	// Selected setting number in m_CtrlComboSetting
	int m_SelectedSetting;
	afx_msg void OnCbnDropdownComboSetting();
};
