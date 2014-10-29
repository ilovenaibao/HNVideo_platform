
// ScanDeviceDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "./DeviceManager/sal.h"
#include "./DeviceManager/DeviceInfo.h"
#include <vector>

using namespace std;


typedef struct _MsgHead 
{
	UINT32 length;
	UINT32 seq;
	UINT32 reserved;
}MsgHead;


// CScanDeviceDlg dialog
class CScanDeviceDlg : public CDialogEx
{
// Construction
public:
	CScanDeviceDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SCANDEVICE_DIALOG };

	enum { SearchButtonEnableTimerId = 1000 }; 
	enum { SearchTimeOut = 10 };

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
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonSearch();

	void OnTimer (UINT_PTR nIDEvent);
	bool StartListening();
	bool ParseData(CString sInfo, CString sIP);
	bool Broadcast( const char* sRemoteIP="255.255.255.255", UINT16 nRemotePort=7210, 
		const char* sBuffer="BROADCAST\r\n" );


public:
	CListCtrl m_DeviceList;

	CButton m_cSearchButton;
	CString m_SearchButtonOldText;
	UINT32	m_nFreeTime;
	UINT32	m_nSearchTime;
	bool    m_bSearching;
	UINT32  m_nIndex;

private:
	CWinThread* m_pThread;
	bool m_bInitaled;
	CritSec     m_CritSec;
	
	vector<CDeviceInfo> m_cDeviceInfo;
	vector<CString>		m_sIpList;
	
};
