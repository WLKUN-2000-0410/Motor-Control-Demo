// MotorControlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MotorControl.h"
#include "MotorControlDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


const UINT_PTR TIMER_INTERVAL = 200;  //定时器时间间隔

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
	, m_nBaudRate(19200) // 后续变成可选项
	, m_nNodeID(1)       // 这个后续变成可选项 
	, m_dJogSpeed(10.0)    // 默认速度 10 rps
	, m_dJogAccel(100.0)   // 默认加速度 100 rps/s
	, m_dJogDecel(100.0)
	, m_sCurrentPosition(_T("0 steps"))
	, m_bAutoHomeOnConnect(TRUE) //  默认勾选“自动回零”
	, m_dPtpVelocity(10.0)
	, m_dPtpAccel(100.0)
	, m_dPtpDecel(100.0)
	, m_nAbsPosition(20000)
	, m_nRelPosition(20000)
	, m_nRelDirection(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); //加载并设置应用程序图标，左上角，任务栏中的  
	m_hIconLedGray = NULL; // 初始化为 NULL
	m_hIconLedGreen = NULL;
	m_bIsHoming = false;
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

	// 初始化连接状态为未连接
	UpdateConnectStatus(false);

	// 扫描COM端口
	ScanComPorts();


	// 初始化波特率下拉框
	m_cmbBaud.AddString(_T("9600"));
	m_cmbBaud.AddString(_T("19200")); 
	m_cmbBaud.AddString(_T("38400"));
	m_cmbBaud.AddString(_T("57600"));
	m_cmbBaud.AddString(_T("115200"));

	// 设置默认选项为 "19200"
	// SelectString 会自动查找并选中包含该文本的项
	m_cmbBaud.SelectString(-1, _T("19200"));

	CString strNodeID;
	for (int i = 1; i <= 20; ++i)
	{
		strNodeID.Format(_T("%d"), i);
		m_cmbNodeID.AddString(strNodeID);
	}
	// 设置默认选项为 "1"
	m_cmbNodeID.SetCurSel(0);

	m_hIconLedGray = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_LED_GRAY), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	m_hIconLedGreen = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_LED_GREEN), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	if (m_hIconLedGray == NULL )
	{
		AfxMessageBox(_T("加载 LED 图标资源失败！"));
	}
	else
	{
		// 成功加载，设置图标
		m_ledX1.SetIcon(m_hIconLedGray);
		m_ledX2.SetIcon(m_hIconLedGray);
		m_ledX3.SetIcon(m_hIconLedGray);
	}

	//ptp方向选择
	m_cmbRelDirection.AddString(_T("CW (Clockwise)"));    // Index 0
	m_cmbRelDirection.AddString(_T("CCW (Counter-Clockwise)")); // Index 1
	m_cmbRelDirection.SetCurSel(0); // 默认选中CW

	UpdateData(FALSE); // 将所有成员变量的初始值更新到UI上
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
		m_cmbComPort.SetCurSel(-1); // <--- 修改这里！将 0 改为 -1
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
		return;
	}

	// 提取端口号
	strPort.Replace(_T("COM"), _T(""));
	m_nCOMPort = _ttoi(strPort);

	// 尝试连接

	CString strBaudRate;
	int nBaudSel = m_cmbBaud.GetCurSel();
	if (nBaudSel == CB_ERR)
	{
		AfxMessageBox(_T("请选择波特率"));
		return;
	}
	m_cmbBaud.GetLBText(nBaudSel, strBaudRate);
	m_nBaudRate = _ttoi(strBaudRate); // 将字符串转换为整数，并更新成员变量

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
		// 连接失败
		UpdateConnectStatus(false, false);
		AfxMessageBox(_T("连接失败，请检查端口是否被占用"));
	}

	//TestMotorCommunication();

}

void CMotorControlDlg::OnBnClickedButtonDisconnect()
{

	KillTimer(ID_TIMER_UPDATE_STATUS); //先停止定时器


	m_ModbusRTUHelper.Close();	// 断开连接

	UpdateConnectStatus(false);
	m_ledX1.SetIcon(m_hIconLedGray);
	m_ledX2.SetIcon(m_hIconLedGray);
	m_ledX3.SetIcon(m_hIconLedGray);
}

