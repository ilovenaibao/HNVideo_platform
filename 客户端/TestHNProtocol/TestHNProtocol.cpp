//// TestHNProtocol.cpp : 定义控制台应用程序的入口点。
////
//
//#include "stdafx.h"
//
//
//int _tmain(int argc, _TCHAR* argv[])
//{
//	return 0;
//}
//


/**
 * @file     scew_write.c
 * @brief    SCEW usage example
 * @author   Aleix Conchillo Flaque <aleix@member.fsf.org>
 * @date     Sun Mar 30, 2003 12:21
 *
 * @if copyright
 *
 * Copyright (C) 2002-2009 Aleix Conchillo Flaque
 *
 * SCEW is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SCEW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 * @endif
 *
 * This example shows the usage of the SCEW API. It will create a new
 * XML and write it to a file.
 *
 * We will create an XML with the follwing structure:
 *
 *   <scew_test>
 *     <element>element contents</element>
 *     <element attribute="value"/>
 *     <element attribute1="value1" attribute2="value2"/>
 *     <element>
 *       <sub_element attribute="value"/>
 *       <sub_element attribute1="value1" attribute2="value2">
 *         <sub_sub_element attribute1="value1"
 *                          attribute2="new_value2"
 *                          attribute3="value3">
 *           With accents: à é è í ó ú
 *         </sub_sub_element>
 *       </sub_element>
 *     </element>
 *   </scew_test>
 */


#include "./process_xml/ProcessXML.h"
#include "./test/Client.h"
#include "./test/Server.h"
#include "./leak_detector/leak_detector_c.h"
#include <cstdlib>
#include <ctime>

//extern XmlElement client_send_video_req[];
//extern XmlElement client_recv[];

#if defined(_MSC_VER) && defined(XML_UNICODE_WCHAR_T)
#include <fcntl.h>
#include <io.h>
#endif /* _MSC_VER && XML_UNICODE_WCHAR_T */

#include <stdio.h>

//enum { MAX_OUTPUT_BUFFER_ = 2000 };

//得到一帧数据的长度
int get_frame_length(const BYTE *buf, int buf_size)
{
    int i;
    unsigned int state=0;
    int frame_start_found=0;
    for(i=0; i<=buf_size; i++){
        if(state==0x00000001){
            if(frame_start_found)
			{
                if (i >= buf_size) 
					break;
				state=-1; 
				return i-4;
            }
			//* frame_start=i-4;
            frame_start_found = 1;
        }
        if (i<buf_size)
            state= (state<<8) | buf[i];
    }
    return -1;
}

// 测试客户端->分发服务器发送请求视频
void TestSendVideoRequest()
{	
	ProcessXML process_xml;
	static unsigned int count = 10000;
	printf("count = %d\n");
	XML_Char p_xml_buf[MAX_OUTPUT_BUFFER_] = { '\0' };

	/*while (TRUE) {
		XmlElement *client_send_video_req = process_xml.RegConnectM("session", "1", "video name", "顾纯辉");
		XmlElement *client_send_video_req1 = process_xml.RegConnectM("", "", "", "");
		process_xml.CreateXmlBuffer(p_xml_buf, client_send_video_req);
		process_xml.GetElementInXmlBuffer(client_send_video_req1, p_xml_buf);
		process_xml.PrintXmlBuffer(p_xml_buf);

		process_xml.FreeXmlElement(client_send_video_req1);
		process_xml.FreeXmlElement(client_send_video_req);
		CHECK_MEME_LEAK
	}*/

	XmlElement *xml_frame = process_xml.RegConnectM("586","1","采集点7","客户端_zhang");
	process_xml.CreateXmlBuffer(p_xml_buf, xml_frame);
	
	SendMegHead send_data;
	send_data.xml_len = strlen(p_xml_buf);
	send_data.version = 100;
	send_data.cmd_code = CMD_CODE_CREATE_CONNECT;
	send_data.source_id = count++;
	send_data.dest_id = 7;
	send_data.data_len = sizeof(SendMegHead) + send_data.xml_len;

	process_xml.PrintXmlBuffer(p_xml_buf);
	
	client_main((char *)&send_data, sizeof(SendMegHead), p_xml_buf, NULL);

	process_xml.FreeXmlElement(xml_frame);
}

// 测试分发服务器->客户端发送视频请求的结果
void TestRecvVideoRequest()
{
	ProcessXML process_xml;
	XML_Char p_xml_buffer[MAX_OUTPUT_BUFFER_] = { '\0' };

	process_xml.CreateXmlBuffer(p_xml_buffer, process_xml.RegConnectM("session", "2"));
	//process_xml.CreateXmlBuffer(p_xml_buffer, client_recv);
	RecvMegHead send_data;
	send_data.version = 100;
	send_data.cmd_code = CMD_CODE_ASSIGN_SERVER_2_CLIENT_VIDEO_REQ;
	send_data.success_flag = 0;
	server_main((char*)&send_data, sizeof(RecvMegHead), p_xml_buffer, NULL);

	//// 接收使用
	//ReceiveVideoReqData request_data = process_xml.GetRecvVideoRequest(NULL);
	//request_data.Release();
}

// 测试分发服务器->接入服务器发送保持连接&停止接收视频请求
void TestAssignServer2AccessServerKeepConnectAndStopVideo()
{
	ProcessXML process_xml;
	XML_Char p_xml_buffer[MAX_OUTPUT_BUFFER_] = { '\0' };

	XmlElement *client_recv = process_xml.RegAssign2AccessKeepConnectM("-1", "session",
		"bclj", "sjyz");

	process_xml.CreateXmlBuffer(p_xml_buffer, client_recv);
	RecvMegHead send_data;
	send_data.xml_len = strlen(p_xml_buffer);
	send_data.version = 100;
	send_data.cmd_code = CMD_CODE_ASSIGN_SERVER_2_ACCESS_SERVER_KEEP_CONNECT;
	send_data.success_flag = 0;
	send_data.data_len = send_data.xml_len + sizeof(RecvMegHead);

	server_main((char*)&send_data, sizeof(RecvMegHead), p_xml_buffer, NULL);

	process_xml.FreeXmlElement(client_recv);
}

int
main(int argc, char *argv[])
{
	/*TestAssignServer2AccessServerKeepConnectAndStopVideo();
	CHECK_MEME_LEAK
	return 0;*/


	/*TestRecvVideoRequest();
	return 0;*/


	//TestSendVideoRequest();
	
LOOP:
	TestSendVideoRequest();
	CHECK_MEME_LEAK
	Sleep(10);
	goto LOOP;

	return 0;
}