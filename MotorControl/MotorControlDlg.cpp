// MotorControlDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MotorControl.h"
#include "MotorControlDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const UINT_PTR TIMER_INTERVAL = 200;  //��ʱ��ʱ����

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
	, m_nBaudRate(19200) // ������ɿ�ѡ��
	, m_nNodeID(1)       // ���������ɿ�ѡ�� 
	, m_dJogSpeed(10.0)    // Ĭ���ٶ� 10 rps
	, m_dJogAccel(100.0)   // Ĭ�ϼ��ٶ� 100 rps/s
	, m_dJogDecel(100.0)
	, m_sCurrentPosition(_T("0 steps"))
	, m_bAutoHomeOnConnect(TRUE) //  Ĭ�Ϲ�ѡ���Զ����㡱
	, m_dPtpVelocity(10.0)
	, m_dPtpAccel(100.0)
	, m_dPtpDecel(100.0)
	, m_nAbsPosition(20000)
	, m_nRelPosition(20000)
	, m_nRelDirection(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); //���ز�����Ӧ�ó���ͼ�꣬���Ͻǣ��������е�  
	m_hIconLedGray = NULL; // ��ʼ��Ϊ NULL
	m_hIconLedGreen = NULL;
	m_bIsHoming = false;
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
}

BEGIN_MESSAGE_MAP(CMotorControlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CMotorControlDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CMotorControlDlg::OnBnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_HOME, &CMotorControlDlg::OnBnClickedButtonHome)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CMotorControlDlg::OnBnClickedButtonTest)
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

	// ��ʼ������״̬Ϊδ����
	UpdateConnectStatus(false);

	// ɨ��COM�˿�
	ScanComPorts();


	// ��ʼ��������������
	m_cmbBaud.AddString(_T("9600"));
	m_cmbBaud.AddString(_T("19200")); 
	m_cmbBaud.AddString(_T("38400"));
	m_cmbBaud.AddString(_T("57600"));
	m_cmbBaud.AddString(_T("115200"));

	// ����Ĭ��ѡ��Ϊ "19200"
	// SelectString ���Զ����Ҳ�ѡ�а������ı�����
	m_cmbBaud.SelectString(-1, _T("19200"));

	CString strNodeID;
	for (int i = 1; i <= 20; ++i)
	{
		strNodeID.Format(_T("%d"), i);
		m_cmbNodeID.AddString(strNodeID);
	}
	// ����Ĭ��ѡ��Ϊ "1"
	m_cmbNodeID.SetCurSel(0);

	m_hIconLedGray = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_LED_GRAY), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	m_hIconLedGreen = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_LED_GREEN), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	if (m_hIconLedGray == NULL )
	{
		AfxMessageBox(_T("���� LED ͼ����Դʧ�ܣ�"));
	}
	else
	{
		// �ɹ����أ�����ͼ��
		m_ledX1.SetIcon(m_hIconLedGray);
		m_ledX2.SetIcon(m_hIconLedGray);
		m_ledX3.SetIcon(m_hIconLedGray);
	}

	//ptp����ѡ��
	m_cmbRelDirection.AddString(_T("CW (Clockwise)"));    // Index 0
	m_cmbRelDirection.AddString(_T("CCW (Counter-Clockwise)")); // Index 1
	m_cmbRelDirection.SetCurSel(0); // Ĭ��ѡ��CW

	UpdateData(FALSE); // �����г�Ա�����ĳ�ʼֵ���µ�UI��
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
		m_cmbComPort.SetCurSel(-1); // <--- �޸������ 0 ��Ϊ -1
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
		return;
	}

	// ��ȡ�˿ں�
	strPort.Replace(_T("COM"), _T(""));
	m_nCOMPort = _ttoi(strPort);

	// ��������

	CString strBaudRate;
	int nBaudSel = m_cmbBaud.GetCurSel();
	if (nBaudSel == CB_ERR)
	{
		AfxMessageBox(_T("��ѡ������"));
		return;
	}
	m_cmbBaud.GetLBText(nBaudSel, strBaudRate);
	m_nBaudRate = _ttoi(strBaudRate); // ���ַ���ת��Ϊ�����������³�Ա����

	BOOL bResult = m_ModbusRTUHelper.Open(m_nCOMPort, m_nBaudRate, TRUE);
	

	if (bResult)
	{
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
		// ����ʧ��
		UpdateConnectStatus(false, false);
		AfxMessageBox(_T("����ʧ�ܣ�����˿��Ƿ�ռ��"));
	}

	//TestMotorCommunication();

}

