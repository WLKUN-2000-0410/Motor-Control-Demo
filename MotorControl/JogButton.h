#pragma once

// 定义两个自定义消息，用于通知父窗口
#define WM_JOG_BUTTON_DOWN (WM_USER + 101)
#define WM_JOG_BUTTON_UP   (WM_USER + 102)

// CJogButton

class CJogButton : public CButton
{
	DECLARE_DYNAMIC(CJogButton)

public:
	CJogButton();
	virtual ~CJogButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


