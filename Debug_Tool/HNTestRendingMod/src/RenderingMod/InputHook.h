#ifndef _IPVA_DTC_INPUT_HOOK_H_
#define _IPVA_DTC_INPUT_HOOK_H_

#include <windows.h>
#include <utility>
#include <map>

#define MOUSEHOOK_LBUTTONDOWN	(WM_USER + 200)
#define MOUSEHOOK_LBUTTONUP		(WM_USER + 201)
#define MOUSEHOOK_LBUTTONMOVE	(WM_USER + 202)
#define MOUSEHOOK_RBUTTON	    (WM_USER + 203)

class CInputHook
{
public:
	CInputHook(void);
	~CInputHook(void);

	/**
	* @brief	初始化输入接管模块
	* @param[in]	hWndContainer，容器窗口句柄
	* @param[in]	hWndDisplay，视频显示窗口句柄
	* @param[in]	hWndProcesser，处理钩子获得的消息的窗口句柄
	* @param[in]	hWndOperation，输入有效的窗口句柄
	*/
	void Init(HWND hWndContainer, HWND hWndDisplay, HWND hWndProcesser, HWND hWndOperation);

	/**
	* @brief	清除模块资源
	*/
	void Clear(void);

	/**
	* @brief	设置消息处理窗口句柄
	* @param[in]	hWndProcesser，消息处理窗口句柄
	*/
	void SetProcess(HWND hWndProcesser);

	/**
	* @brief	接管容器的键盘与鼠标输入
	* @details	接管后会依据坐标判断点击的位置，屏蔽除视频显示窗口与控件所在位置外的所有位置的鼠标单击
	* @return	是否成功接管键盘与鼠标输入
	*/
	bool Hook(void);

	/**
	* @brief	释放接管的容器输入
	*/
	void UnHook(void);

	/**
	* @brief	禁用视频显示窗口
	* @details	把除本实例关联的视频显示窗口外的其他窗口禁用
	*/
	void LockDisplay(void);

	/**
	* @brief	启用所有已登记的视频显示窗口
	*/
	void UnLockDisplay(void);

	/**
	* @brief	获取应该处理此消息处理的窗口句柄
	* @param[in]	x，坐标x
	* @param[in]	y，坐标y
	* @return	返回消息处理的窗口句柄
	*/
	static HWND GetMsgProcWnd(LONG x, LONG y);

	/**
	* @brief	发送右键消息给所有已经登记的消息处理窗口句柄
	* @param[in]	x，坐标x
	* @param[in]	y，坐标y
	*/
	static void SendMouseRButtonClick(LONG x, LONG y);

	/**
	* @brief	发送左键松开消息给所有已经登记的消息处理窗口句柄
	*/
	static void SendMouseLButtonUp(void);

protected:
	HWND	hWndContainer;			// 容器窗口句柄
	HWND	hWndDisplay;			// 视频显示窗口句柄
	HWND	hWndProcesser;			// 处理钩子获得的消息的窗口句柄
	HWND	hWndOperation;			// 操作有效的窗口句柄

	HHOOK	hHook;					// 消息过滤钩子句柄

	static CCriticalSection		cs;
	static std::map<HWND, std::pair<HWND, HWND>> mapWndHandle;	// 第一个参数为视频绘制的窗口句柄，第二个参数为进行消息处理的窗口句柄
};

#endif
