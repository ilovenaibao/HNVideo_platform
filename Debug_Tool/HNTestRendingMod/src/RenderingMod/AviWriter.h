#pragma once
#include <Vfw.h>

class CAviWriter
{
public:
	CAviWriter(void);
	~CAviWriter(void);

	/**
	* @brief	打开AVI文件
	* @param[in]	filename，文件名
	* @return	是否与指定路径成功打开AVI文件
	* @remark	当返回true时必须调用AVI_Close_file关闭已打开的AVI文件
	*/
	bool AVI_Create_Video_file(LPCTSTR filename);
	
	/**
	* @brief	设置AVI视频格式
	* @param[in]	width，宽度
	* @param[in]	height，高度
	* @param[in]	compressor，视频数据使用的编码，当前使用H264，为无需包含null结尾的4字节字符串
	* @return	是否成功设置视频格式
	* @remark	在调用AVI_Write_frame前必须调用此方法设置AVI视频格式
	*/
	bool AVI_Set_Video(int width,int height,char* compressor);

	/**
	* @brief	把视频帧写入AVI视频流中
	* @param[in]	pVideo，帧数据，只能包含一帧
	* @param[in]	byteLength，帧数据长度
	* @return	是否成功写入帧数据
	*/
	bool AVI_Write_frame(const char* pVideo,long byteLength);

	/**
	* @brief	关闭AVI文件
	* @remark	必须与AVI_Create_Video_file配对使用
	*/
	void AVI_Close_file();//关闭avi文件

	/**
	* @brief	判断AVI文件是否已被打开
	* @return	是否已打开AVI文件
	*/
	bool isOpen();


private:
	PAVIFILE			avi;		// AVI文件
	PAVISTREAM			stream;		// AVI数据流
	long				frameNo;	// 帧序号
	bool				setFormat;	// 标识数据流格式是否已被设置
	CRITICAL_SECTION	hCs;		// 临界区
};

