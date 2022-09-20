
// MozJpegGUIDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "MozJpegGUI.h"
#include "CProgressDlg.h"
#include "MozJpegGUIDlg.h"
#include "afxdialogex.h"
#include <PathCch.h>
#include <vector>
#include "CLibPNGVer.h"
#include "CAboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CMozJpegGUIDlg ダイアログ



CMozJpegGUIDlg::CMozJpegGUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MOZJPEGGUI_DIALOG, pParent), m_pProgressDlg(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	int dbg = _CrtSetDbgFlag(0);
//	_CrtSetDbgFlag(dbg | _CRTDBG_CHECK_ALWAYS_DF);
}

void CMozJpegGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILELIST, m_CtrlListFileList);
	DDX_Control(pDX, IDC_BUTTON_ADDFOLDER, m_CtrlButtonAddFolder);
	DDX_Control(pDX, IDC_BUTTON_CONVERT, m_CtrlButtonConvert);
	DDX_Control(pDX, IDC_BUTTON_CLEAR, m_CtrlButtonClear);
	DDX_Control(pDX, IDC_COMBO_SETTING, m_CtrlComboSetting);
	DDX_Control(pDX, IDC_EDIT_QUALITY, m_CtrlEditQuality);
	DDX_Control(pDX, IDC_COMBO_COLOR, m_CtrlComboColor);
	DDX_Control(pDX, IDC_CHECK_OPTIMIZE, m_CtrlCheckOptimize);
	DDX_Control(pDX, IDC_COMBO_CODING, m_CtrlComboCoding);
	DDX_Control(pDX, IDC_CHECK_OPTIMIZE_PROGRESSIVE_SCAN, m_CtrlCheckOptimizeProgressiveScan);
	DDX_Control(pDX, IDC_COMBO_DC_SCAN_OPT, m_CtrlComboDCScanOpt);
	DDX_Control(pDX, IDC_CHECK_TRELLIS_OPTIMIZATION, m_CtrlCheckTrellisOptimization);
	DDX_Control(pDX, IDC_CHECK_TRELLIS_OPTIMIZATION_OF_DC_COEFFICIENTS, m_CtrlCheckTrellisOptimizationOfDCCoefficients);
	DDX_Control(pDX, IDC_COMBO_TUNE_TRELLIS_OPTIMIZATION, m_CtrlComboTuneTrellisOptimization);
	DDX_Control(pDX, IDC_BUTTON_INITIALIZE, m_CtrlButtonInitialize);
	DDX_Control(pDX, IDC_EDIT_SAVETO, m_CtrlEditSaveTo);
	DDX_Control(pDX, IDC_CHECK_OVERWRITE, m_CtrlCheckOverwrite);
	DDX_Control(pDX, IDC_CHECK_COPY_IF_SMALLER, m_CtrlCheckCopyIfSmaller);
	DDX_Control(pDX, IDC_CHECK_SAVE_TO_ORIGINAL_FOLDER, m_CtrlCheckSaveToOriginalFolder);
	DDX_Control(pDX, IDC_BUTTON_SAVETO, m_CtrlButtonSaveTo);
}

BEGIN_MESSAGE_MAP(CMozJpegGUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CMozJpegGUIDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_ADDFOLDER, &CMozJpegGUIDlg::OnBnClickedButtonAddfolder)
	ON_BN_CLICKED(IDC_BUTTON_SAVETO, &CMozJpegGUIDlg::OnBnClickedButtonSaveto)
	ON_BN_CLICKED(IDC_BUTTON_CONVERT, &CMozJpegGUIDlg::OnBnClickedButtonConvert)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_INITIALIZE, &CMozJpegGUIDlg::OnBnClickedButtonInitialize)
	ON_WM_CLOSE()
	ON_CBN_CLOSEUP(IDC_COMBO_SETTING, &CMozJpegGUIDlg::OnCbnCloseupComboSetting)
	ON_CBN_DROPDOWN(IDC_COMBO_SETTING, &CMozJpegGUIDlg::OnCbnDropdownComboSetting)
	ON_BN_CLICKED(IDC_CHECK_SAVE_TO_ORIGINAL_FOLDER, &CMozJpegGUIDlg::OnBnClickedCheckSaveToOriginalFolder)
END_MESSAGE_MAP()


// CMozJpegGUIDlg メッセージ ハンドラー

