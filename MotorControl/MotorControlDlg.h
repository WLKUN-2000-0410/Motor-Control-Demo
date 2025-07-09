
// MotorControlDlg.h : 头文件
//

#pragma once
#include "ModbusRTUHelper.h"
#include "JogButton.h"
// CMotorControlDlg 对话框
class CMotorControlDlg : public CDialogEx
{
// 构造
public:
	CMotorControlDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CMotorControlDlg();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOTORCONTROL_DIALOG };
#endif
	enum { ID_TIMER_UPDATE_STATUS = 1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	

// 实现
protected:
	HICON m_hIcon;

	// 成员变量
	ModbusRTUHelper m_ModbusRTUHelper;
	int m_nCOMPort;
	int m_nBaudRate;
	byte m_nNodeID;
	
	double m_dJogSpeed;
	double m_dJogAccel;
	double m_dJogDecel;

	CString m_sCurrentPosition;
	BOOL m_bAutoHomeOnConnect; // <--- 1. 与复选框关联的变量
	bool m_bIsHoming;          // <--- 2. 正在回零的状态标志

	// 控件变量
	CComboBox m_cmbComPort;
	CComboBox m_cmbBaud;
	CComboBox m_cmbNodeID;
	CEdit m_editJogSpeed;
	CEdit m_editJogAccel;
	CEdit m_editJogDecel;
	CJogButton m_btnJogPlus;  
	CJogButton m_btnJogMinus; 
	CEdit m_editCurrentPosition;

	//LED
	CStatic m_ledX1;
	CStatic m_ledX2;
	CStatic m_ledX3;
	HICON m_hIconLedGreen; 
	HICON m_hIconLedGray;  

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnJogButtonDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnJogButtonUp(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	//测试按钮，后续删除
	void TestMotorCommunication();

public:
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedButtonHome();
	afx_msg void OnBnClickedButtonTest();

private:
	void UpdateSensorStatus();  //更新传感器状态
	void ScanComPorts();
	void UpdateConnectStatus(bool isConnect, bool isHoming = false); // <--- 3. 修改函数声明以处理回零状态
	void CheckHomingStatus();
};
