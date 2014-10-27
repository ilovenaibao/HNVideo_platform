/**
 * @file     CDataLink.h
 * @brief    ��������&��������
 * @author   guch
 * @date     2014-10-21
 *
 * @if copyright
 *
 * Copyright (C) 2014-201* guch
 *
 * ����HNԶ�̵��Թ�����������·
 *
 * @endif
 *
 */

#ifndef _CDATA_LINK_H_
#define _CDATA_LINK_H_

#include "CNetOption.h"
#include "hv_types.h"
//#include <vector>


#define DATACTRLPORT					20148		// ������·(���ɼ���)�˿�
#define DATACTRLPORT_AGENT_SERVER		19531		// ������·(�����������)�˿�

// ����ͻ�����ַ���������������
#define SPFS							0x53465053	// ��Ƶ����head��ʶ
#define FSTX							0x58545346	// ���������������Ƶ����ı�ʶ
#define BCLJ							0x4A4C4342  // ��������
#define SJYZ							0x5A594A53  // ʱ����֤

/**
* @brief	����Э�������ֶ���
*/
#define	IPVA_DT_DATA_LOGIN				0x0000		// ��¼����
#define	IPVA_DT_DATA_REG				0x0001		// ע��Ŀ��ͨ��
#define IPVA_DT_DATA_UNREG				0x0002		// ע��Ŀ��ͨ��
#define IPVA_DT_DATA_HEARTBEAT			0x0003		// ��������
#define IPVA_DT_DATA_REQDATA			0x0004		// ��������
// 0x0005 �C 0xFFFF						Э�鱣������

/**
* @brief	����Э�������ֶ���
*/
#define IPVA_DT_DATA_DATALINK_TOKEN		0x0000		// ������֤
// 0x0001 �C 0xFFFF						Э�鱣������

/**
* @brief	֧�ֵĴ���ģʽ
*/
#define IPVA_DT_DATA_MODE_TCP			0			// TCPģʽ
#define IPVA_DT_DATA_MODE_TCP_REUSE		1			// TCP����ģʽ
#define IPVA_DT_DATA_MODE_UDP			2			// UDP����
#define IPVA_DT_DATA_MODE_UDP_MULTICAST	3			// UDP�鲥

/**
* @brief	Ӧ���붨��
*/
#define S_DATA_OK						0x0000		// �����ɹ�
#define S_ERR_DATA_CONNECT_TIMEOUT		0x0001		// ���ӳ�ʱ
#define S_ERR_DATA_ILLEGALACCESS		0x0002		// ����ʧ�ܣ��Ƿ�����
#define S_ERR_DATA_USERORPWD_INCORRECT	0x0003		// ��¼ʧ�ܣ��û������������
#define S_ERR_DATA_PROT_NOTSUPP			0x0004		// ��¼ʧ�ܣ�Э��汾����֧��
#define S_ERR_DATA_CHNL_NOTSUPP			0x0005		// ע��ʧ�ܣ�Ŀ��ͨ������֧��
#define S_ERR_DATA_CHNL_CONNECT_TOOMUCH	0x0006		// ע��ʧ�ܣ�Ŀ��ͨ������������
#define S_ERR_DATA_REQ_NOTSUPP			0x0007		// ��������ʧ�ܣ���������֧��
#define S_ERR_DATA_LOGOFF_FAILED		0x0008		// ע��ʧ��
#define S_ERR_DATA_CMD_UNSUPPORT		0x0009		// ���֧��
#define S_ERR_DATA_CMD_TOKEN			0x000A		// ������֤ʧ��

/**
* @brief	��������
*/
#define IPVA_DT_DATA_DATATYPE_TEST		0x00		// �������������������
#define IPVA_DT_DATA_DATATYPE_VIDEO		0x01		// ʵʱ��Ƶ����
//#define IPVA_DT_DATA_DATATYPE_PC		0x02		// ʵʱ������������
//#define IPVA_DT_DATA_DATATYPE_FMPC	0x03		// ʵʱ5���ӽ�����������
#define IPVA_DT_DATA_DATATYPE_OBJLOC	0x04		// ʵʱĿ��λ������
#define IPVA_DT_DATA_DATATYPE_DEVTIME	0x05		// ʵʱ�豸ʱ��
#define IPVA_DT_DATA_DATATYPE_ET		0x06		// ͨ���㷨ƽ����ʱ����
#define IPVA_DT_DATA_DATATYPE_COUNTER   0x07        // �������������������� Add by WangJie [2014-8-26]
// 0x07 �� 0xFF	����

