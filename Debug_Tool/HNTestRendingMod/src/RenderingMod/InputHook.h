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
	* @brief	��ʼ������ӹ�ģ��
	* @param[in]	hWndContainer���������ھ��
	* @param[in]	hWndDisplay����Ƶ��ʾ���ھ��
	* @param[in]	hWndProcesser�������ӻ�õ���Ϣ�Ĵ��ھ��
	* @param[in]	hWndOperation��������Ч�Ĵ��ھ��
	*/
	void Init(HWND hWndContainer, HWND hWndDisplay, HWND hWndProcesser, HWND hWndOperation);

	/**
	* @brief	���ģ����Դ
	*/
	void Clear(void);

	/**
	* @brief	������Ϣ�����ھ��
	* @param[in]	hWndProcesser����Ϣ�����ھ��
	*/
	void SetProcess(HWND hWndProcesser);

	/**
	* @brief	�ӹ������ļ������������
	* @details	�ӹܺ�����������жϵ����λ�ã����γ���Ƶ��ʾ������ؼ�����λ���������λ�õ���굥��
	* @return	�Ƿ�ɹ��ӹܼ������������
	*/
	bool Hook(void);

	/**
	* @brief	�ͷŽӹܵ���������
	*/
	void UnHook(void);

	/**
	* @brief	������Ƶ��ʾ����
	* @details	�ѳ���ʵ����������Ƶ��ʾ��������������ڽ���
	*/
	void LockDisplay(void);

	/**
	* @brief	���������ѵǼǵ���Ƶ��ʾ����
	*/
	void UnLockDisplay(void);

	/**
	* @brief	��ȡӦ�ô������Ϣ����Ĵ��ھ��
	* @param[in]	x������x
	* @param[in]	y������y
	* @return	������Ϣ����Ĵ��ھ��
	*/
	static HWND GetMsgProcWnd(LONG x, LONG y);

	/**
	* @brief	�����Ҽ���Ϣ�������Ѿ��Ǽǵ���Ϣ�����ھ��
	* @param[in]	x������x
	* @param[in]	y������y
	*/
	static void SendMouseRButtonClick(LONG x, LONG y);

	/**
	* @brief	��������ɿ���Ϣ�������Ѿ��Ǽǵ���Ϣ�����ھ��
	*/
	static void SendMouseLButtonUp(void);

protected:
	HWND	hWndContainer;			// �������ھ��
	HWND	hWndDisplay;			// ��Ƶ��ʾ���ھ��
	HWND	hWndProcesser;			// �����ӻ�õ���Ϣ�Ĵ��ھ��
	HWND	hWndOperation;			// ������Ч�Ĵ��ھ��

	HHOOK	hHook;					// ��Ϣ���˹��Ӿ��

	static CCriticalSection		cs;
	static std::map<HWND, std::pair<HWND, HWND>> mapWndHandle;	// ��һ������Ϊ��Ƶ���ƵĴ��ھ�����ڶ�������Ϊ������Ϣ����Ĵ��ھ��
};

#endif
