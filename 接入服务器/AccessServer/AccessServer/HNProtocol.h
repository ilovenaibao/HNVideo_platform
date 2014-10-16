/**************************************************************************
/*
* Copyright (c) 2014
* All rights reserved.
*
* File Name： ALANThrea.h
* Summary：   音视频基础平台协议基础类
* 
* Current version： 1.0
* Author：Alan.zhang(张建中)
* Start： 2014/09/30/
* End：   2014/09/30/
*
* Revise version：  1.0
* Author：Alan.zhang(张建中)
* Start： 2014/09/30/
* End：   2014/09/30/

* Introduction:
* Base Lock Class
* Base Thread Class
*
**************************************************************************/


#ifndef _HNPROTOCOL_H
#define _HNPROTOCOL_H

#include <scew.h>
#include "HNCommon.h"

enum { MAX_OUTPUT_BUFFER_ = 2000 };

enum {
	XML_EOF = -1,
	XML_ROOT = 0,
	XML_ELEMENT,
	XML_SUB_ELEMENT,
};

#define MAX_ELEMENT_SIZE			200
#define MAX_ELEMENT_ATTRIB_SIZE		10

typedef struct _XmlElement {
	int  element_deep;              // 数组结束标志，如果-1则表示结束
	char element_key[MAX_ELEMENT_SIZE];             // key标志
	char key_attrib[MAX_ELEMENT_ATTRIB_SIZE][MAX_ELEMENT_SIZE];         // key的属性
	char key_attrib_content[MAX_ELEMENT_ATTRIB_SIZE][MAX_ELEMENT_SIZE]; // key属性内容
	char element_info[MAX_ELEMENT_SIZE];            // content内容
	char parent_key[MAX_ELEMENT_ATTRIB_SIZE][MAX_ELEMENT_SIZE];         // 父类节点集合
	char *cur_parent_key;          // 当前父节点位置
	int  cur_parent_key_index;     // 当前父节点计数


	void Initialize(void)
	{
		element_deep = 0;
		memset(element_key, '\0', MAX_ELEMENT_SIZE);
		for (int i = 0; i < MAX_ELEMENT_ATTRIB_SIZE; i++) {
			memset(key_attrib, '\0', MAX_ELEMENT_SIZE);
			memset(key_attrib_content, '\0', MAX_ELEMENT_SIZE);
			memset(parent_key, '\0', MAX_ELEMENT_SIZE);
		}
		memset(element_info, '\0', MAX_ELEMENT_SIZE);
		cur_parent_key = parent_key[0];
	}

} XmlElement;


enum {
	CMD_CODE_CREATE_CONNECT	= 2004,							// 建立连接
	CMD_CODE_ASSIGN_SERVER_2_CLIENT_VIDEO_REQ,				// 分发服务器->客户端发送视频请求结果
	CMD_CODE_ASSIGN_SERVER_2_ACCESS_SERVER_KEEP_CONNECT,	// 分发服务器->接入服务器发送保持连接
	CMD_CODE_ASSIGN_SERVER_2_ACCESS_SERVER_STOP_VIDEO,		// 分发服务器->接入服务器发送停止接收视频
};


// 发送data
typedef struct _SendMegHead {
	_SendMegHead(void)
	{
		data_len = 0;
		version = 100;
		cmd_code = 0;
		xml_len = 0;
		source_id = 0;
		dest_id = 0;
	}

	int data_len; // 数据长度
	int version; // 版本号
	int cmd_code; // 指令编号
	int xml_len; // xml长度
	int source_id; // 源业务ID
	int dest_id; // 目的业务ID

} SendMegHead,*PSendMegHead;

typedef struct _SendMeg{
	_SendMeg(void)
	{
		version = 100;
		cmd_code = 0;
		xml_len = 0;
		source_id = 0;
		dest_id = 0;
	}

	int version; // 版本号
	int cmd_code; // 指令编号
	int xml_len; // xml长度
	int source_id; // 源业务ID
	int dest_id; // 目的业务ID

} SendMeg,*PSendMeg;


// 接收data
typedef struct _RecvMegHead {
	_RecvMegHead(void) {
		data_len = 0;
		version = 0;
		cmd_code = 0;
		xml_len = 0;
		success_flag = 0;
	}

	int data_len; // 数据长度
	int version; // 版本号
	int cmd_code; // 指令编号
	int xml_len; // xml长度
	int success_flag; // 成功标志
} RecvMegHead, *PRecvMegHead;


class HNProtocol
{
public:
	HNProtocol();
	~HNProtocol();

public:	
	virtual void SendHeartBeat(int version) = 0;
	virtual void GetHeartBeat(int version)  = 0;
	virtual int  SendVideoReq(int version)  = 0;


public:
	void CreateXmlBuffer(_OUT XML_Char *p_out, _IN XmlElement *all_elements);
	XML_Char* CreateXmlBuffer(_IN XmlElement *all_elements);
	XML_Char* AddElementInXmlBuffer(XML_Char *buffer_in, XmlElement *add_element, int add_kind = SET_ELEMENT);
	int GetElementInXmlBuffer(_OUT XmlElement *in_element, _IN XML_Char *buffer_in);
	void PrintXmlBuffer(_IN XML_Char *xml_buffer);


private:
	int AddFrameInfoToXml(_IN scew_tree *xml_tree, _IN XmlElement *pelement);
	int AddXmlElement_(_IN scew_element *element, _IN XmlElement *add_element, _IN int add_kind);
	int AddXmlElement_2(_IN scew_element *element, _IN XmlElement *add_element, _IN int add_kind);
	int GetXmlElement_(_IN scew_element *element, _IN XmlElement *in_element);
	void GetXmlElement_2(_IN scew_element *element, _IN XmlElement *out_element);
	void SaveXmlIntoBuffer(_OUT XML_Char *xml_buffer, _IN scew_tree *m_xml_tree, _IN unsigned int buf_size = MAX_OUTPUT_BUFFER_);
	XML_Char* SaveXmlIntoBuffer(_IN scew_tree *m_xml_tree, _IN int buf_size = MAX_OUTPUT_BUFFER_);
	XML_Char* save_xml_into_buffer(_IN scew_tree *xml_tree, _IN int buf_size);

public:
	enum {
		SET_ELEMENT		= 0x00000001,	// 设置element相关content
		ADD_ELEMENT		= 0x00000010,	// 添加element
		ALL_OF_KIND		= 0x00000011	// 添加element并设置相关content
	};

};
#endif