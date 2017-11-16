#include "stdafx.h"
#include "SerialPort.h"
#include "PortCheck.h"
#include "PortSelectDlg.h"
#include "Registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const TCHAR STX = '\02';      //Start Transmission
const TCHAR ETX = '\03';      //End Transmission
const TCHAR XON = '\17';      //Signal XON
const TCHAR STATUS = '?';     //Report status of last operation
const TCHAR VERSION = '#';    //Report Version Information
const TCHAR ADC = 'V';        //Return ADC channel data
const TCHAR CHIPTYPES = 'T';  //Report types of chips on board
const TCHAR RESET = 'R';      //Board Reset
const TCHAR Fchar = 'F';      //Character F - "RFF" resets all devices

SerialPort::SerialPort() :
   m_portHandle(NULL)
{
	m_restoreState.DCBlength = sizeof(DCB);

	// Load all default settings here
	m_oSettings.SetAt(_T("PORTNAME"),_T("COM1"));
	m_oSettings.SetAt(_T("BAUDRATE"),_T("57600"));

   m_useAsyncIO = TRUE;
}

SerialPort::~SerialPort()
{
   Close();
}

bool SerialPort::Open()
{
	CString strComPort;
	CString strBaudRate;
	GetSetting("PORTNAME",strComPort);
	GetSetting("BAUDRATE",strBaudRate);
	int nBaudRate = _ttoi(strBaudRate.GetBuffer(0));

	// If we are already open, then first we must close
	if (IsOpen())
	{
		Close();
	}

	// First check port's status...
	if (PortCheck::GetPortStatus(strComPort) != PORT_AVAILABLE)
	{
		m_strLastError = PortCheck::FormatError();
		return false;
	}

   if (m_useAsyncIO)
   {
      m_portHandle = CreateFile(
         strComPort,
         GENERIC_WRITE | GENERIC_READ,
         0,
         NULL,
         OPEN_EXISTING,
         FILE_FLAG_OVERLAPPED,
         NULL
         );
   }
   else
   {
      m_portHandle = CreateFile(
         strComPort,
         GENERIC_WRITE|GENERIC_READ,
         0,
         NULL,
         OPEN_EXISTING,
         FILE_ATTRIBUTE_NORMAL,
         NULL
         );
   }

	// We couldnt open it for some reason
	if (m_portHandle == INVALID_HANDLE_VALUE)
	{
		m_portHandle = NULL;
		m_strLastError = GetLastErrorString();
		return false;
	}

	// Setup the communication channel
	if (!SetupComm(m_portHandle, 1200, 1200))
	{
		CloseHandle(m_portHandle);
		m_portHandle = NULL;
		m_strLastError = GetLastErrorString();
		return false;
	}

	// Setup the DCB state
	if (!SetupCommState(nBaudRate))
	{
		CloseHandle(m_portHandle);
		m_portHandle = NULL;
		m_strLastError = GetLastErrorString();
		return false;
	}

	// Setup the timeouts
	if (!SetupCommTimeout())
	{
		SetCommState(m_portHandle, &m_restoreState);
		CloseHandle(m_portHandle);
		m_portHandle = NULL;
		m_strLastError = GetLastErrorString();
		return false;
	}

	DWORD dwErrors = 0;
	COMSTAT comStatus;

   //*************************************************
   // Patch for Keyspan USB/Serial Cable...
       
   int theCount = 0;
   while (true)
   {
      DWORD modemStatus;

      BOOL result = ClearCommError(
         m_portHandle,     // handle to communications device
         &dwErrors,        // pointer to variable to receive error codes
         &comStatus        // pointer to buffer for communications status  
         );    

      result = GetCommModemStatus(
         m_portHandle,
         &modemStatus
         );

      // Keyspan USB/Serial cables require at least
      // a 21 mS delay in order to see CTS.

      //Break if we see the signal...
      if(!(comStatus.fCtsHold || !(modemStatus & MS_CTS_ON))) 
         break;

      // Loop some more to wait for hardware/software
      Sleep(5);

      if (theCount++ > 100) 
         break;  //5 * 100 = 500 milliseconds
   }

   //*************************************************

	if(comStatus.fCtsHold)
	{
		Close();
		m_strLastError = _T("Serial port waiting for CTS");
		return false;
	}

	m_strLastError = "Success";

	return true;
}

