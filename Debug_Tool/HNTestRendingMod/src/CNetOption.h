#ifndef _CNET_OPTION_H_
#define _CNET_OPTION_H_

#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")

// ��������ֵ
#define IPVA_DTC_UNDEFINE					-1			// δ���巵��ֵ
#define IPVA_DTC_OK							0			// ִ�гɹ�
#define IPVA_DTC_ERR_NET					1			// �����쳣
#define IPVA_DTC_ERR_CONN_CLOSE				2			// ���ӱ��Ͽ�
#define IPVA_DTC_ERR_CHECKSUM				3			// У��ͳ���
#define IPVA_DTC_ERR_OP_FAILED				4			// �ظ����з����벻�ɹ�
#define IPVA_DTC_ERR_BT_FAILED				5			// �����߳�ʧ��
#define IPVA_DTC_ERR_PACKAGE_LEN			6			// ���ݰ������쳣
#define IPVA_DTC_ERR_ALGO_OP_FAILED			7			// �㷨���û�Ӧ�������벻�ɹ�
#define IPVA_DTC_ERR_ALGO_PACKAGE_LEN		8			// �㷨���û�Ӧ�������쳣
#define IPVA_DTC_ERR_ALGO_PARAM_CONV_FAILED	9			// �㷨����ת��ʧ��
#define IPVA_DTC_NO_FRAME					0x10		//��ʱδ�ӵ���Ƶ����

// ���ӳ�ʱ�趨
#define CTRLSNDTIMEOUT						3000		// ������·�������ӷ��ͳ�ʱֵ
#define CTRLRCVTIMEOUT						3000		// ������·�������ӽ��ճ�ʱֵ
#define DATASNDTIMEOUT						0			// ������·�������ӷ��ͳ�ʱֵ
#define DATARCVTIMEOUT						3000		// ������·�������ӽ��ճ�ʱֵ
#define THREADEXITTIMEOUT					2000		// �߳��˳���ʱֵ


class CNetOption {

public:
	CNetOption();
	~CNetOption();

	/**
	* @brief	��ʼ���׽��ֶ���
	* @details	��sndTimeoֵС�ڵ���0ʱ����ִ�����÷��ͳ�ʱ����rcvTimeoֵС�ڵ���0ʱ����ִ�����ý��ճ�ʱ
	* @param[in]	s����Ҫ���õ�SOCKET
	* @param[in]	sndTimeo�����ͳ�ʱֵ����λΪ����
	* @param[in]	rcvTimeo�����ճ�ʱֵ����λΪ����
	* @return	�ɹ�����0��ʧ�ܷ���Windows Sockets Error Codes
	*/
	static int InitSocket(SOCKET& s, int sndTimeo, int rcvTimeo);

	/**
	* @brief	����ָ��������
	* @details	����IPָ���ķ�������ָ���˿�
	* @param[in]	s����Ҫ�������ӵ��׽���
	* @param[in]	lpAddr����βΪ'\0'���ַ�����IP��ַ
	* @param[in]	port�����ӵĶ˿�
	* @return	�ɹ�����0��ʧ�ܷ���Windows Sockets Error Codes������δ֪�����򷵻�-1
	*/
	static int Connect(SOCKET s, const char* lpAddr, unsigned short port);

	/**
	* @brief	��������
	* @param[in]	s����Ҫִ�з��Ͳ������׽���
	* @param[in]	lpBuffer��ָ����Ҫ���͵����ݻ�����
	* @param[in]	len����Ҫ���͵����ݳ���
	* @return	�ɹ�����0��ʧ�ܷ���Windows Sockets Error Codes
	*/
	static int Send(SOCKET s, const char* lpBuffer, int len);
	
	/**
	* @brief	��������
	* @param[in]	s����Ҫִ�н��ղ������׽���
	* @param[in]	lpBuffer��ָ����Ҫ����������ݵĻ�����
	* @param[in]	len����Ҫ���յ����ݳ���
	* @return	�ɹ�����0��ʧ�ܷ���Windows Sockets Error Codes�������ѱ��Է��Ͽ��򷵻�-1
	*/
	static int Recv(SOCKET s, char* lpBuffer, int len);

	/**
	* @brief	�ر��׽���
	* @details	�ر��׽��ֵ�ͬʱ�Ѳ�����ΪINVALID_SOCKET
	* @param[in,out]	s����Ҫ�رյ��׽���
	*/
	void CloseSocket(SOCKET& s);

private:
	static int CreateSocket(SOCKET& s);
	static int SetTimeout(SOCKET s, int sndTimeo, int rcvTimeo);

//////////////////////////
public:

private:

};




#endif