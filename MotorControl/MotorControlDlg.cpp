// MotorControlDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MotorControl.h"
#include "MotorControlDlg.h"
#include "afxdialogex.h"
#include <Shlwapi.h>          
#pragma comment(lib, "shlwapi.lib") 


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const UINT_PTR TIMER_INTERVAL = 100;  //��ʱ��ʱ����

// ����Ӧ�ó���"����"�˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

														// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMotorControlDlg �Ի���



CMotorControlDlg::CMotorControlDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MOTORCONTROL_DIALOG, pParent)
	, m_nBaudRate(19200)				//Ĭ��19200
	, m_nNodeID(1)                      //Ĭ��IDΪ1 
	, m_dJogSpeed(10.0)                 //JOGĬ���ٶ�10unit/s
	, m_dJogAccel(100.0)                //JOGĬ�ϼ��ٶ�100unit/s
	, m_dJogDecel(100.0)                //JOGĬ�ϼ��ٶ�100unit/s
	, m_sCurrentPosition(_T("0 steps")) //λ�ó�ʼ��ʾ0steps
	, m_bAutoHomeOnConnect(TRUE)		//Ĭ�Ϲ�ѡ���Զ����㡱
	, m_dPtpVelocity(10.0)              //PTPĬ���ٶ�10unit/s
	, m_dPtpAccel(100.0)				//PTPĬ�ϼ��ٶ�100unit/s
	, m_dPtpDecel(100.0)				//PTPĬ�ϼ��ٶ�100unit/s
	, m_nAbsPosition(0)					//PositionĬ�϶���0
	, m_nRelPosition(0)
	, m_nRelDirection(1)				//Ĭ�Ϸ���Ϊ��
	, m_nStepsPerRevolution(20000)      //Ĭ��20000��
	, m_dPhysicalValuePerRevolution(1.0)//Ĭ��ÿת1.0����λ (��1mm)
	, m_sPhysicalUnitName(_T("mm"))     //Ĭ�ϵ�λ����Ϊ����
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); //���ز�����Ӧ�ó���ͼ�꣬���Ͻǣ��������е�  
	m_hIconLedGray = NULL; // ��ʼ��Ϊ NULL
	m_hIconLedGreen = NULL;
	m_bIsHoming = false;
	m_bIsEscapingHomeSensor = false;
}


CMotorControlDlg::~CMotorControlDlg()
{
	// ȷ����ʱ���ڶԻ�������ʱ��ֹͣ
	KillTimer(ID_TIMER_UPDATE_STATUS);

	// �ͷ�ͼ����
	if (m_hIconLedGray) DestroyIcon(m_hIconLedGray);
	if (m_hIconLedGreen) DestroyIcon(m_hIconLedGreen);
}


void CMotorControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//connect
	DDX_Control(pDX, IDC_CMB_COMPORT, m_cmbComPort);
	DDX_Control(pDX, IDC_CMB_BAUD, m_cmbBaud);
	DDX_Control(pDX, IDC_CMB_NODEID, m_cmbNodeID);
	//led
	DDX_Control(pDX, IDC_LED_X1, m_ledX1);
	DDX_Control(pDX, IDC_LED_X2, m_ledX2);
	DDX_Control(pDX, IDC_LED_X3, m_ledX3);
	//pos
	DDX_Text(pDX, IDC_EDIT_CURRENT_POS, m_sCurrentPosition);
	DDX_Control(pDX, IDC_EDIT_CURRENT_POS, m_editCurrentPosition);
	//jog
	DDX_Control(pDX, IDC_BUTTON_JOG_PLUS, m_btnJogPlus);
	DDX_Control(pDX, IDC_BUTTON_JOG_MINUS, m_btnJogMinus);
	DDX_Text(pDX, IDC_EDIT_JOG_SPEED, m_dJogSpeed);
	DDX_Text(pDX, IDC_EDIT_JOG_ACCEL, m_dJogAccel);
	DDX_Text(pDX, IDC_EDIT_JOG_DECEL, m_dJogDecel);
	//ptp
	DDX_Check(pDX, IDC_CHECK_AUTO_HOME, m_bAutoHomeOnConnect);
	DDX_Text(pDX, IDC_EDIT_PTP_VELOCITY, m_dPtpVelocity);
	DDX_Text(pDX, IDC_EDIT_PTP_ACCEL, m_dPtpAccel);
	DDX_Text(pDX, IDC_EDIT_PTP_DECEL, m_dPtpDecel);
	DDX_Text(pDX, IDC_EDIT_ABS_POS, m_nAbsPosition);
	DDX_Text(pDX, IDC_EDIT_REL_POS, m_nRelPosition);
	DDX_Control(pDX, IDC_COMBO_REL_DIRECTION, m_cmbRelDirection);
	DDX_CBIndex(pDX, IDC_COMBO_REL_DIRECTION, m_nRelDirection);
	DDX_Control(pDX, IDC_EDIT_LOG, m_editLog);
	//��λ����
	DDX_Text(pDX, IDC_EDIT_PHYSICAL_VALUE_PER_REV,m_dPhysicalValuePerRevolution);
    DDX_Text(pDX, IDC_EDIT_PHYSICAL_UNIT_NAME, m_sPhysicalUnitName);
	DDX_Text(pDX, IDC_EDIT_STEPS_PER_REV, m_nStepsPerRevolution);
}

