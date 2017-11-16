#ifndef COMPARATOR_MODULE_H
#define COMPARATOR_MODULE_H

#include "BaseModule.h"

class Comparator_Module : public BaseModule
{
protected:
	uint8_t Comparator;
	uint8_t ComparatorMode;
	uint8_t InputSamplingPhase;
	uint8_t OutputPolarity;
	uint8_t Hysteresis;
	double Vref;

	uint8_t Cap_Vref_Numerator;
	uint8_t Cap_Vref_Denominator;
	an_Byte VrefNumerator;
	an_Byte VrefDenominator;

public:
	Comparator_Module(
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
	void SetParam_ComparatorMode(uint8_t comparatorMode);
	void SetParam_InputSamplingPhase(uint8_t phase);
	void SetParam_OutputPolarity(uint8_t polarity);
	void SetParam_Hysteresis(uint8_t hysteresis);
	void SetParam_Vref(double vref);
};
#endif
