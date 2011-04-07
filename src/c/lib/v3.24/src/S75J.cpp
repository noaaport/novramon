//#include "stdafx.h"

#include "S75J.h"


#ifdef WINDOWS
    #include "Winsock2.h"
#endif

#ifndef WINDOWS
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>

    typedef int SOCKET;
#endif

#include "math.h"
#include "Password.h"
#include "Keycrypt.h"
#include <time.h>
#include "ConfigPorts.h"

#define NUMBER_OF_PIDS				16

const int PAT_CONFIG_PORT		= 0x1952;
const int PROGRAM_CONFIG_PORT	= 0x1958;
const int NETWORK_CONFIG_PORT	= 0x1975;
const int PID_CONFIG_PORT		= 0x1977;
const int RF_CONFIG_PORT		= 0x1983;
const int PRODUCTID_CONFIG_PORT	= 0x1984;
const int TRAP_CONFIG_PORT		= 0x2005;
const int SET_PASSWORD_PORT		= 0x2010;
const int STATUS_POLL_PORT		= 0x2011;
const int FK_CONFIG_PORT		= 0x2022;
const int CIPHER_SABER_PORT		= 0x9899;
const int PROGRAM_INFO_PORT		= 0x9992;
const int VIDEO_PROGRAM_PORT	= 0x9993;
const int IP_REMAP_PORT			= 0x9994;
const int DEFAULT_CONFIG_PORT	= 0x9995;
const int PSI_TABLE_PORT		= 0x9996;
const int CAM_WATCHDOG_PORT		= 0x9997;
const int RESET_PORT			= 0x9999;


static ReceiverParameter networkParams[] = {	RECEIVER_IP,
												SUBNET_MASK,
												DEFAULT_GATEWAY_IP,
												UNICAST_STATUS_IP,
												UNICAST_STATUS_PORT,
												BROADCAST_STATUS_PORT,
												IGMP_ENABLE				};


static ReceiverParameter rfParams[] = {	LBAND_FREQUENCY,
										SYMBOL_RATE,
										LNB_POWER,
										BAND,
										POLARITY,
										HI_VOLTAGE_SWITCH,
										LONG_LINE_COMPENSATION_SWITCH,
										LO_FREQUENCY  	 				};

static ReceiverParameter pidParams[] = {	PID	};


typedef unsigned char BYTE;
typedef unsigned short WORD;


typedef struct S75JStatusRequestMSG {
	BYTE Message[6];	 
} S75JStatusRequestMSG;


typedef struct S75JPIDRequestMSG {

    WORD					Opcode;               // 1 = download (S75 to host), 2 = upload (host to S75)
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1

} S75JPIDRequestMSG;


typedef struct S75JPIDMSG {

    WORD					Opcode;               // 1 = download (S75 to host), 2 = upload (host to S75)
    WORD					PIDTableEntry[NUMBER_OF_PIDS];
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1

} S75JPIDMSG;


typedef struct S75JSetPasswordMSG
{
	unsigned char newPassword[16];
	BYTE Signature[20];				

} S75JSetPasswordMSG;


typedef struct S75JStatusMSG {
	BYTE			ProductID[32];					// Product Identification
	BYTE			DeviceType;
	BYTE			DSPFirmWare;					// Firmware Version Revision.
	BYTE			ReceiverMAC[6] ;				// Receiver MAC Address.
	BYTE			ReceiverIP[4];
    BYTE			SubnetMask[4];					// Receiver Subnet Mask
    BYTE			DefaultGatewayIP[4];			// Default Gateway Address
	BYTE			UnicastStatusIP[4];				// Destination IP Address for Unicast Status
	WORD			UnicastStatusPort;				// Destination UDP Port.
	WORD			RFStatusValid;
	WORD			MicroFirmWare;					// Micro controller firmware version
	unsigned short	SymbolRate;						// Symbol rate in KSPS
	unsigned short	Frequency;						// Frequency in 100 KHz
	BYTE			LNBConfiguration;				// LNB control settings
	BYTE			ViterbiRate;
	BYTE			SignalStrength;					// AGC
	BYTE			DemodulatorStatus;				// Demodulator Status
	BYTE			VBER_Man[3];					// Viterbi BER Mantisa
	BYTE			VBER_Exp;						// Viterbi BER Exponent
	BYTE			Uncorrectables;					// Uncorrectable errors count since alst status
	BYTE			DemodulatorGain;				// the Digital gain
	BYTE			Reserved[8];
	WORD			RFEND;							// RF message termination .
	unsigned short	EthernetTX;						// Ethernet Transmitted Packets Counter
	unsigned short	EthernetDropped;				// Number of Ethernet Packets Dropped by the Ethernet chip
	unsigned short	EthernetRX;						// Number of Ethernet Packets Received
	unsigned short	EthernetRXErr;					// Count of Ethernet Packets Received In Error.
	unsigned short	EthernetTXErr;
	unsigned short	DVBAccepted;					// Total Number of DVB Packets that passed the Filter.
	unsigned short	FELLoss;
	unsigned short	Sync_Loss;						// Packets Received with FEL high and No Sync Byte.	
	BYTE			Unused;
	BYTE			Filter;							// IGMP On/Off flag[2]

	WORD			PIDList[NUMBER_OF_PIDS];		// List of PIDS to pass through the filter. 
	BYTE			DVBMAC[6];						// the DVB MAC address of the receiver..

	WORD			junk;

	unsigned short	TotalDVBPacketsAccepted[4];		// Statistics counters
	unsigned short	TotalDVBPacketsRXInError[4];
	unsigned short	TotalEthernetPacketsOut[4];
	unsigned short	TotalUncorrectableTSPackets[4];

	unsigned short	BroadcastStatusPort;			// Broadcast status UDP port
	unsigned short	LocalOscFrequency;				// LNB Local Oscillator Frequency
	
} S75JStatusMSG;


typedef struct S75JNetConfigMSG {

	BYTE  ReceiverIP[4];		// the IP address to be assumed by the device
    BYTE  SubnetMask[4];		// the Subnet Mask to be used by the device
    BYTE  DefaultGatewayIP[4];	// the default gateway to be used by the device
	WORD  UnicastStatusPort;  
	BYTE  UnicastStatusIP[4];
	WORD  BroadcastStatusPort;			// the status port 
	WORD  IGMP;                 // 0 = filter off, 1= IGMP Filter on
	WORD  Reserved[2]; 
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1

} S75JNetConfigMSG;


