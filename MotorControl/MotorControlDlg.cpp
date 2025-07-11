// MotorControlDlg.cpp : 实现文件
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


const UINT_PTR TIMER_INTERVAL = 100;  //定时器时间间隔

// 用于应用程序"关于"菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

														// 实现
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


// CMotorControlDlg 对话框



CMotorControlDlg::CMotorControlDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MOTORCONTROL_DIALOG, pParent)
	, m_nBaudRate(19200)				//默认19200
	, m_nNodeID(1)                      //默认ID为1 
	, m_dJogSpeed(10.0)                 //JOG默认速度10unit/s
	, m_dJogAccel(100.0)                //JOG默认加速度100unit/s
	, m_dJogDecel(100.0)                //JOG默认减速度100unit/s
	, m_sCurrentPosition(_T("0 steps")) //位置初始显示0steps
	, m_bAutoHomeOnConnect(TRUE)		//默认勾选“自动回零”
	, m_dPtpVelocity(10.0)              //PTP默认速度10unit/s
	, m_dPtpAccel(100.0)				//PTP默认加速度100unit/s
	, m_dPtpDecel(100.0)				//PTP默认减速度100unit/s
	, m_nAbsPosition(0)					//Position默认都是0
	, m_nRelPosition(0)
	, m_nRelDirection(1)				//默认方向为正
	, m_nStepsPerRevolution(20000)      //默认20000步
	, m_dPhysicalValuePerRevolution(1.0)//默认每转1.0个单位 (如1mm)
	, m_sPhysicalUnitName(_T("mm"))     //默认单位名称为毫米
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); //加载并设置应用程序图标，左上角，任务栏中的  
	m_hIconLedGray = NULL; // 初始化为 NULL
	m_hIconLedGreen = NULL;
	m_bIsHoming = false;
	m_bIsEscapingHomeSensor = false;
}


CMotorControlDlg::~CMotorControlDlg()
{
	// 确保定时器在对话框销毁时被停止
	KillTimer(ID_TIMER_UPDATE_STATUS);

	// 释放图标句柄
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
	//单位换算
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


// CMotorControlDlg 消息处理程序

BOOL CMotorControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将"关于..."菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	
	UpdateConnectStatus(false);  //更新窗口标题和按钮使能状态
	ScanComPorts();

	// 初始化波特率下拉框,设置默认选项为 "19200"
	m_cmbBaud.AddString(_T("9600"));
	m_cmbBaud.AddString(_T("19200")); 
	m_cmbBaud.AddString(_T("38400"));
	m_cmbBaud.AddString(_T("57600"));
	m_cmbBaud.AddString(_T("115200"));
	m_cmbBaud.SelectString(-1, _T("19200"));

	//初始化ID下拉框,设置默认选项为"1"
	CString strNodeID;
	for (int i = 1; i <= 20; ++i)
	{
		strNodeID.Format(_T("%d"), i);
		m_cmbNodeID.AddString(strNodeID);
	}
	m_cmbNodeID.SetCurSel(0);
	
	//加载LED图标
	m_hIconLedGray = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_LED_GRAY), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	m_hIconLedGreen = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_LED_GREEN), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	if (m_hIconLedGray == NULL )
	{
		AfxMessageBox("加载LED图标资源失败!");
		AppendLog("加载LED图标资源失败!");
	}
	else
	{
		// 成功加载，设置图标
		m_ledX1.SetIcon(m_hIconLedGray);
		m_ledX2.SetIcon(m_hIconLedGray);
		m_ledX3.SetIcon(m_hIconLedGray);
	}

	//ptp方向选择,默认选中CW,CW为index 0 , CCW为index 1
	m_cmbRelDirection.AddString(_T("CW (Clockwise)"));
	m_cmbRelDirection.AddString(_T("CCW (Counter-Clockwise)")); 
	
	m_cmbRelDirection.SetCurSel(0);	

	//每次打开软件清空日志框
	m_editLog.SetWindowText(_T(""));

	UpdateData(FALSE);	// 将所有成员变量的初始值更新到UI上
	
	//初始化位置显示(未连接时)
	m_sCurrentPosition.Format(_T("0.000 %s"), m_sPhysicalUnitName);
	m_editCurrentPosition.SetWindowText(m_sCurrentPosition);
	return TRUE;
}

