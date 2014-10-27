#include "StdAfx.h"
#include "logfile.h"

#ifdef _DEBUG
CLogFile::TraceMode CLogFile::ms_TraceMode = TraceToAll;
#else
CLogFile::TraceMode CLogFile::ms_TraceMode = TraceToLogFile;
#endif


CLogFile::CLogFile (char * name)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	m_priority = PRIORITY_NORMAL;
	strcpy (m_name, name);
	m_hFile = 0;
	GetModuleFileName (NULL, m_path, 256);

	size_t i;
	mbstate_t    mbstate;
	::memset((void*)&mbstate, 0, sizeof(mbstate));

	const wchar_t* wt = m_path;
	wcsrtombs_s(&i, m_cpath, MAX_PATH, &wt, MAX_PATH,&mbstate);
	
	_splitpath (m_cpath, drive, dir, fname, ext);

	sprintf(m_cpath, "%s%sLog", drive, dir);
 
	memset(m_path,0,sizeof(m_path));  
	MultiByteToWideChar(CP_ACP,0,m_cpath,strlen(m_cpath)+1,m_path,  
		sizeof(m_path)/sizeof(m_path[0]));
	
	CreateDirectory (m_path, NULL);
	InitializeCriticalSection (&m_cs);
	m_LogFilter = NULL;
}

CLogFile::~CLogFile (void)
{
	if (m_hFile)
		CloseHandle (m_hFile);
	DeleteCriticalSection (&m_cs);
}

// 将日志数据写入缓冲区
void CLogFile::WriteLog (char *sLog, int priority)
{
	if (m_priority >= priority)
	{
		int nLen;
		time_t now;
		tm tmNow;
		DWORD dwWrite;

		time (&now);
		localtime_s (&tmNow, &now);
		if (sLog[strlen (sLog)-1] != '\n')
		{
			nLen = sprintf ((char *) m_logcache,
							 "%.2d-%.2d %.2d:%.2d:%.2d %.5000s\r\n",
							 tmNow.tm_mon + 1, tmNow.tm_mday,
							 tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec, sLog);
		}
		else
		{
			nLen = sprintf ((char *) m_logcache,
							 "%.2d-%.2d %.2d:%.2d:%.2d %.5000s",
							 tmNow.tm_mon + 1, tmNow.tm_mday,
							 tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec, sLog);
		}

		//sprintf (m_fileName, "%.256s\\%s%.2d-%.2d-%.2d.log",
		//		 m_path, m_name, tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday);

		sprintf_s (m_fileName, "%.256s\\%s%.2d-%.2d-%.2d.log",
			 m_cpath, m_name, tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday);

		dwWrite = 0;

		if ((m_tmWrite.tm_yday != tmNow.tm_yday) ||
				(m_tmWrite.tm_year != tmNow.tm_year) ||
				(m_hFile == 0))
		{
			if (m_hFile)
				CloseHandle (m_hFile);
			m_hFile = CreateFileA (/*(LPCWSTR)*/m_fileName,
								  GENERIC_WRITE,
								  FILE_SHARE_READ|FILE_SHARE_DELETE,
								  NULL,
								  OPEN_ALWAYS,
								  FILE_ATTRIBUTE_NORMAL,
								  NULL);
			int i = GetLastError();
			Sleep(0);
		}

		//m_overlapped.OffsetHigh = 0;
		//m_overlapped.Offset = SetFilePointer (m_hFile, 0, NULL, FILE_END);
		//WriteFile (m_hFile, (LPVOID) m_logcache, nLen, &dwWrite, &m_overlapped);
		::SetFilePointer (m_hFile, 0, NULL, FILE_END);
		::WriteFile (m_hFile, (LPVOID) m_logcache, nLen, &dwWrite, NULL);
		m_tmWrite = tmNow;
	}
}
void CLogFile::Create (char *name)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	m_priority = PRIORITY_NORMAL;
	strcpy (m_name, name);
	m_hFile = 0;
	GetModuleFileName (NULL, m_path, 256);

	size_t i;
	mbstate_t    mbstate;
	::memset((void*)&mbstate, 0, sizeof(mbstate));

	const wchar_t* wt = m_path;
	wcsrtombs_s(&i, m_cpath, MAX_PATH, &wt, MAX_PATH,&mbstate);

	_splitpath (m_cpath, drive, dir, fname, ext);

	sprintf(m_cpath, "%s%sLog", drive, dir);
 
	memset(m_path,0,sizeof(m_path));  
	MultiByteToWideChar(CP_ACP,0,m_cpath,strlen(m_cpath)+1,m_path,  
		sizeof(m_path)/sizeof(m_path[0]));

	CreateDirectory (m_path, NULL);
}

// 设置日志等级
void CLogFile::SetPriority (int priority)
{
	m_priority = priority;
}

void CLogFile::Write (const char *format, ...)
{
	EnterCriticalSection (&m_cs);
	va_list arglist;
	va_start (arglist, format);
	vsnprintf_s (m_logbuf, 5120, 5000, format, arglist);
	va_end (arglist);
	if (GetTraceMode () == TraceToAll)
	{
		OutputDebugStringA (m_logbuf);
		printf ("%s", m_logbuf);
	}
	if (m_LogFilter)
	{
		if (strncmp(format, "[Error]", 7) == 0)
		{
			m_LogFilter (m_logbuf);
		}
	}
	WriteLog (m_logbuf);
	LeaveCriticalSection (&m_cs);
}

void CLogFile::Write (int procno, const char *procName, const char *format, ...)
{
	EnterCriticalSection (&m_cs);

	int len = (int)strlen(format) + (int)strlen(procName) + 1000;
	char *tformat = new char[len];
	const char *ptformat;
	if (ptformat = strcasestr(format, "[Error]"))
		sprintf_s(tformat, len, "[Error][%s]%s", procName, ptformat + strlen("[Error]"));
	else if (ptformat = strcasestr(format, "[Warning]"))
		sprintf_s(tformat, len, "[Warning][%s]%s", procName, ptformat + strlen("[Warning]"));
	else
		sprintf_s(tformat, len, "[Info][%s]%s", procName, format);
		
	va_list arglist;
	va_start (arglist, format);
	vsnprintf_s (m_logbuf, 5120, 5000, tformat, arglist);
	va_end (arglist);
	if (GetTraceMode () == TraceToAll)
	{
		OutputDebugStringA (m_logbuf);
		printf ("%s", m_logbuf);
	}
	if (m_LogFilter)
	{
		if (strncmp(format, "[Error]", 7) == 0)
		{
			m_LogFilter (m_logbuf);
		}
	}
	WriteLog (m_logbuf);
	delete [len] tformat;
	LeaveCriticalSection (&m_cs);
}


void CLogFile::SetLogFilter (PFuncLogFilter pFunc)
{
	m_LogFilter = pFunc;
}

const char* CLogFile::strcasestr(const char* str, const char* subStr)
{
	int len = (int)strlen(subStr);
	if(len == 0)
	{
		return NULL;          /*这里我并未照strstr一样返回str，而是返回NULL*/
	} 

	while(*str)
	{
		if(strnicmp(str, subStr, len) == 0)       /* 这里使用了可限定比较长度的strncasecmp */
		{
			return str;
		}
		str++;
	}
	return NULL;
}

void CLogFile::OutPutLock(char * string)
{
    char sz[100] = {0};
    sprintf(sz,string);
    OutputDebugStringA(sz);
}
