#ifndef _RENDERING_MOD_H_
#define _RENDERING_MOD_H_

#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"
#include "ITCPModelDataProc.h"
#include "IRenderingComponent.h"
#include "RenderObject.h"
#include "AviWriter.h"
#include "GdiPlusDraw.h"
#include <map>
#include <fstream>
//#include <afxmt.h>
//#include "afxmt.h"//同步操作时需要包含的头文件

#include "data_protocol.h"

#include "Draw.h"

using namespace std;


typedef BOOL (*InitDecoderFunc)(HANDLE *hDecoder);
typedef BOOL (*DecodeStreamFunc)(HANDLE hDecoder, unsigned char* in_buf, int encode_size, int* pWidth, int* pHeight);
typedef BOOL (*GetYUVStreamFunc)(HANDLE hDecoder, unsigned char* out_y, unsigned char* out_u, unsigned char* out_v);
typedef void (*CloseDecoderFunc)(HANDLE *hDecoder);


/**
* @brief	视频绘制模块类
* @remark	此类不应该动态创建与销毁，因为创建绘制线程前必须所有实例已创建，绘制线程在Init方法被调用时选择时机创建
*/
class CRenderingMod : public ITCPModelDataProc
{
public:
	CRenderingMod(void);
	~CRenderingMod(void);

public:
	/**
	* @brief	初始化实例
	* @param[in]	hWnd，模块需要绘制的窗口
	* @param[in]	hContainer，程序容器窗口句柄
	* @return	初始化是否成功
	* @remark	本方法线程不安全，返回为false时不需要调用Clear释放资源
	*/
	bool Init(HWND hWnd, HWND hContainer);

	/**
	* @brief	释放使用的资源
	* @remark	本方法线程不安全
	*/
	void Clear(void);

	/**
	* @brief	开始执行绘制工作
	* @return	是否成功开始绘制工作
	* @remark	可能由于已启动绘制工作而返回false
	*			本方法线程不安全
	*/
	bool Start(void);

	/**
	* @brief	停止绘制工作
	* @return	返回需要等待结束的线程句柄，线程句柄的关闭由调用者执行
	* @remark	本方法线程不安全
	*/
	void Stop(void);


	int	GetDwID(void)
	{
		return dwID;
	}
	/**
	* @brief	处理视频帧数据
	* @param[in]	nFrameNo，帧序号
	* @param[in]	nType，视频帧类型
	* @param[in]	lpData，视频帧数据
	* @param[in]	nLen，视频数据长度
	*/
	bool ProcFrameData(unsigned int nFrameNo, unsigned int nType, LPBYTE lpData, unsigned int nLen);

	bool ProcFrameData_YUV(char *p_buf);

	///**
	//* @brief	处理实时进出客流
	//* @param[in]	nFrameNo，帧序号
	//* @param[in]	nIn，进客流
	//* @param[in]	nOut，出客流
	//*/
	//void ProcPeopleCount(unsigned int nFrameNo, unsigned short nIn, unsigned short nOut);

	/**
	* @brief	处理实时目标坐标数据
	* @param[in]	nFrameNo，帧序号
	* @param[in]	vObjInfo，目标坐标数据数组
	* @param[in]	nLen，数组长度
	*/
	void ProcObjectLocation(unsigned int nFrameNo, CObjLocInfo* vObjInfo, unsigned int nLen);

	/**
	* @brief	处理实时设备时间
	* @param[in]	nFrameNo，帧序号
	* @param[in]	nHour，小时
	* @param[in]	nMinute，分钟
	* @param[in]	nSecond，秒钟
	* @param[in]	nTenMillisecond，百分之一秒
	*/
	void ProcDevTime(unsigned int nFrameNo, unsigned char nHour, unsigned char nMinute, unsigned char nSecond, unsigned char nTenMillisecond);

	/**
	* @brief	处理实时算法运行时间
	* @param[in]	nFrameNo，帧序号
	* @param[in]	nMillisecond，毫秒数
	*/
	void ProcAlgoWorkingTime(unsigned int nFrameNo, unsigned short nMillisecond);

	/**
	* @brief	设置客流统计数据消息接收窗口
	* @param[in]	hWnd，处理客流统计数据消息的窗口的句柄
	*/
	void SetPCWnd(HWND hWnd);

	/**
	* @brief	设置需要插入绘制过程的模块
	* @param[in]	compoment，指向实现了接口IRenderingCompoment的类实例
	*/
	void SetRenderingProc(IRenderingComponent* compoment);

	/**
	* @brief	重置绘制过程
	*/
	void ResetRenderingProc(void);

	///**
	//* @brief	控制是否绘制5分钟客流数据
	//* @param[in]	bShow，true为显示，false为隐藏
	//*/
	//void ShowFiveMinutePeopleCount(bool bShow);

