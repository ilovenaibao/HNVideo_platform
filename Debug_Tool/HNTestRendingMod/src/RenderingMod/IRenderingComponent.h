#ifndef _IPVA_DTC_IRENDERING_COMPONENT_H_
#define _IPVA_DTC_IRENDERING_COMPONENT_H_

#include "data_protocol.h"
#include "RenderObject.h"
class IRenderingComponent
{
public:
	/**
	* @brief	���ƴ������
	* @details	Ϊ����ģ���������ģ���ṩ���������Ҫ��ͼ��
	* @param[in]	id������ģ����
	* @param[in]	width����Ƶ֡���
	* @param[in]	height����Ƶ֡�߶�
	* @param[in]	wndWidth����ǰ���ƴ��ڵĿ��
	* @param[in]	wndHeight����ǰ���ƴ��ڵĸ߶�
	*/
	virtual void OnRenderingProc(unsigned long id, unsigned int width, unsigned int height, unsigned int wndWidth, unsigned int wndHeight) = 0;

    virtual void OnRenderingAllCounterData(unsigned long id, unsigned int wndWidth, unsigned int wndHeight,char* allCounterData,bool isRetrack) = 0;
};

#endif
