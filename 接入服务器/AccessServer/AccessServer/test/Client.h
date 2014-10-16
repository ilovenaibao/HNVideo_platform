#ifndef CLIENT_H
#define CLIENT_H

/*
ClinetNet.h：
用于表示一个客户的网络操作
*/

#include <stdio.h>
#include<windows.h>

#pragma comment(lib, "Ws2_32.lib")

class CClientNet
{
public:
	//连接上指定服务器
	int Connect(int port,const char* address);
	int Run();
	int SendN(SOCKET s, const char* buf, int len);
	int RecvN(SOCKET s, char* buf, int len);

	//发送信息
	int SendMsg(const char* msg,int len);
	//关闭
	void Close();

private:
	SOCKET m_sock;

public:
	char *m_send_head_buf;
	int m_send_buf_len;
	char *m_send_xml;
	char *m_send_data;

};

void client_main(char *p_send_head, int send_buf_size, char *p_xml, char *p_data, void* lpParameter);


#endif