#pragma once

class ChipRamHelper
{
public:
   ChipRamHelper();

   void Clear();
   void FillRamBanks(const BYTE* pData, int dataSize);

   BYTE* GetConfigurationData(int* pDataSize);
   BYTE  ChipRam[24][32];

private:
   void LoadDataBlock(const BYTE* pDataBlock, int& dataByte);
   void AdvanceRamPosition(BYTE& bank, BYTE& byteNum);

   void BuildConfiguration();

   BYTE m_deviceID;
   BYTE m_control;

   BYTE m_config[591];
};
