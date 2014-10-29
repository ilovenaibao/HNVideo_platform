
// HNTestRendingModDlg.h : header file
//

#pragma once


#include "../src/RenderingMod/RenderingMod.h"

// CHNTestRendingModDlg dialog
class CHNTestRendingModDlg : public CDialogEx
{
// Construction
public:
	CHNTestRendingModDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_HNTESTRENDINGMOD_DIALOG };

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
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

public:
	void TestLinkCollectPoint(CHNTestRendingModDlg *p);

public:

	// 使用的各种句柄
	HWND            hContainer;                    // 容器窗口句柄
	HWND            hDisplay;                    // 绘制视频句柄

	CConnectServerStruct connectStruct;
	CNetTrans netTransMod;

	// 模块实例
	CRenderingMod    renderingMod;                // 视频帧绘制模块


};

