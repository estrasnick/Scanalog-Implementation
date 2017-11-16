#pragma once

#include <afxtempl.h>

class PortBase
{
public:
   virtual ~PortBase();

   // Destination of data sent via this port
   enum Destination
   {
      SerialFlash1 = 'A',
      SerialFlash2 = 'B',
      SerialFlash3 = 'C',
      SerialFlash4 = 'D',
      Program1 = 'a',
      Program2 = 'b',
      Program3 = 'c',
      Program4 = 'd',
      ChipPrimary = '0',
      ChipReconfig = '1',	
	  ChipReset = 'R'		//crs 10/3/2007
   };
   
   virtual bool Open() = 0;            // Open port for communication
   virtual void Close() = 0;           // Close port, if already open
   virtual bool IsOpen() = 0;          // Is the port already open?

   // GUI port select - returns IDOK or IDCANCEL
   virtual UINT SelectPortDlg(bool bEnableAdvancedSettings = false) = 0;

   // Retrive port type
   virtual CString GetPortType() = 0;

   // Set and retrieve port settings
   virtual bool SetSetting(const CString & strSetting,const CString & strValue) = 0;
   virtual bool GetSetting(const CString & strSetting,CString & strValue) = 0;

   // Save and load port settings
   virtual bool WriteSettings(CString regKey) = 0;
   virtual bool ReadSettings(CString regKey) = 0;

   // Send configuration stream via open port
   virtual bool Configure(const BYTE* pData, int size, Destination destination, bool verifyDownload = false) = 0;
   
   // Read data via open port
   virtual bool GetBoardVersionInfo(CString& versionInfo) = 0;
   virtual bool ReadADC(int channel, BYTE* dataBuffer, int bufferSize) = 0;
   
   virtual CString GetLastError() = 0;
   
protected:
   // Last Error
   CString m_strLastError;

   // Port Settings
   CMapStringToString m_oSettings;

  	PortBase();
};
