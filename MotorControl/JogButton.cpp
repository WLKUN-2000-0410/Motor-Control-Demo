// JogButton.cpp
#include "stdafx.h"
#include "MotorControl.h" // ȷ�������������ͷ�ļ�
#include "JogButton.h"

IMPLEMENT_DYNAMIC(CJogButton, CButton)

CJogButton::CJogButton() {}
CJogButton::~CJogButton() {}

BEGIN_MESSAGE_MAP(CJogButton, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void CJogButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// �򸸴��ڷ��Ͱ�ť���µ���Ϣ��������ť��ID��Ϊ����(WPARAM)���ݹ�ȥ
	GetParent()->SendMessage(WM_JOG_BUTTON_DOWN, GetDlgCtrlID(), 0);

	// ���û���ĺ�������ȷ����ť�ı�׼��Ϊ������ư���״̬������ִ��
	CButton::OnLButtonDown(nFlags, point);
}


void CJogButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// �򸸴��ڷ��Ͱ�ţ̌�����Ϣ
	GetParent()->SendMessage(WM_JOG_BUTTON_UP, GetDlgCtrlID(), 0);

	CButton::OnLButtonUp(nFlags, point);
}