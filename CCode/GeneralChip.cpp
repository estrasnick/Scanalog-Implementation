#include "stdafx.h"
#include "GeneralChip.h"
#include "DownloadManager.h"
#include "ChipRamHelper.h"
#include "Helpers.h"

GeneralChip::GeneralChip()
{
	for (int i = 0; i < an_apiChipCount; i++)
	{
		m_instance[i] = an_ChipIDs[i];
	}
};

an_Byte GeneralChip::GetDeviceID(int which)
{
   return an_apiAddress1[m_instance[which]];
}

void GeneralChip::ExecuteConfig(int which, bool reset)	// Dave Lovell added bool Reset
{
   int dataSize = 0;
	const an_Byte* data = an_GetPrimaryConfigData(m_instance[which], &dataSize);

   //DownloadManager::Instance()->DownloadBits(data, dataSize, false);
	DownloadManager::Instance()->DownloadBits(data, dataSize, reset);	// Dave Lovell replaced false with Reset
	currentActiveConfig = which;
}

void GeneralChip::ExecuteReconfig(int which, bool EndExecute)
{
   int dataSize = 0;
   const an_Byte* data = an_GetApexReconfigData(m_instance[which], &dataSize);

   
	//crs 10/3/2007 crs
   if (EndExecute)
	   an_SetApexReconfigControlFlags(m_instance[which], an_ApexControlByte_PUs_UpClks_Ex);
   else
	   an_SetApexReconfigControlFlags(m_instance[which], an_ApexControlByte_PUs_UpClks);
   //crs 10/3/2007 end

   LogHexData((an_Byte*) data, dataSize);

   DownloadManager::Instance()->DownloadBits(data, dataSize);

   // Clear the buffer now that its been sent
   an_ClearApexReconfigData(m_instance[which]);
   currentActiveConfig = which;
}

void GeneralChip::ExecuteReset()
{
//   int dataSize = 0;						//10/3/2007 comment out
//	const an_Byte* data = an_GetResetData(m_instance, &dataSize); //10/3/2007 comment out

   //DownloadManager::Instance()->DownloadBits(data, dataSize); //10/3/2007 comment out
   	DownloadManager::Instance()->ResetChips();    //crs 10/3/2007
}

void GeneralChip::AppendFullReconfig(int which)
{
   ChipRamHelper ramHelper;

   int dataSize = 0;
	const an_Byte* data = an_GetPrimaryConfigData(m_instance[which], &dataSize);

   // Load the chip configuration with all the data
   ramHelper.FillRamBanks(data, dataSize);

   // Now get the full reconfiguration stream, all zero's included
   int fullDataSize = 0;
   const an_Byte* fullData = ramHelper.GetConfigurationData(&fullDataSize);

   // Get the internal C Code reconfig structure
   an_apiReconfigInfo* pInfo = an_apiReconfigData[m_instance[which]];

   // Allocate memory in the reconfig stream for the full chip config
   pInfo->data = (an_Byte*) realloc(pInfo->data, fullDataSize);
   CopyMemory(pInfo->data, fullData, fullDataSize);

   // Manually update the internal reconfig info
   pInfo->capacity = (short) fullDataSize;
   pInfo->length = (short) fullDataSize;
   pInfo->lastUpdateIndex = 331;
   currentActiveConfig = which;
}

int GeneralChip::GetCurrentActiveConfig()
{
	return currentActiveConfig;
}

void GeneralChip::SetCurrentActiveConfig(int which)
{
	currentActiveConfig = which;
}

