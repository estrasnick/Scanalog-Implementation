///-----------------------------------------------------------------
///
/// @file      FPAA_DialogDlg.cpp
/// @author    HP Sprout
/// Created:   11/14/2016 3:37:47 PM
/// @section   DESCRIPTION
///            FPAA_DialogDlg class implementation
///
///------------------------------------------------------------------

#include "FPAA_DialogDlg.h"

//Do not add custom headers
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End

//----------------------------------------------------------------------------
// FPAA_DialogDlg
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(FPAA_DialogDlg,wxDialog)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(FPAA_DialogDlg::OnClose)
END_EVENT_TABLE()
////Event Table End

FPAA_DialogDlg::FPAA_DialogDlg(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxDialog(parent, id, title, position, size, style)
{
	CreateGUIControls();
}

FPAA_DialogDlg::~FPAA_DialogDlg()
{
} 

void FPAA_DialogDlg::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End.
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	SetTitle(_("FPAA_Dialog"));
	SetIcon(wxNullIcon);
	SetSize(8,8,808,487);
	Center();
	
	////GUI Items Creation End
}

void FPAA_DialogDlg::OnClose(wxCloseEvent& /*event*/)
{
	Destroy();
}
