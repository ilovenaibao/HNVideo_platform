#ifndef _IPVA_DTC_INETTRANSMISSION_H_
#define _IPVA_DTC_INETTRANSMISSION_H_

class INetTransmission
{
public:
	/**
	* @brief	�����˷�������
	* @param[in]	lpReq��ָ���������ݵ�ָ��
	* @param[in]	nReqLen���������ݵ����ݳ���
	* @param[out]	lpResp����������ظ��Ļ�����ָ��
	* @param[out]	nRespLen����������ظ������ݳ���
	* @param[out]	nCode��������
	* @return	����ֵ��������ĺ�����������ֵ��ȷ��
	*/
	virtual int ExecCmd(LPBYTE lpReq, unsigned int nReqLen, LPBYTE& lpResp, unsigned int& nRespLen, int& nCode) = 0;

	/**
	* @brief	ע�����Ȩ��
	* @param[out]	nCode��������
	* @return	����ִ�н��
	* @remark	����ִ�е���ϸ�����Ҫ���ݷ���ֵ��������ж�
	*/
	virtual int UpdatePrivileges(int& nCode) = 0;

	/**
	* @brief	ע������Ȩ��
	* @param[out]	nCode��������
	* @return	����ִ�н��
	* @remark	����ִ�е���ϸ�����Ҫ���ݷ���ֵ��������ж�
	*/
	virtual int CancelPrivileges(int& nCode) = 0;
};

#endif
