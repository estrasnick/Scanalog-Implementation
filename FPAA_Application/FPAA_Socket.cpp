#include "stdafx.h"
#include "Helpers.h"
#include "FPAA_Socket.h"
#include "FPAA_ApplicationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


FPAA_Socket::FPAA_Socket()
{
}

FPAA_Socket::~FPAA_Socket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(FPAA_Socket, CAsyncSocket)
	//{{AFX_MSG_MAP(FPAA_Socket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0


void FPAA_Socket::OnAccept(int nErrorCode)
{
	FPAA_Socket *clientSocket = new FPAA_Socket();
	Accept(*clientSocket);
	app->SetClientSocket(clientSocket);
	CAsyncSocket::OnAccept(nErrorCode);
}

void FPAA_Socket::OnClose(int nErrorCode)
{
	CAsyncSocket::OnClose(nErrorCode);
}

void FPAA_Socket::OnConnect(int nErrorCode)
{
	CAsyncSocket::OnConnect(nErrorCode);
}

void FPAA_Socket::OnOutOfBandData(int nErrorCode)
{
	CAsyncSocket::OnOutOfBandData(nErrorCode);
}

void FPAA_Socket::OnReceive(int nErrorCode)
{
	if (nErrorCode == 0)
	{
		PostMessage(*dlg_hwnd, WM_SOCKET_RECEIVE_UPDATE, 0, 0);
	}

	CAsyncSocket::OnReceive(nErrorCode);
}

void FPAA_Socket::OnSend(int nErrorCode)
{
	CAsyncSocket::OnSend(nErrorCode);
}

void FPAA_Socket::SetParentDlg(FPAA_ApplicationDlg *pDlg)
{
	app = pDlg;
}

void FPAA_Socket::SetHWND(HWND* newHwnd)
{
	dlg_hwnd = newHwnd;
}
