#include "StdAfx.h"
#include "DeviceManager.h"
#include <WinSock2.h>

HANDLE g_thread_kill;
HANDLE g_thread_quit;

CDeviceInfo m_cDi;

CDeviceManager::CDeviceManager()
	: m_pThread(NULL)
	, m_nFreeTime(0)
	, m_bInitaled(FALSE)
{
	//启动WinSock
	WSAData wsa;
	WSAStartup(MAKEWORD(2, 2),&wsa);

	g_thread_kill = CreateEvent (NULL, TRUE, FALSE, NULL);
	g_thread_quit = CreateEvent (NULL, TRUE, FALSE, NULL);

	ResetEvent(g_thread_kill);
	ResetEvent(g_thread_quit);
}

CDeviceManager::~CDeviceManager()
{
	if (NULL != m_pThread)
	{
		SetEvent (g_thread_kill);
		WaitForSingleObject (g_thread_quit, INFINITE);
		CloseHandle(g_thread_kill);
		CloseHandle(g_thread_quit);
	}
}

#define MAX_CLINET_COUNT 64
UINT ProcessFunc(LPVOID pParam)
{

	UINT32 m_nnIndex = 1;
	CDeviceManager * pdlg;
	pdlg = (CDeviceManager *) (pParam);

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
	while (WaitForSingleObject(g_thread_kill, 1) == WAIT_TIMEOUT)
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
					CString sInfo = CString(udpbuf + sizeof(MsgHead));
					CString sIp = CString(inet_ntoa(remote.sin_addr));
					if (true == pdlg->ParseData(sInfo, sIp))
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

bool CDeviceManager::StartListening()
{
	if (NULL == m_pThread)
	{
		m_pThread = AfxBeginThread(ProcessFunc, (LPVOID) this, THREAD_PRIORITY_HIGHEST, 0, 0, 0);
	}
	return m_pThread != NULL;
}

bool CDeviceManager::ParseData (CString sInfo, CString sIP)
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
			m_cDeviceInfo.push_back(cDi);

			//// 对窗口对象只发送消息(SendMessage)是没有现成问题的，
			//// 如果获取其DC并直接绘制又不加以临界区保护就有点危险了(GDI32.dll不是线程安全的）
			//CString sData;
			//int nItem = m_cDeviceList.GetItemCount();
			//sData.Format (_T ("%d"), nItem + 1);
			//m_cDeviceList.InsertItem (nItem, sData);
			//char sIndex[255];
			//itoa (m_nIndex++, sIndex, 10);
			//m_cDeviceList.SetItemText (nItem, 1, sIndex);
			//m_cDeviceList.SetItemText (nItem, 2, cDi.m_sName);
			//m_cDeviceList.SetItemText (nItem, 3, cDi.m_sSN);
			//m_cDeviceList.SetItemText (nItem, 4, cDi.m_tTime.Format ("%Y-%m-%d %H:%M:%S") );
			//m_cDeviceList.SetItemText (nItem, 5, cDi.m_sModel);
			//m_cDeviceList.SetItemText (nItem, 6, cDi.m_sSoftVer);
			//m_cDeviceList.SetItemText (nItem, 7, cDi.m_sHddVer);
			//m_cDeviceList.SetItemText (nItem, 8, cDi.m_sDspVer);
			//m_cDeviceList.SetItemText (nItem, 9, cDi.m_sMAC);
			//m_cDeviceList.SetItemText (nItem, 10, cDi.m_sIP);
			//m_sIpList.push_back (cDi.m_sIP);
			//if (cDi.m_sIP != cDi.m_sRemoteIP)
			//	m_cDeviceList.SetItemText (nItem, 9, cDi.m_sIP + "/" + cDi.m_sRemoteIP);
			//m_cDeviceList.SetItemText (nItem, 11, cDi.m_sMask);
			//m_cDeviceList.SetItemText (nItem, 12, cDi.m_sGateway);
			//m_cDeviceList.SetItemText (nItem, 13, cDi.m_sDNS);
			//char sLost[255];
			//itoa (cDi.m_nVideo, sLost, 2);
			//m_cDeviceList.SetItemText (nItem, 14, sLost);

		}
		else
		{
			//m_cDeviceList.SetItemText (nIndex, 3, cDi.m_tTime.Format ("%Y-%m-%d %H:%M:%S") );
		}

		return true;
	}
	return false;
}

bool CDeviceManager::Broadcast(const char* sRemoteIP, UINT16 nRemotePort, const char* sBuffer)
{
	bool bSuccess = false;
	SOCKET sock = INVALID_SOCKET;
	sockaddr_in remoteAddr = {0};
	BOOL optval = TRUE;
	int ret;
	int len;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
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