    void ProcAllCounterData(unsigned int nFrameNo, const char* allCounterData, int nlen);

	/**
	* @brief	控制是否绘制实时设备系统时间
	* @param[in]	bShow，true为显示，false为隐藏
	*/
	void ShowSysTime(bool bShow);

	/**
	* @brief	控制是否绘制跟踪线
	* @param[in]	bShow，true为显示，false为隐藏
	*/
	void ShowTrajectory(bool bShow);

	/**
	* @brief	控制是否绘制跟踪对象ID
	* @param[in]	bShow，true为显示，false为隐藏
	*/
	void ShowObjectID(bool bShow);

	/**
	* @brief	控制是否绘制算法效率
	* @param[in]	bShow，true为显示，false为隐藏
	*/
	void ShowAlgoET(bool bShow);

	/**
	* @brief	获取需要绘制的窗口句柄
	* @return	视频绘制的窗口句柄
	*/
	HWND GetHWnd(void);

	/**
	* @brief	开始录像
	* @param[in]	lpFilePath，录像文件路径，字符串最后一个字符必须为'\0'
	* @return	是否成功开始录像
	* @remark	方法不检查lpFilePath是否合法，需要由调用者进行验证
	*/
	bool StartRecord(LPCTSTR lpFilePath);

	/**
	* @brief	停止录像
	*/
	void StopRecord(void);

	/**
	* @brief	绘制正圆形
	* @details	此方法仅限IRenderingComponent::OnRenderingProc方法中调用，否则不保证线程安全
	*/
	static void DrawCircle(DWORD dwId, int xCenter, int yCenter, int nRadius, int nPixel, DWORD FrameColor);

	/**
	* @brief	绘制矩形
	* @details	此方法仅限IRenderingComponent::OnRenderingProc方法中调用，否则不保证线程安全
	*/
	static void DrawRectangle(DWORD dwId, int xCenter, int yCenter, int nLenOfSide, int nPixel, DWORD FrameColor);

	/**
	* @brief	绘制线集合
	* @details	此方法仅限IRenderingComponent::OnRenderingProc方法中调用，否则不保证线程安全
	*/
	static void DrawLines(DWORD dwId, const HV_Line2D* pLineArray, DWORD dwLineCount, DWORD dwColor);

    static void DrawTimingPassFlow(int dwID, int wndWidth, int wndHeight, int inPC, int outPC);
    static void DrawDelayInfo(int dwID, int wndWidth, int wndHeight, int delayNum, int maxtime, int totaltime);
    static void DrawTotalEventInfo(int dwID, int wndWidth, int wndHeight, int inNum, int outNum);
    //static void DrawIDTrack(int dwID, int wndWidth, int wndHeight,VECT_TRACK vObj, bool isRetrack);
    static void DrawTimingCross(int dwID, int wndWidth, int wndHeight, int crossNum);
    //static std::map<unsigned int, std::vector<std::pair<unsigned short, unsigned short>>> counterTrack;	// 跟踪线的点集合
    //static void ClearCounterTrack(void);

protected:
	/**
	* @brief	绘制工作线程
	* @param[in]	obj，线程的参数，成员param
	* @return	线程函数返回值
	*/
	static unsigned int _stdcall RenderingThreadFunc(void* obj);

	/**
	* @brief	实质线程工作函数
	*/
	void DoWork(void);

	/**
	* @brief	显示视频图
	* @param[in]	nID，绘制模块ID
	*/
	void Present(void);

	/**
	* @brief	准备用于绘制的帧数据
	*/
	void PrepareRender(void);

	/**
	* @brief	绘制视频帧
	*/
	void RenderVideoFrame(void);

	//void RenderPassCountToDlg(void);    //绘制客流统计
    void RenderEventInfoToDlg(void);    //绘制事件信息
	/**
	* @brief	执行外部绘制
	*/
	void RenderExternal(void);

    void RenderAllCountData(bool isRetrack);
	/**
	* @brief	绘制实时设备系统时间
	*/
	void RenderDevTime(void);

	/**
	* @brief	绘制跟踪线及对象ID
	* @param[in]	bRefreshTrajectory，是否刷新跟踪线
	*/

	void RenderTrajectory(bool bRefreshTrajectory);

	/**
	* @biref	清空跟踪线及对象ID
	*/
	void ClearTrajectory(void);

public:
    void RenderTrackCounter(bool bRefreshTrajectory);
protected:
	/**
	* @brief	绘制算法效率
	*/
	void RenderAlgoET(void);

	/**
	* @brief	获取等待组装的包
	* @param[in]	nFrameNo，视频帧序号
	* @return	等待组装的包对象
	*/
	CRenderObject* GetWaitPackage(unsigned int nFrameNo);

