#ifndef FPAA_SOCKET
#define FPAA_SOCKET


#include <afxsock.h>
#include "FPAA_ApplicationDlg.h"

// forward declarations to resolve cyclic dependencies
class FPAA_ApplicationDlg;

class FPAA_Socket : public CAsyncSocket
{
	// Attributes
public:

	// Operations
public:
	FPAA_Socket();
	virtual ~FPAA_Socket();

	// Overrides
public:
public:
	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnOutOfBandData(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	void SetParentDlg(FPAA_ApplicationDlg *pDlg);
	void SetHWND(HWND* newHwnd);

protected:
private:
	FPAA_ApplicationDlg *app;
	HWND* dlg_hwnd;

};

#endif // !FPAA_SOCKET
