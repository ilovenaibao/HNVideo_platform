#ifndef _IPVA_DTC_NET_FUNC_RET_DEF_H_
#define _IPVA_DTC_NET_FUNC_RET_DEF_H_

#include "NetTrans.h"
#include <vector>
using namespace  std;


// msg
#define TASK_MSG_COMPLETE					(WM_USER + 1000)		// 任务完成通知的消息

// 
#define TASK_DEFUALT						0x0000
#define TASK_CONNECT_SERVER					0x0001
#define TASK_DISCONNECT_SERVER				0x0002



#define CHNL_NUM					16			// 通道数

// 方法返回值
#define IPVA_DTC_UNDEFINE			-1			// 未定义返回值
#define IPVA_DTC_OK					0			// 执行成功
#define IPVA_DTC_ERR_NET			1			// 网络异常
#define IPVA_DTC_ERR_CONN_CLOSE		2			// 连接被断开
#define IPVA_DTC_ERR_CHECKSUM		3			// 校验和出错
#define IPVA_DTC_ERR_OP_FAILED		4			// 回复包中返回码不成功
#define IPVA_DTC_ERR_BT_FAILED		5			// 启动线程失败
#define IPVA_DTC_ERR_PACKAGE_LEN	6			// 数据包长度异常
#define IPVA_DTC_ERR_ALGO_OP_FAILED	7			// 算法配置回应包返回码不成功
#define IPVA_DTC_ERR_ALGO_PACKAGE_LEN	8		// 算法配置回应包长度异常
#define IPVA_DTC_ERR_ALGO_PARAM_CONV_FAILED	9	// 算法配置转换失败
#define IPVA_DTC_NO_FRAME			0x10		//超时未接到视频数据

// 传输模式
#define DATA_MODE_TCP				0			// TCP模式
#define DATA_MODE_TCP_REUSE			1			// TCP复用模式
#define DATA_MODE_UDP				2			// UDP单播
#define DATA_MODE_UDP_MULTICAST		3			// UDP组播

// 视频帧类型
#define DATA_TYPE_IDRFRAME			0x01		// I帧或IDR帧
#define DATA_TYPE_BPFRAME			0x02		// B帧或P帧

/**
* @brief	实时目标坐标数据
*/
class CObjLocInfo
{
public:
	unsigned int	objID;
	unsigned short	x;
	unsigned short	y;
};

typedef std::vector<CObjLocInfo>   VECT_TRACK;
typedef VECT_TRACK::iterator       VECT_TRACK_ITER;


class CConnectServerStruct
{
public:
	char			ip[16];				// IP地址
	unsigned char	chnl;				// 通道号
	unsigned char	mode;				// 模式

	unsigned short	srvNetProtMainVer;	// 服务器网络通讯协议主版本号
	unsigned short	srvNetProtSubVer;	// 服务器网络通讯协议次版本号
	char			serverVer[32];		// 服务器程序版本

	unsigned short	srvAlgoProtMainVer;	// 服务器算法配置协议主版本号
	unsigned short	srvAlgoProtSubVer;	// 服务器算法配置协议次版本号

	unsigned int	algoType[CHNL_NUM];	// 所有通道正在执行的算法类型
	unsigned int	algoTypeNum;		// 通道数
	char			g2pcVer[32];		// G2PC算法版本号

	CNetTrans*		netTransMod;		// 网络传输模块
	void*			procMod;			// 数据处理模块
	HWND			hWnd;				// 处理心跳包消息的窗口句柄

	bool			bSilent;			// 是否静默执行，出错误时不以模式窗体显示错误信息
	bool			bResult;			// 是否成功建立连接
};

#endif