void CMotorControlDlg::UpdateConnectStatus(bool isConnect, bool isHoming)
{
	// 更新窗口标题
	CString title;
	if (!isConnect)
	{
		title = _T("Motor Control Demo - Unconnected");
	}
	else if (isHoming)
	{
		title = _T("Motor Control - Homing..."); // 正在回零时，显示 "Homing..."
	}
	else
	{
		title.Format(_T("Motor Control Demo - Connected (COM%d)"), m_nCOMPort); // 普通连接时，显示 "Connected..."
	}
	SetWindowText(title); // 统一在这里设置标题

	
	// 更新常规连接/断开按钮状态
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(!isConnect);
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(isConnect && !isHoming); // 正在回零时不允许断开

    // 根据是否连接和是否正在回零来决定其他运动按钮的状态
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


	// 更新COM口和配置选择的状态
	BOOL bEnableConfig = !isConnect;
	m_cmbComPort.EnableWindow(bEnableConfig);
	m_cmbBaud.EnableWindow(bEnableConfig);
	m_cmbNodeID.EnableWindow(bEnableConfig);
	GetDlgItem(IDC_CHECK_AUTO_HOME)->EnableWindow(bEnableConfig); // 连接前才可设置

	//三个运动参数的状态
    GetDlgItem(IDC_EDIT_PHYSICAL_VALUE_PER_REV)->EnableWindow(bEnableConfig);
    GetDlgItem(IDC_EDIT_PHYSICAL_UNIT_NAME)->EnableWindow(bEnableConfig);
    GetDlgItem(IDC_EDIT_STEPS_PER_REV)->EnableWindow(bEnableConfig);
}

