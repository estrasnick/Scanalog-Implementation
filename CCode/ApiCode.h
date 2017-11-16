#ifndef APICODE_H
#define APICODE_H

/********************************************************************\
*                      AnadigmDesigner2 C Code                       *
*--------------------------------------------------------------------*
*                                                                    *
*  File:      ApiCode.h                                              *
*  Circuit:   (Custom)Half_Rectifier.ad2                             *
*  Generated: November 03, 2016:  02:36 PM                           *
*  Version:   2.8.0.7 -  (Standard) - Release                        *
*  Copyright: Copyright © 2002 Anadigm. All rights reserved.         *
*                                                                    *
\********************************************************************/

#include <stdlib.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

/*##################################################################*\
#                                                                    #
#                        Data and Definitions                        #
#                                                                    #
\*##################################################################*/

/* All basic data types required by the C code. */
typedef unsigned char        an_Byte;
typedef long                 an_Fixed;
typedef short                an_Component;
typedef an_Byte              an_Bool;
typedef an_Byte              an_Chip;
typedef an_Byte              an_CAM;

/* Enumerated type representing VortexControlByte */
typedef enum an_VortexControlByte
{
   an_VortexControlByte_SAFD        = 0x80, /* SAFD            */
   an_VortexControlByte_ResetAll    = 0x40, /* ResetAll        */
   an_VortexControlByte_InhibitRdbck = 0x20, /* InhibitRdbck    */
   an_VortexControlByte_Read        = 0x10, /* Read            */
   an_VortexControlByte_SReset      = 0x08, /* SReset          */
   an_VortexControlByte_EndExecute  = 0x04, /* EndExecute      */
   an_VortexControlByte_TESTCTRL    = 0x02, /* TESTCTRL        */
   an_VortexControlByte_Pullups     = 0x01, /* Pullups         */
   an_VortexControlByte_PUs_Ex      = 0x05  /* PUs_Ex          */
} an_VortexControlByte;

/* Enumerated type representing ApexControlByte */
typedef enum an_ApexControlByte
{
   an_ApexControlByte_EndExecute    = 0x80, /* EndExecute      */
   an_ApexControlByte_Pullups       = 0x40, /* Pullups         */
   an_ApexControlByte_ResetAll      = 0x20, /* ResetAll        */
   an_ApexControlByte_SAFD          = 0x10, /* SAFD            */
   an_ApexControlByte_UpdateClocks  = 0x01, /* UpdateClocks    */
   an_ApexControlByte_SReset        = 0x07, /* SReset          */
   an_ApexControlByte_PUs_UpClks    = 0x41, /* PUs_UpClks      */
   an_ApexControlByte_PUs_UpClks_Ex = 0xC1  /* PUs_UpClks_Ex   */
} an_ApexControlByte;

/* Enumerated type representing ApiReconfigState */
typedef enum an_ApiReconfigState
{
   an_ApiReconfigState_Initialized  = 0x01, /* Initialized     */
   an_ApiReconfigState_CRCEnabled   = 0x02  /* CRCEnabled      */
} an_ApiReconfigState;

/* If NULL has not yet been defined, do it now. */
#ifndef NULL
#define NULL 0
#endif

/* Just a definition for ease of code reading. */
#define an_apiInvalid        0xFF

/* Each chip is given a numeric ID to be used when calling C Code   
functions */                                                        
#define an_Gain_ID                   0 
#define an_HalfRectifier_ID          1 
#define an_LPF_ID					 2
#define an_HPF_ID					 3
#define an_LPF_HPF_ID				 4
#define an_LPF_HalfRectifier_ID      5
#define an_HPF_HalfRectifier_ID      6
#define an_LPF_HPF_HalfRectifier_ID  7
#define an_CUSTOM_INITIAL			 100
#define an_CUSTOM_GAININV            101
#define an_CUSTOM_VREFCOMPARATOR     102
#define an_CUSTOM_GROUNDCOMPARATOR     103

