#ifndef CAMCODE_H
#define CAMCODE_H

#include "ApiCode.h"

/********************************************************************\
*                      AnadigmDesigner2 C Code                       *
*--------------------------------------------------------------------*
*                                                                    *
*  File:      CAMCode.h                                              *
*  Circuit:   (Custom)Half_Rectifier.ad2                             *
*  Generated: November 03, 2016:  02:36 PM                           *
*  Version:   2.8.0.7 -  (Standard) - Release                        *
*  Copyright: Copyright © 2002 Anadigm. All rights reserved.         *
*                                                                    *
\********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

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
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     double an_fixed_setGainRectHold(an_CAM cam, double G);         
  /*                                                              *\
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

  /*--------------------------------------------------------------*\
  |                        setGainRectHold                         |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     double an_setGainRectHold(an_CAM cam, double G);               
  /*                                                              *\
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
|  Function Declaration                                          |
\*  ------------------------------                              */
	double an_fixed_setGainInv(an_CAM cam, double G);
	/*                                                              *\
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

	/*--------------------------------------------------------------*\
	|                           setGainInv                           |
	+----------------------------------------------------------------+
	|                                                                |
	|  Function Declaration                                          |
	\*  ------------------------------                              */
	double an_setGainInv(an_CAM cam, double G);
	/*                                                              *\
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


#ifdef __cplusplus
}
#endif

#endif /* CAMCODE_H */
