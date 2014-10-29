#pragma once

#define DEVICE_INFO_NUM 13

class CDeviceInfo
{
public:
	CDeviceInfo(void);
	bool ParseData(CString sInfo, CString sRemoteIP);
	friend bool operator ==(const CDeviceInfo di1, const CDeviceInfo di2)
	{
		//TRACE(di1.m_sName);
		return (di1.m_sName==di2.m_sName &&
			di1.m_sSN==di2.m_sSN &&
			di1.m_sModel==di2.m_sModel &&
			di1.m_sSoftVer==di2.m_sSoftVer &&
			di1.m_sHddVer==di2.m_sHddVer &&
			di1.m_sDspVer==di2.m_sDspVer &&
			di1.m_sMAC==di2.m_sMAC &&
			di1.m_sIP==di2.m_sIP &&
			di1.m_sMask==di2.m_sMask &&
			di1.m_sGateway==di2.m_sGateway &&
			di1.m_sDNS==di2.m_sDNS &&
			di1.m_nVideo==di2.m_nVideo &&
			di1.m_sRemoteIP==di2.m_sRemoteIP) ? true : false;
	}
public:
	~CDeviceInfo(void);
	CString m_sHead[DEVICE_INFO_NUM];
	CString m_sName;
	CString m_sSN;
	COleDateTime m_tTime;
	CString m_sModel;
	CString m_sSoftVer;
	CString m_sHddVer;
	CString m_sDspVer;
	CString m_sMAC;
	CString m_sIP;
	CString m_sMask;
	CString m_sGateway;
	CString m_sDNS;
	UINT32 m_nVideo;
	CString m_sRemoteIP;
};