void CMotorControlDlg::TestMotorCommunication()
{
	try
	{
		// 1. 首先读取电机状态，验证通信
		UINT statusCode = 0;
		BOOL ret = m_ModbusRTUHelper.ReadStatusCode(m_nNodeID, &statusCode);
		if (!ret)
		{
			AfxMessageBox(_T("读取电机状态失败！请检查电机连接和节点ID"));
			return;
		}

		CString statusMsg;
		statusMsg.Format(_T("电机状态码: 0x%04X"), statusCode);
		//AfxMessageBox(statusMsg);

		// 2. 读取报警代码
		UINT alarmCode = 0;
		ret = m_ModbusRTUHelper.ReadAlarmCode(m_nNodeID, &alarmCode);
		if (ret)
		{
			if (alarmCode != 0)
			{
				CString alarmMsg;
				alarmMsg.Format(_T("电机有报警: 0x%04X，将尝试复位"), alarmCode);
				AfxMessageBox(alarmMsg);

				// 复位报警
				m_ModbusRTUHelper.AlarmReset(m_nNodeID);
				Sleep(100); // 等待复位完成
			}
		}

		// 3. 使能电机
		ret = m_ModbusRTUHelper.DriveEnable(m_nNodeID, TRUE);
		if (!ret)
		{
			AfxMessageBox(_T("电机使能失败！"));
			return;
		}
	//	AfxMessageBox(_T("电机使能成功！"));
		Sleep(500); // 等待使能完成

					// 4. 设置运动参数并执行小幅相对运动
		double velocity = 10.0;    // 10 rps 的速度
		double accel = 100.0;      // 100 rps? 的加速度
		double decel = 100.0;      // 100 rps? 的减速度

								   // 设置P2P运动参数
		ret = m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &velocity, &accel, &decel);
		if (!ret)
		{
			AfxMessageBox(_T("设置运动参数失败！"));
			return;
		}
	    
		//AfxMessageBox(_T("电机开始运动20000步..."));

		// 5. 执行相对运动 (20000步)
		int relativeSteps = 20000;
		ret = m_ModbusRTUHelper.RelMove(m_nNodeID, relativeSteps, &velocity, &accel, &decel);
		if (!ret)
		{
			AfxMessageBox(_T("启动相对运动失败！"));
			return;
		}

		// 6. 等待运动完成（可选）
		Sleep(2000); // 等待2秒

		// 7. 读取当前位置
		int currentPosition = 0;
	//	ret = m_ModbusRTUHelper.ReadImmediateAbsolutePosition(m_nNodeID, &currentPosition);
		ret = m_ModbusRTUHelper.ReadEncoderPosition(m_nNodeID, &currentPosition);
		if (ret)
		{
			CString posMsg;
		//	posMsg.Format(_T("当前位置: %d 步"), currentPosition);
			AfxMessageBox(posMsg);
		}

	//	AfxMessageBox(_T("电机测试完成！通信正常！"));
	}
	catch (...)
	{
		AfxMessageBox(_T("电机测试过程中发生异常！"));
	}
}

void CMotorControlDlg::OnBnClickedButtonHome()
{
	StartHomingProcess();
}


