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

#include "S75PlusPackets.h"


#ifndef		__S75PLUSPROPACKETS__
#define		__S75PLUSPROPACKETS__


typedef S75PlusNetConfig_MSG S75PlusProNetConfig_MSG;

typedef S75PlusRFConfig_MSG S75PlusProRFConfig_MSG;

typedef S75PlusPIDSettings_MSG S75PlusProPIDSettings_MSG;


typedef struct PATEntry {
	WORD ProgramNumber;
	WORD PMTPID;
} PATEntry;


typedef struct S75PlusProPAT_MSG {

	WORD Opcode;
	PATEntry PATTableEntry[NUMBER_OF_DESTINATIONS];
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1

} S75PlusProPAT_MSG;


typedef S75Plus_One_Time_Status_Request_MSG S75PlusPro_One_Time_Status_Request_MSG;

typedef S75Plus_Reset_MSG S75PlusPro_Reset_MSG;

typedef S75PlusStatus_MSG S75PlusProStatus_MSG;


typedef struct S75PlusPro_Event_Trap_MSG {
    WORD		Opcode;               // 1 = download (S75 to host), 2 = upload (host to S75)
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
} S75PlusPro_Event_Trap_MSG;


typedef S75Plus_Password_Change_MSG S75PlusPro_Password_Change_MSG;


#endif
	
