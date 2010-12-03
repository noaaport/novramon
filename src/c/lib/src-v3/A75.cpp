//#include "stdafx.h"

#include "A75.h"


A75::A75()
{
	initAttributes();
}


A75::A75( unsigned char *buffer )
{
	initAttributes();
	setFixedParameters( buffer );
	createIndexedParameters();
}


A75::~A75()
{
}


void A75::initAttributes()
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


//string A75::macFromStatus( unsigned char *buffer )
//{
//	char mac_str[20];
//
//	sprintf( mac_str, "%02x-%02x-%02x-%02x-%02x-%02x", 
//			 buffer[48], buffer[49], buffer[50], buffer[51], buffer[52], buffer[53] );
//
//	return string( mac_str );
//}


bool A75::checkStatusPacket( string ip, unsigned char *buffer )
{
	bool isForThisReceiver = true;

	try {
		if ( ip != getParameter( RECEIVER_IP ).asString() ) throw;
		if ( buffer[32] != ID_A75 ) throw;
		if ( fixedParamFromStatus( RECEIVER_MAC, buffer ) != getParameter( RECEIVER_MAC ).asString() ) throw;
	}

	catch ( int ) {
		isForThisReceiver = false;
	}

	return isForThisReceiver;
}


bool A75::connected( int timeout )
{
	unsigned char junk[1500];

	return getPIDList( junk );
}


bool A75::setFixedParameters( unsigned char *buffer )
{
	char temp[20];

	
	if ( buffer[32] == ID_A75 ) {

		currentParameters[DEVICE_TYPE]->setM_value( string( itoa( R_A75, temp, 10 ) ) );

		currentParameters[RECEIVER_MAC]->setM_value( fixedParamFromStatus( RECEIVER_MAC, buffer ) );

		currentParameters[DSP_VERSION]->setM_value( fixedParamFromStatus( DSP_VERSION, buffer ) );

		currentParameters[DSP_REVISION]->setM_value( fixedParamFromStatus( DSP_REVISION, buffer ) );

		currentParameters[RF_FIRMWARE_VERSION]->setM_value( fixedParamFromStatus( RF_FIRMWARE_VERSION, buffer ) );

		currentParameters[SIZE_PID_LIST]->setM_value( string( itoa( NUM_PIDS_A75 ,temp, 10 ) ) );

		return true;
	
	} else {
	
		return false;

	}

}


string A75::fixedParamFromStatus( ReceiverParameter p, unsigned char *buffer )
{
	char temp[100];
	string return_str = "";

	switch( p ) {

	case RECEIVER_MAC			:	sprintf(	temp, "%02x-%02x-%02x-%02x-%02x-%02x", 
												buffer[48], buffer[49], buffer[50],
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




string A75::dynamicParamFromStatus( ReceiverParameter p, unsigned char *buffer, int index )
{

	string return_str = "";

	switch( p ) {

	default								:	return_str = "";
											break;

	}

	return return_str;
}




