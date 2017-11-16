#ifndef CONNECTIONSITES_H
#define CONNECTIONSITES_H

#include "Helpers.h"
#include "Configurator.h"

enum ConnectionSites_t {
	UNUSED,

	IO1_IN,
	IO2_IN,
	IO3_IN,
	IO4_IN,
	IO5_IN,
	IO6_IN,
	IO7_IN,
	IO1_OUT,
	IO2_OUT,
	IO3_OUT,
	IO4_OUT,
	IO5_OUT,
	IO6_OUT,
	IO7_OUT,

	OA11_IN,
	OA12_IN,
	OA21_IN,
	OA22_IN,
	OA31_IN,
	OA32_IN,
	OA41_IN,
	OA42_IN,
	OA11_OUT,
	OA12_OUT,
	OA21_OUT,
	OA22_OUT,
	OA31_OUT,
	OA32_OUT,
	OA41_OUT,
	OA42_OUT,

	COMP1_IN_POS,
	COMP2_IN_POS,
	COMP3_IN_POS,
	COMP4_IN_POS,
	COMP1_IN_NEG,
	COMP2_IN_NEG,
	COMP3_IN_NEG,
	COMP4_IN_NEG,
	COMP1_OUT,
	COMP2_OUT,
	COMP3_OUT,
	COMP4_OUT,

	CAP11_END1,
	CAP12_END1,
	CAP13_END1,
	CAP14_END1,
	CAP15_END1,
	CAP16_END1,
	CAP17_END1,
	CAP18_END1,
	CAP21_END1,
	CAP22_END1,
	CAP23_END1,
	CAP24_END1,
	CAP25_END1,
	CAP26_END1,
	CAP27_END1,
	CAP28_END1,
	CAP31_END1,
	CAP32_END1,
	CAP33_END1,
	CAP34_END1,
	CAP35_END1,
	CAP36_END1,
	CAP37_END1,
	CAP38_END1,
	CAP41_END1,
	CAP42_END1,
	CAP43_END1,
	CAP44_END1,
	CAP45_END1,
	CAP46_END1,
	CAP47_END1,
	CAP48_END1,
	CAP11_END2,
	CAP12_END2,
	CAP13_END2,
	CAP14_END2,
	CAP15_END2,
	CAP16_END2,
	CAP17_END2,
	CAP18_END2,
	CAP21_END2,
	CAP22_END2,
	CAP23_END2,
	CAP24_END2,
	CAP25_END2,
	CAP26_END2,
	CAP27_END2,
	CAP28_END2,
	CAP31_END2,
	CAP32_END2,
	CAP33_END2,
	CAP34_END2,
	CAP35_END2,
	CAP36_END2,
	CAP37_END2,
	CAP38_END2,
	CAP41_END2,
	CAP42_END2,
	CAP43_END2,
	CAP44_END2,
	CAP45_END2,
	CAP46_END2,
	CAP47_END2,
	CAP48_END2,

	VOLTAGE_REFERENCE_POSITIVE,
	VOLTAGE_REFERENCE_NEGATIVE,

	DEFAULT
};

// SiteID, ResourceType, CabNumber, ResourceNumber, ConnectionType
struct ConnectionSite {
	ConnectionSites_t siteID;
	uint8_t resourceType;
	uint8_t cabNumber;
	uint8_t resourceNumber;
	uint8_t connectionType;
};

/*static bool IsEqualSite(ConnectionSite a, ConnectionSite b)
{
	return (a.resourceType == b.resourceType) && (a.cabNumber == b.cabNumber) && (a.resourceNumber == b.resourceNumber) && (a.connectionType == b.connectionType);
}*/

// Returns true iff the two sites are equal and not null
static bool IsEqualSite(ConnectionSite *a, ConnectionSite *b)
{
	if ((a == NULL) || (b == NULL))
	{
		return false;
	}
	return ((*a).resourceType == (*b).resourceType) && ((*a).cabNumber == (*b).cabNumber) && ((*a).resourceNumber == (*b).resourceNumber) && ((*a).connectionType == (*b).connectionType);
}

