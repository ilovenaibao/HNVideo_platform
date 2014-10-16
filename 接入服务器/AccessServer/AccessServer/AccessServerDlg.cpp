
// AccessServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AccessServer.h"
#include "AccessServerDlg.h"
#include "afxdialogex.h"
#include "ProcessXML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern XmlElement devices_info[];

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAccessServerDlg dialog




CAccessServerDlg::CAccessServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAccessServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	::ResetEvent(m_read_file_event);	
}

void CAccessServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAccessServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CAccessServerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAccessServerDlg::OnBnClickedCancel)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CAccessServerDlg::OnNMClickList)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CAccessServerDlg::OnBnClickedButtonRefresh)
END_MESSAGE_MAP()


// CAccessServerDlg message handlers

BOOL CAccessServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//Set the style to listControl
	ListView_SetExtendedListViewStyle(::GetDlgItem(m_hWnd,IDC_LIST1),LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES); 
	InitListColumnHeader();
	InsertItems();
	//::ShowWindow(::GetDlgItem(m_hWnd,IDC_EDIT1),SW_HIDE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAccessServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAccessServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAccessServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAccessServerDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	::SetEvent(m_read_file_event);
	//CDialogEx::OnOK();
}


void CAccessServerDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}

void CAccessServerDlg::InitListColumnHeader()
{
	HWND hWnd = ::GetDlgItem(m_hWnd, IDC_LIST1);
	if (NULL == hWnd) {
		AfxMessageBox(_T("error: InitListColumnHeader"));
		return ;
	}

	// Set the LVCOLUMN structure with the required column information
	LVCOLUMN list;
	list.mask =  LVCF_TEXT |LVCF_WIDTH| LVCF_FMT |LVCF_SUBITEM;
	list.fmt = LVCFMT_LEFT;
	list.cx = 50;
	list.pszText   = _T("编号");
	list.iSubItem = 0;
	//Inserts the column
	::SendMessage(hWnd,LVM_INSERTCOLUMN, (WPARAM)list.iSubItem,(WPARAM)&list);

	list.cx = 100;
	list.pszText   = _T("设备ID");
	++list.iSubItem;
	::SendMessage(hWnd,LVM_INSERTCOLUMN, (WPARAM)list.iSubItem,(WPARAM)&list);

	list.cx = 100;
	list.pszText   = _T("设备名称");
	++list.iSubItem;
	::SendMessage(hWnd,LVM_INSERTCOLUMN, (WPARAM)list.iSubItem,(WPARAM)&list);

	list.cx = 100;
	list.pszText   = _T("设备IP");
	++list.iSubItem;
	::SendMessage(hWnd,LVM_INSERTCOLUMN, (WPARAM)list.iSubItem,(WPARAM)&list);

	list.cx = 100;
	list.pszText   = _T("消息端口");
	++list.iSubItem;
	::SendMessage(hWnd,LVM_INSERTCOLUMN, (WPARAM)list.iSubItem,(WPARAM)&list);

	list.cx = 100;
	list.pszText   = _T("视频端口");
	++list.iSubItem;
	::SendMessage(hWnd,LVM_INSERTCOLUMN, (WPARAM)list.iSubItem,(WPARAM)&list);

	list.cx = 100;
	list.pszText   = _T("音频端口");
	++list.iSubItem;
	::SendMessage(hWnd,LVM_INSERTCOLUMN, (WPARAM)list.iSubItem,(WPARAM)&list);

	list.cx = 100;
	list.pszText   = _T("本地存储");
	++list.iSubItem;
	::SendMessage(hWnd,LVM_INSERTCOLUMN, (WPARAM)list.iSubItem,(WPARAM)&list);

	list.cx = 100;
	list.pszText   = _T("是否支持云台");
	++list.iSubItem;
	::SendMessage(hWnd,LVM_INSERTCOLUMN, (WPARAM)list.iSubItem,(WPARAM)&list);

	list.cx = 100;
	list.pszText   = _T("连接状态");
	++list.iSubItem;
	::SendMessage(hWnd,LVM_INSERTCOLUMN, (WPARAM)list.iSubItem,(WPARAM)&list);
}

