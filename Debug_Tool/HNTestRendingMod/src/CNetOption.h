#ifndef _CNET_OPTION_H_
#define _CNET_OPTION_H_

#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")

// 方法返回值
#define IPVA_DTC_UNDEFINE					-1			// 未定义返回值
#define IPVA_DTC_OK							0			// 执行成功
#define IPVA_DTC_ERR_NET					1			// 网络异常
#define IPVA_DTC_ERR_CONN_CLOSE				2			// 连接被断开
#define IPVA_DTC_ERR_CHECKSUM				3			// 校验和出错
#define IPVA_DTC_ERR_OP_FAILED				4			// 回复包中返回码不成功
#define IPVA_DTC_ERR_BT_FAILED				5			// 启动线程失败
#define IPVA_DTC_ERR_PACKAGE_LEN			6			// 数据包长度异常
#define IPVA_DTC_ERR_ALGO_OP_FAILED			7			// 算法配置回应包返回码不成功
#define IPVA_DTC_ERR_ALGO_PACKAGE_LEN		8			// 算法配置回应包长度异常
#define IPVA_DTC_ERR_ALGO_PARAM_CONV_FAILED	9			// 算法配置转换失败
#define IPVA_DTC_NO_FRAME					0x10		//超时未接到视频数据

// 连接超时设定
#define CTRLSNDTIMEOUT						3000		// 数据链路控制连接发送超时值
#define CTRLRCVTIMEOUT						3000		// 数据链路控制连接接收超时值
#define DATASNDTIMEOUT						0			// 数据链路数据连接发送超时值
#define DATARCVTIMEOUT						3000		// 数据链路数据连接接收超时值
#define THREADEXITTIMEOUT					2000		// 线程退出超时值


class CNetOption {

public:
	CNetOption();
	~CNetOption();

	/**
	* @brief	初始化套接字对象
	* @details	当sndTimeo值小于等于0时，不执行设置发送超时，当rcvTimeo值小于等于0时，不执行设置接收超时
	* @param[in]	s，需要设置的SOCKET
	* @param[in]	sndTimeo，发送超时值，单位为毫秒
	* @param[in]	rcvTimeo，接收超时值，单位为毫秒
	* @return	成功返回0，失败返回Windows Sockets Error Codes
	*/
	static int InitSocket(SOCKET& s, int sndTimeo, int rcvTimeo);

	/**
	* @brief	连接指定服务器
	* @details	连接IP指定的服务器的指定端口
	* @param[in]	s，需要进行连接的套接字
	* @param[in]	lpAddr，结尾为'\0'的字符串，IP地址
	* @param[in]	port，连接的端口
	* @return	成功返回0，失败返回Windows Sockets Error Codes，出现未知错误则返回-1
	*/
	static int Connect(SOCKET s, const char* lpAddr, unsigned short port);

	/**
	* @brief	发送数据
	* @param[in]	s，需要执行发送操作的套接字
	* @param[in]	lpBuffer，指向需要发送的数据缓冲区
	* @param[in]	len，需要发送的数据长度
	* @return	成功返回0，失败返回Windows Sockets Error Codes
	*/
	static int Send(SOCKET s, const char* lpBuffer, int len);
	
	/**
	* @brief	接收数据
	* @param[in]	s，需要执行接收操作的套接字
	* @param[in]	lpBuffer，指向需要保存接收数据的缓冲区
	* @param[in]	len，需要接收的数据长度
	* @return	成功返回0，失败返回Windows Sockets Error Codes，连接已被对方断开则返回-1
	*/
	static int Recv(SOCKET s, char* lpBuffer, int len);

	/**
	* @brief	关闭套接字
	* @details	关闭套接字的同时把参数置为INVALID_SOCKET
	* @param[in,out]	s，需要关闭的套接字
	*/
	void CloseSocket(SOCKET& s);

private:
	static int CreateSocket(SOCKET& s);
	static int SetTimeout(SOCKET s, int sndTimeo, int rcvTimeo);

//////////////////////////
public:

private:

};




#endif