#ifndef SUMFILTER_MODULE_H
#define SUMFILTER_MODULE_H

#include "BaseModule.h"

class SumFilter_Module : public BaseModule
{
protected:
	uint8_t OpAmp;
	uint8_t Cap_inA;
	uint8_t Cap_inB;
	uint8_t Cap_out;
	uint8_t Cap_int;

	uint8_t Cap_inA_val;
	uint8_t Cap_inB_val;
	uint8_t Cap_out_val;
	uint8_t Cap_int_val;

	uint8_t Input1Sign;
	uint8_t Input2Sign;
	uint8_t OutputChangesOn;
	double Gain1;
	double Gain2;
	double CornerFrequency;

public:
	SumFilter_Module(
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
	void SetCapValues(uint8_t whichCab);

	void SetParam_Input1Sign(uint8_t sign);
	void SetParam_Input2Sign(uint8_t sign);
	void SetParam_OutputChangesOn(uint8_t phase);
	void SetParam_Gain1(double gain);
	void SetParam_Gain2(double gain);
	void SetParam_CornerFrequency(double cornerFrequency);
};
#endif