bool SerialPort::SetupCommState(int nBaudRate)
{
	// Save the state so we can get back to it later
	GetCommState(m_portHandle, &m_restoreState);

	// Get the current state, and tweak it to what we want
	DCB portState;
	portState.DCBlength = sizeof(DCB);
	GetCommState(m_portHandle, &portState);

	// Setup the port how we want it
	portState.BaudRate            = nBaudRate;				   	// Baud rate at which running       
	portState.fBinary             = TRUE;					   	// Binary Mode(skip EOF check)    
	portState.fParity             = FALSE;					   	// Enable parity checking          
	portState.fOutxCtsFlow        = TRUE;					      // CTS handshaking on output       
	portState.fOutxDsrFlow        = FALSE;       			   // DSR handshaking on output   
	portState.fDtrControl         = DTR_CONTROL_DISABLE;  	// DTR Flow control                
	portState.fDsrSensitivity     = FALSE;				         // DSR Sensitivity              
	portState.fTXContinueOnXoff   = FALSE;			            // Continue TX when Xoff sent 
	portState.fOutX               = TRUE;							// Enable output X-ON/X-OFF        
	portState.fInX                = TRUE;							// Enable input X-ON/X-OFF         
	portState.fErrorChar          = FALSE;       			   // Enable Err Replacement     
	portState.fRtsControl         = RTS_CONTROL_ENABLE;	// RTS Flow control                
	portState.fAbortOnError       = TRUE;				      	// Abort all reads and writes on Error 
	portState.ByteSize            = 8;				   			// Number of bits/byte, 4-8        
	portState.Parity              = NOPARITY;				   	// 0-4=None,Odd,Even,Mark,Space    
	portState.StopBits            = 0;						     	// 0,1,2 = 1, 1.5, 2               
	portState.XonChar             = 17;						      // Tx and Rx X-ON character     
	portState.XoffChar            = 19;					         // Tx and Rx X-OFF character     

	// Try to set the new state
	return SetCommState(m_portHandle, &portState) != FALSE;
}

bool SerialPort::SetupCommTimeout()
{
	// Save the timeouts so we can get back to it later
	GetCommTimeouts(m_portHandle,&m_restoreTimeouts);

	// Get the current timouts, and tweak it to what we want
	COMMTIMEOUTS portTimeouts;
	GetCommTimeouts(m_portHandle, &portTimeouts);

	portTimeouts.ReadIntervalTimeout          = 0;			//  0 seconds
	portTimeouts.ReadTotalTimeoutMultiplier   = 0;	      //  0 seconds
	portTimeouts.ReadTotalTimeoutConstant     = 1000;	   //  1 seconds
	portTimeouts.WriteTotalTimeoutMultiplier  = 0;	      //  0 seconds
	portTimeouts.WriteTotalTimeoutConstant    = 1000;	   //  1 second

	// Try to set the new timeouts)
	return SetCommTimeouts(m_portHandle, &portTimeouts) != FALSE;
}

void SerialPort::Close()
{
	if (IsOpen())
	{
		SetCommState(m_portHandle, &m_restoreState);
		SetCommTimeouts(m_portHandle, &m_restoreTimeouts);

		CloseHandle(m_portHandle);
		m_portHandle = NULL;
	}
}

bool SerialPort::IsOpen()
{
	return m_portHandle != NULL && m_portHandle != INVALID_HANDLE_VALUE;
}

