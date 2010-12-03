//************************************************************************
//*
//* Copyright Novra Technologies Incorporated, 2003
//* 1100-330 St. Mary Avenue
//* Winnipeg, Manitoba
//* Canada R3C 3Z5
//* Telephone (204) 989-4724
//*
//* All rights reserved. The copyright for the computer program(s) contained
//* herein is the property of Novra Technologies Incorporated, Canada.
//* No part of the computer program(s) contained herein may be reproduced or
//* transmitted in any form by any means without direct written permission
//* from Novra Technologies Inc. or in accordance with the terms and
//* conditions stipulated in the agreement/contract under which the
//* program(s) have been supplied.
//*
//***************************************************************************
//
// This File defines the Packets used to communicate with the S75CA
// 

#ifndef		__NOVRADEVICEPACKETS__
#define		__NOVRADEVICEPACKETS__

#include "S75JLEOPackets.h"
#include "S75CAPackets.h"

typedef struct Status_MSG {

	BYTE ProductID[32];				//Product Identification
	WORD DSPFirmWare;				// Firmware Version Revision.
	BYTE Device_Specific_Data[sizeof(S75CAStatus_MSG)-34];
	
} Status_MSG;


typedef struct CommonStatus_MSG {

	BYTE ProductID[32];			//Product Identification
	WORD DSPFirmWare;			// Firmware Version Revision.
	BYTE DestIP[4];				// Destination IP Address for Unicast Status
    BYTE SubnetMask[4];			// Receiver Subnet Mask
    BYTE DefaultGatewayIP[4];	// Default Gateway Address
	WORD StatusDestUDP;			//Destination UDP Port.	
	BYTE ReceiverMAC[6] ;		// Receiver MAC Address.
	WORD RFStatusValid;			// RF Status Packet from the Micro Controller. 0x0400
	WORD MicroFirmWare;			// Micro Controller firmware

	BYTE Demod_Specific_Data[24];

	WORD EthernetTX;		// Ethernet Transmitted Packets Counter
	WORD EthernetDropped;	// Number of Ethernet Packets Dropped by the Ethernet chip
	WORD EthernetRX;		// Number of Ethernet Packets Received
	WORD EthernetRXErr;		// Count of Ethernet Packets Received In Error.
	WORD EthernetTXErr;		// Count of Ethernet Packets transmission Errors.
	WORD TotalDVBAccepted;	// Total Number of DVB Packets that passed the Filter.
	WORD FEL_Loss;			// Packets received with FEL Low	
	WORD Sync_Loss;			// Packets Received with FEL high and No Sync Byte.	
	BYTE Filter;			// Filter setting and RF config Flag and IGMP	
	BYTE StatusInterval;		

	BYTE Device_Specific_Data[70];
	
} CommonStatus_MSG;



typedef struct CommonSatelliteStatus_MSG {

	BYTE ProductID[32];		//Product Identification
	WORD DSPFirmWare;		// Firmware Version Revision.
	BYTE DestIP[4];	// Destination IP Address for Unicast Status
    BYTE SubnetMask[4];		// Receiver Subnet Mask
    BYTE DefaultGatewayIP[4];	// Default Gateway Address
	WORD StatusDestUDP;		//Destination UDP Port.	
	BYTE ReceiverMAC[6] ;		// Receiver MAC Address.
	WORD RFStatusValid;		 // RF Status Packet from the Micro Controller. 0x0400
	WORD MicroFirmWare;		// Mivro Controller firmware

	WORD SymbolRate;		// Symbol rate in KSPS
	WORD Frequency;			// Frequency in 100 KHz
	BYTE LNBConfiguration;	// LNB configuration
	BYTE ViterbiRate;		// viterbi code rate
	BYTE SignalStrength;	// AGC analog
	BYTE DemodulatorStatus;	// Demodulator Status
	BYTE VBER_Man[3];		// Viterbi BER Mantisa
	BYTE VBER_Exp;			// Viterbi BER Exponent
	BYTE Uncorrectables;	// Uncorrectable errors count since alst status
	BYTE DemodulatorGain;	// the Digital gain
	BYTE NEST;				// Noise estimate 
	BYTE Clock_Off;			// Clock offset
	WORD Freq_Err;			// Frequency Error 
	BYTE ADC_MID;			// MID point crossing for ADC
	BYTE ADC_CLIP;			// ADC Clipping count.
	BYTE Digital_Gain;		// Digital gain
	BYTE AA_CLAMP;			//Clamping count
	WORD RFEND;				// RF message termination .

	WORD EthernetTX;		// Ethernet Transmitted Packets Counter
	WORD EthernetDropped;	// Number of Ethernet Packets Dropped by the Ethernet chip
	WORD EthernetRX;		// Number of Ethernet Packets Received
	WORD EthernetRXErr;		// Count of Ethernet Packets Received In Error.
	WORD EthernetTXErr;		// Count of Ethernet Packets transmission Errors.
	WORD TotalDVBAccepted;	// Total Number of DVB Packets that passed the Filter.
	WORD FEL_Loss;			// Packets received with FEL Low	
	WORD Sync_Loss;			// Packets Received with FEL high and No Sync Byte.	
	BYTE Filter;			// Filter setting and RF config Flag and IGMP	
	BYTE StatusInterval;		

	BYTE Device_Specific_Data[70];
	
} CommonSatelliteStatus_MSG;


