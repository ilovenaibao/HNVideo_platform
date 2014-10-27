/**
 * @file     CCtrlLink.h
 * @brief    ��������&��������
 * @author   guch
 * @date     2014-10-21
 *
 * @if copyright
 *
 * Copyright (C) 2014-201* guch
 *
 * ����HNԶ�̵��Թ����п�����·
 *
 * @endif
 *
 */

#ifndef _CCTRL_LINK_H_
#define _CCTRL_LINK_H_

#include "CNetOption.h"
#include "hv_types.h"

#define CONTROLPORT					20138			// ������·�˿�
#define CONTROLPORT_AGENT_SERVER	19531			// ������·

/**
* @brief	�����ֶ���
*/
#define	IPVA_DT_CTRL_LOGIN			0x0000			// ��¼����
#define	IPVA_DT_CTRL_REG			0x0001			// ע��Ŀ��ͨ��
#define IPVA_DT_CTRL_UNREG			0x0002			// ע��Ŀ��ͨ��
// 0x0003-0x00FF	ϵͳ��������

#define IPVA_DT_CTRL_HEARTBEAT		0x0100			// ��������
#define IPVA_DT_CTRL_GETPROTVER		0x0101			// ��ȡ�����Э��汾����
#define	IPVA_DT_CTRL_GETSRVVER		0x0102			// ��ȡ����˳���汾����
#define IPVA_DT_CTRL_REGCTRLPRIV	0x0103			// �������Ȩ��
#define IPVA_DT_CTRL_UNREGCTRLPRIV	0x0104			// ע������Ȩ��
#define IPVA_DT_CTRL_ALLOCALGO		0x0105			// IPVA�㷨���úͼ�������������
// 0x0106-0xFFFF	Э�鱣������

/**
* @brief	�����붨��
*/
#define S_CTRL_OK							0x0000	// �����ɹ�
#define S_ERR_CTRL_CONNECT_TIMEOUT			0x0001	// ���ӳ�ʱ
#define S_ERR_CTRL_ILLEGALACCESS			0x0002	// ����ʧ�ܣ��Ƿ�����
#define S_ERR_CTRL_USERORPWD_INCORRECT		0x0003	// ��¼ʧ�ܣ��û������������
#define S_ERR_CTRL_PROT_NOTSUPP				0x0004	// ��¼ʧ�ܣ�Э��汾����֧��
#define S_ERR_CTRL_CHNL_NOTSUPP				0x0005	// ע��ʧ�ܣ�Ŀ��ͨ������֧��
#define S_ERR_CTRL_CHNL_CONNECT_TOOMUCH		0x0006	// ע��ʧ�ܣ�Ŀ��ͨ������������
#define S_ERR_CTRL_CHNL_CTRL_USER_TOOMUCH	0x0007	// Ȩ������ʧ�ܣ�����Ȩ���û�����
#define S_ERR_CTRL_CHNL_NOT_CTRL_USER		0x0008	// Ȩ��ע��ʧ�ܣ��ǿ���Ȩ���û�
#define S_ERR_CTRL_CHNL_PERMISSION_DENIED	0x0009	// ����ʧ�ܣ�Ȩ�޲���
#define S_ERR_CTRL_LOGOFF_FAILED			0x000A	// ע��ʧ��
#define S_ERR_CTRL_CMD_UNSUPPORT			0x000B	// ���֧��


/**
* @brief	���������
*/
typedef struct _reqpkt_t
{
	HV_U32		pkt_num;	// �����
	HV_U16		pkt_chksum;	// ��У���
	HV_U16		pkt_type;	// ��������
	HV_U32		pkt_pldlen;	// ��������صĳ���
}
reqpkt_t;

/**
* @brief	����Ӧ���
*/
typedef	struct _respkt_t
{
	HV_U32		pkt_num;	// ��������
	HV_U16		pkt_chksum;	// ��У���
	HV_U16		pkt_status; // Ӧ����
	HV_U32		pkt_pldlen;	// Ӧ������صĳ���
}
respkt_t;