CString SerialPort::GetLastErrorString()
{
	LPCSTR lpErrorBuffer;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		::GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpErrorBuffer,
		0,
		NULL 
		);

	CString errorMessage(lpErrorBuffer);

	LocalFree((void*) lpErrorBuffer);

	return errorMessage;
}

bool SerialPort::ReadADC(int channel, BYTE* dataBuffer, int bufferSize)
{

	// Assume success
	bool bRet(true);
	m_strLastError = "Success";

	if (!IsOpen())
	{
		m_strLastError = _T("Serial port is not open.");
		return false;
	}

	COMMTIMEOUTS portTimeouts;
	GetCommTimeouts(m_portHandle, &portTimeouts);
	portTimeouts.ReadTotalTimeoutConstant     = 200;	   //  .2 seconds
	portTimeouts.WriteTotalTimeoutConstant    = 200;	   //  .2 seconds
	SetCommTimeouts(m_portHandle, &portTimeouts);

	DWORD bytesWritten = 0;
	DWORD bytesRead = 0;
	BYTE channelByte = (BYTE) ('0' + channel);

	try
	{
		// Send start transmission character:
		//if(!WriteFile(m_portHandle, &STX, 1, &bytesWritten, 0))
		if(!WriteABuffer(m_portHandle, &STX, 1, &bytesWritten))
		{
			throw GetLastErrorString();
		}

		// ADC Request
		if (!WriteABuffer(m_portHandle, &ADC, 1, &bytesWritten))
		{
			throw GetLastErrorString();
		}

		// ADC Channel
		if (!WriteABuffer(m_portHandle, &channelByte, 1, &bytesWritten))
		{
			throw GetLastErrorString();
		}

		// End transmission
		if (!WriteABuffer(m_portHandle, &ETX, 1, &bytesWritten))
		{
			throw GetLastErrorString();
		}

		// Read ADC value
		if (!ReadABuffer(m_portHandle, (char *)dataBuffer, bufferSize, &bytesRead))
		{
			throw GetLastErrorString();
		}

		// If we did read something
		if (bytesRead == 0)
		{
			throw CString("Could not read from serial port.");
		}
	}
	catch (CString errorMessage)
	{
		m_strLastError = errorMessage;
		bRet = false;
	}

	// Make sure to set these back
	portTimeouts.ReadTotalTimeoutConstant = 1000;
	portTimeouts.WriteTotalTimeoutConstant = 10000;
	SetCommTimeouts(m_portHandle, &portTimeouts);

	return bRet;
}

bool SerialPort::GetBoardVersionInfo(CString& versionInfo)
{
	// Assume success
	bool bRet(true);
	m_strLastError = "Success";

	if (!IsOpen())
	{
		m_strLastError = _T("Serial port is not open.");
		return false;
	}

	COMMTIMEOUTS portTimeouts;
	GetCommTimeouts(m_portHandle, &portTimeouts);
	portTimeouts.ReadTotalTimeoutConstant     = 200;	   //  .2 seconds
	portTimeouts.WriteTotalTimeoutConstant    = 200;	   //  .2 seconds
	SetCommTimeouts(m_portHandle, &portTimeouts);

	const int inputBufferSize = 512;
	TCHAR inputBuffer[inputBufferSize];

	DWORD bytesWritten = 0;
	DWORD bytesRead = 0;

	try
	{
		// Send start transmission character:
		if(!WriteABuffer(m_portHandle, &STX, 1, &bytesWritten))
		{
			throw GetLastErrorString();
		}

		// Version Query
		if (!WriteABuffer(m_portHandle, &VERSION, 1, &bytesWritten))
		{
			throw GetLastErrorString();
		}

		// End transmission
		if (!WriteABuffer(m_portHandle, &ETX, 1, &bytesWritten))
		{
			throw GetLastErrorString();
		}

		// Read Version info
		if(!ReadABuffer(m_portHandle, (char *)&inputBuffer, inputBufferSize, &bytesRead))
		{
			throw GetLastErrorString();
		}

		// If we did read something, set the versionInfo string
		if (bytesRead == 0)
		{
			throw CString("Could not read from serial port.");
		}

		versionInfo = inputBuffer;
	}
	catch (CString errorMessage)
	{
		m_strLastError = errorMessage;
		bRet = false;
	}

	// Make sure to set these back
	portTimeouts.ReadTotalTimeoutConstant = 1000;
	portTimeouts.WriteTotalTimeoutConstant = 10000;
	SetCommTimeouts(m_portHandle, &portTimeouts);

	return bRet;
}

