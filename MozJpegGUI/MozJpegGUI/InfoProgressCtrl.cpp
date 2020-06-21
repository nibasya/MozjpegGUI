// InfoProgressCtrl.cpp : インプリメンテーション ファイル
//

/*

03/08/03    CRgn を用いてクリッピングするようにした
            3D表示に対応した
18/08/29	クラス名をCInfoProgressBarからCInfoProgressCtrlに変更。
			CProgressCtrlのバー表示機能を残したまま、上にテキストを表示するよう変更。
			独自のバー表示機能は削除。SetColorをSetTxtColorに変更。
18/09/08	プログレスバーの伸長速度を切替えるSetSmoothMove(bool smooth)を追加。
			smooth=falseで瞬時に伸長するようになる。デフォルトではtrue(元のコントロールと同じ)。
*/

#include "pch.h"
#include "InfoProgressCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/****************************************************************************
    Name: CInfoProgressCtrl()
    Desc: コンストラクタ．
     Arg: ---
  Return: ---
****************************************************************************/
CInfoProgressCtrl::CInfoProgressCtrl()
{
	// デフォルト色設定
	m_rgbIn  = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_rgbOut = ::GetSysColor(COLOR_WINDOWTEXT);

	// プログレスバーの初期値設定
	m_rangeMin = 0;
	m_rangeMax = 100;
	m_pos = 0;
	m_step = 10;
	m_state = PBST_NORMAL;
	m_previousPos = m_pos;
	m_previousMin = m_rangeMin;
	m_previousMax = m_rangeMax;
	m_previousState = m_state;

	// デフォルト伸長速度
	m_smoothMove = true;
}

/****************************************************************************
    Name: CInfoProgressCtrl()
    Desc: デストラクタ．
     Arg: ---
  Return: ---
****************************************************************************/
CInfoProgressCtrl::~CInfoProgressCtrl()
{
}

/****************************************************************************
    Name: DrawBar()
    Desc: バーを描画する．
     Arg: デバイスコンテキスト
  Return: ---
****************************************************************************/
void CInfoProgressCtrl::DrawText(CDC* pDC)
{
	// バーを描画する領域を求める
	CRect rcBar = m_rcBarMax;
	int	nLower, nUpper;

	GetRange(nLower, nUpper);
	rcBar.right = (rcBar.Width() * (GetPos() - nLower)) / (nUpper - nLower);


	//-----------------------------------------------------------------------
	// 文字列描画

	CString	 strText;
	UINT nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;

	// リージョンを作成
	CRect rcIn = rcBar;			// バー内
	CRect rcOut = m_rcBarMax;	// バー外
	rcOut.left = rcBar.right;

	CRgn rgnIn;
	CRgn rgnOut;
	rgnIn.CreateRectRgnIndirect(rcIn);
	rgnOut.CreateRectRgnIndirect(rcOut);

	GetWindowText(strText);
	pDC->SelectObject(&m_font);
	pDC->SetBkMode(TRANSPARENT);

	// バー内にある文字列描画
	pDC->SetTextColor(m_rgbIn);
	pDC->SelectClipRgn(&rgnIn);
	pDC->DrawText(strText, m_rcBarMax, nFormat);

	// バー外にある文字列描画
	pDC->SetTextColor(m_rgbOut);
	pDC->SelectClipRgn(&rgnOut);
	pDC->DrawText(strText, m_rcBarMax, nFormat);
}

/****************************************************************************
    Name: PreSubclassWindow()
    Desc: サブクラス化の前に呼ばれる．
     Arg: ---
  Return: ---
****************************************************************************/
void CInfoProgressCtrl::PreSubclassWindow() 
{
	// デフォルトの文字列を消去
	SetWindowText(_T(""));

	// フォント作成
	LOGFONT lf;
	GetFont()->GetLogFont(&lf);
	m_font.CreateFontIndirect(&lf);

	CProgressCtrl::PreSubclassWindow();
}


/****************************************************************************

    Message Handlers

****************************************************************************/
BEGIN_MESSAGE_MAP(CInfoProgressCtrl, CProgressCtrl)
	//{{AFX_MSG_MAP(CInfoProgressCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETTEXT, OnSetText)  // 宣言を追加
END_MESSAGE_MAP()

/****************************************************************************
    Name: OnPaint()
    Desc: ---
     Arg: ---
  Return: ---
****************************************************************************/
void CInfoProgressCtrl::OnPaint() 
{
	Default();

	CClientDC dc(this); // 描画用のデバイス コンテキスト

	GetClientRect(&m_rcBarMax);
	m_rcBarMax.DeflateRect(1, 1);

	DrawText(&dc);

}

/****************************************************************************
    Name: OnSetText()
    Desc: 文字列が変更されたとき．
     Arg: ---
  Return: ---
****************************************************************************/
LRESULT CInfoProgressCtrl::OnSetText(WPARAM wParam, LPARAM lParam)
{
	if (CString((LPCTSTR)lParam) == m_previousTxt)
		return TRUE;
	m_previousTxt = CString((LPCTSTR)lParam);

	LRESULT res = Default();
	Invalidate();					// 再描画
	return res;
}

