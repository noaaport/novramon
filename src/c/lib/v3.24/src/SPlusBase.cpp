//#include "stdafx.h"

#include "SPlusBase.h"

#ifdef WINDOWS
    #include "Winsock2.h"
#endif

#ifndef WINDOWS
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>

    typedef int SOCKET;
#endif

#include "math.h"
#include "Password.h"
#include "Keycrypt.h"
#include <time.h>
#include "ConfigPorts.h"
#include "sha1.h"

#define NUMBER_OF_PIDS				32
#define NUMBER_OF_DESTINATIONS		16
#define NUMBER_OF_PATS				16
#define NUMBER_OF_PROGRAMS			16
#define NUMBER_OF_FIXED_KEYS		64
#define NUMBER_OF_IP_REMAP_RULES	16

const int PAT_CONFIG_PORT		= 0x1952;
const int PROGRAM_CONFIG_PORT	= 0x1958;
const int NETWORK_CONFIG_PORT	= 0x1975;
const int PID_CONFIG_PORT		= 0x1977;
const int RF_CONFIG_PORT		= 0x1983;
const int PRODUCTID_CONFIG_PORT	= 0x1984;
const int TRAP_CONFIG_PORT		= 0x2005;
const int SET_PASSWORD_PORT		= 0x2010;
const int STATUS_POLL_PORT		= 0x2011;
const int FK_CONFIG_PORT		= 0x2022;
const int CIPHER_SABER_PORT		= 0x9899;
const int PROGRAM_INFO_PORT		= 0x9992;
const int VIDEO_PROGRAM_PORT	= 0x9993;
const int IP_REMAP_PORT			= 0x9994;
const int DEFAULT_CONFIG_PORT	= 0x9995;
const int PSI_TABLE_PORT		= 0x9996;
const int CAM_WATCHDOG_PORT		= 0x9997;
const int RESET_PORT			= 0x9999;


/*enum CONFIG_PORT_INDEXES {	I_PAT_CONFIG_PORT,
							I_PROGRAM_CONFIG_PORT,
							I_NETWORK_PORT,
							I_PID_PORT,
							I_RF_PORT,
							I_PRODUCTID_CONFIG_PORT,
							I_TRAP_CONFIG_PORT,
							I_SET_PASSWORD_PORT,
							I_STATUS_POLL_PORT,
							I_FK_CONFIG_PORT,
							I_IP_REMAP_CONFIG_PORT,
							I_DEFAULT_CONFIG_PORT,
							I_PSI_TABLE_PORT,
							I_CAM_WATCHDOG_PORT,
							I_RESET_PORT,
							I_NUM_CONFIG_PORTS		};
*/

/*
static const unsigned short config_ports[] = {	PAT_CONFIG_PORT,
												PROGRAM_CONFIG_PORT,
												NETWORK_CONFIG_PORT,
												PID_CONFIG_PORT,
												RF_CONFIG_PORT,
												PRODUCTID_CONFIG_PORT,
												TRAP_CONFIG_PORT,
												SET_PASSWORD_PORT,
												STATUS_POLL_PORT,												
												FK_CONFIG_PORT,
												IP_REMAP_PORT,
												DEFAULT_CONFIG_PORT,
												PSI_TABLE_PORT,
												CAM_WATCHDOG_PORT,
												RESET_PORT			};
*/


/*
static ReceiverParameter rfParams[] = {	LBAND_FREQUENCY,
										SYMBOL_RATE,
										DVB_SIGNAL_TYPE_CONTROL,
										LNB_POWER,
										BAND,
										POLARITY,
										HI_VOLTAGE_SWITCH,
										LONG_LINE_COMPENSATION_SWITCH,
										POLARITY_SWITCHING_VOLTAGE,
										BAND_SWITCHING_TONE_FREQUENCY,
										LO_FREQUENCY  	 				};
*/


static ReceiverParameter pidParams[] = {	PID,
											STREAM_DESTINATION_IP,
											STREAM_DESTINATION_PORT,
											RAW_FORWARD_FLAG,
											DESTINATION_MASK,
											PID_DESTINATION_IP,
											PID_DESTINATION_PORT,
											FORWARD_ALL_FLAG,
											FORWARD_NULLS_FLAG		};


static ReceiverParameter patParams[] = {	PROGRAM_NUMBER,
											PMT_PID					};


static ReceiverParameter fkParams[] = {	FIXED_KEY_PID,
										FIXED_KEY,
										FIXED_KEY_SIGN				};

static ReceiverParameter ipRemapParams[] = {	IP_REMAP_ENABLE,
												ORIGINAL_IP,
												NEW_IP,
												IP_REMAP_MASK,
												IP_REMAP_TTL,
												IP_REMAP_ACTION		};


typedef unsigned char BYTE;
typedef unsigned short WORD;


typedef struct SPlusPIDRoute {
	WORD PID;
	WORD DestinationMask;
} SPlusPIDRoute;


typedef struct SPlusPIDDestination {
	BYTE DestinationIP[4];
	WORD DestinationUDP;
} SPlusPIDDestination;


typedef struct SPlusPATEntry {
	WORD ProgramNumber;
	WORD PMTPID;
} SPlusPATEntry;


typedef struct SPlusFKEntry {
	WORD PID;
	BYTE key[8];
} SPlusFKEntry;


typedef struct SPlusIPRemapRule {

	unsigned short action;
	unsigned short ttl;
	BYTE originalIP[4];
	BYTE newIP[4];
	BYTE mask[4];

} SPlusIPRemapRule;


typedef struct SPlusStatusRequestMSG {
	BYTE Message[6];	 
} SPlusStatusRequestMSG;




typedef struct SPlusPIDMSG {

    WORD					Opcode;               // 1 = download (S75 to host), 2 = upload (host to S75)
    SPlusPIDRoute				PIDTableEntry[NUMBER_OF_PIDS];
	SPlusPIDDestination			PIDDestinations[NUMBER_OF_DESTINATIONS];
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1

} SPlusPIDMSG;


typedef struct SPlusPATMSG {

	WORD Opcode;
	SPlusPATEntry PATTableEntry[NUMBER_OF_DESTINATIONS];
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1

} SPlusPATMSG;


typedef struct SPlusCAProgramStatusMSG {

	WORD Opcode;
	WORD ProgramList[NUMBER_OF_PROGRAMS];
	WORD ProgramStatus[NUMBER_OF_PROGRAMS];

} SPlusCAProgramStatusMSG;
	

typedef struct SPlusCAProgramMSG {

	WORD Opcode;
	WORD ProgramList[NUMBER_OF_PROGRAMS];
	WORD Random;		
	BYTE Signature[20];		

} SPlusCAProgramMSG;
	
	
typedef struct SPlusCAMWatchdogMSG {

	WORD Opcode;		
	WORD CAMwatchdogInterval;
	WORD Random;		
	BYTE Signature[20];				

} SPlusCAMWatchdogMSG;


typedef struct SPlusFKMSG {

	WORD Opcode;		
	SPlusFKEntry	FK[NUMBER_OF_FIXED_KEYS];
	WORD Random;		
	BYTE Signature[20];				

} SPlusFKMSG;



typedef struct SPlusIPRemapMSG {

    WORD			Opcode;
	WORD            enable;
	SPlusIPRemapRule		remapping_rule[NUMBER_OF_IP_REMAP_RULES];
	WORD			Random;				// 
	BYTE			Signature[20];		// HMAC-SHA1

} SPlusIPRemapMSG;


typedef struct SPlusSetPasswordMSG
{
	unsigned char newPassword[16];
	WORD Random;		
	BYTE Signature[20];				

} SPlusSetPasswordMSG;


typedef struct CipherSaberMSG
{

    WORD			Opcode;
	char			Key[64];
	WORD			Iterations;
	WORD			Random;				// 
	BYTE			Signature[20];		// HMAC-SHA1

} CipherSaberMSG;


extern pstruct m_sec;



SPlusBase::SPlusBase()
{

	config_ports = new unsigned short[I_NUM_CONFIG_PORTS];

	SETUP_PORT_TABLE;

	currentPIDs = NULL;
	pendingPIDs = NULL;
	currentRawForwardFlags = NULL;
	pendingRawForwardFlags = NULL;
	currentDestinationMasks = NULL;
	pendingDestinationMasks = NULL;
	currentPIDDestinationIPs = NULL;
	pendingPIDDestinationIPs = NULL;
	currentPIDDestinationPorts = NULL;
	pendingPIDDestinationPorts = NULL;
	currentProgramNumbers = NULL;
	pendingProgramNumbers = NULL;
	currentPMTPIDs = NULL;
	pendingPMTPIDs = NULL;
	currentCAProgramNumbers = NULL;
	pendingCAProgramNumbers = NULL;
	CAProgramStatus = NULL;
	currentFKPIDs = NULL;
	pendingFKPIDs = NULL;
	currentFKSigns = NULL;
	pendingFKSigns = NULL;
	currentFKs = NULL;
	pendingFKs = NULL;
	currentOriginalIPs = NULL;
	pendingOriginalIPs = NULL;
	currentNewIPs = NULL;
	pendingNewIPs = NULL;
	currentIPRemapMasks = NULL;
	pendingIPRemapMasks = NULL;
	currentIPRemapTTLs = NULL;
	pendingIPRemapTTLs = NULL;
	currentIPRemapActions = NULL;
	pendingIPRemapActions = NULL;
//	initAttributes();
}


SPlusBase::SPlusBase( unsigned char *buffer )
{
	config_ports = new unsigned short[I_NUM_CONFIG_PORTS];

	SETUP_PORT_TABLE;

	currentPIDs = NULL;
	pendingPIDs = NULL;
	currentRawForwardFlags = NULL;
	pendingRawForwardFlags = NULL;
	currentDestinationMasks = NULL;
	pendingDestinationMasks = NULL;
	currentPIDDestinationIPs = NULL;
	pendingPIDDestinationIPs = NULL;
	currentPIDDestinationPorts = NULL;
	pendingPIDDestinationPorts = NULL;
	currentProgramNumbers = NULL;
	pendingProgramNumbers = NULL;
	currentPMTPIDs = NULL;
	pendingPMTPIDs = NULL;
//	initAttributes();
	currentCAProgramNumbers = NULL;
	pendingCAProgramNumbers = NULL;
	CAProgramStatus = NULL;
	currentFKPIDs = NULL;
	pendingFKPIDs = NULL;
	currentFKSigns = NULL;
	pendingFKSigns = NULL;
	currentFKs = NULL;
	pendingFKs = NULL;
	currentOriginalIPs = NULL;
	pendingOriginalIPs = NULL;
	currentNewIPs = NULL;
	pendingNewIPs = NULL;
	currentIPRemapMasks = NULL;
	pendingIPRemapMasks = NULL;
	currentIPRemapTTLs = NULL;
	pendingIPRemapTTLs = NULL;
	currentIPRemapActions = NULL;
	pendingIPRemapActions = NULL;
//	setFixedParameters( buffer );
//	createIndexedParameters();
}