bool SerialPort::Configure(const BYTE* pBinaryData, int binarySize, Destination destination, bool verifyDownload)
{
	// Assume success
	bool bRet(true);
	m_strLastError = "Success";

	if (!IsOpen())
	{
		m_strLastError = _T("Serial port is not open.");
		return false;
	}

	// Turn XonXoff On
	DCB comState;
	comState.DCBlength = sizeof(DCB);
	GetCommState(m_portHandle, &comState);
	comState.fOutX = TRUE;
	comState.fInX  = TRUE;
	SetCommState(m_portHandle, &comState);

	// When we convert it will be twice as long
	// we use binarySize + 6 because ABK 1.0 requires 5 zero bytes at beginning
	// and 1 zero byte at end of data stream to make sure generated clock is 
	// present with the data
	//int hexSize = (binarySize + 6) * 2;
	int hexSize;				// added these lines
	if (destination == ChipPrimary)
		hexSize = (binarySize + 6) * 2;
	else
		hexSize = (binarySize + 1) * 2;					//don't add 5 dummys at start for reconfigs
	BYTE* pHexData = new BYTE[hexSize];		// this line is unchanged
	BinaryToAsciiHex(pBinaryData, pHexData, binarySize, destination == ChipPrimary);	//added destination to this function

	

	DWORD bytesWritten = 0;

	try
	{
		//Pulse DTR line:
		EscapeCommFunction(m_portHandle, CLRDTR);
		EscapeCommFunction(m_portHandle,	SETDTR);

		// Send start transmission character:
		if(!WriteABuffer(m_portHandle, &STX, 1, &bytesWritten))
		{
			throw GetLastErrorString();
		}

		// Send destination byte
		if(!WriteABuffer(m_portHandle, (BYTE*) &destination, 1, &bytesWritten))
		{
			throw GetLastErrorString();
		}

		//crs 10/3/2007
		if(destination == ChipReset)	//Dave added these lines
		{
			WriteABuffer(m_portHandle, &Fchar, 1, &bytesWritten);
			WriteABuffer(m_portHandle, &Fchar, 1, &bytesWritten);
		}
		//crs 10/3/2007




		// Write byte count.  One count for every ascii character.
		// This is for ABK program on the board according to ABK communication protocol
		TCHAR byteCount[5];
		_stprintf(byteCount, _T("%04X"), hexSize);

      if(!WriteABuffer(m_portHandle, byteCount, 4, &bytesWritten))
		{
			throw GetLastErrorString();
		}

      // Issues with certain USB boards needs delay at this point on primary configuration.
      // USB to Serial cables drag their feet on XOFF so
      // our buffers in ABK are overrun.  The following Sleep of
      // 50 milliSeconds immediately after the byteCount is sent to
      // the ABK allows the FPAA time to reset and be ready to accept
      // primary configuration data at high speed so that buffer overruns
      // within the ABK systems do not occur.  If this Sleep is omitted,
      // a buffer overrun will occur when using certain USB to Serial
      // converter cables.  (ABK has a 40 byte buffer...)
      if(destination == ChipPrimary)
		{
			Sleep(50);
		}

		int packetSize = 1000;
		int totalWritten = 0;

		// Send actual write of data bytes
		for (int packet = 0; packet < hexSize; packet += packetSize)
		{
			// Make sure we dont go over
			if ((hexSize - (int) totalWritten) < packetSize)
			{
				packetSize = (hexSize -(int) totalWritten);  
			}

			// If we have anything to write, then send it.
			if (packetSize != 0)
			{
				if(!WriteABuffer(m_portHandle, &pHexData[packet], packetSize, &bytesWritten))
				{
					throw GetLastErrorString();
				}

				


				if (((DWORD) packetSize) != bytesWritten)
				{
					CString sError;
					sError.Format(
						_T("Data not written successfully:\n") \
						_T("  Bytes sent:    %d\n") \
						_T("  Bytes written: %d"), packetSize, (int) bytesWritten);

					throw sError;
				}

				totalWritten += bytesWritten;
			}
		}

		// Send end of transmission
		WriteABuffer(m_portHandle, &ETX, 1, &bytesWritten);

		// Make sure everything went OK
		if (verifyDownload)
		{
			CheckConfigStatus();
		}
	}
	catch (CString errorMessage)
	{
		// Send end of transmission
		WriteABuffer(m_portHandle, &ETX, 1, &bytesWritten);

      // Issue reset to to board so that we can try
		// to handle configuration of remaining chips.
		WriteABuffer(m_portHandle, &RESET, 1, &bytesWritten);
		WriteABuffer(m_portHandle, &Fchar, 1, &bytesWritten);
		WriteABuffer(m_portHandle, &Fchar, 1, &bytesWritten);
		WriteABuffer(m_portHandle, &ETX, 1, &bytesWritten);

		m_strLastError = errorMessage;
		bRet = false;
	}

	// Turn XonXoff back off
	GetCommState(m_portHandle, &comState);
	comState.fOutX = FALSE;
	comState.fInX  = FALSE;
	SetCommState(m_portHandle, &comState);

	delete[] pHexData;
	return bRet;
}

