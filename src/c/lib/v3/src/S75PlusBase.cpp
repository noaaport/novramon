//#include "stdafx.h"

#include "S75PlusBase.h"

#ifdef WINDOWS
    #include "Winsock2.h"
#endif

#ifndef WINDOWS
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

#include "math.h"
#include "Password.h"
#include "Keycrypt.h"
#include <time.h>
#include "ConfigPorts.h"

#define NUMBER_OF_PIDS				32
#define NUMBER_OF_DESTINATIONS		16
#define NUMBER_OF_PATS				16
#define NUMBER_OF_PROGRAMS			16
#define NUMBER_OF_FIXED_KEYS		64
#define NUMBER_OF_IP_REMAP_RULES	16

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
const int PROGRAM_INFO_PORT		= 0x9992;
const int VIDEO_PROGRAM_PORT	= 0x9993;
const int IP_REMAP_PORT			= 0x9996;
const int DEFAULT_CONFIG_PORT	= 0x9995;
const int PSI_TABLE_PORT		= 0x9996;
const int CAM_WATCHDOG_PORT		= 0x9997;
const int RESET_PORT			= 0x9999;


/*
enum CONFIG_PORT_INDEXES {	I_PAT_CONFIG_PORT,
							I_PROGRAM_CONFIG_PORT,
							I_NETWORK_PORT,
							I_PID_PORT,
							I_RF_PORT,
							I_PRODUCTID_CONFIG_PORT,
							I_TRAP_CONFIG_PORT,
							I_SET_PASSWORD_PORT,
							I_STATUS_POLL_PORT,
							I_FK_CONFIG_PORT,
							I_IP_REMAP_CONFIG_PORT,
							I_DEFAULT_CONFIG_PORT,
							I_PSI_TABLE_PORT,
							I_CAM_WATCHDOG_PORT,
							I_RESET_PORT			};
*/


/*
static const unsigned short config_ports[] = {	PAT_CONFIG_PORT,
												PROGRAM_CONFIG_PORT,
												NETWORK_CONFIG_PORT,
												PID_CONFIG_PORT,
												RF_CONFIG_PORT,
												PRODUCTID_CONFIG_PORT,
												TRAP_CONFIG_PORT,
												SET_PASSWORD_PORT,
												STATUS_POLL_PORT,												
												FK_CONFIG_PORT,
												IP_REMAP_PORT,
												DEFAULT_CONFIG_PORT,
												PSI_TABLE_PORT,
												CAM_WATCHDOG_PORT,
												RESET_PORT			};
*/

static ReceiverParameter networkParams[] = {	RECEIVER_IP,
												SUBNET_MASK,
												DEFAULT_GATEWAY_IP,
												UNICAST_STATUS_IP,
												UNICAST_STATUS_PORT,
												BROADCAST_STATUS_PORT,
												IGMP_ENABLE				};


static ReceiverParameter rfParams[] = {	LBAND_FREQUENCY,
										SYMBOL_RATE,
// DIFF200										DVB_SIGNAL_TYPE_CONTROL,
										LNB_POWER,
										BAND,
										POLARITY,
										HI_VOLTAGE_SWITCH,
										LONG_LINE_COMPENSATION_SWITCH,
										POLARITY_SWITCHING_VOLTAGE,
										BAND_SWITCHING_TONE_FREQUENCY,
										LO_FREQUENCY  	 				};


typedef unsigned char BYTE;
typedef unsigned short WORD;


typedef struct PIDRoute {
	WORD PID;
	WORD DestinationMask;
} PIDRoute;


typedef struct PIDDestination {
	BYTE DestinationIP[4];
	WORD DestinationUDP;
} PIDDestination;


