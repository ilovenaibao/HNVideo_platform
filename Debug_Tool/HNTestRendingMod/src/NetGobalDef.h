#ifndef _IPVA_DTC_NET_FUNC_RET_DEF_H_
#define _IPVA_DTC_NET_FUNC_RET_DEF_H_

#include "NetTrans.h"
#include <vector>
using namespace  std;


// msg
#define TASK_MSG_COMPLETE					(WM_USER + 1000)		// �������֪ͨ����Ϣ

// 
#define TASK_DEFUALT						0x0000
#define TASK_CONNECT_SERVER					0x0001
#define TASK_DISCONNECT_SERVER				0x0002



#define CHNL_NUM					16			// ͨ����

// ��������ֵ
#define IPVA_DTC_UNDEFINE			-1			// δ���巵��ֵ
#define IPVA_DTC_OK					0			// ִ�гɹ�
#define IPVA_DTC_ERR_NET			1			// �����쳣
#define IPVA_DTC_ERR_CONN_CLOSE		2			// ���ӱ��Ͽ�
#define IPVA_DTC_ERR_CHECKSUM		3			// У��ͳ���
#define IPVA_DTC_ERR_OP_FAILED		4			// �ظ����з����벻�ɹ�
#define IPVA_DTC_ERR_BT_FAILED		5			// �����߳�ʧ��
#define IPVA_DTC_ERR_PACKAGE_LEN	6			// ���ݰ������쳣
#define IPVA_DTC_ERR_ALGO_OP_FAILED	7			// �㷨���û�Ӧ�������벻�ɹ�
#define IPVA_DTC_ERR_ALGO_PACKAGE_LEN	8		// �㷨���û�Ӧ�������쳣
#define IPVA_DTC_ERR_ALGO_PARAM_CONV_FAILED	9	// �㷨����ת��ʧ��
#define IPVA_DTC_NO_FRAME			0x10		//��ʱδ�ӵ���Ƶ����

// ����ģʽ
#define DATA_MODE_TCP				0			// TCPģʽ
#define DATA_MODE_TCP_REUSE			1			// TCP����ģʽ
#define DATA_MODE_UDP				2			// UDP����
#define DATA_MODE_UDP_MULTICAST		3			// UDP�鲥

// ��Ƶ֡����
#define DATA_TYPE_IDRFRAME			0x01		// I֡��IDR֡
#define DATA_TYPE_BPFRAME			0x02		// B֡��P֡

/**
* @brief	ʵʱĿ����������
*/
class CObjLocInfo
{
public:
	unsigned int	objID;
	unsigned short	x;
	unsigned short	y;
};

typedef std::vector<CObjLocInfo>   VECT_TRACK;
typedef VECT_TRACK::iterator       VECT_TRACK_ITER;


class CConnectServerStruct
{
public:
	char			ip[16];				// IP��ַ
	unsigned char	chnl;				// ͨ����
	unsigned char	mode;				// ģʽ

	unsigned short	srvNetProtMainVer;	// ����������ͨѶЭ�����汾��
	unsigned short	srvNetProtSubVer;	// ����������ͨѶЭ��ΰ汾��
	char			serverVer[32];		// ����������汾

	unsigned short	srvAlgoProtMainVer;	// �������㷨����Э�����汾��
	unsigned short	srvAlgoProtSubVer;	// �������㷨����Э��ΰ汾��

	unsigned int	algoType[CHNL_NUM];	// ����ͨ������ִ�е��㷨����
	unsigned int	algoTypeNum;		// ͨ����
	char			g2pcVer[32];		// G2PC�㷨�汾��

	CNetTrans*		netTransMod;		// ���紫��ģ��
	void*			procMod;			// ���ݴ���ģ��
	HWND			hWnd;				// ������������Ϣ�Ĵ��ھ��

	bool			bSilent;			// �Ƿ�Ĭִ�У�������ʱ����ģʽ������ʾ������Ϣ
	bool			bResult;			// �Ƿ�ɹ���������
};

#endif