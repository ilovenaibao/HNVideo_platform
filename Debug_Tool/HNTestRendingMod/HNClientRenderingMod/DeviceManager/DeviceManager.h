#ifndef _HN_DEVICE_MANAGER_H_
#define _HN_DEVICE_MANAGER_H_

#include "DeviceInfo.h"
#include "sal.h"

#include <vector>
using namespace std;


typedef struct _MsgHead 
{
	UINT32 length;
	UINT32 seq;
	UINT32 reserved;
}MsgHead;


class CDeviceManager {

public:
	CDeviceManager();
	~CDeviceManager();
	bool StartListening();
	bool ParseData(CString sInfo, CString sIP);
	bool Broadcast( const char* sRemoteIP="255.255.255.255", UINT16 nRemotePort=7210, 
		const char* sBuffer="BROADCAST\r\n" );

private:

public:
	UINT32				m_nFreeTime;

private:
	CWinThread			*m_pThread;
	bool				m_bInitaled;
	CritSec				m_CritSec;

	vector<CDeviceInfo> m_cDeviceInfo;

};







#endif