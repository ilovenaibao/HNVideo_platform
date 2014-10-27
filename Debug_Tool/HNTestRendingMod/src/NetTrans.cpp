#include "NetTrans.h"
#include <stdio.h>
#include <conio.h>
#include "NetGobalDef.h"


CNetTrans::CNetTrans(void)
{
}


CNetTrans::~CNetTrans(void)
{
}


void CNetTrans::Init(void)
{
	WSADATA wsd;
	::WSAStartup(MAKEWORD(2, 2), &wsd);
}


void CNetTrans::Clear(void)
{
	::WSACleanup();
}


int CNetTrans::Connect(HWND hWnd, LPCSTR lpAddr, unsigned char nChnl, unsigned char nMode, void* procMod, int& nCode)
{
	int result = IPVA_DTC_OK;
	do 
	{
		/*result = dataLink.LinkAgentServer("192.168.1.16", nChnl, nMode, procMod, nCode);
		if (result != IPVA_DTC_OK)
		{
			dataLink.UnLink();
			MessageBox(hWnd, L"error : LinkAgentServer()", NULL, NULL);
			break;
		}*/
		dataLink.hDisplay = hWnd;
		result = dataLink.Link(lpAddr, nChnl, nMode, procMod, nCode);
		if (result != IPVA_DTC_OK)
		{
			MessageBox(hWnd, L"error : link server error!", NULL, NULL);
			dataLink.UnLink();
			break;
		}
		getch();
		/*result = ctrlLink.Link(hWnd, lpAddr, nChnl, nCode);
		if (result != IPVA_DTC_OK)
		{
			dataLink.UnLink();
			ctrlLink.UnLink();
		}*/
	} while (false);
	return result;
}


int CNetTrans::ConnectDataLink(LPCSTR lpAddr, unsigned char nChnl, unsigned char nMode, void* procMod, int& nCode)
{
	int result = dataLink.Link(lpAddr, nChnl, nMode, procMod, nCode);
	if (result != IPVA_DTC_OK)
	{
		dataLink.UnLink();
	}
	return result;
}


int CNetTrans::ConnectCtrlLink(HWND hWnd, LPCSTR lpAddr, unsigned char nChnl, int& nCode)
{
	int result = ctrlLink.Link(hWnd, lpAddr, nChnl, nCode);
	if (result != IPVA_DTC_OK)
	{
		ctrlLink.UnLink();
	}
	return result;
}


void CNetTrans::DisConnectCtrlLink(void)
{
	ctrlLink.UnLink();
}


void CNetTrans::DisConnectDataLink(void)
{
	dataLink.UnLink();
}


void CNetTrans::DisConnect(void)
{
	dataLink.UnLink();
	ctrlLink.UnLink();
}


int CNetTrans::UpdatePrivileges(int& nCode)
{
	return ctrlLink.UpdatePrivileges(nCode);
}


int CNetTrans::CancelPrivileges(int& nCode)
{
	return ctrlLink.CancelPrivileges(nCode);
}


int CNetTrans::GetServerProtocol(unsigned short& nMainVer, unsigned short& nSubVer, int& nCode)
{
	return ctrlLink.GetServerProtocol(nMainVer, nSubVer, nCode);
}


int CNetTrans::GetServerVersion(char* sVersion, int& nCode)
{
	return ctrlLink.GetServerVersion(sVersion, nCode);
}


void CNetTrans::SetNetErrorNotify(HWND ctrlLink, HWND dataLink)
{
	//this->dataLink.SetNetErrorNotify(dataLink);
}


int CNetTrans::ExecCmd(LPBYTE lpReq, unsigned int nReqLen, LPBYTE& lpResp, unsigned int& nRespLen, int& nCode)
{
	int result = ctrlLink.AollocAlgo(lpReq, nReqLen, nRespLen, nCode);
	lpResp = (result == IPVA_DTC_OK) ? ctrlLink.GetRespData() : NULL;
	return result;
}