
// MozJpegGUI.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CMozJpegGUIApp:
// このクラスの実装については、MozJpegGUI.cpp を参照してください
//

class CMozJpegGUIApp : public CWinApp
{
public:
	CMozJpegGUIApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CMozJpegGUIApp theApp;