typedef struct S75JRFConfigMSG {

	WORD  SymbolRate;              // in ksps
	WORD  Frequency;               // in units of 10 Hz
	BYTE  LNBConfig;               // bit0 - 0 = power off, 1 = power on
                                   // bit1 - 0 = vertical or right polarization
	                               //        1 = horizontal or left polarization
	                               // bit2 - 0 = low band, 1 = high band
								   // bit3 - Hi voltage switch
								   // bit4 - Long line compensation switch
	BYTE  ViterbiRate;             // 0x08 = auto
	WORD  LocalOscFrequency;
	WORD  Random;				   // 
	BYTE  Signature[20];		   // HMAC-SHA1

} S75JRFConfigMSG;


typedef struct S75JResetMSG {

	BYTE Message[4];	 
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1

} S75JResetMSG;


extern pstruct m_sec;


S75J::S75J()
{
	initAttributes();

	config_ports = new unsigned short[I_NUM_CONFIG_PORTS];

	SETUP_PORT_TABLE;

	currentPIDs = NULL;
	pendingPIDs = NULL;
}


S75J::S75J( unsigned char *buffer )
{
	initAttributes();

	config_ports = new unsigned short[I_NUM_CONFIG_PORTS];

	SETUP_PORT_TABLE;

	currentPIDs = NULL;
	pendingPIDs = NULL;

	setFixedParameters( buffer );
	createIndexedParameters();
}


S75J::~S75J()

{
	if ( config_ports != NULL ) {
		delete [] config_ports;
	}
	if ( currentPIDs != NULL ) {
		delete [] currentPIDs;
	}
	if ( pendingPIDs != NULL ) {
		delete [] pendingPIDs;
	}
}


void S75J::initAttributes()
{

	memset(	parameterMask, 0, MAX_RECEIVER_PARAM * sizeof( unsigned short ) );

	// FIXED READ ONLY VALUES
	//---------------------------------------
	
	// DEVICE INFO
	
	parameterMask[ DEVICE_TYPE			] = F_EXISTS | F_ID | F_DEF | F_READ | F_SHORT | F_ENUM;
	parameterMask[ DSP_VERSION			] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ DSP_REVISION			] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ RF_FIRMWARE_VERSION	] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ RECEIVER_MAC			] = F_EXISTS | F_ID | F_READ | F_STRING;
	parameterMask[ MPE					] = F_EXISTS | F_DEF | F_READ | F_FLAG;
	parameterMask[ SIZE_PID_LIST		] = F_EXISTS | F_DEF | F_READ | F_SHORT;

	// DYNAMIC READ ONLY VALUES
	//----------------------------------------

	parameterMask[ STATUS_TIMESTAMP					] = F_EXISTS | F_STATUS | F_READ | F_STRING;

	// SIGNAL

	parameterMask[ FRONT_END_LOCK_LOSS				] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ DATA_SYNC_LOSS					] = F_EXISTS | F_STATUS | F_READ | F_SHORT;

	// RF

	parameterMask[ VITERBI_RATE						] = F_EXISTS | F_STATUS | F_READ | F_STRING | F_ENUM;
	parameterMask[ SIGNAL_STRENGTH_AS_PERCENTAGE	] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ SIGNAL_LOCK						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ DATA_LOCK						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ LNB_FAULT						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ VBER								] = F_EXISTS | F_STATUS | F_READ | F_FLOAT;
	parameterMask[ UNCORRECTABLES					] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ CARRIER_TO_NOISE					] = F_EXISTS | F_STATUS | F_READ | F_FLOAT;
	parameterMask[ DEMODULATOR_GAIN					] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ AGCA								] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ AGCN								] = F_EXISTS | F_STATUS | F_READ | F_SHORT;

	// CONTENT

	// DVB
	
	parameterMask[ PID_COUNT						] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ DVB_ACCEPTED						] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ TOTAL_DVB_PACKETS_ACCEPTED		] = F_EXISTS | F_STATUS | F_READ | F_I64;
	parameterMask[ TOTAL_DVB_PACKETS_RX_IN_ERROR	] = F_EXISTS | F_STATUS | F_READ | F_I64;
	parameterMask[ TOTAL_UNCORRECTABLE_TS_PACKETS	] = F_EXISTS | F_STATUS | F_READ | F_I64;

	// ETHERNET
		
	parameterMask[ ETHERNET_TRANSMIT				] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ ETHERNET_TRANSMIT_ERROR			] = F_EXISTS | F_STATUS | F_READ | F_LONG;
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


	// CONTENT

	parameterMask[ PID								] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_INDEXED;



	// WRITE ONLY SECTION

	parameterMask[ PASSWORD							] = F_EXISTS | F_WRITE | F_STRING;



	Receiver::initAttributes();

}



bool S75J::connected( int timeout )
{
	unsigned char junk[1500];

	return pollStatus( junk, timeout );
}


bool S75J::setFixedParameters( unsigned char *buffer )
{
	char temp[20];

	if ( buffer[32] == typeIDCode() ) {

		currentParameters[DEVICE_TYPE]->setM_value( string( itoa( receiverTypeCode(), temp, 10 ) ) );

		currentParameters[RECEIVER_MAC]->setM_value( fixedParamFromStatus( RECEIVER_MAC, buffer ) );

		currentParameters[DSP_VERSION]->setM_value( fixedParamFromStatus( DSP_VERSION, buffer ) );

		currentParameters[DSP_REVISION]->setM_value( fixedParamFromStatus( DSP_REVISION, buffer ) );

		if ( hasParameter( RF_FIRMWARE_VERSION ) ) {

			currentParameters[RF_FIRMWARE_VERSION]->setM_value( fixedParamFromStatus( RF_FIRMWARE_VERSION, buffer ) );

		}

		currentParameters[SIZE_PID_LIST]->setM_value( string( itoa( NUMBER_OF_PIDS ,temp, 10 ) ) );

		return true;

	} else {

		return false;

	}
}



