/*
CLientNet.cpp
*/
#include "Client.h"
#include <conio.h>
#include "../process_xml/ProcessXML.h"
#include "../leak_detector/leak_detector_c.h"

CClientNet::CClientNet()
{
	
}
int CClientNet::Connect( int port,const char* address )
{
	int rlt = 0;
	printf("Connect:\naddress:%c\t|port:%d\n", address, port);
	//用于记录错误信息并输出
	int iErrMsg;
	//启动WinSock
	WSAData wsaData;
	iErrMsg = WSAStartup(MAKEWORD(1,1),&wsaData);
	if (iErrMsg != NO_ERROR)
		//有错误
	{
		printf("client: failed with wsaStartup error : %d\n",iErrMsg);

		rlt = 1;
		return rlt;
	}

	//创建Socket
	if (NULL != m_sock)
	{
		closesocket(m_sock);
		m_sock = NULL;
	}

	m_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (m_sock == INVALID_SOCKET)
		//创建Socket失败
	{
		printf("client: socket failed with error : %d\n",WSAGetLastError());

		rlt = 2;
		return rlt;
	}

	//目标服务器数据
	sockaddr_in sockaddrServer;
	sockaddrServer.sin_family = AF_INET;
	sockaddrServer.sin_port = htons(port);
	//sockaddrServer.sin_port = port;
	sockaddrServer.sin_addr.s_addr = inet_addr(address);

	//连接,sock与目标服务器连接
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

	do 
	{
		if (NULL != m_send_head_buf) {
			printf("client: send data\n");
			//iErrMsg = send(m_sock, m_send_head_buf, max(strlen(m_send_head_buf), m_send_buf_len), 0);
			//while (TRUE) {
				SendMegHead *p = (SendMegHead *)m_send_head_buf;
				iErrMsg = SendN(m_sock, m_send_head_buf, m_send_buf_len);
				iErrMsg = SendN(m_sock, m_send_xml, p->xml_len);
				if (iErrMsg < 0)
					//发送失败
				{
					printf("client: send msg failed with error : %d\n",iErrMsg);

					rlt = 1;
					return rlt;
				}
			/*	Sleep(10);
			}*/
		}
		
		//消息处理
		do
		{
			//接收数据
			memset(buf,0,sizeof(buf));
			rval = RecvN(m_sock, buf, sizeof(RecvMegHead));
			RecvMegHead *p = (RecvMegHead *)buf;
			rval = RecvN(m_sock, buf, p->xml_len);
			ProcessXML xml_printer;
			printf("-------------Print XMl buffer----------------\n");
			xml_printer.PrintXmlBuffer(buf);
			break;

			printf("-------------Recv video buffer---------------\n");
			FILE *pf = NULL;
			int recv_file_size = 10 * 1024 * 1024;
			errno_t err = _wfopen_s(&pf, L"shipin.dat", L"w+b");
			if (0 != err) {
				break;
			}
			/*frame_id=*((int *)(buffer+8));
			frame_time=*((int *)(buffer+12));
			frame_len = datalenth-head_len-4;
			m_NetSpeed += frame_len;*/

			int data_len = 0, head_len = 0;
			int frame_id = 0, frame_time = 0, frame_len = 0, one_recv_len = 0;
			const int max_recv_file_len = 1448;
			char recv_file[max_recv_file_len] = { '\0' };

			/*int i = 0;
			while (1)
			{
			if (i > 10) {
			break;
			}
			rval = RecvN(m_sock, recv_file, 1448);
			fwrite(recv_file, max_recv_file_len, 1, pf);
			}

			if (NULL != pf) {
			fclose(pf);
			pf = NULL;
			}*/

			while (1)
			{
				rval = RecvN(m_sock, (char *)&data_len, sizeof(int));
				if (SOCKET_ERROR == rval) {
					printf("file len error\n");
					break;
				}
				rval = RecvN(m_sock, recv_file, /*data_len*/min(max_recv_file_len, data_len));
				if (SOCKET_ERROR == rval) {
					printf("-------------Recv video buffer error---------------\n");
					break;
				}
				head_len = *((int *)recv_file);
				if (SPFS != *((int *)(recv_file + 4))) {
					printf("recv video buffer SPFS error!\n");
					break;
				}
				frame_id = *((int *)(recv_file+8));
				frame_time = *((int *)(recv_file + 12));
				frame_len = data_len - head_len;
				if (data_len > max_recv_file_len) {
					fwrite(recv_file + head_len, max_recv_file_len - head_len, 1, pf);
					frame_len = frame_len - max_recv_file_len + head_len;
					while (TRUE) {
						memset(recv_file, '\0', max_recv_file_len);
						rval = RecvN(m_sock, recv_file, min(max_recv_file_len, frame_len));
						fwrite(recv_file, min(max_recv_file_len, frame_len), 1, pf);
						frame_len -= min(max_recv_file_len, frame_len);
						if (0 >= frame_len) {
							break;
						}
					}
				} else {
					if (NULL != buf || 0 != buf[0]) {
						fwrite(recv_file + head_len, frame_len, 1, pf);
						recv_file_size -= frame_len;
						if (0 >= recv_file_size) {
							break;
						}
					}
				}
			}
			if (NULL != pf) {
				fclose(pf);
				pf = NULL;
			}
			printf("-------------Recv video buffer end---------------\n");
			break;
			

			if (rval == SOCKET_ERROR) {
				//这应该是个异常，当客户端没有调用closeSocket就直接退出游戏的时候，将会进入这里
				printf("client: recv socket error\n");
				break;
			}
			if (rval == 0) {
				//recv返回0表示正常退出
				printf("client: ending connection");
				break;
			} else {
				//显示接收到的数据
				printf("client: recv \n%s\n",buf);
				getch();
				if (NULL != m_send_head_buf) {
					SendMegHead *p = (SendMegHead *)m_send_head_buf;
					iErrMsg = SendN(m_sock, m_send_head_buf, m_send_buf_len);
					iErrMsg = SendN(m_sock, m_send_xml, p->xml_len);
					if (iErrMsg < 0)
						//发送失败
					{
						printf("client: send msg failed with error : %d\n",iErrMsg);
					}
				}
			}

		}while(rval != 0);

		//关闭对应Accept的socket
		closesocket(m_sock);
		break;

	} while (1);


	return rlt;
}


int CClientNet::SendMsg(const char* msg,int len)
{
	int rlt = 0;

	int iErrMsg = 0;

	//发送消息，指定sock发送消息
	iErrMsg = send(m_sock, msg, len, 0);
	if (iErrMsg < 0)
		//发送失败
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

/*功能：发送定长字节的数据包*/
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
void client_main(char *p_send_head, int send_buf_size, char *p_xml, char *p_data)
{
	CClientNet client;

	client.m_send_head_buf = p_send_head;
	client.m_send_buf_len = send_buf_size;
	client.m_send_xml = p_xml;
	client.m_send_data = p_data;
	//char *p = (char *)malloc(100);
	//SendVideoReqData *send_data = (SendVideoReqData *)client.m_send_head_buf;
	//连接到127.0.0.1（即本地）,端口号为8888的服务端

	int connect_flag = client.Connect(19531, "192.168.1.16");
	//int connect_flag = client.Connect(19531, "127.0.0.1");
	if (0 != connect_flag) {
		return;
	}
	
	client.Run();

	//关闭socket
	printf("close\n");
	client.Close();
}