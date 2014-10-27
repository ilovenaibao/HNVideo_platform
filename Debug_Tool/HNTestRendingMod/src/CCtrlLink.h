/**
 * @file     CCtrlLink.h
 * @brief    数据连接&控制连接
 * @author   guch
 * @date     2014-10-21
 *
 * @if copyright
 *
 * Copyright (C) 2014-201* guch
 *
 * 用于HN远程调试工具中控制链路
 *
 * @endif
 *
 */

#ifndef _CCTRL_LINK_H_
#define _CCTRL_LINK_H_

#include "CNetOption.h"
#include "hv_types.h"

#define CONTROLPORT					20138			// 控制链路端口
#define CONTROLPORT_AGENT_SERVER	19531			// 控制链路

/**
* @brief	命令字定义
*/
#define	IPVA_DT_CTRL_LOGIN			0x0000			// 登录命令
#define	IPVA_DT_CTRL_REG			0x0001			// 注册目标通道
#define IPVA_DT_CTRL_UNREG			0x0002			// 注销目标通道
// 0x0003-0x00FF	系统保留命令

#define IPVA_DT_CTRL_HEARTBEAT		0x0100			// 心跳命令
#define IPVA_DT_CTRL_GETPROTVER		0x0101			// 获取服务端协议版本命令
#define	IPVA_DT_CTRL_GETSRVVER		0x0102			// 获取服务端程序版本命令
#define IPVA_DT_CTRL_REGCTRLPRIV	0x0103			// 请求控制权限
#define IPVA_DT_CTRL_UNREGCTRLPRIV	0x0104			// 注销控制权限
#define IPVA_DT_CTRL_ALLOCALGO		0x0105			// IPVA算法配置和计数器配置请求
// 0x0106-0xFFFF	协议保留命令

/**
* @brief	返回码定义
*/
#define S_CTRL_OK							0x0000	// 操作成功
#define S_ERR_CTRL_CONNECT_TIMEOUT			0x0001	// 连接超时
#define S_ERR_CTRL_ILLEGALACCESS			0x0002	// 操作失败，非法访问
#define S_ERR_CTRL_USERORPWD_INCORRECT		0x0003	// 登录失败，用户名或密码错误
#define S_ERR_CTRL_PROT_NOTSUPP				0x0004	// 登录失败，协议版本不被支持
#define S_ERR_CTRL_CHNL_NOTSUPP				0x0005	// 注册失败，目标通道不被支持
#define S_ERR_CTRL_CHNL_CONNECT_TOOMUCH		0x0006	// 注册失败，目标通道连接数过多
#define S_ERR_CTRL_CHNL_CTRL_USER_TOOMUCH	0x0007	// 权限提升失败，控制权限用户过多
#define S_ERR_CTRL_CHNL_NOT_CTRL_USER		0x0008	// 权限注销失败，非控制权限用户
#define S_ERR_CTRL_CHNL_PERMISSION_DENIED	0x0009	// 操作失败，权限不足
#define S_ERR_CTRL_LOGOFF_FAILED			0x000A	// 注销失败
#define S_ERR_CTRL_CMD_UNSUPPORT			0x000B	// 命令不支持


/**
* @brief	基础请求包
*/
typedef struct _reqpkt_t
{
	HV_U32		pkt_num;	// 包序号
	HV_U16		pkt_chksum;	// 包校验和
	HV_U16		pkt_type;	// 包命令字
	HV_U32		pkt_pldlen;	// 请求包负载的长度
}
reqpkt_t;

/**
* @brief	基础应答包
*/
typedef	struct _respkt_t
{
	HV_U32		pkt_num;	// 请求包序号
	HV_U16		pkt_chksum;	// 包校验和
	HV_U16		pkt_status; // 应答码
	HV_U32		pkt_pldlen;	// 应答包负载的长度
}
respkt_t;

/**
* @brief	登录命令
*/
typedef struct _reqpkt_login_t
{
	reqpkt_t	header;
	HV_U16		nCProtMainVer;	// 客户端协议主版本
	HV_U16		nCProtSubVer;	// 客户端协议次版本
	HV_S8		vUser[16];		// 用户名
	HV_S8		vPsw[64];		// 密码
}
reqpkt_login_t;
typedef respkt_t respkt_login_t;

/**
* @brief	注册目标通道
*/
typedef struct _reqpkt_register_t
{
	reqpkt_t	header;
	HV_U8		nChnl;			// 目标通道号
	HV_U8		nReserve[3];	// 保留字段
}
reqpkt_register_t;
typedef respkt_t respkt_register_t;

/**
* @brief	注销目标通道
*/
typedef reqpkt_t reqpkt_unregister_t;
typedef	respkt_t respkt_unregister_t;

/**
* @brief	获取服务端协议版本
*/
typedef reqpkt_t reqpkt_get_server_protocol_t;
typedef struct _respkt_get_server_protocol_t
{
	respkt_t	header;
	HV_U16		nProtMainVer;		// 协议主版本号
	HV_U16		nProtSubVer;		// 协议此版本号
}
respkt_get_server_protocol_t;

