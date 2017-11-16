#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <set>
#include <stack>
#include <vector>
#include "ApiCode.h"
#include "BaseModule.h"
#include "Helpers.h"
#include "ConnectionSites.h"

class BaseModule;

class Configurator
{
protected:
	static std::set<BaseModule*> CurrentModules;
	static std::vector<BaseModule*> OrderedModules;
	static bool IsOrdered;
	static an_Byte LastGoodConfigData[CONFIG_DATA_SIZE];

public:
	static void Reset();
	static void ClearConfigurationData();
	static void AddModule(BaseModule *module);
	static void RemoveModule(BaseModule *module);
	static std::set<BaseModule*> GetCurrentModules();
	static std::vector<BaseModule*> GetOrderedModules();
	static BaseModule *GetDefaultSystemOutput();
	static BaseModule *GetDefaultSystemInput();
	static BaseModule *GetDefaultSystemProbe();

	static int GetNextModuleNumAndIncrement();
	static BaseModule *GetModuleWithName(std::string name); // returns NULL if not found

	static void InitIOs();
	static void SetIOMode(uint8_t which, uint8_t io_mode);

	static an_Byte(&GenerateConfigurationData(an_Byte (&array)[CONFIG_DATA_SIZE]))[CONFIG_DATA_SIZE];
	static an_Byte(&GenerateReconfigurationData(an_Byte(&array)[RECONFIG_DATA_SIZE], int whichCab))[RECONFIG_DATA_SIZE];

	static struct Interconnect {
		const uint8_t Interconnect_ID;
		uint8_t CAB;
		const uint8_t Halfbyte_IO;
		std::vector<ConnectionSite*> OutputtingResourceSites;
		ConnectionSite* InputtingResourceSite;
		bool isUsed;
	};

	static struct cab {
		uint8_t Capacitor_NumFree = CAPACITORS_PER_BANK;
		uint8_t OpAmp_NumFree = OP_AMPS_PER_BANK;
		uint8_t Comparator_NumFree = COMPARATORS_PER_BANK;
		ConnectionSite *Capacitor_Connections[CAPACITORS_PER_BANK][2];
		uint8_t Capacitor_Phasing[CAPACITORS_PER_BANK][2];
		uint8_t *Capacitor_Values[CAPACITORS_PER_BANK]; //= {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
		uint8_t *Comparator_Mode = 0;
		uint8_t *Comparator_InputSamplingPhase = 0;
		uint8_t *Comparator_OutputPolarity = 0;
		uint8_t *Comparator_Hysteresis = 0;
		BaseModule *Capacitor_Modules[CAPACITORS_PER_BANK] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		BaseModule *OpAmp_Modules[OP_AMPS_PER_BANK] = { 0, 0 };
		BaseModule *Comparator_Modules[COMPARATORS_PER_BANK] = { 0 };
		uint8_t Clock = CLOCK_FAST;
	} Cab1, Cab2, Cab3, Cab4;

	static cab CAB_Array[NUM_CABS];

	static Interconnect *Interconnects_1_3[NUM_INTERCONNECTS_TOTAL];
	static Interconnect *Interconnects_2_4[NUM_INTERCONNECTS_TOTAL];

	static uint8_t IO_Modes[NUM_IOS];
	static Interconnect *IO_Interconnects[NUM_IOS];
	static BaseModule* IOs[NUM_IOS];
	static ConnectionSite *InputsToOutputs[NUM_IOS];

	static Interconnect *Comp_Interconnects[COMPARATORS_PER_BANK * NUM_CABS];

private:
	static int currentModuleNum;

	static void UpdateResourcesForModule(BaseModule *module);
	static void SortModules();
	static void TopologicalSort(BaseModule *module, std::set<BaseModule*> *moduleSet, std::stack<BaseModule*> *stack);
	static bool IsConfigurableCircuit();

	// Setting bits of config array based on module data
	static bool DedicateInterconnectResources();
	static bool SetPreconfigurationBits(an_Byte array[NUM_BANKS][BYTES_PER_BANK]);
	static bool SetClockBits(an_Byte array[NUM_BANKS][BYTES_PER_BANK]);
	static bool SetIOControlBits(an_Byte array[NUM_BANKS][BYTES_PER_BANK]);
	static bool SetCapRatioBits(an_Byte array[NUM_BANKS][BYTES_PER_BANK]);
	static bool SetRoutingBits(an_Byte array[NUM_BANKS][BYTES_PER_BANK]);
	static bool SetComparatorControlBits(an_Byte array[NUM_BANKS][BYTES_PER_BANK]);

	static Interconnect *AssignInterconnect(uint8_t whichCab, ConnectionSite *inputSite, ConnectionSite *outputSite, Interconnect *specificInterconnect = NULL);
	static void RawArrayToConfigData(an_Byte rawArray[NUM_BANKS][BYTES_PER_BANK], an_Byte configData[CONFIG_DATA_SIZE]);

};

#endif