/**
* @brief	��������
*/
#define IPVA_DT_DATA_TYPE_TEST			0x00		// ���������԰�
#define IPVA_DT_DATA_TYPE_IDRFRAME		0x01		// H.264 IDR֡
#define IPVA_DT_DATA_TYPE_BPFRAME		0x02		// H.264 B/P֡
// 0x03 �C 0x0F	�������ͣ�������Ƶ����������չ
//#define IPVA_DT_DATA_TYPE_PC			0x10		// ʵʱ��������
//#define IPVA_DT_DATA_TYPE_FMPC			0x11		// ʵʱ5����ͳ�ƽ�������
#define IPVA_DT_DATA_TYPE_OBJLOC		0x12		// ʵʱĿ����������
// 0x13 �C 0x1F	�������ͣ������㷨��������չ
#define IPVA_DT_DATA_TYPE_DEVTIME		0x20		// ʵʱ�豸ʱ��
#define IPVA_DT_DATA_TYPE_AGLOTIME		0x21		// ʵʱ�㷨����ʱ�䣨ƽ��ʱ�䣬����/֡��     
#define IPVA_DT_DATA_TYPE_COUTER        0x22        // �������ĸ�������Add by WangJie [2014-8-26]
// 0x22 �� 0x2F	�������ͣ������豸��Ϣ����������Ϣ����չ
// 0x30 �� 0xFF	�������ͣ�����������������������չ


/**
* @brief	������·����Э����������
*/
typedef struct _reqpkt_data_t
{
	HV_U32		pkt_len;		// ������
	HV_U16		pkt_type;		// ��������
	HV_U8		pkt_reserve[2];	// �����ֶ�
}
reqpkt_data_t;

/**
* @brief	������·����Э�����Ӧ���
*/
typedef struct _respkt_data_t
{
	HV_U32		pkt_len;			// ������
	HV_U16		pkt_status;			// ��Ӧ����
	HV_U8		pkt_reserve[2];		// �����ֶ�
}
respkt_data_t;

/**
* @brief	��¼����
*/
typedef struct _reqpkt_data_login_t
{
	reqpkt_data_t	header;
	HV_U16			nProtMainVer;	// Э�����汾��
	HV_U16			nProtSubVer;	// Э��ΰ汾��
	HV_S8			vUser[16];		// �û���
	HV_S8			vPsw[64];		// ����
}
reqpkt_data_login_t;

typedef struct _respkt_data_login_t
{
	respkt_data_t	header;
	HV_U16			nProtMainVer;	// Э�����汾��
	HV_U16			nProtSubVer;	// Э��ΰ汾��
}
respkt_data_login_t;

/**
* @brief	ע��Ŀ��ͨ��
*/
typedef struct _reqpkt_data_register_t 
{
	reqpkt_data_t	header;
	HV_U8			nChnl;			// ͨ����
	HV_U8			nMode;			// ����ģʽ
	HV_U8			nReserve[2];	// �����ֶ�
}
reqpkt_data_register_t;
typedef respkt_data_t respkt_data_register_t;

/**
* @brief	��TCPģʽע��Ŀ��ͨ��ʱ���ظ����еĸ���
*/
typedef struct _respl_data_mode_tcp_t
{
	HV_U16			nPort;			// TCP���ݷ������˿�
	HV_U16			nReserve1;		// �����ֶ�
	HV_U32			nTokenID;		// ����ID
}
respl_data_mode_tcp_t;

/**
* @brief	��UDP����ģʽע��Ŀ��ͨ��ʱ���ظ����еĸ���
*/
typedef struct _respl_data_mode_udp_t
{
	HV_U16			nPort;			// UDP���ݷ������˿�
	HV_U16			nReserve1;		// �����ֶ�
	HV_U32			nReserve2;		// �����ֶ�
}
respl_data_mode_udp_t;

/**
* @brief	��UDP�鲥ģʽע��Ŀ��ͨ��ʱ���ظ����еĸ���
*/
typedef struct _respl_data_mode_udp_multicast_t
{
	HV_U32			nIPAddr;		// �����鲥��ַ
	HV_U16			nPort;			// �����鲥�˿�
	HV_U16			nReserve1;		// �����ֶ�
	HV_U32			nReserve2;		// �����ֶ�
	HV_U32			nEncryptionKey;	// ������Կ
}
respl_data_mode_udp_multicast_t;

