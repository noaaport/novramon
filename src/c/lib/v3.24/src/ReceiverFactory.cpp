//#include "stdafx.h"

#include "ReceiverFactory.h"


string ReceiverFactory::MACFromStatus( unsigned char *buffer )
{
	char mac_str[20];
	string return_str;

	sprintf( mac_str, "%02x-%02x-%02x-%02x-%02x-%02x", 
			 buffer[48], buffer[49], buffer[50], buffer[51], buffer[52], buffer[53] );

	if ( strncmp( mac_str, "00-06-76", 8 ) != 0 ) {

		sprintf( mac_str, "%02x-%02x-%02x-%02x-%02x-%02x", 
				 buffer[34], buffer[35], buffer[36], buffer[37], buffer[38], buffer[39] );

	}

	if ( strncmp( mac_str, "00-06-76", 8 ) != 0 ) {

		sprintf( mac_str, "%02x-%02x-%02x-%02x-%02x-%02x", 
				 buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9] );

	}

	if ( strncmp( mac_str, "00-06-76", 8 ) != 0 ) {

		return_str = "";

	} else {

		return_str = string( mac_str );

	}

	return return_str;

}


/*
NOVRA_DEVICE ReceiverFactory::DeviceTypeFromStatus( unsigned char *buffer )
{

	NOVRA_DEVICE device_type = UNKNOWN;

	switch( buffer[32] ) {

		case 0x32 : device_type = S75D_V21;
					break;

		case 0x33 : device_type = S75D_V3;
					break;


		case 0x34 : device_type = PesMaker;
					break;

		case 0x43 : device_type = A75;
					break;


		case 0x44 : device_type = A75PesMaker;
					break;

		case 0x55 : device_type = S75J;
					break;

		case 0x66 :
		case 0x76 : device_type = S75CAPRO;
					break;

		case 0x67 : device_type = S75CA;
					break;


		case 0x68 : device_type = S75PLUSPRO;
					break;

		case 0x69 : device_type = S75PLUS;
					break;

		case 0x6A : device_type = S75FKPRO;
					break;

		case 0x6B : device_type = S75FK;
					break;

		case 0x6C : device_type = S75IPREMAP;
					break;

		default   : device_type = UNKNOWN;
					break;

	}


	return device_type;

}
*/


Receiver* ReceiverFactory::Create( unsigned char *buffer, string ip, int status_port )
{
	Receiver *receiver = NULL;
	char temp_str[10];
	unsigned char a, b;

	a = buffer[32];

	switch( buffer[32] ) {

		case ID_S75V21			:	receiver = new S75V21( buffer );
									break;

		case ID_S75V3			:	receiver = new S75V3( buffer );
									break;

		case ID_S75V			:	receiver = new S75V( buffer );
									break;

		case ID_A75				:	receiver = new A75( buffer );
									break;

		case ID_A75V			:	receiver = new A75V( buffer );
									break;

		case ID_S75J			:	receiver = new S75J( buffer );
									break;

		case ID_S75CAPRO		:
		case ID_S75CAPRO_BETA	:	receiver = new S75CAPRO( buffer );
									break;

		case ID_S75CA			:	receiver = new S75CA( buffer );
									break;

		case ID_S75PLUSPRO		:	receiver = new S75PLUSPRO( buffer );
									break;

		case ID_S75PLUS			:	b = buffer[33];
									if ( ( b & 0xF0 ) == 0x50 ) {
										receiver = new S75JPLUS( buffer );
									} else {
										receiver = new S75PLUS( buffer );
									}
									break;

		case ID_S75FKPRO		:	receiver = new S75FKPRO( buffer );
									break;

		case ID_S75FK			:	receiver = new S75FK( buffer );
									break;

		case ID_S75IPREMAP		:	receiver = new S75IPREMAP( buffer );
									break;

		case ID_S200			:	receiver = new S200( buffer );
									break;

		case ID_S200PRO			:	receiver = new S200PRO( buffer );
									break;

		case ID_S200CA			:	receiver = new S200CA( buffer );
									break;

		case ID_S200CAPRO		:	receiver = new S200CAPRO( buffer );
									break;

		case ID_S200CAPROCS		:	receiver = new S200CAPROCS( buffer );
									break;

		case ID_S200FK			:	receiver = new S200FK( buffer );
									break;

		case ID_S200FKPRO		:	receiver = new S200FKPRO( buffer );
									break;

		case ID_S200V			:	receiver = new S200V( buffer );
									break;

		case ID_S200VCA			:	receiver = new S200VCA( buffer );
									break;

		case ID_S200J			:	receiver = new S200J( buffer );
									break;

		case ID_SJ300			:	receiver = new SJ300( buffer );
									break;

		default					:	receiver = NULL;
									break;

	};

	if ( receiver && ( ip != "" ) ) {
		receiver->setCurrentParameter( RECEIVER_IP, ip );
	}

	if ( receiver && ( status_port != 0 ) ) {
		receiver->setCurrentParameter( BROADCAST_STATUS_PORT, string( itoa( status_port, temp_str, 10 ) ) );
	} else {
		receiver->setRemote();
	}


	return receiver;
}

