#pragma once
#include <Vfw.h>

class CAviWriter
{
public:
	CAviWriter(void);
	~CAviWriter(void);

	/**
	* @brief	��AVI�ļ�
	* @param[in]	filename���ļ���
	* @return	�Ƿ���ָ��·���ɹ���AVI�ļ�
	* @remark	������trueʱ�������AVI_Close_file�ر��Ѵ򿪵�AVI�ļ�
	*/
	bool AVI_Create_Video_file(LPCTSTR filename);
	
	/**
	* @brief	����AVI��Ƶ��ʽ
	* @param[in]	width�����
	* @param[in]	height���߶�
	* @param[in]	compressor����Ƶ����ʹ�õı��룬��ǰʹ��H264��Ϊ�������null��β��4�ֽ��ַ���
	* @return	�Ƿ�ɹ�������Ƶ��ʽ
	* @remark	�ڵ���AVI_Write_frameǰ������ô˷�������AVI��Ƶ��ʽ
	*/
	bool AVI_Set_Video(int width,int height,char* compressor);

	/**
	* @brief	����Ƶ֡д��AVI��Ƶ����
	* @param[in]	pVideo��֡���ݣ�ֻ�ܰ���һ֡
	* @param[in]	byteLength��֡���ݳ���
	* @return	�Ƿ�ɹ�д��֡����
	*/
	bool AVI_Write_frame(const char* pVideo,long byteLength);

	/**
	* @brief	�ر�AVI�ļ�
	* @remark	������AVI_Create_Video_file���ʹ��
	*/
	void AVI_Close_file();//�ر�avi�ļ�

	/**
	* @brief	�ж�AVI�ļ��Ƿ��ѱ���
	* @return	�Ƿ��Ѵ�AVI�ļ�
	*/
	bool isOpen();


private:
	PAVIFILE			avi;		// AVI�ļ�
	PAVISTREAM			stream;		// AVI������
	long				frameNo;	// ֡���
	bool				setFormat;	// ��ʶ��������ʽ�Ƿ��ѱ�����
	CRITICAL_SECTION	hCs;		// �ٽ���
};

