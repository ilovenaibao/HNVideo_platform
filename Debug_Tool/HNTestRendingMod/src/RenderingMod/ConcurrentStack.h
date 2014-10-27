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
	* @brief	压栈
	*/
	void Push(T& item)
	{
		::EnterCriticalSection(&hCs);
		innerStack.push(item);
		::LeaveCriticalSection(&hCs);
	}

	/**
	* @brief	出栈
	* @return	当栈中不存在元素时返回false，元素成功出栈时为true
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
	* @brief	获取堆栈中元素的数量
	* @return	已加入堆栈中元素的数量
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