void CMotorControlDlg::ScanComPorts()
{
	HANDLE hCom;
	CString strPort;

	// 清空组合框
	m_cmbComPort.ResetContent();

	// 扫描COM1到COM20
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

	// 如果有可用端口，默认不选择任何项
	if (m_cmbComPort.GetCount() > 0)
	{
		m_cmbComPort.SetCurSel(-1); //默认不选任何项
	}
	else
	{
		// 如果没有可用端口，依然显示提示信息并选中它
		m_cmbComPort.AddString(_T("无可用端口"));
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

// 如果向对话框添加最小化按钮，则需要下面的代码来绘制该图标。
// 对于使用文档/视图模型的 MFC 应用程序，这将由框架自动完成。

void CMotorControlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CMotorControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMotorControlDlg::OnBnClickedButtonConnect()
{
	UpdateData(TRUE);// 获取复选框的最新状态到 m_bAutoHomeOnConnect

	// 获取选择的COM端口
	CString strPort;
	int nSel = m_cmbComPort.GetCurSel();
	if (nSel == CB_ERR)
	{
		AfxMessageBox(_T("请选择COM端口"));
		return;
	}

	m_cmbComPort.GetLBText(nSel, strPort);

	// 检查是否为无效端口
	if (strPort == _T("无可用端口"))
	{
		AfxMessageBox(_T("没有可用的COM端口"));
		AppendLog("连接失败--无可用COM端口");
		return;
	}

	// 提取端口号
	strPort.Replace(_T("COM"), _T(""));
	m_nCOMPort = _ttoi(strPort);

	// 获取波特率
	CString strBaudRate;
	int nBaudSel = m_cmbBaud.GetCurSel();
	if (nBaudSel == CB_ERR)
	{
		AfxMessageBox(_T("请选择波特率"));
		return;
	}
	m_cmbBaud.GetLBText(nBaudSel, strBaudRate);
	m_nBaudRate = _ttoi(strBaudRate); 

	//获取选择的节点ID
	int nSelNode = m_cmbNodeID.GetCurSel();
	if (nSelNode == CB_ERR) {
		AfxMessageBox(_T("请选择节点ID"));
		return;
	}
	m_nNodeID = static_cast<byte>(nSelNode + 1);

	//尝试打开com端口
	BOOL bOpenResult = m_ModbusRTUHelper.Open(m_nCOMPort, m_nBaudRate, TRUE);
	if (bOpenResult)
	{
		
		AppendLog(_T("COM端口打开成功，尝试验证电机通信..."));
		UINT statusCode = 0;
		if (!m_ModbusRTUHelper.ReadStatusCode(m_nNodeID, &statusCode))
		{
			// 通信验证失败
			m_ModbusRTUHelper.Close(); // 关闭COM端口
			UpdateConnectStatus(false, false);
			AfxMessageBox(_T("通信验证失败！请检查节点ID、波特率或电机连接。"));
			AppendLog(_T("电机通信验证失败，连接中断。"));
			return; // 退出，不进行后续操作
		}
		
		CString str;
		str.Format(_T("电机通信验证成功! Node ID: %d, 状态码: 0x%04X"), m_nNodeID, statusCode);
		AppendLog(str);

		//启动定时器
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
		//连接失败
		UpdateConnectStatus(false, false);
		AfxMessageBox(_T("连接失败，请检查端口是否被占用"));
		AppendLog(_T("COM连接失败，请检查端口是否被占用。"));
	}
	//TestMotorCommunication();
}

void CMotorControlDlg::OnBnClickedButtonDisconnect()
{

	KillTimer(ID_TIMER_UPDATE_STATUS); //先停止定时器
	m_ModbusRTUHelper.Close();	// 断开连接
	AppendLog(_T("COM连接断开。"));
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


void CMotorControlDlg::OnBnClickedButtonEmergencyStop()  //急停
{
	if (!m_ModbusRTUHelper.IsOpen())
	{
		AfxMessageBox(_T("电机未连接。"));
		return;
	}
	BOOL bResult = m_ModbusRTUHelper.StopAndKill(m_nNodeID);
	if (bResult)
	{
		AppendLog(_T("执行急停!电机将以最大减速度停止!"));
		// 如果成功发送急停指令，重置回零状态，因为回零过程会被中断
		if (m_bIsHoming || m_bIsEscapingHomeSensor)
		{
			m_bIsHoming = false;
			m_bIsEscapingHomeSensor = false;
			//状态恢复到连接就绪
			UpdateConnectStatus(true, false);
		}
	}
	else
	{
		AppendLog(_T("急停失败!"));
	}
}


void CMotorControlDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ID_TIMER_UPDATE_STATUS) {
		if (!m_ModbusRTUHelper.IsOpen()) {
			KillTimer(ID_TIMER_UPDATE_STATUS);
			return;
		}

		// 1.检查并处理回零/脱离状态
		CheckHomingStatus();

		// 2.更新传感器LED状态
		UpdateSensorStatus();

		// 3.更新位置显示
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
		// 读取成功，解析状态
		bool isX1_On = (inputStatus & (1 << 0)) != 0; // 检查 bit 0 (X1)
		bool isX2_On = (inputStatus & (1 << 1)) != 0; // 检查 bit 1 (X2)
		bool isX3_On = (inputStatus & (1 << 2)) != 0; // 检查 bit 2 (X3)

		// 根据文档，输入ON时，对应位为0。所以逻辑需要反转。

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
		AfxMessageBox(_T("运动参数无效,请检查:每转步数,每转单位数,单位名称"));
		AppendLog(_T("运动参数无效,请检查:每转步数,每转单位数,单位名称"));
		return 0;
	}

	// 获取是哪个按钮被按下了
	UINT nBtnID = (UINT)wParam;

	double dSpeed_units_per_sec = m_dJogSpeed;
	double dAccel_units_per_sec2 = m_dJogAccel;
	double dDecel_units_per_sec2 = m_dJogDecel;

	// 转换为驱动器需要的 "转/秒" 和 "转/秒^2" (rps, rps/s^2)
	double dSpeed_rps = ConvertUnitsPerSecToRps(dSpeed_units_per_sec);
	double dAccel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(dAccel_units_per_sec2);
	double dDecel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(dDecel_units_per_sec2);

	if (nBtnID == IDC_BUTTON_JOG_MINUS)
	{
		// 如果是JOG-按钮，将速度设为负值
		dSpeed_rps = -dSpeed_rps;
	}

	// 根据官方示例，JOG前最好先调用此函数 (功能未知，但保留)
	m_ModbusRTUHelper.WriteDistanceOrPosition(m_nNodeID, 1);

	// 设置JOG参数
	BOOL ret = m_ModbusRTUHelper.SetJogProfile(m_nNodeID, &dSpeed_rps, &dAccel_rps_s2, &dDecel_rps_s2);
	if (!ret)
	{
		AfxMessageBox(_T("设置JOG参数失败(速度)。"));
		AppendLog(_T("设置JOG参数失败。(速度)"));
		return 0;
	}

	CString strLog;
	if (nBtnID == IDC_BUTTON_JOG_MINUS)
	{
		strLog.Format(_T("JOG- 启动，速度: %.2f %s/s"), dSpeed_units_per_sec, m_sPhysicalUnitName);
	}
	else
	{
		strLog.Format(_T("JOG+ 启动，速度: %.2f %s/s"), dSpeed_units_per_sec, m_sPhysicalUnitName);
	}

	// 启动JOG运动
	m_ModbusRTUHelper.StartJogging(m_nNodeID);
	AppendLog(strLog);
}

afx_msg LRESULT CMotorControlDlg::OnJogButtonUp(WPARAM wParam, LPARAM lParam)
{
	if (!m_ModbusRTUHelper.IsOpen())
	{
		return 0;
	}

	// 停止JOG运动，无论哪个按钮抬起都执行此操作
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
		// --- 当前处于“正在脱离”状态 ---
		// 检查“运动中”状态位 (第4位)。如果为0，表示上一步的40000步已走完。
		if ((statusCode & (1 << 4)) == 0)
		{
			// 既然运动停止了，就再次检查传感器状态
			USHORT inputStatus = 0;
			m_ModbusRTUHelper.ReadDriverBoardInputs(m_nNodeID, &inputStatus);

			if ((inputStatus & (1 << 2)) == 0)
			{
				// **传感器依然被触发**，说明脱离还未完成，需要再走40000步
				ExecuteOneEscapeStep(); 
			}
			else
			{
				// **传感器已经脱离！** 脱离阶段成功完成。
				m_bIsEscapingHomeSensor = false;
				AppendLog(_T("正在回零..."));
				BOOL bResult = m_ModbusRTUHelper.SeekHome(m_nNodeID, 3, 'L');
				if (bResult)
				{
					m_bIsHoming = true; // 进入“正在回零”
				}
				else
				{
					AfxMessageBox(_T("回零指令发送失败！"));
					AppendLog(_T("回零指令发送失败！"));
					UpdateConnectStatus(true, false);
				}
			}
		}
		// 如果电机还在运动中，则什么都不做，等待下一轮Timer检查
	}
	else if (m_bIsHoming)
	{
		// --- 当前处于“正在回零”状态 ---
		// 检查“回原点中”标志位 (第10位)。当它为0时，表示回零已完成。
		if ((statusCode & (1 << 10)) == 0)
		{
			m_bIsHoming = false;
			m_ModbusRTUHelper.SetPosition(m_nNodeID, 0);
			AppendLog(_T("回零完成，设备已就绪！"));
			UpdateConnectStatus(true, false);
		}
	}
}

