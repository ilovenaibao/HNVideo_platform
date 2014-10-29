
// ScanDeviceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScanDevice.h"
#include "ScanDeviceDlg.h"
#include "afxdialogex.h"
#include "DeviceManager/DeviceInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

HANDLE g_thread_kill;
HANDLE g_thread_quit;

CDeviceInfo m_cDi;


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


// CScanDeviceDlg dialog




CScanDeviceDlg::CScanDeviceDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScanDeviceDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pThread = NULL;
	m_bInitaled = false;
	m_bSearching = false;
}

void CScanDeviceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DEVICE, m_DeviceList);
	DDX_Control(pDX, IDC_BUTTON_SEARCH, m_cSearchButton);
}

BEGIN_MESSAGE_MAP(CScanDeviceDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CScanDeviceDlg::OnBnClickedButtonSearch)
END_MESSAGE_MAP()


// CScanDeviceDlg message handlers

BOOL CScanDeviceDlg::OnInitDialog()
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

	m_DeviceList.SetExtendedStyle (LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_DeviceList.InsertColumn (0, _T ("序号"), LVCFMT_CENTER, 38);
	m_DeviceList.InsertColumn (1, _T ("ID"), LVCFMT_CENTER, 0);
	m_DeviceList.InsertColumn (2, _T ("设备名"), LVCFMT_CENTER, 140);
	m_DeviceList.InsertColumn (3, _T ("序列号"), LVCFMT_CENTER, 110);
	m_DeviceList.InsertColumn (4, _T ("设备时钟"), LVCFMT_CENTER, 130);
	m_DeviceList.InsertColumn (5, _T ("型号"), LVCFMT_CENTER, 80);
	m_DeviceList.InsertColumn (6, _T ("软件版本"), LVCFMT_CENTER, 60);
	m_DeviceList.InsertColumn (7, _T ("硬件版本"), LVCFMT_CENTER, 60);
	m_DeviceList.InsertColumn (8, _T ("DSP版本"), LVCFMT_CENTER, 60);
	m_DeviceList.InsertColumn (9, _T ("MAC地址"), LVCFMT_CENTER, 120);
	m_DeviceList.InsertColumn (10, _T ("IP地址"), LVCFMT_CENTER, 100);
	m_DeviceList.InsertColumn (11, _T ("子网掩码"), LVCFMT_CENTER, 100);
	m_DeviceList.InsertColumn (12, _T ("网关"), LVCFMT_CENTER, 100);
	m_DeviceList.InsertColumn (13, _T ("DNS"), LVCFMT_CENTER, 100);
	m_DeviceList.InsertColumn (14, _T ("视频状态"), LVCFMT_CENTER, 80);
	//m_device_list.SetBkImage(NULL, IDB_BITMAP1);

	g_thread_kill = CreateEvent (NULL, TRUE, FALSE, NULL);
	g_thread_quit = CreateEvent (NULL, TRUE, FALSE, NULL);

	m_bInitaled = true;

	m_nFreeTime = 0;
	m_nSearchTime = 0;

	m_cSearchButton.GetWindowText(m_SearchButtonOldText);
	SetTimer (SearchButtonEnableTimerId, 1000, NULL);

	//启动WinSock
	WSAData wsa;
	WSAStartup(MAKEWORD(2, 2),&wsa);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CScanDeviceDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CScanDeviceDlg::OnPaint()
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
HCURSOR CScanDeviceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CScanDeviceDlg::OnBnClickedButtonSearch()
{
	// TODO: Add your control notification handler code here
	m_cDeviceInfo.clear();
	m_sIpList.clear();
	m_DeviceList.DeleteAllItems();
	StartListening();

	//GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);
	//Sleep (10*1000);
	//GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(TRUE);

	//// 获取本机配置的IP地址列表
	//WORD wVersionRequested;
	//WSADATA wsaData;
	//wVersionRequested = MAKEWORD( 2, 2 );
	//HOSTENT *host=NULL;
	//if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
	//{
	//  char szhn[256];
	//  int nStatus = gethostname(szhn, sizeof(szhn));
	//  if (nStatus != SOCKET_ERROR )
	//  {
	//      host = gethostbyname(szhn);
	//      WSACleanup();
	//  }
	//}

	//if (host)
	//{
	//  int i=0;
	//  do
	//  {
	//      Send(7210, inet_ntoa( *(IN_ADDR*)host->h_addr_list[i]));
	//      i++;
	//  } while(host->h_addr_list[i] + host->h_length >= host->h_name);
	//}
	//else
	//{
	Broadcast();
	//}
}

void CScanDeviceDlg::OnTimer (UINT_PTR nIDEvent)
{
	if (nIDEvent == SearchButtonEnableTimerId)
	{
		if (m_bSearching)
		{
			m_nSearchTime++;
			m_nFreeTime++;
			if (m_nFreeTime < SearchTimeOut)
			{
				CString str;
				str.Format (_T("搜索中(%u)…"), m_nSearchTime);
				m_cSearchButton.SetWindowText (str);
			}
			else
			{
				m_cSearchButton.EnableWindow (TRUE);
				m_cSearchButton.SetWindowText (m_SearchButtonOldText);
				m_bSearching = false;
			}
		}
	}
	CDialog::OnTimer (nIDEvent);
}

#define MAX_CLINET_COUNT 64
UINT ProcessFunc (LPVOID pParam)
{

	UINT32 m_nnIndex = 1;
	CScanDeviceDlg * pdlg;
	pdlg = (CScanDeviceDlg *) (pParam);

	struct sockaddr_in remote;
	struct sockaddr_in serv_addr;
	int addr_size;
	//bool optval;
	//SOCKET listen_sock;
	SOCKET udp_sock;
	//SOCKET client[MAX_CLINET_COUNT];
	char udpbuf[1024];
	char udpsendbuf[1024];
	//char tcpbuf[1024];
	int offset = 0;
	MsgHead *head = (MsgHead *) udpbuf;
	udp_sock = socket (AF_INET, SOCK_DGRAM, 0);
	//unsigned long argp = 1;
	//ioctlsocket(udp_sock,FIONBIO,(unsigned long*)&argp);
	if (udp_sock != INVALID_SOCKET)
	{
		memset (&serv_addr, 0, sizeof (serv_addr) );
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons (7216);
		addr_size = sizeof (serv_addr);

		if (bind (udp_sock, (struct sockaddr *) &serv_addr, addr_size) < 0)
		{
			TRACE ("bind failed errcode=%d\n", WSAGetLastError() );
			//closesocket(listen_sock);
			closesocket (udp_sock);
			return 0;
		}
	}

	fd_set fdread;
	//fd_set fdwrite;
	int ret;
	timeval tv;
	//int i;
	int len;
	while (WaitForSingleObject (g_thread_kill, 1) == WAIT_TIMEOUT)
	{
		tv.tv_sec = 0;
		tv.tv_usec = 1;

		FD_ZERO (&fdread);
		FD_SET (udp_sock, &fdread);
		if ((ret = select (0, &fdread, NULL, NULL, &tv) ) == SOCKET_ERROR)
		{
		}
		if (ret > 0)
		{
			if (FD_ISSET (udp_sock, &fdread) )
			{
				memset (udpbuf, 0, sizeof (udpbuf) );
				ret = recvfrom (udp_sock, udpbuf, sizeof (udpbuf), 0, (struct sockaddr *) &remote, &addr_size);
				if (ret > 0)
				{
					if (true == pdlg->ParseData (udpbuf + sizeof (MsgHead), inet_ntoa (remote.sin_addr) ) )
					{
						/*majf, add, 2012.7.6*/
						MsgHead *head2 = (MsgHead *) udpsendbuf;
						memset (udpsendbuf , 0, sizeof (udpsendbuf) );
						//sprintf(udpsendbuf + sizeof (MsgHead), "INFO ACK\r\n");
						sprintf (udpsendbuf + sizeof (MsgHead), "INFO %s ACK\r\n", m_cDi.m_sName);
						head2->length = sizeof (MsgHead) + (int) strlen (udpsendbuf + sizeof (MsgHead) );
						head2->seq = head->seq;
						head2->reserved = 0;

						len = sendto (udp_sock, udpsendbuf, head2->length, 0, (struct sockaddr*) &remote, sizeof (struct sockaddr_in) );
						/*majf, add, 2012.7.6*/

						pdlg->m_nFreeTime = 0;
					}
				}
			}
		}
	}
	closesocket (udp_sock);
	SetEvent (g_thread_quit);
	return 1;
}

bool CScanDeviceDlg::StartListening()
{
	if (m_pThread == NULL)
	{
		m_pThread = AfxBeginThread (ProcessFunc, (LPVOID) this, THREAD_PRIORITY_HIGHEST, 0, 0, 0);
	}
	return m_pThread != NULL;
}

bool CScanDeviceDlg::ParseData (CString sInfo, CString sIP)
{
	AutoLock    lock(&m_CritSec);

	CDeviceInfo cDi;
	if (cDi.ParseData (sInfo, sIP) )
	{
		m_cDi = cDi;
		vector<CDeviceInfo>::iterator iter;
		int nIndex (0);
		for (iter = m_cDeviceInfo.begin(); iter != m_cDeviceInfo.end(); iter++, nIndex++)
		{
			if (cDi == (CDeviceInfo) (*iter))
			{
				(*iter).m_tTime = cDi.m_tTime;
				break;
			}
		}
		if (iter == m_cDeviceInfo.end())
		{
			// not found, new device
			m_cDeviceInfo.push_back (cDi);

			// 对窗口对象只发送消息(SendMessage)是没有现成问题的，
			// 如果获取其DC并直接绘制又不加以临界区保护就有点危险了(GDI32.dll不是线程安全的）
			CString sData;
			int nItem = m_DeviceList.GetItemCount();
			sData.Format (_T ("%d"), nItem + 1);
			m_DeviceList.InsertItem (nItem, sData);
			char sIndex[255];
			itoa (m_nIndex++, sIndex, 10);
			m_DeviceList.SetItemText (nItem, 1, sIndex);
			m_DeviceList.SetItemText (nItem, 2, cDi.m_sName);
			m_DeviceList.SetItemText (nItem, 3, cDi.m_sSN);
			m_DeviceList.SetItemText (nItem, 4, cDi.m_tTime.Format ("%Y-%m-%d %H:%M:%S") );
			m_DeviceList.SetItemText (nItem, 5, cDi.m_sModel);
			m_DeviceList.SetItemText (nItem, 6, cDi.m_sSoftVer);
			m_DeviceList.SetItemText (nItem, 7, cDi.m_sHddVer);
			m_DeviceList.SetItemText (nItem, 8, cDi.m_sDspVer);
			m_DeviceList.SetItemText (nItem, 9, cDi.m_sMAC);
			m_DeviceList.SetItemText (nItem, 10, cDi.m_sIP);
			m_sIpList.push_back (cDi.m_sIP);
			if (cDi.m_sIP != cDi.m_sRemoteIP)
				m_DeviceList.SetItemText (nItem, 9, cDi.m_sIP + "/" + cDi.m_sRemoteIP);
			m_DeviceList.SetItemText (nItem, 11, cDi.m_sMask);
			m_DeviceList.SetItemText (nItem, 12, cDi.m_sGateway);
			m_DeviceList.SetItemText (nItem, 13, cDi.m_sDNS);
			char sLost[255];
			itoa (cDi.m_nVideo, sLost, 2);
			m_DeviceList.SetItemText (nItem, 14, sLost);

		}
		else
		{
			m_DeviceList.SetItemText (nIndex, 3, cDi.m_tTime.Format ("%Y-%m-%d %H:%M:%S") );
		}

		return true;
	}
	return false;
}

bool CScanDeviceDlg::Broadcast ( const char* sRemoteIP, UINT16 nRemotePort, const char* sBuffer)
{
	bool bSuccess = false;
	SOCKET sock = INVALID_SOCKET;
	sockaddr_in remoteAddr = {0};
	BOOL optval = TRUE;
	int ret;
	int len;
	sock = socket (AF_INET, SOCK_DGRAM, 0);
	if (sock != INVALID_SOCKET)
	{
		ret = setsockopt (sock, SOL_SOCKET, SO_BROADCAST, (char*) &optval, sizeof (UINT32) );
		if (SOCKET_ERROR == ret)
		{
			AfxMessageBox (_T ("set opt broadcast failed") );
		}
		else
		{
			ZeroMemory (&remoteAddr, sizeof (remoteAddr) );
			remoteAddr.sin_family = AF_INET;
			remoteAddr.sin_addr.s_addr = inet_addr (sRemoteIP);
			remoteAddr.sin_port = htons (nRemotePort);
			char buf[1024] = "";
			MsgHead *pHead = (MsgHead *) buf;
			pHead->length = (int) strlen (sBuffer) + sizeof (MsgHead);
			pHead->seq = (UINT32) time (NULL);
			pHead->reserved = 0;
			memcpy (buf + sizeof (MsgHead), sBuffer, (strlen (sBuffer) > 1000) ? 1000 : strlen (sBuffer) );
			len = sendto (sock, buf, pHead->length, 0, (struct sockaddr*) &remoteAddr, sizeof (struct sockaddr_in) );
			if ( (UINT32) len < pHead->length)
			{
				AfxMessageBox (_T ("Error occured while broadcast") );
			}
			else
			{
				bSuccess = true;
			}
		}
		closesocket (sock);
	}
	else
	{
		AfxMessageBox (_T ("Create socket failed") );
	}
	return bSuccess;
}