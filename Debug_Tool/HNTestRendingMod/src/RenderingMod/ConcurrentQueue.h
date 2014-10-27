#ifndef _HV_G3PCDT_CONCURRENTQUEUE_H_
#define _HV_G3PCDT_CONCURRENTQUEUE_H_

#include <queue>

template<class T>
class CConcurrentQueue
{
public:
	CConcurrentQueue(void)
	{
		::InitializeCriticalSection(&hCs);
	}

	~CConcurrentQueue(void)
	{
		::DeleteCriticalSection(&hCs);
	}

	/**
	* @brief	入队
	*/
	void Push(T& item)
	{
		::EnterCriticalSection(&hCs);
		innerQueue.push(item);
		::LeaveCriticalSection(&hCs);
	}

	/**
	* @brief	出队
	* @return	当队列中不存在元素时返回false，元素成功出栈时为true
	*/
	bool Pop(T& item)
	{
		bool result = false;
		::EnterCriticalSection(&hCs);
		if (!innerQueue.empty())
		{
			item = innerQueue.front();
			innerQueue.pop();
			result = true;
		}
		::LeaveCriticalSection(&hCs);
		return result;
	}

	/**
	* @brief	获取队列中元素的数量
	* @return	已加入队列中元素的数量
	*/
	int Size(void)
	{
		int result = 0;
		::EnterCriticalSection(&hCs);
		result = innerQueue.size();
		::LeaveCriticalSection(&hCs);
		return result;
	}

protected:
	std::queue<T>		innerQueue;
	CRITICAL_SECTION	hCs;
};


#endif