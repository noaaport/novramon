//#include "stdafx.h"

#include "S75V3.h"


typedef unsigned char BYTE;
typedef unsigned short WORD;

typedef struct StatusMSG {

	WORD Frequency;			// Frequency in 100 KHz

} StatusMSG;


S75V3::S75V3()
{
	initAttributes();
}


S75V3::S75V3( unsigned char *buffer )
{
	initAttributes();
	setFixedParameters( buffer );
	createIndexedParameters();
}


S75V3::~S75V3()
{
}


void S75V3::initAttributes()
{

	memset(	parameterMask, 0, MAX_RECEIVER_PARAM * sizeof( unsigned char ) );

	// FIXED READ ONLY VALUES
	//---------------------------------------
	
	// DEVICE INFO
	
	parameterMask[ DEVICE_TYPE			] = F_EXISTS | F_ID | F_DEF | F_READ | F_SHORT | F_ENUM;
	parameterMask[ DSP_VERSION			] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ DSP_REVISION			] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ RF_FIRMWARE_VERSION	] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ RECEIVER_MAC			] = F_EXISTS | F_ID | F_READ | F_STRING;
	parameterMask[ SIZE_PID_LIST		] = F_EXISTS | F_DEF | F_READ | F_SHORT;

	// Read/Write Network Parameters

	parameterMask[ RECEIVER_IP           ] = F_EXISTS | F_ID | F_CONFIG | F_READ | F_WRITE | F_STRING;
	parameterMask[ SUBNET_MASK           ] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_STRING;
	parameterMask[ DEFAULT_GATEWAY_IP    ] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_STRING;
	parameterMask[ BROADCAST_STATUS_PORT ] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT;

	Receiver::initAttributes();

}


//string S75V3::macFromStatus( unsigned char *buffer )
//{
//	char mac_str[20];
//
//	sprintf( mac_str, "%02x-%02x-%02x-%02x-%02x-%02x", 
//			 buffer[48], buffer[49], buffer[50], buffer[51], buffer[52], buffer[53] );
//
//	return string( mac_str );
//}


bool S75V3::checkStatusPacket( string ip, unsigned char *buffer )
{
	bool isForThisReceiver = true;

	try {
		if ( ip != getParameter( RECEIVER_IP ).asString() ) throw;
		if ( buffer[32] != ID_S75V3 ) throw;
		if ( fixedParamFromStatus( RECEIVER_MAC, buffer ) != getParameter( RECEIVER_MAC ).asString() ) throw;
	}

	catch ( int ) {
		isForThisReceiver = false;
	}

	return isForThisReceiver;
}


bool S75V3::connected( int timeout )
{
	unsigned char junk[1500];

	return getPIDList( junk );
}


bool S75V3::setFixedParameters( unsigned char *buffer )
{
	char temp[20];

	if ( buffer[32] == ID_S75V3 ) {

		currentParameters[DEVICE_TYPE]->setM_value( string( itoa( R_S75_V3, temp, 10 ) ) );

		currentParameters[RECEIVER_MAC]->setM_value( fixedParamFromStatus( RECEIVER_MAC, buffer ) );

		currentParameters[DSP_VERSION]->setM_value( fixedParamFromStatus( DSP_VERSION, buffer ) );

		currentParameters[DSP_REVISION]->setM_value( fixedParamFromStatus( DSP_REVISION, buffer ) );

		currentParameters[RF_FIRMWARE_VERSION]->setM_value( fixedParamFromStatus( RF_FIRMWARE_VERSION, buffer ) );

		currentParameters[SIZE_PID_LIST]->setM_value( string( itoa( NUM_PIDS_S75V3 ,temp, 10 ) ) );

		return true;

	} else {

		return false;

	}

}


string S75V3::fixedParamFromStatus( ReceiverParameter p, unsigned char *buffer )
{
	char temp[100];
	string return_str = "";

	switch( p ) {

	case RECEIVER_MAC			:	sprintf(	temp, "%02x-%02x-%02x-%02x-%02x-%02x", 
												buffer[48], buffer[49], buffer[40],
												buffer[51], buffer[52], buffer[53] );
									return_str = string( temp );
									break;

	case DSP_VERSION			:
									break;

	case DSP_REVISION			:
									break;

	case RF_FIRMWARE_VERSION	:
									break;

	default						:	return_str = "";
									break;

	}

	return return_str;
}


string S75V3::dynamicParamFromStatus( ReceiverParameter p, unsigned char *buffer, int index )
{

	string return_str = "";
	char temp[100];
	StatusMSG *status = (StatusMSG *)buffer;


	switch( p ) {

	case CARRIER_FREQUENCY				:	sprintf( temp, "%f", (double)(status->Frequency) );
											return_str = string( temp );
											break;

	default								:	return_str = "";
											break;

	}

	return return_str;
}