/* How many chips with programmable Chips there are. */             
#define an_apiChipCount                2

/* This is a new array to store the ids of the various chips, in order */
extern const an_Byte an_ChipIDs[an_apiChipCount];

/* This array maps the numeric ID assigned to each chip in the      
ApiCode.h file to the chip's Address1. */                           
extern const an_Byte an_apiAddress1[an_apiChipCount];

/* This structure contains all the information about the primary    
configuration of a chip */                                          
typedef struct an_apiPrimaryConfigInfo
{
   const an_Byte* data;
   short length;
} an_apiPrimaryConfigInfo;

/* These hold the initial configuration streams for the chips. */   
extern const an_Byte an_Gain_PrimaryConfigInfo[];

/* These hold the initial configuration streams for the chips. */
extern const an_Byte an_HalfRectifier_PrimaryConfigInfo[];

/* This array maps the numeric ID assigned to each chip in the      
ApiCode.h file to its primary configuration data. */                
extern const an_apiPrimaryConfigInfo an_apiPrimaryConfigData[an_apiChipCount];

/* This is the constant synchronizing header byte. */               
extern const an_Byte an_apiSynchByte;

/* This structure contains all the information about the current state  
of an update stream. */                                             
typedef struct an_apiReconfigInfo
{
   an_Byte* data; /* Pointer to the configuration data. */
   short length;   /* Length of the configuration data */
   short capacity; /* Amount of memory allocated */
   short lastUpdateIndex; /* Index of address1 of the last update. */
   an_ApiReconfigState flags; /* Internal state */
} an_apiReconfigInfo;

/* There is one of these structures for each programmable chip. */  
extern an_apiReconfigInfo an_Gain_ReconfigInfo; /* GainOfTwo */

/* There is one of these structures for each programmable chip. */
extern an_apiReconfigInfo an_HalfRectifier_ReconfigInfo; /* HalfRectifier */

/* This array maps the numeric ID assigned to each chip in the      
ApiCode.h fileto the apiReconfigInfo struct of that chip. */        
extern an_apiReconfigInfo* an_apiReconfigData[an_apiChipCount];

/* There is one of these buffers for each programmable chip. */     
extern an_Byte an_Gain_ReconfigBuffer[64];    /* GainOfTwo */

/* There is one of these buffers for each programmable chip. */
extern an_Byte an_HalfRectifier_ReconfigBuffer[64];    /* HalfRectifier */

/* This array maps the numeric ID assigned to each chip in the      
ApiCode.h file to the reconfiguration buffer of that chip. */       
extern an_Byte* an_apiReconfigBuffer[an_apiChipCount];

/* Each CAM that is controllable is given a numeric ID to be used when
calling C Code functions. */
#define an_Gain_GainInv1          0         /* "GainInv1" */   
#define an_HalfRectifier_RectifierHold1    1         /* "RectifierHold1" */

/* How many programmable CAMs there are. */                         
#define an_apiCAMCount                 2

/* This array maps the numeric ID assigned to each CAM in the       
ApiCode.h file to the numeric ID of the chip which the CAM is in. */  
extern const an_Byte an_apiChipFromCAM[an_apiCAMCount];

/* Component ID's for AnadigmApex\RectifierHold */
#define an_AnadigmApex_RectifierHold_Cin 0          /* Cin */
#define an_AnadigmApex_RectifierHold_Cout 1          /* Cout */

/* Component ID's for AnadigmApex\GainInv */
#define an_AnadigmApex_GainInv_Cin       0          /* Cin */
#define an_AnadigmApex_GainInv_Cout      1          /* Cout */
#define an_AnadigmApex_GainInv_aCin      2          /* aCin */
#define an_AnadigmApex_GainInv_aCout     3          /* aCout */

/* This is the data structure for a capacitor. */
typedef struct an_apiCapacitor
{
   an_Byte bank;
   an_Byte byteNum;
} an_apiCapacitor;


