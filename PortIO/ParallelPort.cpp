#include "stdafx.h"
#include "ParallelPort.h"
#include "PortSelectDlg.h"
#include "WinIoWrapper.h"
#include "PortCheck.h"
#include "Registry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Build the reset configuration data
BYTE resetData[] =
{
   0xD5,
   0xFF,    // Device ID
   0x08
};

ParallelPort::ParallelPort()
{
    // Load all default settings here
    m_oSettings.SetAt(_T("PORTNAME"),_T("LPT1"));
    m_oSettings.SetAt(_T("DCLK"),_T("5"));
    m_oSettings.SetAt(_T("DIN"),_T("2"));

    // The actual address will be read later, when WinIO is ensured to be open
    m_oSettings.SetAt(_T("PORTADDRESS"), "");

    m_bPortOpen = false;
}

ParallelPort::~ParallelPort()
{
   WinIoWrapper::Instance()->Shutdown();
}

bool ParallelPort::Open()
{
   if (m_bPortOpen)
   {
      Close();
   }

   bool success = true;


   // If WinIO is open, try to get the actual port address
   if (WinIoWrapper::Instance()->Initialize())
   {
      CString badAddress;
      badAddress.Format(_T("0x%X"),-1);

      CString portname;
      GetSetting("PORTNAME", portname);

      CString portaddress = GetLptPortAddressAsString(portname);

      // If its still not valid, return an error
      if (portaddress == badAddress)
      {
         success = false;
         m_strLastError = "Physical port address not found.  Make sure your port is configured properly.";
      }
      // We found it, store the address
      else
      {
         m_oSettings.SetAt("PORTADDRESS", portaddress);
      }
   }
   else
   {
      m_strLastError = CString("") +
         "Unable to initialize WinIO parallel port driver:" +
         "\n\nPlease ensure the WinIO drivers are present in the application directory.";
      
      success = false;
   }

   m_bPortOpen = success;

   return success;
}

void ParallelPort::Close()
{
    m_bPortOpen = false;
}

bool ParallelPort::IsOpen()
{    
    return m_bPortOpen;
}

bool ParallelPort::SetSetting(const CString & strSetting,const CString & strValue)
{
    bool retVal = false;
    
    CString oldValue;
    if( m_oSettings.Lookup(strSetting,oldValue) )
    {
        if(strSetting == _T("PORTNAME"))
        {
            retVal = PortCheck::IsParallelPort(strValue);
            
            if(retVal)
            {
               // Force the address to be read at a later time, when WinIO is ensured
               // to be open
               m_oSettings.SetAt(_T("PORTADDRESS"), "");
            }
            else
            {
                m_strLastError.Format(_T("Invalid parallel port name '%s'."), strValue);
            }
        }
        else if(strSetting == _T("PORTADDRESS"))
        {
            m_strLastError.Format(_T("PORTADDRESS is a read only setting."));
        }
        else
        {
            retVal = true;   
        }
    }
    else
    {
        m_strLastError.Format(_T("SetSetting - Unrecognized setting '%s'."),strSetting);
    }
    
    if(retVal)
    {
        m_oSettings.SetAt(strSetting,strValue);
    }
    
    return retVal;   
}

bool ParallelPort::GetSetting(const CString & strSetting,CString & strValue)
{
    bool retVal = false;
    
    if( m_oSettings.Lookup(strSetting,strValue) )
    {
        retVal = true;
    }
    else
    {
        m_strLastError.Format(_T("SetSetting - Unrecognized setting '%s'"),strSetting);
    }
    
    return retVal;      
}

bool ParallelPort::GetBoardVersionInfo(CString& versionInfo)
{
   versionInfo = "";
   m_strLastError = "Parallel Port does not implement reading board information.";

   return false;
}

bool ParallelPort::ReadADC(int /* channel */, BYTE* /* dataBuffer */, int /* bufferSize */)
{
   m_strLastError = "Parallel Port does not implement reading ADC data.";

   return false;
}

const BYTE startPadBytes[] = {0x0,0x0,0x0,0x0, 0x0};
const BYTE endPadBytes[] = {0x0};

bool ParallelPort::Configure(const BYTE* pData, int size, Destination destination, bool /* verifyDownload */)
{
   bool success = false;
   CString portname;
   GetSetting("PORTNAME", portname);

   if(!IsOpen())
   {
      m_strLastError = portname + _T(" port not open.");
      return false;
   }

   if (destination == ChipPrimary || destination == ChipReconfig)
   {
      success =  WriteParallel(startPadBytes, sizeof(startPadBytes));
      success &= WriteParallel(pData, size);
      success &= WriteParallel(endPadBytes, sizeof(endPadBytes));

      if (!success)
      {
         m_strLastError = "Configuration failed on " + portname + ".";
      }
   }
   else 
   {
      m_strLastError = "Unhandled destination.";
   }

   return success;   
}

