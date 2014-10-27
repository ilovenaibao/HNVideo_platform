#include "CCtrlLink.h"
#include "hv_versions.h"
#include <random>
#include <ctime>
#include <process.h>

CCtrlLink::CCtrlLink(void)
	: s(INVALID_SOCKET)
	, extRecvBuffer(NULL)
	, extRecvBufferSize(0)
	, packNo(0)
	, hHBThread(NULL)
	, bRegister(false)
	, hHBProcWnd(NULL)
{
	::InitializeCriticalSection(&hExecCmdCS);
	hExitHBThreadEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
}

CCtrlLink::~CCtrlLink(void)
{
	::DeleteCriticalSection(&hExecCmdCS);
	::CloseHandle(hExitHBThreadEvent);
}

int CCtrlLink::Link(HWND hWnd, const char* lpAddr, unsigned char nChnl, int& nCode)
{
	int result = IPVA_DTC_OK;
	nCode = 0;
	do 
	{
		int res;
		if (extRecvBuffer == NULL)
		{
			extRecvBuffer = new char[1000];
			extRecvBufferSize = 1000;
		}
		if (s == INVALID_SOCKET)
		{
			// 初始化控制链路套接字
			res = CNetOption::InitSocket(s, CTRLSNDTIMEOUT, CTRLRCVTIMEOUT);
			if (0 != res)
			{
				result = IPVA_DTC_ERR_NET;
				nCode = res;
				break;
			}
		}
		// 连接服务器
		res = CNetOption::Connect(s, lpAddr, CONTROLPORT);
		if (res == -1)
		{
			result = IPVA_DTC_UNDEFINE;
			break;
		}
		else if (res != 0)
		{
			result = IPVA_DTC_ERR_NET;
			nCode = res;
			break;
		}

		// 创建随机包序号
		std::uniform_int_distribution<int> u(1, 999999);
		std::default_random_engine e(static_cast<unsigned long>(std::time(NULL)));
		packNo = u(e);

		// 登录服务器
		res = Login(nCode);
		if (res != IPVA_DTC_OK)
		{
			result = res;
			break;
		}
		// 注册目标通道
		res = Register(nChnl, nCode);
		if (res != IPVA_DTC_OK)
		{
			result = res;
			break;
		}
		hHBProcWnd = hWnd;
		//::ResetEvent(hExitHBThreadEvent);
		//// 启动心跳包发送线程
		//hHBThread = (HANDLE)_beginthreadex(NULL, 0, CCtrlLink::HBThreadFunc, this, 0 , NULL);
		//if (hHBThread == NULL)
		//{
		//	result = IPVA_DTC_ERR_BT_FAILED;
		//	break;
		//}
		result = res;
	} while (false);
	return result;
}

void CCtrlLink::UnLink(void)
{
	//	int nCode;
	//	UnRegister(nCode);
	CNetOption::CloseSocket(s);
	bRegister = false;
	if (hHBThread != NULL)
	{
		::SetEvent(hExitHBThreadEvent);
		if (WAIT_TIMEOUT == ::WaitForSingleObject(hHBThread, 2000))
		{
			::TerminateThread(hHBThread, -1);
		}
		::CloseHandle(hHBThread);
		hHBThread = NULL;
	}
	hHBProcWnd = NULL;
	if (extRecvBuffer)
	{
		delete[] extRecvBuffer;
		extRecvBuffer = NULL;
	}
	extRecvBufferSize = 0;
}

