#include "stdafx.h"
#include "PortCheck.h"
#include "WinIoWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString g_oPortStatus[] = 
{
   "Port is available and ready for use.", 
   "Port not found.",
   "Port is already in use.",
   "Unrecognized port error."
};   

CString GetEnumString(PortStatus nStatus)
{
   ASSERT((nStatus >= 0) && (nStatus < NUM_OF_PORT_STATUS));

   return g_oPortStatus[nStatus];
}

CString g_oPortType[] = 
{
   "LPT1",
   "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9", "COM10",
   "COM11", "COM12", "COM13", "COM14", "COM15", "COM16", "COM17", "COM18", "COM19", "COM20",
   "COM21", "COM22", "COM23", "COM24", "COM25", "COM26", "COM27", "COM28", "COM29", "COM30",
   "COM31", "COM32", "COM33", "COM34", "COM35", "COM36", "COM37", "COM38", "COM39", "COM40",
   "COM41", "COM42", "COM43", "COM44", "COM45", "COM46", "COM47", "COM48", "COM49", "COM50",
   "COM51", "COM52", "COM53", "COM54", "COM55", "COM56", "COM57", "COM58", "COM59", "COM60",
   "COM61", "COM62", "COM63", "COM64", "COM65", "COM66", "COM67", "COM68", "COM69", "COM70",
   "COM71", "COM72", "COM73", "COM74", "COM75", "COM76", "COM77", "COM78", "COM79", "COM80",
   "COM81", "COM82", "COM83", "COM84", "COM85", "COM86", "COM87", "COM88", "COM89", "COM90",
   "COM91", "COM92", "COM93", "COM94", "COM95", "COM96", "COM97", "COM98", "COM99", "COM100",
   "COM101", "COM102", "COM103", "COM104", "COM105", "COM106", "COM107", "COM108", "COM109", "COM110",
   "COM111", "COM112", "COM113", "COM114", "COM115", "COM116", "COM117", "COM118", "COM119", "COM120",
   "COM121", "COM122", "COM123", "COM124", "COM125", "COM126", "COM127", "COM128", "COM129", "COM130",
   "COM131", "COM132", "COM133", "COM134", "COM135", "COM136", "COM137", "COM138", "COM139", "COM140",
   "COM141", "COM142", "COM143", "COM144", "COM145", "COM146", "COM147", "COM148", "COM149", "COM150",
   "COM151", "COM152", "COM153", "COM154", "COM155", "COM156", "COM157", "COM158", "COM159", "COM160",
   "COM161", "COM162", "COM163", "COM164", "COM165", "COM166", "COM167", "COM168", "COM169", "COM170",
   "COM171", "COM172", "COM173", "COM174", "COM175", "COM176", "COM177", "COM178", "COM179", "COM180",
   "COM181", "COM182", "COM183", "COM184", "COM185", "COM186", "COM187", "COM188", "COM189", "COM190",
   "COM191", "COM192", "COM193", "COM194", "COM195", "COM196", "COM197", "COM198", "COM199", "COM200",
   "COM201", "COM202", "COM203", "COM204", "COM205", "COM206", "COM207", "COM208", "COM209", "COM210",
   "COM211", "COM212", "COM213", "COM214", "COM215", "COM216", "COM217", "COM218", "COM219", "COM220",
   "COM221", "COM222", "COM223", "COM224", "COM225", "COM226", "COM227", "COM228", "COM229", "COM230",
   "COM231", "COM232", "COM233", "COM234", "COM235", "COM236", "COM237", "COM238", "COM239", "COM240",
   "COM241", "COM242", "COM243", "COM244", "COM245", "COM246", "COM247", "COM248", "COM249", "COM250",
   "COM251", "COM252", "COM253", "COM254", "COM255", "COM256", 
   "LPT2",
   "LPT3"
};   

CString GetEnumString(PortType nType)
{
   ASSERT((nType >= 0) && (nType < NUM_OF_PORT_TYPES));

   return g_oPortType[nType];
}


