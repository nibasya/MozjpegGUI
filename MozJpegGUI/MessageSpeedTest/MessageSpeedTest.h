
// MessageSpeedTest.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CMessageSpeedTestApp:
// このクラスの実装については、MessageSpeedTest.cpp を参照してください
//

class CMessageSpeedTestApp : public CWinApp
{
public:
	CMessageSpeedTestApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CMessageSpeedTestApp theApp;
