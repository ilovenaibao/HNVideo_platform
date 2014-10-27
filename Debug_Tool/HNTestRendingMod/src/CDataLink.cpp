#include "CDataLink.h"
#include "hv_versions.h"
#include "NetGobalDef.h"
#include "./RenderingMod/RenderingMod.h"
#include "../../../video_platform/HNCommon/process_xml/ProcessXML.h"
#include <process.h>
#include <stdio.h>
#include <conio.h>


CDataLink::CDataLink(void)
	: sCtrl(INVALID_SOCKET)
	, sData(INVALID_SOCKET)
	, sAgentServerData(INVALID_SOCKET)
	, nMainVersion(0)
	, nSubVersion(0)
	, nPort(0)
	, nTokenID(0)
	, hExitHBThreadEvent(NULL)
	, hHBThread(NULL)
	, hAgentServerThread(NULL)
	, hDataRcvThread(NULL)
	, processModule(NULL)
	, bRegister(false)
	, hWndNotify(NULL)
	, bUnLink(false)
	, nCount(0)
	, bRecord(false)
	, nSendVideoFlag(0)
	, pMyBuffer(NULL)
	, iFrameSize(0)
	, iFrameFlag(0)
{
	::InitializeCriticalSection(&hExecCmdCS);
	hExitHBThreadEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	pMyBuffer = new char[1024*1024*20];
}

CDataLink::~CDataLink(void)
{
	::DeleteCriticalSection(&hExecCmdCS);
	::CloseHandle(hExitHBThreadEvent);

	delete pMyBuffer;
}

int CDataLink::Link(const char* lpAddr, unsigned char nChnl, unsigned char nMode, void* procMod, int& nCode)
{
	int result = IPVA_DTC_OK;
	nCode = 0;
	int res;

	if (sCtrl == INVALID_SOCKET)
	{
		// ��ʼ��������·�׽���
		res = CNetOption::InitSocket(sCtrl, CTRLSNDTIMEOUT, CTRLRCVTIMEOUT);
		if (0 != res)
		{
			result = IPVA_DTC_ERR_NET;
			nCode = res;
			return result;
		}
	}
	// ���ӷ�����
	res = CNetOption::Connect(sCtrl, lpAddr, DATACTRLPORT);
	if (res == -1)
	{
		result = IPVA_DTC_UNDEFINE;
		return result;
	}
	else if (res != 0)
	{
		result = IPVA_DTC_ERR_NET;
		nCode = res;
		return result;
	}
	// ��¼������
	res = Login(nCode);
	if (res != IPVA_DTC_OK)
	{
		result = res;
		return result;
	}
	// ע��Ŀ��ͨ��
	res = Register(nChnl, nMode, nCode);
	if (res != IPVA_DTC_OK)
	{
		result = res;
		return result;
	}
	bUnLink = false;

	processModule = procMod;
	if (sData == INVALID_SOCKET)
	{
		// ��ʼ��������·�׽���
		res = CNetOption::InitSocket(sData, DATASNDTIMEOUT, DATARCVTIMEOUT);
		if (0 != res)
		{
			result = IPVA_DTC_ERR_NET;
			nCode = res;
			return result;
		}
	}
	// �������ݷ�����
	res = CNetOption::Connect(sData, lpAddr, nPort);
	if (res == -1)
	{
		result = IPVA_DTC_UNDEFINE;
		return result;
	}
	else if (res != 0)
	{
		result = IPVA_DTC_ERR_NET;
		nCode = res;
		return result;
	}
	// ��������
	res = SendToken(nTokenID, nCode);
	if (res != IPVA_DTC_OK)
	{
		result = res;
		return result;
	}
	// �������ݽ����߳�
	if (hDataRcvThread == NULL)
	{
		hDataRcvThread = (HANDLE)_beginthreadex(NULL, 0, CDataLink::DataRcvAndSendThreadFunc, (CDataLink *)this, 0 , NULL);
	}
	if (hDataRcvThread == NULL)
	{
		result = IPVA_DTC_ERR_BT_FAILED;
		return result;
	}

	return result;
}

void CDataLink::UnLink(void)
{
	//	int nCode;
	//	UnRegister(nCode);						// ע��ͨ��
	bRegister = false;
	bUnLink = true;
	CNetOption::CloseSocket(sData);		// �ر�������·
	::SetEvent(hExitHBThreadEvent);
	HANDLE handles[] = { hDataRcvThread, hHBThread };		// ����
	if (WAIT_TIMEOUT == ::WaitForMultipleObjects(2, handles, TRUE, 2000))
	{
		for (int count = 0; count < 2; count++)
		{
			if (WAIT_TIMEOUT == ::WaitForSingleObject(handles[count], 0))
			{
				::TerminateThread(handles[count], -1);
			}
		}
	}
	if (hDataRcvThread != NULL)
	{
		::CloseHandle(hDataRcvThread);
		hDataRcvThread = NULL;
	}
	if (hHBThread != NULL)
	{
		::CloseHandle(hHBThread);
		hHBThread = NULL;
	}
	CNetOption::CloseSocket(sCtrl);		// �رտ�����·
	processModule = NULL;
}