//////////////////////////////////////////////////////////////////////
// Functions used to obtain parallel port address
// Taken from http://www.lvr.com/jansfaq.htm
//////////////////////////////////////////////////////////////////////

#define TEST_WINDOWS_NT (!(GetVersion() & 0x80000000))
/***************************************************************************************/
int GetParallelControllerKey(TCHAR *parKey)
{
    HKEY hKey;
    TCHAR myData[255];
    LONG res;
    DWORD mySize;
    FILETIME ftLastWriteTime;
    
    if (NULL==parKey) return(-1);
    
    *parKey=0;
    
    TCHAR myKey[255];
    _stprintf(myKey,_T("HARDWARE\\DESCRIPTION\\System"));
    
    res = RegOpenKeyEx(HKEY_LOCAL_MACHINE,myKey, 0, KEY_READ, &hKey);
    
    if (res!=ERROR_SUCCESS)
        return(-1);
    
    DWORD dwIndex1;
    TCHAR myKey1[255];
    for (dwIndex1=0;dwIndex1<=10;dwIndex1++)
    {
        mySize=sizeof(myData);
        res = RegEnumKeyEx(hKey,dwIndex1,myData,&mySize,NULL,NULL,NULL,&ftLastWriteTime);
        
        if (res==ERROR_SUCCESS) // ERROR_SUCCESS 1
        {
            _tcscpy(myKey1,myKey);
            _tcscat(myKey1,_T("\\"));
            _tcscat(myKey1,myData);
            
            HKEY hKey1;
            res = RegOpenKeyEx(HKEY_LOCAL_MACHINE,myKey1, 0, KEY_READ, &hKey1);
            
            if (res!=ERROR_SUCCESS) return(-1);
            
            DWORD dwIndex2;
            TCHAR myKey2[255];
            for (dwIndex2=0;dwIndex2<=10;dwIndex2++)
            {
                mySize=sizeof(myData);
                res = RegEnumKeyEx(hKey1,dwIndex2,myData,&mySize,NULL,NULL,NULL,&ftLastWriteTime);
                
                if (res==ERROR_SUCCESS) // ERROR_SUCCESS 2
                {
                    _tcscpy(myKey2,myKey1);
                    _tcscat(myKey2,_T("\\"));
                    _tcscat(myKey2,myData);
                    
                    HKEY hKey2;
                    res = RegOpenKeyEx(HKEY_LOCAL_MACHINE,myKey2, 0, KEY_READ, &hKey2);
                    
                    if (res!=ERROR_SUCCESS)
                        return(-1);
                    
                    DWORD dwIndex3;
                    for (dwIndex3=0;dwIndex3<=10;dwIndex3++)
                    {
                        mySize=sizeof(myData);
                        res = RegEnumKeyEx(hKey2,dwIndex3,myData,&mySize,NULL,NULL,NULL,&ftLastWriteTime);
                        
                        if (res==ERROR_SUCCESS) // ERROR_SUCCESS 3
                        {
                            if 
                                (0==_tcscmp(myData,_T("ParallelController")) )
                            {
                                
                                _tcscpy(parKey,myKey2);
                                
                                _tcscat(parKey,_T("\\"));
                                
                                _tcscat(parKey,myData);
                                return(0);
                            }
                        } // ERROR_SUCCESS 3
                        
                    } // for (dwIndex3
                    
                } // // ERROR_SUCCESS 2
                
            } // for (dwIndex2
            
        } // ERROR_SUCCESS 1
        
    } // for (dwIndex1
    
    return(-1);
} //GetParallelControllerKey 