BEGIN_MESSAGE_MAP(CMotorControlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CMotorControlDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CMotorControlDlg::OnBnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_HOME, &CMotorControlDlg::OnBnClickedButtonHome)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CMotorControlDlg::OnBnClickedButtonEmergencyStop)
	ON_BN_CLICKED(IDC_BUTTON_ABS_START, &CMotorControlDlg::OnBnClickedButtonAbsMoveStart)
	ON_BN_CLICKED(IDC_BUTTON_ABS_STOP, &CMotorControlDlg::OnBnClickedButtonAbsMoveStop)
	ON_BN_CLICKED(IDC_BUTTON_REL_START, &CMotorControlDlg::OnBnClickedButtonRelMoveStart)
	ON_BN_CLICKED(IDC_BUTTON_REL_STOP, &CMotorControlDlg::OnBnClickedButtonRelMoveStop)
	ON_MESSAGE(WM_JOG_BUTTON_DOWN, &CMotorControlDlg::OnJogButtonDown)
	ON_MESSAGE(WM_JOG_BUTTON_UP, &CMotorControlDlg::OnJogButtonUp)

END_MESSAGE_MAP()


// CMotorControlDlg ��Ϣ�������

BOOL CMotorControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��"����..."�˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	
	UpdateConnectStatus(false);  //���´��ڱ���Ͱ�ťʹ��״̬
	ScanComPorts();

	// ��ʼ��������������,����Ĭ��ѡ��Ϊ "19200"
	m_cmbBaud.AddString(_T("9600"));
	m_cmbBaud.AddString(_T("19200")); 
	m_cmbBaud.AddString(_T("38400"));
	m_cmbBaud.AddString(_T("57600"));
	m_cmbBaud.AddString(_T("115200"));
	m_cmbBaud.SelectString(-1, _T("19200"));

	//��ʼ��ID������,����Ĭ��ѡ��Ϊ"1"
	CString strNodeID;
	for (int i = 1; i <= 20; ++i)
	{
		strNodeID.Format(_T("%d"), i);
		m_cmbNodeID.AddString(strNodeID);
	}
	m_cmbNodeID.SetCurSel(0);
	
	//����LEDͼ��
	m_hIconLedGray = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_LED_GRAY), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	m_hIconLedGreen = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_LED_GREEN), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	if (m_hIconLedGray == NULL )
	{
		AfxMessageBox("����LEDͼ����Դʧ��!");
		AppendLog("����LEDͼ����Դʧ��!");
	}
	else
	{
		// �ɹ����أ�����ͼ��
		m_ledX1.SetIcon(m_hIconLedGray);
		m_ledX2.SetIcon(m_hIconLedGray);
		m_ledX3.SetIcon(m_hIconLedGray);
	}

	//ptp����ѡ��,Ĭ��ѡ��CW,CWΪindex 0 , CCWΪindex 1
	m_cmbRelDirection.AddString(_T("CW (Clockwise)"));
	m_cmbRelDirection.AddString(_T("CCW (Counter-Clockwise)")); 
	
	m_cmbRelDirection.SetCurSel(0);	

	//ÿ�δ���������־��
	m_editLog.SetWindowText(_T(""));

	UpdateData(FALSE);	// �����г�Ա�����ĳ�ʼֵ���µ�UI��
	
	//��ʼ��λ����ʾ(δ����ʱ)
	m_sCurrentPosition.Format(_T("0.000 %s"), m_sPhysicalUnitName);
	m_editCurrentPosition.SetWindowText(m_sCurrentPosition);
	return TRUE;
}

