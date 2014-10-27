#ifndef _IPVA_DTC_ITCP_MODEL_DATA_PROC_H_
#define _IPVA_DTC_ITCP_MODEL_DATA_PROC_H_

#include "../NetGobalDef.h"
#include <WinSock2.h>

class ITCPModelDataProc
{
public:
	/**
	* @brief	������Ƶ֡����
	* @param[in]	nFrameNo��֡���
	* @param[in]	nType����Ƶ֡����
	* @param[in]	lpData����Ƶ֡����
	* @param[in]	nLen����Ƶ���ݳ���
	*/
	virtual bool ProcFrameData(unsigned int nFrameNo, unsigned int nType, LPBYTE lpData, unsigned int nLen) = 0;

	virtual bool ProcFrameData_YUV(char *p_buf) = 0;

	/**
	* @brief	����ʵʱĿ����������
	* @param[in]	nFrameNo��֡���
	* @param[in]	vObjInfo��Ŀ��������������
	* @param[in]	nLen�����鳤��
	*/
	virtual void ProcObjectLocation(unsigned int nFrameNo, CObjLocInfo* vObjInfo, unsigned int nLen) = 0;

	/**
	* @brief	����ʵʱ�豸ʱ��
	* @param[in]	nFrameNo��֡���
	* @param[in]	nHour��Сʱ
	* @param[in]	nMinute������
	* @param[in]	nSecond������
	* @param[in]	nTenMillisecond���ٷ�֮һ��
	*/
	virtual void ProcDevTime(unsigned int nFrameNo, unsigned char nHour, unsigned char nMinute, unsigned char nSecond, unsigned char nTenMillisecond) = 0;

	/**
	* @brief	����ʵʱ�㷨����ʱ��
	* @param[in]	nFrameNo��֡���
	* @param[in]	nMillisecond��������
	*/
	virtual void ProcAlgoWorkingTime(unsigned int nFrameNo, unsigned short nMillisecond) = 0;

    virtual void ProcAllCounterData(unsigned int nFrameNo,const char* allCounterData, int nlen) = 0;
};

#endif
