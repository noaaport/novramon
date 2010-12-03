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
// This File defines the Packets used to communicate with the S75 version 2.1
// 

#ifndef		__S75V2PACKETS__
#define		__S75V2PACKETS__

#include "NovraDevicePackets.h"


//typedef struct S75V2NetConfig_MSG{
//
//	BYTE  DestinationIP[4];		// The forwarding destination address
//    BYTE  SubnetMask[4];		// the Subnet Mask to be used by the device
//    BYTE  DefaultGatewayIP[4];	// the default gateway to be used by the device
//	WORD  DestinationUDP;       // destination port for forwarded data
//	BYTE  ReceiverIP[4];		// the IP address to be assumed by the device
//	WORD  StatusPort;			// the status port 
//	BYTE  StatusInterval;       // in increments of 100 ms (ignored by the receiver)
//	BYTE  Filter;               // 0 = filter off, 1 = filter on  PID filtering
//}S75V2NetConfig_MSG;

typedef struct S75V2RFConfig_MSG{
	WORD  SymbolRate;              // in ksps
	WORD  Frequency;               // in units of 10 Hz
	BYTE  LNBConfig;               // bit0 - 0 = power off, 1 = power on
                                   // bit1 - 0 = vertical or right polarization
	                               //        1 = horizontal or left polarization
	                               // bit2 - 0 = low band, 1 = high band
	BYTE  ViterbiRate;             // 0x08 = auto
} S75V2RFConfig_MSG;

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

typedef struct S75V2_PIDSettings_MSG {

    WORD      Opcode;               // 1 = download (S75 to host), 2 = upload (host to S75)
    PIDEntry  PIDTableEntry[32];

} S75V2_PIDSetting_MSG;

typedef struct S75D_V2_Status_MSG {

	BYTE ProductID[32];		//Product Identification
	WORD DSPFirmWare;		// Firmware Version Revision.
	BYTE DestinationIP[4];	// Destination IP Address
    BYTE SubnetMask[4];		// Receiver Subnet Mask
    BYTE DefaultGatewayIP[4];	// Default Gateway Address
	WORD DestinationUDP;		//Destination UDP Port.	
	BYTE ReceiverMAC[6] ;		// Receiver MAC Address.
	WORD RFStatusValid;		 // RF Status Packet from the Micro Controller. 0x0400
	WORD MicroFirmWare;		// Mivro Controller firmware
	WORD SymbolRate;		// Symbol rate in KSPS
	WORD Frequency;			// Frequency in 100 KHz
	BYTE LNBConfiguration;	// LNB configuration
	BYTE ViterbiRate;		// viterbi code rate
	BYTE SignalStrength;	// AGC
	BYTE DemodulatorStatus;	// Demodulator Status
	BYTE VBER_Man[3];		// Viterbi BER Mantisa
	BYTE VBER_Exp;			// Viterbi BER Exponent
	BYTE Uncorrectables;	// Uncorrectable errors count since alst status
	BYTE DemodulatorGain;	// the Digital gain
	BYTE Reserved[8];		// reserved values for internal Use
	WORD RFEND;				// RF message termination .
	WORD EthernetTX;		// Ethernet Transmitted Packets Counter
	WORD EthernetDropped;	// Number of Ethernet Packets Dropped by the Ethernet chip
	WORD EthernetRX;		// Number of Ethernet Packets Received
	WORD EthernetRXErr;		// Count of Ethernet Packets Received In Error.
	WORD EthernetTXErr;		// Count of Ethernet Packets transmission Errors.
	WORD TotalDVBAccepted;	// Total Number of DVB Packets that passed the Filter.
	WORD FEL_Loss;			// Packets received with FEL Low	
	WORD Sync_Loss;			// Packets Received with FEL high and No Sync Byte.	
	BYTE Filter;			// Filter setting and RF config Flag.	
	BYTE StatusInterval;		
	WORD PIDList[32];		// List of PIDS to pass through the filter.
	
} S75D_V2_Status_MSG;


#endif
	
