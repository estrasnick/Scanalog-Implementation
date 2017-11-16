#include "ApiCode.h"

/********************************************************************\
*                      AnadigmDesigner2 C Code                       *
*--------------------------------------------------------------------*
*                                                                    *
*  File:      ApiCode.c                                              *
*  Circuit:   (Custom)Half_Rectifier.ad2                             *
*  Generated: November 03, 2016:  02:36 PM                           *
*  Version:   2.8.0.7 -  (Standard) - Release                        *
*  Copyright: Copyright © 2002 Anadigm. All rights reserved.         *
*                                                                    *
\********************************************************************/

/*##################################################################*\
#                                                                    #
#                        Data and Definitions                        #
#                                                                    #
\*##################################################################*/

/* This array maps the numeric ID assigned to each chip in the      
ApiCode.h file to the chip's Address1. */                           
const an_Byte an_apiAddress1[an_apiChipCount] =
{
   0x01,                 /* an_Gain_ID: 00000001 */
   0x01                  /* an_HalfRectifier_ID: 1011011 */
};

/* This is a new array to store the ids of the various chips, in order */
const an_Byte an_ChipIDs[an_apiChipCount] =
{
	an_Gain_ID,
	an_HalfRectifier_ID
};

/* This is the initial full configuration stream for GainOfTwo. */
const an_Byte an_Gain_PrimaryConfigInfo[] =
{
	/* The header for the configuration stream */
	0xD5, /* Synch     */
	0xB7, /* JTAG0     */
	0x20, /* JTAG1     */
	0x01, /* JTAG2     */
	0x00, /* JTAG3     */
	0x01, /* Address1  */
	0xC1, /* Control   */

	/* Start of data block */
	0xC4, /* Byte address:  4 */
	0x00, /* Bank address:  0 */
	0x0E, /* Byte count:   14 */

	/* Data bytes for block */
	0x20,  0x04,  0x00,  0x02,  0x05,  0x00,  0x00,  0x40,
	0x00,  0x00,  0x51,  0xFF,  0x0F,  0xF1,

	/* Check Byte: Inverse Synch */
	0x2A,

	/* Start of data block */
	0xC2, /* Byte address:  2 */
	0x01, /* Bank address:  1 */
	0x01, /* Byte count:    1 */

	/* Data bytes for block */
	0x40,

	/* Check Byte: Inverse Synch */
	0x2A,

	/* Start of data block */
	0xDE, /* Byte address: 30 */
	0x01, /* Bank address:  1 */
	0x0D, /* Byte count:   13 */

	/* Data bytes for block */
	0x02,  0xFF,  0x00,  0x00,  0x00,  0x05,  0x00,  0x00,
	0x00,  0x10,  0x00,  0x00,  0x10,

	/* Check Byte: Inverse Synch */
	0x2A,

	/* Start of data block */
	0xD1, /* Byte address: 17 */
	0x02, /* Bank address:  2 */
	0x03, /* Byte count:    3 */

	/* Data bytes for block */
	0xF0,  0x82,  0x05,

	/* Check Byte: Inverse Synch */
	0x2A,

	/* Start of data block */
	0xC4, /* Byte address:  4 */
	0x03, /* Bank address:  3 */
	0x04, /* Byte count:    4 */

	/* Data bytes for block */
	0xAA,  0xAA,  0xFF,  0xFF,

	/* Check Byte: Inverse Synch */
	0x2A,

	/* Start of data block */
	0xD8, /* Byte address: 24 */
	0x03, /* Bank address:  3 */
	0x0B, /* Byte count:   11 */

	/* Data bytes for block */
	0x01,  0x13,  0x01,  0x81,  0x01,  0x19,  0x01,  0x81,
	0x0C,  0x00,  0x01,

	/* Check Byte: Inverse Synch */
	0x2A,

	/* Start of data block */
	0x97, /* Byte address: 23 */
	0x04, /* Bank address:  4 */
	0x09, /* Byte count:    9 */

	/* Data bytes for block */
	0x30,  0x00,  0x10,  0x00,  0x05,  0x00,  0x90,  0x00,
	0x10,

	/* Check Byte: Inverse Synch */
	0x2A
};

