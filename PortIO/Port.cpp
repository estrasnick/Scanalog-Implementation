#include "stdafx.h"
#include "Port.h"
#include "PortSelectDlg.h"
#include "PortCheck.h"
#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

Port::Port()
{
   m_pActivePort = &m_oSerialPort;
}

Port::~Port()
{

}

bool Port::Open()
{
   bool retVal = false;

   if(m_pActivePort)
   {
      retVal = m_pActivePort->Open();
   }
   else
   {
      m_strLastError.Format(_T("No active port."));
   }

   return retVal;
}

void Port::Close()
{
   if(m_pActivePort) m_pActivePort->Close();
}

bool Port::IsOpen()
{
   return m_pActivePort ? m_pActivePort->IsOpen() : false;
}

UINT Port::SelectPortDlg(bool bEnableAdvancedSettings)
{
   UINT retVal = IDCANCEL;

   if(!m_pActivePort) return retVal;
   m_pActivePort->Close();
  
   PortSelectDlg oDlg;

   oDlg.m_bCOMonly = false;   // allow user to pick any port type
   oDlg.m_bLPTonly = false;   // allow user to pick any port type
   oDlg.m_bDisableExtraSettings = !bEnableAdvancedSettings;

   GetSetting(_T("PORTNAME"),oDlg.m_strPort);

   // Send settings to the parallel and serial port
   m_oSerialPort.SetDialogSettings(oDlg);
   m_oParallelPort.SetDialogSettings(oDlg);

   retVal = (UINT) oDlg.DoModal();
   if(retVal == IDOK)
   {
      m_oSerialPort.GetDialogSettings(oDlg);
      m_oParallelPort.GetDialogSettings(oDlg);

	  if(PortCheck::IsSerialPort(oDlg.m_strPort))
	  {
		  m_pActivePort = &m_oSerialPort;
	  }
	  else
	  {
		  m_pActivePort = &m_oParallelPort;
	  }

      SetSetting(_T("PORTNAME"),oDlg.m_strPort);      
   }

   m_pActivePort->Open();

   return retVal;
}

CString Port::GetPortType()
{
   CString retVal(_T(""));

   if(m_pActivePort)
   {
      retVal = m_pActivePort->GetPortType();
   }
   else
   {
      m_strLastError.Format(_T("No active port."));
   }

   return retVal;
}

bool Port::SetPortType(CString strPortType)
{
   strPortType.MakeUpper();
   if(strPortType == "SERIAL")
   {  
      if(m_pActivePort) m_pActivePort->Close();

      m_pActivePort = &m_oSerialPort;
   }
   else if(strPortType == "PARALLEL")
   {
      if(m_pActivePort) m_pActivePort->Close();

      m_pActivePort = &m_oParallelPort;
   }
   else
   {
      m_strLastError.Format(_T("SetPortType - Unrecognized port type '%s'"),strPortType);
      return false;
   }

   return true;
}

bool Port::SetSetting(const CString & strSetting,const CString & strValue)
{
   bool retVal = false;

   if(m_pActivePort)
   {
      retVal = m_pActivePort->SetSetting(strSetting,strValue);
   }
   else
   {
      m_strLastError.Format(_T("No active port."));
   }

   return retVal;   
}

bool Port::GetSetting(const CString & strSetting,CString & strValue)
{
   bool retVal = false;

   if(m_pActivePort)
   {
      retVal = m_pActivePort->GetSetting(strSetting,strValue);
   }
   else
   {
      m_strLastError.Format(_T("No active port."));
   }

   return retVal;      
}

