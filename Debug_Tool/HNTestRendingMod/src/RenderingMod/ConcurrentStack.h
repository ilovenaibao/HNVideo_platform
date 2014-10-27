#ifndef _HV_G3PCDT_CONCURRENTSTACK_H_
#define _HV_G3PCDT_CONCURRENTSTACK_H_

#include <stack>

template<class T>
class CConcurrentStack
{
public:
	CConcurrentStack(void)
	{
		::InitializeCriticalSection(&hCs);
	}

	~CConcurrentStack(void)
	{
		::DeleteCriticalSection(&hCs);
	}

	/**
	* @brief	ѹջ
	*/
	void Push(T& item)
	{
		::EnterCriticalSection(&hCs);
		innerStack.push(item);
		::LeaveCriticalSection(&hCs);
	}

	/**
	* @brief	��ջ
	* @return	��ջ�в�����Ԫ��ʱ����false��Ԫ�سɹ���ջʱΪtrue
	*/
	bool Pop(T& item)
	{
		bool result = false;
		::EnterCriticalSection(&hCs);
		if (!innerStack.empty())
		{
			item = innerStack.top();
			innerStack.pop();
			result = true;
		}
		::LeaveCriticalSection(&hCs);
		return result;
	}

	/**
	* @brief	��ȡ��ջ��Ԫ�ص�����
	* @return	�Ѽ����ջ��Ԫ�ص�����
	*/
	int Size(void)
	{
		int result = 0;
		::EnterCriticalSection(&hCs);
		result = innerStack.size();
		::LeaveCriticalSection(&hCs);
		return result;
	}

protected:
	std::stack<T>		innerStack;
	CRITICAL_SECTION	hCs;
};

#endif
