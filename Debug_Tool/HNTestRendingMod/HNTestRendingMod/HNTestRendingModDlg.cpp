
// HNTestRendingModDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HNTestRendingMod.h"
#include "HNTestRendingModDlg.h"
#include "afxdialogex.h"

#include <conio.h>

#include "./src/NetGobalDef.h"
//#include "./src/main.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHNTestRendingModDlg dialog




CHNTestRendingModDlg::CHNTestRendingModDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHNTestRendingModDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHNTestRendingModDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHNTestRendingModDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(TASK_MSG_COMPLETE, OnTaskComplete)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


LRESULT CHNTestRendingModDlg::OnTaskComplete(WPARAM wParam, LPARAM lParam)
{
	int nTaskID = static_cast<int>(wParam);
	switch(nTaskID)
	{
	case TASK_CONNECT_SERVER:		// 连接服务器
		renderingMod.Start(); // 启动视频绘制模块
		//OnConnectResult(reinterpret_cast<void*>(lParam));
		break;
	default:
		break;
	}
	return 0;
}

// CHNTestRendingModDlg message handlers

BOOL CHNTestRendingModDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	hDisplay = GetSafeHwnd();
	hContainer = GetSafeHwnd();
	renderingMod.Init(hDisplay, hContainer);

	TestLinkCollectPoint(this);
	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHNTestRendingModDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHNTestRendingModDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHNTestRendingModDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CHNTestRendingModDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	POINT pos; 
	//将消息参数定义为屏幕坐标值 
	GetCursorPos(&pos); 
	::SendMessage(GetSafeHwnd(), TASK_MSG_COMPLETE, TASK_CONNECT_SERVER, 0);

	CDialogEx::OnLButtonDown(nFlags, point);
}


// 测试连接采集点
void CHNTestRendingModDlg::TestLinkCollectPoint(CHNTestRendingModDlg *p)
{
	wchar_t ip[20] = { L"192.168.1.147" };
	int result = 0, ncode = 0;
	int step = 0; // 记录执行至哪一步

	//netTransMod.Init(); // 初始化网络传输模块
	ZeroMemory(&connectStruct, sizeof(connectStruct));
	size_t num;
	wcstombs_s(&num, connectStruct.ip, 16, ip, _TRUNCATE);
	connectStruct.chnl = 0;
	connectStruct.netTransMod = &netTransMod;
	connectStruct.procMod = &renderingMod; // 视频绘制模块
	connectStruct.mode = DATA_MODE_TCP;
	connectStruct.hWnd = p->hDisplay;
	connectStruct.bSilent = false;
	for (int i = 0; i < CHNL_NUM; i++)
	{	
		connectStruct.algoType[i] = 0x0000FFFF;
	}

	CConnectServerStruct *param = &connectStruct;
	// 连接服务器
	result = param->netTransMod->Connect(param->hWnd, param->ip, param->chnl, param->mode, param->procMod, ncode);
	if (result != IPVA_DTC_OK)
	{
		// 连接服务器过程中出错
		return;
	}
	step = 1;
	getch();

	//// 获取网络通讯协议版本号
	//result = param->netTransMod->GetServerProtocol(param->srvNetProtMainVer, param->srvNetProtSubVer, ncode);
	//if (result != IPVA_DTC_OK && result != IPVA_DTC_ERR_OP_FAILED)
	//{
	//	// 非服务端操作失败的异常
	//	return;
	//}
	//step = 2;

	//// 获取服务器程序版本号
	//result = param->netTransMod->GetServerVersion(param->serverVer, ncode);
	//if (result != IPVA_DTC_OK && result != IPVA_DTC_ERR_OP_FAILED)
	//{
	//	// 非服务端操作失败的异常
	//	return;
	//}
	//step = 3;

	netTransMod.Clear(); // 清除网络传输模块资源
}