typedef struct StatusMSG {
	BYTE			ProductID[32];					// Product Identification
	BYTE			DeviceType;
	BYTE			DSPFirmWare;					// Firmware Version Revision.
	BYTE			ReceiverMAC[6] ;				// Receiver MAC Address.
	BYTE			ReceiverIP[4];
    BYTE			SubnetMask[4];					// Receiver Subnet Mask
    BYTE			DefaultGatewayIP[4];			// Default Gateway Address
	BYTE			UnicastStatusIP[4];				// Destination IP Address for Unicast Status
	WORD			UnicastStatusPort;				// Destination UDP Port.	
	WORD			MicroFirmWare;					// Micro controller firmware version
	unsigned short	SymbolRate;						// Symbol rate in KSPS
	unsigned short	Frequency;						// Frequency in 100 KHz
	BYTE			LNBConfiguration;				// LNB control settings
	BYTE			ViterbiRate;
	BYTE			DemodulatorStatus;				// Demodulator Status
	BYTE			SignalStrength;					// AGC
	BYTE			VBER_Man[3];					// Viterbi BER Mantisa
	BYTE			VBER_Exp;						// Viterbi BER Exponent
	BYTE			Uncorrectables;					// Uncorrectable errors count since alst status
	BYTE			DemodulatorGain;				// the Digital gain
	BYTE			NEST;							// Noise estimate 
	BYTE			Clock_Off;						// Clock offset
	WORD			Freq_Err;						// Frequency Error 
	BYTE			ADC_MID;						// MID point crossing for ADC
	BYTE			ADC_CLIP;						// ADC Clipping count.
	BYTE			Digital_Gain;					// Digital gain
	BYTE			AA_CLAMP;						//Clamping count
	WORD			RFEND;							// RF message termination .
	unsigned short	EthernetTX;						// Ethernet Transmitted Packets Counter
	unsigned short	EthernetDropped;				// Number of Ethernet Packets Dropped by the Ethernet chip
	unsigned short	EthernetRX;						// Number of Ethernet Packets Received
	unsigned short	EthernetRXErr;					// Count of Ethernet Packets Received In Error.
	unsigned short	DVBScrambled;					// DVB packets forwarded in a scrambled state
	unsigned short	DVBAccepted;					// Total Number of DVB Packets that passed the Filter.
	unsigned short	DVBDescrambled;					// DVB packets descrambled or not scrambled
	unsigned short	Sync_Loss;						// Packets Received with FEL high and No Sync Byte.	
	BYTE			Filter;							// IGMP On/Off flag[2]
	BYTE			CardStatus;						// CA Controller Status

	unsigned short	TotalDVBPacketsAccepted[4];		// Statistics counters
	unsigned short	TotalDVBPacketsRXInError[4];
	unsigned short	TotalEthernetPacketsOut[4];
	unsigned short	TotalUncorrectableTSPackets[4];
	unsigned short	BroadcastStatusPort;			// Broadcast status UDP port
	unsigned short	LocalOscFrequency;				// LNB Local Oscillator Frequency

	// PID routing tables
    PIDRoute PIDList[NUMBER_OF_PIDS];
	PIDDestination PIDDestinations[NUMBER_OF_DESTINATIONS];

	// CA controller status		

	unsigned short ProgramList[NUMBER_OF_PROGRAMS];
	unsigned short ProgramStatus[NUMBER_OF_PROGRAMS];
	
} StatusMSG;


typedef struct NetConfigMSG {

	BYTE  ReceiverIP[4];		// the IP address to be assumed by the device
    BYTE  SubnetMask[4];		// the Subnet Mask to be used by the device
    BYTE  DefaultGatewayIP[4];	// the default gateway to be used by the device
	BYTE  UnicastStatusIP[4];
	WORD  UnicastStatusPort;  
	WORD  BroadcastStatusPort;			// the status port 
	WORD  IGMP;                 // 0 = filter off, 1= IGMP Filter on
	WORD  Reserved; 
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1

} NetConfigMSG;


typedef struct RFConfigMSG {

	WORD  SymbolRate;              // in ksps
	WORD  Frequency;               // in units of 10 Hz
	BYTE  LNBConfig;               // bit0 - 0 = power off, 1 = power on
                                   // bit1 - 0 = vertical or right polarization
	                               //        1 = horizontal or left polarization
	                               // bit2 - 0 = low band, 1 = high band
								   // bit3 - Hi voltage switch
								   // bit4 - Long line compensation switch
								   // bit5 - Polarity Switching Voltage, 0 = 13/18, 1 = 11/15
								   // bit6 - Tone switching frequency, 0 = 22kHz, 1 = 44kHz
	BYTE  ViterbiRate;             // 0x08 = auto
	WORD  LocalOscFrequency;
	WORD  Random;				   // 
	BYTE  Signature[20];		   // HMAC-SHA1

} RFConfigMSG;