/***************************************************************************************/
int GetAddressLptPortInTheRegistry(int myPort)
{
    HKEY phkResult;
    TCHAR myKey[255];
    TCHAR myData[255];
    LONG res;
    DWORD mySize;
    DWORD myType;
    
    res=GetParallelControllerKey(myKey);
    if (res < 0)
        return(-1);
    
    _stprintf(myData,_T("%s\\%d"),myKey,myPort-1);
    
    res = RegOpenKeyEx(HKEY_LOCAL_MACHINE,myData, 0, KEY_READ, 
        &phkResult);
    if (res != ERROR_SUCCESS)
        return(-1);
    
    mySize=sizeof(myData);
    myType=REG_BINARY;
    
    res=RegQueryValueEx(
        phkResult,                  // handle to key to query
        _T("Configuration Data"),       // address of name of value to query
        NULL,                       // reserved
        &myType,                    // address of buffer for value type
        (unsigned char *)myData,    // address of data buffer
        &mySize                     // address of data buffer size
        );
    
    if (res != ERROR_SUCCESS)
        return(-1);
    
    //printf("config data port %d 0x14 = 0x%02X 0x15 = 0x%02X\n",myPort,myData[0x14],myData[0x15]);
    return(myData[0x14] | myData[0x15]<<8 );
}


typedef BOOL (CALLBACK * PROCTYPE_Toolhelp32ReadProcessMemory)( DWORD, LPCVOID, LPVOID, DWORD ,LPDWORD);

/***************************************************************************************/
int GetAddressLptPortInTheMemory(int myPort)
{
    HINSTANCE hDLL=NULL; // Handle to DLL
    PROCTYPE_Toolhelp32ReadProcessMemory myProcPointer=NULL;
    
    hDLL = LoadLibrary(_T("kernel32"));
    if (hDLL==NULL) return(-1);
    
    myProcPointer=(PROCTYPE_Toolhelp32ReadProcessMemory)GetProcAddress(hDLL,"Toolhelp32ReadProcessMemory");
    if (myProcPointer==NULL) /*handle the error*/
    { 
        FreeLibrary(hDLL); 
        return -1;
    }
    
    int portAddresses[]={0,0,0,0,0};
    BOOL rtn=0;
    //DWORD cbLen=0;
    //rtn = Toolhelp32ReadProcessMemory 
    rtn = myProcPointer
        (0,
        (LPCVOID *) 0x408,
        portAddresses,
        8,
        NULL) ;
    
    FreeLibrary(hDLL); 
    
    if (rtn==0)
        return(-1);
    
    if (portAddresses[myPort-1]<=0)
        return(-1);
    
    if (portAddresses[myPort-1]>=0x1000)
        return(-1);
    
    return(portAddresses[myPort-1]);
}

bool IsCommonParallelAddress(int address)
{
   return (address == 0x278 || address == 0x378 || address == 0x3BC);
}

/***************************************************************************************/
int GetAddressLptPort(int myPort)
{
   if (myPort < 1)
   {
      return -1;
   }

   if (myPort > 3)
   {
      return -1;
   }
    
   // We try to find all 3 each time, so we know when we need to fall back
   // to manually trying port addresses.
   int lpt1 = -1;
   int lpt2 = -1;
   int lpt3 = -1;

   // The address of the port actual requested
   int requested = -1;

   if (TEST_WINDOWS_NT)
   {
      lpt1 = GetAddressLptPortInTheRegistry(1);
      lpt2 = GetAddressLptPortInTheRegistry(2);
      lpt3 = GetAddressLptPortInTheRegistry(3);

      requested = GetAddressLptPortInTheRegistry(myPort);
   }
   else
   {
      lpt1 = GetAddressLptPortInTheMemory(1);
      lpt2 = GetAddressLptPortInTheMemory(2);
      lpt3 = GetAddressLptPortInTheMemory(3);

      requested = GetAddressLptPortInTheMemory(myPort);
   }

   // If our requested port is not valid, we try to look it up using common address,
   if (!IsCommonParallelAddress(requested))
   {
      // Ensure its set to our standard for "invalid"
      requested = -1;

      // Only to a manual forced check if no other ports were found on the system
      // the standard way.
      if (!IsCommonParallelAddress(lpt1) && 
          !IsCommonParallelAddress(lpt2) &&
          !IsCommonParallelAddress(lpt3))
      {
         // If we still havnt found it, check common port addresses manually
         if (WinIoWrapper::Instance()->Initialize())
         {
            DWORD data = 0xFFF;

            // When manually looking for ports, we always just call address 0x278 "LPT1"
            if (myPort == 1 && WinIoWrapper::Instance()->GetPortVal(0x278, &data, 1) && ((data & 0xFFF) != 0xFFF))
            {
               requested = 0x278;
            }

            // When manually looking for ports, we always just call address 0x378 "LPT2"
            if (myPort == 2 && WinIoWrapper::Instance()->GetPortVal(0x378, &data, 1) && ((data & 0xFFF) != 0xFFF))
            {
               requested = 0x378;
            }

            // When manually looking for ports, we always just call address 0x3BC "LPT3"
            if (myPort == 3 && WinIoWrapper::Instance()->GetPortVal(0x3BC, &data, 1) && ((data & 0xFFF) != 0xFFF))
            {
               requested = 0x3BC;
            }
         }
      }
   }

   return requested;
}


