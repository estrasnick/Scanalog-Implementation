#pragma once

#include "PortIO_resource.h"

/////////////////////////////////////////////////////////////////////////////
// PortSelectDlg dialog
class PortSelectDlg : public CDialog
{
// Construction
public:
	PortSelectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PortSelectDlg)
	enum { IDD = IDD_SELECT_PORT };
	CComboBox	m_oPort;
	CString	m_oPortStatus;
	CString	m_oBaudRate;
	int		m_nDCLK;
	int		m_nDIN;
	//}}AFX_DATA

   CString m_strPort;   // Current port selection (rw)
   bool m_bCOMonly;     // Only show com ports? (rw)
   bool m_bLPTonly;     // Only shoe lpt ports? (rw)
   bool m_bDisableExtraSettings;        // Disable baud rate and pin setting selection


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PortSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PortSelectDlg)
	afx_msg void OnChangePort();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDclk();
	afx_msg void OnSelchangeDin();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