// This function inserts the default values into the listControl
void CAccessServerDlg::InsertItems()
{
	HWND hWnd = ::GetDlgItem(m_hWnd, IDC_LIST1);
	if (NULL == hWnd) {
		AfxMessageBox(_T("error: InsertItems"));
		return ;
	}

	// load xml file in buf
	ProcessXML process_xml;
	char xml_buf[MAX_OUTPUT_BUFFER_] = { '\0' };
	FILE *pf = NULL;
	int file_len = 0;
	errno_t err = _wfopen_s(&pf, _T("devices_info.xml"), _T("r+b"));
	if (0 != err && NULL != pf) {
		AfxMessageBox(_T("error: InsertItems, load xml file"));
		return ;
	}
	fseek(pf, 0, SEEK_END);
	file_len = ftell(pf);
	fseek(pf, 0, SEEK_SET);
	fread(xml_buf, min(MAX_OUTPUT_BUFFER_, file_len), 1, pf);
	fclose(pf);
	pf = NULL;

	int device_count = 0;
	int row = 0;
	while (TRUE) {
		if (row >= MAX_OUTPUT_BUFFER_) {
			break;
		}
		CString tmp_str = _T("");
		char count_num[MAX_OUTPUT_BUFFER_] = { '\0' };
		char str_code_convert[MAX_ELEMENT_SIZE] = { '\0' };

		_itoa_s(row + 1, count_num, 10);
		XmlElement *one_devices_info = process_xml.RegAccessShowContentM();
		if (NULL == one_devices_info) {
			AfxMessageBox(_T("error: RegAccessShowContent"));
			return ;
		}
		strcat_s(one_devices_info[LIST_COLUMN_NUMBER].element_key, count_num);
		strcat_s(one_devices_info[LIST_COLUMN_DEVICE_ID].parent_key[1], count_num);
		strcat_s(one_devices_info[LIST_COLUMN_DEVICE_NAME].parent_key[1], count_num);
		strcat_s(one_devices_info[LIST_COLUMN_DEVICE_IP].parent_key[1], count_num);
		strcat_s(one_devices_info[LIST_COLUMN_MSG_PORT].parent_key[1], count_num);
		strcat_s(one_devices_info[LIST_COLUMN_VIDEO_PORT].parent_key[1], count_num);
		strcat_s(one_devices_info[LIST_COLUMN_AUDIO_PORT].parent_key[1], count_num);
		strcat_s(one_devices_info[LIST_COLUMN_LOCAL_SAVE].parent_key[1], count_num);
		strcat_s(one_devices_info[LIST_COLUMN_SUPPORT_CLOUD].parent_key[1], count_num);
		strcat_s(one_devices_info[LIST_COLUMN_CONNECT_STATUS].parent_key[1], count_num);

		int flag = process_xml.GetElementInXmlBuffer(one_devices_info, xml_buf);
		if (0 == flag || XML_EOF == one_devices_info[LIST_COLUMN_DEVICE_ID].element_deep) {
			break;
		}
		tmp_str.Format(_T("%d"), row + 1);
		SetCell(hWnd, tmp_str, row, LIST_COLUMN_NUMBER); // 编号

		// 设备ID
		memset(str_code_convert, '\0', MAX_ELEMENT_SIZE);
		UTF_8ToGB2312(str_code_convert, one_devices_info[LIST_COLUMN_DEVICE_ID].element_info, 
			strlen(one_devices_info[LIST_COLUMN_DEVICE_ID].element_info));
		tmp_str = str_code_convert;
		SetCell(hWnd, tmp_str, row, LIST_COLUMN_DEVICE_ID);

		// 设备名称
		memset(str_code_convert, '\0', MAX_ELEMENT_SIZE);
		UTF_8ToGB2312(str_code_convert, one_devices_info[LIST_COLUMN_DEVICE_NAME].element_info, 
			strlen(one_devices_info[LIST_COLUMN_DEVICE_NAME].element_info));
		tmp_str = str_code_convert;
		SetCell(hWnd, tmp_str, row, LIST_COLUMN_DEVICE_NAME);

		// 设备IP
		memset(str_code_convert, '\0', MAX_ELEMENT_SIZE);
		UTF_8ToGB2312(str_code_convert, one_devices_info[LIST_COLUMN_DEVICE_IP].element_info, 
			strlen(one_devices_info[LIST_COLUMN_DEVICE_IP].element_info));
		tmp_str = str_code_convert;
		SetCell(hWnd, tmp_str, row, LIST_COLUMN_DEVICE_IP);

		// 消息端口
		memset(str_code_convert, '\0', MAX_ELEMENT_SIZE);
		UTF_8ToGB2312(str_code_convert, one_devices_info[LIST_COLUMN_MSG_PORT].element_info, 
			strlen(one_devices_info[LIST_COLUMN_MSG_PORT].element_info));
		tmp_str = str_code_convert;
		SetCell(hWnd, tmp_str, row, LIST_COLUMN_MSG_PORT);

		// 视频端口
		memset(str_code_convert, '\0', MAX_ELEMENT_SIZE);
		UTF_8ToGB2312(str_code_convert, one_devices_info[LIST_COLUMN_VIDEO_PORT].element_info, 
			strlen(one_devices_info[LIST_COLUMN_VIDEO_PORT].element_info));
		tmp_str = str_code_convert;
		SetCell(hWnd, tmp_str, row, LIST_COLUMN_VIDEO_PORT);

		// 音频端口
		memset(str_code_convert, '\0', MAX_ELEMENT_SIZE);
		UTF_8ToGB2312(str_code_convert, one_devices_info[LIST_COLUMN_AUDIO_PORT].element_info, 
			strlen(one_devices_info[LIST_COLUMN_AUDIO_PORT].element_info));
		tmp_str = str_code_convert;
		SetCell(hWnd, tmp_str, row, LIST_COLUMN_AUDIO_PORT);

		// 本地存储
		memset(str_code_convert, '\0', MAX_ELEMENT_SIZE);
		UTF_8ToGB2312(str_code_convert, one_devices_info[LIST_COLUMN_LOCAL_SAVE].element_info, 
			strlen(one_devices_info[LIST_COLUMN_LOCAL_SAVE].element_info));
		tmp_str = str_code_convert;
		SetCell(hWnd, tmp_str, row, LIST_COLUMN_LOCAL_SAVE);

		// 是否支持云台
		memset(str_code_convert, '\0', MAX_ELEMENT_SIZE);
		UTF_8ToGB2312(str_code_convert, one_devices_info[LIST_COLUMN_SUPPORT_CLOUD].element_info, 
			strlen(one_devices_info[LIST_COLUMN_SUPPORT_CLOUD].element_info));
		tmp_str = str_code_convert;
		SetCell(hWnd, tmp_str, row, LIST_COLUMN_SUPPORT_CLOUD);

		// 连接状态
		memset(str_code_convert, '\0', MAX_ELEMENT_SIZE);
		UTF_8ToGB2312(str_code_convert, one_devices_info[LIST_COLUMN_CONNECT_STATUS].element_info, 
			strlen(one_devices_info[LIST_COLUMN_CONNECT_STATUS].element_info));
		tmp_str = str_code_convert;
		SetCell(hWnd, tmp_str, row, LIST_COLUMN_CONNECT_STATUS);

		process_xml.FreeXmlElement(one_devices_info);

		row++;
	}
}