void CMotorControlDlg::UpdateConnectStatus(bool isConnect, bool isHoming)
{
	// ���´��ڱ���
	CString title;
	if (!isConnect)
	{
		title = _T("Motor Control Demo - Unconnected");
	}
	else if (isHoming)
	{
		title = _T("Motor Control - Homing..."); // ���ڻ���ʱ����ʾ "Homing..."
	}
	else
	{
		title.Format(_T("Motor Control Demo - Connected (COM%d)"), m_nCOMPort); // ��ͨ����ʱ����ʾ "Connected..."
	}
	SetWindowText(title); // ͳһ���������ñ���

	
	// ���³�������/�Ͽ���ť״̬
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(!isConnect);
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(isConnect && !isHoming); // ���ڻ���ʱ������Ͽ�

    // �����Ƿ����Ӻ��Ƿ����ڻ��������������˶���ť��״̬
	BOOL bEnableMotionButtons = isConnect && !isHoming;

	GetDlgItem(IDC_BUTTON_HOME)->EnableWindow(bEnableMotionButtons);
	GetDlgItem(IDC_EDIT_JOG_SPEED)->EnableWindow(bEnableMotionButtons);
	GetDlgItem(IDC_EDIT_JOG_ACCEL)->EnableWindow(bEnableMotionButtons);
	GetDlgItem(IDC_EDIT_JOG_DECEL)->EnableWindow(bEnableMotionButtons);
	GetDlgItem(IDC_BUTTON_JOG_PLUS)->EnableWindow(bEnableMotionButtons);
	GetDlgItem(IDC_BUTTON_JOG_MINUS)->EnableWindow(bEnableMotionButtons);

	GetDlgItem(IDC_EDIT_PTP_VELOCITY)->EnableWindow(bEnableMotionButtons);
	GetDlgItem(IDC_EDIT_PTP_ACCEL)->EnableWindow(bEnableMotionButtons);
	GetDlgItem(IDC_EDIT_PTP_DECEL)->EnableWindow(bEnableMotionButtons);

	GetDlgItem(IDC_EDIT_ABS_POS)->EnableWindow(bEnableMotionButtons);
	GetDlgItem(IDC_BUTTON_ABS_START)->EnableWindow(bEnableMotionButtons);
	GetDlgItem(IDC_BUTTON_ABS_STOP)->EnableWindow(bEnableMotionButtons);

	GetDlgItem(IDC_EDIT_REL_POS)->EnableWindow(bEnableMotionButtons);
	GetDlgItem(IDC_COMBO_REL_DIRECTION)->EnableWindow(bEnableMotionButtons);
	GetDlgItem(IDC_BUTTON_REL_START)->EnableWindow(bEnableMotionButtons);
	GetDlgItem(IDC_BUTTON_REL_STOP)->EnableWindow(bEnableMotionButtons);


	// ����COM�ں�����ѡ���״̬
	BOOL bEnableConfig = !isConnect;
	m_cmbComPort.EnableWindow(bEnableConfig);
	m_cmbBaud.EnableWindow(bEnableConfig);
	m_cmbNodeID.EnableWindow(bEnableConfig);
	GetDlgItem(IDC_CHECK_AUTO_HOME)->EnableWindow(bEnableConfig); // ����ǰ�ſ�����

	//�����˶�������״̬
    GetDlgItem(IDC_EDIT_PHYSICAL_VALUE_PER_REV)->EnableWindow(bEnableConfig);
    GetDlgItem(IDC_EDIT_PHYSICAL_UNIT_NAME)->EnableWindow(bEnableConfig);
    GetDlgItem(IDC_EDIT_STEPS_PER_REV)->EnableWindow(bEnableConfig);
}

