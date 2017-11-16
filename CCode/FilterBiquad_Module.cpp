#include "stdafx.h"
#include "BaseModule.h"
#include "FilterBiquad_Module.h"
#include <sstream>


FilterBiquad_Module::FilterBiquad_Module(
	std::string moduleName,
	std::string simpleName,
	std::vector<ModuleInput*> inputs,
	std::vector<ModuleOutput*> outputs,
	std::map<std::string, std::string> params) :
	BaseModule(CAM_FILTERBIQUAD, "filterbiquad", moduleName, simpleName, inputs, outputs, params)
{
	if (Inputs.size() == 0)
	{
		Inputs.push_back(new ModuleInput{ new std::vector<BaseModule*>(), DEFAULT_INPUT_NAME });
	}
	if (Outputs.size() == 0)
	{
		Outputs.push_back(new ModuleOutput{ new std::vector<BaseModule*>(), DEFAULT_OUTPUT_NAME });
	}

	BaseModule::NumCapacitors = 8;
	BaseModule::NumOpAmps = 2;
	BaseModule::NumComparators = 0;

	SetParam_FilterType(DEFAULT_FILTER_TYPE);
	SetParam_Gain(DEFAULT_FILTER_GAIN);
	SetParam_InputSamplingPhase(DEFAULT_INPUT_SAMPLING_PHASE);
	SetParam_OutputPolarity(DEFAULT_OUTPUT_POLARITY);
	SetParam_CornerFrequency(DEFAULT_FILTER_CORNER_FREQUENCY);
	SetParam_Q(DEFAULT_FILTER_Q);
}

const type_info& FilterBiquad_Module::GetDerivedClass()
{
	return typeid(this);
}

std::string FilterBiquad_Module::GetModuleName()
{
	return BaseModule::GetModuleName();
}

std::string FilterBiquad_Module::GetSimpleName()
{
	return BaseModule::GetSimpleName();
}

std::string FilterBiquad_Module::GetModuleInformation()
{
	std::stringstream s;
	s << "(";
	s << "filtertype=";
	s << GetParamValue("filtertype");
	s << ", cornerfrequency=";
	s << GetParamValue("cornerfrequency");
	s << ", gain=";
	s << GetParamValue("gain");
	s << ", q=";
	s << GetParamValue("q");
	s << ", inputsamplingphase=";
	s << GetParamValue("inputsamplingphase");
	s << ", outputpolarity=";
	s << GetParamValue("outputpolarity");
	s << ")";
	return s.str();
}

