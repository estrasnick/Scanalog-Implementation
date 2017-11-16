#include "stdafx.h"
#include <stdlib.h>
#include <string>
#include <sstream>
#include <algorithm>
#include "Configurator.h"
#include "Message.h"
#include "BaseModule.h"
#include "ConnectionSites.h"
#include "IO.h"

/* This class stores a graph-like representation of the circuit and 
   converts it into a byte array to be written into the FPAA. */

std::set<BaseModule*> Configurator::CurrentModules;
std::vector<BaseModule*> Configurator::OrderedModules;
BaseModule *Configurator::IOs[NUM_IOS];
ConnectionSite *Configurator::InputsToOutputs[NUM_IOS];
Configurator::Interconnect *Configurator::IO_Interconnects[NUM_IOS] = { NULL };
Configurator::Interconnect *Configurator::Comp_Interconnects[COMPARATORS_PER_BANK * NUM_CABS] = { NULL };
Configurator::cab Configurator::Cab1, Configurator::Cab2, Configurator::Cab3, Configurator::Cab4;
bool Configurator::IsOrdered = false;
an_Byte Configurator::LastGoodConfigData[CONFIG_DATA_SIZE];
Configurator::Interconnect *Configurator::Interconnects_1_3[NUM_INTERCONNECTS_TOTAL] =
{
	new Configurator::Interconnect{ INTERCONNECT_ID_1 , 0, INTERCONNECT_1_HALFBYTE, std::vector<ConnectionSite*>(), NULL, true },
	new Configurator::Interconnect{ INTERCONNECT_ID_2 , 0, INTERCONNECT_2_HALFBYTE, std::vector<ConnectionSite*>(), NULL, true },
	new Configurator::Interconnect{ INTERCONNECT_ID_3 , 0, INTERCONNECT_3_HALFBYTE, std::vector<ConnectionSite*>(), NULL, true },
	new Configurator::Interconnect{ INTERCONNECT_ID_4 , 0, INTERCONNECT_4_HALFBYTE, std::vector<ConnectionSite*>(), NULL, true },
	new Configurator::Interconnect{ INTERCONNECT_ID_5 , 0, INTERCONNECT_5_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false },
	new Configurator::Interconnect{ INTERCONNECT_ID_6 , 0, INTERCONNECT_6_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false },
	new Configurator::Interconnect{ INTERCONNECT_ID_7 , 0, INTERCONNECT_7_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false },
	new Configurator::Interconnect{ INTERCONNECT_ID_8 , 0, INTERCONNECT_8_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false },
	new Configurator::Interconnect{ INTERCONNECT_ID_9 , 0, INTERCONNECT_9_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false },
	new Configurator::Interconnect{ INTERCONNECT_ID_10 , 0, INTERCONNECT_10_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false },
	new Configurator::Interconnect{ INTERCONNECT_ID_11 , 0, INTERCONNECT_11_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false }
};
Configurator::Interconnect *Configurator::Interconnects_2_4[NUM_INTERCONNECTS_TOTAL] =
{
	new Configurator::Interconnect{ INTERCONNECT_ID_1 , 0, INTERCONNECT_1_HALFBYTE, std::vector<ConnectionSite*>(), NULL, true },
	new Configurator::Interconnect{ INTERCONNECT_ID_2 , 0, INTERCONNECT_2_HALFBYTE, std::vector<ConnectionSite*>(), NULL, true },
	new Configurator::Interconnect{ INTERCONNECT_ID_3 , 0, INTERCONNECT_3_HALFBYTE, std::vector<ConnectionSite*>(), NULL, true },
	new Configurator::Interconnect{ INTERCONNECT_ID_4 , 0, INTERCONNECT_4_HALFBYTE, std::vector<ConnectionSite*>(), NULL, true },
	new Configurator::Interconnect{ INTERCONNECT_ID_5 , 0, INTERCONNECT_5_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false },
	new Configurator::Interconnect{ INTERCONNECT_ID_6 , 0, INTERCONNECT_6_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false },
	new Configurator::Interconnect{ INTERCONNECT_ID_7 , 0, INTERCONNECT_7_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false },
	new Configurator::Interconnect{ INTERCONNECT_ID_8 , 0, INTERCONNECT_8_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false },
	new Configurator::Interconnect{ INTERCONNECT_ID_9 , 0, INTERCONNECT_9_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false },
	new Configurator::Interconnect{ INTERCONNECT_ID_10 , 0, INTERCONNECT_10_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false },
	new Configurator::Interconnect{ INTERCONNECT_ID_11 , 0, INTERCONNECT_11_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false }
};

Configurator::cab Configurator::CAB_Array[] =
{
	Cab1,
	Cab2,
	Cab3,
	Cab4
};

uint8_t Configurator::IO_Modes[] = DEFAULT_IO_CONFIG;

int Configurator::currentModuleNum = 0;

void Configurator::SetIOMode(uint8_t which, uint8_t io_mode)
{
	IO_Modes[which] = io_mode;
}

void Configurator::Reset()
{
	currentModuleNum = 0;
	CurrentModules.clear();
	InitIOs();
	ClearConfigurationData();
}