static PortStatus ConvertErrorCodeToPortStatus(UINT errCode)
{
   switch(errCode)
   {
   case ERROR_SUCCESS:
      return PORT_AVAILABLE;
      break;
   case ERROR_FILE_NOT_FOUND:
   case ERROR_PATH_NOT_FOUND:
      return PORT_NOT_FOUND;
      break;
   case ERROR_ACCESS_DENIED:
   case ERROR_INVALID_HANDLE: // See Defect #367
      return PORT_IN_USE;
      break;
   default:
      return PORT_STATUS_UNKNOWN;
      break;
   }
}

PortStatus PortCheck::GetPortStatus(PortType nPort)
{
   ASSERT((nPort >= 0) && (nPort < NUM_OF_PORT_TYPES));
   PortStatus nStatus = PORT_STATUS_UNKNOWN;

   switch(nPort)
   {
   case LPT1:
       nStatus = GetPortStatus(_T("LPT1"));
      break;
   case LPT2:
       nStatus = GetPortStatus(_T("LPT2"));
      break;
   case LPT3:
       nStatus = GetPortStatus(_T("LPT3"));
      break;
   default: //COM1...COM256
      {
         CString oPortName;
         oPortName.Format(_T("COM%1d"),nPort);
         nStatus = GetPortStatus(oPortName);
      }
   }

   return nStatus;
}

PortStatus PortCheck::GetPortStatus(CString oPortName)
{
    if(!IsParallelPort(oPortName))
    {
        HANDLE h = CreateFile(oPortName,
            GENERIC_WRITE|GENERIC_READ,
            0,
            NULL,
            OPEN_EXISTING,
            0,		
            NULL
            );   
        
        m_nErrorCode = ::GetLastError();
        
        CloseHandle(h);      
    }
    else
    {
       if(GetPortAddress(oPortName) == -1)
       {
           m_nErrorCode = ERROR_FILE_NOT_FOUND;
       }
       else
       {
           m_nErrorCode = ERROR_SUCCESS;
       }
    }

    return ConvertErrorCodeToPortStatus(m_nErrorCode);
}

CString PortCheck::FormatError()
{
   PortStatus nStatus = ConvertErrorCodeToPortStatus(m_nErrorCode);
   CString errMsg = GetEnumString(nStatus);
   
   switch(nStatus)
   {
   case PORT_IN_USE:
      {
         // Give instruction on possible solution
         errMsg += "\r\n\r\nPossible solution:\r\n" \
            "Close all other programs that may access this port.";
      }
      break;
   case PORT_AVAILABLE:
      {
         // No extra data needed here...
      }
      break;
   case PORT_NOT_FOUND:
      {
         // Check spelling... etc...
         errMsg += "\r\n\r\nPossible solutions:\r\n" \
            "Ensure port name is correct.\r\n" \
            "Ensure port is installed correctly.";
      }
      break;
   case PORT_STATUS_UNKNOWN:
      {
         LPVOID lpMsgBuf;
         FormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            PortCheck::GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR) &lpMsgBuf,
            0,
            NULL 
            );
         CString msgId;
         msgId.Format(_T("%d"),PortCheck::GetLastError());
         errMsg += "\r\n\r\nSystem error message (" + msgId + "):\r\n" + CString((LPCTSTR)lpMsgBuf);
         
         LocalFree(lpMsgBuf);
      }
      break;
   }

   return errMsg;
}

