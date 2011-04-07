//#include "stdafx.h"

#include "S75V.h"


typedef unsigned char BYTE;
typedef unsigned short WORD;

typedef struct S75VStatusMSG {

	WORD Frequency;			// Frequency in 100 KHz

} S75VStatusMSG;


S75V::S75V()
{
	initAttributes();
}


S75V::S75V( unsigned char *buffer )
{
	initAttributes();
	setFixedParameters( buffer );
	createIndexedParameters();
}


S75V::~S75V()
{
}


void S75V::initAttributes()
{

	memset(	parameterMask, 0, MAX_RECEIVER_PARAM * sizeof( unsigned short ) );

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


//string S75V::macFromStatus( unsigned char *buffer )
//{
//	char mac_str[20];
//
//	sprintf( mac_str, "%02x-%02x-%02x-%02x-%02x-%02x", 
//			 buffer[48], buffer[49], buffer[50], buffer[51], buffer[52], buffer[53] );
//
//	return string( mac_str );
//}


bool S75V::checkStatusPacket( string ip, unsigned char *buffer )
{
	bool isForThisReceiver = true;


	try {
		if ( ip != getParameter( RECEIVER_IP ).asString() ) throw;
		if ( buffer[32] != ID_S75V ) throw;
		if ( fixedParamFromStatus( RECEIVER_MAC, buffer ) != getParameter( RECEIVER_MAC ).asString() ) throw;
	}

	catch ( int ) {
		isForThisReceiver = false;
	}

	return isForThisReceiver;
}


bool S75V::connected( int timeout )
{
	unsigned char junk[1500];

	return getPIDList( junk );
}


bool S75V::setFixedParameters( unsigned char *buffer )
{
	char temp[20];

	if ( buffer[32] == ID_S75V ) {

		currentParameters[DEVICE_TYPE]->setM_value( string( itoa( R_S75V, temp, 10 ) ) );

		currentParameters[RECEIVER_MAC]->setM_value( fixedParamFromStatus( RECEIVER_MAC, buffer ) );

		currentParameters[DSP_VERSION]->setM_value( fixedParamFromStatus( DSP_VERSION, buffer ) );

		currentParameters[DSP_REVISION]->setM_value( fixedParamFromStatus( DSP_REVISION, buffer ) );

		currentParameters[RF_FIRMWARE_VERSION]->setM_value( fixedParamFromStatus( RF_FIRMWARE_VERSION, buffer ) );

		currentParameters[SIZE_PID_LIST]->setM_value( string( itoa( NUM_PIDS_S75V ,temp, 10 ) ) );

		return true;

	} else {

		return false;

	}

}


string S75V::fixedParamFromStatus( ReceiverParameter p, unsigned char *buffer )
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


string S75V::dynamicParamFromStatus( ReceiverParameter p, unsigned char *buffer, int index )
{
	string return_str = "";
	char temp[100];
	S75VStatusMSG *status = (S75VStatusMSG *)buffer;

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
								// Use S75V status to fill in a S75Plus status structure
								DeviceStatus->FirmWareVer = PMDeviceStatus.FirmWareVer;
								DeviceStatus->FirmWareRev = PMDeviceStatus.FirmWareRev;
								DeviceStatus->RF_FIRMWARE = PMDeviceStatus.RF_FIRMWARE;
								DeviceStatus->IGMP_Enable = PMDeviceStatus.IGMP_Enable;
								memcpy( DeviceStatus->ReceiverIP, PMDeviceStatus.ReceiverIP, 4 );
								memcpy( DeviceStatus->UniCastStatusDestIP, PMDeviceStatus.UniCastStatusDestIP, 4 );
								memcpy( DeviceStatus->SubnetMask, PMDeviceStatus.SubnetMask, 4 );
								memcpy( DeviceStatus->DefaultGatewayIP, PMDeviceStatus.DefaultGatewayIP, 4 );
								DeviceStatus->UniCastStatusDestUDP = PMDeviceStatus.UniCastStatusDestUDP;
								memcpy( DeviceStatus->ReceiverMAC, PMDeviceStatus.ReceiverMAC, 6 );
								DeviceStatus->RFStatusValid = PMDeviceStatus.RFStatusValid;
								DeviceStatus->SymbolRate = PMDeviceStatus.SymbolRate;
								DeviceStatus->Frequency = PMDeviceStatus.Frequency;
								DeviceStatus->Polarity = PMDeviceStatus.Polarity;
								DeviceStatus->Band = PMDeviceStatus.Band;
								DeviceStatus->Power = PMDeviceStatus.Power;
								DeviceStatus->ViterbiRate = PMDeviceStatus.ViterbiRate;
								DeviceStatus->Long_Line_Comp = PMDeviceStatus.Long_Line_Comp;
								DeviceStatus->Hi_VoltageMode = PMDeviceStatus.Hi_VoltageMode;
								DeviceStatus->PolaritySwitchingVoltage = 0;
								DeviceStatus->HiLoBandSwitchingTone = 0;
								DeviceStatus->SignalStrength = PMDeviceStatus.SignalStrength;
								DeviceStatus->SignalLock = PMDeviceStatus.SignalLock;
								DeviceStatus->DataLock = PMDeviceStatus.DataLock;
								DeviceStatus->LNBFault = PMDeviceStatus.LNBFault;
								DeviceStatus->VBER = PMDeviceStatus.VBER;
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
								DeviceStatus->Uncorrectables = PMDeviceStatus.Uncorrectables;
								DeviceStatus->DemodulatorGain = 0;
								DeviceStatus->CardStatus = 0;
								memcpy( DeviceStatus->UID,	PMDeviceStatus.UID, 32 );
								DeviceStatus->EthernetTransmit = PMDeviceStatus.EthernetTransmit;
								DeviceStatus->EthernetReceive = PMDeviceStatus.EthernetReceive;
								DeviceStatus->DVBScrambled = PMDeviceStatus.EthernetTransmitError;
								DeviceStatus->EthernetPacketDropped = PMDeviceStatus.EthernetPacketDropped;
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
								memcpy( (void *)&(DeviceStatus->PIDList), (void *)&(PMDeviceStatus.PIDList), sizeof( S75D_PIDList ) );
								memset( DeviceStatus->PIDRoutes, 0, sizeof( PIDRoute ) * 32 );
								memset(	DeviceStatus->PIDDestinations, 0, sizeof( PIDDestination ) * NUM_S75PRO_DESTINATIONS );
								memset(	DeviceStatus->ProgramList, 0, 32 );
								memset(	DeviceStatus->ProgramStatus, 0, 32 );
*/