void Configurator::ClearConfigurationData()
{
	for (int i = 0; i < NUM_CABS; i++)
	{
		CAB_Array[i] = Configurator::cab();
	}
	for (int i = 0; i < NUM_IOS; i++)
	{
		dynamic_cast<IO*>(IOs[i])->ResetConnectedCABs();
	}
	IsOrdered = false;
	for (int i = 0; i < NUM_IOS; i++)
	{
		IO_Interconnects[i] = NULL;
	}
	for (int i = 0; i < COMPARATORS_PER_BANK * NUM_CABS; i++)
	{
		Comp_Interconnects[i] = NULL;
	}
	
	Interconnects_1_3[INTERCONNECT_ID_1] = new Interconnect{ INTERCONNECT_ID_1 , 0, INTERCONNECT_1_HALFBYTE, std::vector<ConnectionSite*>(), NULL, true };
	Interconnects_1_3[INTERCONNECT_ID_2] = new Interconnect{ INTERCONNECT_ID_2 , 0, INTERCONNECT_2_HALFBYTE, std::vector<ConnectionSite*>(), NULL, true };
	Interconnects_1_3[INTERCONNECT_ID_3] = new Interconnect{ INTERCONNECT_ID_3 , 0, INTERCONNECT_3_HALFBYTE, std::vector<ConnectionSite*>(), NULL, true };
	Interconnects_1_3[INTERCONNECT_ID_4] = new Interconnect{ INTERCONNECT_ID_4 , 0, INTERCONNECT_4_HALFBYTE, std::vector<ConnectionSite*>(), NULL, true };
	Interconnects_1_3[INTERCONNECT_ID_5] = new Interconnect{ INTERCONNECT_ID_5 , 0, INTERCONNECT_5_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false };
	Interconnects_1_3[INTERCONNECT_ID_6] = new Interconnect{ INTERCONNECT_ID_6 , 0, INTERCONNECT_6_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false };
	Interconnects_1_3[INTERCONNECT_ID_7] = new Interconnect{ INTERCONNECT_ID_7 , 0, INTERCONNECT_7_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false };
	Interconnects_1_3[INTERCONNECT_ID_8] = new Interconnect{ INTERCONNECT_ID_8 , 0, INTERCONNECT_8_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false };
	Interconnects_1_3[INTERCONNECT_ID_9] = new Interconnect{ INTERCONNECT_ID_9 , 0, INTERCONNECT_9_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false };
	Interconnects_1_3[INTERCONNECT_ID_10] = new Interconnect{ INTERCONNECT_ID_10 , 0, INTERCONNECT_10_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false };
	Interconnects_1_3[INTERCONNECT_ID_11] = new Interconnect{ INTERCONNECT_ID_11 , 0, INTERCONNECT_11_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false };
	
	Interconnects_2_4[INTERCONNECT_ID_1] = new Interconnect{ INTERCONNECT_ID_1 , 0, INTERCONNECT_1_HALFBYTE, std::vector<ConnectionSite*>(), NULL, true };
	Interconnects_2_4[INTERCONNECT_ID_2] = new Interconnect{ INTERCONNECT_ID_2 , 0, INTERCONNECT_2_HALFBYTE, std::vector<ConnectionSite*>(), NULL, true };
	Interconnects_2_4[INTERCONNECT_ID_3] = new Interconnect{ INTERCONNECT_ID_3 , 0, INTERCONNECT_3_HALFBYTE, std::vector<ConnectionSite*>(), NULL, true };
	Interconnects_2_4[INTERCONNECT_ID_4] = new Interconnect{ INTERCONNECT_ID_4 , 0, INTERCONNECT_4_HALFBYTE, std::vector<ConnectionSite*>(), NULL, true };
	Interconnects_2_4[INTERCONNECT_ID_5] = new Interconnect{ INTERCONNECT_ID_5 , 0, INTERCONNECT_5_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false };
	Interconnects_2_4[INTERCONNECT_ID_6] = new Interconnect{ INTERCONNECT_ID_6 , 0, INTERCONNECT_6_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false };
	Interconnects_2_4[INTERCONNECT_ID_7] = new Interconnect{ INTERCONNECT_ID_7 , 0, INTERCONNECT_7_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false };
	Interconnects_2_4[INTERCONNECT_ID_8] = new Interconnect{ INTERCONNECT_ID_8 , 0, INTERCONNECT_8_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false };
	Interconnects_2_4[INTERCONNECT_ID_9] = new Interconnect{ INTERCONNECT_ID_9 , 0, INTERCONNECT_9_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false };
	Interconnects_2_4[INTERCONNECT_ID_10] = new Interconnect{ INTERCONNECT_ID_10 , 0, INTERCONNECT_10_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false };
	Interconnects_2_4[INTERCONNECT_ID_11] = new Interconnect{ INTERCONNECT_ID_11 , 0, INTERCONNECT_11_HALFBYTE, std::vector<ConnectionSite*>(), NULL, false };
}

void Configurator::AddModule(BaseModule *module)
{
	CurrentModules.insert(module);
}

void Configurator::RemoveModule(BaseModule *module)
{
	CurrentModules.erase(module);
}

std::set<BaseModule*> Configurator::GetCurrentModules()
{
	return CurrentModules;
}

std::vector<BaseModule*> Configurator::GetOrderedModules()
{
	ASSERT(IsOrdered);
	return OrderedModules;
}

BaseModule *Configurator::GetDefaultSystemOutput()
{
	return Configurator::IOs[DEFAULT_OUTPUT_NUMBER];
}

BaseModule *Configurator::GetDefaultSystemInput()
{
	return Configurator::IOs[DEFAULT_INPUT_NUMBER];
}

BaseModule *Configurator::GetDefaultSystemProbe()
{
	return Configurator::IOs[DEFAULT_PROBE_NUMBER];
}

int Configurator::GetNextModuleNumAndIncrement()
{
	return currentModuleNum++;
}

BaseModule *Configurator::GetModuleWithName(std::string name)
{
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	for (auto it = CurrentModules.begin(); it != CurrentModules.end(); it++)
	{
		std::string strcpy((*it)->GetModuleName());
		std::transform(strcpy.begin(), strcpy.end(), strcpy.begin(), ::tolower);
		if (strcpy.compare(name) == 0)
		{
			return *it;
		}
	}
	return NULL;
}

// Default IO modes are set in Helpers.h
void Configurator::InitIOs()
{
	uint8_t ioModes[] = DEFAULT_IO_CONFIG;
	for (uint8_t i = 0; i < NUM_IOS; i++)
	{
		std::stringstream name;
		name << "io" << (int) i + 1;

		IO *newIO = new IO(i, i, name.str(), name.str());
		Configurator::AddModule(newIO);
		Configurator::IOs[i] = newIO;
		Configurator::IO_Modes[i] = ioModes[i];
	}
}

/* Used to fully reconfigure the FPAA (i.e. change topology) */
// Declare an array of the correct size, and then pass it by reference here, before 
// using it to reconfigure the chip
an_Byte (&Configurator::GenerateConfigurationData(an_Byte(&array)[CONFIG_DATA_SIZE]))[CONFIG_DATA_SIZE]
{
	// First, we need to topologically sort modules based on their output, as inputs
	// to a given module must be analyzed before the module itself
	Configurator::OrderedModules.clear();
	Configurator::SortModules();
	IsOrdered = true;

	// Perform resource allocation, recording information about what must be connected to what
	for (auto module = OrderedModules.begin(); module != OrderedModules.end(); ++module)
	{
		UpdateResourcesForModule(*module);
	}

	an_Byte rawArray[NUM_BANKS][BYTES_PER_BANK] = { 0 };

	// Set the bytes of the data array according to the information we've gathered
	if (!DedicateInterconnectResources()
		|| !SetPreconfigurationBits(rawArray)
		|| !SetClockBits(rawArray)
		|| !SetIOControlBits(rawArray)
		|| !SetComparatorControlBits(rawArray)
		|| !SetCapRatioBits(rawArray)
		|| !SetRoutingBits(rawArray))
	{
		return LastGoodConfigData;
	}

	// Convert the raw (clean) array into a valid configuration array for the chip
	RawArrayToConfigData(rawArray, array);
	
	// Return the completed data array, storing it as last known good
	for (int i = 0; i < CONFIG_DATA_SIZE; i++)
	{
		LastGoodConfigData[i] = array[i];
	}
	return array;
}