BOOL CMozJpegGUIDlg::OnInitDialog()
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

	// 初期化をここに追加します。
	SetWindowText(CAboutDlg::GetAppVersion());
	InitGUI();
	InitSettingCombo();
	ReadSetting();
	OnBnClickedButtonSettingLoad();
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CMozJpegGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMozJpegGUIDlg::OnPaint()
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
HCURSOR CMozJpegGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CMozJpegGUIDlg::PreTranslateMessage(MSG* pMsg)
{
	m_CtrlToolTip.RelayEvent(pMsg);

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CMozJpegGUIDlg::OnDropFiles(HDROP hDropInfo)
{
	// ファイルリストに追加。拡張子のチェック等は実際に開くときに確認。
	UINT filenum = DragQueryFile(hDropInfo, -1, NULL, 0);	// ファイル数を取得
	UINT i;
	int pathlength = 0;
	int reqiredPathLength;
	TCHAR* pathbuff = new TCHAR[1];
	bool fSkip = false;

	for (i = 0; i < filenum; i++) {
		// 必要なメモリ容量確認
		reqiredPathLength = DragQueryFile(hDropInfo, i, NULL, 0);
		if (reqiredPathLength > pathlength) {
			pathlength = reqiredPathLength;
			delete[] pathbuff;
			pathbuff = new TCHAR[static_cast<size_t>(pathlength) + 1];
		}
		// ファイル名をパスごと取得
		DragQueryFile(hDropInfo, i, pathbuff, pathlength + 1);
		if (!CheckFileExtern(pathbuff)) {
			fSkip = true;
			continue;
		}
		// リストコントロールに追加。
		int index;
		index = m_CtrlListFileList.AddString(PathFindFileName(pathbuff));
		if (index == LB_ERRSPACE) {
			CString str;
			if (!str.LoadString(IDS_ERR_TOO_MANY_FILES)) {
				OutputDebugString(_T("Failed to load resource: IDS_ERR_TOO_MANY_FILES\n"));
			}
			MessageBox(str);
			break;
		}
		m_CtrlListFileList.SetItemDataPtr(index, new CString(pathbuff));
	}
	delete[] pathbuff;

	if (fSkip) {
		CString str;
		if (!str.LoadString(IDS_MSG_SKIPPED_SOME_NONE_IMAGE_FILES)) {
			OutputDebugString(_T("Failed to load resource: IDS_MSG_SKIPPED_SOME_NONE_IMAGE_FILES\n"));
		}
		MessageBox(str);
	}
	CDialogEx::OnDropFiles(hDropInfo);
}


// Returns true if the file can be handled by this software. Filename can include its path.
bool CMozJpegGUIDlg::CheckFileExtern(CString filename)
{
	CString ext = PathFindExtension(filename);
	ext = ext.MakeLower();
	if (ext == _T(".png")
		|| ext == _T(".jpg")
		|| ext == _T(".jpeg")
		|| ext == _T(".gif")
		|| ext == _T(".bmp")
		|| ext == _T(".ppm")
		|| ext == _T(".tga")
		//		|| ext == _T("")
		) return true;
	return false;
}


void CMozJpegGUIDlg::OnDestroy()
{
	// If processing image, ask if it is ok to quit
	if (m_pProgressDlg != NULL) {
		CString str;
		if (!str.LoadString(IDS_MSG_QUIT_ON_PROGRESS)) {
			OutputDebugString(_T("Failed to load resource: IDS_MSG_QUIT_ON_PROGRESS\n"));
		}
		if(MessageBox(str, 0, MB_YESNO) != IDYES)
			return;
		m_pProgressDlg->OnBnClickedButtonStop();
		return;
	}
	
	SaveSetting();

	while (m_CtrlListFileList.GetCount() > 0) {
		delete static_cast<CString*>(m_CtrlListFileList.GetItemDataPtr(0));
		m_CtrlListFileList.DeleteString(0);
	}

	CDialogEx::OnDestroy();
}


void CMozJpegGUIDlg::OnBnClickedButtonClear()
{
	while (m_CtrlListFileList.GetCount() > 0) {
		delete static_cast<CString*>(m_CtrlListFileList.GetItemDataPtr(0));
		m_CtrlListFileList.DeleteString(0);
	}
}


void CMozJpegGUIDlg::OnBnClickedButtonAddfolder()
{
	IFileDialog* pDialog = NULL;
	HRESULT hr;
	DWORD options;
	CString path;
	
	// インスタンス生成
	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDialog));
	if (FAILED(hr)) {
		CString str;
		if (!str.LoadString(IDS_ERR_FAILED_TO_CREATE_DIALOG)) {
			OutputDebugString(_T("Failed to load resource: IDS_ERR_FAILED_TO_CREATE_DIALOG\n"));
		}
		MessageBox(str);
		return;
	}

	// 設定の初期化
	pDialog->GetOptions(&options);
	pDialog->SetOptions(options | FOS_PICKFOLDERS);
	COMDLG_FILTERSPEC rgSpec[] =
	{
		{ _T("Images"), _T("*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.ppm;*.tga") },
		{ _T("BMP"), _T("*.bmp") },
		{ _T("GIF"), _T("*.gif") },
		{ _T("JPEG"), _T("*.jpg;*.jpeg") },
		{ _T("PNG"), _T("*.png") },
		{ _T("PPM"), _T("*.ppm") },
		{ _T("TARGA"), _T("*.tga") },
		{ _T("all"), _T("*.*") },
	};
	pDialog->SetFileTypes(sizeof(rgSpec) / sizeof(COMDLG_FILTERSPEC), rgSpec);

	// フォルダ選択ダイアログを表示
	hr = pDialog->Show(NULL);

	// 結果取得
	if (!SUCCEEDED(hr)) {
		pDialog->Release();
		return;
	}
	IShellItem* pItem = NULL;
	PWSTR pPath = NULL;
	try {
		hr = pDialog->GetResult(&pItem);
		if (!SUCCEEDED(hr))
			throw hr;
		hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pPath);
		if (!SUCCEEDED(hr))
			throw hr;
		path = pPath;
		CoTaskMemFree(pPath);
		pItem->Release();
	}
	catch (HRESULT e) {
		if (e == S_OK) {
			OutputDebugString(_T("exception thrown with S_OK\n"));
		}
		if(pItem != NULL)
			pItem->Release();
		pDialog->Release();
		return;
	}
	pDialog->Release();
	
	/*
	CString str;
	if (!str.LoadString(IDS_MSG_SELECT_FOLDER_TO_LOAD_IMAGE)) {
		OutputDebugString(_T("Failed to load resource: IDS_MSG_SELECT_FOLDER_TO_LOAD_IMAGE"));
	}

	BROWSEINFO bi = { m_hWnd, NULL, NULL, NULL, BIF_USENEWUI | BIF_VALIDATE, NULL, NULL, 0};
	bi.lpszTitle = str;
	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);
	if (pIDList == NULL) {
		return;
	}
	TCHAR pPath[MAX_PATH];
	if (SHGetPathFromIDList(pIDList, pPath) != TRUE) {
		if (!str.LoadString(IDS_ERR_NOT_VALID_FOLDER)) {
			OutputDebugString(_T("Failed to load resource: IDS_ERR_NOT_VALID_FOLDER"));
		}
		MessageBox(str);
		CoTaskMemFree(pIDList);
		pIDList = NULL;
		return;
	}
	CoTaskMemFree(pIDList);
	pIDList = NULL;
	path = pPath;
	*/

	// 検索文字列の生成
	if (path.Right(1) != _T("\\\n"))
		path += _T("\\");
	CString search = path + _T("*");

	// ファイル一覧の取得
	WIN32_FIND_DATA ffd;
	HANDLE hFind;
	bool fFound = false;

	hFind = FindFirstFile(search, &ffd);
	if (hFind == INVALID_HANDLE_VALUE) {
		CString str;
		if (!str.LoadString(IDS_ERR_FAILED_TO_GET_FILE_LIST)) {
			OutputDebugString(_T("Failed to load resource: IDS_ERR_FAILED_TO_GET_FILE_LIST\n"));
		}
		MessageBox(str);
	}
	do {
		if (!CheckFileExtern(ffd.cFileName)) {
			continue;
		}
		fFound = true;
		// リストコントロールに追加。
		int index;
		index = m_CtrlListFileList.AddString(PathFindFileName(ffd.cFileName));
		if (index == LB_ERRSPACE) {
			CString str;
			if (!str.LoadString(IDS_ERR_TOO_MANY_FILES)) {
				OutputDebugString(_T("Failed to load resource: IDS_ERR_TOO_MANY_FILES\n"));
			}
			MessageBox(str);
			break;
		}
		m_CtrlListFileList.SetItemDataPtr(index, new CString(path + ffd.cFileName));
	} while (FindNextFile(hFind, &ffd) !=0 );
	FindClose(hFind);

	if (!fFound) {
		CString str;
		if (!str.LoadString(IDS_MSG_NO_IMAGE_FILES)) {
			OutputDebugString(_T("Failed to load resource: IDS_MSG_NO_IMAGE_FILES\n"));
		}
		MessageBox(str);
	}
}