/* OLDLIB
							// Use S75D_V3 status to fill in a S75Plus status structure
							DeviceStatus->FirmWareVer = V3DeviceStatus.FirmWareVer;
							DeviceStatus->FirmWareRev = V3DeviceStatus.FirmWareRev;
							DeviceStatus->RF_FIRMWARE = V3DeviceStatus.RF_FIRMWARE;
							DeviceStatus->IGMP_Enable = V3DeviceStatus.IGMP_Enable;
							memcpy( DeviceStatus->ReceiverIP, V3DeviceStatus.ReceiverIP, 4 );
							memcpy( DeviceStatus->UniCastStatusDestIP, V3DeviceStatus.UniCastStatusDestIP, 4 );
							memcpy( DeviceStatus->SubnetMask, V3DeviceStatus.SubnetMask, 4 );
							memcpy( DeviceStatus->DefaultGatewayIP, V3DeviceStatus.DefaultGatewayIP, 4 );
							DeviceStatus->UniCastStatusDestUDP = V3DeviceStatus.UniCastStatusDestUDP;
							memcpy( DeviceStatus->ReceiverMAC, V3DeviceStatus.ReceiverMAC, 6 );
							DeviceStatus->RFStatusValid = V3DeviceStatus.RFStatusValid;
							DeviceStatus->SymbolRate = V3DeviceStatus.SymbolRate;
							DeviceStatus->Frequency = V3DeviceStatus.Frequency;
							DeviceStatus->Polarity = V3DeviceStatus.Polarity;
							DeviceStatus->Band = V3DeviceStatus.Band;
							DeviceStatus->Power = V3DeviceStatus.Power;
							DeviceStatus->ViterbiRate = V3DeviceStatus.ViterbiRate;
							DeviceStatus->Long_Line_Comp = V3DeviceStatus.Long_Line_Comp;
							DeviceStatus->Hi_VoltageMode = V3DeviceStatus.Hi_VoltageMode;
							DeviceStatus->PolaritySwitchingVoltage = 0;
							DeviceStatus->HiLoBandSwitchingTone = 0;
							DeviceStatus->SignalStrength = V3DeviceStatus.SignalStrength;
							DeviceStatus->SignalLock = V3DeviceStatus.SignalLock;
							DeviceStatus->DataLock = V3DeviceStatus.DataLock;
							DeviceStatus->LNBFault = V3DeviceStatus.LNBFault;
							DeviceStatus->VBER = V3DeviceStatus.VBER;
							DeviceStatus->AGCA = 0;
							DeviceStatus->AGCN = 0;
							DeviceStatus->GNYQA = 0;
							DeviceStatus->GFARA = 0;
							DeviceStatus->NEST = 0;
							DeviceStatus->Clock_Off = 0;
							DeviceStatus->Freq_Err = 0;
							DeviceStatus->ADC_MID = 0;
							DeviceStatus->ADC_CLIP = 0;
							DeviceStatus->Digital_Gain = 0;
							DeviceStatus->AA_CLAMP = 0;
							DeviceStatus->CarrierToNoise = 0;
							DeviceStatus->Uncorrectables = V3DeviceStatus.Uncorrectables;
							DeviceStatus->DemodulatorGain = 0;
							DeviceStatus->CardStatus = 0;
							memcpy( DeviceStatus->UID,	V3DeviceStatus.UID, 32 );
							DeviceStatus->EthernetTransmit = V3DeviceStatus.EthernetTransmit;
							DeviceStatus->EthernetReceive = V3DeviceStatus.EthernetReceive;
							DeviceStatus->DVBScrambled = V3DeviceStatus.EthernetTransmitError;
							DeviceStatus->EthernetPacketDropped = V3DeviceStatus.EthernetPacketDropped;
							DeviceStatus->EthernetReceiveError = 0;
							DeviceStatus->DVBDescrambled = 0;
							DeviceStatus->DataSyncLoss = 0;
							DeviceStatus->DVBAccepted = 0;							
							DeviceStatus->TotalDVBPacketsAccepted[0] = 0;
							DeviceStatus->TotalDVBPacketsAccepted[1] = 0;
							DeviceStatus->TotalDVBPacketsRXInError[0] = 0;
							DeviceStatus->TotalDVBPacketsRXInError[1] = 0;
							DeviceStatus->TotalEthernetPacketsOut[0] = 0;
							DeviceStatus->TotalEthernetPacketsOut[1] = 0;
							DeviceStatus->TotalUncorrectableTSPackets[0] = 0;
							DeviceStatus->TotalUncorrectableTSPackets[1] = 0;
							DeviceStatus->BroadcastStatusPort = 0;
							DeviceStatus->LocalOscFrequency = 0;
							memcpy( (void *)&(DeviceStatus->PIDList), (void *)&(V3DeviceStatus.PIDList), sizeof( S75D_PIDList ) );
							memset( DeviceStatus->PIDRoutes, 0, sizeof( PIDRoute ) * 32 );
							memset(	DeviceStatus->PIDDestinations, 0, sizeof( PIDDestination ) * NUM_S75PRO_DESTINATIONS );
							memset(	DeviceStatus->ProgramList, 0, 32 );
							memset(	DeviceStatus->ProgramStatus, 0, 32 );
*/