string S75J::fixedParamFromStatus( ReceiverParameter p, unsigned char *buffer )
{
	char temp[100];
	string return_str = "";
	S75JStatusMSG *status = (S75JStatusMSG *)buffer;


	switch( p ) {

	case RECEIVER_MAC			:	sprintf(	temp, "%02x-%02x-%02x-%02x-%02x-%02x", 
												status->ReceiverMAC[0], status->ReceiverMAC[1],
												status->ReceiverMAC[2],	status->ReceiverMAC[3],
												status->ReceiverMAC[4], status->ReceiverMAC[5]  );
									return_str = string( temp );
									break;

	case DSP_VERSION			:	return_str = string ( itoa( ( status->DSPFirmWare & 0xf0 ) >> 4, temp, 10 ) );
									break;

	case DSP_REVISION			:	return_str = string ( itoa( status->DSPFirmWare & 0xf, temp, 10 ) );
									break;

	case RF_FIRMWARE_VERSION	:	return_str = string ( itoa( ntohs( status->MicroFirmWare ), temp, 10 ) );
									break;

	default						:	return_str = "";
									break;


	}

	return return_str;
}



string S75J::dynamicParamFromStatus( ReceiverParameter p, unsigned char *buffer, int index )
{
	S75JStatusMSG *status = (S75JStatusMSG *)buffer;
	char temp[100];
	string return_str = "";
	bool signal_lock;
	bool data_lock;
	double ss;
	float vber;
	int	BERmantissa;   // Bit Error Rate mantissa
	int	BERexponent;   // Bit Error Rate exponent
	const double AGC[] = { 250, 220, 140,120, 105,80, 65 }; // Automatic Gain Control
	const double PERCENTAGE[] = { 4, 35,40,60,75, 95,100 };
	const int SIZE_ARRAY = 7;
	const int OffsetByGain[] = {0, 8, 16 , 25};

	double slope = 0.0;             // Values for calculating signal strength
	double offset = 0.0;            // Values for calculating signal strength

#ifdef WINDOWS
	unsigned __int64 big_number = 0;
#else
	unsigned long long big_number = 0;
#endif

											
	switch( p ) {

	// DSP Version and revision can change if code download is used

	case DSP_VERSION					:	return_str = string ( itoa( ( status->DSPFirmWare & 0xf0 ) >> 4, temp, 10 ) );
											break;

	case DSP_REVISION					:	return_str = string ( itoa( status->DSPFirmWare & 0xf, temp, 10 ) );
											break;


	case DATA_SYNC_LOSS					:	return_str = string ( itoa( ntohs( status->Sync_Loss ), temp, 10 ) );									
											break;

	case FRONT_END_LOCK_LOSS			:	return_str = string ( itoa( ntohs( status->FELLoss ), temp, 10 ) );									
											break;

	case VITERBI_RATE					:	switch ( status->ViterbiRate ) {
												case 0x00	:		return_str = string( itoa( HALF, temp, 10 ) );
																	break;
												case 0x01	:		return_str = string( itoa( TWO_THIRDS, temp, 10 ) );
																	break;
												case 0x02	:		return_str = string( itoa( THREE_QUARTERS, temp, 10 ) );
																	break;
												case 0x04	:		return_str = string( itoa( FIVE_SIXTHS, temp, 10 ) );
																	break;
												case 0x06	:		return_str = string( itoa( SEVEN_EIGTHS, temp, 10 ) );
																	break;
												default		:		return_str = string( itoa( VITERBI_ERROR, temp, 10 ) );
																	break;
											};
											break;


	case SIGNAL_STRENGTH_AS_PERCENTAGE	:	signal_lock = status->DemodulatorStatus & 0x01;
											data_lock = (bool)(( status->DemodulatorStatus & 0x02 ) >> 1);
											if (status->SignalStrength >= 250) {
												ss=3;
												if(! data_lock) ss= 0;

												switch( status->DemodulatorGain) {
													case 1:
														ss += 2.2 * OffsetByGain[3];
														break;
													case 2:
														ss += 2 *OffsetByGain[2];
														break ;
													case 4:
														ss += 10+2* OffsetByGain[1];
														break ;
													case 8:
														ss += OffsetByGain[0];
														break;
												}
											} else if (status->SignalStrength < AGC[SIZE_ARRAY - 1]) {
												ss = 100;

											} else {
												for (int i = 0; i < SIZE_ARRAY - 1; i++ ) {
													if( ( AGC[i] >= status->SignalStrength) && 
														( AGC[i + 1] <= status->SignalStrength) ) {	// @@@DBW
														slope = ( PERCENTAGE[i + 1] - PERCENTAGE[i] ) / ( AGC[i + 1] - AGC[i] );
														offset = PERCENTAGE[i] - slope * AGC[i];
														ss = slope * (double)status->SignalStrength + offset;
														switch( status->DemodulatorGain){
															case 1:
																	{
																	ss=1.2 *ss;
																	ss += OffsetByGain[3];
																	if (ss < (3 + 2.2 * OffsetByGain[3]) )
																		ss=(3 + 2.2 * OffsetByGain[3]) ;
																	break;
																	}
															case 2:
																	{
																	ss =1.1 *ss;
																	ss += OffsetByGain[2];
																	if (ss < (3+2 *OffsetByGain[2]) )
																		ss = (3+2 *OffsetByGain[2]) ;
																	break ;
																	}
															case 4:
																	{
																	ss = 0.8 *ss ;
																	ss += OffsetByGain[1];
																	if(ss<(13+2* OffsetByGain[1]))
																		ss = 13+2* OffsetByGain[1] ;
																	break ;
																	}
															case 8:
																	{
																	ss = 0.7*ss;
																	break;
																	}
														}
			
														break;
													}
												}
											}

											if ( (int)ss > 100 ) ss = 100; else ss = (int)ss;
											if ( ( ss < 10 ) && ( status->DemodulatorStatus & 0x02 ) ) ss = 10;
											sprintf( temp, "%f", ss );
											return_str = string( temp );
											break;

	case SIGNAL_LOCK					:	return_str = string ( itoa( status->DemodulatorStatus & 0x01, temp, 10 ) );
											break;

	case DATA_LOCK						:	return_str = string ( itoa( ( status->DemodulatorStatus & 0x02 ) >> 1, temp, 10 ) );
											break;

	case LNB_FAULT						:	return_str = string ( itoa( ( status->DemodulatorStatus & 0x04 ) >> 2, temp, 10 ) );
											break;

	case VBER							:	// Convert the BER to a floating point

										    BERmantissa =	status->VBER_Man[2] |
															status->VBER_Man[1] * 0x100 |
															status->VBER_Man[0] * 0x10000;
            
										    vber = (float)((double)BERmantissa / pow(10.0, (double)(status->VBER_Exp)));	

											sprintf( temp, "%.2e", vber );
											return_str = string( temp );
											break;

	case UNCORRECTABLES					:	return_str = string ( itoa( status->Uncorrectables, temp, 10 ) );
											break;


	case CARRIER_TO_NOISE				:	BERmantissa = status->VBER_Man[2] |
														  ( status->VBER_Man[1] * 0x100 ) |
														  ( status->VBER_Man[0] * 0x10000 );
            
											BERexponent = status->VBER_Exp;

											vber = (float)((double)BERmantissa / pow(10.0, (double)BERexponent));	
											
											if ( status->ViterbiRate == 0x02 ) {
												if ( vber < 1E-8 ) {
													return_str = "50.0";
												} else {
													if ( vber > 1E-2 ) {
														return_str = "0.0";
													} else {
														sprintf( temp, "%f", (float)(4.0 - 0.625 * log10( vber )) );
														return_str = string( temp );
													}
												}
											} else {
												return_str = "100.0";
											}
											break;


	case DEMODULATOR_GAIN				:	return_str = string ( itoa( status->DemodulatorGain, temp, 10 ) );
											break;


	case AGCA							:	return_str = string ( itoa( status->SignalStrength, temp, 10 ) );
											break;


	case AGCN							:	return_str = string ( itoa( status->DemodulatorGain, temp, 10 ) );
											break;

	case PID_COUNT						:
											break;


	case DVB_ACCEPTED					:	return_str = string ( itoa( ntohs( status->DVBAccepted ), temp, 10 ) );
											break;


	case TOTAL_DVB_PACKETS_ACCEPTED		:	big_number = ntohs( status->TotalDVBPacketsAccepted[0] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalDVBPacketsAccepted[1] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalDVBPacketsAccepted[2] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalDVBPacketsAccepted[3] );
#ifdef WINDOWS
											return_str = string ( _ui64toa( big_number, temp, 10 ) );											
#else
											sprintf( temp, "%Lu", big_number );
											return_str = string( temp );											
#endif
											break;


	case TOTAL_DVB_PACKETS_RX_IN_ERROR	:	big_number = ntohs( status->TotalDVBPacketsRXInError[0] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalDVBPacketsRXInError[1] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalDVBPacketsRXInError[2] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalDVBPacketsRXInError[3] );
#ifdef WINDOWS
											return_str = string ( _ui64toa( big_number, temp, 10 ) );											
#else
											sprintf( temp, "%Lu", big_number );
											return_str = string( temp );											
#endif
											break;


	case TOTAL_UNCORRECTABLE_TS_PACKETS	:	big_number = ntohs( status->TotalUncorrectableTSPackets[0] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalUncorrectableTSPackets[1] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalUncorrectableTSPackets[2] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalUncorrectableTSPackets[3] );
#ifdef WINDOWS
											return_str = string ( _ui64toa( big_number, temp, 10 ) );											
#else
											sprintf( temp, "%Lu", big_number );
											return_str = string( temp );											
#endif
											break;


	case ETHERNET_TRANSMIT				:	return_str = string ( itoa( ntohs( status->EthernetTX ), temp, 10 ) );
											break;

	case ETHERNET_TRANSMIT_ERROR		:	return_str = string ( itoa( ntohs( status->EthernetTXErr ), temp, 10 ) );
											break;

	case ETHERNET_RECEIVE				:	return_str = string ( itoa( ntohs( status->EthernetRX ), temp, 10 ) );
											break;


	case ETHERNET_PACKET_DROPPED		:	return_str = string ( itoa( ntohs( status->EthernetDropped ), temp, 10 ) );
											break;


	case ETHERNET_RECEIVE_ERROR			:	return_str = string ( itoa( ntohs( status->EthernetRXErr ), temp, 10 ) );
											break;


	case TOTAL_ETHERNET_PACKETS_OUT	:		big_number = ntohs( status->TotalEthernetPacketsOut[0] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalEthernetPacketsOut[1] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalEthernetPacketsOut[2] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalEthernetPacketsOut[3] );
#ifdef WINDOWS
											return_str = string ( _ui64toa( big_number, temp, 10 ) );											
#else
											sprintf( temp, "%Lu", big_number );
											return_str = string( temp );											
#endif
											break;


	case SUBNET_MASK					:	sprintf( temp, "%d.%d.%d.%d", 
													 status->SubnetMask[0], status->SubnetMask[1],
													 status->SubnetMask[2], status->SubnetMask[3]  );
											return_str = string( temp );
											break;


	case DEFAULT_GATEWAY_IP				:	sprintf( temp, "%d.%d.%d.%d", 
													 status->DefaultGatewayIP[0],
													 status->DefaultGatewayIP[1],
													 status->DefaultGatewayIP[2],
													 status->DefaultGatewayIP[3]  );
											return_str = string( temp );
											break;


	case UNICAST_STATUS_IP				:	sprintf( temp, "%d.%d.%d.%d", 
													 status->UnicastStatusIP[0], 
													 status->UnicastStatusIP[1],
													 status->UnicastStatusIP[2],
													 status->UnicastStatusIP[3]  );
											return_str = string( temp );
											break;


	case UNICAST_STATUS_PORT			:	return_str = string ( itoa( ntohs( status->UnicastStatusPort ), temp, 10 ) );
											break;


	case BROADCAST_STATUS_PORT			:	return_str = string ( itoa( ntohs( status->BroadcastStatusPort ), temp, 10 ) );
											break;

	case IGMP_ENABLE					:	return_str = string ( itoa( ( status->Filter & 0x04 ) >> 2, temp, 10 ) );
											break;


	case LBAND_FREQUENCY				:	return_str = string ( itoa( ntohs( status->Frequency ) / 10, temp, 10 ) );
											break;


	case LO_FREQUENCY					:	return_str = string ( itoa( ntohs( status->LocalOscFrequency ), temp, 10 ) );
											break;


	case SYMBOL_RATE					:	return_str = string ( itoa( ntohs( status->SymbolRate ), temp, 10 ) );
											break;

	case LNB_POWER						:	return_str = string ( itoa( status->LNBConfiguration & 0x01, temp, 10 ) );
											break;

	case POLARITY						:	return_str = string ( itoa( ( status->LNBConfiguration & 0x02 ) >> 1, temp, 10 ) );
											break;

	case BAND							:	return_str = string ( itoa( ( status->LNBConfiguration & 0x04 ) >> 2, temp, 10 ) );
											break;

	case HI_VOLTAGE_SWITCH				:	return_str = string ( itoa( ( status->LNBConfiguration & 0x08 ) >> 3, temp, 10 ) );
											break;

	case LONG_LINE_COMPENSATION_SWITCH	:	return_str = string ( itoa( ( status->LNBConfiguration & 0x10 ) >> 4, temp, 10 ) );
											break;

	case STATUS_TIMESTAMP				:	return timestamp();
											break;

	default								:	return_str = "";
											break;

	}

	return return_str;
}



