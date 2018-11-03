// ImageSaver.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ImageSaver.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int cnum = 0;  // 控制开启一次stream采图的数量
int CINDEX = 0; //开启的相机索引，默认开启第0号相机
// CImageSaverApp

BEGIN_MESSAGE_MAP(CImageSaverApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CImageSaverApp construction

CImageSaverApp::CImageSaverApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CImageSaverApp object

CImageSaverApp theApp;


// CImageSaverApp initialization

BOOL CImageSaverApp::InitInstance()
{
	

	/*****************接收ShellExecute的消息************************/
	CString str;
	for (int i = 1; i < __argc; i++)
	{
		str += __argv[i];     
	}

	/***************************设置INDEX以选择需要的相机**********************/
	if (str == "openCamera1")	    CINDEX = 0;
	else if (str == "openCamera2")	CINDEX = 1;
	else if (str == "openCamera3")	CINDEX = 2;
	else if (str == "openCamera4")	CINDEX = 3;
	else if (str == "openCamera5")	CINDEX = 4;
	else if (str == "openCamera6")	CINDEX = 5;

		

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

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CImageSaverDlg dlg;
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

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