void CMotorControlDlg::OnBnClickedButtonTest()
{
	//// TODO: 在此添加控件通知处理程序代码
	//if (!m_ModbusRTUHelper.IsOpen())
	//{
	//	AfxMessageBox(_T("请先连接电机！"));
	//	return;
	//}

	//USHORT inputStatus = 0;
	//BOOL bResult = m_ModbusRTUHelper.ReadDriverBoardInputs( m_nNodeID, &inputStatus);

	//if (bResult)
	//{
	//	// 读取成功，现在解析状态
	//	// 使用位与(&)操作符来检查特定位是否为1
	//	// (1 << 0) 就是 0x0001, (1 << 1) 就是 0x0002, (1 << 2) 就是 0x0004

	//	bool isX1_On = (inputStatus & (1 << 0)) != 0; // 检查 bit 0 (X1)
	//	bool isX2_On = (inputStatus & (1 << 1)) != 0; // 检查 bit 1 (X2)
	//	bool isX3_On = (inputStatus & (1 << 2)) != 0; // 检查 bit 2 (X3)

	//	CString msg;
	//	msg.Format(_T("输入端口状态:\n")
	//		_T("X1: %s\n")
	//		_T("X2: %s\n")
	//		_T("X3: %s"),
	//		isX1_On ? _T("ON (导通/低电平)") : _T("OFF (断开/高电平)"),
	//		isX2_On ? _T("ON (导通/低电平)") : _T("OFF (断开/高电平)"),
	//		isX3_On ? _T("ON (导通/低电平)") : _T("OFF (断开/高电平)"));

	//	AfxMessageBox(msg);

	//	m_ledX1.SetIcon(isX1_On ? m_hIconLedGray : m_hIconLedGreen);
	//	m_ledX2.SetIcon(isX2_On ? m_hIconLedGray : m_hIconLedGreen);
	//	m_ledX3.SetIcon(isX3_On ? m_hIconLedGray : m_hIconLedGreen);

	//}
	//else
	//{
	//	AfxMessageBox(_T("读取输入状态失败！"));
	//}
	//TestMotorCommunication();
	//m_ModbusRTUHelper.SetPosition(m_nNodeID, 0);
	//UpdateData(TRUE); // 从UI获取最新的参数值

					  // 1. 设置P2P运动参数
	//BOOL bResult = m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &m_dPtpVelocity, &m_dPtpAccel, &m_dPtpDecel);

	if (!m_ModbusRTUHelper.IsOpen())
	{
		AfxMessageBox(_T("电机未连接。"));
		return;
	}

	//AfxMessageBox(_T("执行急停！电机将以最大减速度停止并清除指令队列。"));
	BOOL bResult = m_ModbusRTUHelper.StopAndKill(m_nNodeID);

	if (bResult)
	{
		// 如果成功发送急停指令，应该重置回零状态，因为回零过程会被中断
		if (m_bIsHoming || m_bIsEscapingHomeSensor)
		{
			m_bIsHoming = false;
			m_bIsEscapingHomeSensor = false;
			// 急停会强制停止，所以状态应该恢复到连接就绪
			UpdateConnectStatus(true, false);
		}
	//	AfxMessageBox(_T("急停指令已发送。"));
	}
	else
	{
	//	AfxMessageBox(_T("急停指令发送失败！"));
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

		// 检查并处理回零/脱离状态
		CheckHomingStatus();

		// 更新传感器LED状态
		UpdateSensorStatus();

		// 更新位置显示
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
		return; // 如果未连接，则不执行任何操作
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
	// 从UI获取最新的参数
	UpdateData(TRUE);

	// 检查是否已连接
	if (!m_ModbusRTUHelper.IsOpen())
	{
		return 0;
	}

	// 获取是哪个按钮被按下了
	UINT nBtnID = (UINT)wParam;

	double dSpeed = m_dJogSpeed;
	double dAccel = m_dJogAccel;
	// 通常JOG的减速度和加速度可以设为一样
	 double dDecel = m_dJogDecel;

	if (nBtnID == IDC_BUTTON_JOG_MINUS)
	{
		// 如果是JOG-按钮，将速度设为负值
		dSpeed = -m_dJogSpeed;
	}

	// 根据官方示例，JOG前最好先调用此函数
	m_ModbusRTUHelper.WriteDistanceOrPosition(m_nNodeID, 1);

	// 设置JOG参数
	BOOL ret = m_ModbusRTUHelper.SetJogProfile(m_nNodeID, &dSpeed, &dAccel, &dDecel);
	if (!ret)
	{
		AfxMessageBox(_T("设置JOG参数失败！"));
		return 0;
	}

	// 启动JOG
	m_ModbusRTUHelper.StartJogging(m_nNodeID);

	return 0;
}

afx_msg LRESULT CMotorControlDlg::OnJogButtonUp(WPARAM wParam, LPARAM lParam)
{
	// 检查是否已连接
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
		// 检查“运动中”状态位 (第4位)。如果为0，表示上一步的-3000步已走完。
		if ((statusCode & (1 << 4)) == 0)
		{
			// 既然运动停止了，就再次检查传感器状态
			USHORT inputStatus = 0;
			m_ModbusRTUHelper.ReadDriverBoardInputs(m_nNodeID, &inputStatus);

			if ((inputStatus & (1 << 2)) == 0)
			{
				// **传感器依然被触发**，说明脱离还未完成，需要再走一步
				ExecuteOneEscapeStep(); // 再次发送-3000步指令
			}
			else
			{
				// **传感器已经脱离！** 脱离阶段成功完成。
				m_bIsEscapingHomeSensor = false;
				//AfxMessageBox(_T("脱离传感器完成，开始正式回零。"));

				// 立即启动正式回零
				BOOL bResult = m_ModbusRTUHelper.SeekHome(m_nNodeID, 3, 'L');
				if (bResult)
				{
					m_bIsHoming = true; // 进入“正在回零”状态
				}
				else
				{
					AfxMessageBox(_T("回零指令发送失败！"));
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
			//AfxMessageBox(_T("回零完成，设备已就绪！"));
			UpdateConnectStatus(true, false);
		}
	}
}

void CMotorControlDlg::ExecuteOneEscapeStep()
{
	UpdateData(TRUE);
	int escape_dist = 40000;  // 脱离距离 (负数代表反向)

	m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &m_dPtpVelocity,&m_dPtpAccel, &m_dPtpDecel);
	
	//m_ModbusRTUHelper.RelMove(m_nNodeID, escape_dist, NULL, NULL, NULL);
	m_ModbusRTUHelper.FeedtoLength(m_nNodeID, &escape_dist);
}

void CMotorControlDlg::StartHomingProcess()
{
	if (m_bIsHoming || m_bIsEscapingHomeSensor) // 防止重复触发
	{
		return;
	}
	UpdateData(TRUE);

	//在发起任何回零动作前，先用PTP参数设置驱动器,其实这一步操作是多余的，因为
	//脱离的时候就已经设置了速度
	BOOL bSetProfile = m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &m_dPtpVelocity, &m_dPtpAccel, &m_dPtpDecel);
	if (!bSetProfile)
	{
		AfxMessageBox(_T("设置PTP参数失败，无法启动回零！"));
		return;
	}


	// 检查X3传感器的当前状态
	USHORT inputStatus = 0;
	if (!m_ModbusRTUHelper.ReadDriverBoardInputs(m_nNodeID, &inputStatus))
	{
		AfxMessageBox(_T("读取传感器状态失败，无法启动回零！"));
		return;
	}

	// X3对应bit 2。灯亮(绿色)表示ON，此时 inputStatus 的 bit 2 为 0。
	if ((inputStatus & (1 << 2)) == 0)
	{
		// 情况一：原点传感器已被触发，进入“正在脱离”状态，并执行第一步脱离
		//AfxMessageBox(_T("原点传感器已被触发，开始执行循环脱离..."));
		m_bIsEscapingHomeSensor = true;
		UpdateConnectStatus(true, true); // 更新UI进入“特殊操作中”状态
		ExecuteOneEscapeStep();          // 执行第一步脱离
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
	UpdateData(TRUE); // 从UI获取最新的参数值

	 // 1. 设置P2P运动参数
	BOOL bResult = m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &m_dPtpVelocity, &m_dPtpAccel, &m_dPtpDecel);
	if (!bResult)
	{
		AfxMessageBox(_T("设置运动参数失败！"));
		return;
	}

	// 2. 执行绝对位置运动 (FeedtoPosition)
	// 注意：AbsMove 是一个封装好的函数，它内部可能也是调用 FeedtoPosition
	// 这里直接用 FeedtoPosition，因为它更基础。
	// 如果用 AbsMove，则不需要预先调用 SetP2PProfile。
	bResult = m_ModbusRTUHelper.FeedtoPosition(m_nNodeID, &m_nAbsPosition);
	if (!bResult)
	{
		AfxMessageBox(_T("启动绝对运动失败！"));
	}
}

