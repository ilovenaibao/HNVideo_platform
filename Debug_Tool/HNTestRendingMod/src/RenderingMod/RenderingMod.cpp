#include "StdAfx.h"
#include "RenderingMod.h"

//#include "Resource.h "

#include <assert.h>
#include <tchar.h>
#include "MacroDefine.h"


#include "LogFile.h"
extern CLogFile  g_runLog;

#define WAIT_EXITTHREAD_TIMEOUT		3000	// 线程等待退出超时
#define RENDERING_INTERVAL			20		// 绘制视频帧间隔
#define RENDERING_OBJECT_NUM		10		// 保持绘制对象的数量
#define DEFAULTWIDTH				352		// 默认视频帧长度
#define DEFAULTHEIGHT				288		// 默认视频帧宽度
#define MAX_FRAME_SIZE				(DEFAULTWIDTH * DEFAULTHEIGHT * 3 / 2)
#define MAX_RECORD_FRAME_COUNT		900000

long CRenderingMod::nInstance = 0;				// 实例计数器
CDraw* CRenderingMod::drawMod = NULL;			// 绘制模块
HANDLE CRenderingMod::hRenderingThread = NULL;	// 绘制工作线程返回的句柄
bool CRenderingMod::bExitThread = false;		// 是否退出绘制线程
std::vector<CRenderingMod*>	CRenderingMod::vRenderingMod;	// 绘制模块数组vRenderingMod
//CCriticalSection CRenderingMod::csRenderingMod;	// 绘制模块锁
HWND CRenderingMod::hContainer = NULL;			// 程序容器父窗口句柄
bool CRenderingMod::bIsDirectX = false;			// 是否使用DirectX绘图

CCriticalSection csRenderingMod;


CRenderingMod::CRenderingMod(void)
	: bInitialized(false)
	, hWndDisplay(NULL)
	, wndWidth(0)
	, wndHeight(0)
	, hTimer(NULL)
	, delayRound(0)
	, curRenderObject(NULL)
	, nextRenderObject(NULL)
	, renderObjectArray(NULL)
	, hIns(NULL)
	, hDecoder(NULL)
	, initDecoder(NULL)
	, decodeStream(NULL)
	, getYUVStream(NULL)
	, closeDecoder(NULL)
	, waitPackage(NULL)
	, m_component(NULL)
	, m_showPC(false)
	, m_showSysTime(false)
	, m_showTrajectory(false)
	, m_showObjectID(false)
	, m_showAlgoET(false)
	, m_record(false)
	, m_bRecordIFrame(false)
	, m_nRecordedFrameCount(0)
	, nCount(0)
	, clearBuffer(true)
	, bStart(false)
	, m_nIgnoreFrameNo(0)
	, dwID(-1)
    , m_showDelayManCount(false)
    , m_showDelayMaxTime(false)
    , m_showDelayTotalTime(false)
    , renderData(NULL)
{
	::InterlockedIncrement(&nInstance);
}


CRenderingMod::~CRenderingMod(void)
{
	::InterlockedDecrement(&nInstance);
}


bool CRenderingMod::Init(HWND hWnd, HWND hContainer)
{
	bool result = false;
	do 
	{
		if (bInitialized)
		{
			// 已初始化
			break;
		}

		// 保存需要绘制的窗口句柄
		hWndDisplay = hWnd;

		nCount = 0;
		clearBuffer = true;

		RECT rect;
		GetWindowRect(hWndDisplay, &rect);
		wndWidth = rect.right - rect.left;
		wndHeight = rect.bottom - rect.top;

		csRenderingMod.Lock();
		// 添加进绘制队列
		vRenderingMod.push_back(this);
		if (vRenderingMod.size() == nInstance) 
		{
			if (!hRenderingThread)
			{
				bExitThread = false;
				CRenderingMod::hContainer = hContainer;
				if (drawMod == NULL)
				{
					drawMod = new CDraw();
				}
				hRenderingThread = (HANDLE)::_beginthreadex(NULL, 0, CRenderingMod::RenderingThreadFunc, hContainer, 0, NULL);
			}
			else if (drawMod && bIsDirectX)
			{
				// 在已创建DirectX绘制实例的前提下添加新的绘制模块对象
				HWND* hWndArray = NULL;
				int vRenderingModSize = 0;
				DWORD* dwIDArray = NULL;

				vRenderingModSize = vRenderingMod.size();
				if (vRenderingModSize > 0)
				{
					hWndArray = new HWND[vRenderingModSize];
					dwIDArray = new DWORD[vRenderingModSize];
					for (int count = 0; count < vRenderingModSize; count++)
					{
						hWndArray[count] = vRenderingMod[count]->GetHWnd();
					}
				}
				if (hWndArray && dwIDArray && drawMod)
				{
					drawMod->Delete();
					bIsDirectX = drawMod->Create(hContainer, hWndArray, dwIDArray, vRenderingModSize) == FALSE ? false : true;

					//bIsDirectX = FALSE;
                    if (bIsDirectX)
					{
						for(int count = 0; count < vRenderingModSize; count++)
						{
							vRenderingMod[count]->dwID = dwIDArray[count];			// 设置使用模块ID
						}
					}
					else
					{
						for(int count = 0; count < vRenderingModSize; count++)
						{
							vRenderingMod[count]->m_gdiDraw.Delete();
							vRenderingMod[count]->m_gdiDraw.Create(hWndArray[count]);
							vRenderingMod[count]->dwID = count;
						}
					}

                    // wangjie 10/15
                    //for(int count = 0; count < vRenderingModSize; count++)
                    //{
                    //    vRenderingMod[count]->m_gdiDraw.Delete();
                    //    vRenderingMod[count]->m_gdiDraw.Create(hWndArray[count]);
                    //    vRenderingMod[count]->dwID = count;
                    //}
				}
				if (hWndArray)
				{
					delete[] hWndArray;
				}
				if (dwIDArray)
				{
					delete[] dwIDArray;
				}
			}
		}

		if (vRenderingMod.size() == nInstance && !hRenderingThread)
		{
			// 创建线程失败
			csRenderingMod.Unlock();
			break;
		}
		csRenderingMod.Unlock();

		hIns = ::LoadLibrary(_T("VideoDecoder.dll"));
		if (hIns)
		{
			initDecoder = (InitDecoderFunc)::GetProcAddress(hIns, "InitDecoder");
			decodeStream = (DecodeStreamFunc)::GetProcAddress(hIns, "DecodeStream");
			getYUVStream = (GetYUVStreamFunc)::GetProcAddress(hIns, "GetYUVStream");
			closeDecoder = (CloseDecoderFunc)::GetProcAddress(hIns, "CloseDecoder");
		} else {
			AfxMessageBox(L"error : LoadLibrary->VideoDecoder.dll");
			exit(0);
		}

		// 创建绘制视频缓冲区
		renderBuffer = new BYTE[MAX_FRAME_SIZE];
		bufferLen = MAX_FRAME_SIZE;
		dataLen = 0;

		// 创建临界区
		::InitializeCriticalSection(&hCs_component);
		::InitializeCriticalSection(&hCsDecoder);
		::InitializeCriticalSection(&m_csRecordOp);

		// 集中创建绘制对象
		renderObjectArray = new CRenderObject[RENDERING_OBJECT_NUM];
		CRenderObject* renderObjPtr;
		for (int count = 0; count < RENDERING_OBJECT_NUM; count++)
		{
			renderObjPtr = &renderObjectArray[count];
			renderObjPtr->Init();
			renderObjectStack.Push(renderObjPtr);
		}

		bInitialized = true;
		result = true;
	} while (false);
	return result;
}


