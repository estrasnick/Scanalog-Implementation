#include "StdAfx.h"
#include "chipramhelper.h"

ChipRamHelper::ChipRamHelper()
{
   Clear();
}

void ChipRamHelper::Clear()
{
   // Zero out the chip data
   for (int i = 0; i < 24; i++)
   {
      for (int j = 0; j < 32; j++)
      {
         ChipRam[i][j] = 0;
      }
   }
}

void ChipRamHelper::FillRamBanks(const BYTE* pData, int dataSize)
{
   Clear();

   m_deviceID = 0;
   m_control = 0;

   // Represents the byte we are currently looking at in the passed in configuration.
   int dataByte = 0;

      // Keep going until we find the synch
   while (dataByte < dataSize && pData[dataByte] != 0xD5)
   {
      dataByte++;
   }

   // Now pump the sycnh and JTAG's out of the stream
   pData[dataByte++];
   pData[dataByte++];
   pData[dataByte++];
   pData[dataByte++];
   pData[dataByte++];

   // Now we get the device id and control byte
   m_deviceID = pData[dataByte++];
   m_control = pData[dataByte++];

   while (dataByte < dataSize)
   {
      LoadDataBlock(pData, dataByte);
   }
}

BYTE* ChipRamHelper::GetConfigurationData(int* pDataSize)
{
   BuildConfiguration();

   *pDataSize = 591;

   return m_config;
}

void ChipRamHelper::LoadDataBlock(const BYTE* pData, int& dataByte)
{
   BYTE byteNum = pData[dataByte++];
   BYTE bank    = pData[dataByte++];

   int byteCount = pData[dataByte++];
   byteCount = (byteCount == 0) ? 256 : byteCount;

   bool bCRC16    = (byteNum & 0x20) != FALSE;

   // Take off the control bits
   byteNum &= 0x1F;

   // Load in all the bytes to this RAM location.
   for (int i = 0; i < byteCount; i++)
   {
      // Set the value
      ChipRam[bank][byteNum] = pData[dataByte++];

      // And increment the bank \ byteNum to the next spot
      AdvanceRamPosition(bank, byteNum);
   }

   // Eat the error bytes
   pData[dataByte++];
   if (bCRC16) pData[dataByte++];
}

void ChipRamHelper::AdvanceRamPosition(BYTE& bank, BYTE& byteNum)
{
   if (byteNum == 31)
   {
      byteNum = 0;
      bank++;
   }
   else
   {
      byteNum++;
   }
}

void ChipRamHelper::BuildConfiguration()
{
   int nBank;

   // Zero it out first
   for (int i = 0; i < sizeof(m_config); i++)
   {
      m_config[i] = 0;
   }

   int dataByte = 0;

   //------------------------------
   // Header
   //------------------------------
   m_config[dataByte++] = 0xD5;
   m_config[dataByte++] = m_deviceID;
   m_config[dataByte++] = m_control;

   //------------------------------
   // Data block for auxiliary banks
   //------------------------------
   m_config[dataByte++] = 0xC0;
   m_config[dataByte++] = 0x00;
   m_config[dataByte++] = 64;

   for (nBank = 0; nBank <= 1; nBank++)
   {
      for (int nByteNum = 0; nByteNum < 32; nByteNum++)
      {
         m_config[dataByte++] = ChipRam[nBank][nByteNum];
      }
   }
   
   m_config[dataByte++] = 0x2A;

   //------------------------------
   // Data block for CAB banks
   //------------------------------
   m_config[dataByte++] = 0xC0;
   m_config[dataByte++] = 0x02;
   m_config[dataByte++] = 0;

   for (nBank = 2; nBank <= 9; nBank++)
   {
      for (int nByteNum = 0; nByteNum < 32; nByteNum++)
      {
         m_config[dataByte++] = ChipRam[nBank][nByteNum];
      }
   }
   
   m_config[dataByte++] = 0x2A;

   //------------------------------
   // Data block for LUT banks
   //------------------------------
   m_config[dataByte++] = 0x80;
   m_config[dataByte++] = 0x10;
   m_config[dataByte++] = 0;

   for (nBank = 16; nBank <= 23; nBank++)
   {
      for (int nByteNum = 0; nByteNum < 32; nByteNum++)
      {
         m_config[dataByte++] = ChipRam[nBank][nByteNum];
      }
   }
   
   m_config[dataByte++] = 0x2A;
}