/* This is the initial full configuration stream for HalfRectifier. */
const an_Byte an_HalfRectifier_PrimaryConfigInfo[] =
{
	/* The header for the configuration stream */
	0xD5, /* Synch     */
	0xB7, /* JTAG0     */
	0x20, /* JTAG1     */
	0x01, /* JTAG2     */
	0x00, /* JTAG3     */
	0x01, /* Address1  */
	0xC1, /* Control   */

	/* Start of data block */
	0xC4, /* Byte address:  4 */
	0x00, /* Bank address:  0 */
	0x0E, /* Byte count:   14 */

	/* Data bytes for block */
	0x20,  0x04,  0x00,  0x02,  0x05,  0x00,  0x00,  0x40,
	0x00,  0x00,  0x51,  0xFF,  0x0F,  0xF1,

	/* Check Byte: Inverse Synch */
	0x2A,

	/* Start of data block */
	0xC2, /* Byte address:  2 */
	0x01, /* Bank address:  1 */
	0x01, /* Byte count:    1 */

	/* Data bytes for block */
	0x40,

	/* Check Byte: Inverse Synch */
	0x2A,

	/* Start of data block */
	0xDE, /* Byte address: 30 */
	0x01, /* Bank address:  1 */
	0x0D, /* Byte count:   13 */

	/* Data bytes for block */
	0x02,  0xFF,  0x00,  0x00,  0x00,  0x05,  0x00,  0x00,
	0x00,  0x10,  0x00,  0x00,  0x10,

	/* Check Byte: Inverse Synch */
	0x2A,

	/* Start of data block */
	0xD1, /* Byte address: 17 */
	0x02, /* Bank address:  2 */
	0x03, /* Byte count:    3 */

	/* Data bytes for block */
	0xF0,  0x82,  0x05,

	/* Check Byte: Inverse Synch */
	0x2A,

	/* Start of data block */
	0xC5, /* Byte address:  5 */
	0x03, /* Bank address:  3 */
	0x03, /* Byte count:    3 */

	/* Data bytes for block */
	0xFF,  0xFF,  0xFF,

	/* Check Byte: Inverse Synch */
	0x2A,

	/* Start of data block */
	0xCE, /* Byte address: 14 */
	0x03, /* Bank address:  3 */
	0x01, /* Byte count:    1 */

	/* Data bytes for block */
	0x08,

	/* Check Byte: Inverse Synch */
	0x2A,

	/* Start of data block */
	0xDC, /* Byte address: 28 */
	0x03, /* Bank address:  3 */
	0x0F, /* Byte count:   15 */

	/* Data bytes for block */
	0x01,  0x13,  0x00,  0x10,  0x0C,  0x00,  0x01,  0x00,
	0x00,  0x00,  0x40,  0x00,  0x00,  0x03,  0x01,

	/* Check Byte: Inverse Synch */
	0x2A,

	/* Start of data block */
	0x97, /* Byte address: 23 */
	0x04, /* Bank address:  4 */
	0x09, /* Byte count:    9 */

	/* Data bytes for block */
	0x30,  0x01,  0x18,  0x00,  0x05,  0x1D,  0x19,  0x00,
	0x10,

	/* Check Byte: Inverse Synch */
	0x2A
};

/* This array maps the numeric ID assigned to each chip in the      
ApiCode.h file to its primary configuration data. */                
const an_apiPrimaryConfigInfo an_apiPrimaryConfigData[an_apiChipCount] =
{
   /* GainOfTwo */
   {
      an_Gain_PrimaryConfigInfo, /* data */
      90                            /* length */
   },
	/* HalfRectifier */
   {
	   an_HalfRectifier_PrimaryConfigInfo, /* data */
	   98                            /* length */
   }
};

/* This is the constant synchronizing header byte. */               
const an_Byte an_apiSynchByte = 0xD5;  /* 11010101 */

/* apiReconfigInfo for GainOfTwo */
an_apiReconfigInfo an_Gain_ReconfigInfo =
{
	NULL,   /* data */
	0,      /* length */
	64,     /* capacity */
	0,      /* lastUpdateIndex */
	0       /* flags */
};

/* apiReconfigInfo for HalfRectifier */
an_apiReconfigInfo an_HalfRectifier_ReconfigInfo =
{
   NULL,   /* data */
   0,      /* length */
   64,     /* capacity */
   0,      /* lastUpdateIndex */
   0       /* flags */
};

/* This array maps the numeric ID assigned to each chip in the      
ApiCode.h fileto the apiReconfigInfo struct of that chip. */        
an_apiReconfigInfo* an_apiReconfigData[an_apiChipCount] =
{
   &an_Gain_ReconfigInfo,     /* GainOfTwo */
   &an_HalfRectifier_ReconfigInfo     /* HalfRectifier */
};

/* There is one of these buffers for each programmable chip. */     
an_Byte an_Gain_ReconfigBuffer[64] = {0};     /* GainOfTwo */