void CMozJpegGUIDlg::OnBnClickedButtonSaveto()
{
	IFileDialog* pDialog = NULL;
	HRESULT hr;
	DWORD options;
	CString path;

	// インスタンス生成
	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDialog));
	if (FAILED(hr)) {
		CString str;
		if (!str.LoadString(IDS_ERR_FAILED_TO_CREATE_DIALOG)) {
			OutputDebugString(_T("Failed to load resource: IDS_ERR_FAILED_TO_CREATE_DIALOG\n"));
		}
		MessageBox(str);
		return;
	}

	// 設定の初期化
	pDialog->GetOptions(&options);
	pDialog->SetOptions(options | FOS_PICKFOLDERS);

	// フォルダ選択ダイアログを表示
	hr = pDialog->Show(NULL);

	// 結果取得
	if (!SUCCEEDED(hr)) {
		pDialog->Release();
		return;
	}
	IShellItem* pItem = NULL;
	PWSTR pPath = NULL;
	hr = pDialog->GetResult(&pItem);
	if (SUCCEEDED(hr)) {
		hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pPath);
		if (SUCCEEDED(hr)) {
			path = pPath;
			CoTaskMemFree(pPath);
		}
		pItem->Release();
	}
	pDialog->Release();

	m_CtrlEditSaveTo.SetWindowText(path);
	m_CtrlEditSaveTo.SetSel(0, -1, FALSE);
}