int CCtrlLink::LinkAgentServer(HWND hWnd, const char* lpAddr, unsigned char nChnl, int& nCode)
{
	int result = IPVA_DTC_OK;
	nCode = 0;
	do 
	{
		int res;
		if (extRecvBuffer == NULL)
		{
			extRecvBuffer = new char[1000];
			extRecvBufferSize = 1000;
		}
		if (s == INVALID_SOCKET)
		{
			// 初始化控制链路套接字
			res = CNetOption::InitSocket(sAgentServerCtrl, CTRLSNDTIMEOUT, CTRLRCVTIMEOUT);
			if (0 != res)
			{
				result = IPVA_DTC_ERR_NET;
				nCode = res;
				break;
			}
		}
		// 连接服务器
		res = CNetOption::Connect(sAgentServerCtrl, lpAddr, CONTROLPORT);
		if (res == -1)
		{
			result = IPVA_DTC_UNDEFINE;
			break;
		}
		else if (res != 0)
		{
			result = IPVA_DTC_ERR_NET;
			nCode = res;
			break;
		}

		//::ResetEvent(hExitHBThreadEvent);
		//// 启动心跳包发送线程
		//hHBThread = (HANDLE)_beginthreadex(NULL, 0, CCtrlLink::HBThreadFunc, this, 0 , NULL);
		//if (hHBThread == NULL)
		//{
		//	result = IPVA_DTC_ERR_BT_FAILED;
		//	break;
		//}
		result = res;
	} while (false);
	return result;
}

int CCtrlLink::GetServerProtocol(unsigned short& nMainVer, unsigned short& nSubVer, int& nCode)
{
	reqpkt_get_server_protocol_t req;
	respkt_get_server_protocol_t resp;

	::EnterCriticalSection(&hExecCmdCS);
	set_reqpkt_get_server_protocol_t(req, packNo);

	int recvRemainLen;
	int result = ExecCmd(reinterpret_cast<char*>(&req), sizeof(req), reinterpret_cast<char*>(&resp), recvRemainLen, nCode);
	if (recvRemainLen != (sizeof(resp) - sizeof(respkt_t)))
	{
		result = IPVA_DTC_ERR_PACKAGE_LEN;
	}
	else if (result == IPVA_DTC_OK)
	{
		memcpy_s(reinterpret_cast<char*>(&resp) + sizeof(respkt_t), sizeof(resp) - sizeof(respkt_t), extRecvBuffer, recvRemainLen);
		convert_respkt_get_server_protocol_t(resp);
		if (resp.header.pkt_status != S_CTRL_OK)
		{
			result = IPVA_DTC_ERR_OP_FAILED;
			nCode = resp.header.pkt_status;
		}
		else
		{
			nMainVer = resp.nProtMainVer;
			nSubVer = resp.nProtSubVer;
		}
	}
	::LeaveCriticalSection(&hExecCmdCS);
	return result;
}

int CCtrlLink::GetServerVersion(char* sVersion, int& nCode)
{
	reqpkt_get_server_version_t req;
	respkt_get_server_version_t resp;

	::EnterCriticalSection(&hExecCmdCS);
	set_reqpkt_get_server_version_t(req, packNo);

	int recvRemainLen;
	int result = ExecCmd(reinterpret_cast<char*>(&req), sizeof(req), reinterpret_cast<char*>(&resp), recvRemainLen, nCode);
	if (recvRemainLen != (sizeof(resp) - sizeof(respkt_t)))
	{
		result = IPVA_DTC_ERR_PACKAGE_LEN;
	}
	else if (result == IPVA_DTC_OK)
	{
		memcpy_s(reinterpret_cast<char*>(&resp) + sizeof(respkt_t), sizeof(resp) - sizeof(respkt_t), extRecvBuffer, recvRemainLen);
		convert_respkt_get_server_version_t(resp);
		if (resp.header.pkt_status != S_CTRL_OK)
		{
			result = IPVA_DTC_ERR_OP_FAILED;
			nCode = resp.header.pkt_status;
		}
		else
		{
			memcpy_s(sVersion, 32, resp.vVersion, 32);
		}
	}
	::LeaveCriticalSection(&hExecCmdCS);
	return result;
}