void CMotorControlDlg::ScanComPorts()
{
	HANDLE hCom;
	CString strPort;

	// �����Ͽ�
	m_cmbComPort.ResetContent();

	// ɨ��COM1��COM20
	for (int i = 1; i <= 20; i++)
	{
		strPort.Format(_T("\\\\.\\COM%d"), i);
		hCom = CreateFile(strPort, 0, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (INVALID_HANDLE_VALUE != hCom)
		{
			CloseHandle(hCom);
			strPort.Format(_T("COM%d"), i);
			m_cmbComPort.AddString(strPort);
		}
	}

	// ����п��ö˿ڣ�Ĭ�ϲ�ѡ���κ���
	if (m_cmbComPort.GetCount() > 0)
	{
		m_cmbComPort.SetCurSel(-1); //Ĭ�ϲ�ѡ�κ���
	}
	else
	{
		// ���û�п��ö˿ڣ���Ȼ��ʾ��ʾ��Ϣ��ѡ����
		m_cmbComPort.AddString(_T("�޿��ö˿�"));
		m_cmbComPort.SetCurSel(0);
	}
}

void CMotorControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ��������Ƹ�ͼ�ꡣ
// ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó����⽫�ɿ���Զ���ɡ�

void CMotorControlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CMotorControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMotorControlDlg::OnBnClickedButtonConnect()
{
	UpdateData(TRUE);// ��ȡ��ѡ�������״̬�� m_bAutoHomeOnConnect

	// ��ȡѡ���COM�˿�
	CString strPort;
	int nSel = m_cmbComPort.GetCurSel();
	if (nSel == CB_ERR)
	{
		AfxMessageBox(_T("��ѡ��COM�˿�"));
		return;
	}

	m_cmbComPort.GetLBText(nSel, strPort);

	// ����Ƿ�Ϊ��Ч�˿�
	if (strPort == _T("�޿��ö˿�"))
	{
		AfxMessageBox(_T("û�п��õ�COM�˿�"));
		AppendLog("����ʧ��--�޿���COM�˿�");
		return;
	}

	// ��ȡ�˿ں�
	strPort.Replace(_T("COM"), _T(""));
	m_nCOMPort = _ttoi(strPort);

	// ��ȡ������
	CString strBaudRate;
	int nBaudSel = m_cmbBaud.GetCurSel();
	if (nBaudSel == CB_ERR)
	{
		AfxMessageBox(_T("��ѡ������"));
		return;
	}
	m_cmbBaud.GetLBText(nBaudSel, strBaudRate);
	m_nBaudRate = _ttoi(strBaudRate); 

	//��ȡѡ��Ľڵ�ID
	int nSelNode = m_cmbNodeID.GetCurSel();
	if (nSelNode == CB_ERR) {
		AfxMessageBox(_T("��ѡ��ڵ�ID"));
		return;
	}
	m_nNodeID = static_cast<byte>(nSelNode + 1);

	//���Դ�com�˿�
	BOOL bOpenResult = m_ModbusRTUHelper.Open(m_nCOMPort, m_nBaudRate, TRUE);
	if (bOpenResult)
	{
		
		AppendLog(_T("COM�˿ڴ򿪳ɹ���������֤���ͨ��..."));
		UINT statusCode = 0;
		if (!m_ModbusRTUHelper.ReadStatusCode(m_nNodeID, &statusCode))
		{
			// ͨ����֤ʧ��
			m_ModbusRTUHelper.Close(); // �ر�COM�˿�
			UpdateConnectStatus(false, false);
			AfxMessageBox(_T("ͨ����֤ʧ�ܣ�����ڵ�ID�������ʻ������ӡ�"));
			AppendLog(_T("���ͨ����֤ʧ�ܣ������жϡ�"));
			return; // �˳��������к�������
		}
		
		CString str;
		str.Format(_T("���ͨ����֤�ɹ�! Node ID: %d, ״̬��: 0x%04X"), m_nNodeID, statusCode);
		AppendLog(str);

		//������ʱ��
		SetTimer(ID_TIMER_UPDATE_STATUS, TIMER_INTERVAL, NULL);
		if (m_bAutoHomeOnConnect)
		{
			StartHomingProcess();
		}
		else
		{
			UpdateConnectStatus(true, false);
		}
	}
	else
	{
		//����ʧ��
		UpdateConnectStatus(false, false);
		AfxMessageBox(_T("����ʧ�ܣ�����˿��Ƿ�ռ��"));
		AppendLog(_T("COM����ʧ�ܣ�����˿��Ƿ�ռ�á�"));
	}
	//TestMotorCommunication();
}

void CMotorControlDlg::OnBnClickedButtonDisconnect()
{

	KillTimer(ID_TIMER_UPDATE_STATUS); //��ֹͣ��ʱ��
	m_ModbusRTUHelper.Close();	// �Ͽ�����
	AppendLog(_T("COM���ӶϿ���"));
	UpdateConnectStatus(false);
	m_ledX1.SetIcon(m_hIconLedGray);
	m_ledX2.SetIcon(m_hIconLedGray);
	m_ledX3.SetIcon(m_hIconLedGray);
}

void CMotorControlDlg::OnBnClickedButtonHome()
{
	int zeroPosition = 0;
	m_ModbusRTUHelper.FeedtoPosition(m_nNodeID, &zeroPosition);
}


void CMotorControlDlg::OnBnClickedButtonEmergencyStop()  //��ͣ
{
	if (!m_ModbusRTUHelper.IsOpen())
	{
		AfxMessageBox(_T("���δ���ӡ�"));
		return;
	}
	BOOL bResult = m_ModbusRTUHelper.StopAndKill(m_nNodeID);
	if (bResult)
	{
		AppendLog(_T("ִ�м�ͣ!������������ٶ�ֹͣ!"));
		// ����ɹ����ͼ�ָͣ����û���״̬����Ϊ������̻ᱻ�ж�
		if (m_bIsHoming || m_bIsEscapingHomeSensor)
		{
			m_bIsHoming = false;
			m_bIsEscapingHomeSensor = false;
			//״̬�ָ������Ӿ���
			UpdateConnectStatus(true, false);
		}
	}
	else
	{
		AppendLog(_T("��ͣʧ��!"));
	}
}


void CMotorControlDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ID_TIMER_UPDATE_STATUS) {
		if (!m_ModbusRTUHelper.IsOpen()) {
			KillTimer(ID_TIMER_UPDATE_STATUS);
			return;
		}

		// 1.��鲢�������/����״̬
		CheckHomingStatus();

		// 2.���´�����LED״̬
		UpdateSensorStatus();

		// 3.����λ����ʾ
		int nPositionSteps = 0;
		if (m_ModbusRTUHelper.ReadImmediateAbsolutePosition(m_nNodeID, &nPositionSteps)) {
			CString strPos;

			if (IsKinematicParamsValid()) {
			
				double dCurrentPosition_units = ConvertStepsToUnits(static_cast<long>(nPositionSteps));
				strPos.Format(_T("%.3f %s"), dCurrentPosition_units, m_sPhysicalUnitName);
			}
			else {
				strPos.Format(_T("%d steps (Config Invalid)"), nPositionSteps);
			}

			m_editCurrentPosition.SetWindowText(strPos);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}
void CMotorControlDlg::UpdateSensorStatus()
{
	if (!m_ModbusRTUHelper.IsOpen())
	{
		return; 
	}

	USHORT inputStatus = 0;
	BOOL bResult = m_ModbusRTUHelper.ReadDriverBoardInputs(m_nNodeID, &inputStatus);

	if (bResult)
	{
		// ��ȡ�ɹ�������״̬
		bool isX1_On = (inputStatus & (1 << 0)) != 0; // ��� bit 0 (X1)
		bool isX2_On = (inputStatus & (1 << 1)) != 0; // ��� bit 1 (X2)
		bool isX3_On = (inputStatus & (1 << 2)) != 0; // ��� bit 2 (X3)

		// �����ĵ�������ONʱ����ӦλΪ0�������߼���Ҫ��ת��

		m_ledX1.SetIcon(isX1_On ? m_hIconLedGray : m_hIconLedGreen);
		m_ledX2.SetIcon(isX2_On ? m_hIconLedGray : m_hIconLedGreen);
		m_ledX3.SetIcon(isX3_On ? m_hIconLedGray : m_hIconLedGreen);
	}
}



afx_msg LRESULT CMotorControlDlg::OnJogButtonDown(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	if (!m_ModbusRTUHelper.IsOpen())
	{
		return 0;
	}

	if (!IsKinematicParamsValid()) {
		AfxMessageBox(_T("�˶�������Ч,����:ÿת����,ÿת��λ��,��λ����"));
		AppendLog(_T("�˶�������Ч,����:ÿת����,ÿת��λ��,��λ����"));
		return 0;
	}

	// ��ȡ���ĸ���ť��������
	UINT nBtnID = (UINT)wParam;

	double dSpeed_units_per_sec = m_dJogSpeed;
	double dAccel_units_per_sec2 = m_dJogAccel;
	double dDecel_units_per_sec2 = m_dJogDecel;

	// ת��Ϊ��������Ҫ�� "ת/��" �� "ת/��^2" (rps, rps/s^2)
	double dSpeed_rps = ConvertUnitsPerSecToRps(dSpeed_units_per_sec);
	double dAccel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(dAccel_units_per_sec2);
	double dDecel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(dDecel_units_per_sec2);

	if (nBtnID == IDC_BUTTON_JOG_MINUS)
	{
		// �����JOG-��ť�����ٶ���Ϊ��ֵ
		dSpeed_rps = -dSpeed_rps;
	}

	// ���ݹٷ�ʾ����JOGǰ����ȵ��ô˺��� (����δ֪��������)
	m_ModbusRTUHelper.WriteDistanceOrPosition(m_nNodeID, 1);

	// ����JOG����
	BOOL ret = m_ModbusRTUHelper.SetJogProfile(m_nNodeID, &dSpeed_rps, &dAccel_rps_s2, &dDecel_rps_s2);
	if (!ret)
	{
		AfxMessageBox(_T("����JOG����ʧ��(�ٶ�)��"));
		AppendLog(_T("����JOG����ʧ�ܡ�(�ٶ�)"));
		return 0;
	}

	CString strLog;
	if (nBtnID == IDC_BUTTON_JOG_MINUS)
	{
		strLog.Format(_T("JOG- �������ٶ�: %.2f %s/s"), dSpeed_units_per_sec, m_sPhysicalUnitName);
	}
	else
	{
		strLog.Format(_T("JOG+ �������ٶ�: %.2f %s/s"), dSpeed_units_per_sec, m_sPhysicalUnitName);
	}

	// ����JOG�˶�
	m_ModbusRTUHelper.StartJogging(m_nNodeID);
	AppendLog(strLog);
}

afx_msg LRESULT CMotorControlDlg::OnJogButtonUp(WPARAM wParam, LPARAM lParam)
{
	if (!m_ModbusRTUHelper.IsOpen())
	{
		return 0;
	}

	// ֹͣJOG�˶��������ĸ���ţ̌��ִ�д˲���
	m_ModbusRTUHelper.StopJogging(m_nNodeID);

	return 0;
}
void CMotorControlDlg::CheckHomingStatus()
{
	if (!m_bIsHoming && !m_bIsEscapingHomeSensor)
	{
		return;
	}

	UINT statusCode = 0;
	if (!m_ModbusRTUHelper.ReadStatusCode(m_nNodeID, &statusCode))
	{
		return;
	}

	if (m_bIsEscapingHomeSensor)
	{
		// --- ��ǰ���ڡ��������롱״̬ ---
		// ��顰�˶��С�״̬λ (��4λ)�����Ϊ0����ʾ��һ����40000�������ꡣ
		if ((statusCode & (1 << 4)) == 0)
		{
			// ��Ȼ�˶�ֹͣ�ˣ����ٴμ�鴫����״̬
			USHORT inputStatus = 0;
			m_ModbusRTUHelper.ReadDriverBoardInputs(m_nNodeID, &inputStatus);

			if ((inputStatus & (1 << 2)) == 0)
			{
				// **��������Ȼ������**��˵�����뻹δ��ɣ���Ҫ����40000��
				ExecuteOneEscapeStep(); 
			}
			else
			{
				// **�������Ѿ����룡** ����׶γɹ���ɡ�
				m_bIsEscapingHomeSensor = false;
				AppendLog(_T("���ڻ���..."));
				BOOL bResult = m_ModbusRTUHelper.SeekHome(m_nNodeID, 3, 'L');
				if (bResult)
				{
					m_bIsHoming = true; // ���롰���ڻ��㡱
				}
				else
				{
					AfxMessageBox(_T("����ָ���ʧ�ܣ�"));
					AppendLog(_T("����ָ���ʧ�ܣ�"));
					UpdateConnectStatus(true, false);
				}
			}
		}
		// �����������˶��У���ʲô���������ȴ���һ��Timer���
	}
	else if (m_bIsHoming)
	{
		// --- ��ǰ���ڡ����ڻ��㡱״̬ ---
		// ��顰��ԭ���С���־λ (��10λ)������Ϊ0ʱ����ʾ��������ɡ�
		if ((statusCode & (1 << 10)) == 0)
		{
			m_bIsHoming = false;
			m_ModbusRTUHelper.SetPosition(m_nNodeID, 0);
			AppendLog(_T("������ɣ��豸�Ѿ�����"));
			UpdateConnectStatus(true, false);
		}
	}
}

void CMotorControlDlg::ExecuteOneEscapeStep()
{
	UpdateData(TRUE);
	if (!IsKinematicParamsValid()) {
		AfxMessageBox(_T("�˶�������Ч,����:ÿת����,ÿת��λ��,��λ����"));
		AppendLog(_T("�˶�������Ч,����:ÿת����,ÿת��λ��,��λ����"));
		return;
	}
	double dPtpVelocity_rps = ConvertUnitsPerSecToRps(m_dPtpVelocity);
	double dPtpAccel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(m_dPtpAccel);
	double dPtpDecel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(m_dPtpDecel);

	// �ڷ����κ��˶�ǰ������PTP��������������
	m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &dPtpVelocity_rps, &dPtpAccel_rps_s2, &dPtpDecel_rps_s2);

	double escape_dist_units = 2.0;
	long escape_dist_steps_long = ConvertUnitsToSteps(escape_dist_units);
	//escape_dist_steps_long = -escape_dist_steps_long;
	//��ǰ����������
	if (escape_dist_steps_long > INT_MAX || escape_dist_steps_long < INT_MIN) {
		CString str;
		AfxMessageBox(_T("���봫����ʧ�ܣ�����Ĳ��� %lld ����INT_MAX/MIN��Χ��"), escape_dist_steps_long);
		str.Format(_T("���봫����ʧ�ܣ�����Ĳ��� %lld ����INT_MAX/MIN��Χ��"), escape_dist_steps_long);
		AppendLog(str);
		return;
	}
	int escape_dist_steps = static_cast<int>(escape_dist_steps_long);
	m_ModbusRTUHelper.FeedtoLength(m_nNodeID, &escape_dist_steps);
}