void CMozJpegGUIDlg::OnBnClickedButtonConvert()
{
	if (m_CtrlListFileList.GetCount() == 0) {
		CString str;
		if (!str.LoadString(IDS_ERR_FILE_NOT_SELECTED)) {
			OutputDebugString(_T("Failed to load resource: IDS_ERR_FILE_NOT_SELECTED"));
		}
		MessageBox(str);
		return;
	}

	bool saveToOriginalFolder = m_CtrlCheckSaveToOriginalFolder.GetCheck() == BST_CHECKED;

	CString outputDir;
	m_CtrlEditSaveTo.GetWindowText(outputDir);
	if (!saveToOriginalFolder) {
		if (outputDir.GetLength() == 0) {
			CString str;
			if (!str.LoadString(IDS_ERR_SET_SAVE_DIR)) {
				OutputDebugString(_T("Failed to load resource: IDS_ERR_SET_SAVE_DIR"));
			}
			MessageBox(str);
			return;
		}
		if (PathIsDirectory(outputDir) == FALSE) {
			CString str;
			if (!str.LoadString(IDS_ERR_OUTPUT_DIR_NOT_EXIST)) {
				OutputDebugString(_T("Failed to load resource: IDS_ERR_OUTPUT_DIR_NOT_EXIST"));
			}
			MessageBox(str);
			return;
		}
		size_t outLen = static_cast<size_t>(outputDir.GetLength()) + 2;
		TCHAR* outBuff = new TCHAR[outLen];
		_tcscpy_s(outBuff, outLen, outputDir);
		PathCchAddBackslash(outBuff, outLen);
		outputDir = outBuff;
		delete[] outBuff;
	}

	m_CtrlButtonConvert.EnableWindow(FALSE);


	ASSERT(m_pProgressDlg == NULL);
	m_pProgressDlg = new CProgressDlg();
	m_pProgressDlg->m_pParent = this;
	m_pProgressDlg->m_OutputDir = outputDir;
	m_pProgressDlg->m_fOverwrite = m_CtrlCheckOverwrite.GetCheck() == BST_CHECKED;
	m_pProgressDlg->m_Options = CreateOptions();
	m_pProgressDlg->m_fCopyIfSmaller = m_CtrlCheckCopyIfSmaller.GetCheck() == BST_CHECKED;
	m_pProgressDlg->m_fSaveToOriginalDir = saveToOriginalFolder;

	SYSTEM_INFO sysInfo;
	GetNativeSystemInfo(&sysInfo);
	m_pProgressDlg->m_MaxCPU = sysInfo.dwNumberOfProcessors;
	m_pProgressDlg->m_MaxReadBuff = sysInfo.dwNumberOfProcessors * 2;
	m_pProgressDlg->m_MaxHDD = 1;

	int i;
	for (i = 0; i < m_CtrlListFileList.GetCount(); i++) {
		m_pProgressDlg->m_FileList.push_back(*static_cast<CString*>(m_CtrlListFileList.GetItemDataPtr(i)));
	}

	m_pProgressDlg->Create(IDD_PROGRESS_DIALOG);
	m_pProgressDlg->ShowWindow(SW_SHOW);
	m_pProgressDlg->Start();
}


void CMozJpegGUIDlg::ReadSetting()
{
	CWinApp* p = AfxGetApp();
	int defNum;
	defNum = p->GetProfileInt(_T("Common"), _T("Default setting"), 0);
	m_CtrlComboSetting.SetCurSel(defNum);
	m_CurrentSetting = defNum;
	m_SelectedSetting = defNum;
	OnBnClickedButtonSettingLoad();
}


void CMozJpegGUIDlg::SaveSetting()
{
	CWinApp* p = AfxGetApp();
	p->WriteProfileInt(_T("Common"), _T("Default setting"), m_CurrentSetting);
	OnBnClickedButtonSettingSave();
}


void CMozJpegGUIDlg::CreateInitialSetting()
{
	CString sec;
	for (int i = 0; i < 3; i++) {
		sec.Format(_T("%d"), i);
		AfxGetApp()->WriteProfileString(sec, _T("Profile name"), CString(_T("Setting ")) + sec);
		CreateInitialSettingSub(i);
	}
}


void CMozJpegGUIDlg::CreateInitialSettingSub(int num)
{
	CWinApp* p = AfxGetApp();
	CString sec;
	sec.Format(_T("%d"), num);

	// 	p->WriteProfileInt(sec, _T(""), );
	p->WriteProfileInt(sec, _T("Quality"), 75);
	p->WriteProfileInt(sec, _T("Color"), 2);
	p->WriteProfileInt(sec, _T("Coding"), 0);
	p->WriteProfileInt(sec, _T("Optimize"), 1);
	p->WriteProfileInt(sec, _T("Optimize progressive scan"), 1);
	p->WriteProfileInt(sec, _T("DC scan optimize"), 1);
	p->WriteProfileInt(sec, _T("Trellis optimization"), 1);
	p->WriteProfileInt(sec, _T("Trellis optimization of DC coefficients"), 1);
	p->WriteProfileInt(sec, _T("Tune trellis optimization"), 1);
	p->WriteProfileString(sec, _T("Output Directory"), _T(""));
	p->WriteProfileInt(sec, _T("Overwrite"), 0);
	p->WriteProfileInt(sec, _T("Copy file if original is smaller"), 0);
}