bool S75J::pollStatus( unsigned char *packet, int timeout )
{
	bool success = false;
	unsigned char message[6] = { 0x47, 0x47, 0x48, 0x48, 0x49, 0x49 };
	struct timeval timeout_struct;
	fd_set readfs;
	int result;
	int recv_len;
	string mac_string;
	char temp[20];


	// Try up to 4 times

    for ( int i = 0; (!success) && ( i < 4 ); i++ ) {

		try {
        
			if ( send( config_sockets[I_STATUS_POLL_PORT], (const char *)(&message), 6, 0 ) != 6 ) throw 1;

#ifdef WINDOWS
			Sleep(30);
#endif
			timeout_struct.tv_sec = timeout/1000;
			timeout_struct.tv_usec = timeout % 1000;

			FD_ZERO( &readfs );
			FD_SET( config_sockets[I_STATUS_POLL_PORT], &readfs );

			// Try to receive the response from receiver.  Check for errors.

#ifdef WINDOWS
			if ( ( result = select( 1, &readfs, NULL, NULL, &timeout_struct ) ) == SOCKET_ERROR ) throw 2;
#else
			if ( ( result = select( config_sockets[I_STATUS_POLL_PORT]+1, &readfs, NULL, NULL, &timeout_struct ) ) == -1 ) throw 2;
#endif

			if ( result == 0 ) throw 3;

#ifdef WINDOWS
			if ( ( recv_len = recv( config_sockets[I_STATUS_POLL_PORT], (char *)packet, 1500, 0 ) ) == SOCKET_ERROR ) {

				result = GetLastError();

				if ( result != WSAETIMEDOUT ) throw 4;

			}
#else
			if ( ( recv_len = recv( config_sockets[I_STATUS_POLL_PORT], (char *)packet, 1500, 0 ) ) == -1 ) throw 4;
#endif

			// Make sure we have received the right number of bytes back

			if ( recv_len != sizeof( S75JStatusMSG ) ) throw 5;

			sprintf( temp, "%02x-%02x-%02x-%02x-%02x-%02x",
					 ((S75JStatusMSG *)packet)->ReceiverMAC[0],
					 ((S75JStatusMSG *)packet)->ReceiverMAC[1],
					 ((S75JStatusMSG *)packet)->ReceiverMAC[2],
					 ((S75JStatusMSG *)packet)->ReceiverMAC[3],
					 ((S75JStatusMSG *)packet)->ReceiverMAC[4],
					 ((S75JStatusMSG *)packet)->ReceiverMAC[5] );

			if ( !matches( RECEIVER_MAC, string( temp ) ) ) throw 6;

			if ( ((S75JStatusMSG *)packet)->DeviceType != typeIDCode() ) throw 7;

			success = true;

		}

		catch ( int e ) {
			success = false;
		}
	
	} // for 4 retires

	return success;
}




