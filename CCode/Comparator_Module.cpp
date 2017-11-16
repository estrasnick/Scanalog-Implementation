#include "stdafx.h"
#include "BaseModule.h"
#include "Comparator_Module.h"
#include <sstream>

Comparator_Module::Comparator_Module(
	std::string moduleName,
	std::string simpleName,
	std::vector<ModuleInput*> inputs,
	std::vector<ModuleOutput*> outputs,
	std::map<std::string, std::string> params) :
	BaseModule(CAM_COMPARATOR, "comparator", moduleName, simpleName, inputs, outputs, params)
{
	if (Inputs.size() == 0)
	{
		Inputs.push_back(new ModuleInput { new std::vector<BaseModule*>(), COMPARATOR_POSITIVE_INPUT_NAME });
		Inputs.push_back(new ModuleInput{ new std::vector<BaseModule*>(), COMPARATOR_NEGATIVE_INPUT_NAME });
	}
	if (Outputs.size() == 0)
	{
		Outputs.push_back(new ModuleOutput{ new std::vector<BaseModule*>(), DEFAULT_OUTPUT_NAME });
	}

	BaseModule::NumCapacitors = 0;
	BaseModule::NumOpAmps = 0;
	BaseModule::NumComparators = 1;

	SetParam_ComparatorMode(DEFAULT_COMPARATOR_MODE);
	SetParam_InputSamplingPhase(DEFAULT_INPUT_SAMPLING_PHASE);
	SetParam_OutputPolarity(DEFAULT_OUTPUT_POLARITY);
	SetParam_Hysteresis(DEFAULT_HYSTERESIS);
}

const type_info& Comparator_Module::GetDerivedClass()
{
	return typeid(this);
}

std::string Comparator_Module::GetModuleName()
{
	return BaseModule::GetModuleName();
}

std::string Comparator_Module::GetSimpleName()
{
	return BaseModule::GetSimpleName();
}

std::string Comparator_Module::GetModuleInformation()
{
	std::stringstream s;
	s << "(mode=";
	s << GetParamValue("mode");
	s << ", phase=";
	s << GetParamValue("inputsamplingphase");
	s << ", polarity=";
	s << GetParamValue("outputpolarity");
	s << ", hysteresis=";
	s << GetParamValue("hysteresis");
	s << ", vref=";
	s << GetParamValue("vref");
	s << ")";
	return s.str();
}

void Comparator_Module::SetResourceConnections(uint8_t whichCab)
{
	uint8_t index;
	bool found = false;

	// Assign comparator
	for (index = 0; index < COMPARATORS_PER_BANK; index++)
	{
		if (Configurator::CAB_Array[whichCab].Comparator_Modules[index] == 0)
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		throw std::exception("Could not find an open comparator in CAB " + whichCab);
	}
	else
	{
		// Note that the op amp belongs to this module
		Configurator::CAB_Array[whichCab].Comparator_Modules[index] = this;

		Configurator::CAB_Array[whichCab].Comparator_Mode = &ComparatorMode;
		Configurator::CAB_Array[whichCab].Comparator_InputSamplingPhase = &InputSamplingPhase;
		Configurator::CAB_Array[whichCab].Comparator_OutputPolarity = &OutputPolarity;
		Configurator::CAB_Array[whichCab].Comparator_Hysteresis = &Hysteresis;

		// Record the output of this module, so that downstream modules can reference it
		CurrentOutputSite = new ConnectionSite{
			DEFAULT,
			TYPE_COMPARATOR,
			whichCab,
			index,
			TYPE_OUT
		};

		// Remember which op amp this is
		Comparator = index;
	}

	// If needed (i.e. variable reference mode), assign capacitors
	if (ComparatorMode == COMPARATOR_MODE_VARIABLE_REFERENCE)
	{
		// Set Vref_Numerator
		// Note that comparators require the last two capacitors to be free in order to work.
		// Op amps should be counting from the other direction, so if these aren't free, we 
		//   don't have enough resources anyway
		index = CAPACITORS_PER_BANK - 1;
		if (Configurator::CAB_Array[whichCab].Capacitor_Modules[index] != 0)
		{
			throw std::exception("Could not find an open Capacitor in CAB " + whichCab);
		}
		else
		{
			// Note that the capacitor belongs to this module
			Configurator::CAB_Array[whichCab].Capacitor_Modules[index] = this;

			// Set connection information
			if (GetInputSiteByName(COMPARATOR_POSITIVE_INPUT_NAME)->input->size() > 0)
			{
				Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = BaseModule::GetDefaultInputSite()->input->front()->GetCurrentOutputSite();
			}
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_COMPARATOR, whichCab, Comparator, TYPE_IN_POS };

			// Set phase information
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = InputSamplingPhase;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = InputSamplingPhase;

			// Remember which capacitor
			Cap_Vref_Numerator = index;
		}

		// Set Vref_Denominator
		// Note that comparators require the last two capacitors to be free in order to work.
		// Op amps should be counting from the other direction, so if these aren't free, we 
		//   don't have enough resources anyway
		index = CAPACITORS_PER_BANK - 2;
		if (Configurator::CAB_Array[whichCab].Capacitor_Modules[index] != 0)
		{
			throw std::exception("Could not find an open Capacitor in CAB " + whichCab);
		}
		else
		{
			// Note that the capacitor belongs to this module
			Configurator::CAB_Array[whichCab].Capacitor_Modules[index] = this;

			// Set connection information

			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][0] = new ConnectionSite{ (Vref >= 0.0) ? VOLTAGE_REFERENCE_POSITIVE : VOLTAGE_REFERENCE_NEGATIVE, TYPE_VOLTAGE_REFERENCE, CABVREF, (Vref >= 0.0) ? (uint8_t) RESOURCE_1 : (uint8_t) RESOURCE_2, TYPE_OUT};
			Configurator::CAB_Array[whichCab].Capacitor_Connections[index][1] = new ConnectionSite{ DEFAULT, TYPE_COMPARATOR, whichCab, Comparator, TYPE_IN_POS };

			// Set phase information
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][0] = (InputSamplingPhase == PHASE_1) ? PHASE_2 : PHASE_1;
			Configurator::CAB_Array[whichCab].Capacitor_Phasing[index][1] = InputSamplingPhase;

			// Remember which capacitor
			Cap_Vref_Denominator = index;
		}

		// Set capacitor values here
		Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_Vref_Numerator] = &VrefNumerator;
		Configurator::CAB_Array[whichCab].Capacitor_Values[Cap_Vref_Denominator] = &VrefDenominator;
	}

	BaseModule::SetResourceConnections(whichCab);
}