/* Component tables for RectifierHold1 */
extern const an_apiCapacitor an_HalfRectifier_RectifierHold1_Capacitor[2];

/* Component tables for GainInv1 */
extern const an_apiCapacitor an_Gain_GainInv1_Capacitor[4];

/* There is one of these tables for each programmable CAM. Each table  
maps the ID of a component to its ComponentFinder. */     
extern void* an_Gain_GainInv1_Components[4];
extern void* an_HalfRectifier_RectifierHold1_Components[2];

/* This array maps the numeric ID assigned to each CAM in the       
ApiCode.h file to its component map */                              
extern void** an_apiCAMComponentData[an_apiCAMCount];

/*##################################################################*\
#                                                                    #
#                             Functions                              #
#                                                                    #
\*##################################################################*/

/********************************************************************\
 *                      Primary Configuration                       * 
\********************************************************************/

  /*--------------------------------------------------------------*\
  |                      GetPrimaryConfigData                      |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     const an_Byte* an_GetPrimaryConfigData(an_Chip chip, int* count); 
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Retrieves the primary configuration data for the chip. The    |
  |  returned data begins with the synch byte and ends with the    |
  |  last error byte. There are no pad bytes included. The number  |
  |  of bytes in the configuration data is set in count.           |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                          GetResetData                          |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     const an_Byte* an_GetResetData(an_Chip chip, int* count);      
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Retrieves configuration data that will perform a soft reset   |
  |  on the chip. The returned data begins with the synch byte and |
  |  ends with the control byte. There are no pad bytes included.  |
  |  The number of bytes in the configuration data is set in       |
  |  count.                                                        |
  |                                                                |
  \*--------------------------------------------------------------*/

/********************************************************************\
 *                         Power Management                         * 
\********************************************************************/

  /*--------------------------------------------------------------*\
  |                       GetVortexSleepData                       |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     const an_Byte* an_GetVortexSleepData(an_Chip chip, int* count, an_Bool powerDown); 
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Retrieves configuration data that will put the chip to sleep. |
  |  If powerDown is non-zero, then all analog functions will be   |
  |  turned off except the crystal oscillator. The returned data   |
  |  begins with the synch byte and ends with the last error byte. |
  |  There are no pad bytes included. The number of bytes in the   |
  |  configuration data is set in count.                           |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                        GetApexSleepData                        |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     const an_Byte* an_GetApexSleepData(an_Chip chip, int* count, 
                                       an_Bool powerDown,
                                       an_Bool VMR,
                                       an_Bool watchDog
                                       ); 
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Retrieves configuration data that will put the chip to sleep. |
  |  If powerDown is non-zero, then the analog cells are powered   |
  |  DOWN. If VMR is non-zero, then VMR is powered UP. If watchDog |
  |  is non-zero, then the watchDog circuit is powered up. The     |
  |  returned data begins with the synch byte and ends with the    |
  |  last error byte. There are no pad bytes included. The number  |
  |  of bytes in the configuration data is set in count.           |
  |                                                                |
  \*--------------------------------------------------------------*/

