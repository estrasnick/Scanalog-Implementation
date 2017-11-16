#ifndef BASEMODULE_H
#define BASEMODULE_H

#include "ApiCode.h"
#include "Configurator.h"
#include "ConnectionSites.h"

#include <vector>
#include <map>

class BaseModule
{
public: 
	struct ModuleInput {
		std::vector<BaseModule*> *input;
		std::string inputSiteName;
	};

	struct ModuleOutput {
		std::vector<BaseModule*> *output;
		std::string outputSiteName;
	};
protected:

	std::vector<ModuleInput*> Inputs;
	std::vector<ModuleOutput*> Outputs;
	std::map<std::string, std::string> Params;

	std::string ModuleName;
	std::string SimpleName;

	an_CAM Cam;

	uint8_t NumCapacitors;
	uint8_t NumOpAmps;
	uint8_t NumComparators;

	uint8_t CurrentCab;
	ConnectionSite *CurrentOutputSite = NULL;

public:
	BaseModule(
		an_CAM cam,
		std::string defaultModuleName,
		std::string moduleName = "",
		std::string simpleName = "",
		std::vector<ModuleInput*> inputs = std::vector<ModuleInput*>(),
		std::vector<ModuleOutput*> outputs = std::vector<ModuleOutput*>(),
		std::map<std::string, std::string> params = std::map<std::string, std::string>());

	virtual ~BaseModule();

	virtual const type_info& GetDerivedClass();

	virtual void SetResourceConnections(uint8_t whichCab);

	void SetCam(an_CAM cam);

	an_CAM GetCam();

	// Do not call directly - use ConnectToOutput() instead
	void AddInput(BaseModule *newInput, ModuleInput *whichInputSite);
	// Do not call directly - use ConnectToOutput() instead
	void AddOutput(BaseModule *newOutput, ModuleOutput *whichOutputSite);

	void RemoveInput(BaseModule *input);
	void RemoveOutput(BaseModule *output);
	
	// Use when connecting two modules
	virtual void ConnectToOutput(BaseModule *moduleProvidingInput, ModuleInput *whichInputSite = NULL, ModuleOutput *whichOutputSite = NULL);

	// Adds the module to the default position in the current linear chain (just before the output)
	virtual void AddToEnd(ModuleInput *whichInputSiteOfThis = NULL, ModuleOutput *whichOutputSiteOfThis = NULL);

	// Adds the module between the two given modules
	virtual void AddBetween(BaseModule *in, BaseModule *out, ModuleInput *whichInputSiteOfThis = NULL, ModuleOutput *whichOutputSiteOfThis = NULL, ModuleOutput *whichOutputSiteFromIn = NULL, ModuleInput *whichInputSiteFromOut = NULL);

	const std::vector<ModuleInput*> &GetInputs();

	const std::vector<ModuleOutput*> &GetOutputs();

	ModuleInput *GetInputSiteByName(std::string inputSiteName);
	ModuleOutput *GetOutputSiteByName(std::string outputSiteName);

	virtual ModuleInput *GetDefaultInputSite();
	virtual ModuleOutput *GetDefaultOutputSite();

	virtual void SetParamValue(std::string key, std::string value);
	virtual std::string GetParamValue(std::string key);

	uint8_t GetNumCapacitors();
	uint8_t GetNumOpAmps();
	uint8_t GetNumComparators();

	uint8_t GetCurrentCab();
	ConnectionSite *GetCurrentOutputSite();

	virtual std::string GetModuleName();
	virtual std::string GetSimpleName();
	virtual std::string GetModuleInformation();
	virtual void SetModuleName(std::string newName);
	virtual void SetSimpleName(std::string newName);
};

#endif
