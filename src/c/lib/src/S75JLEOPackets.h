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
// This File defines the Packets used to communicate with the S75 JSAT Leo Palace
// 

#ifndef		__S75JLEOPACKETS__
#define		__S75JLEOPACKETS__

#ifdef LINUX
	#include "linux_windows_types.h"
#endif

typedef struct S75JLEONetConfig_MSG {

	BYTE  ReceiverIP[4];		// the IP address to be assumed by the device
    BYTE  SubnetMask[4];		// the Subnet Mask to be used by the device
    BYTE  DefaultGatewayIP[4];	// the default gateway to be used by the device
	WORD  DestinationUDP;       // destination port for forwarded data
	BYTE  DestinationIP[4];		// The forwarding destination address
	WORD  StatusPort;			// the status port 
	WORD  IGMP;               // 0 = filter off, 1= IGMP Filter on
	BYTE  Reserved[4];			// unused
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1

} S75JLEONetConfig_MSG;


typedef struct S75JLEORFConfig_MSG {

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

} S75JLEORFConfig_MSG;



typedef struct S75JLEO_PIDRequest_MSG {
	WORD	Opcode;	// 1 = download (s75 to host)
	WORD	Random;
	BYTE	Signature[20];
} S75JLEO_PIDRequest_MSG;


typedef struct S75JLEO_PIDResponse_MSG {
	WORD  Opcode;	// 1 = download (s75 to host)
  	WORD  PIDTableEntry[16];
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1
} S75JLEO_PIDResponse_MSG;

typedef struct S75JLEO_PIDSettings_MSG {
    WORD  Opcode;               // 1 = download (S75 to host), 2 = upload (host to S75)
    WORD  PIDTableEntry[16];
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1
} S75JLEO_PIDSetting_MSG;

typedef struct S75JLEO_Status_MSG {

	BYTE ProductID[32];		//Product Identification
	WORD DSPFirmWare;		// Firmware Version Revision.
	BYTE ReceiverMAC[6] ;		// Receiver MAC Address.
	BYTE ReceiverIP[4];
    BYTE SubnetMask[4];		// Receiver Subnet Mask
    BYTE DefaultGatewayIP[4];	// Default Gateway Address
	BYTE UniCastStatusDestIP[4];	// Destination IP Address for Unicast Status
	WORD UniCastStatusDestUDP;		//Destination UDP Port.	
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
	WORD DVBAccepted;	// Total Number of DVB Packets that passed the Filter.
	WORD FEL_Loss;			// Packets received with FEL Low	
	WORD Sync_Loss;			// Packets Received with FEL high and No Sync Byte.	
	WORD Filter;			// Filter setting and RF config Flag and IGMP	
	WORD PIDList[16];		// List of PIDS to pass through the filter. 
	BYTE DVBMAC[6];			// the DVB MAC address of the receiver..
	unsigned long TotalDVBPacketsAccepted[2];
	unsigned long TotalDVBPacketsRXInError[2];
	unsigned long TotalEthernetPacketsOut[2];
	unsigned long TotalUncorrectableTSPackets[2];
	WORD BroadcastStatusPort;
	WORD LocalOscFrequency;
	
} S75JLEO_Status_MSG;

typedef struct S75JLEO_Reset_MSG {
	BYTE ResetKey[4];		// Reset Key (0x19751403)
	WORD Random;
	BYTE Signature[20];		// HMAC-SHA1
} S75JLEO_Reset_MSG;

typedef struct S75JLEO_Set_MAC_MSG {
	BYTE MAC[6];			// MAC Address
	WORD Random;
	BYTE Signature[20];		// HMAC-SHA1
} S75JLEO_Set_MAC_MSG;

typedef struct S75JLEO_Event_Trap_Set_MSG {
	WORD		MonitoringInterval;			// Monitoring Interval in seconds
	BYTE		NotificationIPAddress[4];		// IP where to send trap
	WORD		NotificationPort;			// Port where to send trap
	WORD		DataLockMonitorOn;			// Flag 1 = monitor data lock, 0 = don't monitor DL
	WORD		VBERThreshold;				// Viterbi Bit Error Rate Threshold
	BYTE 		UncorrectableTSErrorThreshold[4];	
	WORD		EthernetTxErrorThreshold;
	WORD		Mask;		
	WORD		Random;	
	BYTE		Signature[20];				// HMAC-SHA1
} S75JLEO_Event_Trap_Set_MSG;

typedef struct S75JLEO_Password_Change_MSG {
	BYTE Password[16];		// Encrypted Password	
	BYTE Signature[20];		// HMAC-SHA1
} S75JLEO_Password_Change_MSG;

typedef struct S75JLEO_One_Time_Status_Request_MSG {
	BYTE Message[6];	 
} S75JLEO_One_Time_Status_Request_MSG;

#endif