bool ParallelPort::WriteParallel(const BYTE* data, int size)
{
    CString val;
    GetSetting(_T("DCLK"),val);
    int nBitDCLK = _ttoi(val);
    
    GetSetting(_T("DIN"),val);
    int nBitDIN = _ttoi(val);
    
    GetSetting(_T("PORTADDRESS"),val);
    DWORD wPortAddress = (WORD)~0x0;
    _stscanf(val ,_T("%X"), &wPortAddress);

    if(wPortAddress == ~0x0) return false;

    const BYTE dataCLK((BYTE)(0x01 << nBitDCLK));
    
    try
    {
      //nByte increments through bytes of stream
      for (int nByte = 0; nByte < size; ++nByte)			
      {
         BYTE dataByte = *(data + nByte);

         //nBit decrementss through bits of each byte (because data is written serially)
         for (int nBit = 7; nBit >= 0; --nBit)			
         {
               // Data is sent bit serially, output to port is one byte at a time
               // Shift dataByte right by current bit position to move bit of interest to bit 0
               // (ie, for bit 7 ==> shift data byte left 7 times to move bit 7 to bit 0
               // bitwise AND result with 0x01 and shift result right to data input bit location
               BYTE dataIN( (BYTE)(((dataByte >> nBit) & 0x01) << nBitDIN) );

               // Write data with clock low
               if(!WinIoWrapper::Instance()->SetPortVal((WORD) wPortAddress, dataIN, 1))
               {
                  return false;
               }

               // Write data with clock high
               if(!WinIoWrapper::Instance()->SetPortVal((WORD) wPortAddress, dataIN + dataCLK, 1)) 
               {
                  return false;
               }
         }
      }

      return WinIoWrapper::Instance()->SetPortVal((WORD) wPortAddress, 0x00, 1);
    }
    catch (...)
    {
       return false;
    }
}

UINT ParallelPort::SelectPortDlg(bool bEnableAdvancedSettings)
{
    UINT retVal = IDCANCEL;
    
    Close();
    
    PortSelectDlg oDlg;
    
    oDlg.m_bCOMonly = false;
    oDlg.m_bLPTonly = true;
    oDlg.m_bDisableExtraSettings = !bEnableAdvancedSettings;
    
    GetSetting(_T("PORTNAME"),oDlg.m_strPort);
    
    // Send settings to the parallel and serial port
    SetDialogSettings(oDlg);

    retVal = (UINT) oDlg.DoModal();
    if(retVal == IDOK)
    {
        SetSetting(_T("PORTNAME"),oDlg.m_strPort);
        GetDialogSettings(oDlg);
    }
    
    Open();
    
    return retVal;
}

void ParallelPort::SetDialogSettings(PortSelectDlg & oDlg)
{
    CString val;
    GetSetting(_T("DCLK"),val);
    oDlg.m_nDCLK = _ttoi(val.GetBuffer(0));
    
    GetSetting(_T("DIN"),val);
    oDlg.m_nDIN = _ttoi(val.GetBuffer(0));
}

void ParallelPort::GetDialogSettings(PortSelectDlg & oDlg)
{
    CString val;
    val.Format(_T("%d"),oDlg.m_nDCLK);
    SetSetting(_T("DCLK"),val);
    val.Format(_T("%d"),oDlg.m_nDIN);
    SetSetting(_T("DIN"),val);
}

CString ParallelPort::GetLptPortAddressAsString(CString strPort)
{
    CString portAddress;
    int nAddress = GetLptPortAddressAsInt(strPort);
    portAddress.Format(_T("0x%X"),nAddress);    
    return portAddress;
}

int ParallelPort::GetLptPortAddressAsInt(CString strPort)
{
    return PortCheck::GetPortAddress(strPort);
}

bool ParallelPort::WriteSettings(CString regKey)
{
    bool retVal = true;

    CRegistry reg;
    retVal &= (reg.SetRootKey(HKEY_CURRENT_USER) == TRUE);
    retVal &= (reg.SetKey(regKey,TRUE) == TRUE);
    if(retVal)
    {
        for( POSITION pos = m_oSettings.GetStartPosition(); pos != NULL; )
        {
            CString key, value;
            m_oSettings.GetNextAssoc( pos, key, value );
            retVal &= (reg.WriteString(key,value) == TRUE);
        }
    }    
    
    if(!retVal) m_strLastError.Format(_T("Unable to write serial port settings to registry key '%s'."),regKey);
    
    return retVal;   
}

bool ParallelPort::ReadSettings(CString regKey)
{
    bool retVal = true;

    CRegistry reg;
    retVal &= (reg.SetRootKey(HKEY_CURRENT_USER) == TRUE);
    retVal &= (reg.SetKey(regKey,FALSE) == TRUE);
    if(retVal)
    {
        for( POSITION pos = m_oSettings.GetStartPosition(); pos != NULL; )
        {
            CString key, value;
            m_oSettings.GetNextAssoc( pos, key, value );
            SetSetting(key,reg.ReadString(key,value));
        }
    }

    if(!retVal) m_strLastError.Format(_T("Unable to read serial port settings from registry key '%s'."),regKey);
    
    return retVal;   
}