/**
* @brief	��¼����
*/
typedef struct _reqpkt_login_t
{
	reqpkt_t	header;
	HV_U16		nCProtMainVer;	// �ͻ���Э�����汾
	HV_U16		nCProtSubVer;	// �ͻ���Э��ΰ汾
	HV_S8		vUser[16];		// �û���
	HV_S8		vPsw[64];		// ����
}
reqpkt_login_t;
typedef respkt_t respkt_login_t;

/**
* @brief	ע��Ŀ��ͨ��
*/
typedef struct _reqpkt_register_t
{
	reqpkt_t	header;
	HV_U8		nChnl;			// Ŀ��ͨ����
	HV_U8		nReserve[3];	// �����ֶ�
}
reqpkt_register_t;
typedef respkt_t respkt_register_t;

/**
* @brief	ע��Ŀ��ͨ��
*/
typedef reqpkt_t reqpkt_unregister_t;
typedef	respkt_t respkt_unregister_t;

/**
* @brief	��ȡ�����Э��汾
*/
typedef reqpkt_t reqpkt_get_server_protocol_t;
typedef struct _respkt_get_server_protocol_t
{
	respkt_t	header;
	HV_U16		nProtMainVer;		// Э�����汾��
	HV_U16		nProtSubVer;		// Э��˰汾��
}
respkt_get_server_protocol_t;

/**
* @brief	��ȡ�������汾
*/
typedef reqpkt_t reqpkt_get_server_version_t;
typedef struct _respkt_get_server_version_t
{
	respkt_t	header;
	HV_S8		vVersion[32];
}
respkt_get_server_version_t;

/**
* @brief	�������Ȩ��
*/
typedef reqpkt_t reqpkt_reg_ctrl_priv_t;
typedef struct _respkt_reg_ctrl_priv_t
{
	respkt_t	header;
	HV_U8		nReserve[4];
}
respkt_reg_ctrl_priv_t;

/**
* @brief	ע������Ȩ��
*/
typedef reqpkt_t reqpkt_unreg_ctrl_priv_t;
typedef respkt_t respkt_unreg_ctrl_priv_t;

/**
* @brief	IPVA�㷨��������
*/
typedef struct _reqpkt_set_algo_params_t
{
	reqpkt_t	header;
	HV_U8		nReserve[4];
}
reqpkt_set_algo_params_t;
typedef respkt_t respkt_set_algo_params_t;


class CCtrlLink : public CNetOption {

public:
	CCtrlLink(void);
	~CCtrlLink(void);

	/**
	* @brief	��ָ��IP���豸��������
	* @details	�˷���������ӣ���¼��ע��Ĺ���
	* @param[in]	HWND�����տ�����·��������Ϣ�Ĵ��ڣ�ΪNULL�򲻷�����������Ϣ
	* @param[in]	lpAddr����'\0'��β��IP�ַ���
	* @param[in]	nChnl��ͨ����
	* @param[out]	nCode��������
	* @return	������
	*/
	int Link(HWND hWnd, const char* lpAddr, unsigned char nChnl, int& nCode);

	/**
	* @brief	�Ͽ�����
	* @details	�Ͽ����豸����
	*/
	void UnLink(void);

	/**
	* @brief	��ָ��IP�Ĵ����������������
	* @details	�˷����������
	* @param[in]	HWND�����տ�����·��������Ϣ�Ĵ��ڣ�ΪNULL�򲻷�����������Ϣ
	* @param[in]	lpAddr����'\0'��β��IP�ַ���
	* @param[in]	nChnl��ͨ����
	* @param[out]	nCode��������
	* @return	������
	*/
	int LinkAgentServer(HWND hWnd, const char* lpAddr, unsigned char nChnl, int& nCode);

	/**
	* @brief	��ȡ�����Э��汾
	* @param[out]	nMainVer�������Э�����汾
	* @param[out]	nSubVer�������Э��ΰ汾
	* @param[out]	nCode��������
	* @return	������
	*/
	int GetServerProtocol(unsigned short& nMainVer, unsigned short& nSubVer, int& nCode);

