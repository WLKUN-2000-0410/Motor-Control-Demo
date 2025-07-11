
// MotorControlDlg.h : ͷ�ļ�
//
#pragma once

#include "ModbusRTUHelper.h"     //Modbus RTUͨ�Ÿ�����
#include "JogButton.h"			 //�Զ���JOG��ť��



// CMotorControlDlg �Ի���
class CMotorControlDlg : public CDialogEx
{
public:
	CMotorControlDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CMotorControlDlg();
// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOTORCONTROL_DIALOG };
#endif
	enum { ID_TIMER_UPDATE_STATUS = 1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	

// ʵ��
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
	int m_nRelDirection;					// 0 for CCW (����), 1 for CW (����)

	int m_nStepsPerRevolution;              //���ÿת�Ĳ�������ÿת20000��
	double m_dPhysicalValuePerRevolution;   //ÿת�ĵ�λ�� 
	CString m_sPhysicalUnitName;            //��λ���ƣ���mm��um��

	// �ؼ�����
	CComboBox m_cmbComPort;
	CComboBox m_cmbBaud;
	CComboBox m_cmbNodeID;
      
	CJogButton m_btnJogPlus;      //��ת
	CJogButton m_btnJogMinus;     //��ת
	CComboBox m_cmbRelDirection;  //����ѡ��
	CEdit m_editCurrentPosition;
	//LED
	CStatic m_ledX1;
	CStatic m_ledX2;
	CStatic m_ledX3;
	HICON m_hIconLedGreen; 
	HICON m_hIconLedGray;  

	//��־����༭��
	CEdit m_editLog;

	// ���ɵ���Ϣӳ�亯��
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
	void UpdateSensorStatus();										  //���´�����x1,x2,x3״̬
	void ScanComPorts();											  //ɨ��com��
	void UpdateConnectStatus(bool isConnect, bool isHoming = false);  //**���´��ڱ���Ͱ�ťʹ��״̬
	void CheckHomingStatus();
	void StartHomingProcess();
	void ExecuteOneEscapeStep();
	
	//��־
	void AppendLog(const CString& sMessage);					      //ͳһ����־���ɺ�����ͬʱ�����UI����־�ļ�
	void RotateLogFiles();											  //������͹�����־�ļ��ĺ���
	CString GetLogFilePath(const CString& sFileName);                 //��־����λ��:
	
	//��λת��
	bool IsKinematicParamsValid();									  //����˶�ѧ�����Ƿ���Ч������ÿת��λ���͵�λ����
	double ConvertUnitsPerSecToRps(double units_per_sec);			  //Units-->rps
	double ConvertUnitsPerSecSqToRpsPerSecSq(double units_per_sec_sq);//Units-->rps/s
	long ConvertUnitsToSteps(double units);							  //Units-->steps
	double ConvertStepsToUnits(long steps);							  //Steps-->Units

	bool CMotorControlDlg::IsStepsValueSafe(long steps);
	bool CMotorControlDlg::ValidateMotionParameters(double position_units, CString& errorMsg);
};