// Note: currently the configuration always configures in the type II topology (see anadigm module docs)
// If enabling type I topology, change connections for cap 4
void FilterBiquad_Module::SetResourceConnections(uint8_t whichCab)
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

		// Remember which op amp this is
		OpAmp1 = index;
	}

	// Set OP Amp 2
	found = false;
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
		OpAmp2 = index;
	}

	found = false;
	// Set Cap_1
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
		if (FilterType == FILTER_TYPE_HIPASS)
		{
			// In Hipass mode, cap 1 is used in parallel with Cap A
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp1, TYPE_IN };
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp1, TYPE_OUT };

			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_ALL;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_ALL;
		}
		else if (FilterType == FILTER_TYPE_LOWPASS)
		{
			if (BaseModule::GetDefaultInputSite()->input->size() > 0)
			{
				Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = BaseModule::GetDefaultInputSite()->input->front()->GetCurrentOutputSite();
			}
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp1, TYPE_IN };

			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = InputSamplingPhase;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = (OutputPolarity == OUTPUT_POLARITY_INV) ? InputSamplingPhase : ((InputSamplingPhase == PHASE_1) ? PHASE_2 : PHASE_1);
		}
		
		// Remember which capacitor
		Cap_1 = index;
	}

	found = false;
	// Set Cap_2
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
		if (FilterType == FILTER_TYPE_HIPASS)
		{
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp1, TYPE_IN };
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp2, TYPE_OUT };

			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_1;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_1;
		}
		else if (FilterType == FILTER_TYPE_LOWPASS)
		{
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp1, TYPE_IN };
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp2, TYPE_OUT };

			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = (OutputPolarity == OUTPUT_POLARITY_INV) ? InputSamplingPhase : ((InputSamplingPhase == PHASE_1) ? PHASE_2 : PHASE_1);
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = (OutputPolarity == OUTPUT_POLARITY_INV) ? InputSamplingPhase : ((InputSamplingPhase == PHASE_1) ? PHASE_2 : PHASE_1);
		}

		// Remember which capacitor
		Cap_2 = index;
	}

	found = false;
	// Set Cap_3
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
		if (FilterType == FILTER_TYPE_HIPASS)
		{
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp1, TYPE_OUT };
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp2, TYPE_IN };

			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_2;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_1;
		}
		else if (FilterType == FILTER_TYPE_LOWPASS)
		{
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp1, TYPE_OUT };
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp2, TYPE_IN };

			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = (OutputPolarity == OUTPUT_POLARITY_INV) ? ((InputSamplingPhase == PHASE_1) ? PHASE_2 : PHASE_1) : InputSamplingPhase;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = (OutputPolarity == OUTPUT_POLARITY_INV) ? InputSamplingPhase : ((InputSamplingPhase == PHASE_1) ? PHASE_2 : PHASE_1);
		}

		// Remember which capacitor
		Cap_3 = index;
	}

	found = false;
	// Set Cap_4
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
		if (FilterType == FILTER_TYPE_HIPASS)
		{
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp1, TYPE_IN };
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp2, TYPE_OUT };

			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_ALL;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_ALL;
		}
		else if (FilterType == FILTER_TYPE_LOWPASS)
		{
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp1, TYPE_IN };
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp2, TYPE_OUT };

			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_ALL;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_ALL;
		}

		// Remember which capacitor
		Cap_4 = index;
	}

	found = false;
	// Set Cap_A
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
		if (FilterType == FILTER_TYPE_HIPASS)
		{
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp1, TYPE_IN };
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp1, TYPE_OUT };

			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_ALL;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_ALL;
		}
		else if (FilterType == FILTER_TYPE_LOWPASS)
		{
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp1, TYPE_IN };
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp1, TYPE_OUT };

			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_ALL;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_ALL;
		}

		// Remember which capacitor
		Cap_A = index;
	}

	found = false;
	// Set Cap_B
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
		if (FilterType == FILTER_TYPE_HIPASS)
		{
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp2, TYPE_IN };
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp2, TYPE_OUT };

			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_ALL;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_ALL;
		}
		else if (FilterType == FILTER_TYPE_LOWPASS)
		{
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp2, TYPE_IN };
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp2, TYPE_OUT };

			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_ALL;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_ALL;
		}

		// Remember which capacitor
		Cap_B = index;
	}

	found = false;
	// Set Cap_p
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
		if (FilterType == FILTER_TYPE_HIPASS)
		{
			// In Hipass mode, cap p is used in parallel with cap B
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp2, TYPE_IN };
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp2, TYPE_OUT };

			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_ALL;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_ALL;
		}
		else if (FilterType == FILTER_TYPE_LOWPASS)
		{
			// In Lowpass mode, cap p is used in parallel with cap B
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp2, TYPE_IN };
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp2, TYPE_OUT };

			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_ALL;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_ALL;
		}

		// Remember which capacitor
		Cap_p = index;
	}

	found = false;
	// Set Cap_pp
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
		if (FilterType == FILTER_TYPE_HIPASS)
		{
			if (BaseModule::GetDefaultInputSite()->input->size() > 0)
			{
				Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = BaseModule::GetDefaultInputSite()->input->front()->GetCurrentOutputSite();
			}
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp2, TYPE_IN };

			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_ALL;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_ALL;
		}
		else if (FilterType == FILTER_TYPE_LOWPASS)
		{
			// In Lowpass mode, cap pp is used in parallel with cap A
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp1, TYPE_IN };
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp1, TYPE_OUT };

			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_ALL;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_ALL;
		}

		// Remember which capacitor
		Cap_pp = index;
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
	if (FilterType == FILTER_TYPE_HIPASS)
	{
		SetCapValues_Hipass(whichCab);
	}
	else if (FilterType == FILTER_TYPE_LOWPASS)
	{		
		SetCapValues_Lopass(whichCab);
	}
	Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_1] = &Cap_1_val;
	Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_2] = &Cap_2_val;
	Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_3] = &Cap_3_val;
	Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_4] = &Cap_4_val;
	Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_A] = &Cap_A_val; // NOTE: Anadigm's implementation splits A and B  
	Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_B] = &Cap_B_val; // in half, using two caps each. Use CA1 and CA2 here instead if splitting.
	Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_p] = &Cap_p_val;
	Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_pp] = &Cap_pp_val;

	BaseModule::SetResourceConnections(whichCab);
}