/* Used to update a parameter (not a full topological change) */
an_Byte(&Configurator::GenerateReconfigurationData(an_Byte(&array)[RECONFIG_DATA_SIZE], int whichCab))[RECONFIG_DATA_SIZE]
{
	// First, we need to topologically sort modules based on their output, as inputs
	// to a given module must be analyzed before the module itself
	Configurator::OrderedModules.clear();
	Configurator::SortModules();
	IsOrdered = true;

	// Perform resource allocation, recording information about what must be connected to what
	for (auto module = OrderedModules.begin(); module != OrderedModules.end(); ++module)
	{
		UpdateResourcesForModule(*module);
	}

	// Set update header bits
	array[0] = 0xD5;
	array[1] = 0x01;
	array[2] = 0xC1;
	array[3] = 0x80;
	// choose bank address based on whichCab
	array[4] = CAB_BANK_A(whichCab);
	array[5] = 0x08;

	// Fill with new capacitor values
	for (int cap = 0; cap < CAPACITORS_PER_BANK; cap++)
	{
		if (Configurator::CAB_Array[whichCab].Capacitor_Values[cap] != NULL)
		{
			array[13 - cap] = *Configurator::CAB_Array[whichCab].Capacitor_Values[cap];
		}
	}

	array[14] = 0x2A;

	return array;
}

void Configurator::UpdateResourcesForModule(BaseModule *module)
{
	uint8_t capsNeeded = module->GetNumCapacitors();
	uint8_t opAmpsNeeded = module->GetNumOpAmps();
	uint8_t comparatorsNeeded = module->GetNumComparators();
	bool enoughResources = false;
	uint8_t whichCab;
	
	if (dynamic_cast<IO*>(module))
	{
		whichCab = CABIO;
		module->SetResourceConnections(whichCab);
	}
	else
	{
		for (whichCab = 0; whichCab < NUM_CABS; whichCab++)
		{
			if (CAB_Array[whichCab].Capacitor_NumFree >= capsNeeded && CAB_Array[whichCab].OpAmp_NumFree >= opAmpsNeeded && CAB_Array[whichCab].Comparator_NumFree >= comparatorsNeeded)
			{
				enoughResources = true;
				// do not adjust available resources here, as it is done later
				break;
			}
		}
		if (!enoughResources)
		{
			Message::ErrorMessage("Not enough resources for module: " + module->GetModuleName());
			return;
		}
		else
		{
			module->SetResourceConnections(whichCab); 
		}
	}
}

// Topologically sort modules based on output, and store in OrderedModules
void Configurator::SortModules()
{
	std::set<BaseModule*> CurrentModulesCopy;
	std::stack<BaseModule*> stack;
	for (auto module = CurrentModules.begin(); module != CurrentModules.end(); ++module)
	{
		CurrentModulesCopy.insert(*module);
	}

	for (auto module = CurrentModules.begin(); module != CurrentModules.end(); ++module)
	{
		Configurator::TopologicalSort(*module, &CurrentModulesCopy, &stack);
	}

	// pop the stack and create the sorted list of modules
	while (stack.size() > 0)
	{
		Configurator::OrderedModules.push_back(stack.top());
		stack.pop();
	}
}

// Recursive topological sort logic
void Configurator::TopologicalSort(BaseModule *module, std::set<BaseModule*> *moduleSet, std::stack<BaseModule*> *stack)
{
	if (moduleSet->count(module) == 0)
	{
		return;
	}
	moduleSet->erase(module);
	const std::vector<BaseModule::ModuleOutput*> &outputs = module->GetOutputs();
	for (auto iter = outputs.begin(); iter != outputs.end(); iter++)
	{
		for (auto iter2 = (*iter)->output->begin(); iter2 != (*iter)->output->end(); iter2++)
		{
			if (moduleSet->count(*iter2) != 0)
			{
				TopologicalSort(*iter2, moduleSet, stack);
			}
		}
	}
	stack->push(module);
}

/* Returns true iff the circuit passes configurability checks. Otherwise, sends an error message and returns false*/
bool Configurator::IsConfigurableCircuit()
{
	/* IMPLEMENT DESIRED CIRCUIT CHECKS HERE */
	return true;
}

// Interconnects (between CABs) are needed for:
	// Comparator output
	// I/O Output
	// I/O Input
