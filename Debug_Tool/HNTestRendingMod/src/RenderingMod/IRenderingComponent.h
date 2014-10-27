#ifndef _IPVA_DTC_IRENDERING_COMPONENT_H_
#define _IPVA_DTC_IRENDERING_COMPONENT_H_

#include "data_protocol.h"
#include "RenderObject.h"
class IRenderingComponent
{
public:
	/**
	* @brief	绘制处理过程
	* @details	为绘制模块外的其他模块提供机会绘制需要的图形
	* @param[in]	id，绘制模块编号
	* @param[in]	width，视频帧宽度
	* @param[in]	height，视频帧高度
	* @param[in]	wndWidth，当前绘制窗口的宽度
	* @param[in]	wndHeight，当前绘制窗口的高度
	*/
	virtual void OnRenderingProc(unsigned long id, unsigned int width, unsigned int height, unsigned int wndWidth, unsigned int wndHeight) = 0;

    virtual void OnRenderingAllCounterData(unsigned long id, unsigned int wndWidth, unsigned int wndHeight,char* allCounterData,bool isRetrack) = 0;
};

#endif
