#ifndef CLIENT_H
#define CLIENT_H

#include <vector>


/*
ClinetNet.h��
���ڱ�ʾһ���ͻ����������
*/

#include <stdio.h>
//#include<windows.h>
#include <WinSock2.h>
#include "../RenderingMod/ITCPModelDataProc.h"

#pragma comment(lib, "Ws2_32.lib")


#define SPFS 0x53465053


class CClientNet
{
public:
	CClientNet();
	//~CClientNet();
	//������ָ��������
	int Connect(SOCKET &s, int port, const char* address);

	int Run();
	int SendN(SOCKET s, const char* buf, int len);
	int RecvN(SOCKET s, char* buf, int len);

	//������Ϣ
	int SendMsg(const char* msg,int len);
	//�ر�
	void Close();

	static unsigned int __stdcall RunThreadPlayVideo(void *obj);
	static unsigned int __stdcall RunThreadPlayVideo_YUV(void *obj);

private:
	

public:
	SOCKET			m_sock;
	SOCKET			sCtrl;							// ������·�������ӵ��׽���
	SOCKET			sData;							// ������·�������ӵ��׽���

	unsigned short	nMainVersion, nSubVersion;		// �����Э�����汾����Э��ΰ汾��
	char			extRecvBuffer[16];				// ���������·�ظ������̶�ͷ����
	bool			bRegister;						// �Ƿ�ɹ�ע��ͨ��
	unsigned short	nPort;							// ���ݴ���ʹ�õĶ˿ڣ�����TCPģʽ��UDP����ģʽ��UDP�鲥ģʽ��ʹ�õĶ˿�
	unsigned int	nTokenID;						// ����ID
	HANDLE			hDataRcvThread;					// ���ݽ����߳̾��
	unsigned int	packNo;							// �����

	char *m_send_head_buf;
	int m_send_buf_len;
	char *m_send_xml;
	char *m_send_data;

	HWND hDisplayWnd;
	ITCPModelDataProc *processModule;							// ��Ƶ����ģ��

	/*char *video_buf;
	int max_video_buf;
	int video_buf_len;*/

};

void client_main(char *p_send_head, int send_buf_size, char *p_xml, char *p_data);


#endif