void CRenderingMod::Clear(void)
{
	if (!bInitialized)
	{
		// 未进行过初始化
		return;
	}

	bool exitThread = false;		// 标识是否由本方法结束线程


	csRenderingMod.Lock();
	// 从绘制队列中移除
	for(std::vector<CRenderingMod*>::iterator it = vRenderingMod.begin(); it != vRenderingMod.end(); it++)
	{
		if (*it == this)
		{
			m_gdiDraw.Delete();
			vRenderingMod.erase(it);
			break;
		}
	}

	// 释放绘制线程
	if (vRenderingMod.size() == 0 && hRenderingThread)
	{
		// 设置线程退出标识位
		bExitThread = true;
		exitThread = true;
	}
	csRenderingMod.Unlock();

	if (exitThread)
	{
		if (WAIT_TIMEOUT == ::WaitForSingleObject(hRenderingThread, WAIT_EXITTHREAD_TIMEOUT))
		{
			::TerminateThread(hRenderingThread, -1);
		}
		::CloseHandle(hRenderingThread);
		hRenderingThread = NULL;
		if (bIsDirectX)
		{
			// 释放绘制模块资源
			drawMod->Delete();
		}
		if (drawMod)
		{
			// 释放绘制模块实例
			drawMod->Release();
			drawMod = NULL;
		}
	}

	nCount = 0;
	clearBuffer = true;

	hWndDisplay = NULL;
	wndWidth = 0;
	wndHeight = 0;

	// 释放绘制缓冲区
	delete[] renderBuffer;
	bufferLen = 0;
	dataLen = 0;

	::DeleteCriticalSection(&hCs_component);
	::DeleteCriticalSection(&hCsDecoder);
	::DeleteCriticalSection(&m_csRecordOp);

	// 释放绘制对象占用的资源
	CRenderObject* renderObjPtr;
	while (renderObjectStack.Pop(renderObjPtr))
	{
	}
	while(renderingBufferQueue.Pop(renderObjPtr))
	{
	}
	for (int count = 0; count < RENDERING_OBJECT_NUM; count++)
	{
		renderObjectArray[count].Clear();
	}
	if (renderObjectArray)
	{
		delete[] renderObjectArray;
	}

	// 释放解码库资源
	if (hIns)
	{
		initDecoder = NULL;
		decodeStream = NULL;
		getYUVStream = NULL;
		closeDecoder = NULL;
		::FreeLibrary(hIns);
		hIns = NULL;
	}

	// 设置未进行初始化
	bInitialized = false;
}

bool CRenderingMod::Start(void)
{
	if (hRenderingThread)
	{
		bStart = true;
		::EnterCriticalSection(&hCsDecoder);
		bIFrame = false;
		if (!hDecoder && initDecoder)
		{
			initDecoder(&hDecoder);
		}
		::LeaveCriticalSection(&hCsDecoder);
		return true;
	}
	return false;
}


void CRenderingMod::Stop(void)
{
    if (&hCsDecoder == NULL)
    {
        return;
    }
	bStart = false;
	::EnterCriticalSection(&hCsDecoder);
	bIFrame = false;
	if (hDecoder && closeDecoder)
	{
		closeDecoder(&hDecoder);
		hDecoder = NULL;
	}
	::LeaveCriticalSection(&hCsDecoder);
}

bool CRenderingMod::ProcFrameData_YUV(char *p_buf)
{
	static unsigned int nFrameNo = 0;

	bIFrame = TRUE;
	if (hCsDecoder.DebugInfo == NULL)
	{
		return false;
	}
	int height = 288, width = 352;
	::EnterCriticalSection(&hCsDecoder);

	unsigned int dataLen = static_cast<unsigned int>(width * height * 3 / 2);
	if (bufferLen < dataLen)
	{
		delete[] renderBuffer;
		renderBuffer = new BYTE[dataLen];
		bufferLen = dataLen;
	}

	memcpy(renderBuffer, p_buf, dataLen);
	CRenderObject* obj = GetWaitPackage(nFrameNo++);
	if (obj != NULL)
	{
		obj->SetWidth(width);
		obj->SetHeight(height);
		obj->SetType(1);
		obj->SetFrameData(renderBuffer, dataLen);
	}
	::LeaveCriticalSection(&hCsDecoder);
	return m_bRecordIFrame;
}

bool CRenderingMod::ProcFrameData(unsigned int nFrameNo, unsigned int nType, LPBYTE lpData, unsigned int nLen)
{
    if (hCsDecoder.DebugInfo == NULL)
    {
        return false;
    }
	int height = 0, width = 0;
	::EnterCriticalSection(&hCsDecoder);
	if (!bIFrame && nType == FRAME_TYPE_IDR)
	{
		bIFrame = true;
	}
	if (bIFrame && hDecoder && decodeStream && getYUVStream && decodeStream(hDecoder, lpData, nLen, &width, &height))
	{
		unsigned int dataLen = static_cast<unsigned int>(width * height * 3 / 2);
		if (0 == dataLen && (0 == width || 0 == height)) {
			return false;
		} else {
			int xx = 0;
			xx = 1;
		}
		if (bufferLen < dataLen)
		{
			delete[] renderBuffer;
			renderBuffer = new BYTE[dataLen];
			bufferLen = dataLen;
		}
		getYUVStream(hDecoder, renderBuffer, renderBuffer + width * height, renderBuffer + width * height * 5 / 4);
		if (m_record && !m_bRecordIFrame && nType == FRAME_TYPE_IDR)
		{
			m_bRecordIFrame = true;
			aviFile.AVI_Set_Video(width, height, "H264");

		}
		if (m_bRecordIFrame)
		{
			if (m_nRecordedFrameCount < MAX_RECORD_FRAME_COUNT)
			{
				WriteRecord(lpData, nLen);					// 录像
				m_nRecordedFrameCount++;
			}
			else
			{
				// 新建文件录像
			//TODO:将文件写入录像文件后删除
				aviFile.AVI_Close_file();
				//DeleteFile()
				time_t rawtime;
				time(&rawtime);
				struct tm timeinfo;
				localtime_s(&timeinfo, &rawtime);	// 获取系统时间

				TCHAR tmpStr[MAX_PATH];
				_tcscpy_s(tmpStr, m_strRecordPath);
				TCHAR* pos = _tcsrchr(tmpStr, _T('_'));
				*pos = 0;
				pos = _tcsrchr(tmpStr, _T('_'));

				TCHAR strNum[10] = _T("");
				for (int count = 0; count < 256; count++)
				{
					_stprintf_s(pos, 50, _T("_%04d%02d%02d_%02d%02d%02d%s.avi"),
						timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, strNum);
					if (!::PathFileExists(tmpStr))
					{
						_tcscpy_s(m_strRecordPath, tmpStr);
						aviFile.AVI_Create_Video_file(m_strRecordPath);
						if (aviFile.isOpen())
						{
							m_bRecordIFrame = false;
							m_nRecordedFrameCount = 0;
						}
						break;
					}
					_stprintf_s(strNum, 10, _T("(%d)"), count + 1);
				}
			}
		}

		CRenderObject* obj = GetWaitPackage(nFrameNo);
		if (obj != NULL)
		{
			obj->SetWidth(width);
			obj->SetHeight(height);
			obj->SetType(nType);
			obj->SetFrameData(renderBuffer, dataLen);
		}
	}
	::LeaveCriticalSection(&hCsDecoder);
	return m_bRecordIFrame;
}