void PortCheck::GetAvailablePorts(CStringArray &oPorts, bool bCOMonly /* = true */, bool bLPTonly /* = false */, void (*pSendStatusString)(CString) /*= NULL*/)
{
   oPorts.RemoveAll();

   // Load strings from available ports
   for(int nPort = 0;nPort < NUM_OF_PORT_TYPES;nPort++)
   {
      // Skip non-com ports if so desired
      if(bCOMonly && !IsSerialPort((PortType)nPort)) continue;
      if(bLPTonly && !IsParallelPort((PortType)nPort)) continue;

	  CString strPort = GetEnumString((PortType)nPort);

	  if(pSendStatusString != NULL)
	  {
		  CString msg;
		  msg.Format(_T("Checking for %s . . . "),strPort);
		  (*pSendStatusString)(msg);
	  }

	  PortStatus nStatus = GetPortStatus((PortType)nPort);
	  switch(nStatus)
	  {
	  case PORT_IN_USE:
	  case PORT_AVAILABLE:
	  case PORT_STATUS_UNKNOWN:
		  {
			  oPorts.Add(strPort);

			  if(pSendStatusString != NULL)
			  {
				  CString msg;
				  msg.Format(_T("%s found."),strPort);
				  (*pSendStatusString)(msg);
			  }
		  }
		  break;
	  case PORT_NOT_FOUND:
		  if(pSendStatusString != NULL)
		  {
			  CString msg;
			  msg.Format(_T("%s not found."),strPort);
			  (*pSendStatusString)(msg);
		  }
         break;
      }
   }
}

bool PortCheck::IsSerialPort(CString oPortName)
{
   // Get name, first three chars
   CString portName = oPortName.Left(3);
   portName.MakeUpper();

   // Get num, last char
   int portNum = _ttoi(oPortName.Right(1).GetBuffer(0));

   // Serial if in form "COM#" where # is 1,2,3,4,5,6,7,8,9
   return  (oPortName.GetLength() == 4) && (portName == _T("COM")) && (portNum > 0);
}

bool PortCheck::IsSerialPort(PortType nPort)
{
   return (nPort >= COM_START) && (nPort <= COM_END);
}

bool PortCheck::IsParallelPort(PortType nPort)
{
   return (nPort == LPT1 ||
           nPort == LPT2 ||
           nPort == LPT3);
}

bool PortCheck::IsParallelPort(CString oPortName)
{
   // Get name, first three chars
   CString portName = oPortName.Left(3);
   portName.MakeUpper();

   // Get num, last char
   int portNum = _ttoi(oPortName.Right(1).GetBuffer(0));

   // Serial if in form "COM#" where # is 1,2,3
   return  (oPortName.GetLength() == 4) && (portName == _T("LPT") && (portNum > 0) && (portNum < 4));
}

int PortCheck::GetPortAddress(CString oPortName)
{
    int retVal = -1;

    if (IsParallelPort(oPortName))
    {
        int portNum = _ttoi(oPortName.Right(1).GetBuffer(0));
        retVal = GetAddressLptPort(portNum);
    }

    return retVal;
}

int PortCheck::GetPortAddress(PortType nPort)
{
    return GetPortAddress(GetEnumString(nPort));
}

UINT PortCheck::m_nErrorCode = PORT_AVAILABLE;

