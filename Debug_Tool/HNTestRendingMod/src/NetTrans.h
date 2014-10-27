#ifndef _IPVA_DTC_NET_TRANS_H_
#define _IPVA_DTC_NET_TRANS_H_

#include "CDataLink.h"
#include "CCtrlLink.h"
#include "INetTransmission.h"

class CNetTrans : public INetTransmission
{
public:
	CNetTrans(void);
	~CNetTrans(void);

	/**
	* @brief	初始化
	*/
	void Init(void);

	/**
	* @brief	清除占用的资源
	*/
	void Clear(void);

	/**
	* @brief	同时连接服务器的控制端与数据端
	* @param[in]	hWnd，处理心跳包消息的窗口句柄
	* @param[in]	lpAddr，服务器地址
	* @param[in]	nChnl，连接的通道号
	* @param[in]	nMode，数据连接使用的模式
	* @param[in]	proc，数据连接使用的数据处理模块
	* @param[out]	nCode，错误码
	* @return	方法执行结果
	* @remark	方法执行的详细结果需要依据返回值与错误码判断
	*/
	int Connect(HWND hWnd, LPCSTR lpAddr, unsigned char nChnl, unsigned char nMode, void* procMod, int& nCode);

	/**
	* @brief	连接服务器数据端
	* @param[in]	lpAddr，服务器地址
	* @param[in]	nChnl，连接的通道号
	* @param[in]	nMode，数据连接使用的模式
	* @param[in]	proc，数据连接使用的数据处理模块
	* @param[out]	nCode，错误码
	* @return	方法执行结果
	* @remark	方法执行的详细结果需要依据返回值与错误码判断
	*/
	int ConnectDataLink(LPCSTR lpAddr, unsigned char nChnl, unsigned char nMode, void* procMod, int& nCode);

	/**
	* @brief	连接服务器控制端
	* @param[in]	hWnd，接收心跳包失败消息的窗口
	* @param[in]	lpAddr，服务器地址
	* @param[in]	nChnl，连接的通道号
	* @param[out]	nCode，错误码
	* @return	方法执行结果
	* @remark	方法执行的详细结果需要依据返回值与错误码判断
	*/
	int ConnectCtrlLink(HWND hWnd, LPCSTR lpAddr, unsigned char nChnl, int& nCode);

	void DisConnectCtrlLink(void);

	void DisConnectDataLink(void);

	/**
	* @brief	断开服务器
	*/
	void DisConnect(void);

	/**
	* @brief	注册控制权限
	* @param[out]	nCode，错误码
	* @return	方法执行结果
	* @remark	方法执行的详细结果需要依据返回值与错误码判断
	*/
	int UpdatePrivileges(int& nCode);

	/**
	* @brief	注销控制权限
	* @param[out]	nCode，错误码
	* @return	方法执行结果
	* @remark	方法执行的详细结果需要依据返回值与错误码判断
	*/
	int CancelPrivileges(int& nCode);

	/**
	* @brief	获取服务器协议版本
	* @param[out]	nMainVer，服务器协议主版本
	* @param[out]	nSubVer，服务器协议次版本
	* @param[out]	nCode，错误码
	* @return	方法执行结果
	* @remark	方法执行的详细结果需要依据返回值与错误码判断
	*/
	int GetServerProtocol(unsigned short& nMainVer, unsigned short& nSubVer, int& nCode);

	/**
	* @brief	获取服务器版本
	* @param[out]	sVersion，服务器版本
	* @param[out]	nCode，错误码
	* @return	方法执行结果
	* @remark	方法执行的详细结果需要依据返回值与错误码判断
	*/
	int GetServerVersion(char* sVersion, int& nCode);

	/**
	* @brief	设置接收网络异常通知的窗口
	* @param[in]	ctrlLink，控制链路发生异常需要通知的窗口句柄
	* @param[in]	dataLink，数据链路发生异常需要通知的窗口句柄
	*/
	void SetNetErrorNotify(HWND ctrlLink, HWND dataLink);

	/**
	* @brief	算法配置命令
	* @param[in]	lpReq，需要传输的命令数据
	* @param[in]	nReqLen，需要传输的命令数据长度
	* @param[out]	lpResp，保存着指向命令回复包中的负载数据指针，在下一次调用本方法前有效
	* @param[out]	nRespLen，命令回包中负载数据的长度
	* @param[out]	nCode，错误码
	* @return	方法执行结果
	* @remark	方法执行的详细结果需要依据返回值与错误码判断
	*/
	virtual int ExecCmd(LPBYTE lpReq, unsigned int nReqLen, LPBYTE& lpResp, unsigned int& nRespLen, int& nCode);

public:
	CCtrlLink ctrlLink;		// 控制链路对象
	CDataLink dataLink;		// 数据链路对象
};

#endif