void CMotorControlDlg::ExecuteOneEscapeStep()
{
	UpdateData(TRUE);
	if (!IsKinematicParamsValid()) {
		AfxMessageBox(_T("运动参数无效,请检查:每转步数,每转单位数,单位名称"));
		AppendLog(_T("运动参数无效,请检查:每转步数,每转单位数,单位名称"));
		return;
	}
	double dPtpVelocity_rps = ConvertUnitsPerSecToRps(m_dPtpVelocity);
	double dPtpAccel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(m_dPtpAccel);
	double dPtpDecel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(m_dPtpDecel);

	// 在发起任何运动前，先用PTP参数设置驱动器
	m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &dPtpVelocity_rps, &dPtpAccel_rps_s2, &dPtpDecel_rps_s2);

	double escape_dist_units = 2.0;
	long escape_dist_steps_long = ConvertUnitsToSteps(escape_dist_units);
	//escape_dist_steps_long = -escape_dist_steps_long;
	//当前回零往右走
	if (escape_dist_steps_long > INT_MAX || escape_dist_steps_long < INT_MIN) {
		CString str;
		AfxMessageBox(_T("脱离传感器失败：计算的步数 %lld 超出INT_MAX/MIN范围。"), escape_dist_steps_long);
		str.Format(_T("脱离传感器失败：计算的步数 %lld 超出INT_MAX/MIN范围。"), escape_dist_steps_long);
		AppendLog(str);
		return;
	}
	int escape_dist_steps = static_cast<int>(escape_dist_steps_long);
	m_ModbusRTUHelper.FeedtoLength(m_nNodeID, &escape_dist_steps);
}

