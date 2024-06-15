
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
	// File controls
	CListBox m_CtrlListFileList;
	CButton m_CtrlButtonAddFolder;
	CButton m_CtrlButtonConvert;
	CButton m_CtrlButtonClear;
	CButton m_CtrlCheckOverwrite;
	CButton m_CtrlCheckCopyIfSmaller;
	CButton m_CtrlCheckSaveToOriginalFolder;
	CButton m_CtrlButtonSaveTo;
	CEdit m_CtrlEditSaveTo;

	// Convert options
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

	// misc. controls
	CButton m_CtrlButtonInitialize;	// Initialize convertion setting
	CToolTipCtrl m_CtrlToolTip;

	// other variables
	CProgressDlg* m_pProgressDlg;
	int m_CurrentSetting;	// Current setting number used for mozJpegGUI setting
	int m_SelectedSetting;	// Selected setting number in m_CtrlComboSetting

	// MFC functions
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	bool CheckFileExtern(CString filename);	// Returns true if the file can be handled by this software. The filename can include its path.
	afx_msg void OnDestroy();

	// support functions
	void ReadSetting();
	void SaveSetting();
	void ReadConvertSetting(int num);
	void SaveConvertSetting(int num);
	int InitSettingCombo();
	void InitGUI();
	void CreateInitialSetting();
	void CreateInitialSettingSub(int num);
	CString CreateOptions();
	static UINT WaitForAbort(LPVOID pData);	// Wait for prgress control to complete abort


	// GUI functions
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonAddfolder();
	afx_msg void OnBnClickedButtonSaveto();
	afx_msg void OnBnClickedButtonConvert();
	afx_msg void OnBnClickedButtonSettingLoad();
	afx_msg void OnBnClickedButtonSettingSave();
	afx_msg void OnBnClickedButtonInitialize();
	afx_msg void OnClose();
	afx_msg void OnCbnCloseupComboSetting();
	afx_msg void OnCbnDropdownComboSetting();
	afx_msg void OnBnClickedCheckSaveToOriginalFolder();
};