/**
* @brief	获取服务器版本
*/
typedef reqpkt_t reqpkt_get_server_version_t;
typedef struct _respkt_get_server_version_t
{
	respkt_t	header;
	HV_S8		vVersion[32];
}
respkt_get_server_version_t;

/**
* @brief	请求控制权限
*/
typedef reqpkt_t reqpkt_reg_ctrl_priv_t;
typedef struct _respkt_reg_ctrl_priv_t
{
	respkt_t	header;
	HV_U8		nReserve[4];
}
respkt_reg_ctrl_priv_t;

/**
* @brief	注销控制权限
*/
typedef reqpkt_t reqpkt_unreg_ctrl_priv_t;
typedef respkt_t respkt_unreg_ctrl_priv_t;

/**
* @brief	IPVA算法配置命令
*/
typedef struct _reqpkt_set_algo_params_t
{
	reqpkt_t	header;
	HV_U8		nReserve[4];
}
reqpkt_set_algo_params_t;
typedef respkt_t respkt_set_algo_params_t;


class CCtrlLink : public CNetOption {

public:
	CCtrlLink(void);
	~CCtrlLink(void);

	/**
	* @brief	与指定IP的设备建立连接
	* @details	此方法完成连接，登录，注册的功能
	* @param[in]	HWND，接收控制链路心跳包消息的窗口，为NULL则不发送心跳包消息
	* @param[in]	lpAddr，以'\0'结尾的IP字符串
	* @param[in]	nChnl，通道号
	* @param[out]	nCode，错误码
	* @return	返回码
	*/
	int Link(HWND hWnd, const char* lpAddr, unsigned char nChnl, int& nCode);

	/**
	* @brief	断开连接
	* @details	断开与设备连接
	*/
	void UnLink(void);

	/**
	* @brief	与指定IP的代理服务器建立连接
	* @details	此方法完成连接
	* @param[in]	HWND，接收控制链路心跳包消息的窗口，为NULL则不发送心跳包消息
	* @param[in]	lpAddr，以'\0'结尾的IP字符串
	* @param[in]	nChnl，通道号
	* @param[out]	nCode，错误码
	* @return	返回码
	*/
	int LinkAgentServer(HWND hWnd, const char* lpAddr, unsigned char nChnl, int& nCode);

	/**
	* @brief	获取服务端协议版本
	* @param[out]	nMainVer，服务端协议主版本
	* @param[out]	nSubVer，服务端协议次版本
	* @param[out]	nCode，错误码
	* @return	返回码
	*/
	int GetServerProtocol(unsigned short& nMainVer, unsigned short& nSubVer, int& nCode);

	/**
	* @brief	获取服务端程序版本
	* @param[out]	sVersion，服务端程序版本，调用者提供的缓冲区长度不小于32个字节
	* @param[out]	nCode，错误码
	* @return	返回码
	*/
	int GetServerVersion(char* sVersion, int& nCode);

	/**
	* @brief	请求控制权限
	* @param[out]	nCode，错误码
	* @return	成功返回IPVA_DTC_OK，失败返回对应的返回值，nCode根据返回值决定返回Windows Sockets Error Codes还是回复包中的返回码
	*/
	int UpdatePrivileges(int& nCode);

	/**
	* @brief	注销控制权限
	* @param[out]	nCode，错误码
	* @return	成功返回IPVA_DTC_OK，失败返回对应的返回值，nCode根据返回值决定返回Windows Sockets Error Codes还是回复包中的返回码
	*/
	int CancelPrivileges(int& nCode);

	/**
	* @brief	配置算法
	* @param[in]	lpData，指向需要作为负载的数据
	* @param[in]	nLen，数据长度
	* @param[out]	nRespDataLen，回复包中的负载数据长度
	* @param[out]	nCode，错误码
	* @return	返回码
	*/
	int AollocAlgo(LPBYTE lpData, unsigned int nLen, unsigned int& nRespDataLen, int& nCode);

	/**
	* @brief	获取回复中的负载数据
	* @return	类实例保存回复包中负载数据的缓冲区
	* @remark	本方法应在方法AollocAlgo调用后其他实例方法调用前进行调用
	*/
	LPBYTE GetRespData(void);

private:

	/**
	* @brief	登录
	* @param[out]	nCode，错误码
	* @return	成功返回IPVA_DTC_OK，失败返回对应的返回值，nCode根据返回值决定返回Windows Sockets Error Codes还是回复包中的返回码
	*/
	int Login(int& nCode);

	/**
	* @brief	注册目标通道
	* @param[in]	nChnl，通道号
	* @param[out]	nCode，错误码
	* @return	成功返回IPVA_DTC_OK，失败返回对应的返回值，nCode根据返回值决定返回Windows Sockets Error Codes还是回复包中的返回码
	*/
	int Register(unsigned char nChnl, int& nCode);

	/**
	* @brief	注销目标通道
	* @param[out]	nCode，错误码
	* @return	成功返回IPVA_DTC_OK，失败返回对应的返回值，nCode根据返回值决定返回Windows Sockets Error Codes还是回复包中的返回码
	*/
	int UnRegister(int& nCode);

