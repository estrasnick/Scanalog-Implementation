#ifndef MESSAGE_H
#define MESSAGE_H

#include "ApiCode.h"
#include <string>
#include <iostream>

class Message
{
protected:

public:
	Message();

	virtual ~Message();

	static void ErrorMessage(std::string s);

	static void StandardMessage(std::string s);

	static void ErrorMessageIfNotZero(an_Byte byte, std::string errorMessage);
};

#endif
