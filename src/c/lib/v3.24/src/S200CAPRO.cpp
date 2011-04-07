//#include "stdafx.h"

#include "S200CAPRO.h"

S200CAPRO::S200CAPRO()
{
	initAttributes();
}


S200CAPRO::S200CAPRO( unsigned char *buffer ) : S200Base( buffer )
{
	initAttributes();
	setFixedParameters( buffer );
	createIndexedParameters();
}

S200CAPRO::~S200CAPRO()
{
}

void S200CAPRO::initAttributes()
{


	memset(	parameterMask, 0, MAX_RECEIVER_PARAM * sizeof( unsigned short ) );


	// FIXED READ ONLY VALUES
	//---------------------------------------
	
	// DEVICE INFO
	
	parameterMask[ DEVICE_TYPE						] = F_EXISTS | F_ID | F_DEF | F_READ | F_SHORT | F_ENUM;
	parameterMask[ DSP_VERSION						] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ DSP_REVISION						] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ CAM_VERSION						] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ CAM_REVISION						] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ FPGA_VERSION						] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ FPGA_REVISION					] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ RECEIVER_MAC						] = F_EXISTS | F_ID | F_READ | F_STRING;
	parameterMask[ SIZE_PID_LIST					] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ SIZE_IP_REMAP_LIST				] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ SIZE_CA_PROGRAM_LIST				] = F_EXISTS | F_DEF | F_READ | F_SHORT;
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
	parameterMask[ MODCOD							] = F_EXISTS | F_STATUS | F_READ | F_STRING | F_ENUM;
	parameterMask[ SIGNAL_STRENGTH_AS_DBM			] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ SIGNAL_STRENGTH_AS_PERCENTAGE	] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ SIGNAL_LOCK						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ DATA_LOCK						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ LNB_FAULT						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ VBER								] = F_EXISTS | F_STATUS | F_READ | F_FLOAT;
	parameterMask[ PER								] = F_EXISTS | F_STATUS | F_READ | F_FLOAT;
	parameterMask[ UNCORRECTABLES					] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ CARRIER_TO_NOISE					] = F_EXISTS | F_STATUS | F_READ | F_FLOAT;
	parameterMask[ CARRIER_FREQUENCY				] = F_EXISTS | F_STATUS | F_READ | F_STRING;
	parameterMask[ FREQUENCY_OFFSET					] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ SPECTRAL_INVERSION_FLAG			] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ PILOT_SYMBOL_FLAG				] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ FRAME_LENGTH						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ DVB_SIGNAL_TYPE					] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;


	// CONTENT

	// DVB
	
	parameterMask[ PID_COUNT						] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ CA_PROGRAM_NUMBER				] = F_EXISTS | F_READ | F_SHORT | F_INDEXED;
	parameterMask[ CA_PROGRAM_STATUS				] = F_EXISTS | F_READ | F_SHORT | F_ENUM | F_INDEXED;
	parameterMask[ CAM_STATUS					] = F_EXISTS | F_STATUS | F_READ | F_SHORT | F_ENUM;
	parameterMask[ DVB_ACCEPTED						] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ TOTAL_DVB_PACKETS_ACCEPTED		] = F_EXISTS | F_STATUS | F_READ | F_I64;
	parameterMask[ TOTAL_UNCORRECTABLE_TS_PACKETS	] = F_EXISTS | F_STATUS | F_READ | F_I64;

	// ETHERNET
		
	parameterMask[ ETHERNET_TRANSMIT				] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ ETHERNET_RECEIVE					] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ ETHERNET_PACKET_DROPPED			] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ ETHERNET_RECEIVE_ERROR			] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ TOTAL_ETHERNET_PACKETS_OUT		] = F_EXISTS | F_STATUS | F_READ | F_I64;
	parameterMask[ DVB_SCRAMBLED					] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ DVB_CLEAR						] = F_EXISTS | F_STATUS | F_READ | F_LONG;


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
	parameterMask[ DVB_SIGNAL_TYPE_CONTROL			] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_ENUM;


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
	parameterMask[ CA_PROGRAM_NUMBER				] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_INDEXED;
	parameterMask[ FORWARD_ALL_FLAG					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ FORWARD_NULLS_FLAG				] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ IP_REMAP_ENABLE					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ ORIGINAL_IP						] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_STRING | F_INDEXED;
	parameterMask[ NEW_IP							] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_STRING | F_INDEXED;
	parameterMask[ IP_REMAP_MASK					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_STRING | F_INDEXED;
	parameterMask[ IP_REMAP_TTL						] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_INDEXED;
	parameterMask[ IP_REMAP_ACTION					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_STRING | F_ENUM | F_INDEXED;


	// MISC

	parameterMask[ CAM_WATCHDOG_INTERVAL			] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT;


	// WRITE ONLY SECTION

	parameterMask[ PASSWORD							] = F_EXISTS | F_WRITE | F_STRING;


	Receiver::initAttributes();
}
