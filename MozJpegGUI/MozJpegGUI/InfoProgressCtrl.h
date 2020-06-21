/****************************************************************************

    Name: InfoProgresssCtrl.h

    Desc: CProgressCtrlに文字情報表示機能を追加．

  Author: donadona
  reidphoaさんのInfoProgressBar.hより改変

*********************************************************************/
#if !defined(AFX_INFOPROGRESSCTRL_H__09E9DBA9_0BB8_4EF2_9208_C4A44A679B89__INCLUDED_)
#define AFX_INFOPROGRESSCTRL_H__09E9DBA9_0BB8_4EF2_9208_C4A44A679B89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CInfoProgressCtrl : public CProgressCtrl
{
public:

	CInfoProgressCtrl();
	virtual ~CInfoProgressCtrl();

	virtual void SetTxtColor(COLORREF rgbIn, COLORREF rgbOut)
	{
		m_rgbIn  = rgbIn;
		m_rgbOut = rgbOut;
	}
	// smooth:true(default) バーを滑らかに伸ばす false:バーを速やかに伸ばす
	virtual bool SetSmoothMove(const bool smooth);

	// オーバーライド
	//{{AFX_VIRTUAL(CInfoProgressCtrl)

	virtual void PreSubclassWindow();

	//}}AFX_VIRTUAL

	// プログレスバーの下限値と上限値を設定し再描画する
	virtual void SetRange(short nLower, short nUpper);
	// プログレスバーの下限値と上限値を設定し再描画する
	virtual void SetRange32(int nLower, int nUpper);
	// プログレスバーを伸ばす量を設定する。返り値：直前の伸長量
	virtual int SetStep(int nStep);
	// プログレスバーを伸ばす。返り値：直前の位置
	virtual int StepIt();
	// プログレスバーをnPos伸ばす。返り値：直前の位置
	virtual int OffsetPos(int nPos);
	// プログレスバーの位置を設定する。返り値：直前の位置
	virtual int SetPos(int nPos);
	// REVIEW: Sets the state of the progress bar.
	virtual int SetState(int iState);


protected:
	virtual void DrawText(CDC* pDC);
	// プログレスバーの位置を瞬間的にm_Posへ変更する
	int InstantSetPos();


	CFont		m_font;			// フォント
	COLORREF	m_rgbIn;		// 文字列の色(バー内）
	COLORREF	m_rgbOut;		// 文字列の色(バー外)
	CRect		m_rcBarMax;		// バーの領域
	bool		m_smoothMove;	// true:滑らかに伸びる(デフォルト動作)
	int			m_rangeMin;		// プログレスバーコントロールの下限値
	int			m_rangeMax;		// プログレスバーコントロールの上限値
	int			m_pos;			// プログレスバーコントロールの現在値
	int			m_step;			// プログレスバーコントロールのステップ値
	int			m_state;		// プログレスバーコントロールの状態
	int			m_previousPos;	// プログレスバーコントロールの過去値
	int			m_previousMin;	// プログレスバーコントロールの過去下限値
	int			m_previousMax;	// プログレスバーコントロールの過去上限値
	CString		m_previousTxt;	// プログレスバーコントロールの過去テキスト
	int			m_previousState;// プログレスバーコントロールの過去状態

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CInfoProgressCtrl)
	afx_msg void OnPaint();
	afx_msg LRESULT	OnSetText(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_INFOPROGRESSCTRL_H__09E9DBA9_0BB8_4EF2_9208_C4A44A679B89__INCLUDED_)
