#ifndef _IPVA_DTC_RENDER_OBJECT_H_
#define _IPVA_DTC_RENDER_OBJECT_H_

#include "../NetGobalDef.h"
#include <vector>
#include "data_protocol.h"

const int FRAME_TYPE_IDR = 1;
const int FRAME_TYPE_BP = 2;

/**
* @brief	视频帧的数据及信息
* @remark	此类实例的所有方法线程不安全
*/
class CRenderObject
{
public:
	CRenderObject(void);
	~CRenderObject(void);

	/**
	* @brief	初始化实例
	*/
	void Init(void);

	/**
	* @brief	释放资源
	*/
	void Clear(void);

	/**
	* @brief	获取视频帧数据包数据
	* @param[in]	lpData，指向数据指针
	* @param[in]	nDataLen，数据长度
	*/
	void GetFrameData(LPBYTE& lpData, unsigned int& nLen);

	/**
	* @brief	设置视频帧数据
	* @param[in]	lpData，指向视频帧数据
	* @param[in]	nLen，视频帧数据长度
	*/
	void SetFrameData(const BYTE* lpData, unsigned int nLen);

	/**
	* @brief	获取帧序号
	* @return	帧序号
	*/
	unsigned int GetFrameNo(void);

	/**
	* @brief	设置视频帧序号
	* @param[in]	nFrameNo，视频帧序号
	*/
	void SetFrameNo(unsigned int nFrameNo);

	/**
	* @brief	获取视频帧宽度
	* @return	视频帧宽度
	*/
	int GetWidth(void);

	/**
	* @brief	设置视频帧宽度
	* @param[in]	nWidth，视频帧宽度
	*/
	void SetWidth(int nWidth);

	/**
	* @brief	获取视频帧高度
	* @return	视频帧高度
	*/
	int GetHeight(void);

	/**
	* @brief	设置视频帧高度
	* @param[in]	nHeight，视频帧高度
	*/
	void SetHeight(int nHeight);

	/**
	* @brief	获取视频帧
	* @return	视频帧类型
	*/
	unsigned int GetType(void);

	/**
	* @brief	设置视频帧类型
	* @param[in]	nType，视频帧类型
	*/
	void SetType(unsigned int nType);

	///**
	//* @brief	获取实时客流进出数据
	//* @param[out]	nIn，进客流数据
	//* @param[out]	nOut，出客流数据
	//*/
	//void GetPC(unsigned short& nIn, unsigned short& nOut);

	///**
	//* @brief	设置实时客流进出数据
	//* @param[in]	nIn，进客流数据
	//* @param[in]	nOut，出客流数据
	//*/
	//void SetPC(unsigned short nIn, unsigned short nOut);

	/**
	* @brief	获取设备实时系统时间
	* @param[out]	nHour，小时
	* @param[out]	nMinute，分钟
	* @param[out]	nSecond，秒钟
	* @param[out]	nTenmillisecond，百分一秒
	*/
	void GetDevTime(unsigned char& nHour, unsigned char& nMinute, unsigned char& nSecond, unsigned char& nTenmillisecond);

	/**
	* @brief	设置设备实时系统时间
	* @param[in]	nHour，小时
	* @param[in]	nMinute，分钟
	* @param[in]	nSecond，秒钟
	* @param[in]	nTenmillisecond，百分一秒
	*/
	void SetDevTime(unsigned char nHour, unsigned char nMinute, unsigned char nSecond, unsigned char nTenmillisecond);

	/**
	* @brief	设置目标位置数据
	* @param[in]	vObj，指向目标位置数据数组
	* @param[in]	nLen，目标位置数组长度
	*/
	void SetObjLoc(CObjLocInfo* vObj, unsigned int nLen);

	/**
	* @brief	获取目标位置数据
	* @param[out]	vObj，指向目标位置数据数组
	* @param[out]	nLen，目标位置数组长度
	* @return	此对象的目标位置数据是否已被设置
	*/
	bool GetObjLoc(CObjLocInfo*& vObj, unsigned int& nLen);

	/**
	* @brief	获取算法效率
	* @return	算法效率
	*/
	unsigned short GetAlgoET(void);

	/**
	* @brief	设置算法效率
	* @param[in]	nAlgoET，算法效率
	*/
	void SetAlgoET(unsigned short nAlgoET);

	/**
	* @brief	重置当前实例
	*/
	void Reset(void);

    void GetAllCounterData(char* allCounterData, int nlength);
    int  GetAllCounterDataSize();
    void SetAllCounterData(const char* allCounterData, int nlen);

protected:
	// 视频帧信息
	unsigned int				frameNo;		// 帧序号
	LPBYTE						buffer;			// 指向视频帧数据
	unsigned int				bufferLen;		// 缓冲区大小
	unsigned int				dataLen;		// 视频帧数据长度
	int							width;			// 视频帧宽度
	int							height;			// 视频帧高度
	unsigned int				type;			// 视频帧类型

	// 视频帧时间戳
	unsigned char				hour;			// 小时
	unsigned char				minute;			// 分钟
	unsigned char				second;			// 秒数
	unsigned char				tenmillisecond;	// 百分一秒

	// 实时客流进出数据
	unsigned short				inPC, outPC;	// 进客流与出客流

	// 五分钟客流进出数据
	unsigned short				inFMPC, outFMPC;// 进客流与出客流
	bool						bSetFMPC;		// 5分钟进出客流是否被设置

    char* recvCounterData;
    int m_nlen;

	std::vector<CObjLocInfo>	vObjLoc;		// 目标数据
	bool						bSetObjLoc;		// 目标数据是否被设置

	// 算法耗时
	unsigned short				algoET;			// 算法效率耗时

    CRITICAL_SECTION		hCs_recvData;				// 保护操作插入绘制过程的模块的临界区
    VECT_TRACK             vObjTrack;
    bool                   bSetObjTrack;
};

#endif