void CMotorControlDlg::StartHomingProcess()
{
	//��Ļ��㺯��
	if (m_bIsHoming || m_bIsEscapingHomeSensor) // ��ֹ�ظ�����
	{
		return;
	}
	UpdateData(TRUE);

	if (!IsKinematicParamsValid()) {
		AfxMessageBox(_T("�˶�������Ч,����:ÿת����,ÿת��λ��,��λ����"));
		AppendLog(_T("�˶�������Ч,����:ÿת����,ÿת��λ��,��λ����"));
		return;
	}

	// ��UI PTP�ٶ�/���ٶ� (����λ/s) ת��Ϊ��������Ҫ�� (ת/s)
	double dPtpVelocity_rps = ConvertUnitsPerSecToRps(m_dPtpVelocity);
	double dPtpAccel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(m_dPtpAccel);
	double dPtpDecel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(m_dPtpDecel);

	// �ڷ����κλ��㶯��ǰ������PTP��������������
	BOOL bSetProfile = m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &dPtpVelocity_rps, &dPtpAccel_rps_s2, &dPtpDecel_rps_s2);
	if (!bSetProfile)
	{
		AfxMessageBox(_T("����ʧ�ܣ�����ǰ����PTP����ʧ�ܡ�"));
		AppendLog(_T("����ʧ�ܣ�����ǰ����PTP����ʧ�ܡ�"));
		return;
	}

	// ���X3�������ĵ�ǰ״̬
	USHORT inputStatus = 0;
	if (!m_ModbusRTUHelper.ReadDriverBoardInputs(m_nNodeID, &inputStatus))
	{
		AfxMessageBox(_T("��ȡ������״̬ʧ�ܣ��޷��������㣡"));
		AppendLog(_T("��ȡ������״̬ʧ�ܣ��޷��������㣡"));
		return;
	}

	// X3��Ӧbit 2������(��ɫ)��ʾON����ʱ inputStatus �� bit 2 Ϊ 0��
	if ((inputStatus & (1 << 2)) == 0)
	{
		// ���һ��ԭ�㴫�����ѱ����������롰�������롱״̬����ִ�е�һ������
		m_bIsEscapingHomeSensor = true;
		UpdateConnectStatus(true, true); // ����UI���롰��������С�״̬
		ExecuteOneEscapeStep();          // ִ�е�һ������,֮���ٶ�ʱ���м������״̬,Ȼ��ִ�л���
	}
	else
	{
		// �������ԭ�㴫����δ��������ֱ��ִ�б�׼����
		BOOL bResult = m_ModbusRTUHelper.SeekHome(m_nNodeID, 3, 'L');
		if (bResult)
		{
			m_bIsHoming = true;
			UpdateConnectStatus(true, true);
		}
		else
		{
			AfxMessageBox(_T("����ָ���ʧ�ܣ�"));
		}
	}
}
void CMotorControlDlg::OnBnClickedButtonAbsMoveStart()
{
	if (!m_ModbusRTUHelper.IsOpen()) return;
	UpdateData(TRUE); 

   
	// ʹ���µ���֤����
	CString errorMsg;
	if (!ValidateMotionParameters(m_nAbsPosition, errorMsg)) {
		AfxMessageBox(errorMsg);
		AppendLog(errorMsg);
		return;
	}

	// ��UI PTP�ٶ�/���ٶ� (����λ/s) ת��Ϊ��������Ҫ�� (ת/s)
	double dPtpVelocity_rps = ConvertUnitsPerSecToRps(m_dPtpVelocity);
	double dPtpAccel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(m_dPtpAccel);
	double dPtpDecel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(m_dPtpDecel);

	// 1. ����P2P�˶�����
	BOOL bResult = m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &dPtpVelocity_rps, &dPtpAccel_rps_s2, &dPtpDecel_rps_s2);
	if (!bResult)
	{
		
		AfxMessageBox(_T("�˶�ǰ����PTP����ʧ��(�ٶ�)"));
		AppendLog(_T("�˶�ǰ����PTP����ʧ��(�ٶ�)"));

		return;
	}

	// 2. ��UI����ľ���λ�� (����λ) ת��Ϊ��������Ҫ�Ĳ��� 
	long nAbsPositionSteps = ConvertUnitsToSteps(m_nAbsPosition);

	// ת��Ϊint��APIҪ��
	int nAbsPositionStepsInt = static_cast<int>(nAbsPositionSteps);

	// 3. ִ�о���λ���˶� (FeedtoPosition)
	bResult = m_ModbusRTUHelper.FeedtoPosition(m_nNodeID, &nAbsPositionStepsInt);
	if (!bResult)
	{
		AfxMessageBox(_T("���������˶�ʧ�ܣ�"));
		AppendLog(_T("���������˶�ʧ�ܡ�"));
	}
	else {
		CString str;
		str.Format(_T("�����˶�������Ŀ��λ��: %.3f %s"), m_nAbsPosition, m_sPhysicalUnitName);
		AppendLog(str);
	}
}