bool S75J::applyRFConfig()
{ 
	int num_items = sizeof( rfParams ) / sizeof( ReceiverParameter );
//	unsigned char statusMsg[1500];
	bool dirty = false;
	bool up_to_date = true;
	int i;
	char temp[50];


	for ( i = 0; !dirty && ( i < num_items ); i++ ) {
		if ( dirtyFlags[rfParams[i]] ) {
			dirty = true;
		}
	}

	if ( dirty ) {

		try {

			S75JRFConfigMSG rfConfig;

			memset( (void *)&rfConfig, 0, sizeof( rfConfig ) );

			rfConfig.SymbolRate = htons( getParameter( SYMBOL_RATE, PENDING ).asShort() );

			rfConfig.Frequency = htons( 10 * getParameter( LBAND_FREQUENCY, PENDING ).asShort() );

			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( LONG_LINE_COMPENSATION_SWITCH, PENDING ).asShort();
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( HI_VOLTAGE_SWITCH, PENDING ).asShort();
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( BAND, PENDING ).asShort();
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( POLARITY, PENDING ).asShort();
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( LNB_POWER, PENDING ).asShort();

			rfConfig.ViterbiRate = 0x08;

			rfConfig.LocalOscFrequency = htons( getParameter( LO_FREQUENCY, PENDING ).asShort() );

			srand( time(NULL) );
			rfConfig.Random = rand();

			digitalsignature( &m_sec, (uint8_t *)(&rfConfig), sizeof( S75JRFConfigMSG ) - 20 );

			memcpy( rfConfig.Signature, m_sec.digest, 20 );

			if ( !sendConfig( I_RF_PORT, (unsigned char *)&rfConfig, sizeof( rfConfig ) ) ) throw 1;

			// Make pending parameters current

			for ( i = 0; i < num_items ; i++ ) {
				if ( parameterIsEnumerated( rfParams[i] ) ) {
					setCurrentParameter( rfParams[i], itoa( getParameter( rfParams[i], PENDING ).asShort(), temp, 10 ) );
				} else {
					setCurrentParameter( rfParams[i], getParameter( rfParams[i], PENDING ).asString() );
				}
				dirtyFlags[rfParams[i]] = false;
			}
		}

		catch ( int e ) {

			up_to_date = false;

		}

	}
	
	return up_to_date;
}


