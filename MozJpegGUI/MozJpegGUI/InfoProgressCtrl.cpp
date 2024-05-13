// InfoProgressCtrl.cpp : �C���v�������e�[�V���� �t�@�C��
//

/*

03/08/03    CRgn ��p���ăN���b�s���O����悤�ɂ���
            3D�\���ɑΉ�����
18/08/29	�N���X����CInfoProgressBar����CInfoProgressCtrl�ɕύX�B
			CProgressCtrl�̃o�[�\���@�\���c�����܂܁A��Ƀe�L�X�g��\������悤�ύX�B
			�Ǝ��̃o�[�\���@�\�͍폜�BSetColor��SetTxtColor�ɕύX�B
18/09/08	�v���O���X�o�[�̐L�����x��ؑւ���SetSmoothMove(bool smooth)��ǉ��B
			smooth=false�ŏu���ɐL������悤�ɂȂ�B�f�t�H���g�ł�true(���̃R���g���[���Ɠ���)�B
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
    Desc: �R���X�g���N�^�D
     Arg: ---
  Return: ---
****************************************************************************/
CInfoProgressCtrl::CInfoProgressCtrl()
{
	// �f�t�H���g�F�ݒ�
	m_rgbIn  = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_rgbOut = ::GetSysColor(COLOR_WINDOWTEXT);

	// �v���O���X�o�[�̏����l�ݒ�
	m_rangeMin = 0;
	m_rangeMax = 100;
	m_pos = 0;
	m_step = 10;
	m_state = PBST_NORMAL;
	m_previousPos = m_pos;
	m_previousMin = m_rangeMin;
	m_previousMax = m_rangeMax;
	m_previousState = m_state;

	// �f�t�H���g�L�����x
	m_smoothMove = true;
}

/****************************************************************************
    Name: CInfoProgressCtrl()
    Desc: �f�X�g���N�^�D
     Arg: ---
  Return: ---
****************************************************************************/
CInfoProgressCtrl::~CInfoProgressCtrl()
{
}

/****************************************************************************
    Name: DrawBar()
    Desc: �o�[��`�悷��D
     Arg: �f�o�C�X�R���e�L�X�g
  Return: ---
****************************************************************************/
void CInfoProgressCtrl::DrawText(CDC* pDC)
{
	// �o�[��`�悷��̈�����߂�
	CRect rcBar = m_rcBarMax;
	int	nLower, nUpper;

	GetRange(nLower, nUpper);
	rcBar.right = (rcBar.Width() * (GetPos() - nLower)) / (nUpper - nLower);


	//-----------------------------------------------------------------------
	// ������`��

	CString	 strText;
	UINT nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;

	// ���[�W�������쐬
	CRect rcIn = rcBar;			// �o�[��
	CRect rcOut = m_rcBarMax;	// �o�[�O
	rcOut.left = rcBar.right;

	CRgn rgnIn;
	CRgn rgnOut;
	rgnIn.CreateRectRgnIndirect(rcIn);
	rgnOut.CreateRectRgnIndirect(rcOut);

	GetWindowText(strText);
	pDC->SelectObject(&m_font);
	pDC->SetBkMode(TRANSPARENT);

	// �o�[���ɂ��镶����`��
	pDC->SetTextColor(m_rgbIn);
	pDC->SelectClipRgn(&rgnIn);
	pDC->DrawText(strText, m_rcBarMax, nFormat);

	// �o�[�O�ɂ��镶����`��
	pDC->SetTextColor(m_rgbOut);
	pDC->SelectClipRgn(&rgnOut);
	pDC->DrawText(strText, m_rcBarMax, nFormat);
}

/****************************************************************************
    Name: PreSubclassWindow()
    Desc: �T�u�N���X���̑O�ɌĂ΂��D
     Arg: ---
  Return: ---
****************************************************************************/
void CInfoProgressCtrl::PreSubclassWindow() 
{
	// �f�t�H���g�̕����������
	SetWindowText(_T(""));

	// �t�H���g�쐬
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
	ON_MESSAGE(WM_SETTEXT, OnSetText)  // �錾��ǉ�
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

	CClientDC dc(this); // �`��p�̃f�o�C�X �R���e�L�X�g

	GetClientRect(&m_rcBarMax);
	m_rcBarMax.DeflateRect(1, 1);

	DrawText(&dc);

}

