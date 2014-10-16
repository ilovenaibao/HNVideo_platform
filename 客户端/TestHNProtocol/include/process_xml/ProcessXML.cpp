#include "ProcessXML.h"
#include "HNCommon.h"
#include <assert.h>


ProcessXML::ProcessXML()
{

}

ProcessXML::~ProcessXML()
{

}

int ProcessXML::SendVideoReq(int version)
{

	return 1;
}

void ProcessXML::FreeXmlElement(XmlElement *p)
{
	if (NULL != p) {
		free(p);
		p = NULL;
	}
}

// 注册连接(基本上通信的时候首发)
XmlElement *ProcessXML::RegConnectM(const char *p_session, const char *p_client_type,
	const char *p_video_name, const char *p_usr_name)
{
	char p_in[MAX_ELEMENT_SIZE] = { '\0' };
	char p_out[MAX_ELEMENT_SIZE] = { '\0' };
	const int element_size = 6;

	XmlElement *p_regist = (XmlElement *)malloc(element_size * sizeof(XmlElement));
	if (NULL == p_regist) {
		return p_regist;
	}

	XmlElement client_send_video_req[] = { 
		{XML_ROOT,			"Message" },
		{XML_ELEMENT,		"Session",		{ "" },		{ "" },		"",			{ "Message" }},
		{XML_ELEMENT,		"ClientType",	{ "" },		{ "" },		"",			{ "Message" }},
		{XML_ELEMENT,		"VideoName",	{ "" },		{ "" },		"",			{ "Message" }},
		{XML_ELEMENT,		"UsrName",		{ "" },		{ "" },		"默认用户",	{ "Message" }},
		{XML_EOF}
	};

	if (NULL != p_session) {
		memset(p_in, '\0', MAX_ELEMENT_SIZE);
		memset(p_out, '\0', MAX_ELEMENT_SIZE);
		memcpy(p_in, p_session, _MIN(strlen(p_session), MAX_ELEMENT_SIZE));
		GB2312ToUTF_8(p_out, p_in, strlen(p_in));

		memset(client_send_video_req[1].element_info, '\0', MAX_ELEMENT_SIZE);
		memcpy(client_send_video_req[1].element_info, p_out, _MIN(strlen(p_out), MAX_ELEMENT_SIZE));
	}
	if (NULL != p_client_type) {
		memset(p_in, '\0', MAX_ELEMENT_SIZE);
		memset(p_out, '\0', MAX_ELEMENT_SIZE);
		memcpy(p_in, p_client_type, _MIN(strlen(p_client_type), MAX_ELEMENT_SIZE));
		GB2312ToUTF_8(p_out, p_in, strlen(p_in));

		memset(client_send_video_req[2].element_info, '\0', MAX_ELEMENT_SIZE);
		memcpy(client_send_video_req[2].element_info, p_out, _MIN(strlen(p_out), MAX_ELEMENT_SIZE));
	}
	if (NULL != p_video_name) {
		memset(p_in, '\0', MAX_ELEMENT_SIZE);
		memset(p_out, '\0', MAX_ELEMENT_SIZE);
		memcpy(p_in, p_video_name, _MIN(strlen(p_video_name), MAX_ELEMENT_SIZE));
		GB2312ToUTF_8(p_out, p_in, strlen(p_in));

		memset(client_send_video_req[3].element_info, '\0', MAX_ELEMENT_SIZE);
		memcpy(client_send_video_req[3].element_info, p_out, _MIN(strlen(p_out), MAX_ELEMENT_SIZE));
	}
	if (NULL != p_usr_name) {
		memset(p_in, '\0', MAX_ELEMENT_SIZE);
		memset(p_out, '\0', MAX_ELEMENT_SIZE);
		memcpy(p_in, p_usr_name, _MIN(strlen(p_usr_name), MAX_ELEMENT_SIZE));
		GB2312ToUTF_8(p_out, p_in, strlen(p_in));

		memset(client_send_video_req[4].element_info, '\0', MAX_ELEMENT_SIZE);		
		memcpy(client_send_video_req[4].element_info, p_out, _MIN(strlen(p_out), MAX_ELEMENT_SIZE));
	}
	memcpy(p_regist, client_send_video_req, element_size * sizeof(XmlElement));
	return p_regist;
}