// There are a limited quantity, and as a result some circuit configurations are not possible on the FPAA
bool Configurator::DedicateInterconnectResources()
{
	// First, finish configuring interconnects:
	// Same-CAB-partition defaults for OA1 and OA2 output
	Interconnects_1_3[INTERCONNECT_ID_1]->InputtingResourceSite = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, CAB1, OP_AMP_2, TYPE_OUT };
	Interconnects_1_3[INTERCONNECT_ID_2]->InputtingResourceSite = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, CAB1, OP_AMP_1, TYPE_OUT };
	Interconnects_1_3[INTERCONNECT_ID_3]->InputtingResourceSite = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, CAB3, OP_AMP_2, TYPE_OUT };
	Interconnects_1_3[INTERCONNECT_ID_4]->InputtingResourceSite = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, CAB3, OP_AMP_1, TYPE_OUT };
	for (int whichInterconnect = 0; whichInterconnect < NUM_INTERCONNECTS_TOTAL; whichInterconnect++)
	{
		Interconnects_1_3[whichInterconnect]->CAB = CAB_UNUSED;
	}
	Interconnects_2_4[INTERCONNECT_ID_1]->InputtingResourceSite = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, CAB2, OP_AMP_2, TYPE_OUT };
	Interconnects_2_4[INTERCONNECT_ID_2]->InputtingResourceSite = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, CAB2, OP_AMP_1, TYPE_OUT };
	Interconnects_2_4[INTERCONNECT_ID_3]->InputtingResourceSite = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, CAB4, OP_AMP_2, TYPE_OUT };
	Interconnects_2_4[INTERCONNECT_ID_4]->InputtingResourceSite = new ConnectionSite{ DEFAULT, TYPE_OP_AMP, CAB4, OP_AMP_1, TYPE_OUT };
	for (int whichInterconnect = 0; whichInterconnect < NUM_INTERCONNECTS_TOTAL; whichInterconnect++)
	{
		Interconnects_2_4[whichInterconnect]->CAB = CAB_UNUSED;
	}

	// Here is out simplified heuristic: 
	// The order in which we iterate is important. Output from a bank must be assigned first, and to the correct
	// interconnects, as this simplifies the setting of byte 2 in Bank B
	//   Iterate through modules. If any receive comparator output, then that comparator needs an interconnect.
	//       This should preferentially assign to the interconnect with halfbyte 5
	//   Iterate through output I/Os. Assign CROSS-PARTITION OA1 output and OA2 output specifically to halfbytes 6 and 7 respectively
	//   Iterate through input I/Os.
	//   If we exceed NUM_INTERCONNECTS_CONFIGURABLE (3) interconnects needed for a given bank, then fail

	for (auto it = OrderedModules.begin(); it != OrderedModules.end(); it++)
	{
		for (auto it2 = (*it)->GetInputs().begin(); it2 != (*it)->GetInputs().end(); it2++)
		{
			for (auto it3 = (*it2)->input->begin(); it3 != (*it2)->input->end(); it3++)
			{
				ConnectionSite *inputSource = (*it3)->GetCurrentOutputSite();
				if (inputSource->resourceType == TYPE_COMPARATOR)
				{
					Comp_Interconnects[inputSource->cabNumber] = AssignInterconnect((*it3)->GetCurrentCab(), inputSource, NULL, INTERCONNECTS_FOR_CAB(inputSource->cabNumber)[INTERCONNECT_ID_DEFAULT_COMP]);
					if (Comp_Interconnects[inputSource->cabNumber] == NULL)
					{
						Message::ErrorMessage("Not enough resources to configure this chip");
						return false;
					}
				}
			}
		}
	}

	// output pass must come first
	for (uint8_t whichIO = 0; whichIO < NUM_IOS; whichIO++)
	{
		if (IO_Modes[whichIO] == IO_MODE_OUTPUT)
		{
			IO *io = dynamic_cast<IO*>(IOs[whichIO]);
			for (uint8_t whichCab = 0; whichCab < NUM_CABS_INCLUDING_IO; whichCab++)
			{
				if (io->IsConnectedToCAB(whichCab))
				{
					ConnectionSite *inputSite = io->GetInputs().front()->input->front()->GetCurrentOutputSite();
					// if this is a cross-partition op-amp
					if ((inputSite->resourceType == TYPE_OP_AMP) 
						&& ((((whichIO == RESOURCE_1) || (whichIO == RESOURCE_2) || (whichIO == RESOURCE_5) || (whichIO == RESOURCE_6)) && ((whichCab == CAB3) || (whichCab == CAB4)))
							|| (((whichIO == RESOURCE_3) || (whichIO == RESOURCE_4) || (whichIO == RESOURCE_7)) && ((whichCab == CAB1) || (whichCab == CAB2)))))
					{
						int whichInterconnectID = (inputSite->resourceNumber == RESOURCE_1) ? INTERCONNECT_ID_DEFAULT_OA1 : INTERCONNECT_ID_DEFAULT_OA2;
						IO_Interconnects[whichIO] = AssignInterconnect(whichCab, inputSite, new ConnectionSite{ DEFAULT, TYPE_IO, CABIO, whichIO, TYPE_IN }, INTERCONNECTS_FOR_CAB(whichCab)[whichInterconnectID]);
					}
					else
					{
						IO_Interconnects[whichIO] = AssignInterconnect(whichCab, inputSite, new ConnectionSite{ DEFAULT, TYPE_IO, CABIO, whichIO, TYPE_IN });
					}
					
					if (IO_Interconnects[whichIO] == NULL)
					{
						Message::ErrorMessage("Not enough resources to configure this chip");
						return false;
					}
				}
			}
		}
		
	}

	// then input pass
	for (uint8_t whichIO = 0; whichIO < NUM_IOS; whichIO++)
	{
		if (IO_Modes[whichIO] == IO_MODE_INPUT)
		{
			IO *io = dynamic_cast<IO*>(IOs[whichIO]);
			for (uint8_t whichCab = 0; whichCab < NUM_CABS_INCLUDING_IO; whichCab++)
			{
				if (io->IsConnectedToCAB(whichCab))
				{
					IO_Interconnects[whichIO] = AssignInterconnect(whichCab, io->GetCurrentOutputSite(), NULL);
					if (IO_Interconnects[whichIO] == NULL)
					{
						Message::ErrorMessage("Not enough resources to configure this chip");
						return false;
					}
				}
			}
		}
	}

	return true;
}

Configurator::Interconnect *Configurator::AssignInterconnect(uint8_t whichCab, ConnectionSite *inputSite, ConnectionSite *outputSite, Interconnect *specificInterconnect)
{
	// Check if we are to attempt a specific interconnect
	if (specificInterconnect != NULL)
	{
		if (!specificInterconnect->isUsed)
		{
			specificInterconnect->isUsed = true;
			if (outputSite != NULL)
			{
				specificInterconnect->OutputtingResourceSites.push_back(outputSite);
			}
			specificInterconnect->InputtingResourceSite = inputSite;
			specificInterconnect->CAB = whichCab;
			return specificInterconnect;
		}
		else if (IsEqualSite(specificInterconnect->InputtingResourceSite, inputSite))
		{
			// Add output site only if it already does not exist
			if (outputSite != NULL)
			{
				bool found = false;
				for (auto siteIt = specificInterconnect->OutputtingResourceSites.begin(); siteIt != specificInterconnect->OutputtingResourceSites.end(); siteIt++)
				{
					if (IsEqualSite(*siteIt, outputSite))
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					specificInterconnect->OutputtingResourceSites.push_back(outputSite);
				}
			}
			specificInterconnect->CAB = whichCab;
			return specificInterconnect;
		}
		else
		{
			return NULL;
		}
	}

	// Determine if we can assign this to the input only interconnects
	//  (i.e. input which goes only to same-partition Op Amps/comps)
	if (inputSite->resourceType == TYPE_IO)
	{
		bool valid = true;
		uint8_t whichIO = inputSite->resourceNumber;
		// This detection needs improvement. We should be checking to see if the input site of the receiving module
		//  is an op amp or a comparator. Instead, we check if the receiving module has any comparators at all
		for (auto it = IOs[whichIO]->GetOutputs().begin(); it != IOs[whichIO]->GetOutputs().end(); it++)
		{
			for (auto it2 = (*it)->output->begin(); it2 != (*it)->output->end(); it2++)
			{
				if ((whichIO == RESOURCE_1) || (whichIO == RESOURCE_2) || (whichIO == RESOURCE_5) || (whichIO == RESOURCE_6))
				{
					if (((*it2)->GetCurrentCab() != CAB1) && ((*it2)->GetCurrentCab() != CAB2) )//|| ((*it2)->GetNumComparators() > 0))
					{
						valid = false;
						break;
					}
				}
				else
				{
					if (((*it2)->GetCurrentCab() != CAB3) && ((*it2)->GetCurrentCab() != CAB4) )// || ((*it2)->GetNumComparators() > 0))
					{
						valid = false;
						break;
					}
				}
			}
		}

		if (valid)
		{
			Interconnect *interconnect1 = INTERCONNECTS_FOR_CAB(whichCab)[(whichCab > CAB2) ? INTERCONNECT_ID_10 : INTERCONNECT_ID_8];
			Interconnect *interconnect2 = INTERCONNECTS_FOR_CAB(whichCab)[(whichCab > CAB2) ? INTERCONNECT_ID_11 : INTERCONNECT_ID_9];
			if (!interconnect1->isUsed)
			{
				interconnect1->isUsed = true;
				if (outputSite != NULL)
				{
					interconnect1->OutputtingResourceSites.push_back(outputSite);
				}
				interconnect1->InputtingResourceSite = inputSite;
				interconnect1->CAB = whichCab;
				return interconnect1;
			}
			else if (!interconnect2->isUsed)
			{
				interconnect2->isUsed = true;
				if (outputSite != NULL)
				{
					interconnect2->OutputtingResourceSites.push_back(outputSite);
				}
				interconnect2->InputtingResourceSite = inputSite;
				interconnect2->CAB = whichCab;
				return interconnect2;
			}
		}
	}
	for (int i = 0; i < NUM_INTERCONNECTS_NORMAL; i++)
	{
		Interconnect *interconnect = INTERCONNECTS_FOR_CAB(whichCab)[i];

		// unused, so assign
		// we can be greedy because we always assign in order
		if (!interconnect->isUsed)
		{
			interconnect->isUsed = true;
			if (outputSite != NULL)
			{
				interconnect->OutputtingResourceSites.push_back(outputSite);
			}
			interconnect->InputtingResourceSite = inputSite;
			interconnect->CAB = whichCab;
			return interconnect;
		}
		// if the inputting resource has already been assigned, we can reuse it
		else if (IsEqualSite(interconnect->InputtingResourceSite, inputSite))
		{
			// Add output site only if it already does not exist
			if (outputSite != NULL)
			{
				bool found = false;
				for (auto siteIt = interconnect->OutputtingResourceSites.begin(); siteIt != interconnect->OutputtingResourceSites.end(); siteIt++)
				{
					if (IsEqualSite(*siteIt, outputSite))
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					interconnect->OutputtingResourceSites.push_back(outputSite);
				}
			}
			interconnect->CAB = whichCab;
			return interconnect;
		}
	}
	return NULL;
}


