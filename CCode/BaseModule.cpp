#include "stdafx.h"
#include "BaseModule.h"
#include "IO.h"

/* All modules inherit this BaseModule object */
BaseModule::BaseModule(
	an_CAM cam,
	std::string defaultModuleName,
	std::string moduleName,
	std::string simpleName,
	std::vector<ModuleInput*> inputs,
	std::vector<ModuleOutput*> outputs,
	std::map<std::string, std::string> params)
{
	Cam = cam;

	// If a name is specified, use that. Otherwise, use the module type plus a unique ID
	if (moduleName.compare("") != 0)
	{
		ModuleName = moduleName;
	}
	else
	{
		char buf[10];
		sprintf(buf, "%d", Configurator::GetNextModuleNumAndIncrement());
		ModuleName = defaultModuleName + buf;
	}
	if (simpleName.compare("") != 0)
	{
		SimpleName = simpleName;
	}
	else
	{
		SimpleName = ModuleName;
	}
	
	Inputs = inputs;
	Outputs = outputs;
	Params = params;

	Configurator::AddModule(this);
}

BaseModule::~BaseModule()
{
}

const type_info& BaseModule::GetDerivedClass()
{
	Message::ErrorMessage("No derived class defined.");
	return typeid(this);
}

void BaseModule::SetResourceConnections(uint8_t whichCab)
{
	Configurator::CAB_Array[whichCab].Capacitor_NumFree -= NumCapacitors;
	Configurator::CAB_Array[whichCab].OpAmp_NumFree -= NumOpAmps;
	Configurator::CAB_Array[whichCab].Comparator_NumFree -= NumComparators;

	if (Configurator::CAB_Array[whichCab].Capacitor_NumFree < 0)
	{
		throw std::exception("Tried to allocate more capacitors than were available in CAB " + whichCab);
	}
	if (Configurator::CAB_Array[whichCab].OpAmp_NumFree < 0)
	{
		throw std::exception("Tried to allocate more op amps than were available in CAB " + whichCab);
	}
	if (Configurator::CAB_Array[whichCab].Comparator_NumFree < 0)
	{
		throw std::exception("Tried to allocate more comparators than were available in CAB " + whichCab);
	}

	CurrentCab = whichCab;

	// Check to see if we are connecting to any inputs
	//if (BaseModule::GetInputs().size() >= 1)
	{
		for (auto it = BaseModule::GetInputs().begin(); it != BaseModule::GetInputs().end(); it++)
		{
			for (auto it2 = (*it)->input->begin(); it2 != (*it)->input->end(); it2++)
			{
				if (*it2 != NULL)
				{
					if ((*it2)->GetCurrentOutputSite()->resourceType == TYPE_IO)
					{
						IO *io = dynamic_cast<IO*>((*it2));
						io->AddConnectedCAB(whichCab);
					}
				}
			}
		}
	}
}

void BaseModule::SetCam(an_CAM cam)
{
	Cam = cam;
}

an_CAM BaseModule::GetCam()
{
	return Cam;
}

void BaseModule::AddInput(BaseModule *newInput, ModuleInput *whichInputSite)
{
	if (whichInputSite == NULL)
	{
		whichInputSite = GetDefaultInputSite();
	}

	for (auto it = Inputs.begin(); it != Inputs.end(); it++)
	{
		if ((*it) == whichInputSite)
		{
			if ((*it)->input->size() != 0)
			{
				Message::ErrorMessage("Multiple inputs cannot be tied to the same site");
				return;
			}
			(*it)->input->push_back(newInput);
		}
	}
}

void BaseModule::AddOutput(BaseModule *newOutput, ModuleOutput *whichOutputSite)
{
	if (whichOutputSite == NULL)
	{
		whichOutputSite = GetDefaultOutputSite();
	}

	for (auto it = Outputs.begin(); it != Outputs.end(); it++)
	{
		if ((*it) == whichOutputSite)
		{
			(*it)->output->push_back(newOutput);
		}
	}
}

void BaseModule::RemoveInput(BaseModule *input)
{
	for (auto it = Inputs.begin(); it != Inputs.end(); it++)
	{
		for (auto it2 = (*it)->input->begin(); it2 != (*it)->input->end(); )
		{
			if ((*it2) == input)
			{
				it2 = (*it)->input->erase(it2);
			}
			else
			{
				it2++;
			}
		}
	}
}

