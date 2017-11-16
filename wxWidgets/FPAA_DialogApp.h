//---------------------------------------------------------------------------
//
// Name:        FPAA_DialogApp.h
// Author:      HP Sprout
// Created:     11/14/2016 3:37:47 PM
// Description: 
//
//---------------------------------------------------------------------------

#ifndef __FPAA_DIALOGDLGApp_h__
#define __FPAA_DIALOGDLGApp_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#else
	#include <wx/wxprec.h>
#endif

class FPAA_DialogDlgApp : public wxApp
{
	public:
		bool OnInit();
		int OnExit();
};

#endif