// --- �����˶� Stop ---
void CMotorControlDlg::OnBnClickedButtonAbsMoveStop()
{
	if (!m_ModbusRTUHelper.IsOpen()) return;
	// ʹ�� StopAndKillwithNormalDecel �������������ٶ���ֹͣ���������
	// �����Ҫ����ֹͣ��ʹ�� StopAndKill
	m_ModbusRTUHelper.StopAndKillwithNormalDecel(m_nNodeID);
	AppendLog(_T("�����˶�ֹͣ��"));
}

// --- ����˶� Start ---
void CMotorControlDlg::OnBnClickedButtonRelMoveStart()
{
	if (!m_ModbusRTUHelper.IsOpen()) return;
	UpdateData(TRUE);

	CString errorMsg;
	if (!ValidateMotionParameters(m_nRelPosition, errorMsg)) {
		AfxMessageBox(errorMsg);
		AppendLog(errorMsg);
		return;
	}


	// ��UI PTP�ٶ�/���ٶ� (����λ/s) ת��Ϊ��������Ҫ�� (ת/s)
	double dPtpVelocity_rps = ConvertUnitsPerSecToRps(m_dPtpVelocity);
	double dPtpAccel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(m_dPtpAccel);
	double dPtpDecel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(m_dPtpDecel);

	// 1. ����P2P�˶�����
	BOOL bResult = m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &dPtpVelocity_rps, &dPtpAccel_rps_s2, &dPtpDecel_rps_s2);
	if (!bResult)
	{
		AfxMessageBox(_T("�˶�ǰ����PTP����ʧ��(�ٶ�)"));
		AppendLog(_T("�˶�ǰ����PTP����ʧ��(�ٶ�)"));
		return;
	}

	// 2. ��UI�������Ծ��� (����λ) ת��Ϊ��������Ҫ�Ĳ��� (���ܳ���int��Χ)
	long nRelPositionSteps = ConvertUnitsToSteps(m_nRelPosition);
	// 3. ���ݷ���ȷ��λ��ֵ������
	if (m_nRelDirection == 0) { 
		nRelPositionSteps = -nRelPositionSteps;
	}
	int nRelPositionStepsInt = static_cast<int>(nRelPositionSteps);
	// 4.ִ������˶�
	bResult = m_ModbusRTUHelper.FeedtoLength(m_nNodeID, &nRelPositionStepsInt);

	if (!bResult)
	{
		AfxMessageBox(_T("��������˶�ʧ�ܣ�"));
		AppendLog(_T("��������˶�ʧ�ܡ�"));
	}
	else {
		CString str;
		str.Format(_T("����˶�����������: %.3f %s������: %s"), m_nRelPosition, m_sPhysicalUnitName, m_nRelDirection == 0 ? _T("CW") : _T("CCW"));
		AppendLog(str);
	}
}

