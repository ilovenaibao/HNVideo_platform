#ifndef _IPVA_DTC_NET_TRANS_H_
#define _IPVA_DTC_NET_TRANS_H_

#include "CDataLink.h"
#include "CCtrlLink.h"
#include "INetTransmission.h"

class CNetTrans : public INetTransmission
{
public:
	CNetTrans(void);
	~CNetTrans(void);

	/**
	* @brief	��ʼ��
	*/
	void Init(void);

	/**
	* @brief	���ռ�õ���Դ
	*/
	void Clear(void);

	/**
	* @brief	ͬʱ���ӷ������Ŀ��ƶ������ݶ�
	* @param[in]	hWnd��������������Ϣ�Ĵ��ھ��
	* @param[in]	lpAddr����������ַ
	* @param[in]	nChnl�����ӵ�ͨ����
	* @param[in]	nMode����������ʹ�õ�ģʽ
	* @param[in]	proc����������ʹ�õ����ݴ���ģ��
	* @param[out]	nCode��������
	* @return	����ִ�н��
	* @remark	����ִ�е���ϸ�����Ҫ���ݷ���ֵ��������ж�
	*/
	int Connect(HWND hWnd, LPCSTR lpAddr, unsigned char nChnl, unsigned char nMode, void* procMod, int& nCode);

	/**
	* @brief	���ӷ��������ݶ�
	* @param[in]	lpAddr����������ַ
	* @param[in]	nChnl�����ӵ�ͨ����
	* @param[in]	nMode����������ʹ�õ�ģʽ
	* @param[in]	proc����������ʹ�õ����ݴ���ģ��
	* @param[out]	nCode��������
	* @return	����ִ�н��
	* @remark	����ִ�е���ϸ�����Ҫ���ݷ���ֵ��������ж�
	*/
	int ConnectDataLink(LPCSTR lpAddr, unsigned char nChnl, unsigned char nMode, void* procMod, int& nCode);

	/**
	* @brief	���ӷ��������ƶ�
	* @param[in]	hWnd������������ʧ����Ϣ�Ĵ���
	* @param[in]	lpAddr����������ַ
	* @param[in]	nChnl�����ӵ�ͨ����
	* @param[out]	nCode��������
	* @return	����ִ�н��
	* @remark	����ִ�е���ϸ�����Ҫ���ݷ���ֵ��������ж�
	*/
	int ConnectCtrlLink(HWND hWnd, LPCSTR lpAddr, unsigned char nChnl, int& nCode);

	void DisConnectCtrlLink(void);

	void DisConnectDataLink(void);

	/**
	* @brief	�Ͽ�������
	*/
	void DisConnect(void);

	/**
	* @brief	ע�����Ȩ��
	* @param[out]	nCode��������
	* @return	����ִ�н��
	* @remark	����ִ�е���ϸ�����Ҫ���ݷ���ֵ��������ж�
	*/
	int UpdatePrivileges(int& nCode);

	/**
	* @brief	ע������Ȩ��
	* @param[out]	nCode��������
	* @return	����ִ�н��
	* @remark	����ִ�е���ϸ�����Ҫ���ݷ���ֵ��������ж�
	*/
	int CancelPrivileges(int& nCode);

	/**
	* @brief	��ȡ������Э��汾
	* @param[out]	nMainVer��������Э�����汾
	* @param[out]	nSubVer��������Э��ΰ汾
	* @param[out]	nCode��������
	* @return	����ִ�н��
	* @remark	����ִ�е���ϸ�����Ҫ���ݷ���ֵ��������ж�
	*/
	int GetServerProtocol(unsigned short& nMainVer, unsigned short& nSubVer, int& nCode);

	/**
	* @brief	��ȡ�������汾
	* @param[out]	sVersion���������汾
	* @param[out]	nCode��������
	* @return	����ִ�н��
	* @remark	����ִ�е���ϸ�����Ҫ���ݷ���ֵ��������ж�
	*/
	int GetServerVersion(char* sVersion, int& nCode);

	/**
	* @brief	���ý��������쳣֪ͨ�Ĵ���
	* @param[in]	ctrlLink��������·�����쳣��Ҫ֪ͨ�Ĵ��ھ��
	* @param[in]	dataLink��������·�����쳣��Ҫ֪ͨ�Ĵ��ھ��
	*/
	void SetNetErrorNotify(HWND ctrlLink, HWND dataLink);

	/**
	* @brief	�㷨��������
	* @param[in]	lpReq����Ҫ�������������
	* @param[in]	nReqLen����Ҫ������������ݳ���
	* @param[out]	lpResp��������ָ������ظ����еĸ�������ָ�룬����һ�ε��ñ�����ǰ��Ч
	* @param[out]	nRespLen������ذ��и������ݵĳ���
	* @param[out]	nCode��������
	* @return	����ִ�н��
	* @remark	����ִ�е���ϸ�����Ҫ���ݷ���ֵ��������ж�
	*/
	virtual int ExecCmd(LPBYTE lpReq, unsigned int nReqLen, LPBYTE& lpResp, unsigned int& nRespLen, int& nCode);

public:
	CCtrlLink ctrlLink;		// ������·����
	CDataLink dataLink;		// ������·����
};

#endif
