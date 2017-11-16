#pragma once

#include "PortBase.h"
#include "SerialPort.h"
#include "ParallelPort.h"

class Port : public PortBase  
{
public:
	Port();

   virtual bool Open();            // Open port for communication
   virtual void Close();           // Close port, if already open
   virtual bool IsOpen();          // Is the port already open?

   // GUI port select - returns IDOK or IDCANCEL
   virtual UINT SelectPortDlg(bool bEnableAdvancedSettings = false);

   // Set and Retrive port type
   virtual CString GetPortType();
   virtual bool SetPortType(CString strPortType);

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

   virtual CString GetLastError();
	virtual ~Port();

private:

   // Keep a port object of each type of port
   PortBase * m_pActivePort;
   SerialPort m_oSerialPort;
   ParallelPort m_oParallelPort;
};
