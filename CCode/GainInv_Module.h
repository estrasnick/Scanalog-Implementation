#ifndef GAININV_MODULE_H
#define GAININV_MODULE_H

#include "BaseModule.h"

class GainInv_Module : public BaseModule
{
protected:
	uint8_t OpAmp;
	uint8_t Cap_Cin;
	uint8_t Cap_Cout;
	uint8_t Cap_aCin;
	uint8_t Cap_aCout;
	an_Byte GainNumerator;
	an_Byte GainDenominator;

public:
	GainInv_Module(
		std::string moduleName = "",
		std::string simpleName = "",
		std::vector<ModuleInput*> inputs = std::vector<ModuleInput*>(),
		std::vector<ModuleOutput*> outputs = std::vector<ModuleOutput*>(),
		std::map<std::string, std::string> params = std::map<std::string, std::string>());

	virtual const type_info& GetDerivedClass();

	virtual void SetResourceConnections(uint8_t whichCab);

	virtual std::string GetModuleName();
	virtual std::string GetSimpleName();
	virtual std::string GetModuleInformation();

	virtual void SetParamValue(std::string key, std::string value);
	virtual std::string GetParamValue(std::string key);

private:
	void SetParam_Gain(double gain);
};
#endif