void CMotorControlDlg::OnBnClickedButtonDisconnect()
{

	KillTimer(ID_TIMER_UPDATE_STATUS); //��ֹͣ��ʱ��


	m_ModbusRTUHelper.Close();	// �Ͽ�����

	UpdateConnectStatus(false);
	m_ledX1.SetIcon(m_hIconLedGray);
	m_ledX2.SetIcon(m_hIconLedGray);
	m_ledX3.SetIcon(m_hIconLedGray);
}

void CMotorControlDlg::TestMotorCommunication()
{
	try
	{
		// 1. ���ȶ�ȡ���״̬����֤ͨ��
		UINT statusCode = 0;
		BOOL ret = m_ModbusRTUHelper.ReadStatusCode(m_nNodeID, &statusCode);
		if (!ret)
		{
			AfxMessageBox(_T("��ȡ���״̬ʧ�ܣ����������Ӻͽڵ�ID"));
			return;
		}

		CString statusMsg;
		statusMsg.Format(_T("���״̬��: 0x%04X"), statusCode);
		//AfxMessageBox(statusMsg);

		// 2. ��ȡ��������
		UINT alarmCode = 0;
		ret = m_ModbusRTUHelper.ReadAlarmCode(m_nNodeID, &alarmCode);
		if (ret)
		{
			if (alarmCode != 0)
			{
				CString alarmMsg;
				alarmMsg.Format(_T("����б���: 0x%04X�������Ը�λ"), alarmCode);
				AfxMessageBox(alarmMsg);

				// ��λ����
				m_ModbusRTUHelper.AlarmReset(m_nNodeID);
				Sleep(100); // �ȴ���λ���
			}
		}

		// 3. ʹ�ܵ��
		ret = m_ModbusRTUHelper.DriveEnable(m_nNodeID, TRUE);
		if (!ret)
		{
			AfxMessageBox(_T("���ʹ��ʧ�ܣ�"));
			return;
		}
	//	AfxMessageBox(_T("���ʹ�ܳɹ���"));
		Sleep(500); // �ȴ�ʹ�����

					// 4. �����˶�������ִ��С������˶�
		double velocity = 10.0;    // 10 rps ���ٶ�
		double accel = 100.0;      // 100 rps? �ļ��ٶ�
		double decel = 100.0;      // 100 rps? �ļ��ٶ�

								   // ����P2P�˶�����
		ret = m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &velocity, &accel, &decel);
		if (!ret)
		{
			AfxMessageBox(_T("�����˶�����ʧ�ܣ�"));
			return;
		}
	    
		//AfxMessageBox(_T("�����ʼ�˶�20000��..."));

		// 5. ִ������˶� (20000��)
		int relativeSteps = 20000;
		ret = m_ModbusRTUHelper.RelMove(m_nNodeID, relativeSteps, &velocity, &accel, &decel);
		if (!ret)
		{
			AfxMessageBox(_T("��������˶�ʧ�ܣ�"));
			return;
		}

		// 6. �ȴ��˶���ɣ���ѡ��
		Sleep(2000); // �ȴ�2��

		// 7. ��ȡ��ǰλ��
		int currentPosition = 0;
	//	ret = m_ModbusRTUHelper.ReadImmediateAbsolutePosition(m_nNodeID, &currentPosition);
		ret = m_ModbusRTUHelper.ReadEncoderPosition(m_nNodeID, &currentPosition);
		if (ret)
		{
			CString posMsg;
		//	posMsg.Format(_T("��ǰλ��: %d ��"), currentPosition);
			AfxMessageBox(posMsg);
		}

	//	AfxMessageBox(_T("���������ɣ�ͨ��������"));
	}
	catch (...)
	{
		AfxMessageBox(_T("������Թ����з����쳣��"));
	}
}

