/**
 * @file     CDataLink.h
 * @brief    数据连接&控制连接
 * @author   guch
 * @date     2014-10-21
 *
 * @if copyright
 *
 * Copyright (C) 2014-201* guch
 *
 * 用于HN远程调试工具中数据链路
 *
 * @endif
 *
 */

#ifndef _CDATA_LINK_H_
#define _CDATA_LINK_H_

#include "CNetOption.h"
#include "hv_types.h"
//#include <vector>


#define DATACTRLPORT					20148		// 数据链路(到采集点)端口
#define DATACTRLPORT_AGENT_SERVER		19531		// 数据链路(到代理服务器)端口

// 接入客户端与分发服务器交互命令
#define SPFS							0x53465053	// 视频发送head标识
#define FSTX							0x58545346	// 代理服务器发送视频请求的标识
#define BCLJ							0x4A4C4342  // 保持连接
#define SJYZ							0x5A594A53  // 时间验证

/**
* @brief	控制协议命令字定义
*/
#define	IPVA_DT_DATA_LOGIN				0x0000		// 登录命令
#define	IPVA_DT_DATA_REG				0x0001		// 注册目标通道
#define IPVA_DT_DATA_UNREG				0x0002		// 注销目标通道
#define IPVA_DT_DATA_HEARTBEAT			0x0003		// 心跳命令
#define IPVA_DT_DATA_REQDATA			0x0004		// 请求数据
// 0x0005 – 0xFFFF						协议保留命令

/**
* @brief	数据协议命令字定义
*/
#define IPVA_DT_DATA_DATALINK_TOKEN		0x0000		// 令牌验证
// 0x0001 – 0xFFFF						协议保留命令

/**
* @brief	支持的传输模式
*/
#define IPVA_DT_DATA_MODE_TCP			0			// TCP模式
#define IPVA_DT_DATA_MODE_TCP_REUSE		1			// TCP复用模式
#define IPVA_DT_DATA_MODE_UDP			2			// UDP单播
#define IPVA_DT_DATA_MODE_UDP_MULTICAST	3			// UDP组播

/**
* @brief	应答码定义
*/
#define S_DATA_OK						0x0000		// 操作成功
#define S_ERR_DATA_CONNECT_TIMEOUT		0x0001		// 连接超时
#define S_ERR_DATA_ILLEGALACCESS		0x0002		// 操作失败，非法访问
#define S_ERR_DATA_USERORPWD_INCORRECT	0x0003		// 登录失败，用户名或密码错误
#define S_ERR_DATA_PROT_NOTSUPP			0x0004		// 登录失败，协议版本不被支持
#define S_ERR_DATA_CHNL_NOTSUPP			0x0005		// 注册失败，目标通道不被支持
#define S_ERR_DATA_CHNL_CONNECT_TOOMUCH	0x0006		// 注册失败，目标通道连接数过多
#define S_ERR_DATA_REQ_NOTSUPP			0x0007		// 请求数据失败，服务器不支持
#define S_ERR_DATA_LOGOFF_FAILED		0x0008		// 注销失败
#define S_ERR_DATA_CMD_UNSUPPORT		0x0009		// 命令不支持
#define S_ERR_DATA_CMD_TOKEN			0x000A		// 令牌验证失败

/**
* @brief	数据类型
*/
#define IPVA_DT_DATA_DATATYPE_TEST		0x00		// 保留用作程序测试数据
#define IPVA_DT_DATA_DATATYPE_VIDEO		0x01		// 实时视频数据
//#define IPVA_DT_DATA_DATATYPE_PC		0x02		// 实时进出客流数据
//#define IPVA_DT_DATA_DATATYPE_FMPC	0x03		// 实时5分钟进出客流数据
#define IPVA_DT_DATA_DATATYPE_OBJLOC	0x04		// 实时目标位置数据
#define IPVA_DT_DATA_DATATYPE_DEVTIME	0x05		// 实时设备时间
#define IPVA_DT_DATA_DATATYPE_ET		0x06		// 通道算法平均耗时数据
#define IPVA_DT_DATA_DATATYPE_COUNTER   0x07        // 计数器的所有数据类型 Add by WangJie [2014-8-26]
// 0x07 — 0xFF	保留