int CCtrlLink::UpdatePrivileges(int& nCode)
{
	reqpkt_reg_ctrl_priv_t req;
	respkt_reg_ctrl_priv_t resp;

	int result = IPVA_DTC_OK;
	::EnterCriticalSection(&hExecCmdCS);
	set_reqpkt_reg_ctrl_priv_t(req, packNo);

	int recvRemainLen;
	result = ExecCmd(reinterpret_cast<char*>(&req), sizeof(req), reinterpret_cast<char*>(&resp), recvRemainLen, nCode);
	if (recvRemainLen != sizeof(resp) - sizeof(respkt_t))
	{
		result = IPVA_DTC_ERR_PACKAGE_LEN;
	}
	else if (result == IPVA_DTC_OK)
	{
		memcpy_s(reinterpret_cast<char*>(&resp) + sizeof(respkt_t),
			sizeof(resp) - sizeof(respkt_t), extRecvBuffer, recvRemainLen);
		convert_respkt_reg_ctrl_priv_t(resp);
		if (resp.header.pkt_status != S_CTRL_OK)
		{
			result = IPVA_DTC_ERR_OP_FAILED;
			nCode = resp.header.pkt_status;
		}
	}
	::LeaveCriticalSection(&hExecCmdCS);
	return result;
}


int CCtrlLink::CancelPrivileges(int& nCode)
{
	reqpkt_unreg_ctrl_priv_t req;
	respkt_unreg_ctrl_priv_t resp;

	int result = IPVA_DTC_OK;
	::EnterCriticalSection(&hExecCmdCS);
	set_reqpkt_unreg_ctrl_priv_t(req, packNo);

	int recvRemainLen;
	result = ExecCmd(reinterpret_cast<char*>(&req), sizeof(req), reinterpret_cast<char*>(&resp), recvRemainLen, nCode);
	if (recvRemainLen != sizeof(resp) - sizeof(respkt_t))
	{
		result = IPVA_DTC_ERR_PACKAGE_LEN;
	}
	else if (result == IPVA_DTC_OK)
	{
		convert_respkt_t(resp);
		if (resp.pkt_status != S_CTRL_OK)
		{
			result = IPVA_DTC_ERR_OP_FAILED;
			nCode = resp.pkt_status;
		}
	}
	::LeaveCriticalSection(&hExecCmdCS);
	return result;
}


int CCtrlLink::AollocAlgo(LPBYTE lpData, unsigned int nLen, unsigned int& nRespDataLen, int& nCode)
{
	reqpkt_set_algo_params_t req;
	respkt_set_algo_params_t resp;

	::EnterCriticalSection(&hExecCmdCS);
	set_reqpkt_set_algo_params_t(req, packNo, nLen);

	// 处理校验和

	int result = IPVA_DTC_OK;
	nRespDataLen = 0;
	nCode = 0;
	do 
	{
		result = CNetOption::Send(s, reinterpret_cast<char*>(&req), sizeof(req));
		if (result != 0)
		{
			nCode = result;
			result = IPVA_DTC_ERR_NET;
			break;
		}
		result = CNetOption::Send(s, reinterpret_cast<char*>(lpData), nLen);
		if (result != 0)
		{
			nCode = result;
			result = IPVA_DTC_ERR_NET;
			break;
		}

		do
		{
			// 接收标准回复包数据
			result = CNetOption::Recv(s, reinterpret_cast<char*>(&resp), sizeof(respkt_t));
			if (-1 == result)
			{
				// 连接被断开
				result = IPVA_DTC_ERR_CONN_CLOSE;
				break;
			}
			else if (0 != result)
			{
				nCode = result;
				result = IPVA_DTC_ERR_NET;
				break;
			}
			convert_respkt_t(resp);
			if (resp.pkt_status != IPVA_DTC_OK)
			{
				result = IPVA_DTC_ERR_OP_FAILED;
				nCode = resp.pkt_status;
				break;
			}
			unsigned long pktRemainLen = resp.pkt_pldlen;
			if (pktRemainLen > 2000)
			{
				// 未定义错误，超出预备的缓冲区大小
				result = IPVA_DTC_UNDEFINE;
				break;
			}
			if (pktRemainLen < (sizeof(resp) - sizeof(respkt_t)))
			{
				result = IPVA_DTC_ERR_PACKAGE_LEN;
				break;
			}
			// 接收算法配置命令固定头的余下部分
			result = CNetOption::Recv(s, reinterpret_cast<char*>(&resp) + sizeof(respkt_t), sizeof(resp) - sizeof(respkt_t));
			if (-1 == result)
			{
				// 连接被断开
				result = IPVA_DTC_ERR_CONN_CLOSE;
				break;
			}
			else if (0 != result)
			{
				nCode = result;
				result = IPVA_DTC_ERR_NET;
				break;
			}
			pktRemainLen -= (sizeof(resp) - sizeof(respkt_t));
			if (extRecvBufferSize < pktRemainLen)
			{
				delete[] extRecvBuffer;
				extRecvBuffer = new char[pktRemainLen];
				extRecvBufferSize = pktRemainLen;
			}
			// 接收指定回复长度除标准回复包外余下的数据
			result = CNetOption::Recv(s, extRecvBuffer, pktRemainLen);
			if (-1 == result)
			{
				// 连接被断开
				result = IPVA_DTC_ERR_CONN_CLOSE;
				break;
			}
			else if (0 != result)
			{
				nCode = result;
				result = IPVA_DTC_ERR_NET;
				break;
			}
			// 检查校验和

			nRespDataLen = pktRemainLen;
		}while(resp.pkt_num != packNo);		// 检查包序号是否与发送的一致
	} while (false);
	::InterlockedIncrement(&packNo);			// 包序号加一
	::LeaveCriticalSection(&hExecCmdCS);
	return result;
}

