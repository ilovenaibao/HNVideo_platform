
// AccessServerDlg.h : header file
//

#pragma once


// CAccessServerDlg dialog
class CAccessServerDlg : public CDialogEx
{
// Construction
public:
	CAccessServerDlg(CWnd* pParent = NULL);	// standard constructor
	void InitListColumnHeader();
	void InsertItems();
	void SetCell(HWND hWnd1, CString value, int nRow, int nCol);
	CString GetItemText(HWND hWnd, int nItem, int nSubItem) const;

// Dialog Data
	enum { IDD = IDD_ACCESSSERVER_DIALOG };

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
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonRefresh();

public:
	// device info
	enum {
		LIST_COLUMN_NUMBER = 0,			// 编号
		LIST_COLUMN_DEVICE_ID,			// 设备ID
		LIST_COLUMN_DEVICE_NAME,		// 设备名称
		LIST_COLUMN_DEVICE_IP,			// 设备IP
		LIST_COLUMN_MSG_PORT,			// 消息端口
		LIST_COLUMN_VIDEO_PORT,			// 视频端口
		LIST_COLUMN_AUDIO_PORT,			// 音频端口
		LIST_COLUMN_LOCAL_SAVE,			// 本地存储
		LIST_COLUMN_SUPPORT_CLOUD,		// 是否支持云台
		LIST_COLUMN_CONNECT_STATUS,		// 连接状态

	};

	int nItem, nSubItem;
	CEvent m_read_file_event;
	
private:
	

};