/**
* @brief	负载类型
*/
#define IPVA_DT_DATA_TYPE_TEST			0x00		// 负责程序测试包
#define IPVA_DT_DATA_TYPE_IDRFRAME		0x01		// H.264 IDR帧
#define IPVA_DT_DATA_TYPE_BPFRAME		0x02		// H.264 B/P帧
// 0x03 – 0x0F	保留类型，用于视频数据类型扩展
//#define IPVA_DT_DATA_TYPE_PC			0x10		// 实时进出数据
//#define IPVA_DT_DATA_TYPE_FMPC			0x11		// 实时5分钟统计进出数据
#define IPVA_DT_DATA_TYPE_OBJLOC		0x12		// 实时目标坐标数据
// 0x13 – 0x1F	保留类型，用于算法输出结果扩展
#define IPVA_DT_DATA_TYPE_DEVTIME		0x20		// 实时设备时间
#define IPVA_DT_DATA_TYPE_AGLOTIME		0x21		// 实时算法运行时间（平均时间，毫秒/帧）     
#define IPVA_DT_DATA_TYPE_COUTER        0x22        // 计数器的负载类型Add by WangJie [2014-8-26]
// 0x22 — 0x2F	保留类型，用于设备信息（或性能信息）扩展
// 0x30 — 0xFF	保留类型，用于其他负载数据类型扩展


/**
* @brief	数据链路控制协议基础请求包
*/
typedef struct _reqpkt_data_t
{
	HV_U32		pkt_len;		// 包长度
	HV_U16		pkt_type;		// 包命令字
	HV_U8		pkt_reserve[2];	// 保留字段
}
reqpkt_data_t;

/**
* @brief	数据链路控制协议基础应答包
*/
typedef struct _respkt_data_t
{
	HV_U32		pkt_len;			// 包长度
	HV_U16		pkt_status;			// 包应答码
	HV_U8		pkt_reserve[2];		// 保留字段
}
respkt_data_t;

/**
* @brief	登录命令
*/
typedef struct _reqpkt_data_login_t
{
	reqpkt_data_t	header;
	HV_U16			nProtMainVer;	// 协议主版本号
	HV_U16			nProtSubVer;	// 协议次版本号
	HV_S8			vUser[16];		// 用户名
	HV_S8			vPsw[64];		// 密码
}
reqpkt_data_login_t;

typedef struct _respkt_data_login_t
{
	respkt_data_t	header;
	HV_U16			nProtMainVer;	// 协议主版本号
	HV_U16			nProtSubVer;	// 协议次版本号
}
respkt_data_login_t;

/**
* @brief	注册目标通道
*/
typedef struct _reqpkt_data_register_t 
{
	reqpkt_data_t	header;
	HV_U8			nChnl;			// 通道号
	HV_U8			nMode;			// 传输模式
	HV_U8			nReserve[2];	// 保留字段
}
reqpkt_data_register_t;
typedef respkt_data_t respkt_data_register_t;

/**
* @brief	以TCP模式注册目标通道时，回复包中的负载
*/
typedef struct _respl_data_mode_tcp_t
{
	HV_U16			nPort;			// TCP数据服务器端口
	HV_U16			nReserve1;		// 保留字段
	HV_U32			nTokenID;		// 令牌ID
}
respl_data_mode_tcp_t;

/**
* @brief	以UDP单播模式注册目标通道时，回复包中的负载
*/
typedef struct _respl_data_mode_udp_t
{
	HV_U16			nPort;			// UDP数据服务器端口
	HV_U16			nReserve1;		// 保留字段
	HV_U32			nReserve2;		// 保留字段
}
respl_data_mode_udp_t;

/**
* @brief	以UDP组播模式注册目标通道时，回复包中的负载
*/
typedef struct _respl_data_mode_udp_multicast_t
{
	HV_U32			nIPAddr;		// 数组组播地址
	HV_U16			nPort;			// 数据组播端口
	HV_U16			nReserve1;		// 保留字段
	HV_U32			nReserve2;		// 保留字段
	HV_U32			nEncryptionKey;	// 加密密钥
}
respl_data_mode_udp_multicast_t;