void BaseModule::RemoveOutput(BaseModule *output)
{
	for (auto it = Outputs.begin(); it != Outputs.end(); it++)
	{
		for (auto it2 = (*it)->output->begin(); it2 != (*it)->output->end(); )
		{
			if ((*it2) == output)
			{
				it2 = (*it)->output->erase(it2);
			}
			else
			{
				it2++;
			}
		}
	}
}

const std::vector<BaseModule::ModuleInput*> &BaseModule::GetInputs()
{
	return Inputs;
}

const std::vector<BaseModule::ModuleOutput*> &BaseModule::GetOutputs()
{
	return Outputs;
}

BaseModule::ModuleInput *BaseModule::GetInputSiteByName(std::string inputSiteName)
{
	for (auto it = Inputs.begin(); it != Inputs.end(); it++)
	{
		if ((*it)->inputSiteName.compare(inputSiteName) == 0)
		{
			return (*it);
		}
	}

	Message::ErrorMessage("No input found with name: " + inputSiteName);
}

BaseModule::ModuleOutput *BaseModule::GetOutputSiteByName(std::string outputSiteName)
{
	for (auto it = Outputs.begin(); it != Outputs.end(); it++)
	{
		if ((*it)->outputSiteName.compare(outputSiteName) == 0)
		{
			return (*it);
		}
	}

	Message::ErrorMessage("No output found with name: " + outputSiteName);
}

BaseModule::ModuleInput *BaseModule::GetDefaultInputSite()
{
	return Inputs.front();
}

BaseModule::ModuleOutput *BaseModule::GetDefaultOutputSite()
{
	return Outputs.front();
}

void BaseModule::ConnectToOutput(BaseModule *moduleProvidingInput, BaseModule::ModuleInput *whichInputSite, BaseModule::ModuleOutput *whichOutputSite)
{
	moduleProvidingInput->AddOutput(this, whichOutputSite);
	AddInput(moduleProvidingInput, whichInputSite);
}

// Adds the module to the default position in the current linear chain (just before the output)
void BaseModule::AddToEnd(BaseModule::ModuleInput *whichInputSiteOfThis, BaseModule::ModuleOutput *whichOutputSiteOfThis)
{
	BaseModule *out = Configurator::GetDefaultSystemOutput();
	BaseModule *in = out->GetDefaultInputSite()->input->front();

	AddBetween(in, out, whichInputSiteOfThis, whichOutputSiteOfThis);
}

// Adds the module between the two given modules
void BaseModule::AddBetween(BaseModule *in, BaseModule *out, BaseModule::ModuleInput *whichInputSiteOfThis, BaseModule::ModuleOutput *whichOutputSiteOfThis, BaseModule::ModuleOutput *whichOutputSiteFromIn, BaseModule::ModuleInput *whichInputSiteFromOut)
{
	in->RemoveOutput(out);
	out->RemoveInput(in);

	ConnectToOutput(in, whichInputSiteOfThis, whichOutputSiteFromIn);
	out->ConnectToOutput(this, whichInputSiteFromOut, whichOutputSiteOfThis);
}

void BaseModule::SetParamValue(std::string key, std::string value)
{
	Params[key] = value;
	/* Derived class should now call reconfigure function with new value */
}

std::string BaseModule::GetParamValue(std::string key)
{
	return Params[key];
}

uint8_t BaseModule::GetNumCapacitors()
{
	return NumCapacitors;
}

uint8_t BaseModule::GetNumOpAmps()
{
	return NumOpAmps;
}

uint8_t BaseModule::GetNumComparators()
{
	return NumComparators;
}

uint8_t BaseModule::GetCurrentCab()
{
	return CurrentCab;
}

ConnectionSite *BaseModule::GetCurrentOutputSite()
{
	return CurrentOutputSite;
}

std::string BaseModule::GetModuleName()
{
	return ModuleName;
}

std::string BaseModule::GetSimpleName()
{
	return SimpleName;
}

void BaseModule::SetModuleName(std::string newName)
{
	ModuleName = newName;
}

void BaseModule::SetSimpleName(std::string newName)
{
	SimpleName = newName;
}

std::string BaseModule::GetModuleInformation()
{
	return "";
}