void CMotorControlDlg::StartHomingProcess()
{
	//真的回零函数
	if (m_bIsHoming || m_bIsEscapingHomeSensor) // 防止重复触发
	{
		return;
	}
	UpdateData(TRUE);

	if (!IsKinematicParamsValid()) {
		AfxMessageBox(_T("运动参数无效,请检查:每转步数,每转单位数,单位名称"));
		AppendLog(_T("运动参数无效,请检查:每转步数,每转单位数,单位名称"));
		return;
	}

	// 将UI PTP速度/加速度 (物理单位/s) 转换为驱动器需要的 (转/s)
	double dPtpVelocity_rps = ConvertUnitsPerSecToRps(m_dPtpVelocity);
	double dPtpAccel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(m_dPtpAccel);
	double dPtpDecel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(m_dPtpDecel);

	// 在发起任何回零动作前，先用PTP参数设置驱动器
	BOOL bSetProfile = m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &dPtpVelocity_rps, &dPtpAccel_rps_s2, &dPtpDecel_rps_s2);
	if (!bSetProfile)
	{
		AfxMessageBox(_T("回零失败：回零前设置PTP参数失败。"));
		AppendLog(_T("回零失败：回零前设置PTP参数失败。"));
		return;
	}

	// 检查X3传感器的当前状态
	USHORT inputStatus = 0;
	if (!m_ModbusRTUHelper.ReadDriverBoardInputs(m_nNodeID, &inputStatus))
	{
		AfxMessageBox(_T("读取传感器状态失败，无法启动回零！"));
		AppendLog(_T("读取传感器状态失败，无法启动回零！"));
		return;
	}

	// X3对应bit 2。灯亮(绿色)表示ON，此时 inputStatus 的 bit 2 为 0。
	if ((inputStatus & (1 << 2)) == 0)
	{
		// 情况一：原点传感器已被触发，进入“正在脱离”状态，并执行第一步脱离
		m_bIsEscapingHomeSensor = true;
		UpdateConnectStatus(true, true); // 更新UI进入“特殊操作中”状态
		ExecuteOneEscapeStep();          // 执行第一步脱离,之后再定时器中检查脱离状态,然后执行回零
	}
	else
	{
		// 情况二：原点传感器未被触发，直接执行标准回零
		BOOL bResult = m_ModbusRTUHelper.SeekHome(m_nNodeID, 3, 'L');
		if (bResult)
		{
			m_bIsHoming = true;
			UpdateConnectStatus(true, true);
		}
		else
		{
			AfxMessageBox(_T("回零指令发送失败！"));
		}
	}
}
void CMotorControlDlg::OnBnClickedButtonAbsMoveStart()
{
	if (!m_ModbusRTUHelper.IsOpen()) return;
	UpdateData(TRUE); 

   
	// 使用新的验证函数
	CString errorMsg;
	if (!ValidateMotionParameters(m_nAbsPosition, errorMsg)) {
		AfxMessageBox(errorMsg);
		AppendLog(errorMsg);
		return;
	}

	// 将UI PTP速度/加速度 (物理单位/s) 转换为驱动器需要的 (转/s)
	double dPtpVelocity_rps = ConvertUnitsPerSecToRps(m_dPtpVelocity);
	double dPtpAccel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(m_dPtpAccel);
	double dPtpDecel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(m_dPtpDecel);

	// 1. 设置P2P运动参数
	BOOL bResult = m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &dPtpVelocity_rps, &dPtpAccel_rps_s2, &dPtpDecel_rps_s2);
	if (!bResult)
	{
		
		AfxMessageBox(_T("运动前设置PTP参数失败(速度)"));
		AppendLog(_T("运动前设置PTP参数失败(速度)"));

		return;
	}

	// 2. 将UI输入的绝对位置 (物理单位) 转换为驱动器需要的步数 
	long nAbsPositionSteps = ConvertUnitsToSteps(m_nAbsPosition);

	// 转换为int（API要求）
	int nAbsPositionStepsInt = static_cast<int>(nAbsPositionSteps);

	// 3. 执行绝对位置运动 (FeedtoPosition)
	bResult = m_ModbusRTUHelper.FeedtoPosition(m_nNodeID, &nAbsPositionStepsInt);
	if (!bResult)
	{
		AfxMessageBox(_T("启动绝对运动失败！"));
		AppendLog(_T("启动绝对运动失败。"));
	}
	else {
		CString str;
		str.Format(_T("绝对运动启动，目标位置: %.3f %s"), m_nAbsPosition, m_sPhysicalUnitName);
		AppendLog(str);
	}
}