/**
* @brief	数据链路数据协议基础请求包
*/
typedef reqpkt_data_t reqpkt_datalink_data_t;

/**
* @brief	数据链路数据协议基础应答包
*/
typedef respkt_data_t respkt_datalink_data_t;

/**
* @brief	发送令牌
*/
typedef struct _reqpkt_datalink_data_token_t
{
	reqpkt_datalink_data_t	header;
	HV_U32					nTokenid;// 令牌
}
reqpkt_datalink_data_token_t;
typedef respkt_datalink_data_t respkt_datalink_data_token_t;


/**
* @brief	数据包固定头
*/
typedef struct _datapkt_data_t
{
	HV_U32			pkt_len;		// 数据包长度
	HV_U32			pkt_no;			// 帧序号
	HV_U32			pkt_payload_no;	// 本数据包负载的数目
}
datapkt_data_t;

/**
* @brief	数据包负载固定头
*/
typedef struct _datapl_data_t
{
	HV_U32			pl_len;			// 负载长度
	HV_U8			pl_datatype;	// 负载数据类型
	HV_U8			pl_type;		// 负载类型
	HV_U8			pl_reserve[2];	// 保留字段
}
datapl_data_t;

/**
* @brief	实时客流进出数据
*/
typedef struct _datapl_pc_t
{
	HV_U16			pl_in;			// 进客流
	HV_U16			pl_out;			// 出客流
}
datapl_pc_t;

/**
* @brief	5分钟客流进出数据
*/
typedef datapl_pc_t datapl_fmpc_t;

/**
* @brief	实时目标数目
*/
typedef struct _datapl_objnum_t
{
	HV_U16			pl_objnum;		// 实时目标数目
	HV_U16			pl_reserve;		// 保留字段
}
datapl_objnum_t;

/**
* @brief	目标位置信息
*/
typedef struct _datapl_objloc_t
{
	HV_U32			pl_objid;		// 目标ID
	HV_S16			pl_x;			// x坐标
	HV_S16			pl_y;			// y坐标
}
datapl_objloc_t;

/**
* @brief	实时设备时间
*/
typedef struct _datapl_devtime_t
{
	HV_U8			pl_hour;		// 时
	HV_U8			pl_minute;		// 分
	HV_U8			pl_second;		// 秒
	HV_U8			pl_tms;			// 10毫秒
}
datapl_devtime_t;

/**
* @brief	实时算法运行时间
*/
typedef struct _datapl_algo_worktime_t
{
	HV_U16			pl_worktime;	// 实时算法运行时间
	HV_U16			pl_reserve;		// 保留字段
}
datapl_algo_worktime_t;


///**
//* @brief	实时目标坐标数据
//*/
//class CObjLocInfo
//{
//public:
//	unsigned int	objID;
//	unsigned short	x;
//	unsigned short	y;
//};
//
//typedef std::vector<CObjLocInfo>   VECT_TRACK;
//typedef VECT_TRACK::iterator       VECT_TRACK_ITER;


class CDataLink:public CNetOption {

public:
	CDataLink(void);
	~CDataLink(void);

	/**
	* @brief	与指定IP的设备建立连接
	* @details	此方法完成连接，登录，注册的功能
	* @param[in]	lpAddr，以'\0'结尾的IP字符串
	* @param[in]	nChnl，通道号
	* @param[in]	nMode，传输模式
	* @param[in]	procMod，数据链路数据处理模块
	* @param[in]	nCode，错误码
	* @return	返回码
	* @remark	TCP模式下，procMod参数在必须指向实现了ITCPModelDataProc接口的类实例
	*/
	int Link(const char* lpAddr, unsigned char nChnl, unsigned char nMode, void* procMod, int& nCode);

	/**
	* @brief	断开连接
	* @details	断开与设备建立
	*/
	void UnLink(void);

