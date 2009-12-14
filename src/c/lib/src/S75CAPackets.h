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
#include "S75PlusProPackets.h"


#ifndef		__S75CAPACKETS__
#define		__S75CAPACKETS__


typedef S75PlusNetConfig_MSG S75CANetConfig_MSG;

typedef S75PlusRFConfig_MSG S75CARFConfig_MSG;

typedef S75PlusPIDSettings_MSG S75CAPIDSettings_MSG;

typedef S75PlusProPAT_MSG S75CAPAT_MSG;


typedef struct S75CAProgramSettings_MSG {

    WORD  Opcode;               // 1 = download (S75 to host), 2 = upload (host to S75)
    WORD  Program[NUMBER_OF_PROGRAMS];
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1

} S75CAProgramSetting_MSG;


typedef struct S75CAProgramResponse_MSG {

    WORD  Opcode;               // 1 = download (S75 to host), 2 = upload (host to S75)
    WORD  Program[NUMBER_OF_PROGRAMS];
	WORD  Status[NUMBER_OF_PROGRAMS];

} S75CAProgramResponse_MSG;



typedef S75Plus_One_Time_Status_Request_MSG S75CA_One_Time_Status_Request_MSG;

typedef S75Plus_Reset_MSG S75CA_Reset_MSG;

typedef S75PlusStatus_MSG S75CAStatus_MSG;


typedef S75PlusPro_Event_Trap_MSG S75CA_Event_Trap_MSG;

typedef S75Plus_Password_Change_MSG S75CA_Password_Change_MSG;

typedef struct S75CA_CAM_Watchdog_Timeout_MSG {
    WORD		Opcode;               // 1 = download (S75 to host), 2 = upload (host to S75)
	WORD		Timeout;
	WORD		Random;	
	BYTE		Signature[20];				// HMAC-SHA1
} S75CA_CAM_Watchdog_Timeout_MSG;


#endif
	