void FilterBiquad_Module::SetParamValue(std::string key, std::string value)
{
	if (key.compare("gain") == 0)
	{
		SetParam_Gain(atof(value.c_str()));
	}
	else if (key.compare("filtertype") == 0)
	{
		SetParam_FilterType(atoi(value.c_str()));
	}
	else if (key.compare("cornerfrequency") == 0)
	{
		SetParam_CornerFrequency(atof(value.c_str()));
	}
	else if (key.compare("q") == 0)
	{
		SetParam_Q(atof(value.c_str()));
	}
	else if (key.compare("inputsamplingphase") == 0)
	{
		SetParam_InputSamplingPhase(atoi(value.c_str()));
	}
	else if (key.compare("outputpolarity") == 0)
	{
		SetParam_OutputPolarity(atoi(value.c_str()));
	}
	else
	{
		Message::ErrorMessage("Unknown parameter for module " + GetModuleName() + ": " + key);
	}
	BaseModule::SetParamValue(key, value);
}

std::string FilterBiquad_Module::GetParamValue(std::string key)
{
	return Params[key];
}

void FilterBiquad_Module::SetParam_FilterType(uint8_t type)
{
	FilterType = type;
}
void FilterBiquad_Module::SetParam_InputSamplingPhase(uint8_t phase)
{
	InputSamplingPhase = phase;
}
void FilterBiquad_Module::SetParam_OutputPolarity(uint8_t polarity)
{
	OutputPolarity = polarity;
}
void FilterBiquad_Module::SetParam_Gain(double gain)
{
	Gain = gain;
}
void FilterBiquad_Module::SetParam_CornerFrequency(double cornerFrequency)
{
	CornerFrequency = cornerFrequency;
}
void FilterBiquad_Module::SetParam_Q(double q)
{
	Q = q;
}

void FilterBiquad_Module::SetCapValues_Hipass(uint8_t whichCab)
{
	static const double an_Pi = 3.1415926535897931;
	static const double an_TwoPi = 6.2831853071795862;
	static const int maxCA = 510;
	static const int maxCB = 510;

	int 	dCpp, dC2, dC3, dC4, dCA, dCB;
	int CA1, CA2, CB1, CB2;
	int	Qpp = 0, Q2 = 0, Q3 = 0, Q4 = 0, QA = 0, QB = 0;	//final high Q biquad cap values
	double	aFoa, aFoa2, aQ, aGhi;	//achieved parameter values
	long clocka = GET_FREQ_FOR_CLOCK(Configurator::CAB_Array[whichCab].Clock); //an_GetApexClockFrequency(cam, an_CAMClock_ClockA);
	double	FCdiv2PI = clocka / an_TwoPi;

	//Limits in this module are extremely complicated due to the interrelation of parameters

	double Ghi = Gain;

	//Prewarp Fo (kHz) into Foa (Hz)
	double 	Wo = 2.0*clocka*sin(an_Pi*CornerFrequency*1000.0 / clocka);
	double  Foa = Wo / an_TwoPi;

	// calculate normalized capacitor values
	double nC2 = Wo / clocka;

	double  err = 99999;
	double bestErr3 = 9999;
	double bestErr4 = 9999;

	//Second, calculate for high Q circuit
	dCB = maxCB;
	while (dCB>0)
	{
		dC3 = an_AdjustCap(nC2*dCB);
		dCpp = an_AdjustCap(Ghi * dCB);

		aFoa2 = FCdiv2PI*dC3 / dCB;
		aGhi = 1.0* dCpp / dCB;

		err = aFoa2 / Foa + Foa / aFoa2 + aGhi / Ghi + Ghi / aGhi;

		if (err < bestErr3)
		{
			bestErr3 = err;
			Q3 = dC3;
			Qpp = dCpp;
			QB = dCB;
		}
		dCB--;
	}

	err = 99999;
	dCA = maxCA;
	while (dCA>0)
	{
		dC2 = an_AdjustCap(nC2*nC2*QB*dCA / Q3);
		dC4 = an_AdjustCap(sqrt(1.0*dCA*QB*dC2 / Q3) / Q);

		aFoa = FCdiv2PI*sqrt(1.0*dC2*Q3 / dCA / QB);
		aQ = 1.0*sqrt(1.0*dCA*QB*dC2 / Q3) / dC4;

		err = aFoa / Foa + Foa / aFoa + aQ / Q + Q / aQ;
		if (err < bestErr4)
		{
			bestErr4 = err;
			Q2 = dC2;
			Q4 = dC4;
			QA = dCA;
		}
		dCA--;
	}

	CA1 = QA / 2;
	CA2 = QA - CA1;

	CB1 = QB / 2;
	CB2 = QB - CB1;


	Cap_1_val = (uint8_t)CA2;
	Cap_2_val = (uint8_t)Q2;
	Cap_3_val = (uint8_t)Q3;
	Cap_4_val = (uint8_t)Q4;
	Cap_A_val = (uint8_t)CA1; // NOTE: Anadigm's implementation splits A and B  
	Cap_B_val = (uint8_t)CB1; // in half, using two caps each. Use CA1 and CA2 here instead if splitting.
	Cap_p_val = (uint8_t)CB2;
	Cap_pp_val = (uint8_t)Qpp;
}