/* There is one of these buffers for each programmable chip. */
an_Byte an_HalfRectifier_ReconfigBuffer[64] = { 0 };     /* HalfRectifier */

/* This array maps the numeric ID assigned to each chip in the      
ApiCode.h file to the reconfiguration buffer of that chip. */       
an_Byte* an_apiReconfigBuffer[an_apiChipCount] =
{
   an_Gain_ReconfigBuffer,    /* GainOfTwo */
   an_HalfRectifier_ReconfigBuffer /* HalfRectifier */
};

/* This array maps the numeric ID assigned to each CAM in the       
ApiCode.h file to the numeric ID of the chip which the CAM is in. */  
const an_Byte an_apiChipFromCAM[an_apiCAMCount] =
{
   an_Gain_ID,         /* "an_GainOfTwo_RectifierHold1" in chip "GainOfTwo" */
   an_HalfRectifier_ID         /* "an_HalfRectifier_RectifierHold1" in chip "HalfRectifier" */
};

/* Component data table for the apiCapacitor's of RectifierHold1. */
extern const an_apiCapacitor an_HalfRectifier_RectifierHold1_Capacitor[2] = 
{
   /* Cin   */ { 0x03, 0x07, }, /* 3, 7 */
   /* Cout  */ { 0x03, 0x06  }  /* 3, 6 */
};

/* Component data table for the apiCapacitor's of GainInv1. */
extern const an_apiCapacitor an_Gain_GainInv1_Capacitor[4] =
{
	/* Cin   */{ 0x03, 0x06, }, /* 3, 6 */
	/* Cout  */{ 0x03, 0x04, }, /* 3, 4 */
	/* aCin  */{ 0x03, 0x07, }, /* 3, 7 */
	/* aCout */{ 0x03, 0x05 }  /* 3, 5 */
};

/* Component map for RectifierHold1 */
void* an_HalfRectifier_RectifierHold1_Components[2] = 
{
   (void*) &an_HalfRectifier_RectifierHold1_Capacitor[0],  /* Cin */
   (void*) &an_HalfRectifier_RectifierHold1_Capacitor[1]  /* Cout */
};

/* Component map for GainInv1 */
void* an_Gain_GainInv1_Components[4] =
{
	(void*)&an_Gain_GainInv1_Capacitor[0],  /* Cin */
	(void*)&an_Gain_GainInv1_Capacitor[1],  /* Cout */
	(void*)&an_Gain_GainInv1_Capacitor[2],  /* aCin */
	(void*)&an_Gain_GainInv1_Capacitor[3]  /* aCout */
};


/* This array maps the numeric ID assigned to each CAM in the       
ApiCode.h file to its component map */                              
void** an_apiCAMComponentData[an_apiCAMCount] =
{
   an_Gain_GainInv1_Components,         /* an_Gain_GainInv1 */
   an_HalfRectifier_RectifierHold1_Components   /* an_GainOfTwo_RectifierHold1 */
};

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
  |  Description                                                   |
  |  ------------------------------                                |
  |  Retrieves the primary configuration data for the chip. The    |
  |  returned data begins with the synch byte and ends with the    |
  |  last error byte. There are no pad bytes included. The number  |
  |  of bytes in the configuration data is set in count.           |
  |                                                                |
  \*--------------------------------------------------------------*/

     const an_Byte* an_GetPrimaryConfigData(an_Chip chip, int* count)
     {
        /* Set how long it is */
        *count = an_apiPrimaryConfigData[chip].length;
        
        /* Return the pointer to the configuration data */
        return an_apiPrimaryConfigData[chip].data;
     }

  /*--------------------------------------------------------------*\
  |                          GetResetData                          |
  +----------------------------------------------------------------+
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

     const an_Byte* an_GetResetData(an_Chip chip, int* count)
     {
        /* Initialize our data template */
        static an_Byte resetData[] =
        {
           0xD5, /* Synch                 */
           0x00, /* Address1 Placeholder  */
           0x6F  /* Control Byte (48 Vortex | 27 Apex) */
        };
        
        /* Set it to use this chip's Address1 */
        resetData[1] = an_apiAddress1[chip];
        
        /* Set how long it is */
        *count = sizeof(resetData);
        
        /* Return the pointer to the data */
        return resetData;
     }

