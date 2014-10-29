#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include "../../src/CDataLink.h"
#include "../../src/CCtrlLink.h"
#include "../../src/NetGobalDef.h"


// 测试连接采集点
void TestLinkCollectPoint()
{
	wchar_t ip[20] = { L"192.168.1.147" };
	CConnectServerStruct connectStruct;
	CNetTrans netTransMod;
	int result = 0, ncode = 0;
	int step = 0; // 记录执行至哪一步

	//netTransMod.Init(); // 初始化网络传输模块
	ZeroMemory(&connectStruct, sizeof(connectStruct));
	size_t num;
	wcstombs_s(&num, connectStruct.ip, 16, ip, _TRUNCATE);
	connectStruct.chnl = 0;
	connectStruct.netTransMod = &netTransMod;
	connectStruct.procMod = NULL; // 视频绘制模块
	connectStruct.mode = DATA_MODE_TCP;
	connectStruct.hWnd = NULL;
	connectStruct.bSilent = false;
	for (int i = 0; i < CHNL_NUM; i++)
	{	
		connectStruct.algoType[i] = 0x0000FFFF;
	}

	CConnectServerStruct *param = &connectStruct;
	// 连接服务器
	result = param->netTransMod->Connect(param->hWnd, param->ip, param->chnl, param->mode, param->procMod, ncode);
	if (result != IPVA_DTC_OK)
	{
		// 连接服务器过程中出错
		return;
	}
	step = 1;

	//// 获取网络通讯协议版本号
	//result = param->netTransMod->GetServerProtocol(param->srvNetProtMainVer, param->srvNetProtSubVer, ncode);
	//if (result != IPVA_DTC_OK && result != IPVA_DTC_ERR_OP_FAILED)
	//{
	//	// 非服务端操作失败的异常
	//	return;
	//}
	//step = 2;

	//// 获取服务器程序版本号
	//result = param->netTransMod->GetServerVersion(param->serverVer, ncode);
	//if (result != IPVA_DTC_OK && result != IPVA_DTC_ERR_OP_FAILED)
	//{
	//	// 非服务端操作失败的异常
	//	return;
	//}
	//step = 3;

	netTransMod.Clear(); // 清除网络传输模块资源

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