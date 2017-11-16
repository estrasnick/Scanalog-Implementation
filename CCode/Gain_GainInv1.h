#pragma once

class Gain_GainInv1
{
public:
	Gain_GainInv1();

	//-----------------------------------------------------------
	//  A full fixed-point method for setting the gain of the module.  
	//  It accepts floating-point inputs and returns a floating-point  
	//  result.                                                       
	//-----------------------------------------------------------
	double fixed_setGainInv(double G);

	//-----------------------------------------------------------
	//  A full floating point method for setting the gain of this     
	//  module.                                                       
	//-----------------------------------------------------------
	double setGainInv(double G);

private:
	an_CAM m_instance;
};