// This function set the text in the specified SubItem depending on the Row and Column values
void CAccessServerDlg::SetCell(HWND hWnd1, CString value, int nRow, int nCol)
{
	TCHAR     szString [256];
	wsprintf(szString,value ,0);

	//Fill the LVITEM structure with the values given as parameters.
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nRow;
	lvItem.pszText = szString;
	lvItem.iSubItem = nCol;
	if(nCol >0)
		//set the value of listItem
		::SendMessage(hWnd1,LVM_SETITEM, (WPARAM)0,(WPARAM)&lvItem);
	else
		//Insert the value into List
		ListView_InsertItem(hWnd1,&lvItem);

}

//this function will returns the item text depending on the item and SubItem Index
CString CAccessServerDlg::GetItemText(HWND hWnd, int nItem, int nSubItem) const
{
	LVITEM lvi;
	memset(&lvi, 0, sizeof(LVITEM));
	lvi.iSubItem = nSubItem;
	CString str;
	int nLen = 128;
	int nRes;
	do
	{
		nLen *= 2;
		lvi.cchTextMax = nLen;
		lvi.pszText = str.GetBufferSetLength(nLen);
		nRes  = (int)::SendMessage(hWnd, LVM_GETITEMTEXT, (WPARAM)nItem,
			(LPARAM)&lvi);
	} while (nRes == nLen-1);
	str.ReleaseBuffer();
	return str;
}

void CAccessServerDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CAccessServerDlg::OnBnClickedButtonRefresh()
{
	// TODO: Add your control notification handler code here
	HWND hWnd = ::GetDlgItem(m_hWnd, IDC_LIST1);
	if (NULL == hWnd) {
		AfxMessageBox(_T("error: OnBnClickedButtonRefresh"));
		return ;
	}
	::SendMessage(hWnd, LVM_DELETEALLITEMS, 0, 0);
	InsertItems();
}