/********************************************************************\
 *                         Reconfiguration                          * 
\********************************************************************/

  /*--------------------------------------------------------------*\
  |                  InitializeVortexReconfigData                  |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     void an_InitializeVortexReconfigData(an_Chip chip);            
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Configures memory for the reconfiguration data and does       |
  |  initial setup of the header. Must be called prior to using    |
  |  any other reconfiguration functions for the chip.             |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                   InitializeApexReconfigData                   |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     void an_InitializeApexReconfigData(an_Chip chip);              
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Configures memory for the reconfiguration data and does       |
  |  initial setup of the header. Must be called prior to using    |
  |  any other reconfiguration functions for the chip.             |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                    ClearVortexReconfigData                     |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     void an_ClearVortexReconfigData(an_Chip chip);                 
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Resets the reconfiguration data for the chip back to what it  |
  |  was when ApiCall:InitializeVortexReconfigData was first       |
  |  called. This does not free any memory.                        |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                     ClearApexReconfigData                      |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     void an_ClearApexReconfigData(an_Chip chip);                   
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Resets the reconfiguration data for the chip back to what it  |
  |  was when ApiCall:InitializeApexReconfigData was first called. |
  |  This does not free any memory.                                |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                   ShutdownVortexReconfigData                   |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     void an_ShutdownVortexReconfigData(an_Chip chip);              
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Frees memory allocated by an_InitializeVortexReconfigData.    |
  |  an_InitializeVortexReconfigData must be called again prior to |
  |  using any other reconfiguration functions for the chip.       |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                    ShutdownApexReconfigData                    |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     void an_ShutdownApexReconfigData(an_Chip chip);                
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Frees memory allocated by an_InitializeApexReconfigData.      |
  |  an_InitializeApexReconfigData must be called again prior to   |
  |  using any other reconfiguration functions for the chip.       |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                     GetVortexReconfigData                      |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     const an_Byte* an_GetVortexReconfigData(an_Chip chip, int* count); 
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Retrieves the current reconfiguration data for the chip. The  |
  |  returned data begins with the synch byte and ends with the    |
  |  last error byte. There are no pad bytes included. The number  |
  |  of bytes in the reconfiguration is set in count. The data     |
  |  returned from this function is invalidated if another         |
  |  reconfiguration function is called for this chip. The data    |
  |  returned from this function should not be modified.           |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                      GetApexReconfigData                       |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     const an_Byte* an_GetApexReconfigData(an_Chip chip, int* count); 
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Retrieves the current reconfiguration data for the chip. The  |
  |  returned data begins with the synch byte and ends with the    |
  |  last error byte. There are no pad bytes included. The number  |
  |  of bytes in the reconfiguration is set in count. The data     |
  |  returned from this function is invalidated if another         |
  |  reconfiguration function is called for this chip. The data    |
  |  returned from this function should not be modified.           |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                 SetVortexReconfigControlFlags                  |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     void an_SetVortexReconfigControlFlags(an_Chip chip, an_VortexControlByte flags); 
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Sets flags in the control byte of the reconfiguration data    |
  |  for a Vortex chip.                                            |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                  SetApexReconfigControlFlags                   |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     void an_SetApexReconfigControlFlags(an_Chip chip, an_ApexControlByte flags); 
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Sets flags in the control byte of the reconfiguration data    |
  |  for an Apex chip.                                             |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                 SetApexReconfigControlFlagsNOP                 |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     void an_SetApexReconfigControlFlagsNOP(an_Chip chip, an_ApexControlByte flags); 
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Sets flags in the control byte of the reconfiguration data    |
  |  for an Apex chip.                                             |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                 GetVortexReconfigControlFlags                  |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     an_VortexControlByte an_GetVortexReconfigControlFlags(an_Chip chip); 
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Retrieves the flags in the control byte of the                |
  |  reconfiguration data for the chip.                            |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                  GetApexReconfigControlFlags                   |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     an_ApexControlByte an_GetApexReconfigControlFlags(an_Chip chip); 
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Retrieves the flags in the control byte of the                |
  |  reconfiguration data for the chip.                            |
  |                                                                |
  \*--------------------------------------------------------------*/

