
// HNClientRenderingModDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HNClientRenderingMod.h"
#include "HNClientRenderingModDlg.h"
#include "afxdialogex.h"

#include <conio.h>

#include "../src/NetGobalDef.h"
#include "../../../video_platform/HNCommon/process_xml/ProcessXML.h"
#include "../src/test/Client.h"
#include "./DeviceManager/DeviceManager.h"

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


// CHNClientRenderingModDlg dialog




CHNClientRenderingModDlg::CHNClientRenderingModDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHNClientRenderingModDlg::IDD, pParent)
	, hDataRcvThread(NULL)
	, is_connect_server(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHNClientRenderingModDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHNClientRenderingModDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(TASK_MSG_COMPLETE, OnTaskComplete)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


LRESULT CHNClientRenderingModDlg::OnTaskComplete(WPARAM wParam, LPARAM lParam)
{
	int nTaskID = static_cast<int>(wParam);
	switch(nTaskID)
	{
	case TASK_CONNECT_SERVER:		// 连接服务器
		renderingMod.Start(); // 启动视频绘制模块
		//OnConnectResult(reinterpret_cast<void*>(lParam));
		break;
	case TASK_DISCONNECT_SERVER:		// 连接服务器
		renderingMod.Stop(); // 启动视频绘制模块
		//OnConnectResult(reinterpret_cast<void*>(lParam));
		break;
	default:
		break;
	}
	return 0;
}

// CHNClientRenderingModDlg message handlers

BOOL CHNClientRenderingModDlg::OnInitDialog()
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

	// scan devices list
	CDeviceManager device_manager;

	device_manager.StartListening();
	device_manager.Broadcast();

	return TRUE;
	// scan end

	hDisplay = GetSafeHwnd();
	hContainer = GetSafeHwnd();
	renderingMod.Init(hDisplay, hContainer);

	TestSendVideoRequest();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHNClientRenderingModDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHNClientRenderingModDlg::OnPaint()
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
HCURSOR CHNClientRenderingModDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 测试客户端->分发服务器发送请求视频
void CHNClientRenderingModDlg::TestSendVideoRequest()
{	
	static unsigned int count = 10000;
	XmlElement *xml_frame = process_xml.RegConnectM("586","1","采集点7", NULL, NULL, "111");
	process_xml.CreateXmlBuffer(p_xml_buf, xml_frame);
	
	send_data.xml_len = strlen(p_xml_buf);
	send_data.version = 100;
	send_data.cmd_code = CMD_CODE_CREATE_CONNECT;
	send_data.source_id = count;
	send_data.dest_id = 17;
	send_data.data_len = sizeof(SendMegHead) + send_data.xml_len;

	process_xml.PrintXmlBufferWithColor(p_xml_buf);
	
	int res = 0;
	int result = 0;
	int nCode = 0;
	int connect_flag = 0;

	client.hDisplayWnd = this->hDisplay;
	client.m_send_head_buf = (char *)&send_data;
	client.m_send_buf_len = sizeof(SendMegHead);
	client.m_send_xml = p_xml_buf;
	client.m_send_data = NULL;
	//连接到127.0.0.1（即本地）,端口号为8888的服务端
	connect_flag = client.Connect(client.m_sock, 19531, "192.168.1.37");
	//int connect_flag = client.Connect(19531, "127.0.0.1");
	if (0 != connect_flag) {
		return;
	}
	processModule = &renderingMod;
	client.processModule = (ITCPModelDataProc *)processModule;
	// 启动数据接收线程
	if (hDataRcvThread == NULL)
	{
		hDataRcvThread = (HANDLE)_beginthreadex(NULL, 0, CClientNet::RunThreadPlayVideo_YUV, &client, 0 , NULL);
	}
	if (hDataRcvThread == NULL)
	{
		result = IPVA_DTC_ERR_BT_FAILED;
		MessageBox(L"error start Thread!", NULL);
		exit(0);
	}

	/*while(TRUE) {
		Sleep(1000);
	}*/
	//getch();
	process_xml.FreeXmlElement(xml_frame);
}

void CHNClientRenderingModDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	
	

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CHNClientRenderingModDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (!is_connect_server) {
		::SendMessage(GetSafeHwnd(), TASK_MSG_COMPLETE, TASK_CONNECT_SERVER, 0);
		is_connect_server = TRUE;
	} else {

		::SendMessage(GetSafeHwnd(), TASK_MSG_COMPLETE, TASK_DISCONNECT_SERVER, 0);
		is_connect_server = FALSE;
	}

	CDialogEx::OnLButtonDblClk(nFlags, point);
}
