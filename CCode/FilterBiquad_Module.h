#ifndef FILTERBIQUAD_MODULE_H
#define FILTERBIQUAD_MODULE_H

#include "BaseModule.h"

class FilterBiquad_Module : public BaseModule
{
protected:
	uint8_t OpAmp1;
	uint8_t OpAmp2;
	uint8_t Cap_1;
	uint8_t Cap_2;
	uint8_t Cap_3;
	uint8_t Cap_4;
	uint8_t Cap_A;
	uint8_t Cap_B;
	uint8_t Cap_p;
	uint8_t Cap_pp;

	uint8_t Cap_1_val;
	uint8_t Cap_2_val;
	uint8_t Cap_3_val;
	uint8_t Cap_4_val;
	uint8_t Cap_A_val;
	uint8_t Cap_B_val;
	uint8_t Cap_p_val;
	uint8_t Cap_pp_val;

	uint8_t FilterType;
	uint8_t InputSamplingPhase;
	uint8_t OutputPolarity;
	double Gain;
	double CornerFrequency;
	double Q;

public:
	FilterBiquad_Module(
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
	void SetCapValues_Hipass(uint8_t whichCab);
	void SetCapValues_Lopass(uint8_t whichCab);

	void SetParam_FilterType(uint8_t type);
	void SetParam_InputSamplingPhase(uint8_t phase);
	void SetParam_OutputPolarity(uint8_t polarity);
	void SetParam_Gain(double gain);
	void SetParam_CornerFrequency(double cornerFrequency);
	void SetParam_Q(double q);
};
#endif
