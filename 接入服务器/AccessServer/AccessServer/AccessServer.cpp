
// AccessServer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AccessServer.h"
#include "AccessServerDlg.h"
#include "ProcessXML.h"
#include "../leak_detector/leak_detector_c.h"
#include "./test/Client.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAccessServerApp

BEGIN_MESSAGE_MAP(CAccessServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAccessServerApp construction

CAccessServerApp::CAccessServerApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// 连接分发服务器的线程
DWORD WINAPI ConnectAssignServerThreadProc(void* lpParameter)
{
	//CAccessServerDlg *in = (CAccessServerDlg*)lpParameter;//获得参数的地址
	//MessageBox(NULL,TEXT("adf"),NULL,MB_OK);
	ProcessXML process_xml;
	XML_Char p_xml_buf[MAX_OUTPUT_BUFFER_] = { '\0' };

	XmlElement *xml_frame = process_xml.RegConnectM("session", "2");
	process_xml.CreateXmlBuffer(p_xml_buf, xml_frame);

	SendMegHead send_data;
	send_data.xml_len = strlen(p_xml_buf);
	send_data.version = 100;
	send_data.cmd_code = CMD_CODE_CREATE_CONNECT;
	send_data.source_id = 10001;
	send_data.dest_id = 7;
	send_data.data_len = sizeof(SendMegHead) + send_data.xml_len;

	client_main((char *)&send_data, sizeof(SendMegHead), p_xml_buf, NULL, lpParameter);

	process_xml.FreeXmlElement(xml_frame);

	CHECK_MEME_LEAK
		return 0;
}

// 读取准备发送的video的线程
DWORD WINAPI ReadFileThreadProc(void* lpParameter)
{
	//ReadFileThread *in = (ReadFileThread *)lpParameter;//获得参数的地址
	//MessageBox(NULL,TEXT("adf"),NULL,MB_OK);
	int i = 0;
	while (TRUE) {
		if (WAIT_OBJECT_0 == WaitForSingleObject(lpParameter, INFINITE)) {
			// read video file
			i = 2;
		}

		i = 1;
	}

	return 0;
}

// The one and only CAccessServerApp object

CAccessServerApp theApp;

// CAccessServerApp initialization

BOOL CAccessServerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CAccessServerDlg dlg;
	m_pMainWnd = &dlg;

	// start thread to connect assign server
	CEvent read_file_event;
	DWORD dw_thread_id_connect = 0, dw_thread_read_file = 0;
	HANDLE thread_connect = ::CreateThread(0, 0, ConnectAssignServerThreadProc, 
	(void*)&read_file_event, 0, &dw_thread_id_connect);
	/*HANDLE thread_readfile = ::CreateThread(0, 0, ReadFileThreadProc, 
		(void*)&read_file_event, 0, &dw_thread_id_connect);*/
	//::SetEvent(g_read_file_event);

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