/********************************************************************\
 *                         Power Management                         * 
\********************************************************************/

  /*--------------------------------------------------------------*\
  |                       GetVortexSleepData                       |
  +----------------------------------------------------------------+
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

     const an_Byte* an_GetVortexSleepData(an_Chip chip, int* count, an_Bool powerDown)
     {
        /* Initialize our data template */
        static an_Byte sleepData[] =
        {
           0xD5,                            /* Synch                 */
           0x00,                            /* Address1 Placeholder  */
           an_VortexControlByte_EndExecute,   /* Control Byte          */
        
           /* Address: Byte 14, Bank 1 */
           /* The Byte value is required to have bit 7 set...) */
           0x8E,
           0x01,
        
           /* Data count and data byte place holder */
           0x01,
           0x00,
        
           /* Synch */
           0x2A
        };
        
        /* Set the proper power-up bit */
        sleepData[6] = powerDown ? 0x0 : 0x1;
        
        /* Set it to use this chip's Address1*/
        sleepData[1] = an_apiAddress1[chip];
        
        /* Set how long it is */
        *count = sizeof(sleepData);
        
        /* Return the pointer to the data */
        return sleepData;
     }

  /*--------------------------------------------------------------*\
  |                        GetApexSleepData                        |
  +----------------------------------------------------------------+
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

     const an_Byte* an_GetApexSleepData(an_Chip chip, int* count, 
                                       an_Bool powerDown,
                                       an_Bool VMR,
                                       an_Bool watchDog
                                       )
     {
        /* Initialize our data template */
        static an_Byte sleepData[] =
        {
           0xD5,                            /* Synch                 */
           0x00,                            /* Address1 Placeholder  */
           an_ApexControlByte_EndExecute,     /* Control Byte          */
        
           /* Address: Byte 17 (0x11), Bank 0 */
           0x91,
           0x00,
        
           /* Data count and data byte place holder */
           0x01,
           0x00,
        
           /* Synch */
           0x2A
        };
        
        /* Set the proper power-up bits */
        sleepData[6] = 0x80;                      /* Everything powered down value */
        if(!powerDown) sleepData[6]   += 0x01;    /* set analog cells power up bit */
        if(VMR) sleepData[6]          += 0x70;    /* set VMR bits power up         */
        if(watchDog) sleepData[6]     += 0x02;    /* set watchDog enable bit       */
        
        /* Set it to use this chip's Address1*/
        sleepData[1] = an_apiAddress1[chip];
        
        /* Set how long it is */
        *count = sizeof(sleepData);
        
        /* Return the pointer to the data */
        return sleepData;
     }