LPBYTE CCtrlLink::GetRespData(void)
{
	return reinterpret_cast<LPBYTE>(extRecvBuffer);
}

int CCtrlLink::Login(int& nCode)
{
	reqpkt_login_t req;
	respkt_login_t resp;

	::EnterCriticalSection(&hExecCmdCS);
	set_reqpkt_login_t(req, packNo, HV_NET_PROTO_MAIN, HV_NET_PROTO_SUB, "", "");

	int recvRemainLen;
	int result = ExecCmd(reinterpret_cast<char*>(&req), sizeof(req), reinterpret_cast<char*>(&resp), recvRemainLen, nCode);
	if (recvRemainLen != (sizeof(resp) - sizeof(respkt_t)))
	{
		result = IPVA_DTC_ERR_PACKAGE_LEN;
	}
	else if (result == IPVA_DTC_OK)
	{
		convert_respkt_t(resp);
		if (resp.pkt_status != S_CTRL_OK)
		{
			result = IPVA_DTC_ERR_OP_FAILED;
			nCode = resp.pkt_status;
		}
	}
	::LeaveCriticalSection(&hExecCmdCS);
	return result;
}


int CCtrlLink::Register(unsigned char nChnl, int& nCode)
{
	if (bRegister)
	{
		// 通道已注册，返回成功操作
		nCode = 0;
		return IPVA_DTC_OK;
	}

	reqpkt_register_t req;
	respkt_register_t resp;

	::EnterCriticalSection(&hExecCmdCS);
	set_reqpkt_register_t(req, packNo, nChnl);

	int recvRemainLen;
	int result = ExecCmd(reinterpret_cast<char*>(&req), sizeof(req), reinterpret_cast<char*>(&resp), recvRemainLen, nCode);
	if (recvRemainLen != sizeof(resp) - sizeof(respkt_t))
	{
		result = IPVA_DTC_ERR_PACKAGE_LEN;
	}
	else if (result == IPVA_DTC_OK)
	{
		convert_respkt_t(resp);
		if (resp.pkt_status != S_CTRL_OK)
		{
			result = IPVA_DTC_ERR_OP_FAILED;
			nCode = resp.pkt_status;
		}
		else
		{
			bRegister = true;
		}
	}
	::LeaveCriticalSection(&hExecCmdCS);
	return result;
}


