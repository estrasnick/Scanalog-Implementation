#ifndef HELPERS_H
#define HELPERS_H

#include "stdafx.h"
#include <stdint.h>
#include <stdio.h>
#include <sstream>
#include "Message.h"



/* Uncomment this switch to debug without hardware */

//#define DEBUG_WITHOUT_HARDWARE

/* ----------------------------------------- */




#define INTEGER_MAX_VALUE 2147483647
#define ARRAY_SIZE(x) ((sizeof x) / (sizeof *x))

#define SET_LOWER_4_BITS(x, y) x = (x & 0xF0) | (y & 0x0F);
#define SET_UPPER_4_BITS(x, y) x = (x & 0x0F) | ((y << 4) & 0xF0);

#define CAB_BANK_A(whichCab) (2 * whichCab) + 3
#define CAB_BANK_B(whichCab) (2 * whichCab) + 4

#define CAP_ROUTING_BANK(whichCab, cap) (cap % 2) == 0 ? CAB_BANK_B(whichCab) : CAB_BANK_A(whichCab)

#define INTERCONNECTS_FOR_CAB(whichCab) (((whichCab == CAB1) || (whichCab == CAB3)) ? Interconnects_1_3 : Interconnects_2_4)

#define CAPTURE_TIMER 1
#define SOCKET_TIMER 2
#define POLL_TRACE_TIMER 3

#define DEFAULT_PORT 1442
#define COMMAND_BUFFER_SIZE 1024
#define POLL_SOCKET_TIMER_MS 100

#define CAM_UNKNOWN 0
#define CAM_GAININV 1
#define CAM_COMPARATOR 2
#define CAM_FILTERBIQUAD 3
#define CAM_SUMFILTER 4

#define BYTES_PER_BANK 32
#define NUM_BANKS 11
#define CAPACITORS_PER_BANK 8
#define OP_AMPS_PER_BANK 2
#define COMPARATORS_PER_BANK 1
#define CONFIG_ARRAY_SIZE BYTES_PER_BANK * NUM_BANKS
#define CONFIG_DATA_SIZE CONFIG_ARRAY_SIZE + 15 // 1 preconfig header, plus two data headers and check bytes
#define RECONFIG_DATA_SIZE 15
#define NUM_CABS 4
#define NUM_CABS_INCLUDING_IO 5
#define NUM_IOS 7

#define CLOCK_SYS_FREQ 16000000 // THIS NUMBER SHOULD BE IN Hz
#define CLOCK_SLOW 0
#define CLOCK_MEDIUM 1
#define CLOCK_FAST 2

// REPLACE TO ENABLE CORRECT CLOCKING FOR FILTERS
#define CLOCK_SLOW_FREQ 16000000
#define CLOCK_MEDIUM_FREQ 16000000
#define CLOCK_FAST_FREQ 16000000
//#define CLOCK_SLOW_FREQ 31373
//#define CLOCK_MEDIUM_FREQ 2000000
//#define CLOCK_FAST_FREQ 16000000

#define NUM_INTERCONNECTS_TOTAL 11
#define NUM_INTERCONNECTS_NORMAL 7
#define NUM_INTERCONNECTS_INPUT_ONLY 4
#define INTERCONNECT_ID_1 0
#define INTERCONNECT_ID_2 1
#define INTERCONNECT_ID_3 2
#define INTERCONNECT_ID_4 3
#define INTERCONNECT_ID_5 4
#define INTERCONNECT_ID_6 5
#define INTERCONNECT_ID_7 6
#define INTERCONNECT_ID_8 7
#define INTERCONNECT_ID_9 8
#define INTERCONNECT_ID_10 9
#define INTERCONNECT_ID_11 10

#define INTERCONNECT_ID_DEFAULT_COMP INTERCONNECT_ID_5
#define INTERCONNECT_ID_DEFAULT_OA1 INTERCONNECT_ID_6
#define INTERCONNECT_ID_DEFAULT_OA2 INTERCONNECT_ID_7