// --- 绝对运动 Stop ---
void CMotorControlDlg::OnBnClickedButtonAbsMoveStop()
{
	if (!m_ModbusRTUHelper.IsOpen()) return;
	// 使用 StopAndKillwithNormalDecel 可以在正常减速度下停止，体验更好
	// 如果需要紧急停止，使用 StopAndKill
	m_ModbusRTUHelper.StopAndKillwithNormalDecel(m_nNodeID);
	AppendLog(_T("绝对运动停止。"));
}

// --- 相对运动 Start ---
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


	// 将UI PTP速度/加速度 (物理单位/s) 转换为驱动器需要的 (转/s)
	double dPtpVelocity_rps = ConvertUnitsPerSecToRps(m_dPtpVelocity);
	double dPtpAccel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(m_dPtpAccel);
	double dPtpDecel_rps_s2 = ConvertUnitsPerSecSqToRpsPerSecSq(m_dPtpDecel);

	// 1. 设置P2P运动参数
	BOOL bResult = m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &dPtpVelocity_rps, &dPtpAccel_rps_s2, &dPtpDecel_rps_s2);
	if (!bResult)
	{
		AfxMessageBox(_T("运动前设置PTP参数失败(速度)"));
		AppendLog(_T("运动前设置PTP参数失败(速度)"));
		return;
	}

	// 2. 将UI输入的相对距离 (物理单位) 转换为驱动器需要的步数 (可能超出int范围)
	long nRelPositionSteps = ConvertUnitsToSteps(m_nRelPosition);
	// 3. 根据方向确定位移值的正负
	if (m_nRelDirection == 0) { 
		nRelPositionSteps = -nRelPositionSteps;
	}
	int nRelPositionStepsInt = static_cast<int>(nRelPositionSteps);
	// 4.执行相对运动
	bResult = m_ModbusRTUHelper.FeedtoLength(m_nNodeID, &nRelPositionStepsInt);

	if (!bResult)
	{
		AfxMessageBox(_T("启动相对运动失败！"));
		AppendLog(_T("启动相对运动失败。"));
	}
	else {
		CString str;
		str.Format(_T("相对运动启动，距离: %.3f %s，方向: %s"), m_nRelPosition, m_sPhysicalUnitName, m_nRelDirection == 0 ? _T("CW") : _T("CCW"));
		AppendLog(str);
	}
}

// --- 相对运动 Stop ---
void CMotorControlDlg::OnBnClickedButtonRelMoveStop()
{
	if (!m_ModbusRTUHelper.IsOpen()) return;
	m_ModbusRTUHelper.StopAndKillwithNormalDecel(m_nNodeID);
	AppendLog(_T("相对运动停止。"));
}