int CCtrlLink::UnRegister(int& nCode)
{
	if (!bRegister)
	{
		// 通道未注册，立即返回
		nCode = 0;
		return IPVA_DTC_OK;
	}

	reqpkt_unregister_t req;
	respkt_unregister_t resp;

	::EnterCriticalSection(&hExecCmdCS);
	set_reqpkt_unregister_t(req, packNo);

	int recvRemainLen;
	int result = ExecCmd(reinterpret_cast<char*>(&req), sizeof(req), reinterpret_cast<char*>(&resp), recvRemainLen, nCode);
	if (recvRemainLen != sizeof(resp) - sizeof(respkt_t))
	{
		result = IPVA_DTC_ERR_PACKAGE_LEN;
	}
	else if (result == IPVA_DTC_OK)
	{
		convert_respkt_t(resp);
		if (resp.pkt_status != S_CTRL_OK)
		{
			result = IPVA_DTC_ERR_OP_FAILED;
			nCode = resp.pkt_status;
		}
	}
	::LeaveCriticalSection(&hExecCmdCS);
	bRegister = false;
	return result;
}

int CCtrlLink::ExecCmd(const char* sndBuffer, int sndBufferLen, char* recvHeader, int& recvRemainLen, int& nCode)
{
	int result = IPVA_DTC_OK, code = 0;
	do 
	{
		recvRemainLen = 0;
		ZeroMemory(extRecvBuffer, extRecvBufferSize);
		// 发送消息定长内容
		int ret = CNetOption::Send(s, sndBuffer, sndBufferLen);
		if (ret != 0)
		{
			result = IPVA_DTC_ERR_NET;
			code = ret;
			break;
		}

		do
		{
			// 接收标准回复包数据
			ret = CNetOption::Recv(s, recvHeader, sizeof(respkt_t));
			if (-1 == ret)
			{
				// 连接被断开
				result = IPVA_DTC_ERR_CONN_CLOSE;
				code = 0;
				break;
			}
			else if (0 != ret)
			{
				result = IPVA_DTC_ERR_NET;
				code = ret;
				break;
			}
			unsigned long pktRemainLen = ntohl(reinterpret_cast<respkt_t*>(recvHeader)->pkt_pldlen);
			if (pktRemainLen > 2000)
			{
				// 未定义错误，超出预备的缓冲区大小
				result = IPVA_DTC_UNDEFINE;
				code = 0;
				break;
			}
			if (pktRemainLen > 0)
			{
				if (extRecvBufferSize < pktRemainLen)
				{
					delete[] extRecvBuffer;
					extRecvBuffer = new char[pktRemainLen];
					extRecvBufferSize = pktRemainLen;
				}
				// 接收指定回复长度除标准回复包外余下的数据
				ret = CNetOption::Recv(s, extRecvBuffer, pktRemainLen);
				if (-1 == ret)
				{
					// 连接被断开
					result = IPVA_DTC_ERR_CONN_CLOSE;
					break;
				}
				else if (0 != ret)
				{
					result = IPVA_DTC_ERR_NET;
					code = ret;
					break;
				}
				recvRemainLen = pktRemainLen;
			}
			// 检查校验和
		}while(ntohl(reinterpret_cast<respkt_t*>(recvHeader)->pkt_num) != packNo);		// 检查包序号是否与发送的一致
	} while (false);
	::InterlockedIncrement(&packNo);			// 包序号加一
	nCode = code;
	return result;
}

void CCtrlLink::set_reqpkt_get_server_protocol_t(reqpkt_get_server_protocol_t& req, unsigned int packNo)
{
	ZeroMemory(&req, sizeof(req));
	req.pkt_num = htonl(packNo);
	req.pkt_type = htons(IPVA_DT_CTRL_GETPROTVER);
}

void CCtrlLink::convert_respkt_get_server_protocol_t(respkt_get_server_protocol_t& resp)
{
	convert_respkt_t(resp.header);
	resp.nProtMainVer = ntohs(resp.nProtMainVer);
	resp.nProtSubVer = ntohs(resp.nProtSubVer);
}