#define INTERCONNECT_1_HALFBYTE  0x8
#define INTERCONNECT_2_HALFBYTE  0xC
#define INTERCONNECT_3_HALFBYTE  0x8
#define INTERCONNECT_4_HALFBYTE  0xC
#define INTERCONNECT_5_HALFBYTE  0x5
#define INTERCONNECT_6_HALFBYTE  0x6
#define INTERCONNECT_7_HALFBYTE  0x7
#define INTERCONNECT_8_HALFBYTE  0x1
#define INTERCONNECT_9_HALFBYTE  0x2
#define INTERCONNECT_10_HALFBYTE 0x1
#define INTERCONNECT_11_HALFBYTE 0x2

#define IO_MODE_NONE 0
#define IO_MODE_INPUT 1
#define IO_MODE_OUTPUT 2

#define TYPE_CAPACITOR 0
#define TYPE_OP_AMP 1
#define TYPE_COMPARATOR 2
#define TYPE_IO 3
#define TYPE_VOLTAGE_REFERENCE 4
#define TYPE_UNUSED 99

#define COMPARATOR_MODE_UNUSED 0
#define COMPARATOR_MODE_GROUND 1
#define COMPARATOR_MODE_DUAL 2
#define COMPARATOR_MODE_ZERO_CROSSING 3
#define COMPARATOR_MODE_VARIABLE_REFERENCE 4

#define CAB1 0
#define CAB2 1
#define CAB3 2
#define CAB4 3
#define CABIO 4
#define CABVREF 5
#define CAB_UNUSED 99

#define TYPE_IN 0
#define TYPE_OUT 1
#define TYPE_IN_POS 2
#define TYPE_IN_NEG 3
#define END1 4
#define END2 5

#define DEFAULT_IO_CONFIG {1,1,2,2,1,2,2}

#define OP_AMP_1 0
#define OP_AMP_2 1
#define CAP_1 0
#define CAP_2 1
#define CAP_3 2
#define CAP_4 3
#define CAP_5 4
#define CAP_6 5
#define CAP_7 6
#define CAP_8 7
#define COMPARATOR_1 0

#define RESOURCE_1 0
#define RESOURCE_2 1
#define RESOURCE_3 2
#define RESOURCE_4 3
#define RESOURCE_5 4
#define RESOURCE_6 5
#define RESOURCE_7 6
#define RESOURCE_8 7
#define RESOURCE_UNUSED 99

#define PHASE_ALL 0
#define PHASE_1 1
#define PHASE_2 2

#define DEFAULT_INPUT_NUMBER RESOURCE_1
#define DEFAULT_OUTPUT_NUMBER RESOURCE_4
#define DEFAULT_PROBE_NUMBER RESOURCE_3

#define INPUT_SAMPLING_PHASE_1 PHASE_1
#define INPUT_SAMPLING_PHASE_2 PHASE_2

#define OUTPUT_POLARITY_NONINV 0
#define OUTPUT_POLARITY_INV 1

#define HYSTERESIS_0MV 0
#define HYSTERESIS_10MV 1

#define DEFAULT_INPUT_NAME "in"
#define DEFAULT_OUTPUT_NAME "out"
#define INPUT_1_NAME "in1"
#define INPUT_2_NAME "in2"

#define COMPARATOR_POSITIVE_INPUT_NAME "in1"
#define COMPARATOR_NEGATIVE_INPUT_NAME "in2"

#define DEFAULT_COMPARATOR_MODE COMPARATOR_MODE_VARIABLE_REFERENCE
#define DEFAULT_INPUT_SAMPLING_PHASE INPUT_SAMPLING_PHASE_1
#define DEFAULT_OUTPUT_POLARITY OUTPUT_POLARITY_NONINV
#define DEFAULT_HYSTERESIS HYSTERESIS_10MV

#define MAX_VREF 2.0
#define MIN_VREF -2.0

#define FILTER_TYPE_LOWPASS 0
#define FILTER_TYPE_HIPASS 1

#define DEFAULT_FILTER_GAIN 1.0
#define DEFAULT_FILTER_CORNER_FREQUENCY 40.0 // THIS NUMBER SHOULD BE IN KHz
#define DEFAULT_FILTER_Q .707
#define DEFAULT_FILTER_TYPE FILTER_TYPE_LOWPASS
#define DEFAULT_SUMDIFF_CORNER_FREQUENCY 1600.0