/**
* @brief	������·����Э����������
*/
typedef reqpkt_data_t reqpkt_datalink_data_t;

/**
* @brief	������·����Э�����Ӧ���
*/
typedef respkt_data_t respkt_datalink_data_t;

/**
* @brief	��������
*/
typedef struct _reqpkt_datalink_data_token_t
{
	reqpkt_datalink_data_t	header;
	HV_U32					nTokenid;// ����
}
reqpkt_datalink_data_token_t;
typedef respkt_datalink_data_t respkt_datalink_data_token_t;


/**
* @brief	���ݰ��̶�ͷ
*/
typedef struct _datapkt_data_t
{
	HV_U32			pkt_len;		// ���ݰ�����
	HV_U32			pkt_no;			// ֡���
	HV_U32			pkt_payload_no;	// �����ݰ����ص���Ŀ
}
datapkt_data_t;

/**
* @brief	���ݰ����ع̶�ͷ
*/
typedef struct _datapl_data_t
{
	HV_U32			pl_len;			// ���س���
	HV_U8			pl_datatype;	// ������������
	HV_U8			pl_type;		// ��������
	HV_U8			pl_reserve[2];	// �����ֶ�
}
datapl_data_t;

/**
* @brief	ʵʱ������������
*/
typedef struct _datapl_pc_t
{
	HV_U16			pl_in;			// ������
	HV_U16			pl_out;			// ������
}
datapl_pc_t;

/**
* @brief	5���ӿ�����������
*/
typedef datapl_pc_t datapl_fmpc_t;

/**
* @brief	ʵʱĿ����Ŀ
*/
typedef struct _datapl_objnum_t
{
	HV_U16			pl_objnum;		// ʵʱĿ����Ŀ
	HV_U16			pl_reserve;		// �����ֶ�
}
datapl_objnum_t;

/**
* @brief	Ŀ��λ����Ϣ
*/
typedef struct _datapl_objloc_t
{
	HV_U32			pl_objid;		// Ŀ��ID
	HV_S16			pl_x;			// x����
	HV_S16			pl_y;			// y����
}
datapl_objloc_t;

/**
* @brief	ʵʱ�豸ʱ��
*/
typedef struct _datapl_devtime_t
{
	HV_U8			pl_hour;		// ʱ
	HV_U8			pl_minute;		// ��
	HV_U8			pl_second;		// ��
	HV_U8			pl_tms;			// 10����
}
datapl_devtime_t;

/**
* @brief	ʵʱ�㷨����ʱ��
*/
typedef struct _datapl_algo_worktime_t
{
	HV_U16			pl_worktime;	// ʵʱ�㷨����ʱ��
	HV_U16			pl_reserve;		// �����ֶ�
}
datapl_algo_worktime_t;


///**
//* @brief	ʵʱĿ����������
//*/
//class CObjLocInfo
//{
//public:
//	unsigned int	objID;
//	unsigned short	x;
//	unsigned short	y;
//};
//
//typedef std::vector<CObjLocInfo>   VECT_TRACK;
//typedef VECT_TRACK::iterator       VECT_TRACK_ITER;


class CDataLink:public CNetOption {

public:
	CDataLink(void);
	~CDataLink(void);

	/**
	* @brief	��ָ��IP���豸��������
	* @details	�˷���������ӣ���¼��ע��Ĺ���
	* @param[in]	lpAddr����'\0'��β��IP�ַ���
	* @param[in]	nChnl��ͨ����
	* @param[in]	nMode������ģʽ
	* @param[in]	procMod��������·���ݴ���ģ��
	* @param[in]	nCode��������
	* @return	������
	* @remark	TCPģʽ�£�procMod�����ڱ���ָ��ʵ����ITCPModelDataProc�ӿڵ���ʵ��
	*/
	int Link(const char* lpAddr, unsigned char nChnl, unsigned char nMode, void* procMod, int& nCode);

	/**
	* @brief	�Ͽ�����
	* @details	�Ͽ����豸����
	*/
	void UnLink(void);

	/**
	* @brief	��ָ��IP�Ĵ����������������
	* @details	�˷����������
	* @param[in]	lpAddr����'\0'��β��IP�ַ���
	* @param[in]	nChnl��ͨ����
	* @param[in]	nMode������ģʽ
	* @param[in]	procMod��������·���ݴ���ģ��
	* @param[in]	nCode��������
	* @return	������
	* @remark	TCPģʽ�£�procMod�����ڱ���ָ��ʵ����ITCPModelDataProc�ӿڵ���ʵ��
	*/
	int CDataLink::LinkAgentServer(const char* lpAddr, unsigned char nChnl, unsigned char nMode, void* procMod, int& nCode);
	