	/**
	* @brief	与指定IP的代理服务器建立连接
	* @details	此方法完成连接
	* @param[in]	lpAddr，以'\0'结尾的IP字符串
	* @param[in]	nChnl，通道号
	* @param[in]	nMode，传输模式
	* @param[in]	procMod，数据链路数据处理模块
	* @param[in]	nCode，错误码
	* @return	返回码
	* @remark	TCP模式下，procMod参数在必须指向实现了ITCPModelDataProc接口的类实例
	*/
	int CDataLink::LinkAgentServer(const char* lpAddr, unsigned char nChnl, unsigned char nMode, void* procMod, int& nCode);
	
	/**
	* @brief	断开与代理服务器连接
	* @details	断开与设备建立
	*/
	void CDataLink::UnLinkAgentServer(void);

	int get_frame_length(BYTE *buf, int buf_size);

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
	* @param[in]	nMode，传输模式
	* @param[out]	nCode，错误码
	* @return	成功返回IPVA_DTC_OK，失败返回对应的返回值，nCode根据返回值决定返回Windows Sockets Error Codes还是回复包中的返回码
	*/
	int Register(unsigned char nChnl, unsigned char nMode, int& nCode);

	/**
	* @brief	注销目标通道
	* @param[out]	nCode，错误码
	* @return	成功返回IPVA_DTC_OK，失败返回对应的返回值，nCode根据返回值决定返回Windows Sockets Error Codes还是回复包中的返回码
	*/
	int UnRegister(int& nCode);

	/**
	* @brief	发出令牌
	* @param[in]	nToken，令牌
	* @param[out]	nCode，错误码
	* @return	成功返回IPVA_DTC_OK，失败返回对应的返回值，nCode返回Windows Sockets Error Codes
	*/
	int SendToken(unsigned int nToken, int& nCode);

	/**
	* @brief	在数据链路控制线执行命令
	*/
	int ExecCtrlCmd(const char* sndBuffer, int sndBufferLen, char* recvHeader, int& recvRemainLen, int& nCode);
	
	/**
	* @brief	在数据链路数据线执行命令
	*/
	int ExecDataCmd(const char* sndBuffer, int sndBufferLen, char* recvHeader, int& recvRemainLen, int& nCode);
	
	/**
	* @brief	执行命令
	*/
	int ExecCmd(SOCKET s, const char* sndBuffer, int sndBufferLen, char* recvHeader, int& recvRemainLen, int& nCode);
	
	/**
	* @brief	接收数据
	* @param[in]	rcvBuffer，接收数据的缓冲区
	* @param[in]	bufferLen，需要接收数据的长度
	* @param[out]	nCode，错误码
	* @return	返回码
	*/
	int RcvData(char* rcvBuffer, int bufferLen, int& nCode);

	/**
	* @brief	填充reqpkt_data_login_t对象
	* @details	在填充的同时把本地字节序转换成网络字节序
	* @param[out]	req，需要填充的对象
	* @param[in]	user，用户名
	* @param[in]	psw，密码
	*/
	static void set_reqpkt_data_login_t(reqpkt_data_login_t& req, const char* user, const char* psw);
	
	/**
	* @brief	转换respkt_data_login_t对象
	* @details	把对象实例中的网络字节序转换为本地字节序
	* @param[in,out]	resp，需要转换的对象
	*/
	static void convert_respkt_data_login_t(respkt_data_login_t& resp);
	
	/**
	* @brief	填充reqpkt_data_register_t对象
	* @details	在填充的同时把本地字节序转换成网络字节序
	* @param[out]	req，需要填充的对象
	* @param[in]	chnl，通道号
	* @param[in]	mode，传输模式
	*/
	static void set_reqpkt_data_register_t(reqpkt_data_register_t& req, unsigned char chnl, unsigned char mode);
	
	/**
	* @brief	转换respkt_data_t对象
	* @details	把对象实例中的网络字节序转换为本地字节序
	* @param[in,out]	resp，需要转换的对象
	*/
	static void convert_respkt_data_t(respkt_data_t& resp);
	
