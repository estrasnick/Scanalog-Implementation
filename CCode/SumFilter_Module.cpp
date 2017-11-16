#include "stdafx.h"
#include "BaseModule.h"
#include "SumFilter_Module.h"
#include <sstream>


SumFilter_Module::SumFilter_Module(
	std::string moduleName,
	std::string simpleName,
	std::vector<ModuleInput*> inputs,
	std::vector<ModuleOutput*> outputs,
	std::map<std::string, std::string> params) :
	BaseModule(CAM_SUMFILTER, "sumfilter", moduleName, simpleName, inputs, outputs, params)
{
	if (Inputs.size() == 0)
	{
		Inputs.push_back(new ModuleInput{ new std::vector<BaseModule*>(), INPUT_1_NAME });
		Inputs.push_back(new ModuleInput{ new std::vector<BaseModule*>(), INPUT_2_NAME });
	}
	if (Outputs.size() == 0)
	{
		Outputs.push_back(new ModuleOutput{ new std::vector<BaseModule*>(), DEFAULT_OUTPUT_NAME });
	}

	BaseModule::NumCapacitors = 4;
	BaseModule::NumOpAmps = 1;
	BaseModule::NumComparators = 0;

	SetParam_Input1Sign(OUTPUT_POLARITY_NONINV);
	SetParam_Input2Sign(OUTPUT_POLARITY_NONINV);
	SetParam_OutputChangesOn(DEFAULT_INPUT_SAMPLING_PHASE);
	SetParam_Gain1(DEFAULT_FILTER_GAIN);
	SetParam_Gain2(DEFAULT_FILTER_GAIN);
	SetParam_CornerFrequency(DEFAULT_SUMDIFF_CORNER_FREQUENCY);
}

const type_info& SumFilter_Module::GetDerivedClass()
{
	return typeid(this);
}

std::string SumFilter_Module::GetModuleName()
{
	return BaseModule::GetModuleName();
}

std::string SumFilter_Module::GetSimpleName()
{
	return BaseModule::GetSimpleName();
}

std::string SumFilter_Module::GetModuleInformation()
{
	std::stringstream s;
	s << "(";
	s << "input1sign=";
	s << GetParamValue("input1sign");
	s << ", input2sign=";
	s << GetParamValue("input2sign");
	s << ", outputchangeson=";
	s << GetParamValue("outputchangeson");
	s << ", gain1=";
	s << GetParamValue("gain1");
	s << ", gain2=";
	s << GetParamValue("gain2");
	s << ", cornerfrequency=";
	s << GetParamValue("cornerfrequency");
	s << ")";
	return s.str();
}

void SumFilter_Module::SetResourceConnections(uint8_t whichCab)
{
	uint8_t index;
	bool found = false;

	// Set OP Amp 1
	for (index = 0; index < OP_AMPS_PER_BANK; index++)
	{
		if (Configurator::CAB_Array[whichCab].OpAmp_Modules[index] == 0)
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		throw std::exception("Could not find an open OpAmp in CAB " + whichCab);
	}
	else
	{
		// Note that the op amp belongs to this module
		Configurator::CAB_Array[whichCab].OpAmp_Modules[index] = this;

		// Record the output of this module, so that downstream modules can reference it
		CurrentOutputSite = new ConnectionSite{
			DEFAULT,
			TYPE_OP_AMP,
			whichCab,
			index,
			TYPE_OUT
		};

		// Remember which op amp this is
		OpAmp = index;
	}

	found = false;
	// Set Cap_inA
	for (index = 0; index < CAPACITORS_PER_BANK; index++)
	{
		if (Configurator::CAB_Array[whichCab].Capacitor_Modules[index] == 0)
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		throw std::exception("Could not find an open Capacitor in CAB " + whichCab);
	}
	else
	{
		// Note that the capacitor belongs to this module
		Configurator::CAB_Array[whichCab].Capacitor_Modules[index] = this;

		// Set connection and phase information
		if (BaseModule::GetInputSiteByName(INPUT_1_NAME)->input->size() > 0)
		{
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = BaseModule::GetInputSiteByName(INPUT_1_NAME)->input->front()->GetCurrentOutputSite();
		}
		Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp, TYPE_IN };

		Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = (Input1Sign == OUTPUT_POLARITY_INV) ? OutputChangesOn : ((OutputChangesOn == PHASE_1) ? PHASE_2 : PHASE_1);
		Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = OutputChangesOn;

		// Remember which capacitor
		Cap_inA = index;
	}

	found = false;
	// Set Cap_inB
	for (index = 0; index < CAPACITORS_PER_BANK; index++)
	{
		if (Configurator::CAB_Array[whichCab].Capacitor_Modules[index] == 0)
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		throw std::exception("Could not find an open Capacitor in CAB " + whichCab);
	}
	else
	{
		// Note that the capacitor belongs to this module
		Configurator::CAB_Array[whichCab].Capacitor_Modules[index] = this;

		// Set connection and phase information
		if (BaseModule::GetInputSiteByName(INPUT_2_NAME)->input->size() > 0)
		{
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = BaseModule::GetInputSiteByName(INPUT_2_NAME)->input->front()->GetCurrentOutputSite();
		}
		Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp, TYPE_IN };

		Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = (Input2Sign == OUTPUT_POLARITY_INV) ? OutputChangesOn : ((OutputChangesOn == PHASE_1) ? PHASE_2 : PHASE_1);
		Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = OutputChangesOn;

		// Remember which capacitor
		Cap_inB = index;
	}

	found = false;
	// Set Cap_out
	for (index = 0; index < CAPACITORS_PER_BANK; index++)
	{
		if (Configurator::CAB_Array[whichCab].Capacitor_Modules[index] == 0)
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		throw std::exception("Could not find an open Capacitor in CAB " + whichCab);
	}
	else
	{
		// Note that the capacitor belongs to this module
		Configurator::CAB_Array[whichCab].Capacitor_Modules[index] = this;

		// Set connection and phase information
		Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp, TYPE_OUT };
		Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp, TYPE_IN };

		Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = OutputChangesOn;
		Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = OutputChangesOn;

		// Remember which capacitor
		Cap_out = index;
	}

	found = false;
	// Set Cap_int
	for (index = 0; index < CAPACITORS_PER_BANK; index++)
	{
		if (Configurator::CAB_Array[whichCab].Capacitor_Modules[index] == 0)
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		throw std::exception("Could not find an open Capacitor in CAB " + whichCab);
	}
	else
	{
		// Note that the capacitor belongs to this module
		Configurator::CAB_Array[whichCab].Capacitor_Modules[index] = this;

		// Set connection and phase information
		Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp, TYPE_OUT };
		Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp, TYPE_IN };

		Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_ALL;
		Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_ALL;

		// Remember which capacitor
		Cap_int = index;
	}

	// Set Clock depending on corner frequency
	if (CornerFrequency <= 5)
	{
		Configurator::CAB_Array[whichCab].Clock = CLOCK_SLOW;
	}
	else if (CornerFrequency <= 100)
	{
		Configurator::CAB_Array[whichCab].Clock = CLOCK_MEDIUM;
	}
	else
	{
		Configurator::CAB_Array[whichCab].Clock = CLOCK_FAST;
	}

	// Set capacitor values here
	SetCapValues(whichCab);
	
	Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_inA] = &Cap_inA_val;
	Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_inB] = &Cap_inB_val;
	Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_out] = &Cap_out_val;
	Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_int] = &Cap_int_val;

	BaseModule::SetResourceConnections(whichCab);
}

