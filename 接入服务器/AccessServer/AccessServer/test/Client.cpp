/*
CLientNet.cpp
*/
#include "Client.h"
#include <conio.h>
#include "../leak_detector/leak_detector_c.h"
#include "../HNProtocol.h"
#include "../ProcessXML.h"

int CClientNet::Connect( int port,const char* address )
{
	int rlt = 0;
	printf("Connect:\naddress:%c\t|port:%d\n");
	//���ڼ�¼������Ϣ�����
	int iErrMsg;
	//����WinSock
	WSAData wsaData;
	iErrMsg = WSAStartup(MAKEWORD(1,1),&wsaData);
	if (iErrMsg != NO_ERROR)
		//�д���
	{
		printf("client: failed with wsaStartup error : %d\n",iErrMsg);

		rlt = 1;
		return rlt;
	}

	//����Socket
	if (NULL != m_sock)
	{
		closesocket(m_sock);
		m_sock = NULL;
	}
	m_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)
		//����Socketʧ��
	{
		printf("client: socket failed with error : %d\n",WSAGetLastError());

		rlt = 2;
		return rlt;
	}

	//Ŀ�����������
	sockaddr_in sockaddrServer;
	sockaddrServer.sin_family = AF_INET;
	//sockaddrServer.sin_port = htons(port);
	sockaddrServer.sin_port = port;
	sockaddrServer.sin_addr.s_addr = inet_addr(address);

	//����,sock��Ŀ�����������
	iErrMsg = connect(m_sock,(sockaddr*)&sockaddrServer,sizeof(sockaddrServer));
	if (iErrMsg < 0)
	{
		printf("client: connect failed with error : %d\n",iErrMsg);

		rlt = 3;
		return rlt;
	}

	/*closesocket(m_sock);
	rlt = 3;*/
	return rlt;
}

int CClientNet::Run()
{
	int rlt = 0;
	int iErrMsg = 0;
	char buf[4096];
	int rval;
	int keep_connect_or_send_data = 0;

	do 
	{
		if (NULL != m_send_head_buf) {
			printf("client: send data\n");
			//iErrMsg = send(m_sock, m_send_head_buf, max(strlen(m_send_head_buf), m_send_buf_len), 0);
			SendMegHead *p = (SendMegHead *)m_send_head_buf;
			iErrMsg = SendN(m_sock, m_send_head_buf, m_send_buf_len);
			iErrMsg = SendN(m_sock, m_send_xml, p->xml_len);
			if (iErrMsg < 0)
				//����ʧ��
			{
				printf("client: send msg failed with error : %d\n",iErrMsg);

				rlt = 1;
				return rlt;
			}
		}
		
		//��Ϣ����
		do
		{
			//��������
			memset(buf,0, sizeof(buf));
			rval = RecvN(m_sock, buf, sizeof(RecvMegHead));
			RecvMegHead *p_recv_head = (RecvMegHead *)buf;
			char xml_buf[MAX_OUTPUT_BUFFER_] = { '\0' };
			rval = RecvN(m_sock, xml_buf, p_recv_head->xml_len);
			ProcessXML process_xml;
			XmlElement *client_recv;
			client_recv = process_xml.RegAssign2AccessKeepConnectM();
			int flag = process_xml.GetElementInXmlBuffer(client_recv, xml_buf);
			if (1 == flag) {
				process_xml.FreeXmlElement(client_recv);
				continue;
			}
			process_xml.FreeXmlElement(client_recv);
			client_recv = process_xml.RegAssign2AccessSendVideoM();
			flag = process_xml.GetElementInXmlBuffer(client_recv, xml_buf);
			if (1 == flag) {
				// send video data
				
			}
			process_xml.FreeXmlElement(client_recv);

			if (rval == SOCKET_ERROR) {
				//��Ӧ���Ǹ��쳣�����ͻ���û�е���closeSocket�������������
				printf("client: recv socket error\n");
				break;
			}
			if (rval == 0) {
				//recv����0��ʾ�����˳�
				printf("client: ending connection");
				break;
			} else {
				//��ʾ���յ�������
				printf("client: recv \n%s\n",buf);
			}

			SendMegHead *p = (SendMegHead *)m_send_head_buf;
			iErrMsg = SendN(m_sock, m_send_head_buf, m_send_buf_len);
			iErrMsg = SendN(m_sock, m_send_xml, p->xml_len);
			if (iErrMsg < 0)
				//����ʧ��
			{
				printf("client: send msg failed with error : %d\n",iErrMsg);

				rlt = 1;
				return rlt;
			}

		}while(rval != 0);

		//�رն�ӦAccept��socket
		closesocket(m_sock);
		break;

	} while (1);


	return rlt;
}


int CClientNet::SendMsg(const char* msg,int len)
{
	int rlt = 0;

	int iErrMsg = 0;

	//������Ϣ��ָ��sock������Ϣ
	iErrMsg = send(m_sock, msg, len, 0);
	if (iErrMsg < 0)
		//����ʧ��
	{
		printf("send msg failed with error : %d\n",iErrMsg);

		rlt = 1;
		return rlt;
	}

	return rlt;
}

void CClientNet::Close()
{
	closesocket(m_sock);
	m_sock = NULL;
}

/*���ܣ����Ͷ����ֽڵ����ݰ�*/
int CClientNet::SendN(SOCKET s, const char* buf, int len)
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


int CClientNet::RecvN(SOCKET s, char* buf, int len)
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
void client_main(char *p_send_head, int send_buf_size, char *p_xml, char *p_data, void* lpParameter)
{
	CClientNet client;

	client.m_send_head_buf = p_send_head;
	client.m_send_buf_len = send_buf_size;
	client.m_send_xml = p_xml;
	client.m_send_data = p_data;
	//char *p = (char *)malloc(100);
	//SendVideoReqData *send_data = (SendVideoReqData *)client.m_send_head_buf;
	//���ӵ�127.0.0.1�������أ�,�˿ں�Ϊ8888�ķ����

	//int connect_flag = client.Connect(19531, "192.168.1.16");
	int connect_flag = client.Connect(19531, "127.0.0.1");
	if (0 != connect_flag) {
		return;
	}
	
	client.Run();

	////�ر�socket
	//printf("close\n");
	//client.Close();
}