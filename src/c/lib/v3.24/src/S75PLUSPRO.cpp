//#include "stdafx.h"

#include "S75PLUSPRO.h"


S75PLUSPRO::S75PLUSPRO()
{
	initAttributes();
}


S75PLUSPRO::S75PLUSPRO( unsigned char *buffer )
{
	initAttributes();
	setFixedParameters( buffer );
	createIndexedParameters();
}

S75PLUSPRO::~S75PLUSPRO()
{
}

void S75PLUSPRO::initAttributes()
{
	memset(	parameterMask, 0, MAX_RECEIVER_PARAM * sizeof( unsigned short ) );


	// FIXED READ ONLY VALUES
	//---------------------------------------
	
	// DEVICE INFO
	
	parameterMask[ DEVICE_TYPE						] = F_EXISTS | F_ID | F_DEF | F_READ | F_SHORT | F_ENUM;
	parameterMask[ DSP_VERSION						] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ DSP_REVISION						] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ RECEIVER_MAC						] = F_EXISTS | F_ID | F_READ | F_STRING;
	parameterMask[ SIZE_PID_LIST					] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ SIZE_DESTINATION_LIST			] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ SIZE_MINI_PAT_LIST				] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ MPE								] = F_EXISTS | F_DEF | F_READ | F_FLAG;
	parameterMask[ CODE_DOWNLOAD					] = F_EXISTS | F_DEF | F_READ | F_FLAG;

	// DYNAMIC READ ONLY VALUES
	//----------------------------------------

	parameterMask[ STATUS_TIMESTAMP					] = F_EXISTS | F_STATUS | F_READ | F_STRING;

	// SIGNAL

	parameterMask[ DATA_SYNC_LOSS					] = F_EXISTS | F_STATUS | F_READ | F_SHORT;

	// RF

	parameterMask[ VITERBI_RATE						] = F_EXISTS | F_STATUS | F_READ | F_STRING | F_ENUM;
	parameterMask[ SIGNAL_STRENGTH_AS_PERCENTAGE	] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ SIGNAL_LOCK						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ DATA_LOCK						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ LNB_FAULT						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ VBER								] = F_EXISTS | F_STATUS | F_READ | F_FLOAT;
	parameterMask[ UNCORRECTABLES					] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
//	parameterMask[ CARRIER_FREQUENCY				] = F_EXISTS | F_STATUS | F_READ | F_STRING;
	parameterMask[ CARRIER_TO_NOISE					] = F_EXISTS | F_STATUS | F_READ | F_FLOAT;
	parameterMask[ DEMODULATOR_GAIN					] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ AGCA								] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ AGCN								] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ GNYQA							] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ GFARA							] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ NEST								] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ CLOCK_OFFSET						] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ FREQUENCY_ERROR					] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ ADC_MID							] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ ADC_CLIP							] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ DIGITAL_GAIN						] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ AA_CLAMP							] = F_EXISTS | F_STATUS | F_READ | F_SHORT;

	// CONTENT

	// DVB
	
	parameterMask[ PID_COUNT						] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ DVB_ACCEPTED						] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ TOTAL_DVB_PACKETS_ACCEPTED		] = F_EXISTS | F_STATUS | F_READ | F_I64;
	parameterMask[ TOTAL_UNCORRECTABLE_TS_PACKETS	] = F_EXISTS | F_STATUS | F_READ | F_I64;
	parameterMask[ DVB_SCRAMBLED					] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ DVB_CLEAR						] = F_EXISTS | F_STATUS | F_READ | F_LONG;

	// ETHERNET
		
	parameterMask[ ETHERNET_TRANSMIT				] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ ETHERNET_RECEIVE					] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ ETHERNET_PACKET_DROPPED			] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ ETHERNET_RECEIVE_ERROR			] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ TOTAL_ETHERNET_PACKETS_OUT		] = F_EXISTS | F_STATUS | F_READ | F_I64;


	// READ/WRITE SECTION
	//-----------------------------------

	// Network Parameters

	parameterMask[ RECEIVER_IP						] = F_EXISTS | F_ID | F_CONFIG | F_READ | F_WRITE | F_STRING;
	parameterMask[ SUBNET_MASK						] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_STRING;
	parameterMask[ DEFAULT_GATEWAY_IP				] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_STRING;
	parameterMask[ UNICAST_STATUS_IP				] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_STRING;
	parameterMask[ UNICAST_STATUS_PORT				] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT;
	parameterMask[ BROADCAST_STATUS_PORT			] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT;
	parameterMask[ IGMP_ENABLE						] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;

	// RF

	parameterMask[ LBAND_FREQUENCY					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLOAT;
	parameterMask[ LO_FREQUENCY						] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLOAT;
	parameterMask[ SYMBOL_RATE						] = F_EXISTS | F_CONFIG | F_STATUS | F_READ | F_WRITE | F_FLOAT;
	parameterMask[ LNB_POWER						] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ BAND								] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ POLARITY							] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ HI_VOLTAGE_SWITCH				] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ LONG_LINE_COMPENSATION_SWITCH	] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ POLARITY_SWITCHING_VOLTAGE		] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ BAND_SWITCHING_TONE_FREQUENCY	] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;


	// CONTENT

	parameterMask[ PID								] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_INDEXED;
	parameterMask[ RAW_FORWARD_FLAG					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_ENUM | F_INDEXED;
	parameterMask[ DESTINATION_MASK					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_INDEXED;
	parameterMask[ PID_DESTINATION_IP				] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_INDEXED;
	parameterMask[ PID_DESTINATION_PORT				] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_INDEXED;
	parameterMask[ STREAM_DESTINATION_IP			] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT;
	parameterMask[ STREAM_DESTINATION_PORT			] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT;
	parameterMask[ PROGRAM_NUMBER					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_INDEXED;
	parameterMask[ PMT_PID							] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_INDEXED;
	parameterMask[ FORWARD_ALL_FLAG					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ FORWARD_NULLS_FLAG				] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;


	// WRITE ONLY SECTION

	parameterMask[ PASSWORD							] = F_EXISTS | F_WRITE | F_STRING;


	Receiver::initAttributes();
}

