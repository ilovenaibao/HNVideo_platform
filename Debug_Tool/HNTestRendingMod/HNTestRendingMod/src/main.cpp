#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include "../../src/CDataLink.h"
#include "../../src/CCtrlLink.h"
#include "../../src/NetGobalDef.h"


// �������Ӳɼ���
void TestLinkCollectPoint()
{
	wchar_t ip[20] = { L"192.168.1.147" };
	CConnectServerStruct connectStruct;
	CNetTrans netTransMod;
	int result = 0, ncode = 0;
	int step = 0; // ��¼ִ������һ��

	//netTransMod.Init(); // ��ʼ�����紫��ģ��
	ZeroMemory(&connectStruct, sizeof(connectStruct));
	size_t num;
	wcstombs_s(&num, connectStruct.ip, 16, ip, _TRUNCATE);
	connectStruct.chnl = 0;
	connectStruct.netTransMod = &netTransMod;
	connectStruct.procMod = NULL; // ��Ƶ����ģ��
	connectStruct.mode = DATA_MODE_TCP;
	connectStruct.hWnd = NULL;
	connectStruct.bSilent = false;
	for (int i = 0; i < CHNL_NUM; i++)
	{	
		connectStruct.algoType[i] = 0x0000FFFF;
	}

	CConnectServerStruct *param = &connectStruct;
	// ���ӷ�����
	result = param->netTransMod->Connect(param->hWnd, param->ip, param->chnl, param->mode, param->procMod, ncode);
	if (result != IPVA_DTC_OK)
	{
		// ���ӷ����������г���
		return;
	}
	step = 1;

	//// ��ȡ����ͨѶЭ��汾��
	//result = param->netTransMod->GetServerProtocol(param->srvNetProtMainVer, param->srvNetProtSubVer, ncode);
	//if (result != IPVA_DTC_OK && result != IPVA_DTC_ERR_OP_FAILED)
	//{
	//	// �Ƿ���˲���ʧ�ܵ��쳣
	//	return;
	//}
	//step = 2;

	//// ��ȡ����������汾��
	//result = param->netTransMod->GetServerVersion(param->serverVer, ncode);
	//if (result != IPVA_DTC_OK && result != IPVA_DTC_ERR_OP_FAILED)
	//{
	//	// �Ƿ���˲���ʧ�ܵ��쳣
	//	return;
	//}
	//step = 3;

	netTransMod.Clear(); // ������紫��ģ����Դ

	printf("end\n");
	getch();
}


//int main(int argc, char *argv[])
//{
//
//	TestLinkCollectPoint();
//
//	return 0;
//}