#ifndef PROCESS_XML_H
#define PROCESS_XML_H


//#ifndef  _SCEWPRC_H
//#define  _SCEWPRC_H

// for debug
#ifndef _DEBUG_FLAG_
#define _DEBUG_FLAG_	0
#endif


#include <iostream>
#include "HNProtocol.h"
#include "./leak_detector/leak_detector_c.h"
using namespace std;



class ProcessXML:public HNProtocol
{
public:
	ProcessXML();
	~ProcessXML();

public:
	void GetHeartBeat(int version) {}
	void SendHeartBeat(int version) {}
	int  SendVideoReq(int version);


public:
	// �ͷ�ע��
	void FreeXmlElement(XmlElement *p);
	// ע������(������ͨ�ŵ�ʱ���׷�)
	XmlElement *ProcessXML::RegConnectM(const char *p_session = NULL, const char *p_client_type = NULL,
		const char *p_video_name = NULL, const char *p_usr_name = NULL);
	// �ַ�������->�ͻ���(�ͻ��˷�����Ƶ����֮����������ص���Ϣ)
	XmlElement *ProcessXML::RegAssign2ClientM(const char *p_error = NULL, const char *p_session = NULL);
	// �����������ʾ����
	XmlElement *RegAccessShowContentM();
	// �ַ�������->���������(��������)
	XmlElement *ProcessXML::RegAssign2AccessKeepConnectM(const char *p_error = NULL, 
		const char *p_session = NULL, const char *p_status_type = NULL, const char *p_status_time = NULL);
	// �ַ�������->���������(ֹͣ������Ƶ)
	XmlElement *ProcessXML::RegAssign2AccessSendVideoM(const char *p_session = NULL,
		const char *p_video_req = NULL);

public:
	XmlElement *xml_frame;

private:


};

char* strescape(char const *src, unsigned const int size);

//#endif

#endif // PROCESS_XML_H