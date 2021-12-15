
// factory_manager_mfc.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "factory_manager_mfc.h"
#include "factory_manager_mfcDlg.h"
#include "CLoginDlg.h"
#include <mysql.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CfactorymanagermfcApp

BEGIN_MESSAGE_MAP(CfactorymanagermfcApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CfactorymanagermfcApp construction

CfactorymanagermfcApp::CfactorymanagermfcApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CfactorymanagermfcApp object

wchar_t* Utf8_2_Unicode(char* row_i, wchar_t* wszStr);
void Utf8ToGbk(const char* src_str, char* new_str);
TCHAR* CharToTCHAR(char* charStr, TCHAR* str);

CfactorymanagermfcApp theApp;

extern MYSQL* mysql;
void trim(char* str);
TCHAR g_szFactoryname[100];

int connectDB()
{
	FILE* fp = fopen("database.txt", "r");
	g_szFactoryname[0] = 0;
	// 	fread()
	char ip[100], password[100], factory_name[100];
	if (fp)
	{
		fgets(ip, 100, fp); trim(ip);
// 		Utf8ToGbk(ip, ip);
		fgets(password, 100, fp); trim(password);
// 		Utf8ToGbk(password, password);
		fgets(factory_name, 100, fp); trim(factory_name);
		CharToTCHAR(factory_name, g_szFactoryname);
// 		Utf8_2_Unicode(factory_name, g_szFactoryname);
		fclose(fp);
		mysql = mysql_init(0);
		if (mysql && mysql_real_connect(mysql, ip, "root", password, "factory", 0, NULL, 0) == NULL)
		{
			AfxMessageBox(_T("数据库连接失败！"));
			return -1;
		}
		mysql_query(mysql, "set names utf8mb4;");
// 		mysql_query(mysql, "set session character_set_connection=gbk;");
// 		mysql_query(mysql, "set session character_set_client=gbk;");
// 		mysql_query(mysql, "set session character_set_results=gbk;");

	}
	else
	{
		AfxMessageBox(_T("不能打开database.txt"));
		return -1;
	}
	return 0;
}


// CfactorymanagermfcApp initialization

BOOL CfactorymanagermfcApp::InitInstance()
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

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	int res = connectDB();
	if (res == -1) {
		return FALSE;
	}
	CLoginDlg loginDlg;
	if (loginDlg.DoModal() != IDOK) {
		return FALSE;
	}
	CfactorymanagermfcDlg dlg;
	m_pMainWnd = &dlg;
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
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

