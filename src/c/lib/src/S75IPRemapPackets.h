//************************************************************************
//*
//* Copyright Novra Technologies Incorporated, 2008
//* 900-330 St. Mary Avenue
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


#ifndef		__S75IPREMAPPACKETS__
#define		__S75IPREMAPPACKETS__

#define NUM_REMAP_RULES 16

typedef S75PlusNetConfig_MSG S75IPRemapNetConfig_MSG;

typedef S75PlusRFConfig_MSG S75IPRemapRFConfig_MSG;

typedef S75PlusPIDSettings_MSG S75IPRemapPIDSettings_MSG;

typedef S75Plus_One_Time_Status_Request_MSG S75IPRemap_One_Time_Status_Request_MSG;

typedef S75Plus_Reset_MSG S75IPRemap_Reset_MSG;

typedef S75PlusStatus_MSG S75IPRemapStatus_MSG;

typedef S75Plus_Password_Change_MSG S75IPRemap_Password_Change_MSG;

typedef struct RemapRule {

	unsigned short action;
	unsigned short ttl;
	BYTE originalIP[4];
	BYTE newIP[4];
	BYTE mask[4];

} RemapRule;

typedef struct S75IPRemap_IPRemap_MSG {

    WORD			Opcode;               // 1 = download (S75 to host), 2 = upload (host to S75)
	WORD            enable;
	RemapRule		remapping_rule[NUM_REMAP_RULES];
	WORD			Random;				// 
	BYTE			Signature[20];		// HMAC-SHA1

} S75IPRemap_IPRemap_MSG;

#endif
	
