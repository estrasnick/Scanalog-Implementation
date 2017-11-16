#pragma once

#include "RectifierHold.h"
#include "Gain_GainInv1.h"

class GeneralChip
{
public:
	GeneralChip();

   an_Byte GetDeviceID(int which);

   void ExecuteConfig(int which, bool reset);	// Dave Lovell added bool Reset
   void ExecuteReconfig(int which, bool x=false);
   void ExecuteReset();

   void AppendFullReconfig(int which);
   int GetCurrentActiveConfig();
   void SetCurrentActiveConfig(int which);

// Public CAM member variables for easy access to the chip's CAMs
public:
	RectifierHold RectifierHold1;
	Gain_GainInv1 Gain_GainInv1;

private:
   an_Chip m_instance[an_apiChipCount];
   int currentActiveConfig;
};