/******************** SETTING BITS OF CONFIG ARRAY BASED ON MODULE DATA **********************/
/*                                                                                           */
/*********************************************************************************************/

// Set the bits which basic settings of the chip
bool Configurator::SetPreconfigurationBits(an_Byte array[NUM_BANKS][BYTES_PER_BANK])
{
	array[0][11] = 0x40;
	array[0][14] = 0x51;
	array[0][15] = 0xFF;
	array[0][16] = 0x0F;
	array[0][17] = 0xF1;

	array[1][2] = 0x40;
	array[1][30] = 0x02;
	array[1][31] = 0xFF;
	return true;
}

// Set the clock bits to each CAB
bool Configurator::SetClockBits(an_Byte array[NUM_BANKS][BYTES_PER_BANK])
{
	// Set clock divisor bytes
	// sys = 16 kHz
	// Clock 0 - sys / 510
	// Clock 1 - sys / 8
	// Clock 2 - sys / 1
	array[0][6] = 0x04;
	array[0][7] = 0xFF;
	array[0][8] = 0x1D;

	// Set clock select bytes
	array[CAB_BANK_B(CAB1)][0] = GET_CLOCK_SELECT_BYTE(CAB_Array[CAB1].Clock);
	array[CAB_BANK_B(CAB2)][0] = GET_CLOCK_SELECT_BYTE(CAB_Array[CAB2].Clock);
	array[CAB_BANK_B(CAB3)][0] = GET_CLOCK_SELECT_BYTE(CAB_Array[CAB3].Clock);
	array[CAB_BANK_B(CAB4)][0] = GET_CLOCK_SELECT_BYTE(CAB_Array[CAB4].Clock);
	return true;
}

// Configure the I/O control bits, which determine how each I/O module behaves (input, output, etc.)
bool Configurator::SetIOControlBits(an_Byte array[NUM_BANKS][BYTES_PER_BANK])
{
	// First, set the I/O control bits (bytes 0, 6, 8-31 of Bank 2)
	for (int i = 0; i < ARRAY_SIZE(IO_Modes); i++)
	{
		switch(IO_Modes[i])
		{
			case IO_MODE_INPUT:
				if (i < 4)
				{
					array[2][8 + ((3 - i) * 3)] = 0xF0;
					array[2][9 + ((3 - i) * 3)] = 0x82;
					array[2][10 + ((3 - i) * 3)] = 0x05;
				}
				else
				{
					switch (i)
					{
					case 4:
						SET_UPPER_4_BITS(array[2][6], 0x1);
						break;
					case 5:
						SET_LOWER_4_BITS(array[2][6], 0x1);
						break;
					case 6:
						SET_UPPER_4_BITS(array[2][0], 0x1);
						break;
					default:
						Message::ErrorMessage("Unknown I/O port");
						return false;
					}
				}
				break;
			case IO_MODE_OUTPUT:
				if (i < 4)
				{
					array[2][8 + ((3 - i) * 3)] = 0x00;
					array[2][9 + ((3 - i) * 3)] = 0x00;
					array[2][10 + ((3 - i) * 3)] = 0x10;
				}
				else
				{
					switch (i)
					{
					case 4:
						SET_UPPER_4_BITS(array[2][6], 0xC);
						break;
					case 5:
						SET_LOWER_4_BITS(array[2][6], 0xC);
						break;
					case 6:
						SET_UPPER_4_BITS(array[2][0], 0xC);
						break;
					default:
						Message::ErrorMessage("Unknown I/O port");
						return false;
					}
				}
				break;
			case IO_MODE_NONE:
				break;
		}
	}
	return true;
	// The other I/O routing bits will be set in the routing configuration function
}

// Set the values which determine capacitor (gain) ratios
bool Configurator::SetCapRatioBits(an_Byte array[NUM_BANKS][BYTES_PER_BANK])
{
	// recall that each module should have already set its capacitor values when updating resource information

	// iterate through CABs, adding cap value to the array
	for (int whichCab = 0; whichCab < NUM_CABS; whichCab++)
	{
		for (int cap = 0; cap < CAPACITORS_PER_BANK; cap++)
		{
			if (Configurator::CAB_Array[whichCab].Capacitor_Values[cap] != NULL)
			{
				array[CAB_BANK_A(whichCab)][7 - cap] = *Configurator::CAB_Array[whichCab].Capacitor_Values[cap];
			}
		}
	}
	return true;
}