bool Port::Configure(const BYTE* pData, int size, Destination destination, bool verifyDownload)
{
	bool retVal = false;
	BYTE ModifiedConfigData[1000];	//set up a buffer that we can change...

	if(m_pActivePort)
	{
		/*	Dave remmed out this block 9Mar16
		if(destination == ChipPrimary)
		{
			//Updates to support either chip.  HCA - 28 Mar 03, 
			//2 April Moved to here from FilterDemo.cpp with Troy.
			//This is now general for recompiling all existing demos
			memcpy(ModifiedConfigData,pData,size);
			//Try an ANx221
			ModifiedConfigData[4] = (BYTE)0x80;
			retVal = m_pActivePort->Configure(ModifiedConfigData, size, 
				destination, true);
			if(!retVal)
			{
				//ANx221 failed, try an ANx220
				ModifiedConfigData[4] = (BYTE)0x30;
				retVal = m_pActivePort->Configure(ModifiedConfigData, size, 
					destination, true);
			}
			//need to add these lines to check for AN231… crs 10/3/2007
			if(!retVal)
			{
				//ANx220 failed, try an ANx231
				ModifiedConfigData[4] = (BYTE)0x00;
				retVal = m_pActivePort->Configure(ModifiedConfigData, size, 
					destination, true);
			}
			//end 10/03/2007

			 // If verifyDownload is off, then even if there was an error we 
			 // return that there was not
			 if (!verifyDownload)
			 {
				retVal = true;
			 }
		}
		else
		{
			retVal = m_pActivePort->Configure(pData, size, 
				destination, verifyDownload);
		}
		*/
		retVal = m_pActivePort->Configure(pData, size, destination, verifyDownload);	//Dave added this line 9Mar16
	} 
	else 
	{
		m_strLastError.Format(_T("No active port."));
	}

	return retVal;   
}

bool Port::ReadADC(int channel, BYTE* dataBuffer, int bufferSize)
{
   bool retVal = false;

   if (m_pActivePort)
   {
      retVal = m_pActivePort->ReadADC(channel, dataBuffer, bufferSize);
   }
   else
   {
      m_strLastError.Format(_T("No active port."));
   }

   return retVal;

}

bool Port::GetBoardVersionInfo(CString& versionInfo)
{
   bool retVal = false;

   if (m_pActivePort)
   {
      retVal = m_pActivePort->GetBoardVersionInfo(versionInfo);
   }
   else
   {
      m_strLastError.Format(_T("No active port."));
   }

   return retVal;
}

CString Port::GetLastError()
{
   if(m_pActivePort)
   {
      m_strLastError = m_pActivePort->GetLastError();
   }
   else
   {
      m_strLastError.Format(_T("No active port."));
   }

   return m_strLastError;   
}

bool Port::WriteSettings(CString regKey)
{
    bool retVal = true;

    CRegistry reg;
    retVal &= (reg.SetRootKey(HKEY_CURRENT_USER) == TRUE);
    retVal &= (reg.SetKey(regKey,TRUE) == TRUE);
    if(retVal)
    {
        retVal &= (reg.WriteString(_T("PORTTYPE"),m_pActivePort->GetPortType())==TRUE);

        if(m_pActivePort == &m_oParallelPort)
        {
            retVal &= m_oSerialPort.WriteSettings(regKey);
            retVal &= m_oParallelPort.WriteSettings(regKey);
        }
        else
        {
            retVal &= m_oParallelPort.WriteSettings(regKey);
            retVal &= m_oSerialPort.WriteSettings(regKey);
        }
        
    }
    
    if(!retVal) m_strLastError.Format(_T("Unable to write port settings to registry key '%s'."),regKey);
    
    return retVal;   
}

bool Port::ReadSettings(CString regKey)
{
    bool retVal = true;

    CRegistry reg;
    retVal &= (reg.SetRootKey(HKEY_CURRENT_USER) == TRUE);
    retVal &= (reg.SetKey(regKey,FALSE) == TRUE);
    if(retVal)
    {
        CString porttype = reg.ReadString(_T("PORTTYPE"),m_oSerialPort.GetPortType());

        if(porttype == m_oSerialPort.GetPortType())
        {
            m_pActivePort = &m_oSerialPort;
        }
        else if(porttype == m_oParallelPort.GetPortType())
        {
            m_pActivePort = &m_oParallelPort;
        }
        else
        {
            m_strLastError.Format(_T("Invalid port type loaded from registry - '%s'."), porttype);
            return false;
        }

        retVal &= m_oParallelPort.ReadSettings(regKey);
        retVal &= m_oSerialPort.ReadSettings(regKey);
    }

    if(!retVal) m_strLastError.Format(_T("Unable to read port settings from registry key '%s'."),regKey);
    
    return retVal;   
}
