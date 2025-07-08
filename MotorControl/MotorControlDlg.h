
// MotorControlDlg.h : ͷ�ļ�
//

#pragma once
#include "ModbusRTUHelper.h"

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
	
	// �ؼ�����
	CComboBox m_cmbComPort;
	CComboBox m_cmbBaud;
	CComboBox m_cmbNodeID;

	//LED
	CStatic m_ledX1;
	CStatic m_ledX2;
	CStatic m_ledX3;
	HICON m_hIconLedGreen; 
	HICON m_hIconLedGray;  

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

	//���԰�ť������ɾ��
	void TestMotorCommunication();

public:
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedButtonHome();
	afx_msg void OnBnClickedButtonTest();

private:
	void UpdateSensorStatus();  //���´�����״̬
	void ScanComPorts();
	void CMotorControlDlg::UpdateConnectStatus(bool isConnect);
};