SPlusBase::~SPlusBase()
{
	if ( config_ports != NULL ) {
		delete [] config_ports;
	}
	if ( currentPIDs != NULL ) {
		delete [] currentPIDs;
	}
	if ( pendingPIDs != NULL ) {
		delete [] pendingPIDs;
	}
	if ( currentRawForwardFlags != NULL ) {
		delete [] currentRawForwardFlags;
	}
	if ( pendingRawForwardFlags != NULL ) {
		delete [] pendingRawForwardFlags;
	}
	if ( currentDestinationMasks != NULL ) {
		delete [] currentDestinationMasks;
	}
	if ( pendingDestinationMasks != NULL ) {
		delete [] pendingDestinationMasks;
	}
	if ( currentPIDDestinationIPs != NULL ) {
		delete [] currentPIDDestinationIPs;
	}
	if ( pendingPIDDestinationIPs != NULL ) {
		delete [] pendingPIDDestinationIPs;
	}
	if ( currentPIDDestinationPorts != NULL ) {
		delete [] currentPIDDestinationPorts;
	}
	if ( pendingPIDDestinationPorts != NULL ) {
		delete [] pendingPIDDestinationPorts;
	}
	if ( currentProgramNumbers != NULL ) {
		delete [] currentProgramNumbers;
	}
	if ( pendingProgramNumbers != NULL ) {
		delete [] pendingProgramNumbers;
	}
	if ( currentPMTPIDs != NULL ) {
		delete [] currentPMTPIDs;
	}
	if ( pendingPMTPIDs != NULL ) {
		delete [] pendingPMTPIDs;
	}
	if ( currentCAProgramNumbers != NULL ) {
		delete [] currentCAProgramNumbers;
	}
	if ( pendingCAProgramNumbers != NULL ) {
		delete [] pendingCAProgramNumbers;
	}
	if ( CAProgramStatus != NULL ) {
		delete [] CAProgramStatus;
	}
	if ( currentFKPIDs != NULL ) {
		delete [] currentFKPIDs;
	}
	if ( pendingFKPIDs != NULL ) {
		delete [] pendingFKPIDs;
	}
	if ( currentFKSigns != NULL ) {
		delete [] currentFKSigns;
	}
	if ( pendingFKSigns != NULL ) {
		delete [] pendingFKSigns;
	}
	if ( currentFKs != NULL ) {
		delete [] currentFKs;
	}
	if ( pendingFKs != NULL ) {
		delete [] pendingFKs;
	}
	if ( currentOriginalIPs != NULL ) {
		delete [] currentOriginalIPs;
	}
	if ( pendingOriginalIPs != NULL ) {
		delete [] pendingOriginalIPs;
	}
	if ( currentNewIPs != NULL ) {
		delete [] currentNewIPs;
	}
	if ( pendingNewIPs != NULL ) {
		delete [] pendingNewIPs;
	}
	if ( currentIPRemapMasks != NULL ) {
		delete [] currentIPRemapMasks;
	}
	if ( pendingIPRemapMasks != NULL ) {
		delete [] pendingIPRemapMasks;
	}
	if ( currentIPRemapTTLs != NULL ) {
		delete [] currentIPRemapTTLs;
	}
	if ( pendingIPRemapTTLs != NULL ) {
		delete [] pendingIPRemapTTLs;
	}
	if ( currentIPRemapActions != NULL ) {
		delete [] currentIPRemapActions;
	}
	if ( pendingIPRemapActions != NULL ) {
		delete [] pendingIPRemapActions;
	}

}




int SPlusBase::numConfigPorts()
{
	return I_NUM_CONFIG_PORTS;
}


unsigned short SPlusBase::configPort( int i )
{
	if ( i < numConfigPorts() ) {
		return config_ports[i];
	} else {
		return 0;
	}
}




bool SPlusBase::checkStatusPacket( string ip, unsigned char *buffer )
{
	bool isForThisReceiver = true;


	try {
		if ( ip != getParameter( RECEIVER_IP ).asString() ) {
			if ( ip != getParameter( RECEIVER_IP, PENDING ).asString() ) throw 1;
		}
		if ( buffer[32] != typeIDCode() ) throw 1;
		if ( fixedParamFromStatus( RECEIVER_MAC, buffer ) != getParameter( RECEIVER_MAC ).asString() ) throw 1;
	}

	catch ( int ) {
		isForThisReceiver = false;
	}

	return isForThisReceiver;
}


bool SPlusBase::setFixedParameters( unsigned char *buffer )
{
	char temp[20];

	if ( buffer[32] == typeIDCode() ) {

		currentParameters[DEVICE_TYPE]->setM_value( string( itoa( receiverTypeCode(), temp, 10 ) ) );

		currentParameters[RECEIVER_MAC]->setM_value( fixedParamFromStatus( RECEIVER_MAC, buffer ) );

		currentParameters[DSP_VERSION]->setM_value( fixedParamFromStatus( DSP_VERSION, buffer ) );

		currentParameters[DSP_REVISION]->setM_value( fixedParamFromStatus( DSP_REVISION, buffer ) );

		if ( hasParameter( RF_FIRMWARE_VERSION ) ) {

			currentParameters[RF_FIRMWARE_VERSION]->setM_value( fixedParamFromStatus( RF_FIRMWARE_VERSION, buffer ) );

		}

		if ( hasParameter( FPGA_VERSION ) ) {
	
			currentParameters[FPGA_VERSION]->setM_value( fixedParamFromStatus( FPGA_VERSION, buffer ) );
			currentParameters[FPGA_REVISION]->setM_value( fixedParamFromStatus( FPGA_REVISION, buffer ) );
		
		}

		if ( hasParameter( CAM_VERSION ) ) {

			currentParameters[CAM_VERSION]->setM_value( fixedParamFromStatus( CAM_VERSION, buffer ) );
			currentParameters[CAM_REVISION]->setM_value( fixedParamFromStatus( CAM_REVISION, buffer ) );

		}

		currentParameters[SIZE_PID_LIST]->setM_value( string( itoa( NUMBER_OF_PIDS ,temp, 10 ) ) );

		if ( hasParameter( SIZE_IP_REMAP_LIST ) ) {
			currentParameters[SIZE_IP_REMAP_LIST]->setM_value( string( itoa( NUMBER_OF_IP_REMAP_RULES, temp, 10 ) ) );
		}

		if ( hasParameter( SIZE_CA_PROGRAM_LIST ) ) {
			currentParameters[SIZE_CA_PROGRAM_LIST]->setM_value( string( itoa( NUMBER_OF_PROGRAMS ,temp, 10 ) ) );
		}

		if ( hasParameter( SIZE_DESTINATION_LIST ) ) {
			currentParameters[SIZE_DESTINATION_LIST]->setM_value( string( itoa( NUMBER_OF_DESTINATIONS ,temp, 10 ) ) );
		}

		if ( hasParameter( SIZE_MINI_PAT_LIST ) ) {
			currentParameters[SIZE_MINI_PAT_LIST]->setM_value( string( itoa( NUMBER_OF_PATS ,temp, 10 ) ) );
		}

		if ( hasParameter( SIZE_FIXED_KEY_LIST ) ) {
			currentParameters[SIZE_FIXED_KEY_LIST]->setM_value( string( itoa( NUMBER_OF_FIXED_KEYS ,temp, 10 ) ) );
		}

		if ( hasParameter( MPE ) ) {
			currentParameters[MPE]->setM_value( "Supported" );
		}

		if ( hasParameter( CODE_DOWNLOAD ) ) {
			currentParameters[CODE_DOWNLOAD]->setM_value( "Supported" );
		}

		if ( hasParameter( NETWORK_CODE_DOWNLOAD ) ) {
			currentParameters[NETWORK_CODE_DOWNLOAD]->setM_value( "Supported" );
		}

		if ( hasParameter( FACTORY_RESET ) ) {
			currentParameters[FACTORY_RESET]->setM_value( "Supported" );
		}

		if ( hasParameter( AUTO_SYMBOL_RATE ) ) {
			currentParameters[AUTO_SYMBOL_RATE]->setM_value( "Supported" );
		}

		if ( hasParameter( AUTO_DVBS_TYPE ) ) {
			currentParameters[AUTO_DVBS_TYPE]->setM_value( "Supported" );
		}

		return true;

	} else {

		return false;

	}
}


bool SPlusBase::createIndexedParameters()
{
	int i;

	currentPIDs = new ParameterValue[NUMBER_OF_PIDS];
	pendingPIDs = new ParameterValue[NUMBER_OF_PIDS];
//	currentRawForwardFlags = new ParameterValue[NUMBER_OF_PIDS];
//	pendingRawForwardFlags = new ParameterValue[NUMBER_OF_PIDS];
	currentRawForwardFlags = new EnumeratedReceiverParameter[NUMBER_OF_PIDS];
	for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {
		currentRawForwardFlags[i].setEnumType( RAW_FORWARD_FLAG );
	}
	pendingRawForwardFlags = new EnumeratedReceiverParameter[NUMBER_OF_PIDS];
	for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {
		pendingRawForwardFlags[i].setEnumType( RAW_FORWARD_FLAG );
	}
	currentDestinationMasks = new ParameterValue[NUMBER_OF_PIDS];
	pendingDestinationMasks = new ParameterValue[NUMBER_OF_PIDS];
	currentPIDDestinationIPs = new ParameterValue[NUMBER_OF_DESTINATIONS];
	pendingPIDDestinationIPs = new ParameterValue[NUMBER_OF_DESTINATIONS];
	currentPIDDestinationPorts = new ParameterValue[NUMBER_OF_DESTINATIONS];
	pendingPIDDestinationPorts = new ParameterValue[NUMBER_OF_DESTINATIONS];
	currentProgramNumbers = new ParameterValue[NUMBER_OF_PATS];
	pendingProgramNumbers = new ParameterValue[NUMBER_OF_PATS];
	currentPMTPIDs = new ParameterValue[NUMBER_OF_PATS];
	pendingPMTPIDs = new ParameterValue[NUMBER_OF_PATS];
	currentCAProgramNumbers = new ParameterValue[NUMBER_OF_PROGRAMS];
	pendingCAProgramNumbers = new ParameterValue[NUMBER_OF_PROGRAMS];
//	CAProgramStatus = new ParameterValue[NUMBER_OF_PROGRAMS];
	CAProgramStatus = new EnumeratedReceiverParameter[NUMBER_OF_PROGRAMS];
	for ( i = 0; i < NUMBER_OF_PROGRAMS; i++ ) {
		CAProgramStatus[i].setEnumType( CA_PROGRAM_STATUS );
	}
	currentFKPIDs = new ParameterValue[NUMBER_OF_FIXED_KEYS];
	pendingFKPIDs = new ParameterValue[NUMBER_OF_FIXED_KEYS];
//	currentFKSigns = new ParameterValue[NUMBER_OF_FIXED_KEYS];
//	pendingFKSigns = new ParameterValue[NUMBER_OF_FIXED_KEYS];
	currentFKSigns = new EnumeratedReceiverParameter[NUMBER_OF_FIXED_KEYS];
	for ( i = 0; i < NUMBER_OF_FIXED_KEYS; i++ ) {
		currentFKSigns[i].setEnumType( FIXED_KEY_SIGN );
	}
	pendingFKSigns = new EnumeratedReceiverParameter[NUMBER_OF_FIXED_KEYS];
	for ( i = 0; i < NUMBER_OF_FIXED_KEYS; i++ ) {
		pendingFKSigns[i].setEnumType( FIXED_KEY_SIGN );
	}
	currentFKs = new ParameterValue[NUMBER_OF_FIXED_KEYS];
	pendingFKs = new ParameterValue[NUMBER_OF_FIXED_KEYS];
	currentOriginalIPs = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
	pendingOriginalIPs = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
	currentNewIPs = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
	pendingNewIPs = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
	currentIPRemapMasks = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
	pendingIPRemapMasks = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
	currentIPRemapTTLs = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
	pendingIPRemapTTLs = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
//	currentIPRemapActions = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
//	pendingIPRemapActions = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
	currentIPRemapActions = new EnumeratedReceiverParameter[NUMBER_OF_IP_REMAP_RULES];
	for ( i = 0; i < NUMBER_OF_IP_REMAP_RULES; i++ ) {
		currentIPRemapActions[i].setEnumType( IP_REMAP_ACTION );
	}
	pendingIPRemapActions = new EnumeratedReceiverParameter[NUMBER_OF_IP_REMAP_RULES];
	for ( i = 0; i < NUMBER_OF_IP_REMAP_RULES; i++ ) {
		pendingIPRemapActions[i].setEnumType( IP_REMAP_ACTION );
	}

	return true;
}



