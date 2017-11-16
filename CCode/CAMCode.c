#include "CAMCode.h"

/********************************************************************\
*                      AnadigmDesigner2 C Code                       *
*--------------------------------------------------------------------*
*                                                                    *
*  File:      CAMCode.c                                              *
*  Circuit:   (Custom)Half_Rectifier.ad2                             *
*  Generated: November 03, 2016:  02:36 PM                           *
*  Version:   2.8.0.7 -  (Standard) - Release                        *
*  Copyright: Copyright © 2002 Anadigm. All rights reserved.         *
*                                                                    *
\********************************************************************/

/*##################################################################*\
#                                                                    #
#                         RectifierHold.cam                          #
#                                                                    #
\*##################################################################*/

 /*================================================================*\
 )                               Gain                               (
 \*================================================================*/

  /*--------------------------------------------------------------*\
  |                     fixed_setGainRectHold                      |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  A full fixed-point method for setting the gain of the module. |
  |  It accepts floating-point inputs and returns a floating-point |
  |  result.                                                       |
  |                                                                |
  |                                                                |
  |  Instance Name       cam                 chip                  |
  |  ------------------------------------------------------------  |
  |  RectifierHold1      an_GainOfTwo_RectifierHold1 an_Gain_ID       |
  |                                                                |
  \*--------------------------------------------------------------*/

     double an_fixed_setGainRectHold(an_CAM cam, double G)
     {
        an_Byte cap1;
        an_Byte cap2;
        
        an_FixedChooseCapRatio(G, &cap1, &cap2);
        
        an_SetCapValue(cam, an_AnadigmApex_RectifierHold_Cin,cap1);
        an_SetCapValue(cam, an_AnadigmApex_RectifierHold_Cout,cap2);
        return an_FixedToFloat(an_FixedDivide(an_IntToFixed(cap1),an_IntToFixed(cap2)));
     }

  /*--------------------------------------------------------------*\
  |                        setGainRectHold                         |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  A full floating point method for setting the gain of this     |
  |  module.                                                       |
  |                                                                |
  |                                                                |
  |  Instance Name       cam                 chip                  |
  |  ------------------------------------------------------------  |
  |  RectifierHold1      an_GainOfTwo_RectifierHold1 an_Gain_ID       |
  |                                                                |
  \*--------------------------------------------------------------*/

     double an_setGainRectHold(an_CAM cam, double G)
     {
        an_Byte cap1;
        an_Byte cap2;
        
        an_ChooseCapRatio(G, &cap1, &cap2);
        
        an_SetCapValue(cam, an_AnadigmApex_RectifierHold_Cin,cap1);
        an_SetCapValue(cam, an_AnadigmApex_RectifierHold_Cout,cap2);
        
        return (double) cap1 / (double) cap2;
     }

/*##################################################################*\
#                                                                    #
#                            GainInv.cam                             #
#                                                                    #
\*##################################################################*/

/*================================================================*\
)                               Gain                               (
\*================================================================*/

/*--------------------------------------------------------------*\
|                        fixed_setGainInv                        |
+----------------------------------------------------------------+
|                                                                |
|  Description                                                   |
|  ------------------------------                                |
|  A full fixed-point method for setting the gain of the module. |
|  It accepts floating-point inputs and returns a floating-point |
|  result.                                                       |
|                                                                |
|                                                                |
|  Instance Name       cam                 chip                  |
|  ------------------------------------------------------------  |
|  GainInv1            an_Gain_GainInv1 an_Gain_ID        |
|                                                                |
\*--------------------------------------------------------------*/

double an_fixed_setGainInv(an_CAM cam, double G)
{
	an_Byte cap1;
	an_Byte cap2;

	an_FixedChooseCapRatio(G, &cap1, &cap2);

	an_SetCapValue(cam, an_AnadigmApex_GainInv_Cin, cap1);
	an_SetCapValue(cam, an_AnadigmApex_GainInv_aCin, cap1);
	an_SetCapValue(cam, an_AnadigmApex_GainInv_Cout, cap2);
	an_SetCapValue(cam, an_AnadigmApex_GainInv_aCout, cap2);
	return an_FixedToFloat(an_FixedDivide(an_IntToFixed(cap1), an_IntToFixed(cap2)));
}

/*--------------------------------------------------------------*\
|                           setGainInv                           |
+----------------------------------------------------------------+
|                                                                |
|  Description                                                   |
|  ------------------------------                                |
|  A full floating point method for setting the gain of this     |
|  module.                                                       |
|                                                                |
|                                                                |
|  Instance Name       cam                 chip                  |
|  ------------------------------------------------------------  |
|  GainInv1            an_Gain_GainInv1 an_Gain_ID        |
|                                                                |
\*--------------------------------------------------------------*/

double an_setGainInv(an_CAM cam, double G)
{
	an_Byte cap1;
	an_Byte cap2;

	an_ChooseCapRatio(G, &cap1, &cap2);

	an_SetCapValue(cam, an_AnadigmApex_GainInv_Cin, cap1);
	an_SetCapValue(cam, an_AnadigmApex_GainInv_aCin, cap1);
	an_SetCapValue(cam, an_AnadigmApex_GainInv_Cout, cap2);
	an_SetCapValue(cam, an_AnadigmApex_GainInv_aCout, cap2);

	return (double)cap1 / (double)cap2;
}