// @@@DBW Added ResetMSG NOV 26,2010
typedef struct ResetMSG {

	BYTE Message[4];	 
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1

} ResetMSG;


extern pstruct m_sec;


S75PlusBase::S75PlusBase()
{
	SETUP_PORT_TABLE
}


S75PlusBase::S75PlusBase( unsigned char *buffer )
{
	SETUP_PORT_TABLE
}


S75PlusBase::~S75PlusBase()
{
}



string S75PlusBase::fixedParamFromStatus( ReceiverParameter p, unsigned char *buffer )
{
	char temp[100];
	string return_str = "";
	StatusMSG *status = (StatusMSG *)buffer;

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
	case FPGA_VERSION	:	return_str = string ( itoa( ( ntohs( status->MicroFirmWare ) & 0xff00 ) >> 8, temp, 10 ) );
									break;
	case FPGA_REVISION	:	return_str = 	string ( itoa( ( ntohs( status->MicroFirmWare ) & 0x00f0 ) >> 4, temp, 10 ) );
									break;
	case CAM_VERSION	:	return_str = string ( itoa( ( ntohs( status->MicroFirmWare ) & 0xff00 ) >> 8, temp, 10 ) );
									break;
	case CAM_REVISION	:	return_str = 	string ( itoa( ( ntohs( status->MicroFirmWare ) & 0x00f0 ) >> 4, temp, 10 ) );
									break;

	default						:	return_str = "";
									break;

	}

	return return_str;
}