	/**
	* @brief	��ȡ����˳���汾
	* @param[out]	sVersion������˳���汾���������ṩ�Ļ��������Ȳ�С��32���ֽ�
	* @param[out]	nCode��������
	* @return	������
	*/
	int GetServerVersion(char* sVersion, int& nCode);

	/**
	* @brief	�������Ȩ��
	* @param[out]	nCode��������
	* @return	�ɹ�����IPVA_DTC_OK��ʧ�ܷ��ض�Ӧ�ķ���ֵ��nCode���ݷ���ֵ��������Windows Sockets Error Codes���ǻظ����еķ�����
	*/
	int UpdatePrivileges(int& nCode);

	/**
	* @brief	ע������Ȩ��
	* @param[out]	nCode��������
	* @return	�ɹ�����IPVA_DTC_OK��ʧ�ܷ��ض�Ӧ�ķ���ֵ��nCode���ݷ���ֵ��������Windows Sockets Error Codes���ǻظ����еķ�����
	*/
	int CancelPrivileges(int& nCode);

	/**
	* @brief	�����㷨
	* @param[in]	lpData��ָ����Ҫ��Ϊ���ص�����
	* @param[in]	nLen�����ݳ���
	* @param[out]	nRespDataLen���ظ����еĸ������ݳ���
	* @param[out]	nCode��������
	* @return	������
	*/
	int AollocAlgo(LPBYTE lpData, unsigned int nLen, unsigned int& nRespDataLen, int& nCode);

	/**
	* @brief	��ȡ�ظ��еĸ�������
	* @return	��ʵ������ظ����и������ݵĻ�����
	* @remark	������Ӧ�ڷ���AollocAlgo���ú�����ʵ����������ǰ���е���
	*/
	LPBYTE GetRespData(void);

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
	* @param[out]	nCode��������
	* @return	�ɹ�����IPVA_DTC_OK��ʧ�ܷ��ض�Ӧ�ķ���ֵ��nCode���ݷ���ֵ��������Windows Sockets Error Codes���ǻظ����еķ�����
	*/
	int Register(unsigned char nChnl, int& nCode);

	/**
	* @brief	ע��Ŀ��ͨ��
	* @param[out]	nCode��������
	* @return	�ɹ�����IPVA_DTC_OK��ʧ�ܷ��ض�Ӧ�ķ���ֵ��nCode���ݷ���ֵ��������Windows Sockets Error Codes���ǻظ����еķ�����
	*/
	int UnRegister(int& nCode);

	/**
	* @brief	ִ�п�������
	*/
	int ExecCmd(const char* sndBuffer, int sndBufferLen, char* recvHeader, int& recvRemainLen, int& nCode);

	/**
	* @brief	���reqpkt_get_server_protocol_t����
	* @details	������ͬʱ�ѱ����ֽ���ת���������ֽ���
	* @param[out]	req����Ҫ���Ķ���
	* @param[in]	packNo�������
	*/
	static void set_reqpkt_get_server_protocol_t(reqpkt_get_server_protocol_t& req, unsigned int packNo);

	/**
	* @brief	ת��respkt_get_server_protocol_t����
	* @details	�Ѷ���ʵ���е������ֽ���ת��Ϊ�����ֽ���
	* @param[in,out]	resp����Ҫת���Ķ���
	*/
	static void convert_respkt_get_server_protocol_t(respkt_get_server_protocol_t& resp);

	/**
	* @brief	ת��respkt_t����
	* @details	�Ѷ���ʵ���е������ֽ���ת��Ϊ�����ֽ���
	* @param[in,out]	resp����Ҫת���Ķ���
	*/
	static void convert_respkt_t(respkt_t& resp);

	/**
	* @brief	ת��respkt_get_server_version_t����
	* @details	�Ѷ���ʵ���е������ֽ���ת��Ϊ�����ֽ���
	* @param[in,out]	resp����Ҫת���Ķ���
	*/
	static void convert_respkt_get_server_version_t(respkt_get_server_version_t& resp);