void CCtrlLink::convert_respkt_t(respkt_t& resp)
{
	resp.pkt_num = ntohl(resp.pkt_num);
	resp.pkt_chksum = ntohs(resp.pkt_chksum);
	resp.pkt_status = ntohs(resp.pkt_status);
	resp.pkt_pldlen = ntohl(resp.pkt_pldlen);
}

void CCtrlLink::set_reqpkt_get_server_version_t(reqpkt_get_server_version_t& req, unsigned int packNo)
{
	ZeroMemory(&req, sizeof(req));
	req.pkt_num = htonl(packNo);
	req.pkt_type = htons(IPVA_DT_CTRL_GETSRVVER);
}

void CCtrlLink::convert_respkt_get_server_version_t(respkt_get_server_version_t& resp)
{
	convert_respkt_t(resp.header);
}

void CCtrlLink::set_reqpkt_reg_ctrl_priv_t(reqpkt_reg_ctrl_priv_t& req, unsigned int packNo)
{
	ZeroMemory(&req, sizeof(req));
	req.pkt_num = htonl(packNo);
	req.pkt_type = htons(IPVA_DT_CTRL_REGCTRLPRIV);
	req.pkt_pldlen = htonl(sizeof(req) - sizeof(reqpkt_t));
}

void CCtrlLink::convert_respkt_reg_ctrl_priv_t(respkt_reg_ctrl_priv_t& resp)
{
	convert_respkt_t(resp.header);
}

void CCtrlLink::set_reqpkt_unreg_ctrl_priv_t(reqpkt_unreg_ctrl_priv_t& req, unsigned int packNo)
{
	ZeroMemory(&req, sizeof(req));
	req.pkt_num = htonl(packNo);
	req.pkt_type = htons(IPVA_DT_CTRL_UNREGCTRLPRIV);
}

void CCtrlLink::set_reqpkt_set_algo_params_t(reqpkt_set_algo_params_t& req, unsigned int packNo, unsigned int pldLen)
{
	ZeroMemory(&req, sizeof(req));
	req.header.pkt_num = htonl(packNo);
	req.header.pkt_type = htons(IPVA_DT_CTRL_ALLOCALGO);   //计数器消息也是发送这个，在板子会根据参数自己分析
	req.header.pkt_pldlen = htonl(pldLen + sizeof(req) - sizeof(reqpkt_t));
}

void CCtrlLink::set_reqpkt_login_t(reqpkt_login_t& req, unsigned int packNo, unsigned short mainVer, unsigned short subVer, const char* user, const char* psw)
{
	ZeroMemory(&req, sizeof(req));
	req.header.pkt_num = htonl(packNo);
	req.header.pkt_type = htons(IPVA_DT_CTRL_LOGIN);
	req.header.pkt_pldlen = htonl(sizeof(req) - sizeof(reqpkt_t));
	req.nCProtMainVer = htons(mainVer);
	req.nCProtSubVer = htons(subVer);
	strcpy_s(req.vUser, 16, user);
	strcpy_s(req.vPsw, 64, psw);
}

void CCtrlLink::set_reqpkt_register_t(reqpkt_register_t& req, unsigned int packNo, unsigned char chnl)
{
	ZeroMemory(&req, sizeof(req));
	req.header.pkt_num = htonl(packNo);
	req.header.pkt_type = htons(IPVA_DT_CTRL_REG);
	req.header.pkt_pldlen = htonl(sizeof(req) - sizeof(reqpkt_t));
	req.nChnl = chnl;
}

void CCtrlLink::set_reqpkt_unregister_t(reqpkt_unregister_t& req, unsigned int packNo)
{
	ZeroMemory(&req, sizeof(req));
	req.pkt_num = htonl(packNo);
	req.pkt_type = htons(IPVA_DT_CTRL_UNREG);
}