// 分发服务器->客户端(客户端发送视频请求之后服务器返回的消息)
XmlElement *ProcessXML::RegAssign2ClientM(const char *p_error, const char *p_session)
{
	char p_in[MAX_ELEMENT_SIZE] = { '\0' };
	char p_out[MAX_ELEMENT_SIZE] = { '\0' };
	const int element_size = 4;

	XmlElement *p_regist = (XmlElement *)malloc(element_size * sizeof(XmlElement));
	if (NULL == p_regist) {
		return p_regist;
	}

	XmlElement client_recv[] = { 
		{XML_ROOT,			"Message" },
		{XML_ELEMENT,		"Error",		{ "" }, { "" },	"",			{ "Message" }},
		{XML_ELEMENT,		"Session",		{ "" },	{ "" },	"",			{ "Message" }},
		{XML_EOF}
	};

	if (NULL != p_error) {
		memset(p_in, '\0', MAX_ELEMENT_SIZE);
		memset(p_out, '\0', MAX_ELEMENT_SIZE);
		memcpy(p_in, p_error, _MIN(strlen(p_error), MAX_ELEMENT_SIZE));
		GB2312ToUTF_8(p_out, p_in, strlen(p_in));

		memset(client_recv[1].element_info, '\0', MAX_ELEMENT_SIZE);
		memcpy(client_recv[1].element_info, p_out, _MIN(strlen(p_out), MAX_ELEMENT_SIZE));
	}
	if (NULL != p_session) {
		memset(p_in, '\0', MAX_ELEMENT_SIZE);
		memset(p_out, '\0', MAX_ELEMENT_SIZE);
		memcpy(p_in, p_session, _MIN(strlen(p_session), MAX_ELEMENT_SIZE));
		GB2312ToUTF_8(p_out, p_in, strlen(p_in));

		memset(client_recv[2].element_info, '\0', MAX_ELEMENT_SIZE);
		memcpy(client_recv[2].element_info, p_out, _MIN(strlen(p_out), MAX_ELEMENT_SIZE));
	}
	memcpy(p_regist, client_recv, element_size * sizeof(XmlElement));
	return p_regist;
}

// 接入服务器显示内容
XmlElement *ProcessXML::RegAccessShowContentM()
{
	const int element_size = 11;
	XmlElement *p_regist = (XmlElement *)malloc(element_size * sizeof(XmlElement));
	if (NULL == p_regist) {
		return p_regist;
	}
	XmlElement one_devices_info[] = { 
		{XML_ELEMENT,	"Device",			{ "" },		{ "" },		"",		{ "DevicesInfo" }},
		{XML_ELEMENT,	"DeviceID",			{ "" },		{ "" },		"",		{ "DevicesInfo", "Device" }},
		{XML_ELEMENT,	"DeviceName",		{ "" },		{ "" },		"",		{ "DevicesInfo", "Device" }},
		{XML_ELEMENT,	"DeviceIP",			{ "" },		{ "" },		"",		{ "DevicesInfo", "Device" }},
		{XML_ELEMENT,	"MsgPort",			{ "" },		{ "" },		"",		{ "DevicesInfo", "Device" }},
		{XML_ELEMENT,	"VideoPort",		{ "" },		{ "" },		"",		{ "DevicesInfo", "Device" }},
		{XML_ELEMENT,	"AudioPort",		{ "" },		{ "" },		"",		{ "DevicesInfo", "Device" }},
		{XML_ELEMENT,	"LocalSave",		{ "" },		{ "" },		"",		{ "DevicesInfo", "Device" }},
		{XML_ELEMENT,	"SupportCloud",		{ "" },		{ "" },		"",		{ "DevicesInfo", "Device" }},
		{XML_ELEMENT,	"ConnectStatus",	{ "" },		{ "" },		"",		{ "DevicesInfo", "Device" }},

		{XML_EOF}
	};

	memcpy(p_regist, one_devices_info, element_size * sizeof(XmlElement));
	return p_regist;
}

