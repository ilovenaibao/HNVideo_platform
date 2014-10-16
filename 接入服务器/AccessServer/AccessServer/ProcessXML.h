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
	// 释放注册
	void FreeXmlElement(XmlElement *p);
	// 注册连接(基本上通信的时候首发)
	XmlElement *ProcessXML::RegConnectM(const char *p_session = NULL, const char *p_client_type = NULL,
		const char *p_video_name = NULL, const char *p_usr_name = NULL);
	// 分发服务器->客户端(客户端发送视频请求之后服务器返回的消息)
	XmlElement *ProcessXML::RegAssign2ClientM(const char *p_error = NULL, const char *p_session = NULL);
	// 接入服务器显示内容
	XmlElement *RegAccessShowContentM();
	// 分发服务器->接入服务器(保持连接)
	XmlElement *ProcessXML::RegAssign2AccessKeepConnectM(const char *p_error = NULL, 
		const char *p_session = NULL, const char *p_status_type = NULL, const char *p_status_time = NULL);
	// 分发服务器->接入服务器(停止发送视频)
	XmlElement *ProcessXML::RegAssign2AccessSendVideoM(const char *p_session = NULL,
		const char *p_video_req = NULL);

public:
	XmlElement *xml_frame;

private:


};

char* strescape(char const *src, unsigned const int size);

//#endif

#endif // PROCESS_XML_H