/* -------------- SCOPE ---------------- */
#define MY_DEVICES 1 /* open one device only */
#define MY_PROBE_FILE "" /* default probe file if unspecified */

#define MY_DEVICE 0
#define MY_CHANNEL 0
#define MY_MODE BL_MODE_FAST
#define MY_RATE 2000000 /* capture sample rate */
#define MY_SIZE 2000 /* number of samples to capture -- max is 9999 */
#define MAX_CAPTURE_SIZE 12288
#define MAX_SAMPLING_RATE 2000000
#define MAX_TOTAL_SAMPLES 24576000000
#define TRIGGER_TIMEOUT_S .0005

#define PLOT_RECT_COORDS 850, 450, 1600, 1200 // x1, y1, x2, y2

#define DELAY_BETWEEN_CAPTURES_MS 100
#define POLL_TRACE_TIME_MS 10

/* ------------------------------------- */

static std::string DoubleToString(double d)
{
	std::ostringstream ss;
	ss << d;
	return ss.str();
}

static std::string IntToString(int i)
{
	std::ostringstream ss;
	ss << i;
	return ss.str();
}

static void LogStringData(std::string data, const char *filename = "logstring.txt")
{
	FILE * f;
	f = fopen(filename, "w");
	fprintf(f, "%s\n", data);
	fflush(f);
	fclose(f);
}

static void LogArrayData(double data[], int length, const char *filename = "logarray.txt")
{
	FILE * f;
	f = fopen(filename, "w");
	for (int i = 0; i < length; i++)
	{
		fprintf(f, "%f\n", data[i]);
	}
	fflush(f);
	fclose(f);
}

static void LogHexData(an_Byte data[], int dataSize)
{
	FILE * f;
	f = fopen("logout.txt", "w");
	for (int i = 0; i < dataSize; i++)
	{
		fprintf(f, "%02X\n", data[i]);
	}
	fflush(f);
	fclose(f);
}

static void LogFormattedConfigData(an_Byte configData[CONFIG_DATA_SIZE])
{
	FILE * f;
	f = fopen("configout.txt", "w");
	fprintf(f, "     ");
	for (int i = 0; i < 32; i++)
	{
		fprintf(f, "%02d ", i);
	}
	fprintf(f, "\n\n");
	for (int i = 0; i < 6; i++)
	{
		fprintf(f, "%2d   ", i);
		for (int j = 0; j < BYTES_PER_BANK; j++)
		{
			fprintf(f, "%02X ", configData[(i << 5) + j + 10]);
		}
		fprintf(f, "\n");
	}
	for (int i = 6; i < 11; i++)
	{
		fprintf(f, "%02d   ", i);
		for (int j = 0; j < BYTES_PER_BANK; j++)
		{
			fprintf(f, "%02X ", configData[(i << 5) + j + 14]);
		}
		fprintf(f, "\n");
	}
	fflush(f);
	fclose(f);
}


static long GET_FREQ_FOR_CLOCK(uint8_t whichClock)
{
	switch (whichClock)
	{
	case CLOCK_SLOW:
		return CLOCK_SLOW_FREQ;
		break;
	case CLOCK_MEDIUM:
		return CLOCK_MEDIUM_FREQ;
		break;
	case CLOCK_FAST:
		return CLOCK_FAST_FREQ;
		break;
	default:
		return CLOCK_FAST_FREQ;
		break;
	}
}

static uint8_t GET_CLOCK_SELECT_BYTE(uint8_t whichClock)
{
	switch (whichClock)
	{
	case CLOCK_SLOW:
		return 0x0C;
		break;
	case CLOCK_MEDIUM:
		return 0x0D;
		break;
	case CLOCK_FAST:
		return 0x0E;
		break;
	default:
		return CLOCK_FAST_FREQ;
		break;
	}
}

static uint8_t CAP_ROUTING_BYTES[CAPACITORS_PER_BANK] =
{
	28,
	28,
	22,
	24,
	16,
	20,
	12,
	16
};

