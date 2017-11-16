#include "stdafx.h"
#include "BaseModule.h"
#include "IO.h"

IO::IO(uint8_t whichIO,
	an_CAM cam,
	std::string moduleName,
	std::string simpleName,
	std::vector<ModuleInput*> inputs,
	std::vector<ModuleOutput*> outputs,
	std::map<std::string, std::string> params) :
	BaseModule(cam, "io", moduleName, simpleName, inputs, outputs, params)
{
	if (Inputs.size() == 0)
	{
		Inputs.push_back(new ModuleInput{ new std::vector<BaseModule*>(), DEFAULT_INPUT_NAME });
	}
	if (Outputs.size() == 0)
	{
		Outputs.push_back(new ModuleOutput{ new std::vector<BaseModule*>(), DEFAULT_OUTPUT_NAME });
	}

	WhichIO = whichIO;
	BaseModule::NumCapacitors = 0;
	BaseModule::NumOpAmps = 0;
	BaseModule::NumComparators = 0;
	for (int whichCab = 0; whichCab < NUM_CABS_INCLUDING_IO; whichCab++)
	{
		IO::ConnectedCabs[whichCab] = false;
	}
}

std::string IO::GetModuleInformation()
{
	std::stringstream s;
	s << "(mode=";
	if (Configurator::IO_Modes[WhichIO] == IO_MODE_INPUT)
	{
		s << "input";
	}
	else if (Configurator::IO_Modes[WhichIO] == IO_MODE_OUTPUT)
	{
		s << "output";
	}
	else
	{
		s << "unused";
	}
	s << ")";
	return s.str();
}

void IO::AddConnectedCAB(uint8_t whichCab)
{
	IO::ConnectedCabs[whichCab] = true;
}

bool IO::IsConnectedToCAB(uint8_t whichCab)
{
	return IO::ConnectedCabs[whichCab];
}

void IO::ResetConnectedCABs()
{
	for (int whichCab = 0; whichCab < NUM_CABS_INCLUDING_IO; whichCab++)
	{
		IO::ConnectedCabs[whichCab] = false;
	}
}

void IO::SetResourceConnections(uint8_t whichCab)
{
	Configurator::IOs[WhichIO] = this;
	if (Configurator::IO_Modes[WhichIO] == IO_MODE_NONE)
	{ 
		return;
	}

	if (Configurator::IO_Modes[WhichIO] == IO_MODE_OUTPUT)
	{
		if (BaseModule::GetDefaultInputSite()->input->size() > 0)
		{
			IO::AddConnectedCAB(BaseModule::GetDefaultInputSite()->input->front()->GetCurrentCab());
			Configurator::InputsToOutputs[WhichIO] = BaseModule::GetDefaultInputSite()->input->front()->GetCurrentOutputSite();
		}
	}
	CurrentOutputSite = new ConnectionSite{ DEFAULT, TYPE_IO, CABIO, WhichIO, TYPE_OUT };

	BaseModule::SetResourceConnections(whichCab);
}