	/**
	* @brief	执行控制命令
	*/
	int ExecCmd(const char* sndBuffer, int sndBufferLen, char* recvHeader, int& recvRemainLen, int& nCode);

	/**
	* @brief	填充reqpkt_get_server_protocol_t对象
	* @details	在填充的同时把本地字节序转换成网络字节序
	* @param[out]	req，需要填充的对象
	* @param[in]	packNo，包序号
	*/
	static void set_reqpkt_get_server_protocol_t(reqpkt_get_server_protocol_t& req, unsigned int packNo);

	/**
	* @brief	转换respkt_get_server_protocol_t对象
	* @details	把对象实例中的网络字节序转换为本地字节序
	* @param[in,out]	resp，需要转换的对象
	*/
	static void convert_respkt_get_server_protocol_t(respkt_get_server_protocol_t& resp);

	/**
	* @brief	转换respkt_t对象
	* @details	把对象实例中的网络字节序转换为本地字节序
	* @param[in,out]	resp，需要转换的对象
	*/
	static void convert_respkt_t(respkt_t& resp);

	/**
	* @brief	转换respkt_get_server_version_t对象
	* @details	把对象实例中的网络字节序转换为本地字节序
	* @param[in,out]	resp，需要转换的对象
	*/
	static void convert_respkt_get_server_version_t(respkt_get_server_version_t& resp);

	/**
	* @brief	填充reqpkt_reg_ctrl_priv_t对象
	* @details	在填充的同时把本地字节序转换成网络字节序
	* @param[out]	req，需要填充的对象
	* @param[in]	packNo，包序号
	*/
	static void set_reqpkt_reg_ctrl_priv_t(reqpkt_reg_ctrl_priv_t& req, unsigned int packNo);

	/**
	* @brief	转换respkt_reg_ctrl_priv_t对象
	* @details	把对象实例中的网络字节序转换为本地字节序
	* @param[in,out]	resp，需要转换的对象
	*/
	static void convert_respkt_reg_ctrl_priv_t(respkt_reg_ctrl_priv_t& resp);

	/**
	* @brief	填充reqpkt_unregister_t对象
	* @details	在填充的同时把本地字节序转换成网络字节序
	* @param[out]	req，需要填充的对象
	* @param[in]	packNo，包序号
	*/
	static void set_reqpkt_unreg_ctrl_priv_t(reqpkt_unreg_ctrl_priv_t& req, unsigned int packNo);

	/**
	* @brief	填充reqpkt_set_algo_params_t对象
	* @details	在填充的同时把本地字节序转换成网络字节序
	* @param[out]	req，需要填充的对象
	* @param[in]	packNo，包序号
	* @param[in]	pldLen，负载长度
	*/
	static void set_reqpkt_set_algo_params_t(reqpkt_set_algo_params_t& req, unsigned int packNo, unsigned int pldLen);

	/**
	* @brief	填充reqpkt_data_unregister_t对象
	* @details	在填充的同时把本地字节序转换成网络字节序
	* @param[out]	req，需要填充的对象
	* @param[in]	packNo，包序号
	* @param[in]	mainVer，客户端的协议主版本号
	* @param[in]	subVer，客户端的协议次版本号
	* @param[in]	user，用户名
	* @param[in]	psw，密码
	*/
	static void set_reqpkt_login_t(reqpkt_login_t& req, unsigned int packNo, unsigned short mainVer, unsigned short subVer, const char* user, const char* psw);

	/**
	* @brief	填充reqpkt_get_server_version_t对象
	* @details	在填充的同时把本地字节序转换成网络字节序
	* @param[out]	req，需要填充的对象
	* @param[in]	packNo，包序号
	*/
	void set_reqpkt_get_server_version_t(reqpkt_get_server_version_t& req, unsigned int packNo);

	/**
	* @brief	填充reqpkt_register_t对象
	* @details	在填充的同时把本地字节序转换成网络字节序
	* @param[out]	req，需要填充的对象
	* @param[in]	packNo，包序号
	* @param[in]	chnl，通道号
	*/
	static void set_reqpkt_register_t(reqpkt_register_t& req, unsigned int packNo, unsigned char chnl);

	/**
	* @brief	填充reqpkt_data_unregister_t对象
	* @details	在填充的同时把本地字节序转换成网络字节序
	* @param[out]	req，需要填充的对象
	* @param[in]	packNo，包序号
	*/
	static void set_reqpkt_unregister_t(reqpkt_unregister_t& req, unsigned int packNo);

public:

	SOCKET				s;						// 套接字
	SOCKET				sAgentServerCtrl;		// 数据链路发送控制命令到代理服务器的套接字

	char*				extRecvBuffer;			// 缓冲控制链路回复包除固定头部分
	unsigned int		extRecvBufferSize;		// 缓冲区大小
	unsigned int		packNo;					// 包序号
	HANDLE				hHBThread;				// 心跳线程句柄
	HANDLE				hExitHBThreadEvent;		// 退出心跳线程事件句柄
	bool				bRegister;				// 是否成功注册通道

	CRITICAL_SECTION	hExecCmdCS;				// 执行命令锁
	HWND				hHBProcWnd;				// 处理心跳包消息的窗口

private:


};



#endif