void SerialPort::CheckConfigStatus()
{
   COMMTIMEOUTS portTimeouts;
   GetCommTimeouts(m_portHandle, &portTimeouts);
   portTimeouts.ReadTotalTimeoutConstant     = 100;	   //  .1 seconds
   portTimeouts.WriteTotalTimeoutConstant    = 100;	   //  .1 seconds
   SetCommTimeouts(m_portHandle, &portTimeouts);

   DWORD bytesWritten = 0;
   DWORD bytesRead = 0;

   TRACE(_T("\nChecking port status...\n"));

   try
   {
      //Request Config Status
      WriteABuffer(m_portHandle, &STX,    1, &bytesWritten);
      WriteABuffer(m_portHandle, &STATUS, 1, &bytesWritten);
      WriteABuffer(m_portHandle, &ETX,    1, &bytesWritten);


      // Try to read from the serial port
      char buffer[256];
      char readByte;

      if(!ReadABuffer(m_portHandle, buffer, 1, &bytesRead))
      {
         throw CString("Chip did not configure properly.  Could not read serial port.");
      }

      // We got some info back
      if(bytesRead > 0)
      {
         readByte = buffer[0];

         int nStatus = (int) readByte - '0';

         if (nStatus == 1 || nStatus == -1)
         {
            throw CString("Chip did not configure properly.  Could not read serial port.");
         }

         else if (nStatus == 2)
         {
            throw CString("Chip did not configure properly.  Checksum didn't match.");
         }

         else if (nStatus == 3)
         {
            throw CString("Chip did not configure properly.  Serial write timed out.");
         }

         // Make sure to set these back
         portTimeouts.ReadTotalTimeoutConstant = 1000;
         portTimeouts.WriteTotalTimeoutConstant = 10000;
         SetCommTimeouts(m_portHandle, &portTimeouts);

         return;
      }

      //bytesRead <= 0
      throw CString("Could not read serial line.  Chip download status unknown.");
   }
	catch (CString sError)
	{
		// Make sure to set these back
		portTimeouts.ReadTotalTimeoutConstant = 1000;
		portTimeouts.WriteTotalTimeoutConstant = 10000;
		SetCommTimeouts(m_portHandle, &portTimeouts);

		throw;
	}
}

