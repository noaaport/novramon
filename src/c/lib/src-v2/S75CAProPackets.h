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
#include "S75CAPackets.h"


#ifndef		__S75CAPROPACKETS__
#define		__S75CAPROPACKETS__


typedef S75PlusNetConfig_MSG S75CAProNetConfig_MSG;

typedef S75PlusRFConfig_MSG S75CAProRFConfig_MSG;

typedef S75PlusPIDSettings_MSG S75CAProPIDSettings_MSG;

typedef S75PlusProPAT_MSG S75CAProPAT_MSG;

typedef S75CAProgramSettings_MSG S75CAProProgramSettings_MSG;

typedef S75CAProgramResponse_MSG S75CAProProgramResponse_MSG;

typedef S75Plus_One_Time_Status_Request_MSG S75CAPro_One_Time_Status_Request_MSG;

typedef S75Plus_Reset_MSG S75CAPro_Reset_MSG;

typedef S75PlusStatus_MSG S75CAProStatus_MSG;

typedef S75PlusPro_Event_Trap_MSG S75CAPro_Event_Trap_MSG;

typedef S75Plus_Password_Change_MSG S75CAPro_Password_Change_MSG;


#endif
	
