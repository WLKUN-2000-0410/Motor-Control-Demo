
// MotorControlDlg.h : ͷ�ļ�
//

#pragma once
#include "ModbusRTUHelper.h"
#include "JogButton.h"
// CMotorControlDlg �Ի���
class CMotorControlDlg : public CDialogEx
{
// ����
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

	// ��Ա����
	ModbusRTUHelper m_ModbusRTUHelper;
	int m_nCOMPort;
	int m_nBaudRate;
	byte m_nNodeID;
	
	double m_dJogSpeed;
	double m_dJogAccel;
	double m_dJogDecel;

	CString m_sCurrentPosition;
	BOOL m_bAutoHomeOnConnect; 
	bool m_bIsHoming;          //  ���ڻ����״̬��־
	bool m_bIsEscapingHomeSensor;

	double m_dPtpVelocity;   // PTP: Point-to-Point
	double m_dPtpAccel;
	double m_dPtpDecel;
	int m_nAbsPosition;
	int m_nRelPosition;
	int m_nRelDirection;     // 0 for CW (����), 1 for CCW (����)

	// �ؼ�����
	CComboBox m_cmbComPort;
	CComboBox m_cmbBaud;
	CComboBox m_cmbNodeID;
	CEdit m_editJogSpeed;
	CEdit m_editJogAccel;
	CEdit m_editJogDecel;       
	CJogButton m_btnJogPlus;      //��ת
	CJogButton m_btnJogMinus;     //��ת
	CEdit m_editCurrentPosition;  //��ǰλ��
	CComboBox m_cmbRelDirection;  //����ѡ��

	//LED
	CStatic m_ledX1;
	CStatic m_ledX2;
	CStatic m_ledX3;
	HICON m_hIconLedGreen; 
	HICON m_hIconLedGray;  

	//LED
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

	//���԰�ť������ɾ��
	void TestMotorCommunication();

public:
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedButtonHome();
	afx_msg void OnBnClickedButtonTest();

	afx_msg void OnBnClickedButtonAbsMoveStart();
	afx_msg void OnBnClickedButtonAbsMoveStop();
	afx_msg void OnBnClickedButtonRelMoveStart();
	afx_msg void OnBnClickedButtonRelMoveStop();

private:
	void UpdateSensorStatus();  //���´�����״̬
	void ScanComPorts();
	void UpdateConnectStatus(bool isConnect, bool isHoming = false); // <--- 3. �޸ĺ��������Դ������״̬
	void CheckHomingStatus();
	void StartHomingProcess();
	void ExecuteOneEscapeStep();
	//log
	void AppendLog(const CString& sMessage); // ͳһ����־׷�Ӻ���
	void RotateLogFiles();                   // ������͹�����־�ļ��ĺ���
	CString GetLogFilePath(const CString& sFileName);
};
