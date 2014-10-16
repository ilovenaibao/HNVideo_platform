#ifndef SERVER_H
#define SERVER_H

#pragma once

/*
ServerNet.h
用于表示服务端的网络操作

*/

#include <stdio.h>
#include <winsock.h>

#pragma comment (lib,"ws2_32.lib")

class CServerNet
{
public:

	//初始化服务器,返回0表示成功
	int Init(const char* address,int port);
	//更新数据
	void Run();
	int SendN(SOCKET s, const char* buf, int len);
	int RecvN(SOCKET s, char* buf, int len);

private:
	SOCKET m_sock;

public:
	char *m_send_buf;
	int m_send_buf_len;

};

int server_main(char *p_send_head, int send_buf_size, char *p_xml, char *p_data);


#endif