/********************************************************************\
 *                         Reconfiguration                          * 
\********************************************************************/

  /*--------------------------------------------------------------*\
  |                  InitializeVortexReconfigData                  |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Configures memory for the reconfiguration data and does       |
  |  initial setup of the header. Must be called prior to using    |
  |  any other reconfiguration functions for the chip.             |
  |                                                                |
  \*--------------------------------------------------------------*/

     void an_InitializeVortexReconfigData(an_Chip chip)
     {
        /* Get a pointer to the info struct */
        an_apiReconfigInfo* pInfo = an_apiReconfigData[chip];
        
        /* Initialize the pointer to the buffer */
        pInfo->data = an_apiReconfigBuffer[chip];
        
        /* Now do everything that needs to be done in a reset situation */
        an_ClearVortexReconfigData(chip);
     }

  /*--------------------------------------------------------------*\
  |                   InitializeApexReconfigData                   |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Configures memory for the reconfiguration data and does       |
  |  initial setup of the header. Must be called prior to using    |
  |  any other reconfiguration functions for the chip.             |
  |                                                                |
  \*--------------------------------------------------------------*/

     void an_InitializeApexReconfigData(an_Chip chip)
     {
        /* Get a pointer to the info struct */
        an_apiReconfigInfo* pInfo = an_apiReconfigData[chip];
        
        /* Initialize the pointer to the buffer */
        pInfo->data = an_apiReconfigBuffer[chip];
        
        /* Now do everything that needs to be done in a reset situation */
        an_ClearApexReconfigData(chip);
     }

  /*--------------------------------------------------------------*\
  |                    ClearVortexReconfigData                     |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Resets the reconfiguration data for the chip back to what it  |
  |  was when ApiCall:InitializeVortexReconfigData was first       |
  |  called. This does not free any memory.                        |
  |                                                                |
  \*--------------------------------------------------------------*/

     void an_ClearVortexReconfigData(an_Chip chip)
     {
        /* Get a pointer to the info struct */
        an_apiReconfigInfo* pInfo = an_apiReconfigData[chip];
        
        /* We don't have to remove any data, just adjust the info */
        an_apiReconfigData[chip]->length = 3;
        an_apiReconfigData[chip]->lastUpdateIndex = 0;
        
        /* Reset the stream info flags */
        pInfo->flags = an_ApiReconfigState_Initialized;
        
        /* Configure the data header for a reconfiguration */
        pInfo->data[0] = an_apiSynchByte;       /* Synch Byte   */
        pInfo->data[1] = an_apiAddress1[chip];  /* Address1    */
        pInfo->data[2] = an_VortexControlByte_EndExecute;     /* Control Byte */
     }

  /*--------------------------------------------------------------*\
  |                     ClearApexReconfigData                      |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Resets the reconfiguration data for the chip back to what it  |
  |  was when ApiCall:InitializeApexReconfigData was first called. |
  |  This does not free any memory.                                |
  |                                                                |
  \*--------------------------------------------------------------*/

     void an_ClearApexReconfigData(an_Chip chip)
     {
        /* Get a pointer to the info struct */
        an_apiReconfigInfo* pInfo = an_apiReconfigData[chip];
        
        /* We don't have to remove any data, just adjust the info */
        an_apiReconfigData[chip]->length = 3;
        an_apiReconfigData[chip]->lastUpdateIndex = 0;
        
        /* Reset the stream info flags */
        pInfo->flags = an_ApiReconfigState_Initialized;
        
        /* Configure the data header for a reconfiguration */
        pInfo->data[0] = an_apiSynchByte;       /* Synch Byte   */
        pInfo->data[1] = an_apiAddress1[chip];  /* Address1    */
        pInfo->data[2] = an_ApexControlByte_EndExecute;     /* Control Byte */
     }

  /*--------------------------------------------------------------*\
  |                   ShutdownVortexReconfigData                   |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Frees memory allocated by an_InitializeVortexReconfigData.    |
  |  an_InitializeVortexReconfigData must be called again prior to |
  |  using any other reconfiguration functions for the chip.       |
  |                                                                |
  \*--------------------------------------------------------------*/

     void an_ShutdownVortexReconfigData(an_Chip chip)
     {
        /* Get a pointer to the info struct */
        an_apiReconfigInfo* pInfo = an_apiReconfigData[chip];
        
        /* Update the stream info */
        pInfo->data = NULL;
        pInfo->length = 0;
        pInfo->flags  = 0;
     }

  /*--------------------------------------------------------------*\
  |                    ShutdownApexReconfigData                    |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Frees memory allocated by an_InitializeApexReconfigData.      |
  |  an_InitializeApexReconfigData must be called again prior to   |
  |  using any other reconfiguration functions for the chip.       |
  |                                                                |
  \*--------------------------------------------------------------*/

     void an_ShutdownApexReconfigData(an_Chip chip)
     {
        /* Get a pointer to the info struct */
        an_apiReconfigInfo* pInfo = an_apiReconfigData[chip];
        
        /* Update the stream info */
        pInfo->data = NULL;
        pInfo->length = 0;
        pInfo->flags  = 0;
     }

  /*--------------------------------------------------------------*\
  |                     GetVortexReconfigData                      |
  +----------------------------------------------------------------+
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

     const an_Byte* an_GetVortexReconfigData(an_Chip chip, int* count)
     {
        /* Get a pointer to the info struct */
        an_apiReconfigInfo* pInfo = an_apiReconfigData[chip];
        
        /* Set how big it is. If it is 3, then there is no real data. */
        *count = (pInfo->length == 3) ? 0 : pInfo->length;
        
        /* Return the pointer to the data */
        return pInfo->data;
     }

  /*--------------------------------------------------------------*\
  |                      GetApexReconfigData                       |
  +----------------------------------------------------------------+
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

     const an_Byte* an_GetApexReconfigData(an_Chip chip, int* count)
     {
        /* Get a pointer to the info struct */
        an_apiReconfigInfo* pInfo = an_apiReconfigData[chip];
        
        /* Set how big it is. If it is 3, then there is no real data. */
        *count = (pInfo->length == 3) ? 0 : pInfo->length;
        
        /* Return the pointer to the data */
        return pInfo->data;
     }

  /*--------------------------------------------------------------*\
  |                 SetVortexReconfigControlFlags                  |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Sets flags in the control byte of the reconfiguration data    |
  |  for a Vortex chip.                                            |
  |                                                                |
  \*--------------------------------------------------------------*/

     void an_SetVortexReconfigControlFlags(an_Chip chip, an_VortexControlByte flags)
     {
        an_apiReconfigInfo* pInfo = an_apiReconfigData[chip];
        
        /* Control byte is index 2 */
        pInfo->data[2] = flags;
     }

  /*--------------------------------------------------------------*\
  |                  SetApexReconfigControlFlags                   |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Sets flags in the control byte of the reconfiguration data    |
  |  for an Apex chip.                                             |
  |                                                                |
  \*--------------------------------------------------------------*/

     void an_SetApexReconfigControlFlags(an_Chip chip, an_ApexControlByte flags)
     {
        an_apiReconfigInfo* pInfo = an_apiReconfigData[chip];
        
        /* Control byte is index 2 */
        pInfo->data[2] = flags;
     }

  /*--------------------------------------------------------------*\
  |                 SetApexReconfigControlFlagsNOP                 |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Sets flags in the control byte of the reconfiguration data    |
  |  for an Apex chip.                                             |
  |                                                                |
  \*--------------------------------------------------------------*/

     void an_SetApexReconfigControlFlagsNOP(an_Chip chip, an_ApexControlByte flags)
     {
        
     }

  /*--------------------------------------------------------------*\
  |                 GetVortexReconfigControlFlags                  |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Retrieves the flags in the control byte of the                |
  |  reconfiguration data for the chip.                            |
  |                                                                |
  \*--------------------------------------------------------------*/

     an_VortexControlByte an_GetVortexReconfigControlFlags(an_Chip chip)
     {
        an_apiReconfigInfo* pInfo = an_apiReconfigData[chip];
        
        /* Control byte is index 2 */
        return pInfo->data[2];
     }

  /*--------------------------------------------------------------*\
  |                  GetApexReconfigControlFlags                   |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Retrieves the flags in the control byte of the                |
  |  reconfiguration data for the chip.                            |
  |                                                                |
  \*--------------------------------------------------------------*/

     an_ApexControlByte an_GetApexReconfigControlFlags(an_Chip chip)
     {
        an_apiReconfigInfo* pInfo = an_apiReconfigData[chip];
        
        /* Control byte is index 2 */
        return pInfo->data[2];
     }