// 分发服务器->接入服务器(保持连接)
XmlElement *ProcessXML::RegAssign2AccessKeepConnectM(const char *p_error, const char *p_session,
	const char *p_status_type, const char *p_status_time)
{
	char p_in[MAX_ELEMENT_SIZE] = { '\0' };
	char p_out[MAX_ELEMENT_SIZE] = { '\0' };
	const int element_size = 6;

	XmlElement *p_regist = (XmlElement *)malloc(element_size * sizeof(XmlElement));
	if (NULL == p_regist) {
		return p_regist;
	}

	XmlElement assign_access[] = { 
		{XML_ROOT,			"Message" },
		{XML_ELEMENT,		"Error",		{ "" }, { "" },	"",			{ "Message" }},
		{XML_ELEMENT,		"Session",		{ "" },	{ "" },	"",			{ "Message" }},
		{XML_ELEMENT,		"Status_type",	{ "" }, { "" }, "",			{ "Message" }},
		{XML_ELEMENT,		"Status_time",	{ "" }, { "" }, "",			{ "Message" }},
		{XML_EOF}
	};

	if (NULL != p_error) {
		memset(p_in, '\0', MAX_ELEMENT_SIZE);
		memset(p_out, '\0', MAX_ELEMENT_SIZE);
		memcpy(p_in, p_error, _MIN(strlen(p_error), MAX_ELEMENT_SIZE));
		GB2312ToUTF_8(p_out, p_in, strlen(p_in));

		memset(assign_access[1].element_info, '\0', MAX_ELEMENT_SIZE);
		memcpy(assign_access[1].element_info, p_out, _MIN(strlen(p_out), MAX_ELEMENT_SIZE));
	}
	if (NULL != p_session) {
		memset(p_in, '\0', MAX_ELEMENT_SIZE);
		memset(p_out, '\0', MAX_ELEMENT_SIZE);
		memcpy(p_in, p_session, _MIN(strlen(p_session), MAX_ELEMENT_SIZE));
		GB2312ToUTF_8(p_out, p_in, strlen(p_in));

		memset(assign_access[2].element_info, '\0', MAX_ELEMENT_SIZE);
		memcpy(assign_access[2].element_info, p_out, _MIN(strlen(p_out), MAX_ELEMENT_SIZE));
	}
	if (NULL != p_status_type) {
		memset(p_in, '\0', MAX_ELEMENT_SIZE);
		memset(p_out, '\0', MAX_ELEMENT_SIZE);
		memcpy(p_in, p_status_type, _MIN(strlen(p_status_type), MAX_ELEMENT_SIZE));
		GB2312ToUTF_8(p_out, p_in, strlen(p_in));

		memset(assign_access[3].element_info, '\0', MAX_ELEMENT_SIZE);
		memcpy(assign_access[3].element_info, p_out, _MIN(strlen(p_out), MAX_ELEMENT_SIZE));
	}
	if (NULL != p_status_time) {
		memset(p_in, '\0', MAX_ELEMENT_SIZE);
		memset(p_out, '\0', MAX_ELEMENT_SIZE);
		memcpy(p_in, p_status_time, _MIN(strlen(p_status_time), MAX_ELEMENT_SIZE));
		GB2312ToUTF_8(p_out, p_in, strlen(p_in));

		memset(assign_access[4].element_info, '\0', MAX_ELEMENT_SIZE);
		memcpy(assign_access[4].element_info, p_out, _MIN(strlen(p_out), MAX_ELEMENT_SIZE));
	}

	memcpy(p_regist, assign_access, element_size * sizeof(XmlElement));
	return p_regist;
}

// 分发服务器->接入服务器(停止发送视频)
XmlElement *ProcessXML::RegAssign2AccessSendVideoM(const char *p_session, const char *p_video_req)
{
	char p_in[MAX_ELEMENT_SIZE] = { '\0' };
	char p_out[MAX_ELEMENT_SIZE] = { '\0' };
	const int element_size = 4;

	XmlElement *p_regist = (XmlElement *)malloc(element_size * sizeof(XmlElement));
	if (NULL == p_regist) {
		return p_regist;
	}

	XmlElement assign_access[] = { 
		{XML_ROOT,			"Message" },
		{XML_ELEMENT,		"Session",			{ "" },	{ "" },	"",			{ "Message" }},
		{XML_ELEMENT,		"Video_request",	{ "" }, { "" }, "",			{ "Message" }},
		{XML_EOF}
	};

	if (NULL != p_session) {
		memset(p_in, '\0', MAX_ELEMENT_SIZE);
		memset(p_out, '\0', MAX_ELEMENT_SIZE);
		memcpy(p_in, p_session, _MIN(strlen(p_session), MAX_ELEMENT_SIZE));
		GB2312ToUTF_8(p_out, p_in, strlen(p_in));

		memset(assign_access[1].element_info, '\0', MAX_ELEMENT_SIZE);
		memcpy(assign_access[1].element_info, p_out, _MIN(strlen(p_out), MAX_ELEMENT_SIZE));
	}
	if (NULL != p_video_req) {
		memset(p_in, '\0', MAX_ELEMENT_SIZE);
		memset(p_out, '\0', MAX_ELEMENT_SIZE);
		memcpy(p_in, p_video_req, _MIN(strlen(p_video_req), MAX_ELEMENT_SIZE));
		GB2312ToUTF_8(p_out, p_in, strlen(p_in));

		memset(assign_access[2].element_info, '\0', MAX_ELEMENT_SIZE);
		memcpy(assign_access[2].element_info, p_out, _MIN(strlen(p_out), MAX_ELEMENT_SIZE));
	}

	memcpy(p_regist, assign_access, element_size * sizeof(XmlElement));
	return p_regist;
}