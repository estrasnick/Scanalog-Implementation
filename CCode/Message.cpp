#include "stdafx.h"
#include "Message.h"
#include <exception>

Message::Message()
{
}

Message::~Message()
{
}

void Message::ErrorMessage(std::string s)
{
	std::cerr << s;
	AfxGetMainWnd()->MessageBox(s.c_str(), "FPAA_Application", MB_OK | MB_ICONERROR);
	//throw std::runtime_error(s);
}

void Message::StandardMessage(std::string s)
{

}

void Message::ErrorMessageIfNotZero(an_Byte byte, std::string errorMessage)
{
	if (byte != 0x0)
	{
		ErrorMessage(errorMessage);
	}
}