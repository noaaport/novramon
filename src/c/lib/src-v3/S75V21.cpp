//#include "stdafx.h"

#include "S75V21.h"


S75V21::S75V21()
{
	initAttributes();
}


S75V21::S75V21( unsigned char *buffer )
{
	initAttributes();
	setFixedParameters( buffer );
	createIndexedParameters();
}


S75V21::~S75V21()
{
}


void S75V21::initAttributes()
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


//string S75V21::macFromStatus( unsigned char *buffer )
//{
//	char mac_str[20];
//
//	sprintf( mac_str, "%02x-%02x-%02x-%02x-%02x-%02x", 
//			 buffer[48], buffer[49], buffer[50], buffer[51], buffer[52], buffer[53] );
//
//	return string( mac_str );
//}


bool S75V21::checkStatusPacket( string ip, unsigned char *buffer )
{
	bool isForThisReceiver = true;


	try {
		if ( ip != getParameter( RECEIVER_IP ).asString() ) throw;
		if ( buffer[32] != ID_S75V21 ) throw;
		if ( fixedParamFromStatus( RECEIVER_MAC, buffer ) != getParameter( RECEIVER_MAC ).asString() ) throw;
	}

	catch ( int ) {
		isForThisReceiver = false;
	}

	return isForThisReceiver;
}


bool S75V21::connected( int timeout )
{
	unsigned char junk[1500];

	return getPIDList( junk );
}


bool S75V21::setFixedParameters( unsigned char *buffer )
{
	char temp[20];

	if ( buffer[32] == ID_S75V21 ) {

		currentParameters[DEVICE_TYPE]->setM_value( string( itoa( R_S75_V2_1, temp, 10 ) ) );

		currentParameters[RECEIVER_MAC]->setM_value( fixedParamFromStatus( RECEIVER_MAC, buffer ) );

		currentParameters[DSP_VERSION]->setM_value( fixedParamFromStatus( DSP_VERSION, buffer ) );

		currentParameters[DSP_REVISION]->setM_value( fixedParamFromStatus( DSP_REVISION, buffer ) );

		currentParameters[RF_FIRMWARE_VERSION]->setM_value( fixedParamFromStatus( RF_FIRMWARE_VERSION, buffer ) );

		currentParameters[SIZE_PID_LIST]->setM_value( string( itoa( NUM_PIDS_S75V21 ,temp, 10 ) ) );

		return true;

	} else {

		return false;

	}
}


string S75V21::fixedParamFromStatus( ReceiverParameter p, unsigned char *buffer )
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


string S75V21::dynamicParamFromStatus( ReceiverParameter p, unsigned char *buffer, int index )
{

	string return_str = "";

	switch( p ) {

	default								:	return_str = "";
											break;

	}

	return return_str;
}