void FilterBiquad_Module::SetCapValues_Lopass(uint8_t whichCab)
{
	static const double an_Pi = 3.1415926535897931;
	static const double an_TwoPi = 6.2831853071795862;
	static const int maxCA = 510;
	static const int maxCB = 510;

	int 	dC1, dC2, dC3, dC4, dCA, dCB;
	int CA1, CA2, CB1, CB2;
	int	Q1 = 0, Q2 = 0, Q3 = 0, Q4 = 0, QA = 0, QB = 0;	//final high Q biquad cap values
	double	aFoa, aFoa2, aQ, aGlo;	//achieved parameter values

	long clocka = GET_FREQ_FOR_CLOCK(Configurator::CAB_Array[whichCab].Clock); // an_GetApexClockFrequency(cam, an_CAMClock_ClockA);
	double	FCdiv2PI = clocka / an_TwoPi;

	//Limits in this module are extremely complicated due to the interrelation of parameters

	double Glo = Gain;

	//Prewarp Fo (kHz) into Foa (Hz)
	double 	Wo = 2.0*clocka*sin(an_Pi*CornerFrequency*1000.0 / clocka);
	double  Foa = Wo / an_TwoPi;

	// calculate normalized capacitor values
	double nC2 = Wo / clocka;

	//Second, calculate for high Q circuit
	double  err = 99999;
	double bestErr3 = 9999;
	double bestErr4 = 9999;

	dCB = maxCB;
	while (dCB>0)
	{
		dC3 = an_AdjustCap(nC2*dCB);

		aFoa2 = FCdiv2PI*dC3 / dCB;

		err = aFoa2 / Foa + Foa / aFoa2;

		if (err < bestErr3)
		{
			bestErr3 = err;
			Q3 = dC3;
			QB = dCB;
		}
		dCB--;
	}

	err = 99999;
	dCA = maxCA;
	while (dCA>0)
	{
		dC2 = an_AdjustCap(nC2*nC2*QB*dCA / Q3);
		dC4 = an_AdjustCap(sqrt(1.0*dCA*QB*dC2 / Q3) / Q);
		dC1 = an_AdjustCap(Glo * dC2);

		aFoa = FCdiv2PI*sqrt(1.0*dC2*Q3 / dCA / QB);
		aQ = 1.0*sqrt(1.0*dCA*QB*dC2 / Q3) / dC4;
		aGlo = 1.0* dC1 / dC2;

		err = aFoa / Foa + Foa / aFoa + aQ / Q + Q / aQ + aGlo / Glo + Glo / aGlo;

		if (err < bestErr4)
		{
			bestErr4 = err;
			Q2 = dC2;
			Q4 = dC4;
			Q1 = dC1;
			QA = dCA;
		}
		dCA--;
	}

	CA1 = QA / 2;
	CA2 = QA - CA1;

	CB1 = QB / 2;
	CB2 = QB - CB1;

	Cap_1_val = (uint8_t)Q1;
	Cap_2_val = (uint8_t)Q2;
	Cap_3_val = (uint8_t)Q3;
	Cap_4_val = (uint8_t)Q4;
	Cap_A_val = (uint8_t)CA1; // NOTE: Anadigm's implementation splits A and B  
	Cap_B_val = (uint8_t)CB1; // in half, using two caps each. Use CA1 and CA2 here instead if splitting.
	Cap_p_val = (uint8_t)CB2;
	Cap_pp_val = (uint8_t)CA2;
}