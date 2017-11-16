//---------------------------------------------------------------------------
//
// Name:        FPAA_DialogApp.cpp
// Author:      HP Sprout
// Created:     11/14/2016 3:37:47 PM
// Description: 
//
//---------------------------------------------------------------------------

#include "FPAA_DialogApp.h"
#include "FPAA_DialogDlg.h"

IMPLEMENT_APP(FPAA_DialogDlgApp)

bool FPAA_DialogDlgApp::OnInit()
{
	FPAA_DialogDlg* dialog = new FPAA_DialogDlg(NULL);
	SetTopWindow(dialog);
	dialog->Show(true);		
	return true;
}
 
int FPAA_DialogDlgApp::OnExit()
{
	return 0;
}
