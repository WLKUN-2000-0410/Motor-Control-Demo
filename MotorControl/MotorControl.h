
// MotorControl.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMotorControlApp: 
// �йش����ʵ�֣������ MotorControl.cpp
//

class CMotorControlApp : public CWinApp
{
public:
	CMotorControlApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMotorControlApp theApp;