	/**
	* @brief	写录像
	* @param[in]	lpByte，录像数据
	* @param[in]	nLen，需要写的录像数据长度
	*/
	void WriteRecord(LPBYTE lpByte, unsigned int nLen);

	bool					bInitialized;		// 标记是否执行已初始化
	
	HWND					hWndDisplay;		// 需要绘制的窗口句柄
	unsigned int			wndWidth;			// 需要绘制的窗口句柄实际宽度
	unsigned int			wndHeight;			// 需要绘制的窗口句柄实际高度
	
	HANDLE					hTimer;				// 可等待定时器句柄
	volatile DWORD			dwID;				// 绘制模块ID
	volatile HWND			hPCWnd;				// 显示客流统计的数据的窗口
	bool					bIFrame;			// 已经接收到I帧

	CRenderObject*			nextRenderObject;	// 下一帧绘制使用的对象
	CRenderObject*			curRenderObject;	// 当前绘制使用的对象
	int						delayRound;			// 当前需要跳过多少轮的绘制
	CConcurrentQueue<CRenderObject*>	renderingBufferQueue;	// 绘制缓冲队列
	LPBYTE					renderBuffer;		// 绘制用缓冲区
	unsigned int			bufferLen;			// 绘制用缓冲区大小
	unsigned int			dataLen;			// 数据长度
	unsigned int			nCount;				// 累计没新视频帧需要显示的轮数
	bool					clearBuffer;		// 标识当显示黑屏时，时候清空缓冲
	volatile bool			bStart;				// 是否开始处理视频帧的显示

	CRenderObject*			waitPackage;		// 等待数据到达的包
//	std::map<unsigned int, CRenderObject*>	renderObjectMap;		// 等待帧的数据包收集完成的散列表

	CRenderObject*			renderObjectArray;	// 绘制对象
	CConcurrentStack<CRenderObject*>	renderObjectStack;			// 保存空闲绘制对象
	std::map<unsigned int, std::vector<std::pair<unsigned short, unsigned short>>> trajectory;	// 跟踪线的点集合

    //std::map<unsigned int, std::vector<std::pair<unsigned short, unsigned short>>> trackCounter;	// 跟踪线的点集合

	// 解码库
	HINSTANCE				hIns;				// 载入动态链接库的句柄
	HANDLE					hDecoder;			// 解码器的句柄
	CRITICAL_SECTION		hCsDecoder;			// 操作解码器的临界区
	InitDecoderFunc			initDecoder;		// 初始化解码器
	DecodeStreamFunc		decodeStream;		// 解码数据流
	GetYUVStreamFunc		getYUVStream;		// 获取YUV数据
	CloseDecoderFunc		closeDecoder;		// 关闭解码器

	CRITICAL_SECTION		hCs_component;				// 保护操作插入绘制过程的模块的临界区
	IRenderingComponent*	m_component;		// 其他需要插入绘制过程的模块
	volatile bool			m_showPC;			// 是否显示进出客流
	volatile bool			m_showSysTime;		// 是否显示设备时间
	volatile bool			m_showTrajectory;	// 是否显示跟踪线
	volatile bool			m_showObjectID;		// 是否显示跟踪对象ID
	volatile bool			m_showAlgoET;		// 是否显示算法效率

    volatile bool           m_showDelayManCount; // 是否显示滞留人数 Add by WangJie [2014-8-6]
    volatile bool           m_showDelayMaxTime;  // 是否显示最大滞留时长 Add by WangJie [2014-8-6]
    volatile bool           m_showDelayTotalTime;// 是否显示总滞留时长 Add by WangJie [2014-8-6]

	volatile bool			m_record;			// 是否录像
	volatile bool			m_bRecordIFrame;	// 启动录像后是否找到I帧
	CAviWriter				aviFile;			// 写AVI文件模块
	TCHAR					m_strRecordPath[MAX_PATH];	// 录像文件路径
	volatile int			m_nRecordedFrameCount;		// 已经录像的帧数		
	CRITICAL_SECTION		m_csRecordOp;		// 录像使用的临界区
	volatile unsigned int	m_nIgnoreFrameNo;	// 忽略的帧编号
	CGdiPlusDraw			m_gdiDraw;			// GDI+绘制模块

	// 绘制模块
	static long				nInstance;			// 实例计数
	static HANDLE			hRenderingThread;	// 绘制工作线程返回的句柄
	static bool				bExitThread;		// 退出线程的标识
	static std::vector<CRenderingMod*>	vRenderingMod;	// 绘制模块数组
	//static CCriticalSection	csRenderingMod;		// 绘制模块锁
	static HWND				hContainer;			// 容器的窗口句柄
    
public:
	static CDraw*			drawMod;			// 绘制模块
	static bool				bIsDirectX;			// 标识是否使用DirectX绘图

    char *renderData;
};

#endif