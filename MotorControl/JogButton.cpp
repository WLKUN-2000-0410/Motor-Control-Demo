// JogButton.cpp
#include "stdafx.h"
#include "MotorControl.h" // 确保包含了你的主头文件
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
	// 向父窗口发送按钮按下的消息，并将按钮的ID作为参数(WPARAM)传递过去
	GetParent()->SendMessage(WM_JOG_BUTTON_DOWN, GetDlgCtrlID(), 0);

	// 调用基类的函数，以确保按钮的标准行为（如绘制按下状态）得以执行
	CButton::OnLButtonDown(nFlags, point);
}


void CJogButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// 向父窗口发送按钮抬起的消息
	GetParent()->SendMessage(WM_JOG_BUTTON_UP, GetDlgCtrlID(), 0);

	CButton::OnLButtonUp(nFlags, point);
}