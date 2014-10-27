#ifndef _IPVA_DTC_ITCP_MODEL_DATA_PROC_H_
#define _IPVA_DTC_ITCP_MODEL_DATA_PROC_H_

#include "../NetGobalDef.h"
#include <WinSock2.h>

class ITCPModelDataProc
{
public:
	/**
	* @brief	处理视频帧数据
	* @param[in]	nFrameNo，帧序号
	* @param[in]	nType，视频帧类型
	* @param[in]	lpData，视频帧数据
	* @param[in]	nLen，视频数据长度
	*/
	virtual bool ProcFrameData(unsigned int nFrameNo, unsigned int nType, LPBYTE lpData, unsigned int nLen) = 0;

	virtual bool ProcFrameData_YUV(char *p_buf) = 0;

	/**
	* @brief	处理实时目标坐标数据
	* @param[in]	nFrameNo，帧序号
	* @param[in]	vObjInfo，目标坐标数据数组
	* @param[in]	nLen，数组长度
	*/
	virtual void ProcObjectLocation(unsigned int nFrameNo, CObjLocInfo* vObjInfo, unsigned int nLen) = 0;

	/**
	* @brief	处理实时设备时间
	* @param[in]	nFrameNo，帧序号
	* @param[in]	nHour，小时
	* @param[in]	nMinute，分钟
	* @param[in]	nSecond，秒钟
	* @param[in]	nTenMillisecond，百分之一秒
	*/
	virtual void ProcDevTime(unsigned int nFrameNo, unsigned char nHour, unsigned char nMinute, unsigned char nSecond, unsigned char nTenMillisecond) = 0;

	/**
	* @brief	处理实时算法运行时间
	* @param[in]	nFrameNo，帧序号
	* @param[in]	nMillisecond，毫秒数
	*/
	virtual void ProcAlgoWorkingTime(unsigned int nFrameNo, unsigned short nMillisecond) = 0;

    virtual void ProcAllCounterData(unsigned int nFrameNo,const char* allCounterData, int nlen) = 0;
};

#endif
