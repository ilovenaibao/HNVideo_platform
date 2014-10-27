/*
ServerNet.cpp
*/
#include "Server.h"
#include "../../../../video_platform/HNCommon/process_xml/HNProtocol.h"
#include "../../../../video_platform/HNCommon/process_xml/ProcessXML.h"

int CServerNet::Init( const char* address,int port )
{
	int rlt = 0;

	//用于记录错误信息，并输出
	int iErrorMsg;

	//初始化WinSock
	WSAData wsaData;
	iErrorMsg = WSAStartup(MAKEWORD(1,1),&wsaData);

	if (iErrorMsg != NO_ERROR)
	{
		//初始化WinSock失败
		printf("wsastartup failed with error : %d\n",iErrorMsg);

		rlt = 1;
		return rlt;
	}

	//创建服务端Socket
	m_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)

	{
		//创建Socket异常
		printf("socket failed with error : %d\n",WSAGetLastError());

		rlt = 2;
		return rlt;
	}

	//声明信息
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = port;
	serverAddr.sin_addr.s_addr = inet_addr(address);

	//绑定
	iErrorMsg = bind(m_sock,(sockaddr*)&serverAddr,sizeof(serverAddr));
	if (iErrorMsg < 0)
	{
		//绑定失败
		printf("bind failed with error : %d\n",iErrorMsg);
		rlt = 3;
		return rlt;
	}



	return rlt;
}

void CServerNet::Run()
{
	//公开连接
	listen(m_sock,5);

	sockaddr_in tcpAddr;
	int len = sizeof(sockaddr);
	SOCKET newSocket;
	char buf[1024];
	int rval;

	do 
	{
		//接收信息
		printf("server: wait client connect...\n");
		newSocket = accept(m_sock, (sockaddr*)&tcpAddr, &len);
		printf("server: client connected\n");

		if (newSocket == INVALID_SOCKET)
		{
			//非可用socket
		}
		else
		{
			//消息处理
			do
			{
				//接收数据
				memset(buf,0,sizeof(buf));
				rval = recv(newSocket, buf, 1024, 0);
				SendMegHead *p = (SendMegHead *)buf;
				char xml_buf[MAX_OUTPUT_BUFFER_] = { '\0' };
				rval = RecvN(newSocket, xml_buf, p->xml_len);


				if (rval == SOCKET_ERROR) {
					//这应该是个异常，当客户端没有调用closeSocket就直接退出游戏的时候，将会进入这里
					printf("server: client exit, socket error\n");
					break;
				}
				if (rval == 0) {
					//recv返回0表示正常退出
					printf("server: ending connection");
					break;
				} else {
					//显示接收到的数据
					printf("server: recv\n%s\n",buf);

					int iErrMsg = 0;
					if (NULL != m_send_buf) {
						// keep connect
						RecvMegHead *p = (RecvMegHead *)m_send_buf;
						iErrMsg = SendN(newSocket, m_send_buf, max(strlen(m_send_buf), m_send_buf_len));
						iErrMsg = SendN(newSocket, m_xml_buf, p->xml_len);
						Sleep(2000);
						//// send video 
						//ProcessXML process_xml;
						//XML_Char p_xml_buffer[MAX_OUTPUT_BUFFER_] = { '\0' };
						//XmlElement *client_recv = process_xml.RegAssign2AccessSendVideo("session", "send");
						//process_xml.CreateXmlBuffer(p_xml_buffer, client_recv);
						//RecvMegHead send_data;
						//send_data.xml_len = strlen(p_xml_buffer);
						//send_data.version = 100;
						//send_data.cmd_code = CMD_CODE_CREATE_CONNECT;
						//send_data.success_flag = 0;
						//send_data.data_len = sizeof(SendMegHead) + send_data.xml_len;
						//iErrMsg = SendN(newSocket, (char *)&send_data, sizeof(RecvMegHead));
						//iErrMsg = SendN(newSocket, p_xml_buffer, send_data.xml_len);

						if (iErrMsg < 0)
							//发送失败
						{
							printf("server: send msg failed with error : %d\n",iErrMsg);
							break;
						}
						Sleep(2000);
					}
				}

			}while(rval != 0);

			//关闭对应Accept的socket
			closesocket(newSocket);
		}

	} while (1);

	//关闭自身的Socket
	closesocket(m_sock);
}

int CServerNet::SendN(SOCKET s, const char* buf, int len)
{
	int cut = len;
	int cutlen = 0;

	while(cut > 0)
	{
		cutlen = send(s, (const char *)buf, cut, 0 );
		if(cutlen<0)
		{
			return -1;
		}
		if(cutlen == 0)
		{
			return len-cut;
		}

		buf += cutlen;
		cut -= cutlen;
	}
	return len;
}


int CServerNet::RecvN(SOCKET s, char* buf, int len)
{
	int cut = len;
	int cutlen = 0;

	while(cut > 0)
	{
		cutlen = recv(s, (char *)buf, cut, 0 );
		if(cutlen<0)
		{
			return -1;
		}
		if(cutlen == 0)
		{
			return len-cut;
		}

		buf += cutlen;
		cut -= cutlen;
	}
	return len;
}


// test main
int server_main(char *p_send_head, int send_buf_size, char *p_xml, char *p_data)
{
	CServerNet serverNet;
	int iRlt = serverNet.Init("127.0.0.1", 19531);
	if (iRlt == 0)
	{
		printf("init ok...\n");
		serverNet.m_send_buf = p_send_head;
		serverNet.m_send_buf_len = send_buf_size;
		serverNet.m_xml_buf = p_xml;
		serverNet.Run();
	} else {
		printf("serverNet init failed with error : %d\n",iRlt);
	}
	system("pause");

	return 1;
}