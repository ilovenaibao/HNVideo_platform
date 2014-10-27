
// HNClientRenderingModDlg.h : header file
//

#pragma once

#include "../../../video_platform/HNCommon/process_xml/HNProtocol.h"
#include "../../../video_platform/HNCommon/process_xml/ProcessXML.h"
#include "../src/test/Client.h"
#include "../src/RenderingMod/RenderingMod.h"

// CHNClientRenderingModDlg dialog
class CHNClientRenderingModDlg : public CDialogEx
{
// Construction
public:
	CHNClientRenderingModDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_HNCLIENTRENDERINGMOD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTaskComplete(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	void TestSendVideoRequest();

public:

	// 使用的各种句柄
	HWND            hContainer;                    // 容器窗口句柄
	HWND            hDisplay;                    // 绘制视频句柄

	// 模块实例
	CRenderingMod    renderingMod;                // 视频帧绘制模块

	ProcessXML process_xml;
	char p_xml_buf[MAX_OUTPUT_BUFFER_];
	SendMegHead send_data;
	CClientNet client;
	HANDLE hDataRcvThread;

	void* processModule;						// 指向数据处理模块

	bool is_connect_server;

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
