#include "stdafx.h"
#include "BaseModule.h"
#include "GainInv_Module.h"
#include <sstream>


#define DEFAULT_GAIN 1.0

GainInv_Module::GainInv_Module(
	std::string moduleName,
	std::string simpleName,
	std::vector<ModuleInput*> inputs,
	std::vector<ModuleOutput*> outputs,
	std::map<std::string, std::string> params) :
	BaseModule(CAM_GAININV, "gaininv", moduleName, simpleName, inputs, outputs, params)
{
	if (Inputs.size() == 0)
	{
		Inputs.push_back(new ModuleInput{ new std::vector<BaseModule*>(), DEFAULT_INPUT_NAME });
	}
	if (Outputs.size() == 0)
	{
		Outputs.push_back(new ModuleOutput{ new std::vector<BaseModule*>(), DEFAULT_OUTPUT_NAME });
	}

	BaseModule::NumCapacitors = 4;
	BaseModule::NumOpAmps = 1;
	BaseModule::NumComparators = 0;

	SetParam_Gain(DEFAULT_GAIN);
}

const type_info& GainInv_Module::GetDerivedClass()
{
	return typeid(this);
}

std::string GainInv_Module::GetModuleName()
{
	return BaseModule::GetModuleName();
}

std::string GainInv_Module::GetSimpleName()
{
	return BaseModule::GetSimpleName();
}

std::string GainInv_Module::GetModuleInformation()
{
	std::stringstream s;
	s << "(";
	s << GetParamValue("gain");
	s << ")";
	return s.str();
}

void GainInv_Module::SetResourceConnections(uint8_t whichCab)
{
	uint8_t index;
	bool found = false;

	// Set OP Amp
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
	// Set aCin
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

		// Set connection information
		if (BaseModule::GetDefaultInputSite()->input->size() > 0)
		{
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = BaseModule::GetDefaultInputSite()->input->front()->GetCurrentOutputSite();
		}
		Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp, TYPE_IN };

		// Set phase information
		Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_ALL;
		Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_ALL;

		// Remember which capacitor
		Cap_aCin = index;
	}

	found = false;
	// Set Cin
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

		// Set connection information
		if (BaseModule::GetDefaultInputSite()->input->size() > 0)
		{
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = BaseModule::GetDefaultInputSite()->input->front()->GetCurrentOutputSite();
		}
		Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp, TYPE_IN };
			
		// Set phase information
		Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_1;
		Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_1;

		// Remember which capacitor
		Cap_Cin = index;
	}

	found = false;
	// Set aCout
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

		// Set connection information
		Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp, TYPE_OUT };
		Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp, TYPE_IN };

		// Set phase information
		Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_ALL;
		Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_ALL;

		// Remember which capacitor
		Cap_aCout = index;
	}

	found = false;
	// Set Cout
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

		// Set connection information
		Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp, TYPE_OUT };
		Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, whichCab, OpAmp, TYPE_IN };

		// Set phase information
		Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = PHASE_1;
		Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = PHASE_1;

		// Remember which capacitor
		Cap_Cout = index;
	}

	// Set capacitor values here
	Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_Cin] = &GainNumerator;
	Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_Cout] = &GainDenominator;
	Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_aCin] = &GainNumerator;
	Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_aCout] = &GainDenominator;

	BaseModule::SetResourceConnections(whichCab);
}

void GainInv_Module::SetParamValue(std::string key, std::string value)
{
	if (key.compare("gain") == 0)
	{
		SetParam_Gain(atof(value.c_str()));
	}
	else
	{
		Message::ErrorMessage("Unknown parameter for module " + GetModuleName() + ": " + key);
	}
	BaseModule::SetParamValue(key, value);
}

std::string GainInv_Module::GetParamValue(std::string key)
{
	return Params[key];
}

void GainInv_Module::SetParam_Gain(double gain)
{

	// Anadigm's code uses a heinously inefficient way to determine the two best integer values
	// to obtain a target cap ratio, but we reuse it here. Eventually this will use a
	// continued fractions method
	an_ChooseCapRatio(gain, &GainNumerator, &GainDenominator);
}