void CMotorControlDlg::AppendLog(const CString& sMessage)
{
	// 1. 获取当前时间并格式化
	CString sTime = CTime::GetCurrentTime().Format(_T("[%Y-%m-%d %H:%M:%S] "));
	CString sLogEntry = sTime + sMessage + _T("\r\n");

	// 2. 在UI界面上实时追加日志
	int nLength = m_editLog.GetWindowTextLength();
	m_editLog.SetSel(nLength, nLength);
	m_editLog.ReplaceSel(sLogEntry);

	// 3. 检查并滚动日志文件
	RotateLogFiles();

	// 4. 将日志写入物理文件
	try
	{
		CStdioFile file;
		// 使用新的辅助函数来获取完整路径
		if (file.Open(GetLogFilePath(_T("MotorControl.log")), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeText))
		{
			file.SeekToEnd(); // 移动到文件末尾
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

	// 使用辅助函数来获取文件路径
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
				CFile::Remove(sLogPath2);      // 删除 MotorControl.2.log
				CFile::Rename(sLogPath1, sLogPath2); // 将 .1.log 重命名为 .2.log
				CFile::Rename(sLogPath, sLogPath1);  // 将 .log 重命名为 .1.log
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

//对单位输入框的非空检查和有效值判断
bool CMotorControlDlg::IsKinematicParamsValid()
{
	// 检查基本参数
	if (m_nStepsPerRevolution <= 0 || m_dPhysicalValuePerRevolution <= 0.0 || m_sPhysicalUnitName.IsEmpty()) {
		return false;
	}

	// 检查参数是否会导致计算溢出
	// 例如：如果每转步数过大，或每转单位值过小，可能导致换算系数过大
	double conversion_factor = static_cast<double>(m_nStepsPerRevolution) / m_dPhysicalValuePerRevolution;
	if (conversion_factor > 1e10 || conversion_factor < 1e-10) {
		return false; // 换算系数过大或过小，可能导致计算问题
	}

	return true;
}

//======================以下为单位换算函数==========================

//unit --> rps
double CMotorControlDlg::ConvertUnitsPerSecToRps(double units_per_sec)
{
	if (!IsKinematicParamsValid()) {
		// 通常在调用前应检查 IsKinematicParamsValid，这里作为双重保险
		return 0.0;
	}
	// (单位/s) / (单位/转) = 转/s
	return units_per_sec / m_dPhysicalValuePerRevolution;
}

//unit --> rps/s
double CMotorControlDlg::ConvertUnitsPerSecSqToRpsPerSecSq(double units_per_sec_sq)
{
	if (!IsKinematicParamsValid()) {
		return 0.0;
	}
	// (单位/s^2) / (单位/转) = 转/s^2
	return units_per_sec_sq / m_dPhysicalValuePerRevolution;
}

//Unit --> Steps
long CMotorControlDlg::ConvertUnitsToSteps(double units)
{
	if (!IsKinematicParamsValid()) {
		return 0;
	}

	// 使用double进行计算，保持精度
	double steps_double = units * (static_cast<double>(m_nStepsPerRevolution) / m_dPhysicalValuePerRevolution);

	// 检查是否超出long范围
	if (steps_double > LONG_MAX || steps_double < LONG_MIN) {
		// 记录错误日志
		CString errorMsg;
		errorMsg.Format(_T("单位换算溢出: %.3f %s 转换为步数时超出范围"), units, m_sPhysicalUnitName);
		AppendLog(errorMsg);
		return 0;
	}


	return static_cast<long>(steps_double);
}

//Steps --> Unit 
double CMotorControlDlg::ConvertStepsToUnits(long steps) // 参数类型由long改为 int
{
	if (!IsKinematicParamsValid()) {
		return 0.0;
	}

	// 使用double进行计算，保持精度
	return static_cast<double>(steps) * (m_dPhysicalValuePerRevolution / static_cast<double>(m_nStepsPerRevolution));
}


bool CMotorControlDlg::IsStepsValueSafe(long steps)
{
	return (steps >= INT_MIN && steps <= INT_MAX);
}

// 新增：验证运动参数的综合安全性
bool CMotorControlDlg::ValidateMotionParameters(double position_units, CString& errorMsg)
{
	if (!IsKinematicParamsValid()) {
		errorMsg = _T("运动参数无效：请检查每转步数、每转单位数和单位名称");
		return false;
	}

	// 检查输入的物理值是否合理
	if (fabs(position_units) > 1000000.0) { // 假设超过100万单位就认为不合理
		errorMsg.Format(_T("输入的位置值过大：%.3f %s，可能导致计算溢出"), position_units, m_sPhysicalUnitName);
		return false;
	}

	// 转换为步数并检查范围
	long steps = ConvertUnitsToSteps(position_units);
	if (!IsStepsValueSafe(steps)) {
		errorMsg.Format(_T("计算的步数 %ld 超出驱动器支持的范围 [%d, %d]"), steps, INT_MIN, INT_MAX);
		return false;
	}

	return true;
}