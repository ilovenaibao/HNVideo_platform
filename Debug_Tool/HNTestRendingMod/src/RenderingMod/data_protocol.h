#ifndef _IPVA_DEBUG_PROTOCOL_H_
#define _IPVA_DEBUG_PROTOCOL_H_

#include "../hv_types.h"
#include "../hv_versions.h"

#include <list>
using namespace std;


/**
* @brief	控制协议命令字定义
*/
#define	IPVA_DT_DATA_LOGIN				0x0000		// 登录命令
#define	IPVA_DT_DATA_REG				0x0001		// 注册目标通道
#define IPVA_DT_DATA_UNREG				0x0002		// 注销目标通道
#define IPVA_DT_DATA_HEARTBEAT			0x0003		// 心跳命令
#define IPVA_DT_DATA_REQDATA			0x0004		// 请求数据
// 0x0005 – 0xFFFF					协议保留命令


/**
* @brief	数据协议命令字定义
*/
#define IPVA_DT_DATA_DATALINK_TOKEN		0x0000		// 令牌验证
// 0x0001 – 0xFFFF					协议保留命令

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
//#define IPVA_DT_DATA_DATATYPE_FMPC		0x03		// 实时5分钟进出客流数据
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


//计数器上报数据基类
typedef struct
{
    HV_U32 CountId;
    HV_U32  Type;
}Counter_Base_Info;

typedef struct _TCPData_Payload_PFCount:public Counter_Base_Info
{
    HV_U32 nTotalIn;       //累计进客流
    HV_U32 nTotalOut;      //累计出客流
    HV_U32 nCurrentIn;     //实时进客流
    HV_U32 nCurrentOut;    //实时出客流
}
TCPData_Payload_PFCount;

typedef struct _TCPData_PayloadCrossCount:public Counter_Base_Info
{
    HV_U32 nTotalCrossNum;      // 累计过店客流
    HV_U32 nCrossNum;           // 实时过店客流
}
TCPData_PayloadCrossCount;

typedef struct _TCPData_Payload_DelayCount:public Counter_Base_Info
{
    HV_U32 nCurDelayCount;      //周期滞留人数
    HV_U32 nMaxDelayTime;       //周期个体最大滞留时间
    HV_U32 nTotalDelayTime;     //周期滞留时间总和
}
TCPData_Payload_DelayCount;

typedef struct 
{	
    HV_U32   nId;                 //个体Id
    HV_U8   nEventType;          //触发事件类型
    char     nEventTime[23];      //触发时间
}Event_Occur_Info;

typedef struct _TCPData_Payload_EventCount:public Counter_Base_Info
{
    HV_U32 nTotalIn;     //累计进客流
    HV_U32 nTotalOut;    //累计出客流
    HV_U32 nEvnetNum;    //事件个数
    Event_Occur_Info* nEventInfoList;
}
TCPData_Payload_EventCount;


typedef struct
{
    HV_U32 nId;             // 目标ID
    HV_U32 ntrack;           // 坐标
    char nSampleTime[24];
}Track_Point_Info;

typedef struct _TCPData_Payload_TrackCount:public Counter_Base_Info
{
    HV_U32 nObjectCoorNum;              //轨迹点数目
    Track_Point_Info* nObjectCoor;      //轨迹点列表
}
TCPData_Payload_TrackCount;

typedef struct _TCPData_Payload_ALL_COUNTER
{
    HV_U8       nCountersNum;               //计数器个数
    Counter_Base_Info* nCounterOutput[8];	//计数器输出信息列
    _TCPData_Payload_ALL_COUNTER()
    {
        for(int i = 0;i < 8;i++)
        {
            nCounterOutput[i] = 0;
        }
    }
}TCPData_Payload_All_Counter;

typedef list<Counter_Base_Info*>              LIST_ALL_COUNTER_DATA;
typedef LIST_ALL_COUNTER_DATA::iterator       LIST_ALL_COUNTER_DATA_ITER;

#endif