int CDataLink::LinkAgentServer(const char* lpAddr, unsigned char nChnl, unsigned char nMode, void* procMod, int& nCode)
{
	int result = IPVA_DTC_OK;
	nCode = 0;
	int res;
	if (sCtrl == INVALID_SOCKET)
	{
		// ��ʼ��������·�׽���
		res = CNetOption::InitSocket(sAgentServerData, CTRLSNDTIMEOUT, CTRLRCVTIMEOUT);
		if (0 != res)
		{
			result = IPVA_DTC_ERR_NET;
			nCode = res;
			return result;
		}
	}
	// ���ӷ�����
	res = CNetOption::Connect(sAgentServerData, lpAddr, DATACTRLPORT_AGENT_SERVER);
	if (res == -1)
	{
		result = IPVA_DTC_UNDEFINE;
		return result;
	}
	else if (res != 0)
	{
		result = IPVA_DTC_ERR_NET;
		nCode = res;
		return result;
	}
	// �������ݷ����߳�
	if (hAgentServerThread == NULL)
	{
		hAgentServerThread = (HANDLE)_beginthreadex(NULL, 0, CDataLink::LinkAgentServerThreadFunc, this, 0 , NULL);
	}
	if (hAgentServerThread == NULL)
	{
		result = IPVA_DTC_ERR_BT_FAILED;
		return result;
	}

	bUnLink = false;
	processModule = procMod;
	
	return result;
}

void CDataLink::UnLinkAgentServer(void)
{
	//	int nCode;
	//	UnRegister(nCode);						// ע��ͨ��
	bRegister = false;
	bUnLink = true;
	CNetOption::CloseSocket(sAgentServerData);	// �ر�������·
	processModule = NULL;
}

int CDataLink::Login(int& nCode)
{
	reqpkt_data_login_t req;
	respkt_data_login_t resp;

	set_reqpkt_data_login_t(req, "root", "passme");

	int recvRemainLen;
	int result = ExecCtrlCmd(reinterpret_cast<char*>(&req), sizeof(req), reinterpret_cast<char*>(&resp), recvRemainLen, nCode);
	if (result == IPVA_DTC_OK)
	{
		memcpy_s(reinterpret_cast<char*>(&resp) + sizeof(respkt_data_t), 16, extRecvBuffer, recvRemainLen);
		convert_respkt_data_login_t(resp);
		if (resp.header.pkt_status == S_DATA_OK)
		{
			nMainVersion = resp.nProtMainVer;
			nSubVersion = resp.nProtSubVer;
		}
		else
		{
			result = IPVA_DTC_ERR_OP_FAILED;
			nCode = resp.header.pkt_status;
		}
	}
	return result;
}

int CDataLink::Register(unsigned char nChnl, unsigned char nMode, int& nCode)
{
	if (bRegister)
	{
		// ͨ����ע�ᣬ���سɹ�����
		nCode = 0;
		return IPVA_DTC_OK;
	}

	reqpkt_data_register_t req;
	respkt_data_register_t resp;

	set_reqpkt_data_register_t(req, nChnl, nMode);
	int recvRemainLen;
	int result = ExecCtrlCmd(reinterpret_cast<char*>(&req), sizeof(req), reinterpret_cast<char*>(&resp), recvRemainLen, nCode);
	if (result == IPVA_DTC_OK)
	{
		convert_respkt_data_t(resp);
		if (resp.pkt_status == S_DATA_OK)
		{
			switch(nMode)
			{
			case IPVA_DT_DATA_MODE_TCP:				// TCPģʽ
				if (recvRemainLen == sizeof(respl_data_mode_tcp_t))
				{
					respl_data_mode_tcp_t* payload = reinterpret_cast<respl_data_mode_tcp_t*>(extRecvBuffer);
					convert_respl_data_mode_tcp_t(*payload);
					nPort = payload->nPort;
					nTokenID = payload->nTokenID;
				}
				break;
			case IPVA_DT_DATA_MODE_TCP_REUSE:		// TCP����ģʽ
				break;
			case IPVA_DT_DATA_MODE_UDP:				// UDP����ģʽ
				break;
			case IPVA_DT_DATA_MODE_UDP_MULTICAST:	// UDP�鲥ģʽ
				break;
			default:
				break;
			}
			bRegister = true;
		}
		else
		{
			result = IPVA_DTC_ERR_OP_FAILED;
			nCode = resp.pkt_status;
		}
	}
	return result;
}

int CDataLink::SendToken(unsigned int nToken, int& nCode)
{
	reqpkt_datalink_data_token_t req;
	respkt_datalink_data_token_t resp;

	set_reqpkt_datalink_data_token_t(req, nToken);

	int recvRemainLen;
	int result = ExecDataCmd(reinterpret_cast<char*>(&req), sizeof(req), reinterpret_cast<char*>(&resp), recvRemainLen, nCode);
	if (result == IPVA_DTC_OK)
	{
		convert_respkt_datalink_data_t(resp);
		if (resp.pkt_status != S_DATA_OK)
		{
			result = IPVA_DTC_ERR_OP_FAILED;
			nCode = resp.pkt_status;
		}
	}
	return result;
}

int CDataLink::ExecCtrlCmd(const char* sndBuffer, int sndBufferLen, char* recvHeader, int& recvRemainLen, int& nCode)
{
	return ExecCmd(sCtrl, sndBuffer, sndBufferLen, recvHeader, recvRemainLen, nCode);
}

int CDataLink::ExecDataCmd(const char* sndBuffer, int sndBufferLen, char* recvHeader, int& recvRemainLen, int& nCode)
{
	return ExecCmd(sData, sndBuffer, sndBufferLen, recvHeader, recvRemainLen, nCode);
}