// --- 绝对运动 Stop ---
void CMotorControlDlg::OnBnClickedButtonAbsMoveStop()
{
	if (!m_ModbusRTUHelper.IsOpen()) return;
	// 使用 StopAndKillwithNormalDecel 可以在正常减速度下停止，体验更好
	// 如果需要紧急停止，使用 StopAndKill
	m_ModbusRTUHelper.StopAndKillwithNormalDecel(m_nNodeID);
}

// --- 相对运动 Start ---
void CMotorControlDlg::OnBnClickedButtonRelMoveStart()
{
	if (!m_ModbusRTUHelper.IsOpen()) return;
	UpdateData(TRUE); 

	// 1. 设置P2P运动参数
	BOOL bResult = m_ModbusRTUHelper.SetP2PProfile(m_nNodeID, &m_dPtpVelocity, &m_dPtpAccel, &m_dPtpDecel);
	if (!bResult)
	{
		AfxMessageBox(_T("设置运动参数失败！"));
		return;
	}

	// 2. 根据方向确定位移值的正负
	int distance = m_nRelPosition;
	if (m_nRelDirection == 1) // 1 代表 CCW (反向)
	{
		distance = -m_nRelPosition;
	}

	// 3. 执行相对位置运动 (FeedtoLength)
	bResult = m_ModbusRTUHelper.FeedtoLength(m_nNodeID, &distance);
	if (!bResult)
	{
		AfxMessageBox(_T("启动相对运动失败！"));
	}
}

// --- 相对运动 Stop ---
void CMotorControlDlg::OnBnClickedButtonRelMoveStop()
{
	if (!m_ModbusRTUHelper.IsOpen()) return;
	m_ModbusRTUHelper.StopAndKillwithNormalDecel(m_nNodeID);
}