// Return the bytes needed to set a capacitor end to a given op amp based on phasing
static an_Byte GET_OA_CONNECTION_SWITCH_BYTE(uint8_t halfbyte, uint8_t phasing)
{
	switch (phasing)
	{ 
	case PHASE_1:
		return halfbyte | 0x10;
		break;
	case PHASE_2:
		return (halfbyte << 4) | 0x01;
		break;
	case PHASE_ALL:
		return halfbyte << 4;
		break;
	default:
		Message::ErrorMessage("Unknown phase type");
		return 0;
		break;
	}
}

// Return the 4 bits in B2 corresponding to I/O routing for a given IO to a given CAB
static uint8_t GET_B2_HALFBYTE_FOR_IO(uint8_t whichIO, uint8_t whichCab, an_Byte array[NUM_BANKS][BYTES_PER_BANK])
{
	{
		switch (whichIO)
		{
		case RESOURCE_1:
			if (whichCab == CAB1 || whichCab == CAB3)
			{
				return (array[2][7] & 0xF0) >> 4;
			}
			else
			{
				return (array[2][5] & 0xF0) >> 4;
			}
			break;
		case RESOURCE_2:
			if (whichCab == CAB1 || whichCab == CAB3)
			{
				return array[2][7] & 0x0F;
			}
			else
			{
				return array[2][5] & 0x0F;
			}
			break;
		case RESOURCE_3:
			if (whichCab == CAB1 || whichCab == CAB3)
			{
				return (array[2][3] & 0xF0) >> 4;
			}
			else
			{
				return (array[2][1] & 0xF0) >> 4;
			}
			break;
		case RESOURCE_4:
			if (whichCab == CAB1 || whichCab == CAB3)
			{
				return array[2][3] & 0x0F;
			}
			else
			{
				return array[2][1] & 0x0F;
			}
			break;
		case RESOURCE_5:
			if (whichCab == CAB1 || whichCab == CAB3)
			{
				return (array[2][6] & 0xF0) >> 4;
			}
			else
			{
				return (array[2][4] & 0xF0) >> 4;
			}
			break;
		case RESOURCE_6:
			if (whichCab == CAB1 || whichCab == CAB3)
			{
				return array[2][6] & 0x0F;
			}
			else
			{
				return array[2][4] & 0x0F;
			}
			break;
		case RESOURCE_7:
			if (whichCab == CAB1 || whichCab == CAB3)
			{
				return (array[2][2] & 0xF0) >> 4;
			}
			else
			{
				return (array[2][0] & 0xF0) >> 4;
			}
			break;
		default:
			Message::ErrorMessage("Unknown I/O resource");
			return 0;
			break;
		}
	}
}

// Set to a given halfbyte the 4 bits in B2 corresponding to I/O routing for a given IO to a given CAB
static void SET_B2_HALFBYTE_FOR_IO(uint8_t whichIO, uint8_t whichCab, an_Byte array[NUM_BANKS][BYTES_PER_BANK], uint8_t halfByte)
{
	switch (whichIO)
	{
	case RESOURCE_1:
		if (whichCab == CAB1 || whichCab == CAB3)
		{
			SET_UPPER_4_BITS(array[2][7], halfByte);
		}
		else
		{
			SET_UPPER_4_BITS(array[2][5], halfByte);
		}
		break;
	case RESOURCE_2:
		if (whichCab == CAB1 || whichCab == CAB3)
		{
			SET_LOWER_4_BITS(array[2][7], halfByte);
		}
		else
		{
			SET_LOWER_4_BITS(array[2][5], halfByte);
		}
		break;
	case RESOURCE_3:
		if (whichCab == CAB1 || whichCab == CAB3)
		{
			SET_UPPER_4_BITS(array[2][3], halfByte);
		}
		else
		{
			SET_UPPER_4_BITS(array[2][1], halfByte);
		}
		break;
	case RESOURCE_4:
		if (whichCab == CAB1 || whichCab == CAB3)
		{
			SET_LOWER_4_BITS(array[2][3], halfByte);
		}
		else
		{
			SET_LOWER_4_BITS(array[2][1], halfByte);
		}
		break;
	case RESOURCE_5:
		if (whichCab == CAB1 || whichCab == CAB3)
		{
			SET_UPPER_4_BITS(array[2][6], halfByte);
		}
		else
		{
			SET_UPPER_4_BITS(array[2][4], halfByte);
		}
		break;
	case RESOURCE_6:
		if (whichCab == CAB1 || whichCab == CAB3)
		{
			SET_LOWER_4_BITS(array[2][6], halfByte);
		}
		else
		{
			SET_LOWER_4_BITS(array[2][4], halfByte);
		}
		break;
	case RESOURCE_7:
		if (whichCab == CAB1 || whichCab == CAB3)
		{
			SET_UPPER_4_BITS(array[2][2], halfByte);
		}
		else
		{
			SET_UPPER_4_BITS(array[2][0], halfByte);
		}
		break;
	default:
		Message::ErrorMessage("Unknown I/O resource");
		break;
	}
}

