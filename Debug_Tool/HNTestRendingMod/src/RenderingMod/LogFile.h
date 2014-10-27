//-----------------------------------------------------------------------------
// Purpose: 日志文件封装类
// History:
//     yanxs  2011/05/25 21:04 created new file
//-----------------------------------------------------------------------------
#pragma once

#define _MAX_PATH   260 /* max. length of full pathname */

enum PRIORITY_LEVEL
{ 	
	PRIORITY_HIGHEST,
	PRIORITY_NORMAL,	
	PRIORITY_LOWEST,	
};

typedef void (CALLBACK *PFuncLogFilter)(const char * logStr);

class CLogFile
{
public:
	enum TraceMode
	{
		TraceToLogFile = 0,
		TraceToConsole,
		TraceToAll,
	};

	CLogFile (char * name);
	~CLogFile (void);

public:
	void WriteLastError (char * sInf); 
	void WriteLog (char * sLog, int priority = PRIORITY_NORMAL);
	void Write (const char * format, ...);
	void Write (int procno, const char *proName, const char * format, ...);

    void OutPutLock(char * string);

	void SetLogFilter (PFuncLogFilter pFunc);

	void Create (char * name);
	void SetPriority (int priority);

	const char* strcasestr(const char* str, const char* subStr);

	static void SetTraceMode (TraceMode mode) { ms_TraceMode = mode; }
	static TraceMode GetTraceMode () { return ms_TraceMode; }

private:
	char        m_fileName[_MAX_PATH];		// 完整文件路径
	char        m_name[255];				// 日志名
	TCHAR       m_path[_MAX_PATH];
	char        m_cpath[_MAX_PATH];
	HANDLE      m_hFile;					// 文件句柄
	char        m_logcache[5120];			// 日志缓冲区
	char        m_logbuf[5120];			    // 日志缓冲区
	int         m_priority;		            // 写日志等级

	OVERLAPPED  m_overlapped;
	tm          m_tmWrite;

	PFuncLogFilter  m_LogFilter;
	
	static TraceMode    ms_TraceMode;

	CRITICAL_SECTION m_cs;
};