// Set all bits which route components in the circuit (Capacitors/OpAmps/Comparators/IO)
bool Configurator::SetRoutingBits(an_Byte array[NUM_BANKS][BYTES_PER_BANK])
{

// 1. Set I/O routing bits in Bank 2, --- AND ---
// 2. CAB output routing byte (Bank B byte 2)

	// Must iterate through the interconnects, not through the IOs, as some IOs will be used with multiple interconnects
	//   e.g. if we probe io1, which is carrying input to a module
	for (int whichInterconnect = 0; whichInterconnect < NUM_INTERCONNECTS_TOTAL; whichInterconnect++)
	{
		if (Interconnects_1_3[whichInterconnect]->isUsed)
		{
			if (Interconnects_1_3[whichInterconnect]->InputtingResourceSite->cabNumber == CABIO)
			{
				SET_B2_HALFBYTE_FOR_IO(Interconnects_1_3[whichInterconnect]->InputtingResourceSite->resourceNumber, Interconnects_1_3[whichInterconnect]->CAB, array, Interconnects_1_3[whichInterconnect]->Halfbyte_IO);
			}
			for (auto it = Interconnects_1_3[whichInterconnect]->OutputtingResourceSites.begin(); it != Interconnects_1_3[whichInterconnect]->OutputtingResourceSites.end(); it++)
			{
				if ((*it)->cabNumber == CABIO)
				{
					SET_B2_HALFBYTE_FOR_IO((*it)->resourceNumber, Interconnects_1_3[whichInterconnect]->CAB, array, Interconnects_1_3[whichInterconnect]->Halfbyte_IO);
				}
			}
		}

		if (Interconnects_2_4[whichInterconnect]->isUsed)
		{
			if (Interconnects_2_4[whichInterconnect]->InputtingResourceSite->cabNumber == CABIO)
			{
				SET_B2_HALFBYTE_FOR_IO(Interconnects_2_4[whichInterconnect]->InputtingResourceSite->resourceNumber, Interconnects_2_4[whichInterconnect]->CAB, array, Interconnects_2_4[whichInterconnect]->Halfbyte_IO);
			}
			for (auto it = Interconnects_2_4[whichInterconnect]->OutputtingResourceSites.begin(); it != Interconnects_2_4[whichInterconnect]->OutputtingResourceSites.end(); it++)
			{
				if ((*it)->cabNumber == CABIO)
				{
					SET_B2_HALFBYTE_FOR_IO((*it)->resourceNumber, Interconnects_2_4[whichInterconnect]->CAB, array, Interconnects_2_4[whichInterconnect]->Halfbyte_IO);
				}
			}
		}
	}

	// If we did our interconnect assignments correctly, we only need to consider a few possibilities
	for (uint8_t whichCab = 0; whichCab < NUM_CABS; whichCab++)
	{
		if ((!INTERCONNECTS_FOR_CAB(whichCab)[INTERCONNECT_ID_DEFAULT_OA1]->isUsed) && (!INTERCONNECTS_FOR_CAB(whichCab)[INTERCONNECT_ID_DEFAULT_OA2]->isUsed))
		{
			if ((INTERCONNECTS_FOR_CAB(whichCab)[INTERCONNECT_ID_DEFAULT_COMP]->isUsed) && (INTERCONNECTS_FOR_CAB(whichCab)[INTERCONNECT_ID_DEFAULT_COMP]->InputtingResourceSite->cabNumber != CABIO))
			{
				array[CAB_BANK_B(whichCab)][2] = 0x04;
			}
		}
		else
		{
			array[CAB_BANK_B(whichCab)][2] = 0x8C;
		}
	}

// 3. Set 4-byte capacitor routing bits in Banks A/B
	// for each CAB
	for (uint8_t whichCab = 0; whichCab < NUM_CABS; whichCab++)
	{
		// for each capacitor in the CAB
		for (uint8_t cap = 0; cap < CAPACITORS_PER_BANK; cap++)
		{
			// for each side of the capacitor
			for (uint8_t capside = 0; capside < 2; capside++)
			{
				ConnectionSite *site = CAB_Array[whichCab].Capacitor_Connections[cap][capside];

				// Leave as zeroes if cap is unused
				if (!site || (site->connectionType == TYPE_UNUSED))
				{
					continue;
				}

				uint8_t phase = CAB_Array[whichCab].Capacitor_Phasing[cap][capside];

				// Check for Op Amp connections within the same CAB
				if ((site->cabNumber == whichCab) && (site->resourceType == TYPE_OP_AMP))
				{
					if (site->connectionType == TYPE_IN)
					{
						array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 2] = (phase == PHASE_ALL) ? 0x00 : 0x01;
						if (phase == PHASE_1)
						{
							array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 3] = (site->resourceNumber == RESOURCE_1) ? 0x81 : 0x82;
						}
						else if (phase == PHASE_2)
						{
							array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 3] = (site->resourceNumber == RESOURCE_1) ? 0x18 : 0x28;
						}
						else
						{
							array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 3] = (site->resourceNumber == RESOURCE_1) ? 0x10 : 0x20;
						}
					}
					else if (site->connectionType == TYPE_OUT)
					{
						array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 0] = (phase == PHASE_ALL) ? 0x00 : 0x01;
						if (phase == PHASE_1)
						{
							array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 1] = (site->resourceNumber == RESOURCE_1) ? 0x13 : 0x12;
						}
						else if (phase == PHASE_2)
						{
							array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 1] = (site->resourceNumber == RESOURCE_1) ? 0x31 : 0x21;
						}
						else
						{
							array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 1] = (site->resourceNumber == RESOURCE_1) ? 0x30 : 0x20;
						}
					}
				}
				// Check for output from OAs in EXTERNAL CABs
				else if ((site->resourceType == TYPE_OP_AMP) && (site->cabNumber != whichCab))
				{
					array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 0] = (phase == PHASE_ALL) ? 0x00 : 0x01;
					array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 1] = GET_OA_CONNECTION_SWITCH_BYTE(GET_EXTERNAL_OA_HALFBYTE(whichCab, site->cabNumber, site->resourceNumber), phase);
				}
				
				// Check for voltage reference
				else if (site->resourceType == TYPE_VOLTAGE_REFERENCE)
				{
					if (site->resourceNumber == RESOURCE_1) //positive
					{
						array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 0] = (phase == PHASE_ALL) ? 0x00 : 0x01;
						array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 1] = GET_OA_CONNECTION_SWITCH_BYTE(0x05, phase);
					}
					else // negative
					{
						array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 0] = (phase == PHASE_ALL) ? 0x00 : 0x01;
						array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 1] = GET_OA_CONNECTION_SWITCH_BYTE(0x04, phase);
					}
				}

				// Check for I/Os to opamp inputs -- NOTE: This must be preceded by the configuration of the Bank 2 I/O routing bits!
				else if (site->resourceType == TYPE_IO && site->connectionType == TYPE_OUT && site->cabNumber == CABIO)
				{
					array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 0] = (phase == PHASE_ALL) ? 0x00 : 0x01;
					array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 1] = GET_OA_CONNECTION_SWITCH_BYTE(GET_IO_RESOURCE_HALFBYTE_FOR_B2_HALFBYTE(GET_B2_HALFBYTE_FOR_IO(site->resourceNumber, whichCab, array)), phase);
					
				}
				// Capacitor to comparator (e.g. variable reference)
				else if (site->resourceType == TYPE_COMPARATOR)
				{
					if (site->connectionType == TYPE_IN_POS) //positive
					{
						array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 2] = (phase == PHASE_ALL) ? 0x00 : 0x01;
						array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 3] = (phase == PHASE_2) ? 0x48 : 0x84;
					}
					else if (site->connectionType == TYPE_OUT)
					{
						array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 0] = (phase == PHASE_ALL) ? 0x00 : 0x01;
						array[CAP_ROUTING_BANK(whichCab, cap)][CAP_ROUTING_BYTES[cap] + 1] = GET_OA_CONNECTION_SWITCH_BYTE(0x7, phase);
					}
					else // negative
					{
						Message::ErrorMessage("Unknown comparator-capacitor connection");
						return false;
					}
				}

				// Set additional routing bits if necessary
				// if receiving input from an shared resource interconnect
				if ((IsEqualSite(site, INTERCONNECTS_FOR_CAB(whichCab)[INTERCONNECT_ID_5]->InputtingResourceSite)) && (!IsEqualSite(site, CAB_Array[whichCab].Capacitor_Modules[cap]->GetCurrentOutputSite())))
				{
					if ((array[CAB_BANK_B(whichCab)][5] != 0x01) && (array[CAB_BANK_B(whichCab)][4] != 0x01))
					{
						if (array[CAB_BANK_B(whichCab)][5] == 0x00)
						{
							array[CAB_BANK_B(whichCab)][5] = 0x01;
						}
						else if (array[CAB_BANK_B(whichCab)][4] == 0x00)
						{
							array[CAB_BANK_B(whichCab)][4] = 0x01;
						}
						else
						{
							Message::ErrorMessage("Cannot configure shared interconnects. Chip not configurable.");
							return false;
						}
					}
				}
				else if ((IsEqualSite(site, INTERCONNECTS_FOR_CAB(whichCab)[INTERCONNECT_ID_6]->InputtingResourceSite)) && (!IsEqualSite(site, CAB_Array[whichCab].Capacitor_Modules[cap]->GetCurrentOutputSite())))
				{
					if ((array[CAB_BANK_B(whichCab)][5] != 0x02) && (array[CAB_BANK_B(whichCab)][4] != 0x02))
					{
						if (array[CAB_BANK_B(whichCab)][5] == 0x00)
						{
							array[CAB_BANK_B(whichCab)][5] = 0x02;
						}
						else if (array[CAB_BANK_B(whichCab)][4] == 0x00)
						{
							array[CAB_BANK_B(whichCab)][4] = 0x02;
						}
						else
						{
							Message::ErrorMessage("Cannot configure shared interconnects. Chip not configurable.");
							return false;
						}
					}
				}
				else if ((IsEqualSite(site, INTERCONNECTS_FOR_CAB(whichCab)[INTERCONNECT_ID_7]->InputtingResourceSite)) && (!IsEqualSite(site, CAB_Array[whichCab].Capacitor_Modules[cap]->GetCurrentOutputSite())))
				{
					if ((array[CAB_BANK_B(whichCab)][5] != 0x04) && (array[CAB_BANK_B(whichCab)][4] != 0x04))
					{
						if (array[CAB_BANK_B(whichCab)][5] == 0x00)
						{
							array[CAB_BANK_B(whichCab)][5] = 0x04;
						}
						else if (array[CAB_BANK_B(whichCab)][4] == 0x00)
						{
							array[CAB_BANK_B(whichCab)][4] = 0x04;
						}
						else
						{
							Message::ErrorMessage("Cannot configure shared interconnects. Chip not configurable.");
							return false;
						}
					}
				}
			}
		}
	}

