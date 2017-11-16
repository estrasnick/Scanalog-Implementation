#pragma once

#include "resource.h"

class AboutDlg : public CDialog
{
public:
	AboutDlg();

	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

private:
	DECLARE_MESSAGE_MAP()
};
