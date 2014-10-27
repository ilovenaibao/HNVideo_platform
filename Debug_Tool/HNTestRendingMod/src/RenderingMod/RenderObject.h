#ifndef _IPVA_DTC_RENDER_OBJECT_H_
#define _IPVA_DTC_RENDER_OBJECT_H_

#include "../NetGobalDef.h"
#include <vector>
#include "data_protocol.h"

const int FRAME_TYPE_IDR = 1;
const int FRAME_TYPE_BP = 2;

/**
* @brief	��Ƶ֡�����ݼ���Ϣ
* @remark	����ʵ�������з����̲߳���ȫ
*/
class CRenderObject
{
public:
	CRenderObject(void);
	~CRenderObject(void);

	/**
	* @brief	��ʼ��ʵ��
	*/
	void Init(void);

	/**
	* @brief	�ͷ���Դ
	*/
	void Clear(void);

	/**
	* @brief	��ȡ��Ƶ֡���ݰ�����
	* @param[in]	lpData��ָ������ָ��
	* @param[in]	nDataLen�����ݳ���
	*/
	void GetFrameData(LPBYTE& lpData, unsigned int& nLen);

	/**
	* @brief	������Ƶ֡����
	* @param[in]	lpData��ָ����Ƶ֡����
	* @param[in]	nLen����Ƶ֡���ݳ���
	*/
	void SetFrameData(const BYTE* lpData, unsigned int nLen);

	/**
	* @brief	��ȡ֡���
	* @return	֡���
	*/
	unsigned int GetFrameNo(void);

	/**
	* @brief	������Ƶ֡���
	* @param[in]	nFrameNo����Ƶ֡���
	*/
	void SetFrameNo(unsigned int nFrameNo);

	/**
	* @brief	��ȡ��Ƶ֡���
	* @return	��Ƶ֡���
	*/
	int GetWidth(void);

	/**
	* @brief	������Ƶ֡���
	* @param[in]	nWidth����Ƶ֡���
	*/
	void SetWidth(int nWidth);

	/**
	* @brief	��ȡ��Ƶ֡�߶�
	* @return	��Ƶ֡�߶�
	*/
	int GetHeight(void);

	/**
	* @brief	������Ƶ֡�߶�
	* @param[in]	nHeight����Ƶ֡�߶�
	*/
	void SetHeight(int nHeight);

	/**
	* @brief	��ȡ��Ƶ֡
	* @return	��Ƶ֡����
	*/
	unsigned int GetType(void);

	/**
	* @brief	������Ƶ֡����
	* @param[in]	nType����Ƶ֡����
	*/
	void SetType(unsigned int nType);

	///**
	//* @brief	��ȡʵʱ������������
	//* @param[out]	nIn������������
	//* @param[out]	nOut������������
	//*/
	//void GetPC(unsigned short& nIn, unsigned short& nOut);

	///**
	//* @brief	����ʵʱ������������
	//* @param[in]	nIn������������
	//* @param[in]	nOut������������
	//*/
	//void SetPC(unsigned short nIn, unsigned short nOut);

	/**
	* @brief	��ȡ�豸ʵʱϵͳʱ��
	* @param[out]	nHour��Сʱ
	* @param[out]	nMinute������
	* @param[out]	nSecond������
	* @param[out]	nTenmillisecond���ٷ�һ��
	*/
	void GetDevTime(unsigned char& nHour, unsigned char& nMinute, unsigned char& nSecond, unsigned char& nTenmillisecond);

	/**
	* @brief	�����豸ʵʱϵͳʱ��
	* @param[in]	nHour��Сʱ
	* @param[in]	nMinute������
	* @param[in]	nSecond������
	* @param[in]	nTenmillisecond���ٷ�һ��
	*/
	void SetDevTime(unsigned char nHour, unsigned char nMinute, unsigned char nSecond, unsigned char nTenmillisecond);

	/**
	* @brief	����Ŀ��λ������
	* @param[in]	vObj��ָ��Ŀ��λ����������
	* @param[in]	nLen��Ŀ��λ�����鳤��
	*/
	void SetObjLoc(CObjLocInfo* vObj, unsigned int nLen);

	/**
	* @brief	��ȡĿ��λ������
	* @param[out]	vObj��ָ��Ŀ��λ����������
	* @param[out]	nLen��Ŀ��λ�����鳤��
	* @return	�˶����Ŀ��λ�������Ƿ��ѱ�����
	*/
	bool GetObjLoc(CObjLocInfo*& vObj, unsigned int& nLen);

	/**
	* @brief	��ȡ�㷨Ч��
	* @return	�㷨Ч��
	*/
	unsigned short GetAlgoET(void);

	/**
	* @brief	�����㷨Ч��
	* @param[in]	nAlgoET���㷨Ч��
	*/
	void SetAlgoET(unsigned short nAlgoET);

	/**
	* @brief	���õ�ǰʵ��
	*/
	void Reset(void);

    void GetAllCounterData(char* allCounterData, int nlength);
    int  GetAllCounterDataSize();
    void SetAllCounterData(const char* allCounterData, int nlen);

protected:
	// ��Ƶ֡��Ϣ
	unsigned int				frameNo;		// ֡���
	LPBYTE						buffer;			// ָ����Ƶ֡����
	unsigned int				bufferLen;		// ��������С
	unsigned int				dataLen;		// ��Ƶ֡���ݳ���
	int							width;			// ��Ƶ֡���
	int							height;			// ��Ƶ֡�߶�
	unsigned int				type;			// ��Ƶ֡����

	// ��Ƶ֡ʱ���
	unsigned char				hour;			// Сʱ
	unsigned char				minute;			// ����
	unsigned char				second;			// ����
	unsigned char				tenmillisecond;	// �ٷ�һ��

	// ʵʱ������������
	unsigned short				inPC, outPC;	// �������������

	// ����ӿ�����������
	unsigned short				inFMPC, outFMPC;// �������������
	bool						bSetFMPC;		// 5���ӽ��������Ƿ�����

    char* recvCounterData;
    int m_nlen;

	std::vector<CObjLocInfo>	vObjLoc;		// Ŀ������
	bool						bSetObjLoc;		// Ŀ�������Ƿ�����

	// �㷨��ʱ
	unsigned short				algoET;			// �㷨Ч�ʺ�ʱ

    CRITICAL_SECTION		hCs_recvData;				// ��������������ƹ��̵�ģ����ٽ���
    VECT_TRACK             vObjTrack;
    bool                   bSetObjTrack;
};

#endif
