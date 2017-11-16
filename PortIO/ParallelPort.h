#pragma once

#include "PortBase.h"
class PortSelectDlg;

class ParallelPort : public PortBase  
{
public:
	ParallelPort();

   virtual bool Open();            // Open port for communication
   virtual void Close();           // Close port, if already open
   virtual bool IsOpen();          // Is the port already open?

   // GUI port select - returns IDOK or IDCANCEL
   virtual UINT SelectPortDlg(bool bEnableAdvancedSettings = false);
   void SetDialogSettings(PortSelectDlg & oDlg);
   void GetDialogSettings(PortSelectDlg & oDlg);

   // Retrive port type
   virtual CString GetPortType() { return CString(_T("PARALLEL")); }

   // Set and retrieve port settings
   virtual bool SetSetting(const CString & strSetting,const CString & strValue);
   virtual bool GetSetting(const CString & strSetting,CString & strValue);

   // Save and load port settings
   virtual bool WriteSettings(CString regKey);
   virtual bool ReadSettings(CString regKey);

   // Send configuration stream via open port
   virtual bool Configure(const BYTE* pData, int size, Destination destination, bool verifyDownload = false);
   
   // Read data via open port
   virtual bool GetBoardVersionInfo(CString& versionInfo);
   virtual bool ReadADC(int channel, BYTE* dataBuffer, int bufferSize);

   virtual CString GetLastError() { return m_strLastError; }

	virtual ~ParallelPort();

private:
   bool WriteParallel(const BYTE* data, int size);

   // Utility functions
   CString GetLptPortAddressAsString(CString strPort);
   int GetLptPortAddressAsInt(CString strPort);

   bool m_bInitializeWinIo;
   bool m_bPortOpen;
};