string S75PlusBase::dynamicParamFromStatus( ReceiverParameter p, unsigned char *buffer, int index )
{
	StatusMSG *status = (StatusMSG *)buffer;
	char temp[100];
	string return_str = "";
	bool signal_lock;
	double ss;
	float vber;
	int	BERmantissa;   // Bit Error Rate mantissa
	int	BERexponent;   // Bit Error Rate exponent
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
											if ( ( status->SignalStrength == 255 ) && ( !signal_lock ) ) {
												ss = 0;
											} else {
												ss = 140.0 - ( status->SignalStrength / 2.0 );
											}
											sprintf( temp, "%f", ss );
											return_str = string( temp );
											break;

	case SIGNAL_LOCK					:	return_str = string ( itoa( status->DemodulatorStatus & 0x01, temp, 10 ) );
											break;

	case DATA_LOCK						:	return_str = string ( itoa( ( status->DemodulatorStatus & 0x10 ) >> 4, temp, 10 ) );
											break;

	case CAM_STATUS						:	sprintf( temp, "%d", status->CardStatus );
											return_str = string( temp );
											break;

	case CA_PROGRAM_STATUS				:	if ( ( index >= 0 ) && ( index < NUMBER_OF_PROGRAMS ) ) {
												sprintf( temp, "%d", ntohs( status->ProgramStatus[index] ) );
												return_str = string( temp );
											} else {
												return_str = "0";
											}
											break;

	case LNB_FAULT						:	return_str = string ( itoa( ( status->LNBConfiguration & 0x80 ) >> 7, temp, 10 ) );
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
											
											if ( status->ViterbiRate == THREE_QUARTERS ) {
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


	case GNYQA							:	return_str = string ( itoa( ( status->Digital_Gain & 0x30 ) >> 4, temp, 10 ) );
											break;


	case GFARA							:	return_str = string ( itoa( status->Digital_Gain & 0x0f, temp, 10 ) );
											break;


	case NEST							:	return_str = string ( itoa( status->NEST, temp, 10 ) );
											break;


	case CLOCK_OFFSET					:	return_str = string ( itoa( status->Clock_Off, temp, 10 ) );
											break;


	case FREQUENCY_ERROR				:	return_str = string ( itoa( (signed short)( 1.46484 * (ntohs( status->Freq_Err) ) ), temp, 10 ) );
											break;

	case CARRIER_FREQUENCY				:	signed short freq_err;
											freq_err = (signed short)( ntohs( status->Freq_Err ));
											if ( freq_err > 0 ) {
												sprintf( temp, "%d MHz (+%d Hz)", ntohs( status->Frequency ) / 10, (int)( 1.46484 * freq_err ) );
											} else {
												sprintf( temp, "%d MHz (%d Hz)", ntohs( status->Frequency ) / 10, (int)( 1.46484 * freq_err ) );
											}
											return_str = string( temp );
											break;

	case ADC_MID						:	return_str = string ( itoa( status->ADC_MID, temp, 10 ) );
											break;


	case ADC_CLIP						:	return_str = string ( itoa( status->ADC_CLIP, temp, 10 ) );
											break;


	case DIGITAL_GAIN					:	return_str = string ( itoa( status->Digital_Gain, temp, 10 ) );
											break;


	case AA_CLAMP						:	return_str = string ( itoa( status->AA_CLAMP, temp, 10 ) );
											break;


	case PID_COUNT						:
											break;


	case DVB_ACCEPTED					:	return_str = string ( itoa( ntohs( status->DVBAccepted ), temp, 10 ) );
											break;


	case TOTAL_DVB_PACKETS_ACCEPTED		:	if ( status->DSPFirmWare > 0x47 ) {
											big_number = ntohs( status->TotalDVBPacketsAccepted[3] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalDVBPacketsAccepted[2] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalDVBPacketsAccepted[1] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalDVBPacketsAccepted[0] );
							} else {
											big_number = ntohs( status->TotalDVBPacketsAccepted[2] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalDVBPacketsAccepted[3] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalDVBPacketsAccepted[0] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalDVBPacketsAccepted[1] );
							} 
#ifdef WINDOWS
											return_str = string ( _ui64toa( big_number, temp, 10 ) );											
#else
											sprintf( temp, "%Lu", big_number );
											return_str = string( temp );											
#endif
											break;


	case TOTAL_UNCORRECTABLE_TS_PACKETS	:	if ( status->DSPFirmWare > 0x47 ) {
											big_number = ntohs( status->TotalUncorrectableTSPackets[3] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalUncorrectableTSPackets[2] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalUncorrectableTSPackets[1] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalUncorrectableTSPackets[0] );
							} else {
											big_number = ntohs( status->TotalUncorrectableTSPackets[2] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalUncorrectableTSPackets[3] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalUncorrectableTSPackets[0] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalUncorrectableTSPackets[1] );
							}
#ifdef WINDOWS
											return_str = string ( _ui64toa( big_number, temp, 10 ) );											
#else
											sprintf( temp, "%Lu", big_number );
											return_str = string( temp );											
#endif
											break;


	case ETHERNET_TRANSMIT				:	return_str = string ( itoa( ntohs( status->EthernetTX ), temp, 10 ) );
											break;


	case ETHERNET_RECEIVE				:	return_str = string ( itoa( ntohs( status->EthernetRX ), temp, 10 ) );
											break;


	case ETHERNET_PACKET_DROPPED		:	return_str = string ( itoa( ntohs( status->EthernetDropped ), temp, 10 ) );
											break;


	case ETHERNET_RECEIVE_ERROR			:	return_str = string ( itoa( ntohs( status->EthernetRXErr ), temp, 10 ) );
											break;


	case DVB_SCRAMBLED					:	return_str = string ( itoa( ntohs( status->DVBScrambled ), temp, 10 ) );
											break;


	case DVB_CLEAR						:	return_str = string ( itoa( ntohs( status->DVBDescrambled ), temp, 10 ) );
											break;


	case TOTAL_ETHERNET_PACKETS_OUT	:	if ( status->DSPFirmWare > 0x47 ) {
											big_number = ntohs( status->TotalEthernetPacketsOut[3] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalEthernetPacketsOut[2] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalEthernetPacketsOut[1] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalEthernetPacketsOut[0] );
						} else {
											big_number = ntohs( status->TotalEthernetPacketsOut[2] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalEthernetPacketsOut[3] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalEthernetPacketsOut[0] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalEthernetPacketsOut[1] );
						}
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

	case POLARITY_SWITCHING_VOLTAGE		:	return_str = string ( itoa( ( status->LNBConfiguration & 0x20 ) >> 5, temp, 10 ) );
											break;

	case BAND_SWITCHING_TONE_FREQUENCY	:	return_str = string ( itoa( ( status->LNBConfiguration & 0x40 ) >> 6, temp, 10 ) );
											break;

	case STATUS_TIMESTAMP			:	return timestamp();
											break;

	default								:	return_str = "";
											break;

	}

	return return_str;
}



