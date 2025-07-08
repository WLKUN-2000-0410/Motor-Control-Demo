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

{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); //加载并设置应用程序图标，左上角，任务栏中的  
	m_hIconLedGray = NULL; // 初始化为 NULL
	m_hIconLedGreen = NULL;
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
	DDX_Control(pDX, IDC_CMB_COMPORT, m_cmbComPort);
	DDX_Control(pDX, IDC_CMB_BAUD, m_cmbBaud);
	DDX_Control(pDX, IDC_CMB_NODEID, m_cmbNodeID);
	DDX_Control(pDX, IDC_LED_X1, m_ledX1);
	DDX_Control(pDX, IDC_LED_X2, m_ledX2);
	DDX_Control(pDX, IDC_LED_X3, m_ledX3);

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



	return TRUE;
}

void CMotorControlDlg::UpdateConnectStatus(bool isConnect)
{
	// 更新窗口标题
	CString title;
	if (isConnect)
	{
		title.Format(_T("Motor Control Demo - Connected (COM%d)"), m_nCOMPort);
	}
	else
	{
		title = _T("Motor Control Demo - Unconnected");
	}
	SetWindowText(title);

	// 更新按钮状态
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(!isConnect);
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(isConnect);

	// 更新COM口选择状态
	m_cmbComPort.EnableWindow(!isConnect);
	m_cmbBaud.EnableWindow(!isConnect);
	m_cmbNodeID.EnableWindow(!isConnect);
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
		// 连接成功
		SetTimer(ID_TIMER_UPDATE_STATUS, TIMER_INTERVAL, NULL);  //启动定时器
		UpdateConnectStatus(true);
		// 成功不弹窗，只更新状态
	}
	else
	{
		// 连接失败
	
		UpdateConnectStatus(false);
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
		AfxMessageBox(statusMsg);

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
		AfxMessageBox(_T("电机使能成功！"));
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
	    
		AfxMessageBox(_T("电机开始运动20000步..."));

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
		ret = m_ModbusRTUHelper.ReadImmediateAbsolutePosition(m_nNodeID, &currentPosition);
		if (ret)
		{
			CString posMsg;
			posMsg.Format(_T("当前位置: %d 步"), currentPosition);
			AfxMessageBox(posMsg);
		}

		AfxMessageBox(_T("电机测试完成！通信正常！"));
	}
	catch (...)
	{
		AfxMessageBox(_T("电机测试过程中发生异常！"));
	}
}

void CMotorControlDlg::OnBnClickedButtonHome()
{
	// TODO: 在此添加控件通知处理程序代码


	BOOL bResult = m_ModbusRTUHelper.SeekHome(m_nNodeID, 3, 'L');

	if (bResult)
	{
		AfxMessageBox(_T("回零指令已发送！"));
	}
	else
	{
		AfxMessageBox(_T("回零指令发送失败！请检查电机状态。"));
	}
}


void CMotorControlDlg::OnBnClickedButtonTest()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_ModbusRTUHelper.IsOpen())
	{
		AfxMessageBox(_T("请先连接电机！"));
		return;
	}

	USHORT inputStatus = 0;
	BOOL bResult = m_ModbusRTUHelper.ReadDriverBoardInputs( m_nNodeID, &inputStatus);

	if (bResult)
	{
		// 读取成功，现在解析状态
		// 使用位与(&)操作符来检查特定位是否为1
		// (1 << 0) 就是 0x0001, (1 << 1) 就是 0x0002, (1 << 2) 就是 0x0004

		bool isX1_On = (inputStatus & (1 << 0)) != 0; // 检查 bit 0 (X1)
		bool isX2_On = (inputStatus & (1 << 1)) != 0; // 检查 bit 1 (X2)
		bool isX3_On = (inputStatus & (1 << 2)) != 0; // 检查 bit 2 (X3)

		CString msg;
		msg.Format(_T("输入端口状态:\n")
			_T("X1: %s\n")
			_T("X2: %s\n")
			_T("X3: %s"),
			isX1_On ? _T("ON (导通/低电平)") : _T("OFF (断开/高电平)"),
			isX2_On ? _T("ON (导通/低电平)") : _T("OFF (断开/高电平)"),
			isX3_On ? _T("ON (导通/低电平)") : _T("OFF (断开/高电平)"));

		AfxMessageBox(msg);

		m_ledX1.SetIcon(isX1_On ? m_hIconLedGray : m_hIconLedGreen);
		m_ledX2.SetIcon(isX2_On ? m_hIconLedGray : m_hIconLedGreen);
		m_ledX3.SetIcon(isX3_On ? m_hIconLedGray : m_hIconLedGreen);

	}
	else
	{
		AfxMessageBox(_T("读取输入状态失败！"));
	}
	

}
void CMotorControlDlg::OnTimer(UINT_PTR nIDEvent)
{
	
	if (nIDEvent == ID_TIMER_UPDATE_STATUS)
	{
		//更新LED状态
		UpdateSensorStatus();
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