/****************************************************************************
	Name: SetState()
	Desc: プログレスバーの状態をiStateに設定する
	 Arg: iState: 状態
  Return: 直前のプログレスバーの状態
****************************************************************************/
int CInfoProgressCtrl::SetState(int iState)
{
	int pos = m_pos;
	if (m_state == iState)
		return m_state;		// 状態に変化なし

	if (!m_smoothMove) {
		InstantSetPos();
	}
	int ret = CProgressCtrl::SetState(iState);
	m_state = iState;
	return ret;
}

/****************************************************************************
	Name: SetSmoothMove()
	Desc: SetPosによるバーの伸長速度を変更する
	 Arg: smooth: true:滑らかに伸長する(default) false:瞬時に伸長する
  Return: bool 変更前のsmoothを返す
****************************************************************************/
bool CInfoProgressCtrl::SetSmoothMove(const bool smooth)
{
	bool ret = m_smoothMove;
	m_smoothMove = smooth;
	return ret;
}

/****************************************************************************
	Name: SetRange()
	Desc: プログレスバーの下限値と上限値を設定し再描画する SeeAlso:SetRange32
	 Arg: nLower:下限値 nUpper:上限値
  Return: ---
****************************************************************************/
void CInfoProgressCtrl::SetRange(short nLower, short nUpper)
{
	int min = m_rangeMin;
	int max = m_rangeMax;

	m_rangeMin = nLower;
	m_rangeMax = nUpper;

	CProgressCtrl::SetRange(nLower, nUpper);
	if (!m_smoothMove) {
		InstantSetPos();
	}
	m_previousMin = min;
	m_previousMax = max;
}

/****************************************************************************
	Name: SetRange32()
	Desc: プログレスバーの下限値と上限値を設定し再描画する SeeAlso:SetRange
	 Arg: nLower:下限値 nUpper:上限値
  Return: ---
****************************************************************************/
void CInfoProgressCtrl::SetRange32(int nLower, int nUpper)
{
	int min = m_rangeMin;
	int max = m_rangeMax;

	m_rangeMin = nLower;
	m_rangeMax = nUpper;

	CProgressCtrl::SetRange32(nLower, nUpper);
	if (!m_smoothMove) {
		InstantSetPos();
	}
	m_previousMin = min;
	m_previousMax = max;
}

/****************************************************************************
	Name: SetStep()
	Desc: プログレスバーを伸ばす量を設定する
	 Arg: nStep:伸長量
  Return: 直前の伸長量
****************************************************************************/
int CInfoProgressCtrl::SetStep(int nStep)
{
	m_step = nStep;
	return CProgressCtrl::SetStep(nStep);
}

/****************************************************************************
	Name: StepIt()
	Desc: プログレスバーを伸長する
	 Arg: ---
  Return: 直前のプログレスバーの位置
****************************************************************************/
int CInfoProgressCtrl::StepIt()
{
	m_pos += m_step;
	if (!m_smoothMove) {
		return InstantSetPos();
	}
	m_previousPos = m_pos;
	return CProgressCtrl::StepIt();
}

/****************************************************************************
	Name: OffsetPos(int nPos)
	Desc: プログレスバーをnPos伸ばす
	 Arg: nPos:伸長量
  Return: 直前のプログレスバーの位置
****************************************************************************/
int CInfoProgressCtrl::OffsetPos(int nPos)
{
	m_pos += nPos;
	if (!m_smoothMove) {
		return InstantSetPos();
	}
	m_previousPos = m_pos;
	return CProgressCtrl::OffsetPos(nPos);
}

/****************************************************************************
	Name: SetPos()
	Desc: プログレスバーの位置をnPosに設定する
	 Arg: nPos: 位置
  Return: 直前のプログレスバーの位置
****************************************************************************/
int CInfoProgressCtrl::SetPos(int nPos)
{
	m_pos = nPos;
	if (!m_smoothMove) {
		return InstantSetPos();
	}
	m_previousPos = m_pos;
	return CProgressCtrl::SetPos(nPos);
}

/****************************************************************************
	Name: InstantSetPos()
	Desc: プログレスバーの位置を瞬時にm_posに設定する
	 Arg: nPos: 位置
  Return: 直前のプログレスバーの位置
****************************************************************************/
int CInfoProgressCtrl::InstantSetPos() {
	if (m_previousPos == m_pos && m_previousMin == m_rangeMin && m_previousMax == m_rangeMax && m_previousState == m_state)
		return m_pos;	// 更新不要
	m_previousPos = m_pos;
	m_previousMin = m_rangeMin;
	m_previousMax = m_rangeMax;
	m_previousState = m_state;

	int ret;
	if (m_pos >= m_rangeMax) {
		CProgressCtrl::SetRange32(m_rangeMin, m_pos + 1);
		ret = CProgressCtrl::SetPos(m_pos + 1);
		CProgressCtrl::SetPos(m_pos);
		CProgressCtrl::SetRange32(m_rangeMin, m_rangeMax);
	}
	else {
		ret = CProgressCtrl::SetPos(m_pos + 1);
		CProgressCtrl::SetPos(m_pos);
	}

	if (m_rangeMin > m_pos)
		m_pos = m_rangeMin;
	if (m_rangeMax < m_pos)
		m_pos = m_rangeMax;

	return ret;
}