void CRenderingMod::ProcObjectLocation(unsigned int nFrameNo, CObjLocInfo* vObjInfo, unsigned int nLen)
{
	CRenderObject* obj = GetWaitPackage(nFrameNo);
	if (obj != NULL)
	{
		obj->SetObjLoc(vObjInfo, nLen);
	}
}


void CRenderingMod::ProcDevTime(unsigned int nFrameNo, unsigned char nHour, unsigned char nMinute, unsigned char nSecond, unsigned char nTenMillisecond)
{
	CRenderObject* obj = GetWaitPackage(nFrameNo);
	if (obj != NULL)
	{
		obj->SetDevTime(nHour, nMinute, nSecond, nTenMillisecond);
	}
}

void CRenderingMod::ProcAlgoWorkingTime(unsigned int nFrameNo, unsigned short nMillisecond)
{
	CRenderObject* obj = GetWaitPackage(nFrameNo);
	if (obj != NULL)
	{
		obj->SetAlgoET(nMillisecond);
	}
}

void CRenderingMod::SetPCWnd(HWND hWnd)
{
	hPCWnd = hWnd;
}

void CRenderingMod::SetRenderingProc(IRenderingComponent* compoment)
{
	::EnterCriticalSection(&hCs_component);
	m_component = compoment;
	::LeaveCriticalSection(&hCs_component);
}

void CRenderingMod::ResetRenderingProc(void)
{
	::EnterCriticalSection(&hCs_component);
	m_component = NULL;
	::LeaveCriticalSection(&hCs_component);
}

void CRenderingMod::ShowSysTime(bool bShow)
{
	m_showSysTime = bShow;
}


void CRenderingMod::ShowTrajectory(bool bShow)
{
	m_showTrajectory = bShow;
}


void CRenderingMod::ShowObjectID(bool bShow)
{
	m_showObjectID = bShow;
}


void CRenderingMod::ShowAlgoET(bool bShow)
{
	m_showAlgoET = bShow;
}


HWND CRenderingMod::GetHWnd(void)
{
	return hWndDisplay;
}


bool CRenderingMod::StartRecord(LPCTSTR lpFilePath)
{
	bool result = false;
	::EnterCriticalSection(&m_csRecordOp);
	aviFile.AVI_Create_Video_file(lpFilePath);
	if (aviFile.isOpen())
	{
		_tcscpy_s(m_strRecordPath, MAX_PATH, lpFilePath);
		m_record = true;
		m_bRecordIFrame = false;
		result = true;
	}
	::LeaveCriticalSection(&m_csRecordOp);
	return result;
}


void CRenderingMod::StopRecord(void)
{
	::EnterCriticalSection(&m_csRecordOp);
	m_record = false;
	m_bRecordIFrame = false;
	m_nRecordedFrameCount = 0;
	if (aviFile.isOpen())
	{
		//TODO:将文件写入录像文件后删除
		aviFile.AVI_Close_file();
	}
	::LeaveCriticalSection(&m_csRecordOp);
}


void CRenderingMod::DrawCircle(DWORD dwId, int xCenter, int yCenter, int nRadius, int nPixel, DWORD FrameColor)
{
	if (bIsDirectX)
	{
		if (drawMod)
		{
			drawMod->DrawCircleSubWindow(dwId, xCenter, yCenter, nRadius, nPixel, FrameColor);
		}
	}
	else
	{
		vRenderingMod[dwId]->m_gdiDraw.DrawCircle(xCenter - nRadius, yCenter - nRadius, nRadius, nPixel, FrameColor);
	}
}


void CRenderingMod::DrawRectangle(DWORD dwId, int xCenter, int yCenter, int nLenOfSide, int nPixel, DWORD FrameColor)
{
	if (bIsDirectX)
	{
		if (drawMod)
		{
			drawMod->DrawRectangleSubWindow(dwId, xCenter, yCenter, nLenOfSide, nPixel, FrameColor);
		}
	}
	else
	{
		vRenderingMod[dwId]->m_gdiDraw.DrawRectangle(xCenter, yCenter, nLenOfSide, nPixel, FrameColor);
	}
}


void CRenderingMod::DrawLines(DWORD dwId, const HV_Line2D* pLineArray, DWORD dwLineCount, DWORD dwColor)
{
	if (bIsDirectX)
	{
		if (drawMod)
		{
			drawMod->Draw2DLineSubWindow(dwId, pLineArray, dwLineCount, dwColor);
		}
	}
	else
	{
		vRenderingMod[dwId]->m_gdiDraw.Draw2Dline(pLineArray, dwLineCount, dwColor);
	}
}


unsigned int CRenderingMod::RenderingThreadFunc(void* obj)
{
	HWND hContainer = reinterpret_cast<HWND>(obj);

	// 创建绘制实例
	HWND* hWndArray = NULL;
	int vRenderingModSize = 0;
	DWORD* dwIDArray = NULL;

	csRenderingMod.Lock();
	vRenderingModSize = vRenderingMod.size();
	if (vRenderingModSize > 0)
	{
		hWndArray = new HWND[vRenderingModSize];
		dwIDArray = new DWORD[vRenderingModSize];
		for (int count = 0; count < vRenderingModSize; count++)
		{
			hWndArray[count] = vRenderingMod[count]->GetHWnd();
		}
	}
	if (hWndArray && dwIDArray && drawMod)
	{
		bIsDirectX = drawMod->Create(hContainer, hWndArray, dwIDArray, vRenderingModSize) == FALSE ? false : true;

		//bIsDirectX = FALSE;
        if (bIsDirectX)
		{
			for(int count = 0; count < vRenderingModSize; count++)
			{
				vRenderingMod[count]->dwID = dwIDArray[count];			// 设置使用模块ID
			}
		}
		else
		{
			for(int count = 0; count < vRenderingModSize; count++)
			{
				vRenderingMod[count]->m_gdiDraw.Create(hWndArray[count]);
				vRenderingMod[count]->dwID = count;
			}
		}
        // wangjie 10/15
        //for(int count = 0; count < vRenderingModSize; count++)
        //{
        //    vRenderingMod[count]->m_gdiDraw.Create(hWndArray[count]);
        //    vRenderingMod[count]->dwID = count;
        //}
	}
	csRenderingMod.Unlock();

	//	FILE* file = fopen("log.txt", "a");
	//	int tick = 0, tickOld = 0;
	while(!bExitThread)
	{
		csRenderingMod.Lock();
		vRenderingModSize = vRenderingMod.size();
		//		tick = ::GetTickCount();
		for (int count = 0; count < vRenderingModSize; count++)
		{
			//			fprintf(file, "queue:%d stack:%d ", vRenderingMod[count]->renderingBufferQueue.Size(), vRenderingMod[count]->renderObjectStack.Size());
			vRenderingMod[count]->DoWork();
		}
		//		tick = ::GetTickCount();
		//		fprintf(file, "%d\n", tick - tickOld);
		//		tickOld = tick;
		csRenderingMod.Unlock();
		::Sleep(5);
	}
	//	fclose(file);

	if (hWndArray)
	{
		// 删除绘制窗口句柄数组
		delete[] hWndArray;
		hWndArray = NULL;
	}

	if (dwIDArray)
	{
		// 删除绘制模块ID数组
		delete[] dwIDArray;
		dwIDArray = NULL;
	}

	return 0;
}

