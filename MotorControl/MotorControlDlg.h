
// MotorControlDlg.h : 头文件
//
#pragma once

#include "ModbusRTUHelper.h"     //Modbus RTU通信辅助类
#include "JogButton.h"			 //自定义JOG按钮类



// CMotorControlDlg 对话框
class CMotorControlDlg : public CDialogEx
{
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

	ModbusRTUHelper m_ModbusRTUHelper;
	int m_nCOMPort;
	int m_nBaudRate;
	byte m_nNodeID;
	
	double m_dJogSpeed;			
	double m_dJogAccel;
	double m_dJogDecel;

	CString m_sCurrentPosition; 
	BOOL m_bAutoHomeOnConnect;  
	bool m_bIsHoming;          
	bool m_bIsEscapingHomeSensor;

	double m_dPtpVelocity;					// PTP: Point-to-Point
	double m_dPtpAccel;
	double m_dPtpDecel;
	double m_nAbsPosition;
	double m_nRelPosition;
	int m_nRelDirection;					// 0 for CCW (反向), 1 for CW (正向)

	int m_nStepsPerRevolution;              //电机每转的步数，如每转20000步
	double m_dPhysicalValuePerRevolution;   //每转的单位数 
	CString m_sPhysicalUnitName;            //单位名称，如mm，um等

	// 控件变量
	CComboBox m_cmbComPort;
	CComboBox m_cmbBaud;
	CComboBox m_cmbNodeID;
      
	CJogButton m_btnJogPlus;      //正转
	CJogButton m_btnJogMinus;     //反转
	CComboBox m_cmbRelDirection;  //方向选择
	CEdit m_editCurrentPosition;
	//LED
	CStatic m_ledX1;
	CStatic m_ledX2;
	CStatic m_ledX3;
	HICON m_hIconLedGreen; 
	HICON m_hIconLedGray;  

	//日志输出编辑框
	CEdit m_editLog;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnJogButtonDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnJogButtonUp(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()


public:
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedButtonHome();
	afx_msg void OnBnClickedButtonEmergencyStop();

	afx_msg void OnBnClickedButtonAbsMoveStart();
	afx_msg void OnBnClickedButtonAbsMoveStop();
	afx_msg void OnBnClickedButtonRelMoveStart();
	afx_msg void OnBnClickedButtonRelMoveStop();

private:
	void UpdateSensorStatus();										  //更新传感器x1,x2,x3状态
	void ScanComPorts();											  //扫描com口
	void UpdateConnectStatus(bool isConnect, bool isHoming = false);  //**更新窗口标题和按钮使能状态
	void CheckHomingStatus();
	void StartHomingProcess();
	void ExecuteOneEscapeStep();
	
	//日志
	void AppendLog(const CString& sMessage);					      //统一的日志生成函数，同时输出到UI和日志文件
	void RotateLogFiles();											  //负责检查和滚动日志文件的函数
	CString GetLogFilePath(const CString& sFileName);                 //日志生成位置:
	
	//单位转换
	bool IsKinematicParamsValid();									  //检查运动学参数是否有效，包括每转单位数和单位名称
	double ConvertUnitsPerSecToRps(double units_per_sec);			  //Units-->rps
	double ConvertUnitsPerSecSqToRpsPerSecSq(double units_per_sec_sq);//Units-->rps/s
	long ConvertUnitsToSteps(double units);							  //Units-->steps
	double ConvertStepsToUnits(long steps);							  //Steps-->Units

	bool CMotorControlDlg::IsStepsValueSafe(long steps);
	bool CMotorControlDlg::ValidateMotionParameters(double position_units, CString& errorMsg);
};