	/**
	* @brief	转换respl_data_mode_tcp_t对象
	* @details	把对象实例中的网络字节序转换为本地字节序
	* @param[in,out]	pl，需要转换的对象
	*/
	static void convert_respl_data_mode_tcp_t(respl_data_mode_tcp_t& pl);
	
	/**
	* @brief	填充reqpkt_datalink_data_token_t对象
	* @details	在填充的同时把本地字节序转换成网络字节序
	* @param[out]	req，需要填充的对象
	* @param[in]	tokenId，令牌ID
	*/
	static void set_reqpkt_datalink_data_token_t(reqpkt_datalink_data_token_t& req, unsigned int tokenId);
	
	/**
	* @brief	转换respkt_datalink_data_t对象
	* @details	把对象实例中的网络字节序转换为本地字节序
	* @param[in,out]	resp，需要转换的对象
	*/
	static void convert_respkt_datalink_data_t(respkt_datalink_data_t& resp);

	/**
	* @brief	转换datapkt_data_t对象
	* @details	把对象实例中的网络字节序转换为本地字节序
	* @param[in,out]	pkt，需要转换的对象
	*/
	static void convert_datapkt_data_t(datapkt_data_t& pkt);

	/**
	* @brief	转换datapl_data_t对象
	* @details	把对象实例中的网络字节序转换为本地字节序
	* @param[in,out]	pl，需要转换的对象
	*/
	static void convert_datapl_data_t(datapl_data_t& pl);

	/**
	* @brief	转换datapl_objnum_t对象
	* @details	把对象实例中的网络字节序转换为本地字节序
	* @param[in,out]	pl，需要转换的对象
	*/
	static void convert_datapl_objnum_t(datapl_objnum_t& pl);

	/**
	* @brief	转换datapl_algo_worktime_t对象
	* @details	把对象实例中的网络字节序转换为本地字节序
	* @param[in,out]	pl，需要转换的对象
	*/
	static void convert_datapl_algo_worktime_t(datapl_algo_worktime_t& pl);

	/**
	* @brief	数据接收线程函数
	*/
	static unsigned int __stdcall DataRcvThreadFunc(void* obj);

	static unsigned int __stdcall DataRcvAndSendThreadFunc(void* obj);

	static unsigned int __stdcall LinkAgentServerThreadFunc(void* obj);

	int SendVideoToAgentServer(CDataLink *link, char *buffer, int buf_len);
	int SendVideoToAgentServer_old(CDataLink *link, char *buffer, int buf_len);

public:
	char* pMyBuffer;
	int iFrameSize;
	int iFrameFlag;
	int iFrameID;
	int iFrameTime;

	HWND hDisplay;

private:
	SOCKET	sCtrl;									// 数据链路控制连接的套接字
	SOCKET	sData;									// 数据链路数据连接的套接字
	SOCKET  sAgentServerData;						// 数据链路发送数据到代理服务器的套接字


	CRITICAL_SECTION	hExecCmdCS;					// 执行命令锁

	char				extRecvBuffer[16];			// 缓冲控制链路回复包除固定头部分
	unsigned short		nMainVersion, nSubVersion;	// 服务端协议主版本号与协议次版本号
	unsigned short		nPort;						// 数据传输使用的端口，保存TCP模式、UDP单播模式与UDP组播模式中使用的端口
	unsigned int		nTokenID;					// 令牌ID
	HANDLE				hExitHBThreadEvent;			// 退出心跳线程事件句柄
	HANDLE				hHBThread;					// 心跳线程句柄
	HANDLE				hAgentServerThread;			// 连接代理服务器线程句柄
	HANDLE				hDataRcvThread;				// 数据接收线程句柄
	void*				processModule;				// 指向数据处理模块
	bool				bRegister;					// 是否成功注册通道
	volatile bool		bUnLink;					// 是否执行了断开操作			
	unsigned int		nCount;						// 连续接收到时数据无视频包的次数
	HWND				hWndNotify;					// 保存网络异常时需要通知的窗体

	bool				bRecord;

	int					nSendVideoFlag;				// 是否发送视频到代理服务器

};


#endif