/****************************************************************************
    Name: OnSetText()
    Desc: �����񂪕ύX���ꂽ�Ƃ��D
     Arg: ---
  Return: ---
****************************************************************************/
LRESULT CInfoProgressCtrl::OnSetText(WPARAM wParam, LPARAM lParam)
{
	if (CString((LPCTSTR)lParam) == m_previousTxt)
		return TRUE;
	m_previousTxt = CString((LPCTSTR)lParam);

	LRESULT res = Default();
	Invalidate();					// �ĕ`��
	return res;
}

/****************************************************************************
	Name: SetState()
	Desc: �v���O���X�o�[�̏�Ԃ�iState�ɐݒ肷��
	 Arg: iState: ���
  Return: ���O�̃v���O���X�o�[�̏��
****************************************************************************/
int CInfoProgressCtrl::SetState(int iState)
{
	int pos = m_pos;
	if (m_state == iState)
		return m_state;		// ��Ԃɕω��Ȃ�

	if (!m_smoothMove) {
		InstantSetPos();
	}
	int ret = CProgressCtrl::SetState(iState);
	m_state = iState;
	return ret;
}

/****************************************************************************
	Name: SetSmoothMove()
	Desc: SetPos�ɂ��o�[�̐L�����x��ύX����
	 Arg: smooth: true:���炩�ɐL������(default) false:�u���ɐL������
  Return: bool �ύX�O��smooth��Ԃ�
****************************************************************************/
bool CInfoProgressCtrl::SetSmoothMove(const bool smooth)
{
	OutputDebugLog(_T("CInfoProgressCtrl::SetSmoothMove(%s) called.\n"), smooth?_T("true"):_T("false"));

	bool ret = m_smoothMove;
	m_smoothMove = smooth;
	OutputDebugLog(_T("CInfoProgressCtrl::SetSmoothMove() returned %s.\n"), ret ? _T("true") : _T("false"));
	return ret;
}

/****************************************************************************
	Name: SetRange()
	Desc: �v���O���X�o�[�̉����l�Ə���l��ݒ肵�ĕ`�悷�� SeeAlso:SetRange32
	 Arg: nLower:�����l nUpper:����l
  Return: ---
****************************************************************************/
void CInfoProgressCtrl::SetRange(short nLower, short nUpper)
{
	OutputDebugLog(_T("CInfoProgressCtrl::SetRange(%hd, %hd) called.\n"), nLower, nUpper);
	int min = m_rangeMin;
	int max = m_rangeMax;
	OutputDebugLog(_T("CInfoProgressCtrl::SetRange: min = %d, max = %d\n"), min, max);

	m_rangeMin = nLower;
	m_rangeMax = nUpper;
	OutputDebugLog(_T("CInfoProgressCtrl::SetRange: m_rangeMin = %d, m_rangeMax = %d\n"), m_rangeMin, m_rangeMax);

	CProgressCtrl::SetRange(nLower, nUpper);
	OutputDebugLog(_T("CInfoProgressCtrl::SetRange CProgressCtrl::SetRange(%hd, %hd) done.\n"), nLower, nUpper);

	if (!m_smoothMove) {
		OutputDebugLog(_T("CInfoProgressCtrl::SetRange: Calling InstantSetPos()\n"));
		InstantSetPos();
		OutputDebugLog(_T("CInfoProgressCtrl::SetRange: InstantSetPos() done.\n"));
	}
	m_previousMin = min;
	m_previousMax = max;
	OutputDebugLog(_T("CInfoProgressCtrl::SetRange: m_previousMin: %d, m_previousMax: %d\n"), m_previousMin, m_previousMax);
}

/****************************************************************************
	Name: SetRange32()
	Desc: �v���O���X�o�[�̉����l�Ə���l��ݒ肵�ĕ`�悷�� SeeAlso:SetRange
	 Arg: nLower:�����l nUpper:����l
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
	Desc: �v���O���X�o�[��L�΂��ʂ�ݒ肷��
	 Arg: nStep:�L����
  Return: ���O�̐L����
****************************************************************************/
int CInfoProgressCtrl::SetStep(int nStep)
{
	m_step = nStep;
	return CProgressCtrl::SetStep(nStep);
}