void CMozJpegGUIDlg::OnBnClickedButtonSettingLoad()
{
	int num = m_SelectedSetting;
	CWinApp* p = AfxGetApp();
	CString sec;
	sec.Format(_T("%d"), num);
	CString buff;
	buff.Format(_T("%d"), p->GetProfileInt(sec, _T("Quality"), 75));
	m_CtrlEditQuality.SetWindowText(buff);
	m_CtrlComboColor.SetCurSel(p->GetProfileInt(sec, _T("Color"), 2));
	m_CtrlComboCoding.SetCurSel(p->GetProfileInt(sec, _T("Coding"), 0));
	m_CtrlCheckOptimize.SetCheck(p->GetProfileInt(sec, _T("Optimize"), 1) == 0 ? BST_UNCHECKED : BST_CHECKED);
	m_CtrlCheckOptimizeProgressiveScan.SetCheck(p->GetProfileInt(sec, _T("Optimize progressive scan"), 1) == 0 ? BST_UNCHECKED : BST_CHECKED);
	m_CtrlComboDCScanOpt.SetCurSel(p->GetProfileInt(sec, _T("DC scan optimize"), 1));
	m_CtrlCheckTrellisOptimization.SetCheck(p->GetProfileInt(sec, _T("Trellis optimization"), 1) == 0 ? BST_UNCHECKED : BST_CHECKED);
	m_CtrlCheckTrellisOptimizationOfDCCoefficients.SetCheck(p->GetProfileInt(sec, _T("Trellis optimization of DC coefficients"), 1) == 0 ? BST_UNCHECKED : BST_CHECKED);
	m_CtrlComboTuneTrellisOptimization.SetCurSel(p->GetProfileInt(sec, _T("Tune trellis optimization"), 1));
	m_CtrlEditSaveTo.SetWindowText(p->GetProfileString(sec, _T("Output directory"), _T("")));
	m_CtrlCheckOverwrite.SetCheck(p->GetProfileInt(sec, _T("Overwrite"), 0) == 0 ? BST_UNCHECKED : BST_CHECKED);
	m_CtrlCheckCopyIfSmaller.SetCheck(p->GetProfileInt(sec, _T("Copy file if original is smaller"), 0) == 0 ? BST_UNCHECKED : BST_CHECKED);
	m_CtrlCheckSaveToOriginalFolder.SetCheck(p->GetProfileInt(sec, _T("Save to original folder"), 0) == 0 ? BST_UNCHECKED : BST_CHECKED);
	OnBnClickedCheckSaveToOriginalFolder();
	m_CurrentSetting = m_SelectedSetting;
}


void CMozJpegGUIDlg::OnBnClickedButtonSettingSave()
{
	int num = m_SelectedSetting;
	CWinApp* p = AfxGetApp();
	CString sec;
	sec.Format(_T("%d"), num);
	CString profName;
	m_CtrlComboSetting.GetLBText(num, profName);
	CString buff;

	// 	p->WriteProfileInt(sec, _T(""), );
	p->WriteProfileString(sec, _T("Profile name"), profName);
	m_CtrlEditQuality.GetWindowText(buff);
	p->WriteProfileString(sec, _T("Quality"), buff);
	p->WriteProfileInt(sec, _T("Color"), m_CtrlComboColor.GetCurSel());
	p->WriteProfileInt(sec, _T("Coding"), m_CtrlComboCoding.GetCurSel());
	p->WriteProfileInt(sec, _T("Optimize"), m_CtrlCheckOptimize.GetCheck()!=BST_UNCHECKED ? 1 : 0);
	p->WriteProfileInt(sec, _T("Optimize progressive scan"), m_CtrlCheckOptimizeProgressiveScan.GetCheck()!=BST_UNCHECKED ? 1 : 0);
	p->WriteProfileInt(sec, _T("DC scan optimize"), m_CtrlComboDCScanOpt.GetCurSel());
	p->WriteProfileInt(sec, _T("Trellis optimization"), m_CtrlCheckTrellisOptimization.GetCheck()!=BST_UNCHECKED ? 1 : 0);
	p->WriteProfileInt(sec, _T("Trellis optimization of DC coefficients"), m_CtrlCheckTrellisOptimizationOfDCCoefficients.GetCheck()!=BST_UNCHECKED ? 1 : 0);
	p->WriteProfileInt(sec, _T("Tune trellis optimization"), m_CtrlComboTuneTrellisOptimization.GetCurSel());
	m_CtrlEditSaveTo.GetWindowText(buff);
	p->WriteProfileString(sec, _T("Output directory"), buff);
	p->WriteProfileInt(sec, _T("Overwrite"), m_CtrlCheckOverwrite.GetCheck() != BST_UNCHECKED ? 1 : 0);
	p->WriteProfileInt(sec, _T("Copy file if original is smaller"), m_CtrlCheckCopyIfSmaller.GetCheck() != BST_UNCHECKED ? 1 : 0);
	p->WriteProfileInt(sec, _T("Save to original folder"), m_CtrlCheckSaveToOriginalFolder.GetCheck() != BST_UNCHECKED ? 1 : 0);
}