// --- ����˶� Stop ---
void CMotorControlDlg::OnBnClickedButtonRelMoveStop()
{
	if (!m_ModbusRTUHelper.IsOpen()) return;
	m_ModbusRTUHelper.StopAndKillwithNormalDecel(m_nNodeID);
	AppendLog(_T("����˶�ֹͣ��"));
}

void CMotorControlDlg::AppendLog(const CString& sMessage)
{
	// 1. ��ȡ��ǰʱ�䲢��ʽ��
	CString sTime = CTime::GetCurrentTime().Format(_T("[%Y-%m-%d %H:%M:%S] "));
	CString sLogEntry = sTime + sMessage + _T("\r\n");

	// 2. ��UI������ʵʱ׷����־
	int nLength = m_editLog.GetWindowTextLength();
	m_editLog.SetSel(nLength, nLength);
	m_editLog.ReplaceSel(sLogEntry);

	// 3. ��鲢������־�ļ�
	RotateLogFiles();

	// 4. ����־д�������ļ�
	try
	{
		CStdioFile file;
		// ʹ���µĸ�����������ȡ����·��
		if (file.Open(GetLogFilePath(_T("MotorControl.log")), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeText))
		{
			file.SeekToEnd(); // �ƶ����ļ�ĩβ
			file.WriteString(sLogEntry);
			file.Close();
		}
	}
	catch (CFileException* pEx)
	{
		pEx->Delete();
	}
}



