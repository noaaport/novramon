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
// This File defines the Packets used to communicate with the A75 version 1.X
// 

#ifndef		__A75PACKETS__
#define		__A75PACKETS__

#include "NovraDevicePackets.h"

//typedef struct A75NetConfig_MSG{

//	BYTE  DestinationIP[4];		// The forwarding destination address
//    BYTE  SubnetMask[4];		// the Subnet Mask to be used by the device
//    BYTE  DefaultGatewayIP[4];	// the default gateway to be used by the device
//	WORD  DestinationUDP;       // destination port for forwarded data
//	BYTE  ReceiverIP[4];		// the IP address to be assumed by the device
//	WORD  StatusPort;			// the status port 
//	BYTE  StatusInterval;       // in increments of 100 ms (ignored by the receiver)
//	BYTE  Filter;               // 0 = filter off, 4= IGMP Filter on
//}A75NetConfig_MSG;

typedef struct A75RFConfig_MSG{
	WORD  ChannelNumber;              // 2 and up
	WORD  Reserved;               // 0000
	BYTE	BRESERVED;				// 00

} A75RFConfig_MSG;



typedef struct A75PIDSettings_MSG {

    WORD      Opcode;               // 1 = download (S75 to host), 2 = upload (host to S75)
    PIDEntry  PIDTableEntry[16];

} A75PIDSettings_MSG;

typedef struct A75Status_MSG {

	BYTE ProductID[32];		//Product Identification
	WORD DSPFirmWare;		// Firmware Version Revision.
	BYTE UniCastStatusDestIP[4];	// Destination IP Address for Unicast Status
    BYTE SubnetMask[4];		// Receiver Subnet Mask
    BYTE DefaultGatewayIP[4];	// Default Gateway Address
	WORD UniCastStatusDestUDP;		//Destination UDP Port.	
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
	WORD PIDList[16];		// List of PIDS to pass through the filter. 
	BYTE DVBMAC[6];			// the DVB MAC address of the receiver..
	
} A75Status_MSG;


#endif
	