static ConnectionSite unused = {
	UNUSED,
	TYPE_UNUSED,
	CAB_UNUSED,
	RESOURCE_UNUSED,
	TYPE_UNUSED
};

static ConnectionSite *GetUnusedConnectionSite()
{
	return &unused;
}

static const ConnectionSite io1_in = {
	IO1_IN,
	TYPE_IO,
	CABIO,
	RESOURCE_1,
	TYPE_IN
};
static const ConnectionSite io2_in = {
	IO2_IN,
	TYPE_IO,
	CABIO,
	RESOURCE_2,
	TYPE_IN
};
static const ConnectionSite io3_in = {
	IO3_IN,
	TYPE_IO,
	CABIO,
	RESOURCE_3,
	TYPE_IN
};
static const ConnectionSite io4_in = {
	IO4_IN,
	TYPE_IO,
	CABIO,
	RESOURCE_4,
	TYPE_IN
};
static const ConnectionSite io5_in = {
	IO5_IN,
	TYPE_IO,
	CABIO,
	RESOURCE_5,
	TYPE_IN
};
static const ConnectionSite io6_in = {
	IO6_IN,
	TYPE_IO,
	CABIO,
	RESOURCE_6,
	TYPE_IN
};
static const ConnectionSite io7_in = {
	IO7_IN,
	TYPE_IO,
	CABIO,
	RESOURCE_7,
	TYPE_IN
};
static const ConnectionSite io1_out = {
	IO1_OUT,
	TYPE_IO,
	CABIO,
	RESOURCE_1,
	TYPE_OUT
};
static const ConnectionSite io2_out = {
	IO2_OUT,
	TYPE_IO,
	CABIO,
	RESOURCE_2,
	TYPE_OUT
};
static const ConnectionSite io3_out = {
	IO3_OUT,
	TYPE_IO,
	CABIO,
	RESOURCE_3,
	TYPE_OUT
};
static const ConnectionSite io4_out = {
	IO4_OUT,
	TYPE_IO,
	CABIO,
	RESOURCE_4,
	TYPE_OUT
};
static const ConnectionSite io5_out = {
	IO5_OUT,
	TYPE_IO,
	CABIO,
	RESOURCE_5,
	TYPE_OUT
};
static const ConnectionSite io6_out = {
	IO6_OUT,
	TYPE_IO,
	CABIO,
	RESOURCE_6,
	TYPE_OUT
};
static const ConnectionSite io7_out = {
	IO7_OUT,
	TYPE_IO,
	CABIO,
	RESOURCE_7,
	TYPE_OUT
};
static const ConnectionSite oa11_in = {
	OA11_IN,
	TYPE_OP_AMP,
	CAB1,
	RESOURCE_1,
	TYPE_IN
};
static const ConnectionSite oa12_in = {
	OA12_IN,
	TYPE_OP_AMP,
	CAB1,
	RESOURCE_1,
	TYPE_IN
};
static const ConnectionSite oa21_in = {
	OA21_IN,
	TYPE_OP_AMP,
	CAB2,
	RESOURCE_1,
	TYPE_IN
};
static const ConnectionSite oa22_in = {
	OA22_IN,
	TYPE_OP_AMP,
	CAB2,
	RESOURCE_2,
	TYPE_IN
};
static const ConnectionSite oa31_in = {
	OA31_IN,
	TYPE_OP_AMP,
	CAB3,
	RESOURCE_1,
	TYPE_IN
};
static const ConnectionSite oa32_in = {
	OA32_IN,
	TYPE_OP_AMP,
	CAB3,
	RESOURCE_2,
	TYPE_IN
};
static const ConnectionSite oa41_in = {
	OA41_IN,
	TYPE_OP_AMP,
	CAB4,
	RESOURCE_1,
	TYPE_IN
};
static const ConnectionSite oa42_in = {
	OA42_IN,
	TYPE_OP_AMP,
	CAB4,
	RESOURCE_2,
	TYPE_IN
};
static const ConnectionSite oa11_out = {
	OA11_OUT,
	TYPE_OP_AMP,
	CAB1,
	RESOURCE_1,
	TYPE_OUT
};
static const ConnectionSite oa12_out = {
	OA12_OUT,
	TYPE_OP_AMP,
	CAB1,
	RESOURCE_1,
	TYPE_OUT
};
static const ConnectionSite oa21_out = {
	OA21_OUT,
	TYPE_OP_AMP,
	CAB2,
	RESOURCE_1,
	TYPE_OUT
};
static const ConnectionSite oa22_out = {
	OA22_OUT,
	TYPE_OP_AMP,
	CAB2,
	RESOURCE_2,
	TYPE_OUT
};
static const ConnectionSite oa31_out = {
	OA31_OUT,
	TYPE_OP_AMP,
	CAB3,
	RESOURCE_1,
	TYPE_OUT
};
static const ConnectionSite oa32_out = {
	OA32_OUT,
	TYPE_OP_AMP,
	CAB3,
	RESOURCE_2,
	TYPE_OUT
};
static const ConnectionSite oa41_out = {
	OA41_OUT,
	TYPE_OP_AMP,
	CAB4,
	RESOURCE_1,
	TYPE_OUT
};
static const ConnectionSite oa42_out = {
	OA42_OUT,
	TYPE_OP_AMP,
	CAB4,
	RESOURCE_2,
	TYPE_OUT
};
static const ConnectionSite comp_1_in_pos = {
	COMP1_IN_POS,
	TYPE_COMPARATOR,
	CAB1,
	RESOURCE_1,
	TYPE_IN_POS
};
static const ConnectionSite comp_2_in_pos = {
	COMP2_IN_POS,
	TYPE_COMPARATOR,
	CAB2,
	RESOURCE_1,
	TYPE_IN_POS
};
static const ConnectionSite comp_3_in_pos = {
	COMP3_IN_POS,
	TYPE_COMPARATOR,
	CAB3,
	RESOURCE_1,
	TYPE_IN_POS
};
static const ConnectionSite comp_4_in_pos = {
	COMP4_IN_POS,
	TYPE_COMPARATOR,
	CAB4,
	RESOURCE_1,
	TYPE_IN_POS
};
static const ConnectionSite comp_1_in_neg = {
	COMP1_IN_NEG,
	TYPE_COMPARATOR,
	CAB1,
	RESOURCE_1,
	TYPE_IN_NEG
};
static const ConnectionSite comp_2_in_neg = {
	COMP2_IN_NEG,
	TYPE_COMPARATOR,
	CAB2,
	RESOURCE_1,
	TYPE_IN_NEG
};
static const ConnectionSite comp_3_in_neg = {
	COMP3_IN_NEG,
	TYPE_COMPARATOR,
	CAB3,
	RESOURCE_1,
	TYPE_IN_NEG
};
static const ConnectionSite comp_4_in_neg = {
	COMP4_IN_NEG,
	TYPE_COMPARATOR,
	CAB4,
	RESOURCE_1,
	TYPE_IN_NEG
};
static const ConnectionSite comp_1_out = {
	COMP1_OUT,
	TYPE_COMPARATOR,
	CAB1,
	RESOURCE_1,
	TYPE_OUT
};
static const ConnectionSite comp_2_out = {
	COMP2_OUT,
	TYPE_COMPARATOR,
	CAB2,
	RESOURCE_1,
	TYPE_OUT
};
static const ConnectionSite comp_3_out = {
	COMP3_OUT,
	TYPE_COMPARATOR,
	CAB3,
	RESOURCE_1,
	TYPE_OUT
};
static const ConnectionSite comp_4_out = {
	COMP4_OUT,
	TYPE_COMPARATOR,
	CAB4,
	RESOURCE_1,
	TYPE_OUT
};
static const ConnectionSite cap11_end1 = {
	CAP11_END1,
	TYPE_CAPACITOR,
	CAB1,
	RESOURCE_1,
	END1
};
static const ConnectionSite cap12_end1 = {
	CAP12_END1,
	TYPE_CAPACITOR,
	CAB1,
	RESOURCE_2,
	END1
};
static const ConnectionSite cap13_end1 = {
	CAP13_END1,
	TYPE_CAPACITOR,
	CAB1,
	RESOURCE_3,
	END1
};
static const ConnectionSite cap14_end1 = {
	CAP14_END1,
	TYPE_CAPACITOR,
	CAB1,
	RESOURCE_4,
	END1
};
static const ConnectionSite cap15_end1 = {
	CAP15_END1,
	TYPE_CAPACITOR,
	CAB1,
	RESOURCE_5,
	END1
};
static const ConnectionSite cap16_end1 = {
	CAP16_END1,
	TYPE_CAPACITOR,
	CAB1,
	RESOURCE_6,
	END1
};
static const ConnectionSite cap17_end1 = {
	CAP17_END1,
	TYPE_CAPACITOR,
	CAB1,
	RESOURCE_7,
	END1
};
static const ConnectionSite cap18_end1 = {
	CAP18_END1,
	TYPE_CAPACITOR,
	CAB1,
	RESOURCE_8,
	END1
};
static const ConnectionSite cap21_end1 = {
	CAP21_END1,
	TYPE_CAPACITOR,
	CAB2,
	RESOURCE_1,
	END1
};
static const ConnectionSite cap22_end1 = {
	CAP22_END1,
	TYPE_CAPACITOR,
	CAB2,
	RESOURCE_2,
	END1
};
static const ConnectionSite cap23_end1 = {
	CAP23_END1,
	TYPE_CAPACITOR,
	CAB2,
	RESOURCE_3,
	END1
};
static const ConnectionSite cap24_end1 = {
	CAP24_END1,
	TYPE_CAPACITOR,
	CAB2,
	RESOURCE_4,
	END1
};
static const ConnectionSite cap25_end1 = {
	CAP25_END1,
	TYPE_CAPACITOR,
	CAB2,
	RESOURCE_5,
	END1
};
static const ConnectionSite cap26_end1 = {
	CAP26_END1,
	TYPE_CAPACITOR,
	CAB2,
	RESOURCE_6,
	END1
};
static const ConnectionSite cap27_end1 = {
	CAP27_END1,
	TYPE_CAPACITOR,
	CAB2,
	RESOURCE_7,
	END1
};
static const ConnectionSite cap28_end1 = {
	CAP28_END1,
	TYPE_CAPACITOR,
	CAB2,
	RESOURCE_8,
	END1
};
static const ConnectionSite cap31_end1 = {
	CAP31_END1,
	TYPE_CAPACITOR,
	CAB3,
	RESOURCE_1,
	END1
};
static const ConnectionSite cap32_end1 = {
	CAP12_END1,
	TYPE_CAPACITOR,
	CAB3,
	RESOURCE_2,
	END1
};
static const ConnectionSite cap33_end1 = {
	CAP33_END1,
	TYPE_CAPACITOR,
	CAB3,
	RESOURCE_3,
	END1
};
static const ConnectionSite cap34_end1 = {
	CAP34_END1,
	TYPE_CAPACITOR,
	CAB3,
	RESOURCE_4,
	END1
};
static const ConnectionSite cap35_end1 = {
	CAP35_END1,
	TYPE_CAPACITOR,
	CAB3,
	RESOURCE_5,
	END1
};
static const ConnectionSite cap36_end1 = {
	CAP36_END1,
	TYPE_CAPACITOR,
	CAB3,
	RESOURCE_6,
	END1
};
static const ConnectionSite cap37_end1 = {
	CAP37_END1,
	TYPE_CAPACITOR,
	CAB3,
	RESOURCE_7,
	END1
};
static const ConnectionSite cap38_end1 = {
	CAP38_END1,
	TYPE_CAPACITOR,
	CAB3,
	RESOURCE_8,
	END1
};
static const ConnectionSite cap41_end1 = {
	CAP41_END1,
	TYPE_CAPACITOR,
	CAB4,
	RESOURCE_1,
	END1
};
static const ConnectionSite cap42_end1 = {
	CAP42_END1,
	TYPE_CAPACITOR,
	CAB4,
	RESOURCE_2,
	END1
};
static const ConnectionSite cap43_end1 = {
	CAP43_END1,
	TYPE_CAPACITOR,
	CAB4,
	RESOURCE_3,
	END1
};
static const ConnectionSite cap44_end1 = {
	CAP44_END1,
	TYPE_CAPACITOR,
	CAB4,
	RESOURCE_4,
	END1
};
static const ConnectionSite cap45_end1 = {
	CAP45_END1,
	TYPE_CAPACITOR,
	CAB4,
	RESOURCE_5,
	END1
};
static const ConnectionSite cap46_end1 = {
	CAP46_END1,
	TYPE_CAPACITOR,
	CAB4,
	RESOURCE_6,
	END1
};
static const ConnectionSite cap47_end1 = {
	CAP47_END1,
	TYPE_CAPACITOR,
	CAB4,
	RESOURCE_7,
	END1
};
static const ConnectionSite cap48_end1 = {
	CAP48_END1,
	TYPE_CAPACITOR,
	CAB4,
	RESOURCE_8,
	END1
};
static const ConnectionSite cap11_end2 = {
	CAP11_END2,
	TYPE_CAPACITOR,
	CAB1,
	RESOURCE_1,
	END2
};
static const ConnectionSite cap12_end2 = {
	CAP12_END2,
	TYPE_CAPACITOR,
	CAB1,
	RESOURCE_2,
	END2
};
static const ConnectionSite cap13_end2 = {
	CAP13_END2,
	TYPE_CAPACITOR,
	CAB1,
	RESOURCE_3,
	END2
};
static const ConnectionSite cap14_end2 = {
	CAP14_END2,
	TYPE_CAPACITOR,
	CAB1,
	RESOURCE_4,
	END2
};
static const ConnectionSite cap15_end2 = {
	CAP15_END2,
	TYPE_CAPACITOR,
	CAB1,
	RESOURCE_5,
	END2
};
static const ConnectionSite cap16_end2 = {
	CAP16_END2,
	TYPE_CAPACITOR,
	CAB1,
	RESOURCE_6,
	END2
};
static const ConnectionSite cap17_end2 = {
	CAP17_END2,
	TYPE_CAPACITOR,
	CAB1,
	RESOURCE_7,
	END2
};
static const ConnectionSite cap18_end2 = {
	CAP18_END2,
	TYPE_CAPACITOR,
	CAB1,
	RESOURCE_8,
	END2
};
static const ConnectionSite cap21_end2 = {
	CAP21_END2,
	TYPE_CAPACITOR,
	CAB2,
	RESOURCE_1,
	END2
};
static const ConnectionSite cap22_end2 = {
	CAP22_END2,
	TYPE_CAPACITOR,
	CAB2,
	RESOURCE_2,
	END2
};
static const ConnectionSite cap23_end2 = {
	CAP23_END2,
	TYPE_CAPACITOR,
	CAB2,
	RESOURCE_3,
	END2
};
static const ConnectionSite cap24_end2 = {
	CAP24_END2,
	TYPE_CAPACITOR,
	CAB2,
	RESOURCE_4,
	END2
};
static const ConnectionSite cap25_end2 = {
	CAP25_END2,
	TYPE_CAPACITOR,
	CAB2,
	RESOURCE_5,
	END2
};
static const ConnectionSite cap26_end2 = {
	CAP26_END2,
	TYPE_CAPACITOR,
	CAB2,
	RESOURCE_6,
	END2
};
static const ConnectionSite cap27_end2 = {
	CAP27_END2,
	TYPE_CAPACITOR,
	CAB2,
	RESOURCE_7,
	END2
};
static const ConnectionSite cap28_end2 = {
	CAP28_END2,
	TYPE_CAPACITOR,
	CAB2,
	RESOURCE_8,
	END2
};
static const ConnectionSite cap31_end2 = {
	CAP31_END2,
	TYPE_CAPACITOR,
	CAB3,
	RESOURCE_1,
	END2
};
static const ConnectionSite cap32_end2 = {
	CAP12_END2,
	TYPE_CAPACITOR,
	CAB3,
	RESOURCE_2,
	END2
};
static const ConnectionSite cap33_end2 = {
	CAP33_END2,
	TYPE_CAPACITOR,
	CAB3,
	RESOURCE_3,
	END2
};
static const ConnectionSite cap34_end2 = {
	CAP34_END2,
	TYPE_CAPACITOR,
	CAB3,
	RESOURCE_4,
	END2
};
static const ConnectionSite cap35_end2 = {
	CAP35_END2,
	TYPE_CAPACITOR,
	CAB3,
	RESOURCE_5,
	END2
};
static const ConnectionSite cap36_end2 = {
	CAP36_END2,
	TYPE_CAPACITOR,
	CAB3,
	RESOURCE_6,
	END2
};
static const ConnectionSite cap37_end2 = {
	CAP37_END2,
	TYPE_CAPACITOR,
	CAB3,
	RESOURCE_7,
	END2
};
static const ConnectionSite cap38_end2 = {
	CAP38_END2,
	TYPE_CAPACITOR,
	CAB3,
	RESOURCE_8,
	END2
};
static const ConnectionSite cap41_end2 = {
	CAP41_END2,
	TYPE_CAPACITOR,
	CAB4,
	RESOURCE_1,
	END2
};
static const ConnectionSite cap42_end2 = {
	CAP42_END2,
	TYPE_CAPACITOR,
	CAB4,
	RESOURCE_2,
	END2
};
static const ConnectionSite cap43_end2 = {
	CAP43_END2,
	TYPE_CAPACITOR,
	CAB4,
	RESOURCE_3,
	END2
};
static const ConnectionSite cap44_end2 = {
	CAP44_END2,
	TYPE_CAPACITOR,
	CAB4,
	RESOURCE_4,
	END2
};
static const ConnectionSite cap45_end2 = {
	CAP45_END2,
	TYPE_CAPACITOR,
	CAB4,
	RESOURCE_5,
	END2
};
static const ConnectionSite cap46_end2 = {
	CAP46_END2,
	TYPE_CAPACITOR,
	CAB4,
	RESOURCE_6,
	END2
};
static const ConnectionSite cap47_end2 = {
	CAP47_END2,
	TYPE_CAPACITOR,
	CAB4,
	RESOURCE_7,
	END2
};
static const ConnectionSite cap48_end2 = {
	CAP48_END2,
	TYPE_CAPACITOR,
	CAB4,
	RESOURCE_8,
	END2
};
static const ConnectionSite voltage_reference_positive = {
	VOLTAGE_REFERENCE_POSITIVE,
	TYPE_VOLTAGE_REFERENCE,
	CABVREF,
	RESOURCE_1,
	TYPE_OUT
};
static const ConnectionSite voltage_reference_negative = {
	VOLTAGE_REFERENCE_NEGATIVE,
	TYPE_VOLTAGE_REFERENCE,
	CABVREF,
	RESOURCE_2,
	TYPE_OUT
};

#endif