/********************************************************************\
 *                          CAM Functions                           * 
\********************************************************************/

  /*--------------------------------------------------------------*\
  |                          SetCapValue                           |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Set a capacitor to a given value. The new 8-bit value is      |
  |  value. capID is the name given to represent the capacitor in  |
  |  the CAM net list. This should only be called from within CAM  |
  |  functions, and is not intended for use otherwise.             |
  |                                                                |
  \*--------------------------------------------------------------*/

     void an_SetCapValue(an_CAM cam, an_Component capID, an_Byte value)
     {
        /* First figure out what cap this is for */
        const an_apiCapacitor* pCap = an_apiCAMComponentData[cam][capID];
        
        /* And then just do a regular one-byte update */
        an_BuildReconfigDataBlock(an_apiChipFromCAM[cam], pCap->bank, pCap->byteNum, &value, 1);
     }

  /*--------------------------------------------------------------*\
  |                         ChooseCapRatio                         |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Given a desired gain, this function will calculate the ideal  |
  |  capacitor values. The realized capacitor values will be       |
  |  returned in the pointers that are passed in.                  |
  |                                                                |
  \*--------------------------------------------------------------*/

     void an_ChooseCapRatio(double dGain, an_Byte* pCap1, an_Byte* pCap2)
     {
        double realizedGain;
        double realizedError;
        double currentError = 99999;
        
        an_Byte capTry1;
        an_Byte capTry2;
        
        /* Start off with some default values */
        an_Byte currentCap1 = 1;
        an_Byte currentCap2 = 1;
        
        /* Run all the way down the scale looking for the best ratio... */
        for (capTry2 = 255; capTry2 > 0; capTry2--)
        {
           capTry1 = an_AdjustCap(dGain * capTry2);
        
           /* See how good it came out */
           realizedGain  = (double) capTry1 / (double) capTry2;
           realizedError = fabs( ( dGain - realizedGain) / dGain);
        
           /* If its the best so far, then store the results */
           if( realizedError < currentError ) 
           { 
              currentError = realizedError;
        
              currentCap1 = capTry1;
              currentCap2 = capTry2;
           }
        }
        
        *pCap1 = currentCap1;
        *pCap2 = currentCap2;
     }

  /*--------------------------------------------------------------*\
  |                           AdjustCap                            |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Returns the 8-bit capacitor value most closely representing   |
  |  the passed in value.                                          |
  |                                                                |
  \*--------------------------------------------------------------*/

     an_Byte an_AdjustCap(double dValue)
     {
        /* Start by just rounding.  We will handle special cases later */
        an_Byte capValue = (an_Byte) (dValue + 0.5);
        
        /* Constrain to 1 - 255 */
        if (dValue < 1.0)
        {
          capValue = 1;
        }
        else if (dValue > 255.0)
        {
          capValue = 255;
        }
         
        return capValue;
     }

  /*--------------------------------------------------------------*\
  |                      FixedChooseCapRatio                       |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Given a desired gain, this function will calculate the ideal  |
  |  capacitor values. The realized capacitor values will be       |
  |  returned in the pointers that are passed in.                  |
  |                                                                |
  \*--------------------------------------------------------------*/

     void an_FixedChooseCapRatio(double dGain, an_Byte* pCap1, an_Byte* pCap2)
     {
         an_Fixed realizedGain;
        an_Fixed workGain = an_FloatToFixed(dGain);
        an_Fixed realizedError;
        an_Fixed currentError = 0x7FFFFFFF;
         
        an_Byte capTry1;
        an_Byte capTry2;
        
        /* Start off with some default values */
        an_Byte currentCap1 = 1;
        an_Byte currentCap2 = 1;
        
        /* Run all the way down the scale looking for the best ratio... */
        for (capTry2 = 255; capTry2 > 0; capTry2--)
        {
           capTry1 = an_FixedAdjustCap(workGain * capTry2);
        
           /* See how good it came out */
           realizedGain = an_FixedDivide(an_IntToFixed(capTry1), an_IntToFixed(capTry2));
           realizedError = an_FixedDivide(an_FixedAbs(workGain - realizedGain), workGain);
        
           /* If it's the best so far, then store the results */
           if (realizedError < currentError)
           {
              currentError = realizedError;
        
              currentCap1 = capTry1;
              currentCap2 = capTry2;
           }
        }
        
        *pCap1 = currentCap1;
        *pCap2 = currentCap2;
     }

  /*--------------------------------------------------------------*\
  |                         FixedAdjustCap                         |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Returns the 8-bit capacitor value most closely representing   |
  |  the passed in value.                                          |
  |                                                                |
  \*--------------------------------------------------------------*/

     an_Byte an_FixedAdjustCap(an_Fixed fValue)
     {
        an_Byte capValue = (an_Byte) an_FixedToInt(fValue);
        
        if (fValue < 0x00010000)
        {
           capValue = 1;
        }
        else if (fValue > 0x00FF0000)
        {
           capValue = 255;
        }
        
        return capValue;
     }

