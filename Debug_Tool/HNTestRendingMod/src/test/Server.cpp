/*
ServerNet.cpp
*/
#include "Server.h"
#include "../../../../video_platform/HNCommon/process_xml/HNProtocol.h"
#include "../../../../video_platform/HNCommon/process_xml/ProcessXML.h"

int CServerNet::Init( const char* address,int port )
{
	int rlt = 0;

	//���ڼ�¼������Ϣ�������
	int iErrorMsg;

	//��ʼ��WinSock
	WSAData wsaData;
	iErrorMsg = WSAStartup(MAKEWORD(1,1),&wsaData);

	if (iErrorMsg != NO_ERROR)
	{
		//��ʼ��WinSockʧ��
		printf("wsastartup failed with error : %d\n",iErrorMsg);

		rlt = 1;
		return rlt;
	}

	//���������Socket
	m_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)

	{
		//����Socket�쳣
		printf("socket failed with error : %d\n",WSAGetLastError());

		rlt = 2;
		return rlt;
	}

	//������Ϣ
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = port;
	serverAddr.sin_addr.s_addr = inet_addr(address);

	//��
	iErrorMsg = bind(m_sock,(sockaddr*)&serverAddr,sizeof(serverAddr));
	if (iErrorMsg < 0)
	{
		//��ʧ��
		printf("bind failed with error : %d\n",iErrorMsg);
		rlt = 3;
		return rlt;
	}



	return rlt;
}

void CServerNet::Run()
{
	//��������
	listen(m_sock,5);

	sockaddr_in tcpAddr;
	int len = sizeof(sockaddr);
	SOCKET newSocket;
	char buf[1024];
	int rval;

	do 
	{
		//������Ϣ
		printf("server: wait client connect...\n");
		newSocket = accept(m_sock, (sockaddr*)&tcpAddr, &len);
		printf("server: client connected\n");

		if (newSocket == INVALID_SOCKET)
		{
			//�ǿ���socket
		}
		else
		{
			//��Ϣ����
			do
			{
				//��������
				memset(buf,0,sizeof(buf));
				rval = recv(newSocket, buf, 1024, 0);
				SendMegHead *p = (SendMegHead *)buf;
				char xml_buf[MAX_OUTPUT_BUFFER_] = { '\0' };
				rval = RecvN(newSocket, xml_buf, p->xml_len);


				if (rval == SOCKET_ERROR) {
					//��Ӧ���Ǹ��쳣�����ͻ���û�е���closeSocket��ֱ���˳���Ϸ��ʱ�򣬽����������
					printf("server: client exit, socket error\n");
					break;
				}
				if (rval == 0) {
					//recv����0��ʾ�����˳�
					printf("server: ending connection");
					break;
				} else {
					//��ʾ���յ�������
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
							//����ʧ��
						{
							printf("server: send msg failed with error : %d\n",iErrMsg);
							break;
						}
						Sleep(2000);
					}
				}

			}while(rval != 0);

			//�رն�ӦAccept��socket
			closesocket(newSocket);
		}

	} while (1);

	//�ر������Socket
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