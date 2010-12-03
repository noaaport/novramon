//#include "stdafx.h"

#include "S75J.h"

typedef unsigned char BYTE;
typedef unsigned short WORD;

typedef struct StatusMSG {

	BYTE DemodulatorStatus;	// Demodulator Status
	BYTE SignalStrength;	// AGC

} StatusMSG;


S75J::S75J()
{
	initAttributes();
}


S75J::S75J( unsigned char *buffer )
{
	initAttributes();
	setFixedParameters( buffer );
	createIndexedParameters();
}


S75J::~S75J()
{
}


void S75J::initAttributes()
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


//string S75J::macFromStatus( unsigned char *buffer )
//{
//	char mac_str[20];
//
//	sprintf( mac_str, "%02x-%02x-%02x-%02x-%02x-%02x", 
//			 buffer[34], buffer[35], buffer[36], buffer[37], buffer[38], buffer[39] );
//
//	return string( mac_str );
//}


bool S75J::checkStatusPacket( string ip, unsigned char *buffer )
{
	bool isForThisReceiver = true;


	try {
		if ( ip != getParameter( RECEIVER_IP ).asString() ) throw;
		if ( buffer[32] != ID_S75J ) throw;
		if ( fixedParamFromStatus( RECEIVER_MAC, buffer ) != getParameter( RECEIVER_MAC ).asString() ) throw;
	}

	catch ( int ) {
		isForThisReceiver = false;
	}

	return isForThisReceiver;
}


bool S75J::connected( int timeout )
{
	unsigned char junk[1500];

	return pollStatus( junk, timeout );
}


bool S75J::setFixedParameters( unsigned char *buffer )
{
	char temp[20];

	if ( buffer[32] == ID_S75J ) {

		currentParameters[DEVICE_TYPE]->setM_value( string( itoa( R_S75J, temp, 10 ) ) );

		currentParameters[RECEIVER_MAC]->setM_value( fixedParamFromStatus( RECEIVER_MAC, buffer ) );

		currentParameters[DSP_VERSION]->setM_value( fixedParamFromStatus( DSP_VERSION, buffer ) );

		currentParameters[DSP_REVISION]->setM_value( fixedParamFromStatus( DSP_REVISION, buffer ) );

		currentParameters[RF_FIRMWARE_VERSION]->setM_value( fixedParamFromStatus( RF_FIRMWARE_VERSION, buffer ) );

		currentParameters[SIZE_PID_LIST]->setM_value( string( itoa( NUM_PIDS_S75J ,temp, 10 ) ) );

		return true;

	} else {

		return false;

	}
}


string S75J::fixedParamFromStatus( ReceiverParameter p, unsigned char *buffer )
{
	char temp[100];
	string return_str = "";

	switch( p ) {

	case RECEIVER_MAC			:	sprintf(	temp, "%02x-%02x-%02x-%02x-%02x-%02x", 
												buffer[34], buffer[35], buffer[36],
												buffer[37], buffer[38], buffer[39] );
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


string S75J::dynamicParamFromStatus( ReceiverParameter p, unsigned char *buffer, int index )
{
//	StatusMSG *status = (StatusMSG *)buffer;
//	char temp[100];
	string return_str = "";
//	int signal_lock;
//	double ss;


	switch( p ) {


	default								:	return_str = "";
											break;

	}

	return return_str;
}




