#include "stdafx.h"
#include "portcheck.h"
#include "PortSelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PortSelectDlg dialog


PortSelectDlg::PortSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(PortSelectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(PortSelectDlg)
	m_oPortStatus = _T("");
	m_oBaudRate = _T("57600");
	m_nDCLK = 5;
	m_nDIN = 2;
	//}}AFX_DATA_INIT

   m_strPort = _T("COM1");
   m_bCOMonly = true;
   m_bLPTonly = false;
   m_bDisableExtraSettings = false;
}


void PortSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PortSelectDlg)
	DDX_Control(pDX, IDC_PC_SERIAL_PORT, m_oPort);
	DDX_Text(pDX, IDC_PC_PORT_STATUS, m_oPortStatus);
	DDX_CBString(pDX, IDC_PC_SERIALBAUDRATE, m_oBaudRate);
	DDX_CBIndex(pDX, IDC_DCLK, m_nDCLK);
	DDX_CBIndex(pDX, IDC_DIN, m_nDIN);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PortSelectDlg, CDialog)
	//{{AFX_MSG_MAP(PortSelectDlg)
	ON_CBN_SELCHANGE(IDC_PC_SERIAL_PORT, OnChangePort)
	ON_CBN_SELCHANGE(IDC_DCLK, OnSelchangeDclk)
	ON_CBN_SELCHANGE(IDC_DIN, OnSelchangeDin)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PortSelectDlg message handlers

void PortSelectDlg::OnChangePort() 
{
   CString sPort;
   int index;
   if((index = m_oPort.GetCurSel()) == CB_ERR)
   {
      m_oPort.GetWindowText(sPort);
   }
   else
   {
      m_oPort.GetLBText(index,sPort);
   }

   // Check strings from available serial ports
   for(int nPort = COM1;nPort <= COM8;nPort++)
   {
      if(sPort == GetEnumString((PortType)nPort))
      {
         sPort.Format(_T("COM%1d"),nPort);
         break;
      }
   }
      
   m_strPort = sPort;
   
   PortCheck::GetPortStatus(m_strPort);
   m_oPortStatus = PortCheck::FormatError();

   if(PortCheck::IsSerialPort(m_strPort))
   {
       GetDlgItem(IDC_PC_SERIALBAUDRATE)->ShowWindow(TRUE);
       GetDlgItem(IDC_DCLK)->ShowWindow(FALSE);
       GetDlgItem(IDC_DCLK_TEXT)->ShowWindow(FALSE);
       GetDlgItem(IDC_DIN)->ShowWindow(FALSE);
       GetDlgItem(IDC_DIN_TEXT)->ShowWindow(FALSE);
       if(m_bDisableExtraSettings)
           GetDlgItem(IDC_PC_SETTING_LABEL)->SetWindowText(_T("Serial Baud Rate"));
       else
           GetDlgItem(IDC_PC_SETTING_LABEL)->SetWindowText(_T("Select Serial Baud Rate"));
   }
   else
   {
       GetDlgItem(IDC_PC_SERIALBAUDRATE)->ShowWindow(FALSE);
       GetDlgItem(IDC_DCLK)->ShowWindow(TRUE);
       GetDlgItem(IDC_DCLK_TEXT)->ShowWindow(TRUE);
       GetDlgItem(IDC_DIN)->ShowWindow(TRUE);
       GetDlgItem(IDC_DIN_TEXT)->ShowWindow(TRUE);
       if(m_bDisableExtraSettings)
           GetDlgItem(IDC_PC_SETTING_LABEL)->SetWindowText(_T("Parallel Pinout"));           
       else
           GetDlgItem(IDC_PC_SETTING_LABEL)->SetWindowText(_T("Select Parallel Pinout"));           

   }
   
   if(m_bDisableExtraSettings)
   {
       GetDlgItem(IDC_PC_SERIALBAUDRATE)->EnableWindow(FALSE);
       GetDlgItem(IDC_DCLK)->EnableWindow(FALSE);
       GetDlgItem(IDC_DIN)->EnableWindow(FALSE);
   }
   else
   {
       GetDlgItem(IDC_PC_SERIALBAUDRATE)->EnableWindow(TRUE);
       GetDlgItem(IDC_DCLK)->EnableWindow(TRUE);
       GetDlgItem(IDC_DIN)->EnableWindow(TRUE);
   }
   
   UpdateData(FALSE);
}

BOOL PortSelectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

   // Load strings from available serial ports
   CStringArray oPorts;
   PortCheck::GetAvailablePorts(oPorts,m_bCOMonly,m_bLPTonly);

   for(int ct = 0;ct < oPorts.GetSize();ct++)
   {
      int index = m_oPort.AddString(oPorts[ct]);

      if(m_strPort == oPorts[ct])
      {
         m_oPort.SetCurSel(index);
      }
   }
 
   if(m_oPort.GetCurSel() == CB_ERR)
   {
      m_oPort.SetWindowText(m_strPort);
   }

   OnChangePort();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void PortSelectDlg::OnSelchangeDclk() 
{
   CComboBox * pDCLK = (CComboBox*)GetDlgItem(IDC_DCLK);
   CComboBox * pDIN = (CComboBox*)GetDlgItem(IDC_DIN);

   if(pDCLK->GetCurSel() == pDIN->GetCurSel())
   {
      MessageBox(_T("Cannot use same pin for both data and clock."),NULL,MB_ICONWARNING);
      pDCLK->SetCurSel(m_nDCLK);
      pDCLK->SetFocus();
   }
}

void PortSelectDlg::OnSelchangeDin() 
{
   CComboBox * pDCLK = (CComboBox*)GetDlgItem(IDC_DCLK);
   CComboBox * pDIN = (CComboBox*)GetDlgItem(IDC_DIN);

   if(pDCLK->GetCurSel() == pDIN->GetCurSel())
   {
      MessageBox(_T("Cannot use same pin for both data and clock."),NULL,MB_ICONWARNING);
      pDIN->SetCurSel(m_nDIN);
      pDIN->SetFocus();
   }
	
}

void PortSelectDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    if(((nFlags & MK_CONTROL) == MK_CONTROL) &&
       ((nFlags & MK_SHIFT) == MK_SHIFT) &&
       ((nFlags & MK_RBUTTON) == MK_RBUTTON))
    {
        CWnd *pWnd = GetDlgItem(IDC_PC_SERIALBAUDRATE);
        CRect rect;
        pWnd->GetWindowRect(rect);
        ScreenToClient(rect);
        if(rect.PtInRect(point))
        {
            pWnd->EnableWindow(TRUE);
        }

        pWnd = GetDlgItem(IDC_DCLK);
        pWnd->GetWindowRect(rect);
        ScreenToClient(rect);
        if(rect.PtInRect(point))
        {
            pWnd->EnableWindow(TRUE);
        }

        pWnd = GetDlgItem(IDC_DIN);
        pWnd->GetWindowRect(rect);
        ScreenToClient(rect);
        if(rect.PtInRect(point))
        {
            pWnd->EnableWindow(TRUE);
        }
    }
    
	CDialog::OnLButtonDblClk(nFlags, point);
}
