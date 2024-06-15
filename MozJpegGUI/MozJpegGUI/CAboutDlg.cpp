// CAboutDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "MozJpegGUI.h"
#include "CAboutDlg.h"
#include "afxdialogex.h"
#include "mozJpeg/cjpeg.h"
#include "CLibPNGVer.h"
#include "RPTT.h"
#include <strsafe.h>
#include "GetLastErrorToString.h"

// CAboutDlg ダイアログ

IMPLEMENT_DYNAMIC(CAboutDlg, CDialogEx)

CAboutDlg::CAboutDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ABOUTBOX, pParent)
{

}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAboutDlg メッセージ ハンドラー


BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// ここに初期化を追加してください
	::SetWindowText(GetDlgItem(IDC_MOZJPEGGUI_VERSION)->m_hWnd, GetAppVersion());
	::SetWindowText(GetDlgItem(IDC_MOZJPEG_VERSION)->m_hWnd, CJpeg::Version());
	::SetWindowText(GetDlgItem(IDC_LIBPNG_VERSION)->m_hWnd, CLibPNGVer::GetLibPNGVer());
	::SetWindowText(GetDlgItem(IDC_ZLIB_VERSION)->m_hWnd, CLibPNGVer::GetZLibVer());
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


CString CAboutDlg::GetAppVersion()
{
	CString buff;
	BYTE* pVer = NULL;
	TCHAR filename[MAX_PATH + 1];
	DWORD size = 0;
	VS_FIXEDFILEINFO* pFileInfo = NULL;
	UINT queryLen = 0;
	try {
		if (::GetModuleFileName(NULL, filename, MAX_PATH) == 0) {
			throw CString(_T("Failed to get module file name in GetAppVersion()"));
		}
		size = ::GetFileVersionInfoSize(filename, NULL);
		if (size == 0) {
			throw CString(_T("Failed to get fileversioninfo size in GetAppVersion()"));
		}
		pVer = new BYTE[size];
		if (pVer == NULL) {
			throw CString(_T("Failed to malloc fileinfo in GetAppVersion"));
		}
		if (::GetFileVersionInfo(filename, NULL, size, pVer) == 0) {
			throw CString(_T("Failed get fileinfo in GetAppVersion"));
		}
		if (::VerQueryValue(pVer, _T("\\"), reinterpret_cast<void**>(&pFileInfo), &queryLen) == 0) {
			throw CString(_T("Failed to query version value in GetAppVersion"));
		}

		struct LANGCODE{ WORD lang; WORD code; } *pLangCode;
		if (::VerQueryValue(pVer, _T("\\VarFileInfo\\Translation"), (void**)&pLangCode, &queryLen) == 0) {
			throw CString(_T("Failed to query language info in GetAppVersion"));
		}
		unsigned int i;
		int langNum = 0;
		for (i = 0; i < queryLen / sizeof(LANGCODE); i++) {
			if (pLangCode[i].lang == GetUserDefaultLangID()) {
				langNum = i;
				break;
			}
		}

		TCHAR templ[256];
		StringCchPrintf(templ, 256, _T("\\StringFileInfo\\%04x%04x\\%%s"),
			pLangCode[langNum].lang, pLangCode[langNum].code);

		TCHAR query[256];

		TCHAR* pPrNm;
		StringCchPrintf(query, 256, templ, _T("ProductName"));
		if(::VerQueryValue(pVer, query, (void**)&pPrNm, &queryLen) == 0) {
			throw CString(TEXT("Failed  to query product name info int GetAppVersion"));
		}

		buff.Format(TEXT("%s Version %d.%d.%d"), pPrNm,
			HIWORD(pFileInfo->dwProductVersionMS), LOWORD(pFileInfo->dwProductVersionMS), HIWORD(pFileInfo->dwProductVersionLS));

		delete[] pVer;
		pVer = NULL;
	}
	catch(CString e){
		GetLastErrorToString().CreateMsg();
		delete[] pVer;
		pVer = NULL;
		_RPTFTN(_T("%s\n"), e);
		return CString("");
	}

	return buff;
}