void CMozJpegGUIDlg::OnBnClickedButtonInitialize()
{
	m_CtrlEditQuality.SetWindowText(_T("75"));
	m_CtrlComboColor.SetCurSel(2);
	m_CtrlComboCoding.SetCurSel(0);
	m_CtrlCheckOptimize.SetCheck(BST_CHECKED);
	m_CtrlCheckOptimizeProgressiveScan.SetCheck(BST_CHECKED);
	m_CtrlComboDCScanOpt.SetCurSel(1);
	m_CtrlCheckTrellisOptimization.SetCheck(BST_CHECKED);
	m_CtrlCheckTrellisOptimizationOfDCCoefficients.SetCheck(BST_CHECKED);
	m_CtrlComboTuneTrellisOptimization.SetCurSel(1);
	m_CtrlEditSaveTo.SetWindowText(_T(""));
	m_CtrlCheckOverwrite.SetCheck(BST_UNCHECKED);
	m_CtrlCheckCopyIfSmaller.SetCheck(BST_UNCHECKED);
}


int CMozJpegGUIDlg::InitSettingCombo()
{
	const int MAXBUFF = 1024;
	TCHAR buff[MAXBUFF];
	TCHAR name[MAXBUFF];
	int ret;
	int count = 0;
	CString str;

	ret = GetPrivateProfileSectionNames(buff, MAXBUFF, AfxGetApp()->m_pszProfileName);
	if (ret == MAXBUFF - 2) {
		if (!str.LoadString(IDS_ERR_TOO_MANY_SETTINGS)) {
			OutputDebugString(_T("Failed to load resource: IDS_ERR_TOO_MANY_SETTINGS"));
		}
		MessageBox(str);
		exit(-1);
	}

	if (ret == 0) {
		WritePrivateProfileString(_T("Common"), _T("Default setting"), _T("0"), AfxGetApp()->m_pszProfileName);
		CreateInitialSetting();
		ret = GetPrivateProfileSectionNames(buff, MAXBUFF, AfxGetApp()->m_pszProfileName);
		if (ret == MAXBUFF - 2) {
			if (!str.LoadString(IDS_ERR_TOO_MANY_SETTINGS)) {
				OutputDebugString(_T("Failed to load resource: IDS_ERR_TOO_MANY_SETTINGS"));
			}
			MessageBox(str);
			exit(-1);
		}
	}

	TCHAR* p = buff;
	TCHAR* pChk;
	bool flag;
	while (*p != _T('\0')) {
		pChk = p;
		flag = true;
		while (*pChk != _T('\0')) {
			if (flag && !isdigit(static_cast<int>(*pChk))) {
				flag = false;
			}
			pChk++;
		}
		if (flag) {
			ret = GetPrivateProfileString(p, _T("Profile name"), _T("Empty"), name, MAXBUFF, AfxGetApp()->m_pszProfileName);
			if (ret >= MAXBUFF - 1) {
				if (!str.LoadString(IDS_ERR_SETTING_NAME_TOO_LONG)) {
					OutputDebugString(_T("Failed to load resource: IDS_ERR_SETTING_NAME_TOO_LONG"));
				}
				MessageBox(str);
				return 0;
			}
			m_CtrlComboSetting.AddString(name);
			count++;
		}
		p = pChk + 1;
	}

	return count;
}


