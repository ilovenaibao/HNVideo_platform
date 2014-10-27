#include "StdAfx.h"
#include "InputHook.h"

LRESULT CALLBACK MessageProc(int code, WPARAM wParam, LPARAM lParam)
{
	do 
	{
		LPMSG msg = reinterpret_cast<LPMSG>(lParam);

		if (code != MSGF_DIALOGBOX || (msg->message != WM_LBUTTONDOWN && msg->message != WM_LBUTTONUP && msg->message != WM_MOUSEMOVE
			&& msg->message != WM_RBUTTONDOWN && msg->message != WM_KEYDOWN))
		{
			break;
		}

		if (msg->message == WM_LBUTTONDOWN)
		{
			HWND wnd = CInputHook::GetMsgProcWnd(msg->pt.x, msg->pt.y);
			if (wnd != NULL)
			{
				::SendMessage(wnd, MOUSEHOOK_LBUTTONDOWN, msg->pt.x, msg->pt.y);
			}
		}
		else if (msg->message == WM_LBUTTONUP)
		{
			CInputHook::SendMouseLButtonUp();
		}
		else if (msg->message == WM_MOUSEMOVE)
		{
			HWND wnd = CInputHook::GetMsgProcWnd(msg->pt.x, msg->pt.y);
			if (wnd != NULL && msg->wParam == MK_LBUTTON)
			{
				::SendMessage(wnd, MOUSEHOOK_LBUTTONMOVE, msg->pt.x, msg->pt.y);
			}
		}
		else if (msg->message == WM_RBUTTONDOWN)
		{
			CInputHook::SendMouseRButtonClick(msg->pt.x, msg->pt.y);
		}
	} while (false);
	return ::CallNextHookEx(NULL, code, wParam, lParam);
}


CInputHook::CInputHook(void)
	: hWndContainer(NULL)
	, hWndDisplay(NULL)
	, hWndProcesser(NULL)
	, hHook(NULL)
{
}


CInputHook::~CInputHook(void)
{
}


CCriticalSection CInputHook::cs;


std::map<HWND, std::pair<HWND, HWND>> CInputHook::mapWndHandle;


//视频显示窗口  algodlg的窗口句柄  framedlg的窗口句柄
void CInputHook::Init(HWND hWndContainer, HWND hWndDisplay, HWND hWndProcesser, HWND hWndOperation)
{
	if (this->hWndContainer || this->hWndDisplay)
	{
		Clear();
	}

	this->hWndContainer = hWndContainer;
	this->hWndDisplay = hWndDisplay;
	this->hWndProcesser = hWndProcesser;
	this->hWndOperation = hWndOperation;
	cs.Lock();
	std::pair<HWND, HWND> item;
	item.first = hWndProcesser;
	item.second = hWndOperation;
	mapWndHandle[hWndDisplay] = item;
	cs.Unlock();
}


void CInputHook::Clear(void)
{
	UnHook();
	cs.Lock();
	std::map<HWND, std::pair<HWND, HWND>>::iterator it = mapWndHandle.find(hWndDisplay);
	if (it != mapWndHandle.end())
	{
		mapWndHandle.erase(it);
	}
	cs.Unlock();
	hWndContainer = NULL;
	hWndDisplay = NULL;
	hWndProcesser = NULL;
}


void CInputHook::SetProcess(HWND hWndProcesser)
{
	this->hWndProcesser = hWndProcesser;
	cs.Lock();
	std::map<HWND, std::pair<HWND, HWND>>::iterator it = mapWndHandle.find(hWndDisplay);
	if (it != mapWndHandle.end())
	{
		it->second.first = hWndProcesser;
	}
	cs.Unlock();
}


bool CInputHook::Hook()
{
    if (hWndContainer == NULL)
    {
        return false;
    }
	bool result = false;
	do 
	{
		//获取窗口线程号
		DWORD dwThreadId = ::GetWindowThreadProcessId(hWndContainer, NULL);
		if (!dwThreadId)
		{
			break;
		}

		// 注册消息钩子
		hHook = ::SetWindowsHookEx(WH_MSGFILTER, MessageProc, NULL, dwThreadId);
		if (hHook == NULL)
		{
			break;
		}
		result = true;
	} while (false);
	return result;
}


void CInputHook::UnHook(void)
{
	if (hHook)
	{
		::UnhookWindowsHookEx(hHook);
		UnLockDisplay();
	}
}


void CInputHook::LockDisplay(void)
{
	if (hHook)
	{
		cs.Lock();
		for (std::map<HWND, std::pair<HWND, HWND>>::iterator it = mapWndHandle.begin(); it != mapWndHandle.end(); it++)
		{
			if (it->first == hWndDisplay)
			{
				continue;
			}
			::EnableWindow(it->first, FALSE);
		}
		cs.Unlock();
	}
}


void CInputHook::UnLockDisplay(void)
{
	cs.Lock();
	for (std::map<HWND, std::pair<HWND, HWND>>::iterator it = mapWndHandle.begin(); it != mapWndHandle.end(); it++)
	{
		::EnableWindow(it->first, TRUE);
	}
	cs.Unlock();
}


HWND CInputHook::GetMsgProcWnd(LONG x, LONG y)
{
	HWND result = NULL;
	cs.Lock();
	RECT rect, rectOperation;
	for (std::map<HWND, std::pair<HWND, HWND>>::iterator it = mapWndHandle.begin(); it != mapWndHandle.end(); it++)
	{
		if (!::IsWindowVisible(it->first))
		{
			continue;
		}

		::GetWindowRect(it->first, &rect);
		::GetWindowRect(it->second.second, &rectOperation);

		if ((rect.left <= x && rect.right >= x && rect.top <= y && rect.bottom >= y)
			|| (rectOperation.left <= x && rectOperation.right >= x && rectOperation.top <= y && rectOperation.bottom >= y))
		{
			result = it->second.first;
			break;
		}
	}
	cs.Unlock();
	return result;
}


void CInputHook::SendMouseRButtonClick(LONG x, LONG y)
{
	cs.Lock();
	for (std::map<HWND, std::pair<HWND, HWND>>::iterator it = mapWndHandle.begin(); it != mapWndHandle.end(); it++)
	{
		::PostMessage(it->second.first, MOUSEHOOK_RBUTTON, x, y);
	}
	cs.Unlock();
}


void CInputHook::SendMouseLButtonUp(void)
{
	cs.Lock();
	for (std::map<HWND, std::pair<HWND, HWND>>::iterator it = mapWndHandle.begin(); it != mapWndHandle.end(); it++)
	{
		::PostMessage(it->second.first, MOUSEHOOK_LBUTTONUP, NULL, NULL);
	}
	cs.Unlock();
}