void CMotorControlDlg::RotateLogFiles()
{
	const ULONGLONG MAX_LOG_SIZE = 500 * 1024 * 1024; // 500 MB

	// ʹ�ø�����������ȡ�ļ�·��
	CString sLogPath = GetLogFilePath(_T("MotorControl.log"));
	CString sLogPath1 = GetLogFilePath(_T("MotorControl.1.log"));
	CString sLogPath2 = GetLogFilePath(_T("MotorControl.2.log"));

	CFileStatus status;
	if (CFile::GetStatus(sLogPath, status))
	{
		if (status.m_size >= MAX_LOG_SIZE)
		{
			try
			{
				CFile::Remove(sLogPath2);      // ɾ�� MotorControl.2.log
				CFile::Rename(sLogPath1, sLogPath2); // �� .1.log ������Ϊ .2.log
				CFile::Rename(sLogPath, sLogPath1);  // �� .log ������Ϊ .1.log
			}
			catch (CFileException* pEx)
			{
				pEx->Delete();
			}
		}
	}
}
CString CMotorControlDlg::GetLogFilePath(const CString& sFileName)
{
	
	TCHAR szExePath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szExePath, MAX_PATH);

	PathRemoveFileSpec(szExePath);

	CString sLogDir = szExePath;
	sLogDir += _T("\\log");

	CreateDirectory(sLogDir, NULL);

	return sLogDir + _T("\\") + sFileName;
}

//�Ե�λ�����ķǿռ�����Чֵ�ж�
bool CMotorControlDlg::IsKinematicParamsValid()
{
	// ����������
	if (m_nStepsPerRevolution <= 0 || m_dPhysicalValuePerRevolution <= 0.0 || m_sPhysicalUnitName.IsEmpty()) {
		return false;
	}

	// �������Ƿ�ᵼ�¼������
	// ���磺���ÿת�������󣬻�ÿת��λֵ��С�����ܵ��»���ϵ������
	double conversion_factor = static_cast<double>(m_nStepsPerRevolution) / m_dPhysicalValuePerRevolution;
	if (conversion_factor > 1e10 || conversion_factor < 1e-10) {
		return false; // ����ϵ��������С�����ܵ��¼�������
	}

	return true;
}

//======================����Ϊ��λ���㺯��==========================

//unit --> rps
double CMotorControlDlg::ConvertUnitsPerSecToRps(double units_per_sec)
{
	if (!IsKinematicParamsValid()) {
		// ͨ���ڵ���ǰӦ��� IsKinematicParamsValid��������Ϊ˫�ر���
		return 0.0;
	}
	// (��λ/s) / (��λ/ת) = ת/s
	return units_per_sec / m_dPhysicalValuePerRevolution;
}

//unit --> rps/s
double CMotorControlDlg::ConvertUnitsPerSecSqToRpsPerSecSq(double units_per_sec_sq)
{
	if (!IsKinematicParamsValid()) {
		return 0.0;
	}
	// (��λ/s^2) / (��λ/ת) = ת/s^2
	return units_per_sec_sq / m_dPhysicalValuePerRevolution;
}

//Unit --> Steps
long CMotorControlDlg::ConvertUnitsToSteps(double units)
{
	if (!IsKinematicParamsValid()) {
		return 0;
	}

	// ʹ��double���м��㣬���־���
	double steps_double = units * (static_cast<double>(m_nStepsPerRevolution) / m_dPhysicalValuePerRevolution);

	// ����Ƿ񳬳�long��Χ
	if (steps_double > LONG_MAX || steps_double < LONG_MIN) {
		// ��¼������־
		CString errorMsg;
		errorMsg.Format(_T("��λ�������: %.3f %s ת��Ϊ����ʱ������Χ"), units, m_sPhysicalUnitName);
		AppendLog(errorMsg);
		return 0;
	}


	return static_cast<long>(steps_double);
}

//Steps --> Unit 
double CMotorControlDlg::ConvertStepsToUnits(long steps) // ����������long��Ϊ int
{
	if (!IsKinematicParamsValid()) {
		return 0.0;
	}

	// ʹ��double���м��㣬���־���
	return static_cast<double>(steps) * (m_dPhysicalValuePerRevolution / static_cast<double>(m_nStepsPerRevolution));
}


bool CMotorControlDlg::IsStepsValueSafe(long steps)
{
	return (steps >= INT_MIN && steps <= INT_MAX);
}

// ��������֤�˶��������ۺϰ�ȫ��
bool CMotorControlDlg::ValidateMotionParameters(double position_units, CString& errorMsg)
{
	if (!IsKinematicParamsValid()) {
		errorMsg = _T("�˶�������Ч������ÿת������ÿת��λ���͵�λ����");
		return false;
	}

	// ������������ֵ�Ƿ����
	if (fabs(position_units) > 1000000.0) { // ���賬��100��λ����Ϊ������
		errorMsg.Format(_T("�����λ��ֵ����%.3f %s�����ܵ��¼������"), position_units, m_sPhysicalUnitName);
		return false;
	}

	// ת��Ϊ��������鷶Χ
	long steps = ConvertUnitsToSteps(position_units);
	if (!IsStepsValueSafe(steps)) {
		errorMsg.Format(_T("����Ĳ��� %ld ����������֧�ֵķ�Χ [%d, %d]"), steps, INT_MIN, INT_MAX);
		return false;
	}

	return true;
}