void CMotorControlDlg::OnBnClickedButtonHome()
{
	StartHomingProcess();
}


void CMotorControlDlg::OnBnClickedButtonTest()
{
	//// TODO: �ڴ���ӿؼ�֪ͨ����������
	//if (!m_ModbusRTUHelper.IsOpen())
	//{
	//	AfxMessageBox(_T("�������ӵ����"));
	//	return;
	//}

	//USHORT inputStatus = 0;
	//BOOL bResult = m_ModbusRTUHelper.ReadDriverBoardInputs( m_nNodeID, &inputStatus);

	//if (bResult)
	//{
	//	// ��ȡ�ɹ������ڽ���״̬
	//	// ʹ��λ��(&)������������ض�λ�Ƿ�Ϊ1
	//	// (1 << 0) ���� 0x0001, (1 << 1) ���� 0x0002, (1 << 2) ���� 0x0004

	//	bool isX1_On = (inputStatus & (1 << 0)) != 0; // ��� bit 0 (X1)
	//	bool isX2_On = (inputStatus & (1 << 1)) != 0; // ��� bit 1 (X2)
	//	bool isX3_On = (inputStatus & (1 << 2)) != 0; // ��� bit 2 (X3)

	//	CString msg;
	//	msg.Format(_T("����˿�״̬:\n")
	//		_T("X1: %s\n")
	//		_T("X2: %s\n")
	//		_T("X3: %s"),
	//		isX1_On ? _T("ON (��ͨ/�͵�ƽ)") : _T("OFF (�Ͽ�/�ߵ�ƽ)"),
	//		isX2_On ? _T("ON (��ͨ/�͵�ƽ)") : _T("OFF (�Ͽ�/�ߵ�ƽ)"),
	//		isX3_On ? _T("ON (��ͨ/�͵�ƽ)") : _T("OFF (�Ͽ�/�ߵ�ƽ)"));

	//	AfxMessageBox(msg);

	//	m_ledX1.SetIcon(isX1_On ? m_hIconLedGray : m_hIconLedGreen);
	//	m_ledX2.SetIcon(isX2_On ? m_hIconLedGray : m_hIconLedGreen);
	//	m_ledX3.SetIcon(isX3_On ? m_hIconLedGray : m_hIconLedGreen);

	//}
	//else
	//{
	//	AfxMessageBox(_T("��ȡ����״̬ʧ�ܣ�"));
	//}
	//TestMotorCommunication();
	//m_ModbusRTUHelper.SetPosition(m_nNodeID, 0);
	//UpdateData(TRUE); // ��UI��ȡ���µĲ���ֵ

					  // 1. ����P2P�˶�����
	//BOOL bResult = m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &m_dPtpVelocity, &m_dPtpAccel, &m_dPtpDecel);

	if (!m_ModbusRTUHelper.IsOpen())
	{
		AfxMessageBox(_T("���δ���ӡ�"));
		return;
	}

	//AfxMessageBox(_T("ִ�м�ͣ��������������ٶ�ֹͣ�����ָ����С�"));
	BOOL bResult = m_ModbusRTUHelper.StopAndKill(m_nNodeID);

	if (bResult)
	{
		// ����ɹ����ͼ�ָͣ�Ӧ�����û���״̬����Ϊ������̻ᱻ�ж�
		if (m_bIsHoming || m_bIsEscapingHomeSensor)
		{
			m_bIsHoming = false;
			m_bIsEscapingHomeSensor = false;
			// ��ͣ��ǿ��ֹͣ������״̬Ӧ�ûָ������Ӿ���
			UpdateConnectStatus(true, false);
		}
	//	AfxMessageBox(_T("��ָͣ���ѷ��͡�"));
	}
	else
	{
	//	AfxMessageBox(_T("��ָͣ���ʧ�ܣ�"));
	}
}
void CMotorControlDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ID_TIMER_UPDATE_STATUS)
	{
		if (!m_ModbusRTUHelper.IsOpen())
		{
			KillTimer(ID_TIMER_UPDATE_STATUS);
			return;
		}

		// ��鲢�������/����״̬
		CheckHomingStatus();

		// ���´�����LED״̬
		UpdateSensorStatus();

		// ����λ����ʾ
		int nPositionSteps = 0;
		if (m_ModbusRTUHelper.ReadImmediateAbsolutePosition(m_nNodeID, &nPositionSteps))
		{
			CString strPos;
			strPos.Format(_T("%d steps"), nPositionSteps);
			m_editCurrentPosition.SetWindowText(strPos);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}
void CMotorControlDlg::UpdateSensorStatus()
{
	if (!m_ModbusRTUHelper.IsOpen())
	{
		return; // ���δ���ӣ���ִ���κβ���
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
	// ��UI��ȡ���µĲ���
	UpdateData(TRUE);

	// ����Ƿ�������
	if (!m_ModbusRTUHelper.IsOpen())
	{
		return 0;
	}

	// ��ȡ���ĸ���ť��������
	UINT nBtnID = (UINT)wParam;

	double dSpeed = m_dJogSpeed;
	double dAccel = m_dJogAccel;
	// ͨ��JOG�ļ��ٶȺͼ��ٶȿ�����Ϊһ��
	 double dDecel = m_dJogDecel;

	if (nBtnID == IDC_BUTTON_JOG_MINUS)
	{
		// �����JOG-��ť�����ٶ���Ϊ��ֵ
		dSpeed = -m_dJogSpeed;
	}

	// ���ݹٷ�ʾ����JOGǰ����ȵ��ô˺���
	m_ModbusRTUHelper.WriteDistanceOrPosition(m_nNodeID, 1);

	// ����JOG����
	BOOL ret = m_ModbusRTUHelper.SetJogProfile(m_nNodeID, &dSpeed, &dAccel, &dDecel);
	if (!ret)
	{
		AfxMessageBox(_T("����JOG����ʧ�ܣ�"));
		return 0;
	}

	// ����JOG
	m_ModbusRTUHelper.StartJogging(m_nNodeID);

	return 0;
}

afx_msg LRESULT CMotorControlDlg::OnJogButtonUp(WPARAM wParam, LPARAM lParam)
{
	// ����Ƿ�������
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
		// ��顰�˶��С�״̬λ (��4λ)�����Ϊ0����ʾ��һ����-3000�������ꡣ
		if ((statusCode & (1 << 4)) == 0)
		{
			// ��Ȼ�˶�ֹͣ�ˣ����ٴμ�鴫����״̬
			USHORT inputStatus = 0;
			m_ModbusRTUHelper.ReadDriverBoardInputs(m_nNodeID, &inputStatus);

			if ((inputStatus & (1 << 2)) == 0)
			{
				// **��������Ȼ������**��˵�����뻹δ��ɣ���Ҫ����һ��
				ExecuteOneEscapeStep(); // �ٴη���-3000��ָ��
			}
			else
			{
				// **�������Ѿ����룡** ����׶γɹ���ɡ�
				m_bIsEscapingHomeSensor = false;
				//AfxMessageBox(_T("���봫������ɣ���ʼ��ʽ���㡣"));

				// ����������ʽ����
				BOOL bResult = m_ModbusRTUHelper.SeekHome(m_nNodeID, 3, 'L');
				if (bResult)
				{
					m_bIsHoming = true; // ���롰���ڻ��㡱״̬
				}
				else
				{
					AfxMessageBox(_T("����ָ���ʧ�ܣ�"));
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
			//AfxMessageBox(_T("������ɣ��豸�Ѿ�����"));
			UpdateConnectStatus(true, false);
		}
	}
}

void CMotorControlDlg::ExecuteOneEscapeStep()
{
	UpdateData(TRUE);
	int escape_dist = 40000;  // ������� (����������)

	m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &m_dPtpVelocity,&m_dPtpAccel, &m_dPtpDecel);
	
	//m_ModbusRTUHelper.RelMove(m_nNodeID, escape_dist, NULL, NULL, NULL);
	m_ModbusRTUHelper.FeedtoLength(m_nNodeID, &escape_dist);
}

void CMotorControlDlg::StartHomingProcess()
{
	if (m_bIsHoming || m_bIsEscapingHomeSensor) // ��ֹ�ظ�����
	{
		return;
	}
	UpdateData(TRUE);

	//�ڷ����κλ��㶯��ǰ������PTP��������������,��ʵ��һ�������Ƕ���ģ���Ϊ
	//�����ʱ����Ѿ��������ٶ�
	BOOL bSetProfile = m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &m_dPtpVelocity, &m_dPtpAccel, &m_dPtpDecel);
	if (!bSetProfile)
	{
		AfxMessageBox(_T("����PTP����ʧ�ܣ��޷��������㣡"));
		return;
	}


	// ���X3�������ĵ�ǰ״̬
	USHORT inputStatus = 0;
	if (!m_ModbusRTUHelper.ReadDriverBoardInputs(m_nNodeID, &inputStatus))
	{
		AfxMessageBox(_T("��ȡ������״̬ʧ�ܣ��޷��������㣡"));
		return;
	}

	// X3��Ӧbit 2������(��ɫ)��ʾON����ʱ inputStatus �� bit 2 Ϊ 0��
	if ((inputStatus & (1 << 2)) == 0)
	{
		// ���һ��ԭ�㴫�����ѱ����������롰�������롱״̬����ִ�е�һ������
		//AfxMessageBox(_T("ԭ�㴫�����ѱ���������ʼִ��ѭ������..."));
		m_bIsEscapingHomeSensor = true;
		UpdateConnectStatus(true, true); // ����UI���롰��������С�״̬
		ExecuteOneEscapeStep();          // ִ�е�һ������
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
	UpdateData(TRUE); // ��UI��ȡ���µĲ���ֵ

	 // 1. ����P2P�˶�����
	BOOL bResult = m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &m_dPtpVelocity, &m_dPtpAccel, &m_dPtpDecel);
	if (!bResult)
	{
		AfxMessageBox(_T("�����˶�����ʧ�ܣ�"));
		return;
	}

	// 2. ִ�о���λ���˶� (FeedtoPosition)
	// ע�⣺AbsMove ��һ����װ�õĺ��������ڲ�����Ҳ�ǵ��� FeedtoPosition
	// ����ֱ���� FeedtoPosition����Ϊ����������
	// ����� AbsMove������ҪԤ�ȵ��� SetP2PProfile��
	bResult = m_ModbusRTUHelper.FeedtoPosition(m_nNodeID, &m_nAbsPosition);
	if (!bResult)
	{
		AfxMessageBox(_T("���������˶�ʧ�ܣ�"));
	}
}

// --- �����˶� Stop ---
void CMotorControlDlg::OnBnClickedButtonAbsMoveStop()
{
	if (!m_ModbusRTUHelper.IsOpen()) return;
	// ʹ�� StopAndKillwithNormalDecel �������������ٶ���ֹͣ���������
	// �����Ҫ����ֹͣ��ʹ�� StopAndKill
	m_ModbusRTUHelper.StopAndKillwithNormalDecel(m_nNodeID);
}

// --- ����˶� Start ---
void CMotorControlDlg::OnBnClickedButtonRelMoveStart()
{
	if (!m_ModbusRTUHelper.IsOpen()) return;
	UpdateData(TRUE); 

	// 1. ����P2P�˶�����
	BOOL bResult = m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &m_dPtpVelocity, &m_dPtpAccel, &m_dPtpDecel);
	if (!bResult)
	{
		AfxMessageBox(_T("�����˶�����ʧ�ܣ�"));
		return;
	}

	// 2. ���ݷ���ȷ��λ��ֵ������
	int distance = m_nRelPosition;
	if (m_nRelDirection == 1) // 1 ���� CCW (����)
	{
		distance = -m_nRelPosition;
	}

	// 3. ִ�����λ���˶� (FeedtoLength)
	bResult = m_ModbusRTUHelper.FeedtoLength(m_nNodeID, &distance);
	if (!bResult)
	{
		AfxMessageBox(_T("��������˶�ʧ�ܣ�"));
	}
}

// --- ����˶� Stop ---
void CMotorControlDlg::OnBnClickedButtonRelMoveStop()
{
	if (!m_ModbusRTUHelper.IsOpen()) return;
	m_ModbusRTUHelper.StopAndKillwithNormalDecel(m_nNodeID);
}