	/**
	* @brief	�Ͽ���������������
	* @details	�Ͽ����豸����
	*/
	void CDataLink::UnLinkAgentServer(void);

	int get_frame_length(BYTE *buf, int buf_size);

private:

	/**
	* @brief	��¼
	* @param[out]	nCode��������
	* @return	�ɹ�����IPVA_DTC_OK��ʧ�ܷ��ض�Ӧ�ķ���ֵ��nCode���ݷ���ֵ��������Windows Sockets Error Codes���ǻظ����еķ�����
	*/
	int Login(int& nCode);

	/**
	* @brief	ע��Ŀ��ͨ��
	* @param[in]	nChnl��ͨ����
	* @param[in]	nMode������ģʽ
	* @param[out]	nCode��������
	* @return	�ɹ�����IPVA_DTC_OK��ʧ�ܷ��ض�Ӧ�ķ���ֵ��nCode���ݷ���ֵ��������Windows Sockets Error Codes���ǻظ����еķ�����
	*/
	int Register(unsigned char nChnl, unsigned char nMode, int& nCode);

	/**
	* @brief	ע��Ŀ��ͨ��
	* @param[out]	nCode��������
	* @return	�ɹ�����IPVA_DTC_OK��ʧ�ܷ��ض�Ӧ�ķ���ֵ��nCode���ݷ���ֵ��������Windows Sockets Error Codes���ǻظ����еķ�����
	*/
	int UnRegister(int& nCode);

	/**
	* @brief	��������
	* @param[in]	nToken������
	* @param[out]	nCode��������
	* @return	�ɹ�����IPVA_DTC_OK��ʧ�ܷ��ض�Ӧ�ķ���ֵ��nCode����Windows Sockets Error Codes
	*/
	int SendToken(unsigned int nToken, int& nCode);

	/**
	* @brief	��������·������ִ������
	*/
	int ExecCtrlCmd(const char* sndBuffer, int sndBufferLen, char* recvHeader, int& recvRemainLen, int& nCode);
	
	/**
	* @brief	��������·������ִ������
	*/
	int ExecDataCmd(const char* sndBuffer, int sndBufferLen, char* recvHeader, int& recvRemainLen, int& nCode);
	
	/**
	* @brief	ִ������
	*/
	int ExecCmd(SOCKET s, const char* sndBuffer, int sndBufferLen, char* recvHeader, int& recvRemainLen, int& nCode);
	
	/**
	* @brief	��������
	* @param[in]	rcvBuffer���������ݵĻ�����
	* @param[in]	bufferLen����Ҫ�������ݵĳ���
	* @param[out]	nCode��������
	* @return	������
	*/
	int RcvData(char* rcvBuffer, int bufferLen, int& nCode);

	/**
	* @brief	���reqpkt_data_login_t����
	* @details	������ͬʱ�ѱ����ֽ���ת���������ֽ���
	* @param[out]	req����Ҫ���Ķ���
	* @param[in]	user���û���
	* @param[in]	psw������
	*/
	static void set_reqpkt_data_login_t(reqpkt_data_login_t& req, const char* user, const char* psw);
	
	/**
	* @brief	ת��respkt_data_login_t����
	* @details	�Ѷ���ʵ���е������ֽ���ת��Ϊ�����ֽ���
	* @param[in,out]	resp����Ҫת���Ķ���
	*/
	static void convert_respkt_data_login_t(respkt_data_login_t& resp);
	
	/**
	* @brief	���reqpkt_data_register_t����
	* @details	������ͬʱ�ѱ����ֽ���ת���������ֽ���
	* @param[out]	req����Ҫ���Ķ���
	* @param[in]	chnl��ͨ����
	* @param[in]	mode������ģʽ
	*/
	static void set_reqpkt_data_register_t(reqpkt_data_register_t& req, unsigned char chnl, unsigned char mode);
	
	/**
	* @brief	ת��respkt_data_t����
	* @details	�Ѷ���ʵ���е������ֽ���ת��Ϊ�����ֽ���
	* @param[in,out]	resp����Ҫת���Ķ���
	*/
	static void convert_respkt_data_t(respkt_data_t& resp);
	