void CMozJpegGUIDlg::InitGUI()
{
	CString str;
	VERIFY(m_CtrlToolTip.Create(this));
	m_CtrlToolTip.Activate(TRUE);
	m_CtrlToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, 0x7fffffff);

	//////////////////////////////////////////////////////////////
	// General GUIs
	if (!str.LoadString(IDS_COPY_IF_SMALLER)) {
		OutputDebugString(_T("Failed to load resource: IDS_COPY_IF_SMALLER"));
	}
	VERIFY(m_CtrlToolTip.AddTool(GetDlgItem(IDC_CHECK_COPY_IF_SMALLER), str));

	//////////////////////////////////////////////////////////////
	// mozjpeg GUIs
	// -quality
	if (!str.LoadString(IDS_QUALITY_TIP)) {
		OutputDebugString(_T("Failed to load resource: IDS_QUALITY_TIP\n"));
	}
	VERIFY(m_CtrlToolTip.AddTool(GetDlgItem(IDC_EDIT_QUALITY), str));

	// -grayscale / -rgb
	if (str.LoadString(IDS_GRAYSCALE) != 0) {
		m_CtrlComboColor.AddString(str);
	}
	else {
		OutputDebugString(_T("Failed to load resource: IDS_GRAYSCALE\n"));
	}
	if (str.LoadString(IDS_RGB) != 0) {
		m_CtrlComboColor.AddString(str);
	}
	else {
		OutputDebugString(_T("Failed to load resource: IDS_RGB\n"));
	}
	if (str.LoadString(IDS_COLOR_AUTO) != 0) {
		m_CtrlComboColor.AddString(str);
	}
	else {
		OutputDebugString(_T("Failed to load resource: IDS_COLOR_AUTO\n"));
	}
	if (!str.LoadString(IDS_COLOR_TIP)) {
		OutputDebugString(_T("Failed to load resource: IDS_COLOR_TIP\n"));
	}
	VERIFY(m_CtrlToolTip.AddTool(GetDlgItem(IDC_COMBO_COLOR), str));

	// -progressive / -baseline
	if (str.LoadString(IDS_PROGRESSIVE) != 0) {
		m_CtrlComboCoding.AddString(str);
	}
	else {
		OutputDebugString(_T("Failed to load resource: IDS_PROGRESSIVE\n"));
	}
	if (str.LoadString(IDS_BASELINE) != 0) {
		m_CtrlComboCoding.AddString(str);
	}
	else {
		OutputDebugString(_T("Failed to load resource: IDS_BASELINE\n"));
	}
	if (!str.LoadString(IDS_CODING_TIP)) {
		OutputDebugString(_T("Failed to load resource: IDS_CODING_TIP\n"));
	}
	VERIFY(m_CtrlToolTip.AddTool(GetDlgItem(IDC_COMBO_CODING), str));

	// -optimize
	if (!str.LoadString(IDS_OPTIMIZE_TIP)) {
		OutputDebugString(_T("Failed to load resource: IDS_OPTIMIZE_TIP\n"));
	}
	VERIFY(m_CtrlToolTip.AddTool(GetDlgItem(IDC_CHECK_OPTIMIZE), str));

	// -fastcrush
	if (!str.LoadString(IDS_FASTCRASH_TIP)) {
		OutputDebugString(_T("Failed to load resource: IDS_FASTCRASH_TIP\n"));
	}
	VERIFY(m_CtrlToolTip.AddTool(GetDlgItem(IDC_CHECK_OPTIMIZE_PROGRESSIVE_SCAN), str));

	// -dc-scan-opt
	if (str.LoadString(IDS_DC_SCAN_OPTIMIZATION_0) != 0) {
		m_CtrlComboDCScanOpt.AddString(str);
	}
	else {
		OutputDebugString(_T("Failed to load resource: IDS_DC_SCAN_OPTIMIZATION_0\n"));
	}
	if (str.LoadString(IDS_DC_SCAN_OPTIMIZATION_1) != 0) {
		m_CtrlComboDCScanOpt.AddString(str);
	}
	else {
		OutputDebugString(_T("Failed to load resource: IDS_DC_SCAN_OPTIMIZATION_1\n"));
	}
	if (str.LoadString(IDS_DC_SCAN_OPTIMIZATION_2) != 0) {
		m_CtrlComboDCScanOpt.AddString(str);
	}
	else {
		OutputDebugString(_T("Failed to load resource: IDS_DC_SCAN_OPTIMIZATION_2\n"));
	}
	if (!str.LoadString(IDS_DC_SCAN_OPTIMIZATION_TIP)) {
		OutputDebugString(_T("Failed to load resource: IDS_DC_SCAN_OPTIMITZATION_TIP\n"));
	}
	VERIFY(m_CtrlToolTip.AddTool(GetDlgItem(IDC_COMBO_DC_SCAN_OPT), str));

	// -notrellis
	if (!str.LoadString(IDS_TRELLIS_DC_TIP)) {
		OutputDebugString(_T("Failed to load resource: IDS_TRELLIS_DC_TIP\n"));
	}
	VERIFY(m_CtrlToolTip.AddTool(GetDlgItem(IDC_CHECK_TRELLIS_OPTIMIZATION), str));

	// -tune-psnr / -tune-hvs-psnr / -tune-ssim / -tune-ms-ssim
	if (str.LoadString(IDS_TUNE_TRELLIS_PSNR) != 0) {
		m_CtrlComboTuneTrellisOptimization.AddString(str);
	}
	else {
		OutputDebugString(_T("Failed to load resource: IDS_TUNE_TRELLIS_PSNR\n"));
	}
	if (str.LoadString(IDS_TUNE_TRELLIS_HVS_PSNR) != 0) {
		m_CtrlComboTuneTrellisOptimization.AddString(str);
	}
	else {
		OutputDebugString(_T("Failed to load resource: IDS_TUNE_TRELLIS_HVS_PSNR\n"));
	}
	if (str.LoadString(IDS_TUNE_TRELLIS_SSIM) != 0) {
		m_CtrlComboTuneTrellisOptimization.AddString(str);
	}
	else {
		OutputDebugString(_T("Failed to load resource: IDS_TUNE_TRELLIS_SSIM\n"));
	}
	if (str.LoadString(IDS_TUNE_TRELLIS_MS_SSIM) != 0) {
		m_CtrlComboTuneTrellisOptimization.AddString(str);
	}
	else {
		OutputDebugString(_T("Failed to load resource: IDS_TUNE_TRELLIS_MS_SSIM\n"));
	}
	
	if (!str.LoadString(IDS_TUNE_TRELLIS_TIP)) {
		OutputDebugString(_T("Failed to load resource: IDS_TUNE_TRELLIS_TIP\n"));
	}
	VERIFY(m_CtrlToolTip.AddTool(GetDlgItem(IDC_COMBO_TUNE_TRELLIS_OPTIMIZATION), str));

	if (!str.LoadString(IDS_SAVE_TO_ORIGINAL_FOLDER)) {
		OutputDebugString(_T("Failed to load resource: IDS_SAVE_TO_ORIGINAL_FOLDER\n"));
	}
	VERIFY(m_CtrlToolTip.AddTool(GetDlgItem(IDC_CHECK_SAVE_TO_ORIGINAL_FOLDER), str));
}