// 4. Op-Amp chopping (bytes 21, 27 of Bank B)
	// for now, assume we aren't chopping. if an op amp is active, set it to unchopped
	for (int whichCab = 0; whichCab < NUM_CABS; whichCab++)
	{
		for (int whichOA = 0; whichOA < OP_AMPS_PER_BANK; whichOA++)
		{
			if (Configurator::CAB_Array[whichCab].OpAmp_Modules[whichOA] != 0)
			{
				array[CAB_BANK_B(whichCab)][(whichOA == 0) ? 27 : 21] = 0x05;
			}
		}
	}

// 5. OP-Amp bypass switch (bytes 20, 26 of Bank B)
	// REMOVED


// 6. Comparator input + and - source (bytes 13 and 14 of Bank A)
// 7. Comparator external resource controls (bytes 6 and 11 of Bank B)
// 8. Comparator-comparator input switching (bytes A-14 and B-4, B-5, B-6)
	for (int whichCab = 0; whichCab < NUM_CABS; whichCab++)
	{
		if (CAB_Array[whichCab].Comparator_Modules[0] != NULL)
		{
			// Find the sources of each input
			BaseModule *compModule = CAB_Array[whichCab].Comparator_Modules[0];

			// if in variable reference mode, configuration is simple, as we take input from capacitors
			if (CAB_Array[whichCab].Comparator_Mode[0] == COMPARATOR_MODE_VARIABLE_REFERENCE)
			{
				array[CAB_BANK_A(whichCab)][15] = 0x08;
				continue;
			}

			int posInputCab = -1;
			int negInputCab = -1;
			bool exceptionCase = false;
			ConnectionSite *posSite;
			ConnectionSite *negSite;
			if (compModule->GetInputSiteByName(COMPARATOR_POSITIVE_INPUT_NAME)->input->size() > 0)
			{
				posInputCab = compModule->GetInputSiteByName(COMPARATOR_POSITIVE_INPUT_NAME)->input->front()->GetCurrentCab();
				posSite = compModule->GetInputSiteByName(COMPARATOR_POSITIVE_INPUT_NAME)->input->front()->GetCurrentOutputSite();
			}
			if (compModule->GetInputSiteByName(COMPARATOR_NEGATIVE_INPUT_NAME)->input->size() > 0)
			{
				negInputCab = compModule->GetInputSiteByName(COMPARATOR_NEGATIVE_INPUT_NAME)->input->front()->GetCurrentCab();
				negSite = compModule->GetInputSiteByName(COMPARATOR_NEGATIVE_INPUT_NAME)->input->front()->GetCurrentOutputSite();
			}
			
			// Same bank OAs - NEG
			if ((negInputCab == whichCab) && (negSite->resourceType != TYPE_COMPARATOR))
			{
				array[CAB_BANK_A(whichCab)][13] |= (negSite->resourceNumber == RESOURCE_1) ? 0b00010000 : 0b00001000;
			}
			// external CAB or comparator - NEG
			else if (negInputCab != -1)
			{
				if ((posInputCab != whichCab) || (posSite->resourceType == TYPE_COMPARATOR))
				{
					// weird exception case
					if (negInputCab == whichCab && (posInputCab != whichCab) && (posSite->resourceType == TYPE_COMPARATOR))
					{
						array[CAB_BANK_A(whichCab)][13] |= 0b00000001;
						array[CAB_BANK_B(whichCab)][6] = GET_COMPARATOR_EXTERNAL_RESOURCE_SELECTOR_BYTE(whichCab, negSite->resourceType, negSite->cabNumber, negSite->resourceNumber);
						exceptionCase = true;
					}
					else
					{
						array[CAB_BANK_A(whichCab)][14] |= 0b00000001;
						array[CAB_BANK_A(whichCab)][11] = GET_COMPARATOR_EXTERNAL_RESOURCE_SELECTOR_BYTE(whichCab, negSite->resourceType, negSite->cabNumber, negSite->resourceNumber);
					}
				}
				else
				{
					array[CAB_BANK_A(whichCab)][13] |= 0b00000001;
					array[CAB_BANK_B(whichCab)][6] = GET_COMPARATOR_EXTERNAL_RESOURCE_SELECTOR_BYTE(whichCab, negSite->resourceType, negSite->cabNumber, negSite->resourceNumber);
				}
			}

			// Same bank OAs - POS
			if ((posInputCab == whichCab) && (posSite->resourceType != TYPE_COMPARATOR))
			{
				array[CAB_BANK_A(whichCab)][13] |= 0b10000000;
				array[CAB_BANK_A(whichCab)][13] |= (posSite->resourceNumber == RESOURCE_1) ? 0b00000100 : 0b00000010;
			}
			// external CAB or comparator - POS
			else if (posInputCab != -1)
			{
				array[CAB_BANK_A(whichCab)][14] |= 0b00001000;
				array[CAB_BANK_B(whichCab)][exceptionCase ? 11 : 6] = GET_COMPARATOR_EXTERNAL_RESOURCE_SELECTOR_BYTE(whichCab, posSite->resourceType, posSite->cabNumber, posSite->resourceNumber);
			}

			// set comparator source bits if necessary
			if ((posInputCab != -1) && (negInputCab != -1) && (posSite->resourceType == TYPE_COMPARATOR) && (negSite->resourceType == TYPE_COMPARATOR))
			{
				array[CAB_BANK_A(whichCab)][14] |= 0b11000000;
				Message::ErrorMessageIfNotZero(array[CAB_BANK_B(whichCab)][4], "Not enough resources to configure this circuit.");
				Message::ErrorMessageIfNotZero(array[CAB_BANK_B(whichCab)][5], "Not enough resources to configure this circuit.");
				array[CAB_BANK_B(whichCab)][4] = 0x01;
				array[CAB_BANK_B(whichCab)][5] = 0x02;
			}
			else if (((posInputCab != -1) && (posSite->resourceType == TYPE_COMPARATOR)) || ((negInputCab != -1) && (negSite->resourceType == TYPE_COMPARATOR)))
			{
				array[CAB_BANK_A(whichCab)][14] |= 0b01000000;
				Message::ErrorMessageIfNotZero(array[CAB_BANK_B(whichCab)][4], "Not enough resources to configure this circuit.");
				array[CAB_BANK_B(whichCab)][4] = 0x01;
			}
		}
	}
	return true;
}