int CDataLink::ExecCmd(SOCKET s, const char* sndBuffer, int sndBufferLen, char* recvHeader, int& recvRemainLen, int& nCode)
{
	int result = IPVA_DTC_OK, code = 0;
	do 
	{
		recvRemainLen = 0;
		// ������Ϣ��������
		int ret = CNetOption::Send(s, sndBuffer, sndBufferLen);
		if (ret != 0)
		{
			result = IPVA_DTC_ERR_NET;
			code = ret;
			break;
		}
		// ���ձ�׼�ظ�������
		ret = CNetOption::Recv(s, recvHeader, sizeof(respkt_data_t));
		if (-1 == ret)
		{
			// ���ӱ��Ͽ�
			result = IPVA_DTC_ERR_CONN_CLOSE;
			break;
		}
		else if (0 != ret)
		{
			result = IPVA_DTC_ERR_NET;
			code = ret;
			break;
		}
		long pktRemainLen = ntohl(reinterpret_cast<respkt_data_t*>(recvHeader)->pkt_len) - sizeof(respkt_data_t);
		if (pktRemainLen > 16)
		{
			// δ������󣬳���Ԥ���Ļ�������С
			result = IPVA_DTC_UNDEFINE;
			break;
		}
		if (pktRemainLen > 0)
		{
			// ����ָ���ظ����ȳ���׼�ظ��������µ�����
			ret = CNetOption::Recv(sCtrl, extRecvBuffer, pktRemainLen);
			if (-1 == ret)
			{
				// ���ӱ��Ͽ�
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
	} while (false);
	nCode = code;
	return result;
}

int CDataLink::RcvData(char* rcvBuffer, int bufferLen, int& nCode)
{
	int result = IPVA_DTC_OK;
	nCode = 0;
	do 
	{
		int ret = CNetOption::Recv(sData, rcvBuffer, bufferLen);
		if (0 == ret)
		{
			// �����ɹ�����
			break;
		}
		if (-1 == ret)
		{
			// ���ӱ��Ͽ�
			result = IPVA_DTC_ERR_CONN_CLOSE;
		}
		else if (0 != ret)
		{
			result = IPVA_DTC_ERR_NET;
			nCode = ret;
		}
		if (hWndNotify && !bUnLink && result != IPVA_DTC_OK)
		{
			// �����쳣��Ϣ�����յĴ��ھ��
			//::PostMessage(hWndNotify, NETMSG_ERR_DATALINK, result, nCode);
		}
	} while (false);
	return result;
}

void CDataLink::set_reqpkt_data_login_t(reqpkt_data_login_t& req, const char* user, const char* psw)
{
	ZeroMemory(&req, sizeof(req));
	req.header.pkt_len = htonl(sizeof(req));
	req.header.pkt_type = htons(IPVA_DT_DATA_LOGIN);
	req.nProtMainVer = htons(HV_NET_PROTO_MAIN);
	req.nProtSubVer = htons(HV_NET_PROTO_SUB);
	strcpy_s(req.vUser, 16, user);
	strcpy_s(req.vPsw, 64, psw);
}

void CDataLink::convert_respkt_data_login_t(respkt_data_login_t& resp)
{
	resp.header.pkt_len = ntohl(resp.header.pkt_len);
	resp.header.pkt_status = ntohs(resp.header.pkt_status);
	resp.nProtMainVer = ntohs(resp.nProtMainVer);
	resp.nProtSubVer = ntohs(resp.nProtSubVer);
}

void CDataLink::set_reqpkt_data_register_t(reqpkt_data_register_t& req, unsigned char chnl, unsigned char mode)
{
	ZeroMemory(&req, sizeof(req));
	req.header.pkt_len = htonl(sizeof(req));
	req.header.pkt_type = htons(IPVA_DT_DATA_REG);
	req.nChnl = chnl;
	req.nMode = mode;
}

void CDataLink::convert_respkt_data_t(respkt_data_t& resp)
{
	resp.pkt_len = ntohl(resp.pkt_len);
	resp.pkt_status = ntohs(resp.pkt_status);
}

void CDataLink::convert_respl_data_mode_tcp_t(respl_data_mode_tcp_t& pl)
{
	pl.nPort = ntohs(pl.nPort);
	pl.nTokenID = ntohl(pl.nTokenID);
}

void CDataLink::set_reqpkt_datalink_data_token_t(reqpkt_datalink_data_token_t& req, unsigned int tokenId)
{
	ZeroMemory(&req, sizeof(req));
	req.header.pkt_len = htonl(sizeof(req));
	req.header.pkt_type = htons(IPVA_DT_DATA_DATALINK_TOKEN);
	req.nTokenid = htonl(tokenId);
}

void CDataLink::convert_respkt_datalink_data_t(respkt_datalink_data_t& resp)
{
	resp.pkt_len = ntohl(resp.pkt_len);
	resp.pkt_status = ntohs(resp.pkt_status);
}

void CDataLink::convert_datapkt_data_t(datapkt_data_t& pkt)
{
	pkt.pkt_len = ntohl(pkt.pkt_len);
	pkt.pkt_no = ntohl(pkt.pkt_no);
	pkt.pkt_payload_no = ntohl(pkt.pkt_payload_no);
}

void CDataLink::convert_datapl_data_t(datapl_data_t& pl)
{
	pl.pl_len = ntohl(pl.pl_len);
}

void CDataLink::convert_datapl_objnum_t(datapl_objnum_t& pl)
{
	pl.pl_objnum = ntohs(pl.pl_objnum);
}

void CDataLink::convert_datapl_algo_worktime_t(datapl_algo_worktime_t& pl)
{
	pl.pl_worktime = ntohs(pl.pl_worktime);
}

#ifndef _GUCH_WRITE_FILE_
#define _GUCH_WRITE_FILE_	1
#endif
unsigned int CDataLink::DataRcvThreadFunc(void* obj)
{
	CDataLink* link = reinterpret_cast<CDataLink*>(obj);
	int ret, nCode, pktLen, rcvLen = 0;		// �ֱ𱣴溯������ֵ���������ش����롢���������ݳ��ȡ��ѽ������ݳ���
	unsigned bufferLen = 17000;
	char* buffer = new char[bufferLen];

#if _GUCH_WRITE_FILE_
	FILE *pf = NULL;
	errno_t err = _wfopen_s(&pf, L"shipin.h264", L"w+b");
#endif

	std::vector<CObjLocInfo> vObjInfo;
	while (true)
	{
		// 1���Ƚ��ܰ�ͷ�����ݵ���Ŀ
		datapkt_data_t datapkt;   //��ͷ��֡��ţ������ݰ����ص���Ŀ
		ret = link->RcvData(reinterpret_cast<char*>(&datapkt), sizeof(datapkt), nCode);
		if (ret != IPVA_DTC_OK)
		{
			break;
		}
		convert_datapkt_data_t(datapkt);
		pktLen = datapkt.pkt_len;
		rcvLen = sizeof(datapkt);
#if _GUCH_WRITE_FILE_
		printf("Recv data header : pktLen = %d, sizeof(datapkt) = %d, nCode = %d\n", pktLen, rcvLen, nCode);
#endif
		// 2���ٽ�������
		datapl_data_t datapl;   //���س���,������������,��������,�����ֶ�
		unsigned int count;       
		int flags[2] = {false};   
		for (count = 0; count < datapkt.pkt_payload_no; count++)
		{
#if _GUCH_WRITE_FILE_
			printf("Recv data : pkt_payload_no = %d\n", datapkt.pkt_payload_no);
#endif

			ret = link->RcvData(reinterpret_cast<char*>(&datapl), sizeof(datapl), nCode);
			if (ret != IPVA_DTC_OK)
			{
				break;
			}
			convert_datapl_data_t(datapl);
			rcvLen += sizeof(datapl);
			if (bufferLen < datapl.pl_len)
			{
				// �������Ļ�����
				if (buffer)
				{
					delete[] buffer;
				}
				buffer = new char[datapl.pl_len];
				bufferLen = datapl.pl_len;
			} else {
				memset(buffer, '\0', bufferLen);
			}
			link->nCount++;
			if (link->nCount > 350 )
			{
				//::PostMessage(link->hWndNotify, NETMSG_ERR_DATALINK, IPVA_DTC_NO_FRAME, 0);
				link->nCount = 0;//�Ͽ����Ӻ�������0
			}
			//�������ʹ�����յ������ݣ�
			switch(datapl.pl_datatype)
			{
			case IPVA_DT_DATA_DATATYPE_VIDEO:	// ʵʱ��Ƶ����
				if (datapl.pl_type == IPVA_DT_DATA_TYPE_IDRFRAME)
				{
					ret = link->RcvData(buffer, datapl.pl_len - sizeof(datapl), nCode);
					if (ret != IPVA_DTC_OK)
					{
						goto END;
					}
#if _GUCH_WRITE_FILE_
					{
						if (0 != err) {

						} else {
							fwrite(buffer, datapl.pl_len - sizeof(datapl), 1, pf);
						}

					}
#endif

					//�Ƿ�����¼��
					/*link->bRecord = reinterpret_cast<ITCPModelDataProc*>(link->processModule)->ProcFrameData(datapkt.pkt_no, DATA_TYPE_IDRFRAME,
						reinterpret_cast<LPBYTE>(buffer), datapl.pl_len - sizeof(datapl));*/
					rcvLen += datapl.pl_len - sizeof(datapl);
					link->nCount = 0;
					break;
				}
				else if (datapl.pl_type == IPVA_DT_DATA_TYPE_BPFRAME)
				{
					ret = link->RcvData(buffer, datapl.pl_len - sizeof(datapl), nCode);
					if (ret != IPVA_DTC_OK)
					{
						goto END;
					}
#if _GUCH_WRITE_FILE_
					{
						if (0 != err) {

						} else {
							fwrite(buffer, datapl.pl_len - sizeof(datapl), 1, pf);
						}

					}
#endif
					/*link->bRecord = reinterpret_cast<ITCPModelDataProc*>(link->processModule)->ProcFrameData(datapkt.pkt_no, DATA_TYPE_BPFRAME,
						reinterpret_cast<LPBYTE>(buffer), datapl.pl_len - sizeof(datapl));*/
					rcvLen += datapl.pl_len - sizeof(datapl);

					link->nCount = 0;
					break;
				}

				goto DEFAULT;
			case IPVA_DT_DATA_DATATYPE_OBJLOC:	// ʵʱĿ��λ������
				if (datapl.pl_type == IPVA_DT_DATA_TYPE_OBJLOC)
				{
					vObjInfo.clear();
					datapl_objnum_t objnumpl;
					ret = link->RcvData(reinterpret_cast<char*>(&objnumpl), sizeof(objnumpl), nCode);
					if (ret != IPVA_DTC_OK)
					{
						goto END;
					}
					convert_datapl_objnum_t(objnumpl);
					// objNum = objnumpl.pl_objnum;
					CObjLocInfo objInfo;
					for (int count = 0; count < objnumpl.pl_objnum; count++)
					{
						ret = link->RcvData(reinterpret_cast<char*>(&objInfo), sizeof(objInfo), nCode);
						if (ret != IPVA_DTC_OK)
						{
							goto END;
						}
						objInfo.objID = htonl(objInfo.objID);
						objInfo.x = htons(objInfo.x);
						objInfo.y = htons(objInfo.y);
						vObjInfo.push_back(objInfo);
					}
					//reinterpret_cast<ITCPModelDataProc*>(link->processModule)->ProcObjectLocation(datapkt.pkt_no, vObjInfo.size() ? &vObjInfo[0] : NULL, vObjInfo.size());
					rcvLen += (sizeof(objnumpl) + objnumpl.pl_objnum * sizeof(objInfo));
					break;
				}
				goto DEFAULT;
			case IPVA_DT_DATA_DATATYPE_DEVTIME:	// ʵʱ�豸ʱ��
				if (datapl.pl_type == IPVA_DT_DATA_TYPE_DEVTIME && datapl.pl_len == sizeof(datapl) + sizeof(datapl_devtime_t))
				{
					datapl_devtime_t pl;
					ret = link->RcvData(reinterpret_cast<char*>(&pl), sizeof(pl), nCode);
					if (ret != IPVA_DTC_OK)
					{
						goto END;
					}
					//reinterpret_cast<ITCPModelDataProc*>(link->processModule)->ProcDevTime(datapkt.pkt_no, pl.pl_hour, pl.pl_minute, pl.pl_second, pl.pl_tms);
					rcvLen += sizeof(pl);
					break;
				}
				goto DEFAULT;
			case IPVA_DT_DATA_DATATYPE_ET:		// ͨ���㷨ƽ����ʱ����
				if (datapl.pl_type == IPVA_DT_DATA_TYPE_AGLOTIME && datapl.pl_len == sizeof(datapl) + sizeof(datapl_algo_worktime_t))
				{
					datapl_algo_worktime_t pl;
					ret = link->RcvData(reinterpret_cast<char*>(&pl), sizeof(pl), nCode);
					if (ret != IPVA_DTC_OK)
					{
						goto END;
					}
					convert_datapl_algo_worktime_t(pl);
					//reinterpret_cast<ITCPModelDataProc*>(link->processModule)->ProcAlgoWorkingTime(datapkt.pkt_no, pl.pl_worktime);
					rcvLen += sizeof(pl);
					break;
				}
				goto DEFAULT;
			case IPVA_DT_DATA_DATATYPE_COUNTER:                  // ���������ݻ�ȡ Add by WangJie [2014-8-26]
				if (datapl.pl_type == IPVA_DT_DATA_TYPE_COUTER )
				{
					int nlength = datapl.pl_len - sizeof(datapl);

					ret = link->RcvData(buffer, datapl.pl_len - sizeof(datapl), nCode);

					if (ret != IPVA_DTC_OK)
					{
						goto END;
					}

					//reinterpret_cast<ITCPModelDataProc*>(link->processModule)->ProcAllCounterData(datapkt.pkt_no, buffer, datapl.pl_len - sizeof(datapl));
					rcvLen += datapl.pl_len - sizeof(datapl);
					break;
					//��ȡ�����ݣ���Ҫ��ʾ
				}
				goto DEFAULT;

			case IPVA_DT_DATA_DATATYPE_TEST:	// ��������
			default:
DEFAULT:		ret = link->RcvData(buffer, datapl.pl_len, nCode);
				if (ret != IPVA_DTC_OK)
				{
					break;
				}
				break;
			}
		}

END:	if (count != datapkt.pkt_payload_no)
		{
			// �������ݹ����г����쳣
			break;
		}
		::Sleep(15);
	}
	if (!link->bUnLink)
	{
		// �߳��쳣ֹͣ��֪ͨ�û��������öϿ���������
	}
	delete[] buffer;
#if _GUCH_WRITE_FILE_
	if (NULL != pf) {
		fclose(pf);
		pf = NULL;
	}
#endif
	return 0;
}

unsigned int CDataLink::DataRcvAndSendThreadFunc(void* obj)
{
	CDataLink* link = reinterpret_cast<CDataLink*>(obj);
	int ret, nCode, pktLen, rcvLen = 0;		// �ֱ𱣴溯������ֵ���������ش����롢���������ݳ��ȡ��ѽ������ݳ���
	unsigned bufferLen = 17000;
	char* buffer = new char[bufferLen];

	int frame_id= 0, frame_time = 1000 / 25;

	/*if (NULL != link->hDisplay) {
	::SendMessage(link->hDisplay, TASK_MSG_COMPLETE, TASK_CONNECT_SERVER, 0);
	}*/
	

#if _GUCH_WRITE_FILE_
	FILE *pf = NULL;
	errno_t err = _wfopen_s(&pf, L"shipin.h264", L"w+b");

#endif

	std::vector<CObjLocInfo> vObjInfo;
	while (true)
	{
		// 1���Ƚ��ܰ�ͷ�����ݵ���Ŀ
		datapkt_data_t datapkt;   //��ͷ��֡��ţ������ݰ����ص���Ŀ
		ret = link->RcvData(reinterpret_cast<char*>(&datapkt), sizeof(datapkt), nCode);
		if (ret != IPVA_DTC_OK)
		{
			MessageBox(NULL, L"error : ��ͷ��֡��ţ������ݰ����ص���Ŀ error!", NULL, NULL);
			exit(0);
			break;
		}
		convert_datapkt_data_t(datapkt);
		pktLen = datapkt.pkt_len;
		rcvLen = sizeof(datapkt);
#if _GUCH_WRITE_FILE_
		//printf("Recv data header : pktLen = %d, sizeof(datapkt) = %d, nCode = %d\n", pktLen, rcvLen, nCode);
#endif
		// 2���ٽ�������
		datapl_data_t datapl;   //���س���,������������,��������,�����ֶ�
		unsigned int count;       
		int flags[2] = {false};   
		for (count = 0; count < datapkt.pkt_payload_no; count++)
		{
#if _GUCH_WRITE_FILE_
			//printf("Recv data : pkt_payload_no = %d\n", datapkt.pkt_payload_no);
#endif

			ret = link->RcvData(reinterpret_cast<char*>(&datapl), sizeof(datapl), nCode);
			if (ret != IPVA_DTC_OK)
			{
				MessageBox(NULL, L"error : ���س���,������������ error!", NULL, NULL);
				break;
			}
			convert_datapl_data_t(datapl);
			rcvLen += sizeof(datapl);
			if (bufferLen < datapl.pl_len)
			{
				// �������Ļ�����
				if (buffer)
				{
					delete[] buffer;
				}
				buffer = new char[datapl.pl_len];
				bufferLen = datapl.pl_len;
			} else {
				memset(buffer, '\0', bufferLen);
			}
			link->nCount++;
			if (link->nCount > 350 )
			{
				//::PostMessage(link->hWndNotify, NETMSG_ERR_DATALINK, IPVA_DTC_NO_FRAME, 0);
				link->nCount = 0;//�Ͽ����Ӻ�������0
			}
			//�������ʹ�����յ������ݣ�
			switch(datapl.pl_datatype)
			{
			case IPVA_DT_DATA_DATATYPE_VIDEO:	// ʵʱ��Ƶ����
				if (datapl.pl_type == IPVA_DT_DATA_TYPE_IDRFRAME)
				{
					ret = link->RcvData(buffer, datapl.pl_len - sizeof(datapl), nCode);
					if (ret != IPVA_DTC_OK)
					{
						goto END;
					}
#if _GUCH_WRITE_FILE_
					{
						if (0 != err) {

						} else {
							fwrite(buffer, datapl.pl_len - sizeof(datapl), 1, pf);
							/*memcpy(link->pMyBuffer + link->iFrameSize, buffer, datapl.pl_len - sizeof(datapl));
							link->iFrameSize += datapl.pl_len - sizeof(datapl);*/
						}

					}
#endif
					link->SendVideoToAgentServer(link, buffer, datapl.pl_len - sizeof(datapl));
					//if (1 == link->nSendVideoFlag)
					//{
					//	memcpy(link->pMyBuffer+link->iFrameSize,buffer,datapl.pl_len - sizeof(datapl));
					//	link->iFrameSize += datapl.pl_len - sizeof(datapl);
					//	int one_frame_len = 0;
					//	one_frame_len = link->get_frame_length((BYTE*)(link->pMyBuffer+link->iFrameFlag), link->iFrameSize-link->iFrameFlag);
					//	if (0 >= one_frame_len) {
					//		printf("error : get one frame len = %d\r\n", one_frame_len);
					//		break;
					//	}
					//	fwrite(link->pMyBuffer+link->iFrameFlag,one_frame_len,1,fp_1);
					//	fflush(fp_1);
					//	printf("one frame is %d\r\n",one_frame_len);

					//	char headbuf[16] = { '\0' };
					//	int m_temp = 0;

					//	memset(headbuf,0,16);
					//	m_temp=12;
					//	memcpy(headbuf,(char *)&m_temp,4);
					//	m_temp=SPFS;
					//	memcpy(headbuf+4,(char *)&m_temp,4);
					//	memcpy(headbuf+8,(char *)&frame_id,4);
					//	memcpy(headbuf+12,(char *)&frame_time,4);
					//	// �ܳ���
					//	m_temp = 16 + one_frame_len;
					//	m_temp = CNetOption::Send(link->sAgentServerData, (char *)&m_temp, sizeof(int));
					//	if (0 != m_temp) {
					//		printf("send video head error!\r\n");
					//		getch();
					//	}
					//	// head
					//	m_temp = CNetOption::Send(link->sAgentServerData, headbuf, 16);
					//	if (0 != m_temp) {
					//		printf("send video head error!\r\n");
					//		getch();
					//	}
					//	// һ֡����
					//	m_temp = CNetOption::Send(link->sAgentServerData, link->pMyBuffer+link->iFrameFlag, one_frame_len);
					//	if (0 != m_temp) {
					//		printf("send video head error!\r\n");
					//		getch();
					//	}
					//	frame_id++;
					//	link->iFrameFlag += one_frame_len;

					//}
					//�Ƿ�����¼��
					link->bRecord = reinterpret_cast<ITCPModelDataProc*>(link->processModule)->ProcFrameData(datapkt.pkt_no, DATA_TYPE_IDRFRAME,
						reinterpret_cast<LPBYTE>(buffer), datapl.pl_len - sizeof(datapl));
					rcvLen += datapl.pl_len - sizeof(datapl);
					link->nCount = 0;
					break;
				}
				else if (datapl.pl_type == IPVA_DT_DATA_TYPE_BPFRAME)
				{
					ret = link->RcvData(buffer, datapl.pl_len - sizeof(datapl), nCode);
					if (ret != IPVA_DTC_OK)
					{
						goto END;
					}
#if _GUCH_WRITE_FILE_
					{
						if (0 != err) {

						} else {
							fwrite(buffer, datapl.pl_len - sizeof(datapl), 1, pf);
							/*memcpy(link->pMyBuffer+link->iFrameSize,buffer,datapl.pl_len - sizeof(datapl));
							link->iFrameSize += datapl.pl_len - sizeof(datapl);*/
						}

					}
#endif
					link->SendVideoToAgentServer(link, buffer, datapl.pl_len - sizeof(datapl));
					//if (1 == link->nSendVideoFlag)
					//{
					//	memcpy(link->pMyBuffer+link->iFrameSize,buffer,datapl.pl_len - sizeof(datapl));
					//	link->iFrameSize += datapl.pl_len - sizeof(datapl);
					//	int one_frame_len = 0;
					//	one_frame_len = link->get_frame_length((BYTE*)(link->pMyBuffer+link->iFrameFlag), link->iFrameSize-link->iFrameFlag);
					//	if (0 >= one_frame_len) {
					//		printf("error : get one frame len = %d\r\n", one_frame_len);
					//		break;
					//	}
					//	fwrite(link->pMyBuffer+link->iFrameFlag,one_frame_len,1,fp_1);
					//	fflush(fp_1);
					//	printf("one frame is %d\r\n",one_frame_len);

					//	char headbuf[16] = { '\0' };
					//	int m_temp = 0;

					//	memset(headbuf,0,16);
					//	m_temp=12;
					//	memcpy(headbuf,(char *)&m_temp,4);
					//	m_temp=SPFS;
					//	memcpy(headbuf+4,(char *)&m_temp,4);
					//	memcpy(headbuf+8,(char *)&frame_id,4);
					//	memcpy(headbuf+12,(char *)&frame_time,4);
					//	// �ܳ���
					//	m_temp = 16 + one_frame_len;
					//	m_temp = CNetOption::Send(link->sAgentServerData, (char *)&m_temp, sizeof(int));
					//	if (0 != m_temp) {
					//		printf("send video head error!\r\n");
					//		getch();
					//	}
					//	// head
					//	m_temp = CNetOption::Send(link->sAgentServerData, headbuf, 16);
					//	if (0 != m_temp) {
					//		printf("send video head error!\r\n");
					//		getch();
					//	}
					//	// һ֡����
					//	m_temp = CNetOption::Send(link->sAgentServerData, link->pMyBuffer+link->iFrameFlag, one_frame_len);
					//	if (0 != m_temp) {
					//		printf("send video head error!\r\n");
					//		getch();
					//	}
					//	frame_id++;
					//	link->iFrameFlag += one_frame_len;

					//}					
					link->bRecord = reinterpret_cast<ITCPModelDataProc*>(link->processModule)->ProcFrameData(datapkt.pkt_no, DATA_TYPE_BPFRAME,
						reinterpret_cast<LPBYTE>(buffer), datapl.pl_len - sizeof(datapl));
					rcvLen += datapl.pl_len - sizeof(datapl);

					link->nCount = 0;
					break;
				}

				goto DEFAULT;
			case IPVA_DT_DATA_DATATYPE_OBJLOC:	// ʵʱĿ��λ������
				if (datapl.pl_type == IPVA_DT_DATA_TYPE_OBJLOC)
				{
					vObjInfo.clear();
					datapl_objnum_t objnumpl;
					ret = link->RcvData(reinterpret_cast<char*>(&objnumpl), sizeof(objnumpl), nCode);
					if (ret != IPVA_DTC_OK)
					{
						goto END;
					}
					convert_datapl_objnum_t(objnumpl);
					// objNum = objnumpl.pl_objnum;
					CObjLocInfo objInfo;
					for (int count = 0; count < objnumpl.pl_objnum; count++)
					{
						ret = link->RcvData(reinterpret_cast<char*>(&objInfo), sizeof(objInfo), nCode);
						if (ret != IPVA_DTC_OK)
						{
							goto END;
						}
						objInfo.objID = htonl(objInfo.objID);
						objInfo.x = htons(objInfo.x);
						objInfo.y = htons(objInfo.y);
						vObjInfo.push_back(objInfo);
					}
					//reinterpret_cast<ITCPModelDataProc*>(link->processModule)->ProcObjectLocation(datapkt.pkt_no, vObjInfo.size() ? &vObjInfo[0] : NULL, vObjInfo.size());
					rcvLen += (sizeof(objnumpl) + objnumpl.pl_objnum * sizeof(objInfo));
					break;
				}
				goto DEFAULT;
			case IPVA_DT_DATA_DATATYPE_DEVTIME:	// ʵʱ�豸ʱ��
				if (datapl.pl_type == IPVA_DT_DATA_TYPE_DEVTIME && datapl.pl_len == sizeof(datapl) + sizeof(datapl_devtime_t))
				{
					datapl_devtime_t pl;
					ret = link->RcvData(reinterpret_cast<char*>(&pl), sizeof(pl), nCode);
					if (ret != IPVA_DTC_OK)
					{
						goto END;
					}
					//reinterpret_cast<ITCPModelDataProc*>(link->processModule)->ProcDevTime(datapkt.pkt_no, pl.pl_hour, pl.pl_minute, pl.pl_second, pl.pl_tms);
					rcvLen += sizeof(pl);
					break;
				}
				goto DEFAULT;
			case IPVA_DT_DATA_DATATYPE_ET:		// ͨ���㷨ƽ����ʱ����
				if (datapl.pl_type == IPVA_DT_DATA_TYPE_AGLOTIME && datapl.pl_len == sizeof(datapl) + sizeof(datapl_algo_worktime_t))
				{
					datapl_algo_worktime_t pl;
					ret = link->RcvData(reinterpret_cast<char*>(&pl), sizeof(pl), nCode);
					if (ret != IPVA_DTC_OK)
					{
						goto END;
					}
					convert_datapl_algo_worktime_t(pl);
					//reinterpret_cast<ITCPModelDataProc*>(link->processModule)->ProcAlgoWorkingTime(datapkt.pkt_no, pl.pl_worktime);
					rcvLen += sizeof(pl);
					break;
				}
				goto DEFAULT;
			case IPVA_DT_DATA_DATATYPE_COUNTER:                  // ���������ݻ�ȡ Add by WangJie [2014-8-26]
				if (datapl.pl_type == IPVA_DT_DATA_TYPE_COUTER )
				{
					int nlength = datapl.pl_len - sizeof(datapl);

					ret = link->RcvData(buffer, datapl.pl_len - sizeof(datapl), nCode);

					if (ret != IPVA_DTC_OK)
					{
						goto END;
					}

					//reinterpret_cast<ITCPModelDataProc*>(link->processModule)->ProcAllCounterData(datapkt.pkt_no, buffer, datapl.pl_len - sizeof(datapl));
					rcvLen += datapl.pl_len - sizeof(datapl);
					break;
					//��ȡ�����ݣ���Ҫ��ʾ
				}
				goto DEFAULT;

			case IPVA_DT_DATA_DATATYPE_TEST:	// ��������
			default:
DEFAULT:		ret = link->RcvData(buffer, datapl.pl_len, nCode);
				if (ret != IPVA_DTC_OK)
				{
					break;
				}
				break;
			}
		}

END:	if (count != datapkt.pkt_payload_no)
		{
			// �������ݹ����г����쳣
			break;
		}
		::Sleep(15);
	}
	if (!link->bUnLink)
	{
		// �߳��쳣ֹͣ��֪ͨ�û��������öϿ���������
	}
	delete[] buffer;
#if _GUCH_WRITE_FILE_
	if (NULL != pf) {
		fclose(pf);
		pf = NULL;
	}
#endif
	return 0;
}

int CDataLink::SendVideoToAgentServer(CDataLink *link, char *buffer, int buf_len)
{
	int ret = 0;
	if (1 == link->nSendVideoFlag)
	{
		memcpy(link->pMyBuffer + link->iFrameSize, buffer, buf_len);
		link->iFrameSize += buf_len;
		int one_frame_len = 0;
		one_frame_len = link->get_frame_length((BYTE*)(link->pMyBuffer+link->iFrameFlag), link->iFrameSize-link->iFrameFlag);
		if (0 >= one_frame_len) {
			printf("error : get one frame len = %d\r\n", one_frame_len);
			return ret;
		}
		printf("one frame is %d\r\n",one_frame_len);

		char headbuf[16] = { '\0' };
		int m_temp = 0;

		memset(headbuf, 0, 16);
		m_temp=12;
		memcpy(headbuf,(char *)&m_temp, 4);
		m_temp=SPFS;
		memcpy(headbuf + 4,(char *)&m_temp, 4);
		memcpy(headbuf + 8,(char *)&link->iFrameID, 4);
		memcpy(headbuf + 12,(char *)&link->iFrameTime, 4);
		// �ܳ���
		m_temp = 16 + one_frame_len;
		m_temp = CNetOption::Send(link->sAgentServerData, (char *)&m_temp, sizeof(int));
		if (0 != m_temp) {
			printf("send video head error!\r\n");
			getch();
		}
		// head
		m_temp = CNetOption::Send(link->sAgentServerData, headbuf, 16);
		if (0 != m_temp) {
			printf("send video head error!\r\n");
			getch();
		}
		// һ֡����
		m_temp = CNetOption::Send(link->sAgentServerData, link->pMyBuffer+link->iFrameFlag, one_frame_len);
		if (0 != m_temp) {
			printf("send video head error!\r\n");
			getch();
		}
		link->iFrameID++;
		link->iFrameFlag += one_frame_len;
	}
	ret = 1;
	return ret;
}

//�õ�һ֡���ݵĳ���
int CDataLink::get_frame_length(BYTE *buf, int buf_size)
{
    int i;
    unsigned int state=0;
    int frame_start_found=0;
    for(i=0; i<=buf_size; i++){
        if(state==0x00000001){
            if(frame_start_found)
			{
                if (i >= buf_size) 
					break;
				state=-1; 
				return i-4;
            }
			//* frame_start=i-4;
            frame_start_found = 1;
        }
        if (i<buf_size) {
            state= (state<<8) | buf[i];
		}
    }
    return -1;
}

unsigned int CDataLink::LinkAgentServerThreadFunc(void* obj)
{
	CDataLink *p_link = (CDataLink *)obj;
	ProcessXML process_xml;
	XML_Char p_xml_buf[MAX_OUTPUT_BUFFER_] = { '\0' };
	XmlElement *xml_frame = NULL;
	int send_msg = 0;

	xml_frame = process_xml.RegConnectM("586","2","�ɼ���7","�ͻ���_zhang");
	process_xml.CreateXmlBuffer(p_xml_buf, xml_frame);

	SendMegHead send_data;
	send_data.xml_len = strlen(p_xml_buf);
	send_data.version = 100;
	send_data.cmd_code = CMD_CODE_CREATE_CONNECT;
	send_data.source_id = 10000;
	send_data.dest_id = 7;
	send_data.data_len = sizeof(SendMegHead) + send_data.xml_len;

	process_xml.PrintXmlBufferWithColor(p_xml_buf);
	send_msg = CNetOption::Send(p_link->sAgentServerData, (char *)&send_data, sizeof(SendMegHead));
	send_msg = CNetOption::Send(p_link->sAgentServerData, p_xml_buf, send_data.xml_len);

	char buf[2048] = { '\0' };
	int rev_len = 0;
	while (TRUE) {
		// TEST ��������
		memset(buf,0, sizeof(buf));
		send_msg = CNetOption::Recv(p_link->sAgentServerData, buf, sizeof(int));
		rev_len = *(int *)buf;
		memset(buf,0, sizeof(buf));
		send_msg = CNetOption::Recv(p_link->sAgentServerData, buf, rev_len);
		rev_len = *(int *)(buf + 4);
		switch (rev_len) {
		case SJYZ:
			printf("SJYZ\r\n");
			break;
		case BCLJ:
			printf("BCLJ\r\n");
			break;
		case FSTX:
			printf("FSTX\r\n");
			p_link->nSendVideoFlag = 1;
			break;
		}
		Sleep(500);

		//send_msg = CNetOption::Recv(p_link->sAgentServerData, buf, );

		//ProcessXML process_xml;
		//XmlElement *client_recv;

		////��������
		//memset(buf,0, sizeof(buf));
		//send_msg = CNetOption::Recv(p_link->sAgentServerData, buf, sizeof(RecvMegHead));
		//RecvMegHead *p_recv_head = (RecvMegHead *)buf;
		//char xml_buf[MAX_OUTPUT_BUFFER_] = { '\0' };
		//send_msg = CNetOption::Recv(p_link->sAgentServerData, xml_buf, p_recv_head->xml_len);
		//client_recv = process_xml.RegAssign2AccessKeepConnectM();
		//int flag = process_xml.GetElementInXmlBuffer(client_recv, xml_buf);
		//if (1 == flag) {
		//	process_xml.FreeXmlElement(client_recv);
		//	continue;
		//}
		//process_xml.FreeXmlElement(client_recv);
		//client_recv = process_xml.RegAssign2AccessSendVideoM();
		//flag = process_xml.GetElementInXmlBuffer(client_recv, xml_buf);
		//if (1 == flag) {
		//	// send video data
		//	p_link->nSendVideoFlag = 1;
		//}
		//process_xml.FreeXmlElement(client_recv);
	}

	return 0;
}