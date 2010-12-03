//#include "stdafx.h"

#include "SJ300.h"


SJ300::SJ300()
{
	initAttributes();
}


SJ300::SJ300( unsigned char *buffer ) : SJ300Base( buffer )
{
	initAttributes();
	setFixedParameters( buffer );
	createIndexedParameters();
}


void SJ300::initAttributes()
{
	memset(	parameterMask, 0, MAX_RECEIVER_PARAM * sizeof( unsigned char ) );


	// FIXED READ ONLY VALUES
	//---------------------------------------
	
	// DEVICE INFO
	
	parameterMask[ DEVICE_TYPE						] = F_EXISTS | F_ID | F_DEF | F_READ | F_SHORT | F_ENUM;	// checked
	parameterMask[ DSP_VERSION						] = F_EXISTS | F_DEF | F_READ | F_SHORT;	// checked
	parameterMask[ DSP_REVISION						] = F_EXISTS | F_DEF | F_READ | F_SHORT;	// checked
	parameterMask[ FPGA_VERSION						] = F_EXISTS | F_DEF | F_READ | F_SHORT;	
	parameterMask[ FPGA_REVISION					] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ RECEIVER_MAC						] = F_EXISTS | F_ID | F_READ | F_STRING;	// checked
	parameterMask[ SIZE_PID_LIST					] = F_EXISTS | F_DEF | F_READ | F_SHORT;	// checked
	parameterMask[ SIZE_IP_REMAP_LIST				] = F_EXISTS | F_DEF | F_READ | F_SHORT;	// checked
	parameterMask[ CODE_DOWNLOAD					] = F_EXISTS | F_DEF | F_READ | F_FLAG;
	parameterMask[ AUTO_DVBS_TYPE					] = F_EXISTS | F_DEF | F_READ | F_FLAG;

	// DYNAMIC READ ONLY VALUES
	//----------------------------------------

	parameterMask[ STATUS_TIMESTAMP					] = F_EXISTS | F_STATUS | F_READ | F_STRING;

	// SIGNAL

	parameterMask[ DATA_SYNC_LOSS					] = F_EXISTS | F_STATUS | F_READ | F_SHORT;

	// RF

	parameterMask[ DVB_SIGNAL_TYPE					] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;		// checked
	parameterMask[ VITERBI_RATE						] = F_EXISTS | F_STATUS | F_READ | F_STRING | F_ENUM;
	parameterMask[ MODCOD							] = F_EXISTS | F_STATUS | F_READ | F_STRING | F_ENUM;	// checked
	parameterMask[ SIGNAL_STRENGTH_AS_DBM			] = F_EXISTS | F_STATUS | F_READ | F_SHORT;	// checked
	parameterMask[ SIGNAL_LOCK						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;		// checked
	parameterMask[ DATA_LOCK						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;		// checked
	parameterMask[ LNB_FAULT						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;		// checked
	parameterMask[ VBER								] = F_EXISTS | F_STATUS | F_READ | F_FLOAT;
	parameterMask[ PER								] = F_EXISTS | F_STATUS | F_READ | F_FLOAT;
	parameterMask[ UNCORRECTABLES					] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ CARRIER_TO_NOISE					] = F_EXISTS | F_STATUS | F_READ | F_FLOAT;	// checked
	parameterMask[ CARRIER_FREQUENCY				] = F_EXISTS | F_STATUS | F_READ | F_STRING;	// checked
	parameterMask[ FREQUENCY_OFFSET					] = F_EXISTS | F_STATUS | F_READ | F_SHORT;	// checked
	parameterMask[ SPECTRAL_INVERSION_FLAG			] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ PILOT_SYMBOL_FLAG				] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ FRAME_LENGTH						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ LOCKED_SYMBOL_RATE				] = F_EXISTS | F_STATUS | F_READ | F_FLOAT;

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

	parameterMask[ SYMBOL_RATE						] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLOAT;	// checked
	parameterMask[ AUTO_SYMBOL_RATE					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE	| F_FLAG;
	parameterMask[ GOLD_CODE						] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_LONG;
	parameterMask[ LBAND_FREQUENCY					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLOAT;
	parameterMask[ LO_FREQUENCY						] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLOAT;
	parameterMask[ LNB_POWER						] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ BAND								] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ POLARITY							] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ HI_VOLTAGE_SWITCH				] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ LONG_LINE_COMPENSATION_SWITCH	] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ POLARITY_SWITCHING_VOLTAGE		] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ BAND_SWITCHING_TONE_FREQUENCY	] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ DVB_SIGNAL_TYPE_CONTROL			] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_ENUM;


	// CONTENT

	parameterMask[ PID								] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_INDEXED;
	parameterMask[ STREAM_DESTINATION_IP			] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT;
	parameterMask[ STREAM_DESTINATION_PORT			] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT;
	parameterMask[ FORWARD_ALL_FLAG					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ FORWARD_NULLS_FLAG				] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ IP_REMAP_ENABLE					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ ORIGINAL_IP						] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_STRING | F_INDEXED;
	parameterMask[ NEW_IP							] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_STRING | F_INDEXED;
	parameterMask[ IP_REMAP_MASK					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_STRING | F_INDEXED;
	parameterMask[ IP_REMAP_TTL						] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_INDEXED;
	parameterMask[ IP_REMAP_ACTION					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_STRING | F_ENUM | F_INDEXED;


	// WRITE ONLY SECTION

	parameterMask[ PASSWORD							] = F_EXISTS | F_WRITE | F_STRING;


	Receiver::initAttributes();
}
