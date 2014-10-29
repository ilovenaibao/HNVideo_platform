#include "stdafx.h"
#include "DeviceInfo.h"
#include <cstring>

CDeviceInfo::CDeviceInfo(void)
{
	m_sHead[0]=_T("\nINFO");
	m_sHead[1]=_T("\nSN");
	m_sHead[2]=_T("\nTIME");
	m_sHead[3]=_T("\nModel");
	m_sHead[4]=_T("\nSoft");
	m_sHead[5]=_T("\nHdd");
	m_sHead[6]=_T("\nDsp");
	m_sHead[7]=_T("\nMAC");
	m_sHead[8]=_T("\nIP");
	m_sHead[9]=_T("\nMask");
	m_sHead[10]=_T("\nGateway");
	m_sHead[11]=_T("\nDNS");
	m_sHead[12]=_T("\nVideo");

	m_sName="";
	m_sSN="";
	m_tTime.ParseDateTime(L"2000-01-01");
	m_sModel="";
	m_sSoftVer="";
	m_sHddVer="";
	m_sDspVer="";
	m_sMAC="";
	m_sIP="";
	m_sMask="";
	m_sGateway="";
	m_sDNS="";
	m_nVideo=0;
	m_sRemoteIP="";
}

CDeviceInfo::~CDeviceInfo(void)
{
}

bool CDeviceInfo::ParseData(CString sInfo, CString sRemoteIP)
{
	if (sInfo.GetLength()<=30)
		return false;

	bool bRt=false;
	sInfo= "\n";
	sInfo += sInfo;
	m_sRemoteIP=sRemoteIP;
	CString sLine;
	CString sData;
	int nBegin(0),nEnd(0);
	for (int i=0; i<DEVICE_INFO_NUM; i++)
	{
		nBegin=sInfo.Find(m_sHead[i]);
		if (nBegin>=0)
		{
			nEnd=sInfo.Find('\n', nBegin+1);
			if (nEnd > 0 && nEnd > nBegin+1)
			{
				sLine = sInfo.Mid(nBegin+1, nEnd-nBegin-2);
				bRt = true;
				switch (i)
				{
				case 0:
					sData=sLine.Mid(m_sHead[i].GetLength());
					sData.TrimLeft();
					sData.TrimLeft('=');
					m_sName=sData;
					break;
				case 1:
					sData=sLine.Mid(m_sHead[i].GetLength());
					sData.TrimLeft();
					sData.TrimLeft('=');
					m_sSN=sData;
					break;
				case 2:
					sData=sLine.Mid(m_sHead[i].GetLength());
					sData.TrimLeft();
					sData.TrimLeft('=');
					if (!m_tTime.ParseDateTime(sData))
					{
						m_tTime.SetDate(2000,1,1);
					}
					break;
				case 3:
					sData=sLine.Mid(m_sHead[i].GetLength());
					sData.TrimLeft();
					sData.TrimLeft('=');
					m_sModel=sData;
					break;
				case 4:
					sData=sLine.Mid(m_sHead[i].GetLength());
					sData.TrimLeft();
					sData.TrimLeft('=');
					m_sSoftVer=sData;
					break;
				case 5:
					sData=sLine.Mid(m_sHead[i].GetLength());
					sData.TrimLeft();
					sData.TrimLeft('=');
					m_sHddVer=sData;
					break;
				case 6:
					sData=sLine.Mid(m_sHead[i].GetLength());
					sData.TrimLeft();
					sData.TrimLeft('=');
					m_sDspVer=sData;
					break;
				case 7:
					sData=sLine.Mid(m_sHead[i].GetLength());
					sData.TrimLeft();
					sData.TrimLeft('=');
					m_sMAC=sData;
					break;
				case 8:
					sData=sLine.Mid(m_sHead[i].GetLength());
					sData.TrimLeft();
					sData.TrimLeft('=');
					m_sIP=sData;
					break;
				case 9:
					sData=sLine.Mid(m_sHead[i].GetLength());
					sData.TrimLeft();
					sData.TrimLeft('=');
					m_sMask=sData;
					break;
				case 10:
					sData=sLine.Mid(m_sHead[i].GetLength());
					sData.TrimLeft();
					sData.TrimLeft('=');
					m_sGateway=sData;
					break;
				case 11:
					sData=sLine.Mid(m_sHead[i].GetLength());
					sData.TrimLeft();
					sData.TrimLeft('=');
					m_sDNS=sData;
					break;
				case 12:
					sData=sLine.Mid(m_sHead[i].GetLength());
					sData.TrimLeft();
					sData.TrimLeft('=');
					m_nVideo=_wtoi(sData.AllocSysString());
					break;
				}
			}
			else
			{
				TRACE("%s end not found\n", m_sHead[i]);
			}
		}
		else
		{
			TRACE("%s begin not found\n", m_sHead[i]);
		}
	}
	return bRt;
}