bool S75J::applyNetworkConfig()
{

	int num_items = sizeof( networkParams ) / sizeof( ReceiverParameter );
	bool dirty = false;
	bool up_to_date = true;
	unsigned int ip1, ip2, ip3, ip4;
        unsigned char junk[2];
	NOVRA_ERRORS ErrorCode;
//	unsigned char statusMsg[1500];
	int i;
//	char temp_str[20];
	char temp[50];


	for ( i = 0; !dirty && ( i < num_items ); i++ ) {
		if ( dirtyFlags[networkParams[i]] ) {
			dirty = true;
		}
	}

	if ( dirty ) {

		try {

			S75JNetConfigMSG netConfig;

			memset( (void *)&netConfig, 0, sizeof( netConfig ) );

			sscanf( getParameter( RECEIVER_IP, PENDING ).asString().c_str(),
					"%d%c%d%c%d%c%d", &ip1, junk, &ip2, junk, &ip3, junk, &ip4 );
			netConfig.ReceiverIP[0] = (unsigned char)ip1;
			netConfig.ReceiverIP[1] = (unsigned char)ip2;
			netConfig.ReceiverIP[2] = (unsigned char)ip3;
			netConfig.ReceiverIP[3] = (unsigned char)ip4;

			sscanf( getParameter( SUBNET_MASK, PENDING ).asString().c_str(),
					"%d%c%d%c%d%c%d", &ip1, junk, &ip2, junk, &ip3, junk, &ip4 );
			netConfig.SubnetMask[0] = (unsigned char)ip1;
			netConfig.SubnetMask[1] = (unsigned char)ip2;
			netConfig.SubnetMask[2] = (unsigned char)ip3;
			netConfig.SubnetMask[3] = (unsigned char)ip4;

			sscanf( getParameter( DEFAULT_GATEWAY_IP, PENDING ).asString().c_str(),
					"%d%c%d%c%d%c%d", &ip1, junk, &ip2, junk, &ip3, junk, &ip4 );
			netConfig.DefaultGatewayIP[0] = (unsigned char)ip1;
			netConfig.DefaultGatewayIP[1] = (unsigned char)ip2;
			netConfig.DefaultGatewayIP[2] = (unsigned char)ip3;
			netConfig.DefaultGatewayIP[3] = (unsigned char)ip4;

			sscanf( getParameter( UNICAST_STATUS_IP, PENDING ).asString().c_str(),
					"%d%c%d%c%d%c%d", &ip1, junk, &ip2, junk, &ip3, junk, &ip4 );
			netConfig.UnicastStatusIP[0] = (unsigned char)ip1;
			netConfig.UnicastStatusIP[1] = (unsigned char)ip2;
			netConfig.UnicastStatusIP[2] = (unsigned char)ip3;
			netConfig.UnicastStatusIP[3] = (unsigned char)ip4;

			netConfig.UnicastStatusPort = htons( getParameter( UNICAST_STATUS_PORT, PENDING ).asShort() );
			netConfig.BroadcastStatusPort = htons( getParameter( BROADCAST_STATUS_PORT, PENDING ).asShort() );

			netConfig.IGMP = htons( getParameter( IGMP_ENABLE, PENDING ).asShort() << 2 );
//			netConfig.IGMP = getParameter( IGMP_ENABLE, PENDING ).asShort();

			srand( time(NULL) );
			netConfig.Random = rand();

			digitalsignature( &m_sec, (uint8_t *)(&netConfig), sizeof( S75JNetConfigMSG ) - 20 );

			memcpy( netConfig.Signature, m_sec.digest, 20 );

			if ( !sendConfig( I_NETWORK_PORT, (unsigned char *)&netConfig, sizeof( netConfig ) ) ) throw 1;

			if ( !connected( 2000 ) ) {

				// Try to connect using pending network config

				if ( !_connect( 2000, ErrorCode, PENDING ) ) throw 2;

				if ( !connected( 2000 ) ) throw 3;

			}

			// Make pending parameters current

			for ( i = 0; i < num_items ; i++ ) {
				if ( parameterIsEnumerated( networkParams[i] ) ) {
					setCurrentParameter( networkParams[i], itoa( getParameter( networkParams[i], PENDING ).asShort(), temp, 10 ) );
				} else {
					setCurrentParameter( networkParams[i], getParameter( networkParams[i], PENDING ).asString() );
				}
				dirtyFlags[networkParams[i]] = false;
			}
			
		}

		catch ( int e ) {

			up_to_date = false;

		}

	}
	
	return up_to_date;
}



bool S75J::reset()
{
	S75JResetMSG resetMsg;


	resetMsg.Message[0] = 0x19;
	resetMsg.Message[1] = 0x75;
	resetMsg.Message[2] = 0x14;
	resetMsg.Message[3] = 0x03;

	srand( time(NULL) );
	resetMsg.Random = rand();

	digitalsignature( &m_sec, (uint8_t *)(&resetMsg), sizeof( S75JResetMSG ) - 20 );

	memcpy( resetMsg.Signature, m_sec.digest, 20 );

	return sendConfig( I_RESET_PORT, (unsigned char *)&resetMsg, sizeof( S75JResetMSG ) );
}



int S75J::numConfigPorts()
{
	return I_NUM_CONFIG_PORTS;
}


unsigned short S75J::configPort( int i )
{
	if ( i < numConfigPorts() ) {
		return config_ports[i];
	} else {
		return 0;
	}
}




bool S75J::checkStatusPacket( string ip, unsigned char *buffer )
{
	bool isForThisReceiver = true;


	try {
		if ( ip != getParameter( RECEIVER_IP ).asString() ) {
			if ( ip != getParameter( RECEIVER_IP, PENDING ).asString() ) throw 1;
		}
		if ( buffer[32] != typeIDCode() ) throw 1;
		if ( fixedParamFromStatus( RECEIVER_MAC, buffer ) != getParameter( RECEIVER_MAC ).asString() ) throw 1;
	}

	catch ( int ) {
		isForThisReceiver = false;
	}

	return isForThisReceiver;
}