CString CMozJpegGUIDlg::CreateOptions()
{
	CString work, buff, ret;

	m_CtrlEditQuality.GetWindowText(buff);
	work=CString(_T("-quality ")) + buff;
	ret += work + CString(_T(" "));

	switch (m_CtrlComboColor.GetCurSel()) {
	case 0:	// grayscale
		work = CString(_T("-grayscale"));
		break;
	case 1:	// rgb
		work = CString(_T("-rgb"));
		break;
	case 2:	// auto
		// if option is not specified, the software will determine the color mode automatically.
		break;
	default:
		MessageBox(_T("Unknown color type is set. exit."));
		exit(-1);
	}
	ret += work + CString(_T(" "));

	work = (m_CtrlCheckOptimize.GetCheck() == BST_CHECKED) ? CString(_T("-optimize")) : CString(_T(""));
	ret += work + CString(_T(" "));

	switch (m_CtrlComboCoding.GetCurSel()) {
	case 0: // progressive
		work = CString(_T("-progressive"));
		break;
	case 1: // baseline
		work = CString(_T("-baseline"));
		break;
	default:
		MessageBox(_T("Unknown coding type is set. exit."));
		exit(-1);
	}
	ret += work + CString(_T(" "));

	work = (m_CtrlCheckOptimizeProgressiveScan.GetCheck() == BST_CHECKED) ? CString(_T("")) : CString(_T("-fastcrush"));
	ret += work + CString(_T(" "));

	work = CString(_T("-dc-scan-opt "));
	switch (m_CtrlComboDCScanOpt.GetCurSel()) {
	case 0:	// One scan for all components
		work += CString(_T("0"));
		break;
	case 1: // One scan per component (default)
		work += CString(_T("1"));
		break;
	case 2:	// Optimize between one scan for all components and one scan for 1st component plus one scan for remaining components
		work += CString(_T("2"));
		break;
	default:
		MessageBox(_T("Unknown DC scan opt. is set. exit."));
		exit(-1);
	}
	ret += work + CString(_T(" "));

	work = (m_CtrlCheckTrellisOptimization.GetCheck() == BST_CHECKED) ? CString(_T("")) : CString(_T("-notrellis"));
	ret += work + CString(_T(" "));

	work = (m_CtrlCheckTrellisOptimizationOfDCCoefficients.GetCheck() == BST_CHECKED) ? CString(_T("-trellis-dc")) : CString(_T("-notrellis-dc"));
	ret += work + CString(_T(" "));

	return ret;
}


void CMozJpegGUIDlg::OnClose()
{
	if (m_pProgressDlg != NULL) {
		m_pProgressDlg->OnClose();
		if (!m_pProgressDlg->m_Abort) {
			return;
		}
		AfxBeginThread(WaitForAbort, this);
		return;
	}
	CDialogEx::OnClose();
}


// Wait for prgress control to complete abort
UINT __cdecl CMozJpegGUIDlg::WaitForAbort(LPVOID pData)
{
	CMozJpegGUIDlg* p = static_cast<CMozJpegGUIDlg*>(pData);

	while (p->m_pProgressDlg != NULL) {
		Sleep(100);
	}

	p->PostMessage(WM_CLOSE);
	return 0;
}


void CMozJpegGUIDlg::OnCbnCloseupComboSetting()
{
	OnBnClickedButtonSettingSave();
	m_SelectedSetting = m_CtrlComboSetting.GetCurSel();
	OnBnClickedButtonSettingLoad();
}


void CMozJpegGUIDlg::OnCbnDropdownComboSetting()
{
	CString edit, buff;
	m_CtrlComboSetting.GetWindowText(edit);
	for (int i = 0; i < m_CtrlComboSetting.GetCount(); i++) {
		m_CtrlComboSetting.GetLBText(i, buff);
		if (buff == edit) {
			return;
		}
	}
	if (edit.GetLength() == 0) {
		return;
	}

	m_CtrlComboSetting.DeleteString(m_SelectedSetting);
	m_CtrlComboSetting.InsertString(m_SelectedSetting, edit);
}


void CMozJpegGUIDlg::OnBnClickedCheckSaveToOriginalFolder()
{
	if (m_CtrlCheckSaveToOriginalFolder.GetCheck() == BST_CHECKED) {
		m_CtrlEditSaveTo.EnableWindow(FALSE);
		m_CtrlButtonSaveTo.EnableWindow(FALSE);
	}
	else {
		m_CtrlEditSaveTo.EnableWindow(TRUE);
		m_CtrlButtonSaveTo.EnableWindow(TRUE);
	}
}
