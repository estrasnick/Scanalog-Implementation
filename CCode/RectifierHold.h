#pragma once

class RectifierHold
{
public:
	RectifierHold();

   //-----------------------------------------------------------
   //  A full fixed-point method for setting the gain of the module.  
   //  It accepts floating-point inputs and returns a floating-point  
   //  result.                                                       
   //-----------------------------------------------------------
   double fixed_setGainRectHold(double G);

   //-----------------------------------------------------------
   //  A full floating point method for setting the gain of this     
   //  module.                                                       
   //-----------------------------------------------------------
   double setGainRectHold(double G);

private:
   an_CAM m_instance;
};
