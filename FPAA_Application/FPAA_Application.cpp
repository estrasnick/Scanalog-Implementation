#include "stdafx.h"
#include "FPAA_Application.h"
#include "FPAA_ApplicationDlg.h"
#include "DownloadManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// The one and only FPAA_ApplicationApp object
FPAA_ApplicationApp theApp;

BEGIN_MESSAGE_MAP(FPAA_ApplicationApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


FPAA_ApplicationApp::FPAA_ApplicationApp()
{

}

BOOL FPAA_ApplicationApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	// Initialize sockets
	if (AfxSocketInit() == FALSE)
	{
		AfxMessageBox("Sockets Could Not Be Initialized");
		return FALSE;
	}

	//AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
	CWinApp::InitInstance();

	AfxEnableControlContainer();

   // Set the key for MFC uses to read\write the registry.  The final location will be
   //   HKEY_CURRENT_USER\Software\YourCompany\FPAA_Application\ 
   SetRegistryKey("YourCompany");

   // Read port settings from the registry.
   DownloadManager::Instance()->LoadPortSettings(CString(m_pszRegistryKey) + "\\" + m_pszProfileName);

   // Show the main dialog window
	FPAA_ApplicationDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

   // Save port settings to the registry.
   DownloadManager::Instance()->SavePortSettings(CString(m_pszRegistryKey) + "\\" + m_pszProfileName);

   // The window has closed, so we return FALSE here to go ahead and
   // terminate the application.
   return FALSE;
}