bool S75J::createIndexedParameters()
{
	int i;

	currentPIDs = new ParameterValue[NUMBER_OF_PIDS];
	pendingPIDs = new ParameterValue[NUMBER_OF_PIDS];

	return true;
}



bool S75J::login( string password )
{
	struct sockaddr_in device_address;
	SOCKET login_socket;
	int temp1 = 1000;               // Temporary storage variable
	int last_error_code = -1;
	int i;
	string mac;
	int mac_byte[6];
	char junk[2];
	bool success=false;
	struct timeval timeout;
	fd_set readfs;
	int result;
	S75JPIDRequestMSG pidRequest;
    BYTE Buffer2[sizeof(S75JPIDMSG)];


	try {

		// Passwords are tested by requesting the PID List from the receiver

		memset( (void *)&m_sec, 0, sizeof( m_sec ) );
		memset( (void *)&pidRequest, 0, sizeof( pidRequest ) );

		pidRequest.Opcode = htons(0x0001);

		srand( time(NULL) );
		pidRequest.Random = rand();

		mac = getParameter( RECEIVER_MAC ).asString();
		sscanf( mac.c_str(), "%2x%c%2x%c%2x%c%2x%c%2x%c%2x",
				&(mac_byte[0]), junk, &(mac_byte[1]), junk, &(mac_byte[2]),
				junk, &(mac_byte[3]), junk, &(mac_byte[4]), junk, &(mac_byte[5]) );
		for ( i = 0; i < 6; i++ ) {
			m_sec.macaddress[i] = (uint8_t)mac_byte[i];
		}
	
		for ( i = 0; ( i < 8 ) && ( i < (int)(password.length()) ); i++ ) {
			m_sec.password[i] = ((char *)(password.data()))[i];
		}

		digitalsignature( &m_sec, (uint8_t *)(&pidRequest), sizeof( S75JPIDRequestMSG ) - 20 );

		memcpy( pidRequest.Signature, m_sec.digest, 20 );


		// Create a socket to communicate with the receiver

#ifdef WINDOWS
		if ( ( login_socket = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) throw 1;
#else
		if ( ( login_socket = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) throw 1;
#endif


		// Connect the newly created socket to the receiver.

   		device_address.sin_family = AF_INET;

   		device_address.sin_port = htons( PID_CONFIG_PORT );

    	device_address.sin_addr.s_addr = inet_addr( getParameter( RECEIVER_IP ).asString().c_str() );

		if ( ::connect( login_socket, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr_in ) ) != 0) throw 2;

			
        // Download the PID table.  Retry (if required) up to 4 times

        for ( i = 0; (!success) && ( i < 4 ); i++) {

           	// Send the PID table request to the receiver

            if ( send( login_socket, (const char *)(&pidRequest), sizeof( S75JPIDRequestMSG), 0 ) != sizeof( S75JPIDRequestMSG ) ) {

                last_error_code = N_ERROR_PID_SEND;

			} else {

#ifdef WINDOWS
                Sleep(30);
#endif

                timeout.tv_sec = 1;
                timeout.tv_usec = 0;

       	        FD_ZERO( &readfs );
       	        FD_SET( login_socket, &readfs );

                // Try to receive the response from receiver.  Check for errors.

#ifdef WINDOWS
                if ( ( result = select( 1, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#else
                if ( ( result = select( login_socket+1, &readfs, NULL, NULL, &timeout ) ) != -1 ) {
#endif
	       	        if ( result != 0 ) {

#ifdef WINDOWS
  		    	        if( (temp1 = recv( login_socket, (char *)Buffer2, sizeof(S75JPIDMSG), 0 ) ) == SOCKET_ERROR ) {

            	    		temp1 = GetLastError();

                	        if(temp1 != WSAETIMEDOUT){

        	        			last_error_code = N_ERROR_PID_RECEIVE;

							} // if


#else
   		    	        if( (temp1 = recv( login_socket, (char *)Buffer2, sizeof(S75JPIDMSG), 0 ) ) == -1 ) {
							last_error_code = N_ERROR_PID_RECEIVE;
#endif

						} else {

	    	    	        // Make sure we have received the right number of bytes back

    		    	        if (temp1 == sizeof(S75JPIDMSG)) {

				                last_error_code = N_ERROR_SUCCESS;
								success = true;

							} else {

								last_error_code = N_ERROR_PID_ACK_BAD;

							} // if

						} // if recv

					} else {

						last_error_code = N_ERROR_PID_RECEIVE;

					} // if select result 0

				} else {

					last_error_code = N_ERROR_PID_RECEIVE;

				} // if select

			} // if send
	
		} // for 4 retires

	}


	catch ( int e ) {

		switch ( e ) {

			case 1	:	last_error_code = N_ERROR_SOCKET_CREATE;
						break;

			case 2	:	last_error_code = N_ERROR_SOCKET_CONNECT;
						break;

		}
	}


#ifdef WINDOWS
    closesocket( login_socket );
#else
    close( login_socket );
#endif


	if ( last_error_code == N_ERROR_SUCCESS ) {
		
		return true; 
	
	} else {
		
		return false;

	}
}


bool S75J::apply( ReceiverParameter ParameterName )
{

	int result = 0;

	try {
		if ( ParameterName == ALL_PARAMETERS ) {
			if ( !applyPIDConfig() ) throw 1;
			if ( !applyRFConfig() ) throw 4;
			if ( !applyNetworkConfig() ) throw 8;
		} else {
			throw 9;
		}
	}

	catch ( int e ) {

		result = e;

	}

	return !result;

}


bool S75J::applyPIDConfig()
{ 
	int num_items = sizeof( pidParams ) / sizeof( ReceiverParameter );
	bool dirty = false;
	bool up_to_date = true;
	int i;
	unsigned short pid;
	int ip1, ip2, ip3, ip4;
	char junk[2];
	bool forward_ts = false;
//	char temp[10];
	char temp[50];


	for ( i = 0; !dirty && ( i < num_items ); i++ ) {
		if ( dirtyFlags[pidParams[i]] ) {
			dirty = true;
		}
	}

	if ( dirty ) {

		try {

			S75JPIDMSG pidConfig;

			memset( (void *)&pidConfig, 0, sizeof( S75JPIDMSG ) );

			pidConfig.Opcode = htons( (unsigned short)0x2 );


// July 16, 2009			if ( !forward_ts ) {
				for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {
//					if ( pendingPIDs[i].isSet() ) {
						pid = pendingPIDs[i].asShort();
//					} else {
//						pid = 0x1fff;
//					}
					if ( pid == 0x1fff ) {
						pidConfig.PIDTableEntry[i] = htons( 0x1fff );
					} else {
						pidConfig.PIDTableEntry[i] = htons( 0x2000 | pid );
					}
				}

// July 16, 2009			}


			srand( time(NULL) );
			pidConfig.Random = rand();

			digitalsignature( &m_sec, (uint8_t *)(&pidConfig), sizeof( S75JPIDMSG ) - 20 );

			memcpy( pidConfig.Signature, m_sec.digest, 20 );

			if ( !sendConfig( I_PID_PORT, (unsigned char *)&pidConfig, sizeof( pidConfig ) ) ) throw 1;

			// Make pending parameters current


				for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {
					currentPIDs[i].setM_value( pendingPIDs[i].getM_value() );
				}


			for ( i = 0; i < num_items; i++ ) {
				if ( parameterIsEnumerated( pidParams[i] ) ) {
					setCurrentParameter( pidParams[i], itoa( getParameter( pidParams[i], PENDING ).asShort(), temp, 10 ) );
				} else {
					setCurrentParameter( pidParams[i], getParameter( pidParams[i], PENDING ).asString() );
				}
				dirtyFlags[pidParams[i]] = false;
			}

		}

		catch ( int e ) {

			up_to_date = false;

		}

	}

	return up_to_date;
}


bool S75J::readCurrentSettings()
{
	bool success = true;

	if ( !readPIDTable() ) success = false;

	return success;
}


bool S75J::readPIDTable()
{
	bool success = false;
    S75JPIDRequestMSG pidRequest;
 	int i;
	unsigned char receive_buffer[1500];
	S75JPIDMSG *pidReply = (S75JPIDMSG *)receive_buffer;
	char temp[20];
	unsigned short pid;
	unsigned short mask;



	// Download PIDS

	memset( (void *)&pidRequest, 0, sizeof( pidRequest ) );
	memset( (void *)receive_buffer, 0, 1500 );

	// Set the opcode to download (0x0001)

	pidRequest.Opcode = htons(0x0001);
	srand( time(NULL) );
	pidRequest.Random = rand();

	digitalsignature( &m_sec, (uint8_t *)(&pidRequest), sizeof( pidRequest ) - 20 );

	memcpy( pidRequest.Signature, m_sec.digest, 20 );

	if ( getConfig( I_PID_PORT, (unsigned char *)&pidRequest, receive_buffer, sizeof( pidRequest ), sizeof( S75JPIDMSG ) ) ) {


			for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {

				pid = ntohs( pidReply->PIDTableEntry[i] );
				currentPIDs[i].setM_value( string( itoa( ( pid & 0x1fff ), temp, 10 ) ) );			
				pendingPIDs[i].setM_value( currentPIDs[i].getM_value() );

			}

		success = true;

	}
	
	return success;
}



bool S75J::setIndexedParameter( ReceiverParameter ParameterName, int index, string ParameterValue )
{
	bool success = false;

	
	switch ( ParameterName ) {

		case PID					:	if ( index < NUMBER_OF_PIDS ) {

											pendingPIDs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;


		default						:	success = false;
						
										break;

	}

	return success;
}



ParameterValue& S75J::getIndexedParameter(	ReceiverParameter ParameterName, 
											int index, 
											ParameterSet parameter_set  )
{
	switch( ParameterName ) {

		case PID					:	if ( index < NUMBER_OF_PIDS ) {

											if ( parameter_set == CURRENT ) {

												return currentPIDs[index];

											} else {

												if ( pendingPIDs[index].isSet() ) {

													return pendingPIDs[index];

												} else {

													return currentPIDs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}


		default		:	return nullParameter;

	}

}



void S75J::cancelIndexedParameters()
{
	int i;


	for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {

		if ( currentPIDs[i].isSet() ) {

			pendingPIDs[i].setM_value( currentPIDs[i].getM_value() );

		} else {

			pendingPIDs[i].setM_isSet( false );

		}

	}

}


void S75J::clearIndexedParameters()
{
	int i;


	for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {

		pendingPIDs[i].setM_value( "8191" );

	}
}



bool S75J::changePassword( char *password, unsigned char key[16] )
{
	S75JSetPasswordMSG password_message;
	int i;
	string mac;
	int mac_byte[6];
	char junk[2];


	mac = getParameter( RECEIVER_MAC ).asString();
	sscanf( mac.c_str(), "%2x%c%2x%c%2x%c%2x%c%2x%c%2x",
			&(mac_byte[0]), junk, &(mac_byte[1]), junk, &(mac_byte[2]),
			junk, &(mac_byte[3]), junk, &(mac_byte[4]), junk, &(mac_byte[5]) );
	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)mac_byte[i];
	}
	
	memset( m_sec.password, 0, 8 );
	for ( i = 0; ( i < 8 ) && ( i < (int)strlen( password ) ); i++ ) {
		m_sec.password[i] = password[i];
	}

	// MAC address to msg buffer
	memmove( password_message.newPassword, m_sec.macaddress, sizeof( m_sec.macaddress ) );

	// pad MAC address to 8-bytes
	password_message.newPassword[6] = 0;
	password_message.newPassword[7] = 0;

	// copy password to message buffer
	memmove(&(password_message.newPassword[8]), m_sec.password, 8 );

//	srand( time(NULL) );
//	password_message.Random = rand();
	
	// compute digital signature on clear text
	// password and MAC address
	digitalsignature(&m_sec, (unsigned char *)(&password_message), sizeof( S75JSetPasswordMSG ) - 20 );
	memmove( password_message.Signature, m_sec.digest, sizeof(m_sec.digest));

	// encrypt MAC address
	encrypt(key, &(password_message.newPassword[0]));

	// encrypt password
	encrypt(key, &(password_message.newPassword[8]));

	return sendConfig( I_SET_PASSWORD_PORT, (unsigned char *)&password_message, sizeof( password_message ) );
}