/********************************************************************\
 *                         Fixed-Point Math                         * 
\********************************************************************/

  /*--------------------------------------------------------------*\
  |                          FloatToFixed                          |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Convert from float to ApiName::fixed                          |
  |                                                                |
  \*--------------------------------------------------------------*/

     an_Fixed an_FloatToFixed(float fValue)
     {
        return (an_Fixed) (fValue * 65536.0);
     }

  /*--------------------------------------------------------------*\
  |                           IntToFixed                           |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Convert from int to ApiName::fixed                            |
  |                                                                |
  \*--------------------------------------------------------------*/

     an_Fixed an_IntToFixed(int value)
     {
        return ((an_Fixed) (value)) << 16;
     }

  /*--------------------------------------------------------------*\
  |                           FixedToInt                           |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Convert from ApiName::fixed to int                            |
  |                                                                |
  \*--------------------------------------------------------------*/

     int an_FixedToInt(an_Fixed fValue)
     {
        return (int) ((fValue + 0x8000) >> 16);
     }

  /*--------------------------------------------------------------*\
  |                          FixedToFloat                          |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  Convert from ApiName::fixed to float                          |
  |                                                                |
  \*--------------------------------------------------------------*/

     float an_FixedToFloat(an_Fixed fValue)
     {
        //return (float) ((fValue + 0x8000) / 65536.0);
        return (float) (fValue / 65536.0);
     }

  /*--------------------------------------------------------------*\
  |                            FixedAbs                            |
  +----------------------------------------------------------------+
  |                                                                |
  |  Description                                                   |
  |  ------------------------------                                |
  |  This function returns the absolute value of a fixed-point     |
  |  number.                                                       |
  |                                                                |
  \*--------------------------------------------------------------*/

     an_Fixed an_FixedAbs(an_Fixed fValue)
     {
        /* Sign is indicated by the MSB of the value. If set, number is negative. */
        if ((fValue & 0x80000000) != 0)
        {
           return -fValue;
        }
        else
        {
           return fValue;
        }
     }

  /*--------------------------------------------------------------*\
  |                          FixedDivide                           |
  +----------------------------------------------------------------+
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

     an_Fixed an_FixedDivide(an_Fixed fLeft, an_Fixed fRight)
     {
        unsigned int left_neg = (fLeft & 0x80000000) != 0;
        unsigned int right_neg = (fRight & 0x80000000) != 0;
         
        /* Make sure we're using positive numbers for the division. We'll handle sign later. */
        unsigned long lefttemp = (left_neg ? -fLeft : fLeft);
        unsigned long righttemp = (right_neg ? -fRight : fRight);
         
        an_Fixed result = 0;
        unsigned char shiftbits = 16;
        
        /* We'll check the simple cases. */
        /* First, no integer component in the numerator (i.e. left < 1).
         * If this is the case, then we chop off the upper word (turn the number into a 0:32
         * number), and perform the division. */
        if ((lefttemp >> 16) == 0)	
           result = (lefttemp << 16) / righttemp;
        else
           /* Second, no fractional portion in the denominator (i.e. it's a whole number).
            * If this is the case, then we chop off the lower word (turn the number into a 32:0
            * number), and perform the division. */
           if ((righttemp << 16) == 0)
              result = lefttemp / (righttemp >> 16);
           else
           {
              /* Check to see if the numerator is less than the denominator.
               * If so, we'll shift up the numerator as far as we can of the 16 bits
               * we want to shift by, do the divison, then shift the result by the remaining
               * bits of the 16 bits. */
              if (lefttemp < righttemp)
              {
                 while ((shiftbits) && ((lefttemp & 0x80000000) == 0))
                 {
                    lefttemp <<= 1;
                    shiftbits--;
                 }
                 result = (lefttemp / righttemp) << shiftbits;
              }
              else
              {
                 /* Finally, if all else fails, we will do this the long way. Like elementary math,
                  * we will do this by repeatedly dividing, and using the remainder as part of the
                  * next division, until we have no remainder, or we run out of the 16-bits we need
                  * to do the 16:32 effect. */
                 while (((lefttemp & 0x80000000) == 0) &&
                        ((righttemp & 0x80000000) == 0) &&
                        ((righttemp & 0x0000FFFF) != 0))
                 {
                    lefttemp <<= 1;
                    righttemp <<= 1;
                 }
                 result = lefttemp / righttemp;
                 lefttemp = lefttemp % righttemp;
         
                 while ((lefttemp > 0) &&
                        (shiftbits > 0 ))
                 {
                    while (((lefttemp & 0x80000000) == 0) &&
                       (shiftbits != 0))
                    {
                       lefttemp <<= 1;
                       result <<= 1;
                       shiftbits--;
                    }
        
                    result += lefttemp / righttemp;
                    lefttemp = lefttemp % righttemp;
                 }
              }
           }
        
        /* Lastly, we check to see whether one, but not both, of the operators was negative.
         * If so, negate the result. */
        if (left_neg ^ right_neg)
           result = -result;
        
        return result;
     }

