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


#ifndef		__S75FKPACKETS__
#define		__S75FKPACKETS__

#define		NUMBER_OF_FIXED_KEYS	64


typedef S75PlusNetConfig_MSG S75FKNetConfig_MSG;

typedef S75PlusRFConfig_MSG S75FKRFConfig_MSG;

typedef S75PlusPIDSettings_MSG S75FKPIDSettings_MSG;


typedef struct FKEntry {
	WORD PID;
	BYTE key[8];
} FKEntry;


typedef struct S75FKeys_MSG {
    WORD	Opcode;               // 1 = download (S75 to host), 2 = upload (host to S75)
	FKEntry	FK[NUMBER_OF_FIXED_KEYS];
	WORD	Random;				// 
	BYTE	Signature[20];		// HMAC-SHA1
} S75FKeys_MSG;


typedef S75Plus_One_Time_Status_Request_MSG S75FK_One_Time_Status_Request_MSG;

typedef S75Plus_Reset_MSG S75FK_Reset_MSG;

typedef S75PlusStatus_MSG S75FKStatus_MSG;

typedef S75Plus_Password_Change_MSG S75FK_Password_Change_MSG;


#endif
	