void SerialPort::BinaryToAsciiHex(const BYTE* binaryData, BYTE* hexData, int binarySize, bool PrimaryConfig)
   
{
	int hexIndex = 0;
	char hexChars[3];

	// Add 5 zero bytes to beginning
	//for(int j=0; j < 5; j++){
	//	sprintf(hexChars,"%02X", 0);
	//	hexData[hexIndex++] = (BYTE) toupper(hexChars[0]);
	//	hexData[hexIndex++] = (BYTE) toupper(hexChars[1]);
	//}
	if (PrimaryConfig == true)			// added this condition - only add 5 dummys at start if doing primary config
	{
		// Add 5 zero bytes to beginning
		for(int j=0; j < 5; j++){
			sprintf(hexChars,"%02X", 0);
			hexData[hexIndex++] = (BYTE) toupper(hexChars[0]);
			hexData[hexIndex++] = (BYTE) toupper(hexChars[1]);
		}
	}


	// Convert each binary byte to two hex bytes
	for (int binaryIndex = 0; binaryIndex < binarySize; binaryIndex++)
	{
		BYTE byte = binaryData[binaryIndex];

		sprintf(hexChars, "%02X", byte);

		hexData[hexIndex++] = (BYTE) toupper(hexChars[0]);
		hexData[hexIndex++] = (BYTE) toupper(hexChars[1]);
	}

	// zero push byte
	sprintf(hexChars,"%02X", 0);
	hexData[hexIndex++] = (BYTE) toupper(hexChars[0]);
	hexData[hexIndex++] = (BYTE) toupper(hexChars[1]);
}


UINT SerialPort::SelectPortDlg(bool bEnableAdvancedSettings)
{
	UINT retVal = IDCANCEL;

	Close();

	PortSelectDlg oDlg;

	oDlg.m_bCOMonly = true;
	oDlg.m_bLPTonly = false;
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

void SerialPort::SetDialogSettings(PortSelectDlg & oDlg)
{
	GetSetting(_T("BAUDRATE"),oDlg.m_oBaudRate);
}

void SerialPort::GetDialogSettings(PortSelectDlg & oDlg)
{
	SetSetting(_T("BAUDRATE"),oDlg.m_oBaudRate);
}

bool SerialPort::SetSetting(const CString & strSetting,const CString & strValue)
{
	bool retVal = false;

	CString oldValue;
	if( m_oSettings.Lookup(strSetting,oldValue) )
	{
		retVal = true;
		m_oSettings.SetAt(strSetting,strValue);
	}
	else
	{
		m_strLastError.Format(_T("SetSetting - Unrecognized setting '%s'"),strSetting);
	}

	return retVal;   
}

bool SerialPort::GetSetting(const CString & strSetting,CString & strValue)
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

bool SerialPort::WriteSettings(CString regKey)
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

	if(!retVal) m_strLastError.Format(_T("Unable to write parallel port settings to registry key '%s'."),regKey);

	return retVal;   
}

bool SerialPort::ReadSettings(CString regKey)
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

	if(!retVal) m_strLastError.Format(_T("Unable to read parallel port settings from registry key '%s'."),regKey);

	return retVal;   
}