void CRenderingMod::DoWork(void)
{
	do 
	{
		if (!bStart)
		{
			ClearTrajectory();
			if (bIsDirectX)
			{
				drawMod->ClearSubWindow(dwID, 0x00009100);
			}
			else
			{
				m_gdiDraw.Clear(0x00009100);
			}
			// 未开始绘制视频
			Present();
			break;
		}

		nCount++;
		if (renderingBufferQueue.Size() >= 1)
		{
			clearBuffer = true;
			// 帧数缓冲不少于1帧
			renderingBufferQueue.Pop(nextRenderObject);

			PrepareRender();	// 准备绘制需要的数据
			nCount = 0;
		}

		if (!curRenderObject || nCount >= 4000 / RENDERING_INTERVAL)
		{
			drawMod->ClearSubWindow(dwID, 0x00009100);
			HV_Point2D pt;
			pt.x = (DEFAULTWIDTH - 195) * wndWidth / DEFAULTWIDTH;
			pt.y = (DEFAULTHEIGHT - 160) * wndHeight / DEFAULTHEIGHT;
			int width, height;
			width = 8 * wndWidth / DEFAULTWIDTH;
			height = 16 * wndHeight / DEFAULTHEIGHT;
			drawMod->DrawTextSubWindow(dwID, _T("视频丢失"), &pt, 0x00FF0000, width, height);
			ClearTrajectory();                          // 清空保存的跟踪线
			nCount = 4000 / RENDERING_INTERVAL;         // 把计数维持在50,以确保下一轮仍未有视频帧时继续绘制视频已丢失
			if (clearBuffer)
			{
				// 清空缓存的视频帧
				while(renderingBufferQueue.Pop(nextRenderObject))
				{
					PrepareRender();
				}
				nextRenderObject = NULL;
				clearBuffer = false;
			}
			Present();
			break;
		}

		if (nCount == 0)
		{
			RenderVideoFrame();         //绘制视频
			RenderDevTime();            //绘制系统时间
			RenderAlgoET();             //绘制算法效率
			RenderTrajectory(true);     //绘制公共ID和轨迹
            //添加计数器的数据显示
            RenderExternal();           //绘制计数器绘图区域
            RenderAllCountData(true);       //绘制计数器视频通道数据


            RenderEventInfoToDlg();
            //显示绘制
			Present();
		}
		else if (nCount > 9 && nCount < 4000 / RENDERING_INTERVAL)
		{
			RenderVideoFrame();         //绘制视频
			RenderDevTime();            //绘制系统时间
			RenderAlgoET();             //绘制算法效率
			RenderTrajectory(false);    //绘制公共ID和轨迹
            //添加计数器的数据显示
            RenderExternal();           //绘制计数器绘图区域
            RenderAllCountData(false);       //绘制计数器视频通道数据
            RenderEventInfoToDlg();
			Present();
		}
	} while (false);
}

void CRenderingMod::Present(void)
{
	if (!IsWindowVisible(hWndDisplay))
	{
		return;
	}

	if (bIsDirectX && !drawMod->PresentSubWindow(dwID))
	{
		HWND* hWndArray = NULL;
		int vRenderingModSize = 0;
		DWORD* dwIDArray = NULL;

		vRenderingModSize = vRenderingMod.size();
		if (vRenderingModSize > 0)
		{
			hWndArray = new HWND[vRenderingModSize];
			dwIDArray = new DWORD[vRenderingModSize];
			for (int count = 0; count < vRenderingModSize; count++)
			{
				hWndArray[count] = vRenderingMod[count]->GetHWnd();
			}
		}
		if (hWndArray && dwIDArray && drawMod)
		{
			drawMod->Delete();
			bIsDirectX = drawMod->Create(hContainer, hWndArray, dwIDArray, vRenderingModSize) == FALSE ? false : true;

			//bIsDirectX = FALSE;
            if (bIsDirectX)
			{
				for(int count = 0; count < vRenderingModSize; count++)
				{
					vRenderingMod[count]->dwID = dwIDArray[count];			// 设置使用模块ID
				}
			}
			else
			{
				for(int count = 0; count < vRenderingModSize; count++)
				{
					vRenderingMod[count]->m_gdiDraw.Delete();
					vRenderingMod[count]->m_gdiDraw.Create(hWndArray[count]);
					vRenderingMod[count]->dwID = count;
				}
			}

            //wangjie 10/15
            //for(int count = 0; count < vRenderingModSize; count++)
            //{
            //    vRenderingMod[count]->m_gdiDraw.Delete();
            //    vRenderingMod[count]->m_gdiDraw.Create(hWndArray[count]);
            //    vRenderingMod[count]->dwID = count;
            //}
		}
		if (hWndArray)
		{
			delete[] hWndArray;
		}
		if (dwIDArray)
		{
			delete[] dwIDArray;
		}
	}
	else if (!bIsDirectX)
	{
		m_gdiDraw.Present();
	}
}

void CRenderingMod::PrepareRender(void)
{
	if (curRenderObject)
	{
		curRenderObject->Reset();
		renderObjectStack.Push(curRenderObject);
	}
	curRenderObject = nextRenderObject;
	nextRenderObject = NULL;
}

void CRenderingMod::RenderVideoFrame(void)
{
	if (!IsWindowVisible(hWndDisplay))
	{
		// 窗体不可视时返回
		return;
	}
	if (bIsDirectX)
	{
		drawMod->ClearSubWindow(dwID, 0x00009100);
	}
	else
	{
		m_gdiDraw.Clear(0x00009100);
	}

	LPBYTE lpData;
	unsigned int dataLen;
	curRenderObject->GetFrameData(lpData, dataLen);

	if (!bIFrame)
	{
		return;
	}
	if (dataLen != 0 && dataLen == curRenderObject->GetWidth() * curRenderObject->GetHeight() * 3 / 2)
	{
		if (bIsDirectX)
		{
			drawMod->DrawYUV420PSubWindow(dwID,
				curRenderObject->GetWidth(),
				curRenderObject->GetHeight(),
				lpData,
				lpData + curRenderObject->GetWidth() * curRenderObject->GetHeight(),
				lpData + curRenderObject->GetWidth() * curRenderObject->GetHeight() * 5 / 4);
		}
		else
		{
			m_gdiDraw.DrawYUV420P(curRenderObject->GetWidth(), curRenderObject->GetHeight(), lpData);
		}
	}
}

