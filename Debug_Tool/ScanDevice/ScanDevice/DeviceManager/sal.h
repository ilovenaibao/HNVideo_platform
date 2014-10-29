//-----------------------------------------------------------------------------
// Purpose: 系统软件环境抽象层（屏蔽系统编程环境的细节）
// History:
//     yxs  2011/06/18 13:58 created new file
//-----------------------------------------------------------------------------
#pragma once

// 临界区
class CritSec
{
private:
    CritSec(const CritSec & refCritSec);
    CritSec & operator = (const CritSec & refCritSec);

    CRITICAL_SECTION    m_CritSec;
    DWORD               m_dwLineEnter;
    BOOL                m_fValid;

public:
    CritSec()
    {
        __try
        {
            ::InitializeCriticalSection(&m_CritSec);
            m_fValid = TRUE;
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            m_fValid = FALSE;
        }
    }

    ~CritSec()
    {
        ::DeleteCriticalSection(&m_CritSec);
    }

    void Lock(DWORD dwLine)
    {
        __try
        {
            ::EnterCriticalSection(&m_CritSec);
            m_dwLineEnter = dwLine;
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
        }
    }

    void Unlock()
    {
        ::LeaveCriticalSection(&m_CritSec);
    }

    CRITICAL_SECTION * GetCritSec()
    {
        return &m_CritSec;
    }

#define LOCK() Lock(__LINE__)
#define UNLOCK() Unlock()
};


// 自动锁
class AutoLock
{
    AutoLock(const AutoLock & refAutoLock);
    AutoLock & operator = (const AutoLock &refAutoLock);

protected:
    CritSec *     m_pLock;
    BOOL            m_bLocked;

public:
    AutoLock(CritSec * plock)
    {
        m_pLock = plock;
        m_pLock->Lock(0);
        m_bLocked = TRUE;
    }

    ~AutoLock()
    {
        if (m_pLock && m_bLocked)
            m_pLock->Unlock();
    }

    void Unlock()
    {
        if (m_pLock && m_bLocked)
        {
            m_pLock->Unlock();
            m_bLocked = FALSE;
        }
    }

    void Lock()
    {
        if (m_pLock && (!m_bLocked))
        {
            m_pLock->LOCK();
            m_bLocked = TRUE;
        }
    }
};
