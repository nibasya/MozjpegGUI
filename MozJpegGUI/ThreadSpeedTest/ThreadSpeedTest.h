
// ThreadSpeedTest.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CThreadSpeedTestApp:
// このクラスの実装については、ThreadSpeedTest.cpp を参照してください
//

class CThreadSpeedTestApp : public CWinApp
{
public:
	CThreadSpeedTestApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CThreadSpeedTestApp theApp;