BOOL SerialPort::WriteABuffer(HANDLE f1HCA, 
                                     CString lpBuf, 
                                     DWORD dwToWrite,
                                     DWORD *written)
{
   DWORD dwWritten;
   BOOL fRes = TRUE;
   OVERLAPPED osWrite = {0};

   if(!m_useAsyncIO){
      WriteFile(f1HCA,lpBuf,dwToWrite,written,NULL);
   }else{

      // Create this write operation's OVERLAPPED structure's hEvent.
      osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

      if (osWrite.hEvent == NULL){
         // error creating overlapped event handle
         return FALSE;
      }

      // Issue write.
      if (!WriteFile(f1HCA, lpBuf, dwToWrite, &dwWritten, &osWrite)) {
         //OVERLAPPED osWrite = {0};
         if (::GetLastError() != ERROR_IO_PENDING) { 
            // WriteFile failed, but it isn't delayed. Report error and abort.
            MessageBox(NULL, "Serial write failed", "Serial Port error", MB_OK);
            fRes = FALSE;
         } else {
            // Write is pending.
            if (!GetOverlappedResult(f1HCA, &osWrite, &dwWritten, TRUE)){
               fRes = FALSE;
            }else{
               // Write operation completed successfully.
               fRes = TRUE;
            }
         }
      }else{
         // WriteFile completed immediately.
         fRes = TRUE;
      }

      CloseHandle(osWrite.hEvent);

      *written = dwWritten;
   }
   return fRes;
}


BOOL SerialPort::ReadABuffer(HANDLE f1HCA, 
                                    char *lpBuf,
                                    DWORD bytesToRead,
                                    DWORD *bytesRead)
{
   DWORD dwRead;
   lpBuf[0] = '\0';
   OVERLAPPED overLap;
   DWORD Internal;
   DWORD InternalHigh;
   int i;
   DWORD count = 0;
   OVERLAPPED osReader = {0};

   //Sleep(500);

   if(!m_useAsyncIO){
      ReadFile(f1HCA,lpBuf,bytesToRead,bytesRead,NULL);
   }else{

      strcpy(lpBuf,"XXXXX");

      // Read stuff until there is no more stuff.
      // Note:  15 gives 1.2 seconds total sleep time, i.e., we
      // give up after 1.2 seconds if we have not heard from the ABK.
      // Test show that 450 milliseconds is not quite long enough for a
      // board with daughter card.  Amost but not quite...
      for(i = 0; i<15; i++){  //15 gives 1.05 seconds total sleep time 
      //while(TRUE){
         // Create the overlapped event. Must be closed before exiting
         // to avoid a handle leak.

         overLap.Internal = (DWORD)&Internal;
         overLap.InternalHigh = (DWORD)&InternalHigh;
         overLap.Offset = (DWORD) 0;
         overLap.OffsetHigh = (DWORD) 0;
         osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

         if (osReader.hEvent == NULL)return FALSE;
         overLap.hEvent = osReader.hEvent;

         if (ReadFile(f1HCA, &lpBuf[count], bytesToRead, &dwRead, &overLap)) {
         //if (ReadFile(f1HCA, &lpBuf[count], 1, &dwRead, &overLap)) {
            // read completes immediately due to ReadIntervalTimeOut set to MAXDWORD
            count += dwRead;
            if(count){
               //The board transmits NULL-terminated strings...
               if(count>=bytesToRead || lpBuf[count-1] == '\0'){ 
                  break;
               }
            }
         }else{
            //We do get a failure with certain cables.  We have worked
            //around the problem with the patch below...
            //MessageBox(NULL,"Serial read failed", "Serial Port Problem",MB_OK);
            //CloseHandle(osReader.hEvent);
            //return FALSE;
         }

         //Strange patch - required for UART ports and certain
         //USB/serial cable ports...  For some reason, the 1 byte
         //read fails but actually does transfer the data!  The
         //ABK does not currently return an "X" so that is a safe
         //test for the moment...  - HCA
         if(bytesToRead == 1 && lpBuf[0] != 'X'){
            dwRead++;
            count = dwRead;
            break;
         }
         //Progressive sleeps to provide just enough time hopefully...
         CloseHandle(osReader.hEvent);
         Sleep(10*i);
      }

      *bytesRead = count;
   }

   return TRUE;
}