	/**
	* @brief	ת��respl_data_mode_tcp_t����
	* @details	�Ѷ���ʵ���е������ֽ���ת��Ϊ�����ֽ���
	* @param[in,out]	pl����Ҫת���Ķ���
	*/
	static void convert_respl_data_mode_tcp_t(respl_data_mode_tcp_t& pl);
	
	/**
	* @brief	���reqpkt_datalink_data_token_t����
	* @details	������ͬʱ�ѱ����ֽ���ת���������ֽ���
	* @param[out]	req����Ҫ���Ķ���
	* @param[in]	tokenId������ID
	*/
	static void set_reqpkt_datalink_data_token_t(reqpkt_datalink_data_token_t& req, unsigned int tokenId);
	
	/**
	* @brief	ת��respkt_datalink_data_t����
	* @details	�Ѷ���ʵ���е������ֽ���ת��Ϊ�����ֽ���
	* @param[in,out]	resp����Ҫת���Ķ���
	*/
	static void convert_respkt_datalink_data_t(respkt_datalink_data_t& resp);

	/**
	* @brief	ת��datapkt_data_t����
	* @details	�Ѷ���ʵ���е������ֽ���ת��Ϊ�����ֽ���
	* @param[in,out]	pkt����Ҫת���Ķ���
	*/
	static void convert_datapkt_data_t(datapkt_data_t& pkt);

	/**
	* @brief	ת��datapl_data_t����
	* @details	�Ѷ���ʵ���е������ֽ���ת��Ϊ�����ֽ���
	* @param[in,out]	pl����Ҫת���Ķ���
	*/
	static void convert_datapl_data_t(datapl_data_t& pl);

	/**
	* @brief	ת��datapl_objnum_t����
	* @details	�Ѷ���ʵ���е������ֽ���ת��Ϊ�����ֽ���
	* @param[in,out]	pl����Ҫת���Ķ���
	*/
	static void convert_datapl_objnum_t(datapl_objnum_t& pl);

	/**
	* @brief	ת��datapl_algo_worktime_t����
	* @details	�Ѷ���ʵ���е������ֽ���ת��Ϊ�����ֽ���
	* @param[in,out]	pl����Ҫת���Ķ���
	*/
	static void convert_datapl_algo_worktime_t(datapl_algo_worktime_t& pl);

	/**
	* @brief	���ݽ����̺߳���
	*/
	static unsigned int __stdcall DataRcvThreadFunc(void* obj);

	static unsigned int __stdcall DataRcvAndSendThreadFunc(void* obj);

	static unsigned int __stdcall LinkAgentServerThreadFunc(void* obj);

	int SendVideoToAgentServer(CDataLink *link, char *buffer, int buf_len);
	int SendVideoToAgentServer_old(CDataLink *link, char *buffer, int buf_len);

public:
	char* pMyBuffer;
	int iFrameSize;
	int iFrameFlag;
	int iFrameID;
	int iFrameTime;

	HWND hDisplay;

private:
	SOCKET	sCtrl;									// ������·�������ӵ��׽���
	SOCKET	sData;									// ������·�������ӵ��׽���
	SOCKET  sAgentServerData;						// ������·�������ݵ�������������׽���


	CRITICAL_SECTION	hExecCmdCS;					// ִ��������

	char				extRecvBuffer[16];			// ���������·�ظ������̶�ͷ����
	unsigned short		nMainVersion, nSubVersion;	// �����Э�����汾����Э��ΰ汾��
	unsigned short		nPort;						// ���ݴ���ʹ�õĶ˿ڣ�����TCPģʽ��UDP����ģʽ��UDP�鲥ģʽ��ʹ�õĶ˿�
	unsigned int		nTokenID;					// ����ID
	HANDLE				hExitHBThreadEvent;			// �˳������߳��¼����
	HANDLE				hHBThread;					// �����߳̾��
	HANDLE				hAgentServerThread;			// ���Ӵ���������߳̾��
	HANDLE				hDataRcvThread;				// ���ݽ����߳̾��
	void*				processModule;				// ָ�����ݴ���ģ��
	bool				bRegister;					// �Ƿ�ɹ�ע��ͨ��
	volatile bool		bUnLink;					// �Ƿ�ִ���˶Ͽ�����			
	unsigned int		nCount;						// �������յ�ʱ��������Ƶ���Ĵ���
	HWND				hWndNotify;					// ���������쳣ʱ��Ҫ֪ͨ�Ĵ���

	bool				bRecord;

	int					nSendVideoFlag;				// �Ƿ�����Ƶ�����������

};


#endif