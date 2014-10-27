#ifndef CLIENT_H
#define CLIENT_H

#include <vector>


/*
ClinetNet.h：
用于表示一个客户的网络操作
*/

#include <stdio.h>
//#include<windows.h>
#include <WinSock2.h>
#include "../RenderingMod/ITCPModelDataProc.h"

#pragma comment(lib, "Ws2_32.lib")


#define SPFS 0x53465053


class CClientNet
{
public:
	CClientNet();
	//~CClientNet();
	//连接上指定服务器
	int Connect(SOCKET &s, int port, const char* address);

	int Run();
	int SendN(SOCKET s, const char* buf, int len);
	int RecvN(SOCKET s, char* buf, int len);

	//发送信息
	int SendMsg(const char* msg,int len);
	//关闭
	void Close();

	static unsigned int __stdcall RunThreadPlayVideo(void *obj);
	static unsigned int __stdcall RunThreadPlayVideo_YUV(void *obj);

private:
	

public:
	SOCKET			m_sock;
	SOCKET			sCtrl;							// 数据链路控制连接的套接字
	SOCKET			sData;							// 数据链路数据连接的套接字

	unsigned short	nMainVersion, nSubVersion;		// 服务端协议主版本号与协议次版本号
	char			extRecvBuffer[16];				// 缓冲控制链路回复包除固定头部分
	bool			bRegister;						// 是否成功注册通道
	unsigned short	nPort;							// 数据传输使用的端口，保存TCP模式、UDP单播模式与UDP组播模式中使用的端口
	unsigned int	nTokenID;						// 令牌ID
	HANDLE			hDataRcvThread;					// 数据接收线程句柄
	unsigned int	packNo;							// 包序号

	char *m_send_head_buf;
	int m_send_buf_len;
	char *m_send_xml;
	char *m_send_data;

	HWND hDisplayWnd;
	ITCPModelDataProc *processModule;							// 视频播放模块

	/*char *video_buf;
	int max_video_buf;
	int video_buf_len;*/

};

void client_main(char *p_send_head, int send_buf_size, char *p_xml, char *p_data);


#endif