	/**
	* @brief	���reqpkt_reg_ctrl_priv_t����
	* @details	������ͬʱ�ѱ����ֽ���ת���������ֽ���
	* @param[out]	req����Ҫ���Ķ���
	* @param[in]	packNo�������
	*/
	static void set_reqpkt_reg_ctrl_priv_t(reqpkt_reg_ctrl_priv_t& req, unsigned int packNo);

	/**
	* @brief	ת��respkt_reg_ctrl_priv_t����
	* @details	�Ѷ���ʵ���е������ֽ���ת��Ϊ�����ֽ���
	* @param[in,out]	resp����Ҫת���Ķ���
	*/
	static void convert_respkt_reg_ctrl_priv_t(respkt_reg_ctrl_priv_t& resp);

	/**
	* @brief	���reqpkt_unregister_t����
	* @details	������ͬʱ�ѱ����ֽ���ת���������ֽ���
	* @param[out]	req����Ҫ���Ķ���
	* @param[in]	packNo�������
	*/
	static void set_reqpkt_unreg_ctrl_priv_t(reqpkt_unreg_ctrl_priv_t& req, unsigned int packNo);

	/**
	* @brief	���reqpkt_set_algo_params_t����
	* @details	������ͬʱ�ѱ����ֽ���ת���������ֽ���
	* @param[out]	req����Ҫ���Ķ���
	* @param[in]	packNo�������
	* @param[in]	pldLen�����س���
	*/
	static void set_reqpkt_set_algo_params_t(reqpkt_set_algo_params_t& req, unsigned int packNo, unsigned int pldLen);

	/**
	* @brief	���reqpkt_data_unregister_t����
	* @details	������ͬʱ�ѱ����ֽ���ת���������ֽ���
	* @param[out]	req����Ҫ���Ķ���
	* @param[in]	packNo�������
	* @param[in]	mainVer���ͻ��˵�Э�����汾��
	* @param[in]	subVer���ͻ��˵�Э��ΰ汾��
	* @param[in]	user���û���
	* @param[in]	psw������
	*/
	static void set_reqpkt_login_t(reqpkt_login_t& req, unsigned int packNo, unsigned short mainVer, unsigned short subVer, const char* user, const char* psw);

	/**
	* @brief	���reqpkt_get_server_version_t����
	* @details	������ͬʱ�ѱ����ֽ���ת���������ֽ���
	* @param[out]	req����Ҫ���Ķ���
	* @param[in]	packNo�������
	*/
	void set_reqpkt_get_server_version_t(reqpkt_get_server_version_t& req, unsigned int packNo);

	/**
	* @brief	���reqpkt_register_t����
	* @details	������ͬʱ�ѱ����ֽ���ת���������ֽ���
	* @param[out]	req����Ҫ���Ķ���
	* @param[in]	packNo�������
	* @param[in]	chnl��ͨ����
	*/
	static void set_reqpkt_register_t(reqpkt_register_t& req, unsigned int packNo, unsigned char chnl);

	/**
	* @brief	���reqpkt_data_unregister_t����
	* @details	������ͬʱ�ѱ����ֽ���ת���������ֽ���
	* @param[out]	req����Ҫ���Ķ���
	* @param[in]	packNo�������
	*/
	static void set_reqpkt_unregister_t(reqpkt_unregister_t& req, unsigned int packNo);

public:

	SOCKET				s;						// �׽���
	SOCKET				sAgentServerCtrl;		// ������·���Ϳ������������������׽���

	char*				extRecvBuffer;			// ���������·�ظ������̶�ͷ����
	unsigned int		extRecvBufferSize;		// ��������С
	unsigned int		packNo;					// �����
	HANDLE				hHBThread;				// �����߳̾��
	HANDLE				hExitHBThreadEvent;		// �˳������߳��¼����
	bool				bRegister;				// �Ƿ�ɹ�ע��ͨ��

	CRITICAL_SECTION	hExecCmdCS;				// ִ��������
	HWND				hHBProcWnd;				// ������������Ϣ�Ĵ���

private:


};



#endif