#include "StdAfx.h"
#include "DownloadManager.h"

DownloadManager DownloadManager::m_instance;

DownloadManager* DownloadManager::Instance()
{
   return &m_instance;
}

DownloadManager::DownloadManager()
{
   m_throwOnError = false;

//[InitializeReconfigData]
   for (int i = 0; i < an_apiChipCount; i++)
   {
	   an_InitializeApexReconfigData(an_ChipIDs[i]);
   }
}

DownloadManager::~DownloadManager()
{
//[ShutdownReconfigData]
	for (int i = 0; i < an_apiChipCount; i++)
	{
		an_ShutdownApexReconfigData(an_ChipIDs[i]);
	}
}

void DownloadManager::DownloadBits(const BYTE* data, int dataSize, bool isReconfig)
{
   // Figure out the destination to send to the ABK
   PortBase::Destination dest = (isReconfig) ?  PortBase::ChipReconfig : PortBase::ChipPrimary;
   
   // Send the data
   if (!m_port.Configure(data, dataSize, dest, false)) 
   {
      HandleError(m_port.GetLastError());
	}
}

// Dave Lovell remmed this function out
//void DownloadManager::InitializeChips(const BYTE* loadOrder, int chipCount)
//{
//   static BYTE primaryConfig[] = 
//   {
      /* The header for the configuration stream */
//      0xD5,   /* Synch     */
//      0xB7,   /* JTAG1     */
//      0x20,   /* JTAG2  Dave Lovell changed this from 0x22   */
//      0x01,   /* JTAG3  Dave Lovell changed this from 0x00   */
//      0x00,   /* JTAG4  Dave Lovell changed this from 0x80   */
//      0x0 ,   /* Device ID */
//      0xC1 ,   /* Control  Dave Lovell changed this from 0x05 */

      /* Dummy Update Block */
//      0x80,   /* Byte Number */
//      0x0 ,   /* Bank */
//      0x1 ,   /* Byte count */

      /* Dummy Data */
//      0x0 ,

      /* Error Byte: Inverse Synch */
//      0x2A
	  // Note by Dave Lovell - changes above change config data from Vortex to Apex
//   };
/*
   bool issueReset = true;
   
   // Load each chip with the device id we want to target for it
   for (int i = 0; i < chipCount; i++)
   {
      primaryConfig[5] = loadOrder[i];

      bool configured = m_port.Configure(
         primaryConfig, 
         sizeof(primaryConfig),
         issueReset ? PortBase::ChipPrimary : PortBase::ChipReconfig,
         true);

      issueReset = false;

      if (!configured)
      {
         HandleError(m_port.GetLastError());
         break;
      }
   }
}*/


void DownloadManager::ResetChips()
{
   static BYTE resetConfig[] = 
   {
      /* The header for the configuration stream */
      0xD5,   /* Synch     */
      0xFF,   /* Device ID */
      0x6F,   /* Control  Dave Lovell changed this from 0x48 - this changes it from Vortex to BOTH*/

      /* Dummy Update Block */
      0x80,   /* Byte Number */
      0x0 ,   /* Bank */
      0x1 ,   /* Byte count */

      /* Dummy Data */
      0x0 ,

      /* Error Byte: Inverse Synch */
      0x2A
   };

   bool configured = m_port.Configure(
      resetConfig, 
      sizeof(resetConfig),
      //PortBase::ChipReconfig,			//crs 10/3/2007
	  PortBase::ChipReset,				//crs 10/3/2007
      false);

   if (!configured)
   {
      HandleError(m_port.GetLastError());
   }
}

bool DownloadManager::ShowPortDlg() 
{
   // Let the user setup the port
   if(m_port.SelectPortDlg(true) == IDOK)
   {
      if (m_port.IsOpen())
      {

      }
      else
      {
         HandleError(m_port.GetLastError());
      }
   }

   return m_port.IsOpen();
}

void DownloadManager::OpenPort()
{
   if (m_port.Open())
   {

   }
   else
   {
      HandleError(m_port.GetLastError());
   }
}

void DownloadManager::ClosePort()
{
   m_port.Close();
}

bool DownloadManager::IsPortOpen()
{
   return m_port.IsOpen();
}

void DownloadManager::HandleError(CString error)
{
   if (m_throwOnError)
   {
      throw error;
   }
   else
   {
      AfxGetMainWnd()->MessageBox(error, "FPAA_Application", MB_OK | MB_ICONERROR);
   }
}

void DownloadManager::LoadPortSettings(CString regKey)
{
   m_port.ReadSettings("Software\\" + regKey);

   // Attempt to open the port after reading the new settings
   m_port.Open();
}

void DownloadManager::SavePortSettings(CString regKey)
{
   m_port.WriteSettings("Software\\" + regKey);
}