/****************************************************************************
	Name: StepIt()
	Desc: �v���O���X�o�[��L������
	 Arg: ---
  Return: ���O�̃v���O���X�o�[�̈ʒu
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
	Desc: �v���O���X�o�[��nPos�L�΂�
	 Arg: nPos:�L����
  Return: ���O�̃v���O���X�o�[�̈ʒu
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
	Desc: �v���O���X�o�[�̈ʒu��nPos�ɐݒ肷��
	 Arg: nPos: �ʒu
  Return: ���O�̃v���O���X�o�[�̈ʒu
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
	Desc: �v���O���X�o�[�̈ʒu���u����m_pos�ɐݒ肷��
	 Arg: nPos: �ʒu
  Return: ���O�̃v���O���X�o�[�̈ʒu
****************************************************************************/
int CInfoProgressCtrl::InstantSetPos() {
	OutputDebugLog(_T("CInfoProgressCtrl::InstantSetPos called.\n"));
	if (m_previousPos == m_pos && m_previousMin == m_rangeMin && m_previousMax == m_rangeMax && m_previousState == m_state) {
		OutputDebugLog(_T("CInfoProgressCtrl::InstantSetPos no update required. return %d.\n"), m_pos);
		return m_pos;	// �X�V�s�v
	}
	m_previousPos = m_pos;
	m_previousMin = m_rangeMin;
	m_previousMax = m_rangeMax;
	m_previousState = m_state;
	OutputDebugLog(_T("CInfoProgressCtrl::InstantSetPos m_previousPos=%d, m_previousMin=%d, m_previousMax=%d, m_previousState=%d\n"),
		m_previousPos, m_previousMin, m_previousMax, m_previousState);

	int ret;
	if (m_pos >= m_rangeMax) {
		OutputDebugLog(_T("CInfoProgressCtrl::InstantSetPos m_pos(%d) >= m_rangeMax(%d)\n"), m_pos, m_rangeMax);
		CProgressCtrl::SetRange32(m_rangeMin, m_pos + 1);
		OutputDebugLog(_T("CInfoProgressCtrl::InstantSetPos CProgressCtrl::SetRange32(%d, %d) done.\n"), m_rangeMin, m_pos + 1);
		ret = CProgressCtrl::SetPos(m_pos + 1);
		OutputDebugLog(_T("CInfoProgressCtrl::InstantSetPos CProgressCtrl::SetPos(%d) done. ret:%d\n"), m_pos + 1, ret);
		CProgressCtrl::SetPos(m_pos);
		OutputDebugLog(_T("CInfoProgressCtrl::InstantSetPos CProgressCtrl::SetPos(%d) done.\n"), m_pos);
		CProgressCtrl::SetRange32(m_rangeMin, m_rangeMax);
		OutputDebugLog(_T("CInfoProgressCtrl::InstantSetPos CProgressCtrl::SetRange32(%d, %d) done.\n"), m_rangeMin, m_rangeMax);
	}
	else {
		OutputDebugLog(_T("CInfoProgressCtrl::InstantSetPos m_pos(%d) < m_rangeMax(%d)\n"), m_pos, m_rangeMax);
		ret = CProgressCtrl::SetPos(m_pos + 1);
		OutputDebugLog(_T("CInfoProgressCtrl::InstantSetPos CProgressCtrl::SetPos(%d) done. ret:%d\n"), m_pos + 1, ret);
		CProgressCtrl::SetPos(m_pos);
		OutputDebugLog(_T("CInfoProgressCtrl::InstantSetPos CProgressCtrl::SetPos(%d) done.\n"), m_pos);
	}

	if (m_rangeMin > m_pos)
		m_pos = m_rangeMin;
	if (m_rangeMax < m_pos)
		m_pos = m_rangeMax;
	OutputDebugLog(_T("CInfoProgressCtrl::InstantSetPos m_pos=%d\n"), m_pos);
	OutputDebugLog(_T("CInfoProgressCtrl::InstantSetPos returning %d\n"), ret);
	return ret;
}
