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
	* @brief	���
	*/
	void Push(T& item)
	{
		::EnterCriticalSection(&hCs);
		innerQueue.push(item);
		::LeaveCriticalSection(&hCs);
	}

	/**
	* @brief	����
	* @return	�������в�����Ԫ��ʱ����false��Ԫ�سɹ���ջʱΪtrue
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
	* @brief	��ȡ������Ԫ�ص�����
	* @return	�Ѽ��������Ԫ�ص�����
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