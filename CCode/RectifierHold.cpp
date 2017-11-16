#include "stdafx.h"
#include "RectifierHold.h"

RectifierHold::RectifierHold()
{
   m_instance = an_HalfRectifier_RectifierHold1;
}

//-----------------------------------------------------------
//  A full fixed-point method for setting the gain of the module.  
//  It accepts floating-point inputs and returns a floating-point  
//  result.                                                       
//-----------------------------------------------------------
double RectifierHold::fixed_setGainRectHold(double G)
{
   return an_fixed_setGainRectHold(m_instance, G);
}

//-----------------------------------------------------------
//  A full floating point method for setting the gain of this     
//  module.                                                       
//-----------------------------------------------------------
double RectifierHold::setGainRectHold(double G)
{
   return an_setGainRectHold(m_instance, G);
}