void CRenderingMod::RenderExternal(void)
{
	::EnterCriticalSection(&hCs_component);
	if (IsWindowVisible(hWndDisplay) && m_component)
	{
		m_component->OnRenderingProc(dwID, curRenderObject->GetWidth(), curRenderObject->GetHeight(), wndWidth, wndHeight);
	}
	::LeaveCriticalSection(&hCs_component);
}

void CRenderingMod::RenderAllCountData( bool isRetrack )
{
    ::EnterCriticalSection(&hCs_component);
    int nsize = curRenderObject->GetAllCounterDataSize();
    renderData = new char[nsize];
    memset(renderData, NULL, nsize);

    curRenderObject->GetAllCounterData(renderData, nsize);
    if (IsWindowVisible(hWndDisplay) && m_component && (renderData != NULL))
    {
        m_component->OnRenderingAllCounterData(dwID, wndWidth, wndHeight, renderData, isRetrack);
    }
    if (renderData != NULL)
    {
        delete []renderData;
        renderData = NULL;
    }
    ::LeaveCriticalSection(&hCs_component);
}

void CRenderingMod::RenderDevTime(void)
{
	if (!m_showSysTime || !IsWindowVisible(hWndDisplay))
	{
		// 不绘制设备系统时间
		return;
	}

	unsigned char hour, minute, second, tenmillisecond;
	curRenderObject->GetDevTime(hour, minute, second, tenmillisecond);

	TCHAR buffer[50];
	HV_Point2D pt;
	pt.x = (DEFAULTWIDTH - 70) * wndWidth / DEFAULTWIDTH;
	pt.y = 5 * wndHeight / DEFAULTHEIGHT;
	int width, height;
	width = 8 * wndWidth / DEFAULTWIDTH;
	height = (bIsDirectX ? 16 : 12) * wndHeight / DEFAULTHEIGHT;
	int result = _stprintf_s(buffer, 50, _T("%02d:%02d:%02d"), hour, minute, second);
	assert(result != -1);
	if (bIsDirectX)
	{
        drawMod->DrawTextSubWindow(dwID, buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
	}
	else
	{
		m_gdiDraw.DrawText(buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
	}
}

void CRenderingMod::DrawTimingPassFlow( int dwID, int wndWidth, int wndHeight, int inPC, int outPC )
{
    TCHAR buffer[50];
    HV_Point2D pt;
    pt.x = 280 * wndWidth / DEFAULTWIDTH;  
    pt.y = 240 * wndHeight / DEFAULTHEIGHT;  

    int width, height;
    //字体大小
    width = 8 * wndWidth / DEFAULTWIDTH;
    height = (bIsDirectX ? 16 : 12) * wndHeight / DEFAULTHEIGHT;

    int result = _stprintf_s(buffer, 50, _T("进:%d"), inPC);
    assert(result != -1);
    if (bIsDirectX)
    {
        drawMod->DrawTextSubWindow(dwID, buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
    }
    else
    {
        vRenderingMod[dwID]->m_gdiDraw.DrawText(buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
    }

    pt.y = 260 * wndHeight / DEFAULTHEIGHT;

    result = _stprintf_s(buffer, 50, _T("出:%d"), outPC);
    assert(result != -1);
    if (bIsDirectX)
    {
        drawMod->DrawTextSubWindow(dwID, buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
    }
    else
    {
        vRenderingMod[dwID]->m_gdiDraw.DrawText(buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
    }
}

void CRenderingMod::DrawDelayInfo( int dwID, int wndWidth, int wndHeight, int delayNum, int maxtime, int totaltime )
{
    TCHAR buffer[50];
    memset(buffer,0,50);
    HV_Point2D pt;
    pt.x = 280 * wndWidth / DEFAULTWIDTH;
    pt.y = 220 * wndHeight / DEFAULTHEIGHT;
    int width, height;
    width = 8 * wndWidth / DEFAULTWIDTH;
    height = (bIsDirectX ? 16 : 12) * wndHeight / DEFAULTHEIGHT;
    int result = _stprintf_s(buffer, 50, _T("D:%d"), delayNum);
    assert(result != -1);
    if (bIsDirectX)
    {
        if (drawMod)
        {
            drawMod->DrawTextSubWindow(dwID, buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
        }
    }
    else
    {
        vRenderingMod[dwID]->m_gdiDraw.DrawText(buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
    }
    // 最大
    pt.y = 240 * wndHeight / DEFAULTHEIGHT;

    result = _stprintf_s(buffer, 50, _T("M:%d"), maxtime);
    assert(result != -1);

    if (bIsDirectX)
    {
        drawMod->DrawTextSubWindow(dwID, buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
    }
    else
    {
        vRenderingMod[dwID]->m_gdiDraw.DrawText(buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
    }

    // 总时间
    pt.y = 260 * wndHeight / DEFAULTHEIGHT;

    result = _stprintf_s(buffer, 50, _T("T:%d"), totaltime);
    assert(result != -1);

    if (bIsDirectX)
    {
        drawMod->DrawTextSubWindow(dwID, buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
    }
    else
    {
        vRenderingMod[dwID]->m_gdiDraw.DrawText(buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
    }
}

void CRenderingMod::DrawTotalEventInfo( int dwID, int wndWidth, int wndHeight, int inNum, int outNum )
{
    TCHAR buffer[50];
    HV_Point2D pt;
    pt.x = 280 * wndWidth / DEFAULTWIDTH;  
    pt.y = 240 * wndHeight / DEFAULTHEIGHT;  

    int width, height;
    //字体大小
    width = 8 * wndWidth / DEFAULTWIDTH;
    height = (bIsDirectX ? 16 : 12) * wndHeight / DEFAULTHEIGHT;

    int result = _stprintf_s(buffer, 50, _T("In :%d"), inNum);
    assert(result != -1);
    if (bIsDirectX)
    {
        drawMod->DrawTextSubWindow(dwID, buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
    }
    else
    {
        vRenderingMod[dwID]->m_gdiDraw.DrawText(buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
    }

    //vRenderingMod[dwID]->m_gdiDraw.DrawText(buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);

    pt.y = 260 * wndHeight / DEFAULTHEIGHT;

    result = _stprintf_s(buffer, 50, _T("Out:%d"), outNum);
    assert(result != -1);
    if (bIsDirectX)
    {
        drawMod->DrawTextSubWindow(dwID, buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
    }
    else
    {
        vRenderingMod[dwID]->m_gdiDraw.DrawText(buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
    }
}

/*****************************************************************************
* 说明 : 绘制过店累计数据
* 作者 : WangJie
* 日期 : [2014-9-26]
* 参数 : 
*****************************************************************************/
void CRenderingMod::DrawTimingCross( int dwID, int wndWidth, int wndHeight, int crossNum )
{
    TCHAR buffer[50];
    HV_Point2D pt;
    pt.x = 280 * wndWidth / DEFAULTWIDTH;  
    pt.y = 260 * wndHeight / DEFAULTHEIGHT;  

    int width, height;
    //字体大小
    width = 8 * wndWidth / DEFAULTWIDTH;
    height = (bIsDirectX ? 16 : 12) * wndHeight / DEFAULTHEIGHT;

    int result = _stprintf_s(buffer, 50, _T("过:%d"), crossNum);
    assert(result != -1);

    if (bIsDirectX)
    {
        drawMod->DrawTextSubWindow(dwID, buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
    }
    else
    {
        vRenderingMod[dwID]->m_gdiDraw.DrawText(buffer, &pt, 0x00FFFF00, width, height, FALSE, FALSE);
    }
}

void CRenderingMod::RenderTrajectory(bool bRefreshTrajectory)
{
	// 刷新跟踪线
	std::vector<unsigned int> trajectoryObjectsID;		// 保存本帧带有的所有对象ID
	CObjLocInfo* objLoc;
	unsigned int len;
	if (curRenderObject->GetObjLoc(objLoc, len))
	{
		for (unsigned int count = 0; count < len; count++)
		{
			trajectoryObjectsID.push_back(objLoc[count].objID);												// 保存本帧带有的所有跟踪点
			if (bRefreshTrajectory)
			{
				// 更新跟踪线
				if (trajectory[objLoc[count].objID].size()
					&& (trajectory[objLoc[count].objID].back().first == objLoc[count].x)
					&& (trajectory[objLoc[count].objID].back().second == objLoc[count].y))
				{
					continue;
				}
				trajectory[objLoc[count].objID].push_back(std::pair<unsigned short, unsigned short>(objLoc[count].x, objLoc[count].y));		// 添加点至key所对应数组中
			}
		}

		// 遍历维护着的跟踪线，清除已经不使用的跟踪线
		std::vector<unsigned int>::iterator vIter;
		for (std::map<unsigned int, std::vector<std::pair<unsigned short,unsigned short>>>::iterator iter = trajectory.begin(); iter != trajectory.end();)
		{
			unsigned int num = iter->first;
			vIter = find(trajectoryObjectsID.begin(), trajectoryObjectsID.end(), iter->first);
			if (vIter == trajectoryObjectsID.end())
			{
				iter->second.clear();
				iter = trajectory.erase(iter);
			}
			else
			{
				if (iter->second.size() > 200)
				{
					iter->second.erase(iter->second.begin());
				}
				++iter;
			}
		}
	}

	if (!IsWindowVisible(hWndDisplay))
	{
		// 窗口不可视时返回
		return;
	}

	std::vector<HV_Line2D> vLines;
	HV_Line2D line;
	/*	HV_Line2D* line = new HV_Line2D[4096];
	int lineSize = 0;*/
	HV_Point2D pt;
	TCHAR objectID[50];
	for (std::map<unsigned int, std::vector<std::pair<unsigned short, unsigned short>>>::iterator iter = trajectory.begin();
		iter != trajectory.end(); iter++)
	{
		if (iter->second.size() > 1)
		{
			for (unsigned int count = 0; count < (iter->second.size() - 1); count++)
			{
				line.end.x = iter->second[count].first * wndWidth / curRenderObject->GetWidth();
				line.end.y = iter->second[count].second * wndHeight / curRenderObject->GetHeight();
				line.start.x = iter->second[count + 1].first * wndWidth / curRenderObject->GetWidth();
				line.start.y = iter->second[count + 1].second * wndHeight / curRenderObject->GetHeight();
				vLines.push_back(line);
				/*				line[lineSize].end.x = iter->second[count].first * wndWidth / curRenderObject->GetWidth();
				line[lineSize].end.y = iter->second[count].second * wndHeight / curRenderObject->GetHeight();
				line[lineSize].start.x = iter->second[count + 1].first * wndWidth / curRenderObject->GetWidth();
				line[lineSize].start.y = iter->second[count + 1].second * wndHeight / curRenderObject->GetHeight();
				lineSize ++;*/
			}
		}
		else if (iter->second.size() > 0)
		{
			line.start.x = iter->second[0].first * wndWidth / curRenderObject->GetWidth();
			line.start.y = iter->second[0].second * wndHeight / curRenderObject->GetHeight();
			line.end.x = iter->second[0].first * wndWidth / curRenderObject->GetWidth();
			line.end.y = iter->second[0].second * wndHeight / curRenderObject->GetHeight();vLines.push_back(line);
			/*			line[lineSize].start.x = iter->second[0].first * wndWidth / curRenderObject->GetWidth();
			line[lineSize].start.y = iter->second[0].second * wndHeight / curRenderObject->GetHeight();
			line[lineSize].end.x = iter->second[0].first * wndWidth / curRenderObject->GetWidth();
			line[lineSize].end.y = iter->second[0].second * wndHeight / curRenderObject->GetHeight();
			lineSize ++;*/
		}

		if (iter->second.size() > 0)
		{
			pt.x = iter->second[iter->second.size() - 1].first * wndWidth / curRenderObject->GetWidth();
			pt.y = iter->second[iter->second.size() - 1].second * wndHeight / curRenderObject->GetHeight();
			if (m_showObjectID)
			{
				// 绘制对象ID
				int result = _stprintf_s(objectID, 50, _T("%d"), iter->first);
				assert(result != -1);
				if (bIsDirectX)
				{
					drawMod->DrawTextSubWindow(dwID, objectID, &pt, 0x00ff0000, 10 * wndWidth / DEFAULTWIDTH, 10 * wndHeight / DEFAULTHEIGHT, FALSE, FALSE);
				}
				else
				{
					m_gdiDraw.DrawText(objectID, &pt, 0x00ff0000, 10 * wndWidth / DEFAULTWIDTH, 10 * wndHeight / DEFAULTHEIGHT, FALSE, FALSE);
				}
			}
		}
	}

	if (m_showTrajectory && vLines.size())
		//	if (m_showTrajectory && lineSize > 0)
	{
		if (bIsDirectX)
		{
			drawMod->Draw2DLineSubWindow(dwID, &vLines[0], vLines.size(), 0x000000ff);
		}
		else
		{
			m_gdiDraw.Draw2Dline(&vLines[0], vLines.size(), 0x000000ff);
		}
	}

	//	delete[] line;
}
//
//void CRenderingMod::ClearCounterTrack( void )
//{
//    for (std::map<unsigned int, std::vector<std::pair<unsigned short,unsigned short>>>::iterator iter = counterTrack.begin(); iter != counterTrack.end();)
//    {
//        iter = counterTrack.erase(iter);
//    }
//}


void CRenderingMod::RenderTrackCounter(bool bRefreshTrajectory)
{
	// 刷新跟踪线
	std::vector<unsigned int> trajectoryObjectsID;		// 保存本帧带有的所有对象ID
	CObjLocInfo* objLoc;
	unsigned int len;
	if (curRenderObject->GetObjLoc(objLoc, len))
	{
		for (unsigned int count = 0; count < len; count++)
		{
			trajectoryObjectsID.push_back(objLoc[count].objID);												// 保存本帧带有的所有跟踪点
			if (bRefreshTrajectory)
			{
				// 更新跟踪线
				if (trajectory[objLoc[count].objID].size()
					&& (trajectory[objLoc[count].objID].back().first == objLoc[count].x)
					&& (trajectory[objLoc[count].objID].back().second == objLoc[count].y))
				{
					continue;
				}
				trajectory[objLoc[count].objID].push_back(std::pair<unsigned short, unsigned short>(objLoc[count].x, objLoc[count].y));		// 添加点至key所对应数组中
			}
		}

		// 遍历维护着的跟踪线，清除已经不使用的跟踪线
		std::vector<unsigned int>::iterator vIter;
		for (std::map<unsigned int, std::vector<std::pair<unsigned short,unsigned short>>>::iterator iter = trajectory.begin(); iter != trajectory.end();)
		{
			unsigned int num = iter->first;
			vIter = find(trajectoryObjectsID.begin(), trajectoryObjectsID.end(), iter->first);
			if (vIter == trajectoryObjectsID.end())
			{
				iter->second.clear();
				iter = trajectory.erase(iter);
			}
			else
			{
				if (iter->second.size() > 200)
				{
					iter->second.erase(iter->second.begin());
				}
				++iter;
			}
		}
	}

	if (!IsWindowVisible(hWndDisplay))
	{
		// 窗口不可视时返回
		return;
	}

	std::vector<HV_Line2D> vLines;
	HV_Line2D line;
	/*	HV_Line2D* line = new HV_Line2D[4096];
	int lineSize = 0;*/
	HV_Point2D pt;
	TCHAR objectID[50];
	for (std::map<unsigned int, std::vector<std::pair<unsigned short, unsigned short>>>::iterator iter = trajectory.begin();
		iter != trajectory.end(); iter++)
	{
		if (iter->second.size() > 1)
		{
			for (unsigned int count = 0; count < (iter->second.size() - 1); count++)
			{
				line.end.x = iter->second[count].first * wndWidth / curRenderObject->GetWidth();
				line.end.y = iter->second[count].second * wndHeight / curRenderObject->GetHeight();
				line.start.x = iter->second[count + 1].first * wndWidth / curRenderObject->GetWidth();
				line.start.y = iter->second[count + 1].second * wndHeight / curRenderObject->GetHeight();
				vLines.push_back(line);
				/*				line[lineSize].end.x = iter->second[count].first * wndWidth / curRenderObject->GetWidth();
				line[lineSize].end.y = iter->second[count].second * wndHeight / curRenderObject->GetHeight();
				line[lineSize].start.x = iter->second[count + 1].first * wndWidth / curRenderObject->GetWidth();
				line[lineSize].start.y = iter->second[count + 1].second * wndHeight / curRenderObject->GetHeight();
				lineSize ++;*/
			}
		}
		else if (iter->second.size() > 0)
		{
			line.start.x = iter->second[0].first * wndWidth / curRenderObject->GetWidth();
			line.start.y = iter->second[0].second * wndHeight / curRenderObject->GetHeight();
			line.end.x = iter->second[0].first * wndWidth / curRenderObject->GetWidth();
			line.end.y = iter->second[0].second * wndHeight / curRenderObject->GetHeight();vLines.push_back(line);
			/*			line[lineSize].start.x = iter->second[0].first * wndWidth / curRenderObject->GetWidth();
			line[lineSize].start.y = iter->second[0].second * wndHeight / curRenderObject->GetHeight();
			line[lineSize].end.x = iter->second[0].first * wndWidth / curRenderObject->GetWidth();
			line[lineSize].end.y = iter->second[0].second * wndHeight / curRenderObject->GetHeight();
			lineSize ++;*/
		}

		if (iter->second.size() > 0)
		{
			pt.x = iter->second[iter->second.size() - 1].first * wndWidth / curRenderObject->GetWidth();
			pt.y = iter->second[iter->second.size() - 1].second * wndHeight / curRenderObject->GetHeight();
			if (m_showObjectID)
			{
				// 绘制对象ID
				int result = _stprintf_s(objectID, 50, _T("%d"), iter->first);
				assert(result != -1);
				if (bIsDirectX)
				{
					drawMod->DrawTextSubWindow(dwID, objectID, &pt, 0x007FFF00, 10 * wndWidth / DEFAULTWIDTH, 10 * wndHeight / DEFAULTHEIGHT, FALSE, FALSE);
				}
				else
				{
					m_gdiDraw.DrawText(objectID, &pt, 0x007FFF00, 10 * wndWidth / DEFAULTWIDTH, 10 * wndHeight / DEFAULTHEIGHT, FALSE, FALSE);
				}
			}
		}
	}

	if (m_showTrajectory && vLines.size())
		//	if (m_showTrajectory && lineSize > 0)
	{
		if (bIsDirectX)
		{
			drawMod->Draw2DLineSubWindow(dwID, &vLines[0], vLines.size(), 0x00FFBBFF);
		}
		else
		{
			m_gdiDraw.Draw2Dline(&vLines[0], vLines.size(), 0x00FFBBFF);
		}
	}

	//	delete[] line;
}


//void CRenderingMod::DrawIDTrack( int dwID, int wndWidth, int wndHeight, VECT_TRACK vObjCount, bool isRetrack)
//{
//    // 刷新跟踪线
//    std::vector<unsigned int> trajectoryObjectsID;		// 保存本帧带有的所有对象ID
//    CObjLocInfo* objLoc = NULL;
//    unsigned int len = vObjCount.size();
//    if (vObjCount.size() != 0)
//    {
//        objLoc = vObjCount.size() ? &vObjCount[0] : NULL;
//
//        for (unsigned int count = 0; count < len; count++)
//        {
//            trajectoryObjectsID.push_back(objLoc[count].objID);												// 保存本帧带有的所有跟踪点
//            if (isRetrack)
//            {
//                // 更新跟踪线
//                if (counterTrack[objLoc[count].objID].size()
//                    && (counterTrack[objLoc[count].objID].back().first == objLoc[count].x)
//                    && (counterTrack[objLoc[count].objID].back().second == objLoc[count].y))
//                {
//                    continue;
//                }
//                counterTrack[objLoc[count].objID].push_back(std::pair<unsigned short, unsigned short>(objLoc[count].x, objLoc[count].y));		// 添加点至key所对应数组中
//            }
//        }
//
//        // 遍历维护着的跟踪线，清除已经不使用的跟踪线和ID
//        std::vector<unsigned int>::iterator vIter;
//        for (std::map<unsigned int, std::vector<std::pair<unsigned short,unsigned short>>>::iterator iter = counterTrack.begin(); iter != counterTrack.end();)
//        {
//            unsigned int num = iter->first;
//            vIter = find(trajectoryObjectsID.begin(), trajectoryObjectsID.end(), num);
//            if (vIter == trajectoryObjectsID.end())
//            {
//                iter->second.clear();
//                iter = counterTrack.erase(iter);
//            }
//            else
//            {
//                if (iter->second.size() > 200)
//                {
//                    iter->second.erase(iter->second.begin());
//                }
//                ++iter;
//            }
//        }
//    }
//
//    if (!IsWindowVisible(vRenderingMod[dwID]->hWndDisplay))
//    {
//        // 窗口不可视时返回
//        return;
//    }
//
//    std::vector<HV_Line2D> vLines;
//    HV_Line2D line;
//    HV_Point2D pt;
//    TCHAR objectID[50];
//    for (std::map<unsigned int, std::vector<std::pair<unsigned short, unsigned short>>>::iterator iter = counterTrack.begin();
//        iter != counterTrack.end(); iter++)
//    {
//        if (iter->second.size() > 1)
//        {
//            for (unsigned int count = 0; count < (iter->second.size() - 1); count++)
//            {
//                line.end.x = iter->second[count].first * wndWidth / vRenderingMod[dwID]->curRenderObject->GetWidth();
//                line.end.y = iter->second[count].second * wndHeight / vRenderingMod[dwID]->curRenderObject->GetHeight();
//                line.start.x = iter->second[count + 1].first * wndWidth / vRenderingMod[dwID]->curRenderObject->GetWidth();
//                line.start.y = iter->second[count + 1].second * wndHeight / vRenderingMod[dwID]->curRenderObject->GetHeight();
//                vLines.push_back(line);
//            }
//        }
//        else if (iter->second.size() > 0)
//        {
//            line.start.x = iter->second[0].first * wndWidth / vRenderingMod[dwID]->curRenderObject->GetWidth();
//            line.start.y = iter->second[0].second * wndHeight / vRenderingMod[dwID]->curRenderObject->GetHeight();
//            line.end.x = iter->second[0].first * wndWidth / vRenderingMod[dwID]->curRenderObject->GetWidth();
//            line.end.y = iter->second[0].second * wndHeight / vRenderingMod[dwID]->curRenderObject->GetHeight();
//            vLines.push_back(line);
//        }
//
//        if (iter->second.size() > 0)
//        {
//            pt.x = iter->second[iter->second.size() - 1].first * wndWidth / vRenderingMod[dwID]->curRenderObject->GetWidth();
//            pt.y = iter->second[iter->second.size() - 1].second * wndHeight / vRenderingMod[dwID]->curRenderObject->GetHeight();
//
//            // 绘制对象ID
//            int result = _stprintf_s(objectID, 50, _T("%d"), iter->first);
//            assert(result != -1);
//            if (bIsDirectX)
//            {
//                drawMod->DrawTextSubWindow(dwID, objectID, &pt, 0x007FFF00, 10 * wndWidth / DEFAULTWIDTH, 10 * wndHeight / DEFAULTHEIGHT, FALSE, FALSE);
//            }
//            else
//            {
//                vRenderingMod[dwID]->m_gdiDraw.DrawText(objectID, &pt, 0x007FFF00, 10 * wndWidth / DEFAULTWIDTH, 10 * wndHeight / DEFAULTHEIGHT, FALSE, FALSE);
//            }
//        }
//    }
//
//    if (vLines.size())
//    {
//        if (bIsDirectX)
//        {
//            drawMod->Draw2DLineSubWindow(dwID, &vLines[0], vLines.size(), 0x00FFBBFF);
//        }
//        else
//        {
//            vRenderingMod[dwID]->m_gdiDraw.Draw2Dline(&vLines[0], vLines.size(), 0x00FFBBFF);
//        }
//    }
//}

void CRenderingMod::ClearTrajectory(void)
{
	for (std::map<unsigned int, std::vector<std::pair<unsigned short,unsigned short>>>::iterator iter = trajectory.begin(); iter != trajectory.end();)
	{
		iter = trajectory.erase(iter);
	}
}


void CRenderingMod::RenderAlgoET(void)
{
	if (!m_showAlgoET || !IsWindowVisible(hWndDisplay))
	{
		// 不需要显示算法效率
		return;
	}

	unsigned short algoET;
	algoET = curRenderObject->GetAlgoET();

	TCHAR buffer[50];
	HV_Point2D pt;
	pt.x = (DEFAULTWIDTH - 92) * wndWidth / DEFAULTWIDTH;
	pt.y = (DEFAULTHEIGHT - 18) * wndHeight / DEFAULTHEIGHT;
	int width, height;
	width = 8 * wndWidth / DEFAULTWIDTH;
	height = (bIsDirectX ? 16 : 12) * wndHeight / DEFAULTHEIGHT;
	int result = _stprintf_s(buffer, 50, _T("%8dms"), algoET);
	assert(result != -1);
	if (bIsDirectX)
	{
		drawMod->DrawTextSubWindow(dwID, buffer, &pt, 0x0000ff00, width, height, FALSE, FALSE);
	}
	else
	{
		m_gdiDraw.DrawText(buffer, &pt, 0x0000ff00, width, height, FALSE, FALSE);
	}
}

CRenderObject* CRenderingMod::GetWaitPackage(unsigned int nFrameNo)
{
	do 
	{
		if (waitPackage)
		{
			if (waitPackage->GetFrameNo() != nFrameNo)
			{
				// 帧序号与当前的不对
				renderingBufferQueue.Push(waitPackage);		// 把等待数据的包添加至绘制缓冲队列
				waitPackage = NULL;
			}
			else
			{
				// 当前对象正是所需要的帧绘制对象
				break;
			}
		}
		else if (m_nIgnoreFrameNo == nFrameNo)
		{
			// 当前帧已被标记为忽略
			break;
		}

		if (!renderObjectStack.Pop(waitPackage))
		{
			m_nIgnoreFrameNo = nFrameNo;	// 尝试获取绘制实例却失败的帧将自动忽略余下的数据
			waitPackage = NULL;
			break;
		}
		waitPackage->Reset();
		waitPackage->SetFrameNo(nFrameNo);
	} while (false);
	return waitPackage;
}

void CRenderingMod::WriteRecord(LPBYTE lpByte, unsigned int nLen)
{
	if (aviFile.isOpen())
	{
		::EnterCriticalSection(&m_csRecordOp);
		aviFile.AVI_Write_frame(reinterpret_cast<char*>(lpByte), nLen);
		::LeaveCriticalSection(&m_csRecordOp);
	}
}

void CRenderingMod::ProcAllCounterData( unsigned int nFrameNo, const char* allCounterData, int nlen )
{
    CRenderObject* obj = GetWaitPackage(nFrameNo);
    if (obj != NULL)
    {
        obj->SetAllCounterData(allCounterData, nlen);
    }
}

void CRenderingMod::RenderEventInfoToDlg( void )
{
    if (hPCWnd != NULL)
    {
        //::PostMessage(hPCWnd, CDM_EVENT_COUNT, 0, 0);
    }
}