typedef struct CommonATSCStatus_MSG {

	BYTE ProductID[32];		//Product Identification
	WORD DSPFirmWare;		// Firmware Version Revision.
	BYTE DestIP[4];
    BYTE SubnetMask[4];		// Receiver Subnet Mask
    BYTE DefaultGatewayIP[4];	// Default Gateway Address
	WORD StatusDestUDP;		//Destination UDP Port.	
	BYTE ReceiverMAC[6] ;		// Receiver MAC Address.
	WORD RFStatusValid;		 // RF Status Packet from the Micro Controller. 0x0400
	WORD MicroFirmWare;		// Mivro Controller firmware

	WORD ChannelNumber;		// Channel Number
	BYTE DeModStats;		// Demodulator status
	BYTE VSBMode;			// VSB Mode
	BYTE SER_1;				// 1 second SER
	BYTE SER_10;			// 10 Second SER
	WORD SNR_AE;			// SNR After equalisation.
	WORD SNR_BE;			// SNR Before equalization
	WORD Carrier_Off;		// Carrier Offset
	WORD IF_AGC;			// IF AGC...
	WORD RFAGC;				// RF AGC
	WORD FEC_UnErr;			// FEC Unerrorred ... !!
	WORD FEC_Corr;			// FEC Corrected 
	WORD FEC_UnCorr;		// FEC uncorrected
	WORD RFEND;				// RF message termination .

	WORD EthernetTX;		// Ethernet Transmitted Packets Counter
	WORD EthernetDropped;	// Number of Ethernet Packets Dropped by the Ethernet chip
	WORD EthernetRX;		// Number of Ethernet Packets Received
	WORD EthernetRXErr;		// Count of Ethernet Packets Received In Error.
	WORD EthernetTXErr;		// Count of Ethernet Packets transmission Errors.
	WORD TotalDVBAccepted;	// Total Number of DVB Packets that passed the Filter.
	WORD FEL_Loss;			// Packets received with FEL Low	
	WORD Sync_Loss;			// Packets Received with FEL high and No Sync Byte.	
	BYTE Filter;			// Filter setting and RF config Flag and IGMP	
	BYTE StatusInterval;		

	BYTE Device_Specific_Data[70];
	
} CommonATSCStatus_MSG;


/*
typedef struct NetConfig_MSG {

	BYTE  DestinationIP[4];		// The forwarding destination address
    BYTE  SubnetMask[4];		// the Subnet Mask to be used by the device
    BYTE  DefaultGatewayIP[4];	// the default gateway to be used by the device
	WORD  DestinationUDP;       // destination port for forwarded data
	BYTE  ReceiverIP[4];		// the IP address to be assumed by the device
	WORD  StatusPort;			// the status port 
	BYTE  Device_Specific_Data[sizeof(S75JLEONetConfig_MSG)-20];

} NetConfig_MSG;
*/

typedef struct CommonNetConfig_MSG {

	BYTE  DestinationIP[4];		// The forwarding destination address
    BYTE  SubnetMask[4];		// the Subnet Mask to be used by the device
    BYTE  DefaultGatewayIP[4];	// the default gateway to be used by the device
	WORD  DestinationUDP;       // destination port for forwarded data
	BYTE  ReceiverIP[4];		// the IP address to be assumed by the device
	WORD  StatusPort;			// the status port 
	BYTE  StatusInterval;       // in increments of 100 ms (ignored by the receiver)
	BYTE  Filter;               // 0 = filter off,
								// 1 = PID Filter on (S75V2 only),
								// 4= IGMP Filter on (all but S75V2)
} CommonNetConfig_MSG;


typedef struct NetSetting {	

			BYTE StatusDestIP[4];
            BYTE SubnetMask[4];
            BYTE DefaultGatewayIP[4];
            WORD DestUDP;
            BYTE ReceiverIP[4];
            WORD StatusPort;

} NetSetting;


typedef struct SatelliteRFConfig_MSG{
	WORD  SymbolRate;              // in ksps
	WORD  Frequency;               // in units of 10 Hz
	BYTE  LNBConfig;               // bit0 - 0 = power off, 1 = power on
                                   // bit1 - 0 = vertical or right polarization
	                               //        1 = horizontal or left polarization
	                               // bit2 - 0 = low band, 1 = high band
								   // bit3 - 1 = high voltage selected
								   // bit4 - 1 = long line compensation on
	BYTE  ViterbiRate;             // 0x08 = auto
} SatelliteRFConfig_MSG;


typedef struct SatelliteRFSetting {

	float frequency;   // MHz
    float symbolRate;  // Msps
    int powerOn;       // 0 = power to LNB is off, 1 = power to LNB is on
    int band;          // 0 = low, 1 = high
    int polarity;      // 0 = vertical or right, 1 = horizontal or left

} SatelliteRFSetting;


typedef struct ATSCRFConfig_MSG{
	WORD  ChannelNumber;              // 2 and up
	WORD  Reserved;               // 0000
	BYTE	BRESERVED;				// 00

} ATSCRFConfig_MSG;


typedef struct ATSC_RFSetting{		// Network Settings Structure for Version 3.X devices.

	WORD ChannelNumber;

} ATSC_RFSetting;


typedef struct PIDEntry {

    BYTE  DestinationIP[4];
    WORD  DestinationUDP;
    WORD  PID;                     // low 13 bits for PID, bit13 - 0 = pass through, 1 = de-encapsulate

} PIDEntry;


typedef struct PIDSettings_MSG {

    WORD      Opcode;               // 1 = download (S75 to host), 2 = upload (host to S75)
    PIDEntry  PIDTableEntry[1000];

} PIDSettings_MSG;


#endif