// Return the 4 bits which reference a given IO input when setting a capacitor end for an op-amp
static uint8_t GET_IO_RESOURCE_HALFBYTE_FOR_B2_HALFBYTE(uint8_t halfbyte)
{
	switch (halfbyte)
	{
	case 1:
		return 9;
		break;
	case 2:
		return 8;
		break;
	case 5:
		return 7;
		break;
	case 6:
		return 6;
		break;
	case 7:
		return 7;
		break;
	default:
		Message::ErrorMessage("Unknown I/O resource");
		return 0;
		break;
	}
}

// Return the 4 bits used to reference an op amp found in an external cab
static uint8_t GET_EXTERNAL_OA_HALFBYTE(uint8_t whichSrcCab, uint8_t whichCab, uint8_t whichOA)
{
	if (whichSrcCab == CAB1)
	{
		switch (whichCab)
		{
		case CAB2:
			return (whichOA == RESOURCE_1) ? 0x0F : 0x0E;
			break;
		case CAB3:
			return (whichOA == RESOURCE_1) ? 0x0B : 0x0A;
			break;
		case CAB4:
			return (whichOA == RESOURCE_1) ? 0x0D : 0x0C;
			break;
		default:
			Message::ErrorMessage("Unknown CAB number");
			return 0;
			break;
		}
	}
	else
	{
		switch (whichCab)
		{
		case CAB1:
			return (whichOA == RESOURCE_1) ? 0x0B : 0x0A;
			break;
		case CAB2:
			return (whichOA == RESOURCE_1) ? 0x09 : 0x08;
			break;
		case CAB3:
			return (whichOA == RESOURCE_1) ? 0x0D : 0x0C;
			break;
		case CAB4:
			return (whichOA == RESOURCE_1) ? 0x0F : 0x0E;
			break;
		default:
			Message::ErrorMessage("Unknown CAB number");
			return 0;
			break;
		}
	}
}

static an_Byte GET_COMPARATOR_EXTERNAL_RESOURCE_SELECTOR_BYTE(int whichCab, uint8_t externalResourceType, uint8_t externalCabNumber, uint8_t externalResourceNumber)
{
	if (externalResourceType == TYPE_OP_AMP)
	{
		if (whichCab == CAB1)
		{
			switch (externalCabNumber)
			{
				case CAB1:
					return 0;
					break;
				case CAB2:
					return (externalResourceNumber == RESOURCE_1) ? 0x01 : 0x02;
					break;
				case CAB3:
					return (externalResourceNumber == RESOURCE_1) ? 0x10 : 0x20;
					break;
				case CAB4:
					return (externalResourceNumber == RESOURCE_1) ? 0x04 : 0x08;
					break;
				default:
					Message::ErrorMessage("Unknown external resource CAB");
					return 0;
					break;
			}
		}
		else
		{
			switch (externalCabNumber)
			{
				case CAB1:
					return (externalResourceNumber == RESOURCE_1) ? 0x10 : 0x20;
					break;
				case CAB2:
					return (externalResourceNumber == RESOURCE_1) ? 0x40 : 0x80;
					break;
				case CAB3:
					return (externalResourceNumber == RESOURCE_1) ? 0x04 : 0x08;
					break;
				case CAB4:
					return (externalResourceNumber == RESOURCE_1) ? 0x01 : 0x02;
					break;
				default:
					Message::ErrorMessage("Unknown external resource CAB");
					return 0;
					break;
			}
		}
	}
	else if ((externalResourceType == TYPE_COMPARATOR) && (externalResourceNumber != whichCab))
	{
		return 0x40;
	}
	else
	{
		return 0x0;
	}
}


#endif