bool S75PlusBase::pollStatus( unsigned char *packet, int timeout )
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

			if ( recv_len != sizeof( StatusMSG ) ) throw 5;

			sprintf( temp, "%02x-%02x-%02x-%02x-%02x-%02x",
					 ((StatusMSG *)packet)->ReceiverMAC[0],
					 ((StatusMSG *)packet)->ReceiverMAC[1],
					 ((StatusMSG *)packet)->ReceiverMAC[2],
					 ((StatusMSG *)packet)->ReceiverMAC[3],
					 ((StatusMSG *)packet)->ReceiverMAC[4],
					 ((StatusMSG *)packet)->ReceiverMAC[5] );

			if ( !matches( RECEIVER_MAC, string( temp ) ) ) throw 6;

			if ( ((StatusMSG *)packet)->DeviceType != typeIDCode() ) throw 7;

			success = true;

		}

		catch ( int e ) {
			success = false;
		}
	
	} // for 4 retires

	return success;
}




bool S75PlusBase::applyRFConfig()
{ 
	int num_items = sizeof( rfParams ) / sizeof( ReceiverParameter );
//	unsigned char statusMsg[1500];
	bool dirty = false;
	bool up_to_date = true;
	int i;


	for ( i = 0; !dirty && ( i < num_items ); i++ ) {
		if ( dirtyFlags[rfParams[i]] ) {
			dirty = true;
		}
	}

	if ( dirty ) {

		try {

			RFConfigMSG rfConfig;

			memset( (void *)&rfConfig, 0, sizeof( rfConfig ) );

			rfConfig.SymbolRate = htons( getParameter( SYMBOL_RATE, PENDING ).asShort() );

			rfConfig.Frequency = htons( 10 * getParameter( LBAND_FREQUENCY, PENDING ).asShort() );

			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( BAND_SWITCHING_TONE_FREQUENCY, PENDING ).asShort();
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( POLARITY_SWITCHING_VOLTAGE, PENDING ).asShort();
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( LONG_LINE_COMPENSATION_SWITCH, PENDING ).asShort();
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( HI_VOLTAGE_SWITCH, PENDING ).asShort();
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( BAND, PENDING ).asShort();
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( POLARITY, PENDING ).asShort();
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( LNB_POWER, PENDING ).asShort();

			rfConfig.ViterbiRate = 0x08;

			rfConfig.LocalOscFrequency = htons( getParameter( LO_FREQUENCY, PENDING ).asShort() );

			srand( time(NULL) );
			rfConfig.Random = rand();

			digitalsignature( &m_sec, (uint8_t *)(&rfConfig), sizeof( RFConfigMSG ) - 20 );

			memcpy( rfConfig.Signature, m_sec.digest, 20 );

			if ( !sendConfig( I_RF_PORT, (unsigned char *)&rfConfig, sizeof( rfConfig ) ) ) throw 1;

			// Make pending parameters current

			for ( i = 0; i < num_items ; i++ ) {
				setCurrentParameter( rfParams[i], getParameter( rfParams[i], PENDING ).asString() );
				dirtyFlags[rfParams[i]] = false;
			}
		}

		catch ( int e ) {

			up_to_date = false;

		}

	}
	
	return up_to_date;
}