/********************************************************************\
 *                          CAM Functions                           * 
\********************************************************************/

  /*--------------------------------------------------------------*\
  |                          SetCapValue                           |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     void an_SetCapValue(an_CAM cam, an_Component capID, an_Byte value); 
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Set a capacitor to a given value. The new 8-bit value is      |
  |  value. capID is the name given to represent the capacitor in  |
  |  the CAM net list. This should only be called from within CAM  |
  |  functions, and is not intended for use otherwise.             |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                         ChooseCapRatio                         |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     void an_ChooseCapRatio(double dGain, an_Byte* pCap1, an_Byte* pCap2); 
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Given a desired gain, this function will calculate the ideal  |
  |  capacitor values. The realized capacitor values will be       |
  |  returned in the pointers that are passed in.                  |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                           AdjustCap                            |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     an_Byte an_AdjustCap(double dValue);                           
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Returns the 8-bit capacitor value most closely representing   |
  |  the passed in value.                                          |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                      FixedChooseCapRatio                       |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     void an_FixedChooseCapRatio(double dGain, an_Byte* pCap1, an_Byte* pCap2); 
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Given a desired gain, this function will calculate the ideal  |
  |  capacitor values. The realized capacitor values will be       |
  |  returned in the pointers that are passed in.                  |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                         FixedAdjustCap                         |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     an_Byte an_FixedAdjustCap(an_Fixed fValue);                    
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Returns the 8-bit capacitor value most closely representing   |
  |  the passed in value.                                          |
  |                                                                |
  \*--------------------------------------------------------------*/

/********************************************************************\
 *                         Fixed-Point Math                         * 
\********************************************************************/

  /*--------------------------------------------------------------*\
  |                          FloatToFixed                          |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     an_Fixed an_FloatToFixed(float fValue);                        
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Convert from float to ApiName::fixed                          |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                           IntToFixed                           |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     an_Fixed an_IntToFixed(int value);                             
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Convert from int to ApiName::fixed                            |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                           FixedToInt                           |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     int an_FixedToInt(an_Fixed fValue);                            
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Convert from ApiName::fixed to int                            |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                          FixedToFloat                          |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     float an_FixedToFloat(an_Fixed fValue);                        
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Convert from ApiName::fixed to float                          |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                            FixedAbs                            |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     an_Fixed an_FixedAbs(an_Fixed fValue);                         
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  This function returns the absolute value of a fixed-point     |
  |  number.                                                       |
  |                                                                |
  \*--------------------------------------------------------------*/

  /*--------------------------------------------------------------*\
  |                          FixedDivide                           |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     an_Fixed an_FixedDivide(an_Fixed fLeft, an_Fixed fRight);      
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  This function returns the result of dividing two fixed-point  |
  |  numbers. Input fLeft is the left-side of the implied division |
  |  sign. It is the first value to divide. Input fRight is the    |
  |  right-side of the implied division sign. It is the value to   |
  |  divide the first value by. The fixed-point numbers are        |
  |  assumed to be in 16:16 form, although the routine might work  |
  |  for any other fixed-point setup, as long as the two values    |
  |  are identical in their fixed-point setup. But this has not    |
  |  been tested. Division of fixed-point numbers is defined as    |
  |  dividing the mantissas, and subtracting the exponents. Since  |
  |  we are implying 16:16 numbers, the subtraction of exponents   |
  |  would leave us with no fractional part. To solve this         |
  |  problem, we try to shift the numerator up by 16 bits.         |
  |  However, many compilers do not support 64-bit numbers, so we  |
  |  have to improvise a way to do the division without resorting  |
  |  to assembly language or unsupported types.                    |
  |                                                                |
  \*--------------------------------------------------------------*/

/********************************************************************\
 *                           Internal API                           * 
\********************************************************************/

  /*--------------------------------------------------------------*\
  |                     BuildReconfigDataBlock                     |
  +----------------------------------------------------------------+
  |                                                                |
  |  Function Declaration                                          |
  \*  ------------------------------                              */
     void an_BuildReconfigDataBlock(an_Chip chip, an_Byte bank, an_Byte byteNum, an_Byte* values, short count); 
  /*                                                              *\
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Called by other API functions and should not be directly      |
  |  called by the user. The heart of the reconfiguration API,     |
  |  this function packages an address and a list of values into   |
  |  the proper reconfiguration format, adhering to current        |
  |  auto-grow and CRC16 policies.                                 |
  |                                                                |
  \*--------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* APICODE_H */