/********************************************************************\
 *                           Internal API                           * 
\********************************************************************/

  /*--------------------------------------------------------------*\
  |                     BuildReconfigDataBlock                     |
  +----------------------------------------------------------------+
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

     void an_BuildReconfigDataBlock(an_Chip chip, an_Byte bank, an_Byte byteNum, an_Byte* values, short count)
     {
        an_apiReconfigInfo* pInfo = an_apiReconfigData[chip];
        short updateIndex = 0;
        
        /* The size we expect this update to be.  This may change, depending on 
           update mode and CRC setting */
        short updateSize = 4 + count;
        
        /* If there was a previous update, we need to set its
           update to follow bit*/
        if (pInfo->lastUpdateIndex)
        {
           /* Set the update to follow bit */
           pInfo->data[pInfo->lastUpdateIndex] = pInfo->data[pInfo->lastUpdateIndex] | 0x40;
        }
        
        /* The address.  High bit must always be set. */
        pInfo->data[pInfo->length]     = byteNum | 0x80;
        pInfo->data[pInfo->length + 1] = bank;
        
        /* Number of bytes to write */
        pInfo->data[pInfo->length + 2] = (count == 256) ? 0 : count;
        
        /* The data */
        for (updateIndex = 0; updateIndex < count; updateIndex++)
        {
           pInfo->data[pInfo->length + (3 + updateIndex)] = values[updateIndex];
        }
        
        /* Error checking byte */
        pInfo->data[pInfo->length + (3 + count)] = ~an_apiSynchByte;
        
        /* Need to store where the address of this update is so we know for the next one */
        pInfo->lastUpdateIndex = pInfo->length;
        
        /* Update how much stuff is in this stream */
        pInfo->length = pInfo->length + updateSize;
     }