bool S75PlusBase::applyNetworkConfig()
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


	for ( i = 0; !dirty && ( i < num_items ); i++ ) {
		if ( dirtyFlags[networkParams[i]] ) {
			dirty = true;
		}
	}

	if ( dirty ) {

		try {

			NetConfigMSG netConfig;

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

			digitalsignature( &m_sec, (uint8_t *)(&netConfig), sizeof( NetConfigMSG ) - 20 );

			memcpy( netConfig.Signature, m_sec.digest, 20 );

			if ( !sendConfig( I_NETWORK_PORT, (unsigned char *)&netConfig, sizeof( netConfig ) ) ) throw 1;

			if ( !connected( 2000 ) ) {

				// Try to connect using pending network config

				if ( !_connect( 2000, ErrorCode, PENDING ) ) throw 2;

				if ( !connected( 2000 ) ) throw 3;

			}

/*
			if ( !pollStatus( (unsigned char *)statusMsg, 2000 ) ) throw 3;


			sprintf( temp_str, "%d.%d.%d.%d",
					 ((StatusMSG *)statusMsg)->ReceiverIP[0],
					 ((StatusMSG *)statusMsg)->ReceiverIP[1],
					 ((StatusMSG *)statusMsg)->ReceiverIP[2],
					 ((StatusMSG *)statusMsg)->ReceiverIP[3]  );

			if ( !matches( RECEIVER_IP, string( temp_str ), PENDING ) ) throw 4;

			sprintf( temp_str, "%d.%d.%d.%d",
					 ((StatusMSG *)statusMsg)->SubnetMask[0],
					 ((StatusMSG *)statusMsg)->SubnetMask[1],
					 ((StatusMSG *)statusMsg)->SubnetMask[2],
					 ((StatusMSG *)statusMsg)->SubnetMask[3]  );

			if ( !matches( SUBNET_MASK, string( temp_str ), PENDING ) ) throw 5;

			sprintf( temp_str, "%d.%d.%d.%d",
					 ((StatusMSG *)statusMsg)->DefaultGatewayIP[0],
					 ((StatusMSG *)statusMsg)->DefaultGatewayIP[1],
					 ((StatusMSG *)statusMsg)->DefaultGatewayIP[2],
					 ((StatusMSG *)statusMsg)->DefaultGatewayIP[3]  );

			if ( !matches( DEFAULT_GATEWAY_IP, string( temp_str ), PENDING ) ) throw 6;

			sprintf( temp_str, "%d.%d.%d.%d",
					 ((StatusMSG *)statusMsg)->UnicastStatusIP[0],
					 ((StatusMSG *)statusMsg)->UnicastStatusIP[1],
					 ((StatusMSG *)statusMsg)->UnicastStatusIP[2],
					 ((StatusMSG *)statusMsg)->UnicastStatusIP[3]  );

			if ( !matches( UNICAST_STATUS_IP, string( temp_str ), PENDING ) ) throw 7;

			if ( !matches( UNICAST_STATUS_PORT, string( itoa( ntohs( ((StatusMSG *)statusMsg)->UnicastStatusPort ), temp_str, 10 ) ), PENDING ) ) throw 8;

			if ( !matches( BROADCAST_STATUS_PORT, string( itoa( ntohs( ((StatusMSG *)statusMsg)->BroadcastStatusPort ), temp_str, 10 ) ), PENDING ) ) throw 9;

			if ( !matches( IGMP_ENABLE, string ( itoa( ( ((StatusMSG *)statusMsg)->Filter & 0x04 ) >> 2, temp_str, 10 ) ), PENDING ) ) throw 10;
*/
			// Make pending parameters current

			for ( i = 0; i < num_items ; i++ ) {
				setCurrentParameter( networkParams[i], getParameter( networkParams[i], PENDING ).asString() );
				dirtyFlags[networkParams[i]] = false;
			}
			
		}

		catch ( int e ) {

			up_to_date = false;

		}

	}
	
	return up_to_date;
}


/* @@@DBW NOV 26, 2010
bool S75PlusBase::reset()
{
	unsigned char dataBuffer[4] = { 0x19, 0x75, 0x14, 0x03 };  // Data buffer (data to be sent to S75)

	sendConfig( I_RESET_PORT, dataBuffer, 4 );

	return true;
}
*/


// @@@DBW NOV 26, 2010 replacement for above
// 4.10 firmware seems to not use signed packets, 4.12 does
bool S75PlusBase::reset()
{
	ResetMSG resetMsg;


	resetMsg.Message[0] = 0x19;
	resetMsg.Message[1] = 0x75;
	resetMsg.Message[2] = 0x14;
	resetMsg.Message[3] = 0x03;

	srand( time(NULL) );
	resetMsg.Random = rand();

	digitalsignature( &m_sec, (uint8_t *)(&resetMsg), sizeof( ResetMSG ) - 20 );

	memcpy( resetMsg.Signature, m_sec.digest, 20 );

	return sendConfig( I_RESET_PORT, (unsigned char *)&resetMsg, sizeof( ResetMSG ) );
}