void SumFilter_Module::SetParamValue(std::string key, std::string value)
{
	if (key.compare("gain1") == 0)
	{
		SetParam_Gain1(atof(value.c_str()));
	}
	else if (key.compare("gain2") == 0)
	{
		SetParam_Gain2(atof(value.c_str()));
	}
	else if (key.compare("outputchangeson") == 0)
	{
		SetParam_OutputChangesOn(atoi(value.c_str()));
	}
	else if (key.compare("input1sign") == 0)
	{
		SetParam_Input1Sign(atoi(value.c_str()));
	}
	else if (key.compare("input2sign") == 0)
	{
		SetParam_Input2Sign(atoi(value.c_str()));
	}
	else if (key.compare("cornerfrequency") == 0)
	{
		SetParam_CornerFrequency(atof(value.c_str()));
	}
	else
	{
		Message::ErrorMessage("Unknown parameter for module " + GetModuleName() + ": " + key);
	}
	BaseModule::SetParamValue(key, value);
}

std::string SumFilter_Module::GetParamValue(std::string key)
{
	return Params[key];
}

void SumFilter_Module::SetParam_Input1Sign(uint8_t sign)
{
	Input1Sign = sign;
}
void SumFilter_Module::SetParam_Input2Sign(uint8_t sign)
{
	Input2Sign = sign;
}
void SumFilter_Module::SetParam_OutputChangesOn(uint8_t phase)
{
	OutputChangesOn = phase;
}
void SumFilter_Module::SetParam_Gain1(double gain)
{
	Gain1 = gain;
}
void SumFilter_Module::SetParam_Gain2(double gain)
{
	Gain2 = gain;
}
void SumFilter_Module::SetParam_CornerFrequency(double cornerFrequency)
{
	CornerFrequency = cornerFrequency;
}

void SumFilter_Module::SetCapValues(uint8_t whichCab)
{
	static const double an_Pi = 3.1415926535897931;

	int C1 = 1, C2 = 1, Cout = 1, Cint = 1;
	int dC1, dC2, dCout = 255, dCint;
	double err, bestErr = 999;
	long clocka = GET_FREQ_FOR_CLOCK(Configurator::CAB_Array[whichCab].Clock);//an_GetApexClockFrequency(cam, an_CAMClock_ClockA);
	double omega = 2.0 * clocka * tan(an_Pi*CornerFrequency*1000.0 / clocka);
	double aG1, aG2, aomega;

	while (dCout > 0)
	{
		dCint = an_AdjustCap(1.0*dCout*(clocka / omega - 0.5));
		dC1 = an_AdjustCap(Gain1*dCout);
		dC2 = an_AdjustCap(Gain2*dCout);

		aG1 = 1.0 * dC1 / dCout;
		aG2 = 1.0 * dC2 / dCout;
		aomega = 2.0*dCout*clocka / (2.0*dCint + dCout);

		err = aG1 / Gain1 + Gain1 / aG1 + aG2 / Gain2 + Gain2 / aG2 + aomega / omega + omega / aomega;
		if (err < bestErr)
		{
			bestErr = err;
			Cout = dCout;
			Cint = dCint;
			C2 = dC2;
			C1 = dC1;
		}
		dCout--;
	}

	Cap_inA_val = (uint8_t)C1;
	Cap_inB_val = (uint8_t)C2;
	Cap_out_val = (uint8_t)Cout;
	Cap_int_val = (uint8_t)Cint;
}