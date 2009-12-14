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
#include "S75FKPackets.h"


#ifndef		__S75FKPROPACKETS__
#define		__S75FKPROPACKETS__


typedef S75PlusNetConfig_MSG S75FKProNetConfig_MSG;

typedef S75PlusRFConfig_MSG S75FKProRFConfig_MSG;

typedef S75PlusPIDSettings_MSG S75FKProPIDSettings_MSG;

typedef S75PlusProPAT_MSG S75FKProPAT_MSG;

typedef S75Plus_One_Time_Status_Request_MSG S75FKPro_One_Time_Status_Request_MSG;

typedef S75Plus_Reset_MSG S75FKPro_Reset_MSG;

typedef S75PlusStatus_MSG S75FKProStatus_MSG;

typedef S75PlusPro_Event_Trap_MSG S75FKPro_Event_Trap_MSG;

typedef S75Plus_Password_Change_MSG S75FKPro_Password_Change_MSG;


#endif
	
