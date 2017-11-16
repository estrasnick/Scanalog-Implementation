#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

class FPAA_ApplicationApp : public CWinApp
{
public:
	FPAA_ApplicationApp();

protected:
	virtual BOOL InitInstance();

private:
	DECLARE_MESSAGE_MAP()
};

extern FPAA_ApplicationApp theApp;