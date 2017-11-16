#ifndef IO_H
#define IO_H


#include "BaseModule.h"
#include "Configurator.h"
#include <map>
#include "ApiCode.h"

class IO : public BaseModule
{
protected:
	bool ConnectedCabs[NUM_CABS_INCLUDING_IO];
	uint8_t WhichIO;

	virtual void SetResourceConnections(uint8_t whichCab);
	
public:
	IO(
		uint8_t whichIO,
		an_CAM cam,
		std::string moduleName = "",
		std::string simpleName = "",
		std::vector<ModuleInput*> inputs = std::vector<ModuleInput*>(),
		std::vector<ModuleOutput*> outputs = std::vector<ModuleOutput*>(),
		std::map<std::string, std::string> params = std::map<std::string, std::string>());
	void AddConnectedCAB(uint8_t whichCab);
	bool IsConnectedToCAB(uint8_t whichCab);
	void ResetConnectedCABs();
	virtual std::string GetModuleInformation();
};

#endif