bool SPlusBase::connected( int timeout )
{
	unsigned char junk[1500];

	return pollStatus( junk, timeout );
}




bool SPlusBase::login( string password )
{
	struct sockaddr_in device_address;
	SOCKET login_socket;
	int temp1 = 1000;               // Temporary storage variable
	int last_error_code = -1;
	int i;
	string mac;
	int mac_byte[6];
	char junk[2];
//	TCHAR temp[16];
	bool success=false;
	struct timeval timeout;
	fd_set readfs;
	int result;
	SPlusPIDMSG pidRequest;
    BYTE Buffer2[sizeof(SPlusPIDMSG)];


	try {

		// Passwords are tested by requesting the PID List from the receiver

		memset( (void *)&m_sec, 0, sizeof( m_sec ) );
		memset( (void *)&pidRequest, 0, sizeof( pidRequest ) );

		pidRequest.Opcode = htons(0x0001);

		srand( time(NULL) );
		pidRequest.Random = rand();

		mac = getParameter( RECEIVER_MAC ).asString();
		sscanf( mac.c_str(), "%2x%c%2x%c%2x%c%2x%c%2x%c%2x",
				&(mac_byte[0]), junk, &(mac_byte[1]), junk, &(mac_byte[2]),
				junk, &(mac_byte[3]), junk, &(mac_byte[4]), junk, &(mac_byte[5]) );
		for ( i = 0; i < 6; i++ ) {
			m_sec.macaddress[i] = (uint8_t)mac_byte[i];
		}
	
		for ( i = 0; ( i < 8 ) && ( i < (int)(password.length()) ); i++ ) {
			m_sec.password[i] = ((char *)(password.data()))[i];
		}

		digitalsignature( &m_sec, (uint8_t *)(&pidRequest), sizeof( SPlusPIDMSG ) - 20 );

		memcpy( pidRequest.Signature, m_sec.digest, 20 );


		// Create a socket to communicate with the receiver

#ifdef WINDOWS
		if ( ( login_socket = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) throw 1;
#else
		if ( ( login_socket = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) throw 1;
#endif


		// Connect the newly created socket to the receiver.

   		device_address.sin_family = AF_INET;

   		device_address.sin_port = htons( PID_CONFIG_PORT );

    	device_address.sin_addr.s_addr = inet_addr( getParameter( RECEIVER_IP ).asString().c_str() );

		if ( ::connect( login_socket, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr_in ) ) != 0) throw 2;

			
        // Download the PID table.  Retry (if required) up to 4 times

        for ( i = 0; (!success) && ( i < 4 ); i++) {

           	// Send the PID table request to the receiver

            if ( send( login_socket, (const char *)(&pidRequest), sizeof( SPlusPIDMSG), 0 ) != sizeof( SPlusPIDMSG ) ) {

                last_error_code = N_ERROR_PID_SEND;

			} else {

#ifdef WINDOWS
                Sleep(30);
#endif

                timeout.tv_sec = 1;
                timeout.tv_usec = 0;

       	        FD_ZERO( &readfs );
       	        FD_SET( login_socket, &readfs );

                // Try to receive the response from receiver.  Check for errors.

#ifdef WINDOWS
                if ( ( result = select( 1, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#else
                if ( ( result = select( login_socket+1, &readfs, NULL, NULL, &timeout ) ) != -1 ) {
#endif
	       	        if ( result != 0 ) {

#ifdef WINDOWS
  		    	        if( (temp1 = recv( login_socket, (char *)Buffer2, sizeof(SPlusPIDMSG), 0 ) ) == SOCKET_ERROR ) {

            	    		temp1 = GetLastError();

                	        if(temp1 != WSAETIMEDOUT){

        	        			last_error_code = N_ERROR_PID_RECEIVE;

							} // if


#else
   		    	        if( (temp1 = recv( login_socket, (char *)Buffer2, sizeof(SPlusPIDMSG), 0 ) ) == -1 ) {
							last_error_code = N_ERROR_PID_RECEIVE;
#endif

						} else {

	    	    	        // Make sure we have received the right number of bytes back

    		    	        if (temp1 == sizeof(SPlusPIDMSG)) {

				                last_error_code = N_ERROR_SUCCESS;
								success = true;

							} else {

								last_error_code = N_ERROR_PID_ACK_BAD;

							} // if

						} // if recv

					} else {

						last_error_code = N_ERROR_PID_RECEIVE;

					} // if select result 0

				} else {

					last_error_code = N_ERROR_PID_RECEIVE;

				} // if select

			} // if send
	
		} // for 4 retires

	}


	catch ( int e ) {

		switch ( e ) {

			case 1	:	last_error_code = N_ERROR_SOCKET_CREATE;
						break;

			case 2	:	last_error_code = N_ERROR_SOCKET_CONNECT;
						break;

		}
	}


#ifdef WINDOWS
    closesocket( login_socket );
#else
    close( login_socket );
#endif


	if ( last_error_code == N_ERROR_SUCCESS ) {
		
		return true; 
	
	} else {
		
		return false;

	}
}


bool SPlusBase::apply( ReceiverParameter ParameterName )
{

	int result = 0;

	try {
		if ( ParameterName == ALL_PARAMETERS ) {
			if ( !applyPIDConfig() ) throw 1;
			if ( hasParameter( SIZE_CA_PROGRAM_LIST ) ) {
				if ( !applyCAConfig() ) throw 2;
			}
			if ( hasParameter( SIZE_MINI_PAT_LIST ) ) {
				if ( !applyPATConfig() ) throw 3;
			}
			if ( !applyRFConfig() ) throw 4;
			if ( hasParameter( CAM_WATCHDOG_INTERVAL ) ) {
				if ( !applyCAMWatchdogInterval() ) throw 5;
			}
			if ( hasParameter( SIZE_FIXED_KEY_LIST ) ) {
				if ( !applyFixedKeys() ) throw 6;
			}
			if ( hasParameter( SIZE_IP_REMAP_LIST ) ) {
				if ( !applyIPRemapRules() ) throw 7;
			}
			if ( hasParameter( CIPHER_SABER_ITERATIONS ) ) {
				if ( !applyCipherSaber() ) throw 8;
			}
			if ( !applyNetworkConfig() ) throw 9;
		} else if ( ParameterName == CAM_WATCHDOG_INTERVAL ) {
			if ( hasParameter( CAM_WATCHDOG_INTERVAL ) ) {
				if ( !applyCAMWatchdogInterval() ) throw 5;
			}
		} else {
			throw 10;
		}
	}

	catch ( int e ) {

		result = e;

	}

	return !result;

}


bool SPlusBase::applyPATConfig()
{ 
	int num_items = sizeof( patParams ) / sizeof( ReceiverParameter );
	bool dirty = false;
	bool up_to_date = true;
	int i;
//	unsigned short pmt_pid;
//	unsigned short program_number;
	char temp[50];


	for ( i = 0; !dirty && ( i < num_items ); i++ ) {
		if ( dirtyFlags[patParams[i]] ) {
			dirty = true;
		}
	}

	if ( dirty ) {

		try {

			SPlusPATMSG patConfig;

			memset( (void *)&patConfig, 0, sizeof( SPlusPATMSG ) );

			patConfig.Opcode = htons( (unsigned short)0x2 );

			for ( i = 0; i < NUMBER_OF_PATS; i++ ) {
				patConfig.PATTableEntry[i].PMTPID = htons( pendingPMTPIDs[i].asShort() );
				patConfig.PATTableEntry[i].ProgramNumber = htons( pendingProgramNumbers[i].asShort() );
			}


			srand( time(NULL) );
			patConfig.Random = rand();

			digitalsignature( &m_sec, (uint8_t *)(&patConfig), sizeof( SPlusPATMSG ) - 20 );

			memcpy( patConfig.Signature, m_sec.digest, 20 );

			if ( !sendConfig( I_PAT_CONFIG_PORT, (unsigned char *)&patConfig, sizeof( patConfig ) ) ) throw 1;

			// Make pending parameters current

			for ( i = 0; i < NUMBER_OF_PATS; i++ ) {
				currentPMTPIDs[i].setM_value( pendingPMTPIDs[i].getM_value() );
				currentProgramNumbers[i].setM_value( pendingProgramNumbers[i].getM_value() ); 
			}

			for ( i = 0; i < num_items; i++ ) {
				if ( parameterIsEnumerated( patParams[i] ) ) {
					setCurrentParameter( patParams[i], itoa( getParameter( patParams[i], PENDING ).asShort(), temp, 10 ) );
				} else {
					setCurrentParameter( patParams[i], getParameter( patParams[i], PENDING ).asString() );
				}
				dirtyFlags[patParams[i]] = false;
			}

		}

		catch ( int e ) {

			up_to_date = false;

		}

	}

	return up_to_date;
}


bool SPlusBase::applyCAConfig()
{ 
	bool up_to_date = true;
	int i;
//	unsigned short program_number;


	if ( dirtyFlags[CA_PROGRAM_NUMBER] ) {

		try {

			SPlusCAProgramMSG caConfig;

			memset( (void *)&caConfig, 0, sizeof( SPlusCAProgramMSG ) );

			caConfig.Opcode = htons( (unsigned short)0x2 );

			for ( i = 0; i < NUMBER_OF_PROGRAMS; i++ ) {
				caConfig.ProgramList[i] = htons( pendingCAProgramNumbers[i].asShort() );
			}


			srand( time(NULL) );
			caConfig.Random = rand();

			digitalsignature( &m_sec, (uint8_t *)(&caConfig), sizeof( SPlusCAProgramMSG ) - 20 );

			memcpy( caConfig.Signature, m_sec.digest, 20 );

			if ( !sendConfig( I_PROGRAM_CONFIG_PORT, (unsigned char *)&caConfig, sizeof( caConfig ) ) ) throw 1;

			// Make pending parameters current

			for ( i = 0; i < NUMBER_OF_PROGRAMS; i++ ) {
				currentCAProgramNumbers[i].setM_value( pendingCAProgramNumbers[i].getM_value() );
			}

			dirtyFlags[CA_PROGRAM_NUMBER] = false;

		}

		catch ( int e ) {

			up_to_date = false;

		}

	}

	return up_to_date;
}


bool SPlusBase::applyCAMWatchdogInterval()
{
//	bool dirty = false;
	bool up_to_date = true;


	if ( dirtyFlags[CAM_WATCHDOG_INTERVAL] ) {

		try {

			SPlusCAMWatchdogMSG CAMWatchdogConfig;

			memset( (void *)&CAMWatchdogConfig, 0, sizeof( SPlusCAMWatchdogMSG ) );

			CAMWatchdogConfig.Opcode = htons( (unsigned short)0x2 );

			CAMWatchdogConfig.CAMwatchdogInterval = htons( getParameter( CAM_WATCHDOG_INTERVAL, PENDING ).asShort() );

			srand( time(NULL) );
			CAMWatchdogConfig.Random = rand();

			digitalsignature( &m_sec, (uint8_t *)(&CAMWatchdogConfig), sizeof( SPlusCAMWatchdogMSG ) - 20 );

			memcpy( CAMWatchdogConfig.Signature, m_sec.digest, 20 );

			if ( !sendConfig( I_CAM_WATCHDOG_PORT, (unsigned char *)&CAMWatchdogConfig, sizeof( CAMWatchdogConfig ) ) ) throw 1;

			// Make pending parameter current

			setCurrentParameter( CAM_WATCHDOG_INTERVAL, getParameter( CAM_WATCHDOG_INTERVAL, PENDING ).asString() );
			dirtyFlags[CAM_WATCHDOG_INTERVAL] = false;

		}

		catch ( int e ) {

			up_to_date = false;

		}

	}

	return up_to_date;

}


bool SPlusBase::applyPIDConfig()
{ 
	int num_items = sizeof( pidParams ) / sizeof( ReceiverParameter );
	bool dirty = false;
	bool up_to_date = true;
	int i;
	unsigned short pid;
	int ip1, ip2, ip3, ip4;
	char junk[2];
	bool forward_ts = false;
//	char temp[10];
	char temp[50];


	for ( i = 0; !dirty && ( i < num_items ); i++ ) {
		if ( dirtyFlags[pidParams[i]] ) {
			dirty = true;
		}
	}

	if ( dirty ) {

		try {

			SPlusPIDMSG pidConfig;

			memset( (void *)&pidConfig, 0, sizeof( SPlusPIDMSG ) );

			pidConfig.Opcode = htons( (unsigned short)0x2 );


// July 16, 2009			if ( !forward_ts ) {
				for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {
//					if ( pendingPIDs[i].isSet() ) {
						pid = pendingPIDs[i].asShort();
//					} else {
//						pid = 0x1fff;
//					}
					if ( pid == 0x1fff ) {
						pidConfig.PIDTableEntry[i].PID = htons( 0x1fff );
					} else {
						if ( hasParameter( RAW_FORWARD_FLAG ) ) {
							if ( pendingRawForwardFlags[i].asFlag() ) {
								pidConfig.PIDTableEntry[i].PID = htons( pid );
							} else {
								pidConfig.PIDTableEntry[i].PID = htons( 0x2000 | pid );
							}
						} else {
							pidConfig.PIDTableEntry[i].PID = htons( 0x2000 | pid );
						}
					}
					if ( hasParameter( DESTINATION_MASK ) ) {
						pidConfig.PIDTableEntry[i].DestinationMask = htons( pendingDestinationMasks[i].asShort() );
					}
				}

				for ( i = 0; i < NUMBER_OF_DESTINATIONS; i++ ) {
					sscanf( pendingPIDDestinationIPs[i].asString().c_str(), "%d%c%d%c%d%c%d", &ip1, junk, &ip2, junk, &ip3, junk, &ip4 );	
					pidConfig.PIDDestinations[i].DestinationIP[0] = (unsigned char)ip1;
					pidConfig.PIDDestinations[i].DestinationIP[1] = (unsigned char)ip2;
					pidConfig.PIDDestinations[i].DestinationIP[2] = (unsigned char)ip3;
					pidConfig.PIDDestinations[i].DestinationIP[3] = (unsigned char)ip4;
					pidConfig.PIDDestinations[i].DestinationUDP = htons( pendingPIDDestinationPorts[i].asShort() );
				}

// July 16, 2009			}

			if ( hasParameter( FORWARD_ALL_FLAG ) ) {
				if ( pendingParameters[ FORWARD_ALL_FLAG ]->asFlag() ) {
					forward_ts = true;
					if ( pendingParameters[ FORWARD_NULLS_FLAG ]->asFlag() ) {
						pidConfig.PIDTableEntry[0].PID = htons( pendingPIDs[0].asShort() | 0x6000 );	// Feb 25, 2011
// Feb 25, 2011						pidConfig.PIDTableEntry[0].PID = htons( pendingPIDs[0].asShort() | 0xC000 );	// July 16, 2009
// July 16, 2009						pidConfig.PIDTableEntry[0].PID = htons( 0x7FFF );
					} else {
						pidConfig.PIDTableEntry[0].PID = htons( pendingPIDs[0].asShort() | 0x8000 );	// July 16, 2009
// July 16, 2009						pidConfig.PIDTableEntry[0].PID = htons( 0x9FFF );
					}
// July 16, 2009					for ( i = 1; i < NUMBER_OF_PIDS; i++ ) {
// July 16, 2009						pidConfig.PIDTableEntry[i].PID = htons( 0x1fff );
// July 16, 2009					}
					if ( pendingParameters[STREAM_DESTINATION_IP]->isSet() ) {
						sscanf( pendingParameters[ STREAM_DESTINATION_IP ]->asString().c_str(), "%d%c%d%c%d%c%d", &ip1, junk, &ip2, junk, &ip3, junk, &ip4 );	
						pidConfig.PIDDestinations[0].DestinationIP[0] = (unsigned char)ip1;
						pidConfig.PIDDestinations[0].DestinationIP[1] = (unsigned char)ip2;
						pidConfig.PIDDestinations[0].DestinationIP[2] = (unsigned char)ip3;
						pidConfig.PIDDestinations[0].DestinationIP[3] = (unsigned char)ip4;
						pidConfig.PIDDestinations[0].DestinationUDP = htons( pendingParameters[ STREAM_DESTINATION_PORT ]->asShort() );
						pidConfig.PIDTableEntry[0].DestinationMask = htons( 0x8000 );
					}
				}
			}

			srand( time(NULL) );
			pidConfig.Random = rand();

			digitalsignature( &m_sec, (uint8_t *)(&pidConfig), sizeof( SPlusPIDMSG ) - 20 );

			memcpy( pidConfig.Signature, m_sec.digest, 20 );

			if ( !sendConfig( I_PID_PORT, (unsigned char *)&pidConfig, sizeof( pidConfig ) ) ) throw 1;

			// Make pending parameters current

// July 16, 2009			if ( forward_ts ) {

// July 16, 2009				for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {
// July 16, 2009					currentPIDs[i].setM_value( "0x1fff" );
// July 16, 2009					currentRawForwardFlags[i].setM_value( "1" ); 
// July 16, 2009					currentDestinationMasks[i].setM_value( itoa( 0x8000, temp, 10 ) ); 
// July 16, 2009				}

// July 16, 2009				for ( i = 0; i < NUMBER_OF_DESTINATIONS; i++ ) {
// July 16, 2009					currentPIDDestinationIPs[i].setM_value( "0.0.0.0" );
// July 16, 2009					currentPIDDestinationPorts[i].setM_value( "0" );
// July 16, 2009				}

// July 16, 2009			} else {

				for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {
					currentPIDs[i].setM_value( pendingPIDs[i].getM_value() );
					currentRawForwardFlags[i].setM_value( pendingRawForwardFlags[i].getM_value() ); 
					currentDestinationMasks[i].setM_value( pendingDestinationMasks[i].getM_value() ); 
				}

				for ( i = 0; i < NUMBER_OF_DESTINATIONS; i++ ) {
					currentPIDDestinationIPs[i].setM_value( pendingPIDDestinationIPs[i].getM_value() );
					currentPIDDestinationPorts[i].setM_value( pendingPIDDestinationPorts[i].getM_value() );
				}

// July 16, 2009			}

			for ( i = 0; i < num_items; i++ ) {
				if ( parameterIsEnumerated( pidParams[i] ) ) {
					setCurrentParameter( pidParams[i], itoa( getParameter( pidParams[i], PENDING ).asShort(), temp, 10 ) );
				} else {
					setCurrentParameter( pidParams[i], getParameter( pidParams[i], PENDING ).asString() );
				}
				dirtyFlags[pidParams[i]] = false;
			}

		}

		catch ( int e ) {

			up_to_date = false;

		}

	}

	return up_to_date;
}



bool SPlusBase::applyFixedKeys()
{
	int num_items = sizeof( fkParams ) / sizeof( ReceiverParameter );
	bool dirty = false;
	bool up_to_date = true;
	int i;
	unsigned short pid;
	unsigned int k1, k2, k3, k4, k5, k6, k7, k8;
	char temp[50];


	for ( i = 0; !dirty && ( i < num_items ); i++ ) {
		if ( dirtyFlags[fkParams[i]] ) {
			dirty = true;
		}
	}

	if ( dirty ) {

		try {

			SPlusFKMSG fkConfig;

			memset( (void *)&fkConfig, 0, sizeof( SPlusFKMSG ) );

			fkConfig.Opcode = htons( (unsigned short)0x2 );

			for ( i = 0; i < NUMBER_OF_FIXED_KEYS; i++ ) {
				pid = pendingFKPIDs[i].asShort();
				if ( pendingFKSigns[i].asFlag() ) {
					pid = pid | 0x2000;
				}
				fkConfig.FK[i].PID = htons( pid );
				sscanf( pendingFKs[i].asString().c_str(), "%2x%2x%2x%2x%2x%2x%2x%2x", &k1, &k2, &k3, &k4, &k5, &k6, &k7, &k8 );
				fkConfig.FK[i].key[0] = (BYTE)k1;
				fkConfig.FK[i].key[1] = (BYTE)k2;
				fkConfig.FK[i].key[2] = (BYTE)k3;
				fkConfig.FK[i].key[3] = (BYTE)k4;
				fkConfig.FK[i].key[4] = (BYTE)k5;
				fkConfig.FK[i].key[5] = (BYTE)k6;
				fkConfig.FK[i].key[6] = (BYTE)k7;
				fkConfig.FK[i].key[7] = (BYTE)k8;
			}

		 	encryptCSAkeys( (unsigned short *)(fkConfig.FK), NUMBER_OF_FIXED_KEYS );

			srand( time(NULL) );
			fkConfig.Random = rand();

			digitalsignature( &m_sec, (uint8_t *)(&fkConfig), sizeof( SPlusFKMSG ) - 20 );

			memcpy( fkConfig.Signature, m_sec.digest, 20 );

			if ( !sendConfig( I_FK_CONFIG_PORT, (unsigned char *)&fkConfig, sizeof( fkConfig ) ) ) throw 1;

			// Make pending parameters current

			for ( i = 0; i < NUMBER_OF_FIXED_KEYS; i++ ) {
				currentFKPIDs[i].setM_value( pendingFKPIDs[i].getM_value() );
				currentFKSigns[i].setM_value( pendingFKSigns[i].getM_value() );
				currentFKs[i].setM_value( pendingFKs[i].getM_value() );
			}

			for ( i = 0; i < num_items; i++ ) {
				if ( parameterIsEnumerated( fkParams[i] ) ) {
					setCurrentParameter( fkParams[i], itoa( getParameter( fkParams[i], PENDING ).asShort(), temp, 10 ) );
				} else {
					setCurrentParameter( fkParams[i], getParameter( fkParams[i], PENDING ).asString() );
				}
				dirtyFlags[fkParams[i]] = false;
			}

		}

		catch ( int e ) {

			up_to_date = false;

		}

	}

	return up_to_date;
}


bool SPlusBase::applyIPRemapRules()
{
	int num_items = sizeof( ipRemapParams ) / sizeof( ReceiverParameter );
	bool dirty = false;
	bool up_to_date = true;
	int i;
	unsigned char junk[2];
	unsigned int ip1, ip2, ip3, ip4;
	char temp[50];


	for ( i = 0; !dirty && ( i < num_items ); i++ ) {
		if ( dirtyFlags[ipRemapParams[i]] ) {
			dirty = true;
		}
	}

	if ( dirty ) {

		try {

			SPlusIPRemapMSG ipRemapConfig;

			memset( (void *)&ipRemapConfig, 0, sizeof( SPlusIPRemapMSG ) );

			ipRemapConfig.Opcode = htons( (unsigned short)0x2 );

			ipRemapConfig.enable = htons( getParameter( IP_REMAP_ENABLE, PENDING ).asShort() );

			for ( i = 0; i < NUMBER_OF_IP_REMAP_RULES; i++ ) {

				sscanf( pendingOriginalIPs[i].asString().c_str(),
						"%d%c%d%c%d%c%d", &ip1, junk, &ip2, junk, &ip3, junk, &ip4 );
				ipRemapConfig.remapping_rule[i].originalIP[0] = (unsigned char)ip1;
				ipRemapConfig.remapping_rule[i].originalIP[1] = (unsigned char)ip2;
				ipRemapConfig.remapping_rule[i].originalIP[2] = (unsigned char)ip3;
				ipRemapConfig.remapping_rule[i].originalIP[3] = (unsigned char)ip4;

				sscanf( pendingNewIPs[i].asString().c_str(),
						"%d%c%d%c%d%c%d", &ip1, junk, &ip2, junk, &ip3, junk, &ip4 );
				ipRemapConfig.remapping_rule[i].newIP[0] = (unsigned char)ip1;
				ipRemapConfig.remapping_rule[i].newIP[1] = (unsigned char)ip2;
				ipRemapConfig.remapping_rule[i].newIP[2] = (unsigned char)ip3;
				ipRemapConfig.remapping_rule[i].newIP[3] = (unsigned char)ip4;

				sscanf( pendingIPRemapMasks[i].asString().c_str(),
						"%d%c%d%c%d%c%d", &ip1, junk, &ip2, junk, &ip3, junk, &ip4 );
				ipRemapConfig.remapping_rule[i].mask[0] = (unsigned char)ip1;
				ipRemapConfig.remapping_rule[i].mask[1] = (unsigned char)ip2;
				ipRemapConfig.remapping_rule[i].mask[2] = (unsigned char)ip3;
				ipRemapConfig.remapping_rule[i].mask[3] = (unsigned char)ip4;

				ipRemapConfig.remapping_rule[i].ttl = htons( pendingIPRemapTTLs[i].asShort() );

				ipRemapConfig.remapping_rule[i].action = 0;
				if ( pendingIPRemapActions[i].asString() == "Normal" ) {
					ipRemapConfig.remapping_rule[i].action = htons( 1 );
				}
				if ( pendingIPRemapActions[i].asString() == "Forward" ) {
					ipRemapConfig.remapping_rule[i].action = htons( 2 );
				}
				if ( pendingIPRemapActions[i].asString() == "Discard" ) {
					ipRemapConfig.remapping_rule[i].action = htons( 3 );
				}

			}

			srand( time(NULL) );
			ipRemapConfig.Random = rand();

			digitalsignature( &m_sec, (uint8_t *)(&ipRemapConfig), sizeof( SPlusIPRemapMSG ) - 20 );

			memcpy( ipRemapConfig.Signature, m_sec.digest, 20 );

			if ( !sendConfig( I_IP_REMAP_CONFIG_PORT, (unsigned char *)&ipRemapConfig, sizeof( ipRemapConfig ) ) ) throw 1;

			// Make pending parameters current

			for ( i = 0; i < NUMBER_OF_IP_REMAP_RULES; i++ ) {
				currentOriginalIPs[i].setM_value( pendingOriginalIPs[i].getM_value() );
				currentNewIPs[i].setM_value( pendingNewIPs[i].getM_value() );
				currentIPRemapMasks[i].setM_value( pendingIPRemapMasks[i].getM_value() );
				currentIPRemapTTLs[i].setM_value( pendingIPRemapTTLs[i].getM_value() );
				currentIPRemapActions[i].setM_value( pendingIPRemapActions[i].getM_value() );
			}

			for ( i = 0; i < num_items; i++ ) {
				if ( parameterIsEnumerated( ipRemapParams[i] ) ) {
					setCurrentParameter( ipRemapParams[i], itoa( getParameter( ipRemapParams[i], PENDING ).asShort(), temp, 10 ) );
				} else {
					setCurrentParameter( ipRemapParams[i], getParameter( ipRemapParams[i], PENDING ).asString() );
				}
				dirtyFlags[ipRemapParams[i]] = false;
			}

		}

		catch ( int e ) {

			up_to_date = false;

		}

	}

	return up_to_date;
}


bool SPlusBase::applyCipherSaber()
{
//	bool dirty = false;
	bool up_to_date = true;
	char temp[50];
	FILE *key_file = NULL;
	unsigned char key[64];
	unsigned char sig[20];


	if ( dirtyFlags[CIPHER_SABER_ITERATIONS] || dirtyFlags[CIPHER_SABER_KEY_FILE] ) {

		try {

			CipherSaberMSG CipherSaberConfig;

			memset( (void *)&CipherSaberConfig, 0, sizeof( CipherSaberMSG ) );

			// Assume at first that we are just setting the number of iterations
			CipherSaberConfig.Opcode = htons( (unsigned short)0x2 );

			// Can reprogram if we have a new key
			if ( dirtyFlags[CIPHER_SABER_KEY_FILE] ) {
				CipherSaberConfig.Opcode = htons( (unsigned short)0x1 );
			}

			// If iterations has been changed we also need a key
//			if ( dirtyFlags[CIPHER_SABER_ITERATIONS] && !dirtyFlags[CIPHER_SABER_KEY] ) throw 1;

			if ( getParameter( CIPHER_SABER_KEY_FILE, PENDING ).asString() != "" ) {
				if ( ( key_file = fopen( getParameter( CIPHER_SABER_KEY_FILE, PENDING ).asString().c_str(), "rb" ) ) == NULL ) throw 1;
//				if ( fread( (void *)(CipherSaberConfig.Key), 1, 64, key_file ) != 64 ) throw 2;
				if ( fread( (void *)key, 1, 64, key_file ) != 64 ) throw 2;
				if ( fread( (void *)sig, 1, 20, key_file ) != 20 ) throw 3;
				fclose( key_file );
				sha1 verify;
				verify.SHA1( key, 64 );
				if ( memcmp( sig, verify.digest, 20 ) != 0 ) throw 4;
				memcpy( (void *)(CipherSaberConfig.Key), key, 64 );
			}
			if ( dirtyFlags[CIPHER_SABER_ITERATIONS] ) {
				CipherSaberConfig.Iterations = htons( getParameter( CIPHER_SABER_ITERATIONS, PENDING ).asShort() );
			} else {
				CipherSaberConfig.Iterations = htons( getParameter( CIPHER_SABER_ITERATIONS, CURRENT ).asShort() );
			}
			// Boundary Check
			if ( ( ntohs( CipherSaberConfig.Iterations ) < 1 ) || ( ntohs( CipherSaberConfig.Iterations ) > 5 ) ) {
				CipherSaberConfig.Iterations = htons( 1 );
				setParameter( CIPHER_SABER_ITERATIONS, "1" );
			}

//			CipherSaberConfig.Enable = htons( getParameter( CIPHER_SABER_ENABLE, PENDING ).asShort() );

			srand( time(NULL) );
			CipherSaberConfig.Random = rand();

			digitalsignature( &m_sec, (uint8_t *)(&CipherSaberConfig), sizeof( CipherSaberMSG ) - 20 );

			memcpy( CipherSaberConfig.Signature, m_sec.digest, 20 );

			if ( !sendConfig( I_CIPHER_SABER_PORT, (unsigned char *)&CipherSaberConfig, sizeof( CipherSaberConfig ) ) ) throw 1;

			// Make pending parameters current

//			setCurrentParameter( CIPHER_SABER_ENABLE, itoa( getParameter( CIPHER_SABER_ENABLE, PENDING ).asShort(), temp, 10 ) );
			if ( dirtyFlags[CIPHER_SABER_ITERATIONS] ) {
				setCurrentParameter( CIPHER_SABER_ITERATIONS, getParameter( CIPHER_SABER_ITERATIONS, PENDING ).asString() );
			}
			setParameter( CIPHER_SABER_KEY_FILE, "" );
			setCurrentParameter( CIPHER_SABER_KEY_FILE, getParameter( CIPHER_SABER_KEY_FILE, PENDING ).asString() );

//			dirtyFlags[CIPHER_SABER_ENABLE] = false;
			dirtyFlags[CIPHER_SABER_ITERATIONS] = false;
			dirtyFlags[CIPHER_SABER_KEY_FILE] = false;

		}

		catch ( int e ) {

			up_to_date = false;

		}

	}

	return up_to_date;

}



bool SPlusBase::readCurrentSettings()
{
	bool success = true;

	if ( !readPIDTable() ) success = false;
	if ( hasParameter( SIZE_MINI_PAT_LIST ) ) {
		if ( !readPATTable() ) success = false;
	}
	if ( hasParameter( SIZE_CA_PROGRAM_LIST ) ) {
		if ( !readCATable() ) success = false;
		if ( !readCAMWatchdogInterval() ) success = false;
	}
	if ( hasParameter( SIZE_FIXED_KEY_LIST ) ) {
		if ( !readFixedKeys() ) success = false;
	}
	if ( hasParameter( SIZE_IP_REMAP_LIST ) ) {
		if ( !readIPRemapRules() ) success = false;
	}
	if ( hasParameter( CIPHER_SABER_ITERATIONS ) ) {
		if ( !readCipherSaber() ) success = false;
	}

	return success;
}


bool SPlusBase::readPIDTable()
{
	bool success = false;
    SPlusPIDMSG pidRequest;
 	int i;
	unsigned char receive_buffer[1500];
	SPlusPIDMSG *pidReply = (SPlusPIDMSG *)receive_buffer;
	char temp[20];
	unsigned short pid;
	unsigned short mask;



	// Download PIDS

	memset( (void *)&pidRequest, 0, sizeof( pidRequest ) );
	memset( (void *)receive_buffer, 0, 1500 );

	// Set the opcode to download (0x0001)

	pidRequest.Opcode = htons(0x0001);
	srand( time(NULL) );
	pidRequest.Random = rand();

	digitalsignature( &m_sec, (uint8_t *)(&pidRequest), sizeof( pidRequest ) - 20 );

	memcpy( pidRequest.Signature, m_sec.digest, 20 );

	if ( getConfig( I_PID_PORT, (unsigned char *)&pidRequest, receive_buffer, sizeof( pidRequest ), sizeof( SPlusPIDMSG ) ) ) {

		if ( hasParameter( FORWARD_ALL_FLAG ) ) {
			setCurrentParameter( FORWARD_ALL_FLAG, "0" );
		}
		if ( hasParameter( FORWARD_NULLS_FLAG ) ) {
			setCurrentParameter( FORWARD_NULLS_FLAG, "0" );
		}

// July 16, 2009		if ( !getParameter( FORWARD_ALL_FLAG, PENDING ).asFlag() ) {

			for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {

				pid = ntohs( pidReply->PIDTableEntry[i].PID );
				currentPIDs[i].setM_value( string( itoa( ( pid & 0x1fff ), temp, 10 ) ) );			
				pendingPIDs[i].setM_value( currentPIDs[i].getM_value() );

				if ( hasParameter( RAW_FORWARD_FLAG ) ) {
					if ( pid & 0x2000 ) {
						currentRawForwardFlags[i].setM_value( "0" );
					} else {
						currentRawForwardFlags[i].setM_value( "1" );
					}
					pendingRawForwardFlags[i].setM_value( currentRawForwardFlags[i].getM_value() );

					mask = ntohs( pidReply->PIDTableEntry[i].DestinationMask );
					currentDestinationMasks[i].setM_value( string( itoa( mask, temp, 10 ) ) );			
					pendingDestinationMasks[i].setM_value( currentDestinationMasks[i].getM_value() );
				}

			}

			for ( i = 0; i < NUMBER_OF_DESTINATIONS; i++ ) {

				sprintf( temp, "%d.%d.%d.%d",
						 pidReply->PIDDestinations[i].DestinationIP[0],
						 pidReply->PIDDestinations[i].DestinationIP[1],
						 pidReply->PIDDestinations[i].DestinationIP[2],
						 pidReply->PIDDestinations[i].DestinationIP[3]  );
				currentPIDDestinationIPs[i].setM_value( string( temp ) );			
				pendingPIDDestinationIPs[i].setM_value( currentPIDDestinationIPs[i].getM_value() );	

				currentPIDDestinationPorts[i].setM_value( string( itoa( ntohs( pidReply->PIDDestinations[i].DestinationUDP ), temp, 10 ) ) );
				pendingPIDDestinationPorts[i].setM_value( currentPIDDestinationPorts[i].getM_value() );

			}

// July 16, 2009		}

		if ( hasParameter( FORWARD_ALL_FLAG ) ) {
			pid = ntohs( pidReply->PIDTableEntry[0].PID );
// Feb 25, 2011			if ( ( pid & 0xC000 ) == 0xC000 ) {
			if ( ( pid & 0x6000 ) == 0x6000 ) { // Feb 25, 2011
				setCurrentParameter( FORWARD_ALL_FLAG, "1" );
				setCurrentParameter( FORWARD_NULLS_FLAG, "1" );
			}

			if ( ( pid & 0x8000 ) == 0x8000 ) {
				setCurrentParameter( FORWARD_ALL_FLAG, "1" );
			}
			if ( getParameter( FORWARD_ALL_FLAG, PENDING ).asFlag() ) {
				if ( hasParameter( STREAM_DESTINATION_IP ) ) {
					sprintf( temp, "%d.%d.%d.%d",
							 pidReply->PIDDestinations[0].DestinationIP[0],
							 pidReply->PIDDestinations[0].DestinationIP[1],
							 pidReply->PIDDestinations[0].DestinationIP[2],
							 pidReply->PIDDestinations[0].DestinationIP[3]  );
					setCurrentParameter( STREAM_DESTINATION_IP, string( temp ) );			
				}
				if ( hasParameter( STREAM_DESTINATION_PORT ) ) {
					setCurrentParameter( STREAM_DESTINATION_PORT, string( itoa( ntohs( pidReply->PIDDestinations[0].DestinationUDP ), temp, 10 ) ) );
				}
			}
// July 16, 2009			for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {
// July 16, 2009				currentPIDs[i].setM_value( string( itoa( 0x1fff, temp, 10 ) ) );			
// July 16, 2009				pendingPIDs[i].setM_value( currentPIDs[i].getM_value() );
// July 16, 2009			}
// July 16, 2009			for ( i = 0; i < NUMBER_OF_DESTINATIONS; i++ ) {
// July 16, 2009				currentPIDDestinationIPs[i].setM_value( "0.0.0.0" );			
// July 16, 2009				pendingPIDDestinationIPs[i].setM_value( currentPIDDestinationIPs[i].getM_value() );	
// July 16, 2009				currentPIDDestinationPorts[i].setM_value( "0" );			
// July 16, 2009				pendingPIDDestinationPorts[i].setM_value( currentPIDDestinationPorts[i].getM_value() );	
// July 16, 2009				currentDestinationMasks[i].setM_value( "0" );			
// July 16, 2009				pendingDestinationMasks[i].setM_value( currentDestinationMasks[i].getM_value() );
// July 16, 2009			}
		}


		success = true;

	}
	
	return success;
}


bool SPlusBase::readPATTable()
{
	bool success = false;
    SPlusPATMSG patRequest;
	int i;
	unsigned char receive_buffer[1500];
	SPlusPATMSG *patReply = (SPlusPATMSG *)receive_buffer;
	char temp[20];
//	unsigned short pid;



	// Download PAT Config

	memset( (void *)&patRequest, 0, sizeof( patRequest ) );
	memset( (void *)receive_buffer, 0, 1500 );

	// Set the opcode to download (0x0001)

	patRequest.Opcode = htons(0x0001);
	srand( time(NULL) );
	patRequest.Random = rand();

	digitalsignature( &m_sec, (uint8_t *)(&patRequest), sizeof( patRequest ) - 20 );

	memcpy( patRequest.Signature, m_sec.digest, 20 );
			
	if ( getConfig( I_PAT_CONFIG_PORT, (unsigned char *)&patRequest, receive_buffer, sizeof( patRequest ), sizeof( SPlusPATMSG ) ) ) {

		for ( i = 0; i < NUMBER_OF_PATS; i++ ) {
		
			currentProgramNumbers[i].setM_value( string( itoa( ntohs( patReply->PATTableEntry[i].ProgramNumber ), temp, 10 ) ) );
			pendingProgramNumbers[i].setM_value( currentProgramNumbers[i].getM_value() );

			currentPMTPIDs[i].setM_value( string( itoa( ntohs( patReply->PATTableEntry[i].PMTPID ), temp, 10 ) ) );
			pendingPMTPIDs[i].setM_value( currentPMTPIDs[i].getM_value() );

		}

		success = true;

	}
	
	return success;
}


bool SPlusBase::readCATable()
{
	bool success = false;
    SPlusCAProgramMSG CARequest;
	int i;
	unsigned char receive_buffer[1500];
	SPlusCAProgramStatusMSG *CAReply = (SPlusCAProgramStatusMSG *)receive_buffer;
	char temp[20];
//	unsigned short program_number;



	// Download CA Program Config

	memset( (void *)&CARequest, 0, sizeof( CARequest ) );
	memset( (void *)receive_buffer, 0, 1500 );

	// Set the opcode to download (0x0001)

	CARequest.Opcode = htons(0x0001);
	srand( time(NULL) );
	CARequest.Random = rand();

	digitalsignature( &m_sec, (uint8_t *)(&CARequest), sizeof( CARequest ) - 20 );

	memcpy( CARequest.Signature, m_sec.digest, 20 );
			
	if ( getConfig( I_PROGRAM_CONFIG_PORT, (unsigned char *)&CARequest, receive_buffer, sizeof( CARequest ), sizeof( SPlusCAProgramStatusMSG ) ) ) {

		for ( i = 0; i < NUMBER_OF_PROGRAMS; i++ ) {
		
			currentCAProgramNumbers[i].setM_value( string( itoa( ntohs( CAReply->ProgramList[i] ), temp, 10 ) ) );
			pendingCAProgramNumbers[i].setM_value( currentCAProgramNumbers[i].getM_value() );
			CAProgramStatus[i].setM_value( string( itoa( ntohs( CAReply->ProgramStatus[i] ), temp, 10 ) ) );

		}

		success = true;

	}
	
	return success;
}


bool SPlusBase::readCAMWatchdogInterval()
{
	bool success = false;
    SPlusCAMWatchdogMSG CAMWatchdogRequest;
//	int i;
	unsigned char receive_buffer[1500];
	SPlusCAMWatchdogMSG *CAMWatchdogReply = (SPlusCAMWatchdogMSG *)receive_buffer;
	char temp[20];



	// Download CAM Watchdog Timeout

	memset( (void *)&CAMWatchdogRequest, 0, sizeof( CAMWatchdogRequest ) );
	memset( (void *)receive_buffer, 0, 1500 );

	// Set the opcode to download (0x0001)

	CAMWatchdogRequest.Opcode = htons(0x0001);
	srand( time(NULL) );
	CAMWatchdogRequest.Random = rand();

	digitalsignature( &m_sec, (uint8_t *)(&CAMWatchdogRequest), sizeof( CAMWatchdogRequest ) - 20 );

	memcpy( CAMWatchdogRequest.Signature, m_sec.digest, 20 );
			
	if ( getConfig( I_CAM_WATCHDOG_PORT, (unsigned char *)&CAMWatchdogRequest, receive_buffer, sizeof( CAMWatchdogRequest ), sizeof( SPlusCAMWatchdogMSG ) ) ) {

		setCurrentParameter( CAM_WATCHDOG_INTERVAL, string( itoa( ntohs( CAMWatchdogReply->CAMwatchdogInterval ), temp, 10 ) ) ) ;			

		success = true;

	}
	
	return success;
}


bool SPlusBase::readFixedKeys()
{
	bool success = false;
    SPlusFKMSG FKRequest;
	int i;
	unsigned char receive_buffer[1500];
	SPlusFKMSG *FKReply = (SPlusFKMSG *)receive_buffer;
	char temp[20];
	unsigned short pid;
	char key_str[17];


	// Download FK Config

	memset( (void *)&FKRequest, 0, sizeof( FKRequest ) );
	memset( (void *)receive_buffer, 0, 1500 );

	// Set the opcode to download (0x0001)

	FKRequest.Opcode = htons(0x0001);
	srand( time(NULL) );
	FKRequest.Random = rand();

	digitalsignature( &m_sec, (uint8_t *)(&FKRequest), sizeof( FKRequest ) - 20 );

	memcpy( FKRequest.Signature, m_sec.digest, 20 );
			
	if ( getConfig( I_FK_CONFIG_PORT, (unsigned char *)&FKRequest, receive_buffer, sizeof( FKRequest ), sizeof( SPlusFKMSG ) ) ) {

		for ( i = 0; i < NUMBER_OF_FIXED_KEYS; i++ ) {
		
			pid = ntohs( FKReply->FK[i].PID );
			currentFKPIDs[i].setM_value( string( itoa( pid & 0x1fff, temp, 10 ) ) );
			pendingFKPIDs[i].setM_value( currentFKPIDs[i].getM_value() );
			if ( pid & 0x2000 ) {
				currentFKSigns[i].setM_value( string( "1" ) );
			} else {
				currentFKSigns[i].setM_value( string( "0" ) );
			}
			pendingFKSigns[i].setM_value( currentFKSigns[i].getM_value() );
			memset( key_str, 0, 17 );
			sprintf( key_str, "%02x%02x%02x%02x%02x%02x%02x%02x",
					 FKReply->FK[i].key[0], FKReply->FK[i].key[1],
					 FKReply->FK[i].key[2], FKReply->FK[i].key[3],
					 FKReply->FK[i].key[4], FKReply->FK[i].key[5],
					 FKReply->FK[i].key[6], FKReply->FK[i].key[7]  );
			currentFKs[i].setM_value( string( key_str ) );
			pendingFKs[i].setM_value( currentFKs[i].getM_value() );

		}

		success = true;

	}
	
	return success;
}


bool SPlusBase::readIPRemapRules()
{
	bool success = false;
    SPlusIPRemapMSG IPRemapRequest;
	int i;
	unsigned char receive_buffer[1500];
	SPlusIPRemapMSG *IPRemapReply = (SPlusIPRemapMSG *)receive_buffer;
	char temp[20];


	// Download IPRemap Config

	memset( (void *)&IPRemapRequest, 0, sizeof( IPRemapRequest ) );
	memset( (void *)receive_buffer, 0, 1500 );

	// Set the opcode to download (0x0001)

	IPRemapRequest.Opcode = htons(0x0001);
	srand( time(NULL) );
	IPRemapRequest.Random = rand();

	digitalsignature( &m_sec, (uint8_t *)(&IPRemapRequest), sizeof( IPRemapRequest ) - 20 );

	memcpy( IPRemapRequest.Signature, m_sec.digest, 20 );
			
	if ( getConfig( I_IP_REMAP_CONFIG_PORT, (unsigned char *)&IPRemapRequest, receive_buffer, sizeof( IPRemapRequest ), sizeof( SPlusIPRemapMSG ) ) ) {

		setCurrentParameter( IP_REMAP_ENABLE, string( itoa( ntohs( IPRemapReply->enable ), temp, 10 ) ) ) ;			

		for ( i = 0; i < NUMBER_OF_IP_REMAP_RULES; i++ ) {
		
			sprintf( temp, "%d.%d.%d.%d",
					 IPRemapReply->remapping_rule[i].originalIP[0],
					 IPRemapReply->remapping_rule[i].originalIP[1],
					 IPRemapReply->remapping_rule[i].originalIP[2],
					 IPRemapReply->remapping_rule[i].originalIP[3]  );
			currentOriginalIPs[i].setM_value( string( temp ) );
			pendingOriginalIPs[i].setM_value( currentOriginalIPs[i].getM_value() );	

			sprintf( temp, "%d.%d.%d.%d",
					 IPRemapReply->remapping_rule[i].newIP[0],
					 IPRemapReply->remapping_rule[i].newIP[1],
					 IPRemapReply->remapping_rule[i].newIP[2],
					 IPRemapReply->remapping_rule[i].newIP[3]  );
			currentNewIPs[i].setM_value( string( temp ) );
			pendingNewIPs[i].setM_value( currentNewIPs[i].getM_value() );	

			sprintf( temp, "%d.%d.%d.%d",
					 IPRemapReply->remapping_rule[i].mask[0],
					 IPRemapReply->remapping_rule[i].mask[1],
					 IPRemapReply->remapping_rule[i].mask[2],
					 IPRemapReply->remapping_rule[i].mask[3]  );
			currentIPRemapMasks[i].setM_value( string( temp ) );
			pendingIPRemapMasks[i].setM_value( currentIPRemapMasks[i].getM_value() );	

			currentIPRemapTTLs[i].setM_value( string( itoa( ntohs( IPRemapReply->remapping_rule[i].ttl ), temp, 10 ) ) );
			pendingIPRemapTTLs[i].setM_value( currentIPRemapTTLs[i].getM_value() );

//			switch ( ntohs( IPRemapReply->remapping_rule[i].action ) ) {
//				case 0 : currentIPRemapActions[i].setM_value( "No Rule" );
//						 break;	
//				case 1 : currentIPRemapActions[i].setM_value( "Normal" );
//						 break;	
//				case 2 : currentIPRemapActions[i].setM_value( "Forward" );
//						 break;	
//				case 3 : currentIPRemapActions[i].setM_value( "Discard" );
//						 break;	
//			}
			currentIPRemapActions[i].setM_value( string( itoa( ntohs( IPRemapReply->remapping_rule[i].action ), temp, 10 ) ) );
			pendingIPRemapActions[i].setM_value( currentIPRemapActions[i].getM_value() );
		}

		success = true;

	}
	
	return success;
}


bool SPlusBase::readCipherSaber()
{
	bool success = false;
    CipherSaberMSG CipherSaberRequest;
//	int i;
	unsigned char receive_buffer[1500];
	CipherSaberMSG *CipherSaberReply = (CipherSaberMSG *)receive_buffer;
	char temp[20];



	memset( (void *)&CipherSaberRequest, 0, sizeof( CipherSaberRequest ) );
	memset( (void *)receive_buffer, 0, 1500 );


	CipherSaberRequest.Opcode = 0x00;
	srand( time(NULL) );
	CipherSaberRequest.Random = rand();

	digitalsignature( &m_sec, (uint8_t *)(&CipherSaberRequest), sizeof( CipherSaberRequest ) - 20 );

	memcpy( CipherSaberRequest.Signature, m_sec.digest, 20 );
			
	if ( getConfig( I_CIPHER_SABER_PORT, (unsigned char *)&CipherSaberRequest, receive_buffer, sizeof( CipherSaberRequest ), sizeof( CipherSaberMSG ) ) ) {

//		setCurrentParameter( CIPHER_SABER_ENABLE, string( itoa( ntohs( CipherSaberReply->Enable ), temp, 10 ) ) ) ;			
		setCurrentParameter( CIPHER_SABER_ITERATIONS, string( itoa( ntohs( CipherSaberReply->Iterations ), temp, 10 ) ) ) ;			
		setCurrentParameter( CIPHER_SABER_KEY_FILE, "" ) ;			

		success = true;

	}
	
	return success;
}



bool SPlusBase::setIndexedParameter( ReceiverParameter ParameterName, int index, string ParameterValue )
{
	bool success = false;

	
	switch ( ParameterName ) {

		case PID					:	if ( index < NUMBER_OF_PIDS ) {

											pendingPIDs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case RAW_FORWARD_FLAG		:	if ( index < NUMBER_OF_PIDS ) {

											pendingRawForwardFlags[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case DESTINATION_MASK		:	if ( index < NUMBER_OF_PIDS ) {

											pendingDestinationMasks[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case PID_DESTINATION_IP		:	if ( index < NUMBER_OF_DESTINATIONS ) {

											pendingPIDDestinationIPs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case PID_DESTINATION_PORT	:	if ( index < NUMBER_OF_DESTINATIONS ) {

											pendingPIDDestinationPorts[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case PROGRAM_NUMBER			:	if ( index < NUMBER_OF_PATS ) {

											pendingProgramNumbers[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case PMT_PID				:	if ( index < NUMBER_OF_PATS ) {

											pendingPMTPIDs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case CA_PROGRAM_NUMBER		:	if ( index < NUMBER_OF_PROGRAMS ) {

											pendingCAProgramNumbers[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case CA_PROGRAM_STATUS		:	if ( index < NUMBER_OF_PROGRAMS ) {

											CAProgramStatus[index].setM_value( ParameterValue );

											success = true;

										}

										break;

		case FIXED_KEY_PID			:	if ( index < NUMBER_OF_FIXED_KEYS ) {

											pendingFKPIDs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

		case FIXED_KEY_SIGN			:	if ( index < NUMBER_OF_FIXED_KEYS ) {

											pendingFKSigns[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case FIXED_KEY				:	if ( index < NUMBER_OF_FIXED_KEYS ) {

											pendingFKs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case ORIGINAL_IP			:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											pendingOriginalIPs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case NEW_IP					:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											pendingNewIPs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case IP_REMAP_MASK			:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											pendingIPRemapMasks[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case IP_REMAP_TTL			:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											pendingIPRemapTTLs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case IP_REMAP_ACTION		:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											pendingIPRemapActions[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		default						:	success = false;
						
										break;

	}

	return success;
}



ParameterValue& SPlusBase::getIndexedParameter(	ReceiverParameter ParameterName, 
											int index, 
											ParameterSet parameter_set  )
{
	switch( ParameterName ) {

		case PID					:	if ( index < NUMBER_OF_PIDS ) {

											if ( parameter_set == CURRENT ) {

												return currentPIDs[index];

											} else {

												if ( pendingPIDs[index].isSet() ) {

													return pendingPIDs[index];

												} else {

													return currentPIDs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case RAW_FORWARD_FLAG		:	if ( index < NUMBER_OF_PIDS ) {

											if ( parameter_set == CURRENT ) {

												return currentRawForwardFlags[index];

											} else {

												if ( pendingRawForwardFlags[index].isSet() ) {

													return pendingRawForwardFlags[index];

												} else {

													return currentRawForwardFlags[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case DESTINATION_MASK		:	if ( index < NUMBER_OF_PIDS ) {

											if ( parameter_set == CURRENT ) {

												return currentDestinationMasks[index];

											} else {

												if ( pendingDestinationMasks[index].isSet() ) {

													return pendingDestinationMasks[index];

												} else {

													return currentDestinationMasks[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case PID_DESTINATION_IP		:	if ( index < NUMBER_OF_DESTINATIONS ) {

											if ( parameter_set == CURRENT ) {

												return currentPIDDestinationIPs[index];

											} else {

												if ( pendingPIDDestinationIPs[index].isSet() ) {

													return pendingPIDDestinationIPs[index];

												} else {

													return currentPIDDestinationIPs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case PID_DESTINATION_PORT	:	if ( index < NUMBER_OF_DESTINATIONS ) {

											if ( parameter_set == CURRENT ) {

												return currentPIDDestinationPorts[index];

											} else {

												if ( pendingPIDDestinationPorts[index].isSet() ) {

													return pendingPIDDestinationPorts[index];

												} else {

													return currentPIDDestinationPorts[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case PROGRAM_NUMBER			:	if ( index < NUMBER_OF_PATS ) {

											if ( parameter_set == CURRENT ) {

												return currentProgramNumbers[index];

											} else {

												if ( pendingProgramNumbers[index].isSet() ) {

													return pendingProgramNumbers[index];

												} else {

													return currentProgramNumbers[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case PMT_PID				:	if ( index < NUMBER_OF_PATS ) {

											if ( parameter_set == CURRENT ) {

												return currentPMTPIDs[index];

											} else {

												if ( pendingPMTPIDs[index].isSet() ) {

													return pendingPMTPIDs[index];

												} else {

													return currentPMTPIDs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case CA_PROGRAM_NUMBER		:	if ( index < NUMBER_OF_PROGRAMS ) {

											if ( parameter_set == CURRENT ) {

												return currentCAProgramNumbers[index];

											} else {

												if ( pendingCAProgramNumbers[index].isSet() ) {

													return pendingCAProgramNumbers[index];

												} else {

													return currentCAProgramNumbers[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case CA_PROGRAM_STATUS		:	if ( index < NUMBER_OF_PROGRAMS ) {

											return CAProgramStatus[index];
						
										} else {

											return nullParameter;

										}

		case FIXED_KEY_PID			:	if ( index < NUMBER_OF_FIXED_KEYS ) {

											if ( parameter_set == CURRENT ) {

												return currentFKPIDs[index];

											} else {

												if ( pendingFKPIDs[index].isSet() ) {

													return pendingFKPIDs[index];

												} else {

													return currentFKPIDs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case FIXED_KEY_SIGN			:	if ( index < NUMBER_OF_FIXED_KEYS ) {

											if ( parameter_set == CURRENT ) {

												return currentFKSigns[index];

											} else {

												if ( pendingFKSigns[index].isSet() ) {

													return pendingFKSigns[index];

												} else {

													return currentFKSigns[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case FIXED_KEY				:	if ( index < NUMBER_OF_FIXED_KEYS ) {

											if ( parameter_set == CURRENT ) {

												return currentFKs[index];

											} else {

												if ( pendingFKs[index].isSet() ) {

													return pendingFKs[index];

												} else {

													return currentFKs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case ORIGINAL_IP			:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											if ( parameter_set == CURRENT ) {

												return currentOriginalIPs[index];

											} else {

												if ( pendingOriginalIPs[index].isSet() ) {

													return pendingOriginalIPs[index];

												} else {

													return currentOriginalIPs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case NEW_IP				:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											if ( parameter_set == CURRENT ) {

												return currentNewIPs[index];

											} else {

												if ( pendingNewIPs[index].isSet() ) {

													return pendingNewIPs[index];

												} else {

													return currentNewIPs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case IP_REMAP_MASK			:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											if ( parameter_set == CURRENT ) {

												return currentIPRemapMasks[index];

											} else {

												if ( pendingIPRemapMasks[index].isSet() ) {

													return pendingIPRemapMasks[index];

												} else {

													return currentIPRemapMasks[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case IP_REMAP_TTL			:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											if ( parameter_set == CURRENT ) {

												return currentIPRemapTTLs[index];

											} else {

												if ( pendingIPRemapTTLs[index].isSet() ) {

													return pendingIPRemapTTLs[index];

												} else {

													return currentIPRemapTTLs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case IP_REMAP_ACTION		:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											if ( parameter_set == CURRENT ) {

												return currentIPRemapActions[index];

											} else {

												if ( pendingIPRemapActions[index].isSet() ) {

													return pendingIPRemapActions[index];

												} else {

													return currentIPRemapActions[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		default		:	return nullParameter;

	}

}



void SPlusBase::cancelIndexedParameters()
{
	int i;


	for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {

		if ( currentPIDs[i].isSet() ) {

			pendingPIDs[i].setM_value( currentPIDs[i].getM_value() );

		} else {

			pendingPIDs[i].setM_isSet( false );

		}

		if ( currentRawForwardFlags[i].isSet() ) {

			pendingRawForwardFlags[i].setM_value( currentRawForwardFlags[i].getM_value() );

		} else {

			pendingRawForwardFlags[i].setM_isSet( false );

		}

		if ( currentDestinationMasks[i].isSet() ) {

			pendingDestinationMasks[i].setM_value( currentDestinationMasks[i].getM_value() );

		} else {

			pendingDestinationMasks[i].setM_isSet( false );

		}

	}

	for ( i = 0; i < NUMBER_OF_DESTINATIONS; i++ ) {

		if ( currentPIDDestinationIPs[i].isSet() ) {

			pendingPIDDestinationIPs[i].setM_value( currentPIDDestinationIPs[i].getM_value() );

		} else {

			pendingPIDDestinationIPs[i].setM_isSet( false );

		}

		if ( currentPIDDestinationPorts[i].isSet() ) {

			pendingPIDDestinationPorts[i].setM_value( currentPIDDestinationPorts[i].getM_value() );

		} else {

			pendingPIDDestinationPorts[i].setM_isSet( false );

		}

	}

	for ( i = 0; i < NUMBER_OF_PATS; i++ ) {

		if ( currentProgramNumbers[i].isSet() ) {

			pendingProgramNumbers[i].setM_value( currentProgramNumbers[i].getM_value() );

		} else {

			pendingProgramNumbers[i].setM_isSet( false );

		}


		if ( currentPMTPIDs[i].isSet() ) {

			pendingPMTPIDs[i].setM_value( currentPMTPIDs[i].getM_value() );

		} else {

			pendingPMTPIDs[i].setM_isSet( false );

		}

	}

	for ( i = 0; i < NUMBER_OF_PROGRAMS; i++ ) {

		if ( currentCAProgramNumbers[i].isSet() ) {

			pendingCAProgramNumbers[i].setM_value( currentCAProgramNumbers[i].getM_value() );

		} else {

			pendingCAProgramNumbers[i].setM_isSet( false );

		}

	}

	for ( i = 0; i < NUMBER_OF_FIXED_KEYS; i++ ) {

		if ( currentFKPIDs[i].isSet() ) {

			pendingFKPIDs[i].setM_value( currentFKPIDs[i].getM_value() );

		} else {

			pendingFKPIDs[i].setM_isSet( false );

		}

		if ( currentFKSigns[i].isSet() ) {

			pendingFKSigns[i].setM_value( currentFKSigns[i].getM_value() );

		} else {

			pendingFKSigns[i].setM_isSet( false );

		}

		if ( currentFKs[i].isSet() ) {

			pendingFKs[i].setM_value( currentFKs[i].getM_value() );

		} else {

			pendingFKs[i].setM_isSet( false );

		}


	}

	for ( i = 0; i < NUMBER_OF_IP_REMAP_RULES; i++ ) {

		if ( currentOriginalIPs[i].isSet() ) {

			pendingOriginalIPs[i].setM_value( currentOriginalIPs[i].getM_value() );

		} else {

			pendingOriginalIPs[i].setM_isSet( false );

		}

		if ( currentNewIPs[i].isSet() ) {

			pendingNewIPs[i].setM_value( currentNewIPs[i].getM_value() );

		} else {

			pendingNewIPs[i].setM_isSet( false );

		}

		if ( currentIPRemapMasks[i].isSet() ) {

			pendingIPRemapMasks[i].setM_value( currentIPRemapMasks[i].getM_value() );

		} else {

			pendingIPRemapMasks[i].setM_isSet( false );

		}

		if ( currentIPRemapTTLs[i].isSet() ) {

			pendingIPRemapTTLs[i].setM_value( currentIPRemapTTLs[i].getM_value() );

		} else {

			pendingIPRemapTTLs[i].setM_isSet( false );

		}

		if ( currentIPRemapActions[i].isSet() ) {

			pendingIPRemapActions[i].setM_value( currentIPRemapActions[i].getM_value() );

		} else {

			pendingIPRemapActions[i].setM_isSet( false );

		}

	}

}


void SPlusBase::clearIndexedParameters()
{
	int i;


	for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {

		pendingPIDs[i].setM_value( "8191" );
		pendingRawForwardFlags[i].setM_value( "0" );
		pendingDestinationMasks[i].setM_value( "0" );

	}

	for ( i = 0; i < NUMBER_OF_DESTINATIONS; i++ ) {

		pendingPIDDestinationIPs[i].setM_value( "0.0.0.0" );
		pendingPIDDestinationPorts[i].setM_value( "0" );

	}

	for ( i = 0; i < NUMBER_OF_PATS; i++ ) {

		pendingProgramNumbers[i].setM_value( "0" );
		pendingPMTPIDs[i].setM_value( "0" );

	}

	for ( i = 0; i < NUMBER_OF_PROGRAMS; i++ ) {

		pendingCAProgramNumbers[i].setM_value( "0" );

	}

	for ( i = 0; i < NUMBER_OF_FIXED_KEYS; i++ ) {

		pendingFKPIDs[i].setM_value( "8191" );
		pendingFKSigns[i].setM_value( "0" );
		pendingFKs[i].setM_value( "0000000000000000" );

	}

	for ( i = 0; i < NUMBER_OF_IP_REMAP_RULES; i++ ) {

		pendingOriginalIPs[i].setM_value( "0.0.0.0" );
		pendingNewIPs[i].setM_value( "0.0.0.0" );
		pendingIPRemapMasks[i].setM_value( "0.0.0.0" );
		pendingIPRemapTTLs[i].setM_value( "0" );
		pendingIPRemapActions[i].setM_value( "No Action" );

	}

}



bool SPlusBase::changePassword( char *password, unsigned char key[16] )
{
	SPlusSetPasswordMSG password_message;
	int i;
	string mac;
	int mac_byte[6];
	char junk[2];


	mac = getParameter( RECEIVER_MAC ).asString();
	sscanf( mac.c_str(), "%2x%c%2x%c%2x%c%2x%c%2x%c%2x",
			&(mac_byte[0]), junk, &(mac_byte[1]), junk, &(mac_byte[2]),
			junk, &(mac_byte[3]), junk, &(mac_byte[4]), junk, &(mac_byte[5]) );
	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)mac_byte[i];
	}
	
	memset( m_sec.password, 0, 8 );
	for ( i = 0; ( i < 8 ) && ( i < (int)strlen( password ) ); i++ ) {
		m_sec.password[i] = password[i];
	}

	// MAC address to msg buffer
	memmove( password_message.newPassword, m_sec.macaddress, sizeof( m_sec.macaddress ) );

	// pad MAC address to 8-bytes
	password_message.newPassword[6] = 0;
	password_message.newPassword[7] = 0;

	// copy password to message buffer
	memmove(&(password_message.newPassword[8]), m_sec.password, 8 );

	srand( time(NULL) );
	password_message.Random = rand();
	
	// compute digital signature on clear text
	// password and MAC address
	digitalsignature(&m_sec, (unsigned char *)(&password_message), sizeof( SPlusSetPasswordMSG ) - 20 );
	memmove( password_message.Signature, m_sec.digest, sizeof(m_sec.digest));

	// encrypt MAC address
	encrypt(key, &(password_message.newPassword[0]));

	// encrypt password
	encrypt(key, &(password_message.newPassword[8]));

	return sendConfig( I_SET_PASSWORD_PORT, (unsigned char *)&password_message, sizeof( password_message ) );
}