void Comparator_Module::SetParamValue(std::string key, std::string value)
{
	if (key.compare("mode") == 0)
	{
		uint8_t valueInt = atoi(value.c_str());
		if ((valueInt == COMPARATOR_MODE_DUAL) || (valueInt == COMPARATOR_MODE_GROUND) || (valueInt == COMPARATOR_MODE_VARIABLE_REFERENCE) || (valueInt == COMPARATOR_MODE_UNUSED) || (valueInt == COMPARATOR_MODE_ZERO_CROSSING))
		{
			SetParam_ComparatorMode(valueInt);
		}
		else
		{
			Message::ErrorMessage("Parameter " + key + " cannot have value " + value + " in module " + GetModuleName());
		}
	}
	else if (key.compare("inputsamplingphase") == 0)
	{
		uint8_t valueInt = atoi(value.c_str());
		if ((valueInt == PHASE_ALL) || (valueInt == PHASE_1) || (valueInt == PHASE_2))
		{
			SetParam_InputSamplingPhase(valueInt);
		}
		else
		{
			Message::ErrorMessage("Parameter " + key + " cannot have value " + value + " in module " + GetModuleName());
		}
	}
	else if (key.compare("outputpolarity") == 0)
	{
		uint8_t valueInt = atoi(value.c_str());
		if ((valueInt == OUTPUT_POLARITY_INV) || (valueInt == OUTPUT_POLARITY_NONINV))
		{
			SetParam_OutputPolarity(valueInt);
		}
		else
		{
			Message::ErrorMessage("Parameter " + key + " cannot have value " + value + " in module " + GetModuleName());
		}
	}
	else if (key.compare("hysteresis") == 0)
	{
		uint8_t valueInt = atoi(value.c_str());
		if ((valueInt == HYSTERESIS_0MV) || (valueInt == HYSTERESIS_10MV))
		{
			SetParam_Hysteresis(valueInt);
		}
		else
		{
			Message::ErrorMessage("Parameter " + key + " cannot have value " + value + " in module " + GetModuleName());
		}
	}
	else if (key.compare("vref") == 0)
	{
		double valueDouble = atof(value.c_str());
		if ((valueDouble >= MIN_VREF) && (valueDouble <= MAX_VREF))
		{
			SetParam_Vref(valueDouble);
		}
		else
		{
			Message::ErrorMessage("Parameter " + key + " cannot have value " + value + " in module " + GetModuleName());
		}
	}
	else
	{
		Message::ErrorMessage("Unknown parameter for module " + GetModuleName() + ": " + key);
	}
	BaseModule::SetParamValue(key, value);
}

std::string Comparator_Module::GetParamValue(std::string key)
{
	return Params[key];
}

void Comparator_Module::SetParam_ComparatorMode(uint8_t comparatorMode)
{
	if (comparatorMode == COMPARATOR_MODE_VARIABLE_REFERENCE)
	{
		BaseModule::NumCapacitors = 2;
	}
	else
	{
		BaseModule::NumCapacitors = 0;
	}
	ComparatorMode = comparatorMode;
}

void Comparator_Module::SetParam_InputSamplingPhase(uint8_t phase)
{
	InputSamplingPhase = phase;
}

void Comparator_Module::SetParam_OutputPolarity(uint8_t polarity)
{
	OutputPolarity = polarity;
}

void Comparator_Module::SetParam_Hysteresis(uint8_t hysteresis)
{
	Hysteresis = hysteresis;
}

void Comparator_Module::SetParam_Vref(double vref)
{
	Vref = vref;

	an_ChooseCapRatio(2.0 / std::abs(vref), &VrefNumerator, &VrefDenominator);
}