// Set the control bits for comparator configuration
bool Configurator::SetComparatorControlBits(an_Byte array[NUM_BANKS][BYTES_PER_BANK])
{
	// Set comparator mode configuration (bytes 9, 10, 11 of Bank B)
	for (int whichCab = 0; whichCab < NUM_CABS; whichCab++)
	{
		if (CAB_Array[whichCab].Comparator_Mode != 0)
		{
			array[CAB_BANK_B(whichCab)][9] |= 0b00000111;
			array[CAB_BANK_B(whichCab)][10] |= 0b00000001;

			if (*CAB_Array[whichCab].Comparator_Mode == COMPARATOR_MODE_DUAL)
			{
				array[CAB_BANK_B(whichCab)][9] |= 0b00100000;
			}
			else if (*CAB_Array[whichCab].Comparator_Mode == COMPARATOR_MODE_ZERO_CROSSING)
			{
				array[CAB_BANK_B(whichCab)][10] = 0x43;
				array[CAB_BANK_B(whichCab)][11] = 0x04;
			}
			else if (*CAB_Array[whichCab].Comparator_Mode == COMPARATOR_MODE_VARIABLE_REFERENCE)
			{
				array[CAB_BANK_B(whichCab)][11] = 0x02;
			}

			if (*CAB_Array[whichCab].Comparator_InputSamplingPhase == INPUT_SAMPLING_PHASE_1)
			{
				array[CAB_BANK_B(whichCab)][10] |= 0b00000001;
			}

			if (*CAB_Array[whichCab].Comparator_OutputPolarity == OUTPUT_POLARITY_INV)
			{
				array[CAB_BANK_B(whichCab)][9] |= 0b00011000;
			}

			if (*CAB_Array[whichCab].Comparator_Hysteresis == HYSTERESIS_10MV)
			{
				array[CAB_BANK_B(whichCab)][9] |= 0b01000000;
			}
		}
	}
	return true;
}

// Convert the raw (square) array of configuration values into a valid config data array for the chip
void Configurator::RawArrayToConfigData(an_Byte rawArray[NUM_BANKS][BYTES_PER_BANK], an_Byte configData[CONFIG_DATA_SIZE])
{
	// We need to split the massive array into two transfers, since we can only send 255 data bytes per transfer
	/* The header for the configuration stream */
	configData[0] = 0xD5; /* Synch     */
	configData[1] = 0xB7; /* JTAG0     */
	configData[2] = 0x20; /* JTAG1     */
	configData[3] = 0x01; /* JTAG2     */
	configData[4] = 0x00; /* JTAG3     */
	configData[5] = 0x01; /* Address1  */
	configData[6] = 0xC1; /* Control   */
	/* Start of data block */
	configData[7] = 0xC0; /* Byte address:  0 */
	configData[8] = 0x00; /* Bank address:  0 */
	configData[9] = 0xC0; /* Byte count:   192 */

	// Fill with data from the raw array
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < BYTES_PER_BANK; j++)
		{
			configData[(i << 5) + j + 10] = rawArray[i][j];
		}
	}

	configData[202] = 0x2A; /* Check Byte */

	/* Start of data block */
	configData[203] = 0x80; /* Byte address:  0 */
	configData[204] = 0x06; /* Bank address:  6 */
	configData[205] = 0xA0; /* Byte count:   160 */

	// Fill with data from the raw array
	for (int i = 6; i < 11; i++)
	{
		for (int j = 0; j < BYTES_PER_BANK; j++)
		{
			configData[(i << 5) + j + 14] = rawArray[i][j];
		}
	}

	configData[366] = 0x2A; /* Check Byte */
}