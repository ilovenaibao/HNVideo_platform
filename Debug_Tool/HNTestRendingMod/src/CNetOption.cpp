#include "CNetOption.h"


CNetOption::CNetOption()
{

}

CNetOption::~CNetOption()
{

}

int CNetOption::InitSocket(SOCKET& s, int sndTimeo, int rcvTimeo)
{
	int result = 0;
	do 
	{
		// 创建套接字
		result = CNetOption::CreateSocket(s);
		if (0 != result)
		{
			break;
		}
		// 设置超时
		result = CNetOption::SetTimeout(s, sndTimeo, rcvTimeo);
		if (0 != result)
		{
			break;
		}
	} while (false);
	return result;
}

int CNetOption::CreateSocket(SOCKET& s)
{
	int iErrMsg = 0;
	//启动WinSock
	WSAData wsa;
	iErrMsg = WSAStartup(MAKEWORD(2, 2),&wsa);
	if (iErrMsg != NO_ERROR) {
		return iErrMsg;
	}
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return s != INVALID_SOCKET ? 0 : WSAGetLastError();
}

int CNetOption::SetTimeout(SOCKET s, int sndTimeo, int rcvTimeo)
{
	int result = 0;
	do 
	{
		int res;
		if (sndTimeo > 0)
		{
			res = setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char*)&sndTimeo, sizeof(sndTimeo));
			if (result == SOCKET_ERROR)
			{
				result = WSAGetLastError();
				break;
			}
		}
		if (rcvTimeo > 0)
		{
			res = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&rcvTimeo, sizeof(rcvTimeo));
			if (result == SOCKET_ERROR)
			{
				result = WSAGetLastError();
			}
		}
	} while (false);
	return result;
}

int CNetOption::Connect(SOCKET s, const char* lpAddr, unsigned short port)
{
	int result = 0;
	SOCKADDR_IN server;
	ZeroMemory(&server, sizeof(server));
	server.sin_addr.s_addr = inet_addr(lpAddr);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	unsigned long noblock = 1;
	do 
	{
		if (SOCKET_ERROR == ioctlsocket(s, FIONBIO, &noblock))
		{
			result = ::WSAGetLastError();
			break;
		}
		if (SOCKET_ERROR == connect(s, (LPSOCKADDR)&server, sizeof(server)))
		{
			if (WSAEWOULDBLOCK == ::WSAGetLastError())
			{
				fd_set wset;
				FD_ZERO(&wset);
				FD_SET(s, &wset);
				TIMEVAL tv;
				tv.tv_sec = 5;
				tv.tv_usec = 0;
				int err = select(0, NULL, &wset, NULL, &tv);
				if (SOCKET_ERROR == err)
				{
					result = ::WSAGetLastError();
				}
				else if (0 == err)
				{
					result = WSAETIMEDOUT;
				}
				else if (!FD_ISSET(s, &wset))
				{
					result = -1;
				}
			}
		}
	} while (false);
	noblock = 0;
	ioctlsocket(s, FIONBIO, &noblock);
	return result;
}

int CNetOption::Send(SOCKET s, const char* lpBuffer, int len)
{
	int result = 0,	remain = len;
	while (remain > 0)
	{
		result = send(s, lpBuffer, remain, 0);
		if (result == SOCKET_ERROR)
		{
			return ::WSAGetLastError();
		}
		else if (result == 0)
		{
			return -2;		// socket内部缓冲区已满
		}
		remain -= result;
	}
	return 0;
}


int CNetOption::Recv(SOCKET s, char* lpBuffer, int len)
{
	int result = 0,	remain = len, zeroCount = 0;
	while (remain > 0)
	{
		result = recv(s, lpBuffer + (len - remain), remain, 0);
		if (SOCKET_ERROR == result)
		{
			return ::WSAGetLastError();
		}
		else if (0 == result && 0 == ::WSAGetLastError())
		{
			zeroCount++;
			if (zeroCount >= 5)
			{
				return -1;
			}
		}
		remain -= result;
	}
	return 0;
}

void CNetOption::CloseSocket(SOCKET& s)
{
	if (s != INVALID_SOCKET)
	{
		shutdown(s, SD_BOTH);
		closesocket(s);
		s = INVALID_SOCKET;
	}
}
