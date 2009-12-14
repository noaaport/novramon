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
// This File defines the Packets used to communicate with the S75PLUS devices
// 

#ifndef		__S75PLUSPACKETS__
#define		__S75PLUSPACKETS__
#define		NUMBER_OF_PIDS			32
#define		NUMBER_OF_DESTINATIONS	16
#define		NUMBER_OF_PROGRAMS		16


typedef struct S75PlusNetConfig_MSG {

	BYTE  ReceiverIP[4];		// the IP address to be assumed by the device
    BYTE  SubnetMask[4];		// the Subnet Mask to be used by the device
    BYTE  DefaultGatewayIP[4];	// the default gateway to be used by the device
	BYTE  DestinationIP[4];		// The forwarding destination address
	WORD  DestinationUDP;       // destination port for forwarded data
	WORD  StatusPort;			// the status port 
	WORD  IGMP;                 // 0 = filter off, 1= IGMP Filter on
	WORD  Reserved; 
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1

} S75PlusNetConfig_MSG;



typedef struct S75PlusRFConfig_MSG{
	WORD  SymbolRate;              // in ksps
	WORD  Frequency;               // in units of 10 Hz
	BYTE  LNBConfig;               // bit0 - 0 = power off, 1 = power on
                                   // bit1 - 0 = vertical or right polarization
	                               //        1 = horizontal or left polarization
	                               // bit2 - 0 = low band, 1 = high band
	BYTE  ViterbiRate;             // 0x08 = auto
	WORD  LocalOscFrequency;
	WORD  Random;				   // 
	BYTE  Signature[20];		   // HMAC-SHA1
} S75PlusRFConfig_MSG;


#ifndef __PIDEntry__
#define __PIDEntry__
//typedef struct PIDEntry {
//
//    BYTE  DestinationIP[4];
//    WORD  DestinationUDP;
//    WORD  PID;                     // low 13 bits for PID, bit13 - 0 = pass through, 1 = de-encapsulate
//
//} PIDEntry;
#endif


#ifndef PID_ROUTE
	#define PID_ROUTE

	typedef struct PIDRoute {
		WORD PID;
		WORD DestinationMask;
	} PIDRoute;


	typedef struct PIDDestination {
		BYTE DestinationIP[4];
		WORD DestinationUDP;
	} PIDDestination;

#endif

typedef struct S75PlusPIDSettings_MSG {

    WORD					Opcode;               // 1 = download (S75 to host), 2 = upload (host to S75)
    PIDRoute				PIDTableEntry[NUMBER_OF_PIDS];
	PIDDestination			PIDDestinations[NUMBER_OF_DESTINATIONS];
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1

} S75PlusPIDSetting_MSG;


typedef struct S75Plus_One_Time_Status_Request_MSG {
	BYTE Message[6];	 
} S75Plus_One_Time_Status_Request_MSG;


typedef struct S75Plus_Reset_MSG {
	BYTE ResetKey[4];		// Reset Key (0x19751403)
	WORD Random;
	BYTE Signature[20];		// HMAC-SHA1
} S75Plus_Reset_MSG;


typedef struct S75PlusStatus_MSG {

	BYTE ProductID[32];		//Product Identification
	WORD DSPFirmWare;		// Firmware Version Revision.
	BYTE ReceiverMAC[6] ;		// Receiver MAC Address.
	BYTE ReceiverIP[4];
    BYTE SubnetMask[4];		// Receiver Subnet Mask
    BYTE DefaultGatewayIP[4];	// Default Gateway Address
	BYTE UniCastStatusDestIP[4];	// Destination IP Address for Unicast Status
	WORD UniCastStatusDestUDP;		//Destination UDP Port.	
//	WORD RFStatusValid;		 // RF Status Packet from the Micro Controller. 0x0400
	WORD MicroFirmWare;		// Mivro Controller firmware
	WORD SymbolRate;		// Symbol rate in KSPS
	WORD Frequency;			// Frequency in 100 KHz
	BYTE LNBConfiguration;	// LNB configuration
	BYTE ViterbiRate;		// viterbi code rate
	
	BYTE DemodulatorStatus;	// Demodulator Status
	BYTE SignalStrength;	// AGC
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
	WORD DVBScrambled;		// DVB packets forwarded in a scrambled state
	WORD DVBAccepted;		// Total Number of DVB Packets that passed the Filter.
	WORD DVBDescrambled;	// DVB packets descrambled or not scrambled
	WORD Sync_Loss;			// Packets Received with FEL high and No Sync Byte.	
	BYTE Filter;			// Filter setting and RF config Flag and IGMP	
	BYTE CardStatus;		
	WORD TotalDVBPacketsAccepted[4];
	WORD TotalDVBPacketsRXInError[4];
	WORD TotalEthernetPacketsOut[4];
	WORD TotalUncorrectableTSPackets[4];
	WORD BroadcastStatusPort;
	WORD LocalOscFrequency;
    PIDRoute PIDList[NUMBER_OF_PIDS];
	PIDDestination PIDDestinations[NUMBER_OF_DESTINATIONS];
	WORD ProgramList[NUMBER_OF_PROGRAMS];
	WORD ProgramStatus[NUMBER_OF_PROGRAMS];
	
} S75PlusStatus_MSG;


typedef struct S75Plus_Password_Change_MSG {
	BYTE Password[16];		// Encrypted Password	
	WORD Random;	
	BYTE Signature[20];		// HMAC-SHA1
} S75Plus_Password_Change_MSG;


#endif
	
