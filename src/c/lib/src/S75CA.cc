// S75CA.cpp: implementation of the CS75CA class.
//
//////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "stdafx.h"
#include "S75CA.h"
#include "S75CAStatus.h"
#include "ProgramList.h"

#ifdef LINUX
	extern void digitalsignature(pstruct *p, uint8_t *text, uint32_t len);
#endif

	const int PID_CONFIG_PORT = 0x1977;
	const int RESET_PORT = 0x9999;
	const int CAM_RESET_PORT = 0x9998;
	const int CAM_WATCHDOG_TIMEOUT_CONFIG_PORT = 0x9997;
	const int PROGRAM_CONFIG_PORT = 0x1958;
	const int PAT_CONFIG_PORT = 0x1952;
	const int FK_CONFIG_PORT = 0x2022;
	const int TRAP_CONFIG_PORT = 0x2005;
	const int PASSWORD_CHANGE_PORT = 0x2010;
	const int STATUS_POLL_PORT = 0x2011;
	const int RF_CONFIG_PORT = 0x1983;

	const int LNB_HIGH = 1;                         // Possible values for 'band' parameter in TuneS75D() function
	const int LNB_LOW = 0;

	const int LNB_HORIZONTAL = 1;                   // Possible values for 'polarity' parameter in TuneS75D() function
	const int LNB_LEFT = 1;
	const int LNB_VERTICAL = 0;
	const int LNB_RIGHT = 0;
	const int	MAX_SUPP_PIDS = 32;
	const int	MAX_SUPP_DESTINATIONS = 16;
	const int	MAX_SUPP_PROGRAMS = 16;

// CONSTANTS for converting AGC to percentage

	const double  AGC[]        = { 254, 112, 105, 85 };  // Automatic Gain Control
	const double  PERCENTAGE[] = { 10, 65, 80, 100 };
	const int     SIZE_ARRAY   = 4;




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS75CA::CS75CA( bool remote )
{
	m_StatusPacketSize = sizeof(S75CAStatus_MSG);
	m_Device_Type = S75CA;  // when we instantiate this class, the device type will be enforced on attachment.
	m_StatusWrapper = (StatusPacket *)(new S75CAStatusPacket( &m_Status ));
	m_pStatusBuffer = (LPBYTE) &m_Status;	// Initialize the Member variable of the parent.
	m_remote = remote;
#ifdef BROADCAST_POLL_FOR_STATUS
	m_broadcast_send_socket = 0;
	m_broadcast_receive_socket = 0;
	m_broadcast_linked = false;
	m_usingStatusListener = false;
#else
	m_usingStatusListener = !remote;
#endif
}


CS75CA::~CS75CA()
{

#ifdef BROADCAST_POLL_FOR_STATUS
	if ( m_broadcast_receive_socket != 0 ) {

#ifndef LINUX
       	closesocket( m_broadcast_receive_socket );
#else
       	close( m_broadcast_receive_socket );
#endif

	}

	if ( m_broadcast_send_socket != 0 ) {

#ifndef LINUX
       	closesocket( m_broadcast_send_socket );
#else
       	close( m_broadcast_send_socket );
#endif

	}
#endif
}


bool CS75CA::ParseStatusPacket()
{
	BYTE temp[4];

	// If this function is called, the status Structure has been filled with data received by Status Listner.
	// We need to verify that the packet is valid, and that it matches the device to be valid.

	if ( !CNovraDevice::ParseStatusPacket() ) return false;

	((S75CAStatusPacket *)m_StatusWrapper)->ntoh();

//	for(i=0;i<16;i++)
//		(((S75CAStatusPacket *)m_StatusWrapper)->ProgramsPointer())[i] = ntohs((((S75CAStatusPacket *)m_StatusWrapper)->ProgramsPointer())[i]);

//	for(int i=0;i<MAX_SUPP_PIDS;i++)
//		(((S75CAStatusPacket *)m_StatusWrapper)->PIDListPointer())[i] = ntohs((((S75CAStatusPacket *)m_StatusWrapper)->PIDListPointer())[i]);

//	if( (((*(((S75CAStatusPacket *)m_StatusWrapper)->DSPFirmWarePointer())) & 0xFF00) != 0x3400) && (((*(((S75CAStatusPacket *)m_StatusWrapper)->DSPFirmWarePointer())) & 0xFF00) != 0x5400))	// Both 2M and New Hardware 
	if( ( ((*(((S75CAStatusPacket *)m_StatusWrapper)->DSPFirmWarePointer())) & 0xFF00) != 0x6700 )  &&
		( ((*(((S75CAStatusPacket *)m_StatusWrapper)->DSPFirmWarePointer())) & 0xFF00) != 0x7600 )  )
	 return false ;		/// if the device does not follow communication rev 3.0 it is not supported.

	if(m_Device_Type != S75CA) return false;

	m_StatusListener.GetLastPacketSource(temp);

	memcpy(m_Device_IP_Address, temp,4); // Get the address from the Unit.

	return true;
}


int CS75CA::GetProgramTable(LPBYTE ProgramTable)
{
//	WORD opcode;                    // Opcode to send to S75
	int temp1 = 1000;               // Temporary storage variable
//	int count = 0 ;                 // Temporary storage variable
//	DWORD xx = 1;                   // Temporary storage variable
	SOCKET s;
	struct sockaddr_in device_address;
	fd_set readfs;
	struct timeval timeout;
	int result;
	bool success=false;
    BYTE Buffer2[sizeof(S75CAProgramResponse_MSG)];
	int i;
	S75CAProgramResponse_MSG programResponseMSG;
	int last_error_code ;
	TCHAR temp[16];
	S75CAProgramSettings_MSG programRequest;
//

	m_pS75CA_ProgramList = (S75CA_ProgramList *) ProgramTable ;

	  // Convert the IP address to a string.
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);

	memset( (void *)&programResponseMSG, 0, sizeof( programResponseMSG ) );
	memset( (void *)&programRequest, 0, sizeof( programRequest ) );

	// Set the opcode to send to the S75 to download (0x0001)

	programRequest.Opcode = htons(0x0001);
	srand( time(NULL) );
	programRequest.Random = rand();

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	digitalsignature( &m_sec, (uint8_t *)(&programRequest), sizeof( S75CAProgramSettings_MSG ) - 20 );

	memcpy( programRequest.Signature, m_sec.digest, 20 );


	// Create a socket to communicate with the S75

#ifndef LINUX
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) {
#else
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) {
#endif


        last_error_code = ERR_PROGRAM_SOCKET_CREATE;

	} else {

        // Connect the newly created socket to the S75.

    	device_address.sin_family = AF_INET;

    	device_address.sin_port = htons( PROGRAM_CONFIG_PORT );

    	device_address.sin_addr.s_addr = inet_addr( temp);

    	if ( connect( s, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr_in ) ) != 0){

    		last_error_code = ERR_PROGRAM_SOCKET_CONNECT;

		} else {

            // Create a buffer the size of the Program Table to hold the
            // response we receive back.

			
            // Download the Program table.  Retry (if required) up to 4 times

            for (i = 0; (!success) && ( i < NUM_RETRIES ); i++) {

            	// Send the Program reqest to the S75

                if ( send( s, (const char *)(&programRequest), sizeof( S75CAProgramSettings_MSG), 0 ) != sizeof( S75CAProgramSettings_MSG ) ) {

                    last_error_code = ERR_PROGRAM_SEND;

				} else {

#ifndef LINUX
	                Sleep(30);
#endif

	                timeout.tv_sec = 1;

	                timeout.tv_usec = 0;

        	        FD_ZERO( &readfs );

        	        FD_SET( s, &readfs );

                    // Try to receive the response from S75.  Check for errors.

#ifndef LINUX
                    if ( ( result = select( 1, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#else
                    if ( ( result = select( s+1, &readfs, NULL, NULL, &timeout ) ) != -1 ) {
#endif
	        	        if ( result != 0 ) {

#ifndef LINUX
    		    	        if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75CAProgramResponse_MSG), 0 ) ) == SOCKET_ERROR ) {

	            	    		temp1 = GetLastError();

	                	        if(temp1 != WSAETIMEDOUT){

	        	        			last_error_code = ERR_PROGRAM_RECEIVE;

								} // if


#else
    		    	        if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75CAProgramResponse_MSG), 0 ) ) == -1 ) {
					last_error_code = ERR_PROGRAM_RECEIVE;
#endif

							} else {

		    	    	        // Make sure we have received the right number of bytes back

	    		    	        if (temp1 == sizeof(S75CAProgramResponse_MSG)) {

	            		    		memcpy(&programResponseMSG, Buffer2, sizeof(S75CAProgramResponse_MSG));

									// Go through the Program table and change all the Programs and
    				                // status words from network order to host order.
					                for ( i = 0; i < MAX_SUPP_PROGRAMS; i++) {
   						                m_pS75CA_ProgramList->ProgramNumber[i] = ntohs(programResponseMSG.Program[i]);
										m_pS75CA_ProgramList->ProgramStatus[i] = ntohs(programResponseMSG.Status[i]);
									}

					                last_error_code = ERR_SUCCESS;

									success = true;

								} else {
                                    last_error_code = ERR_PROGRAM_ACK_BAD;

								} // if

							} // if recv

						} else {

							last_error_code = ERR_PROGRAM_RECEIVE;

						} // if select result 0

					} else {

						last_error_code = ERR_PROGRAM_RECEIVE;

					} // if select

				} // if send

			} // for 4 retires



		} // if socket connect

#ifndef LINUX
        closesocket( s );
#else
        close( s );
#endif

	} // if socket create

	return last_error_code;
}


int CS75CA::SetProgramTable(LPBYTE ProgramTable)
{
	int i;

	int last_error_code ;

	S75CAProgramSettings_MSG ProgramSettingMsg;

	/* bool	ProgramList = false ; */

	m_pS75CA_ProgramList = (S75CA_ProgramList *) ProgramTable ;
	m_ProgramList = new Program_List( (void *)ProgramTable );


    // Set the opcode in the Program table to Upload (0x0002)

    ProgramSettingMsg.Opcode = htons(0x0002);

    // Copy the Program table to download into the CPIDProgram object
	// Does The Program list have too many entries !!? 

//	if(m_pS75D_ProgramList->NumberofPrograms > MAX_SUPP_PROGRAMS) return ERR_PROGRAM_TOOLONG ;

    for (i = 0; i < MAX_SUPP_PROGRAMS; i++) {

		ProgramSettingMsg.Program[i] = htons( m_pS75CA_ProgramList->ProgramNumber[i] );

    } // for i

	srand( time(NULL) );
	ProgramSettingMsg.Random = rand();

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	digitalsignature( &m_sec, (uint8_t *)(&ProgramSettingMsg), sizeof( ProgramSettingMsg ) - 20 );

	
	memcpy( ProgramSettingMsg.Signature, m_sec.digest, 20 );

	last_error_code = SendPrograms( &ProgramSettingMsg, sizeof( S75CAProgramSettings_MSG ), MAX_SUPP_PROGRAMS );

	delete m_ProgramList;

    return last_error_code;

}


int CS75CA::SendPrograms( S75CAProgramSettings_MSG  *ProgramSettingMsg, int size, int max_programs )
{
	bool success = false;
	TCHAR  temp[16];
	SOCKET s;
	int last_error_code ;
	struct sockaddr_in device_address;
	int i;
	struct timeval timeout;
	fd_set readfs;
	int result;
	int temp1 = 1000;               // Temporary storage variable
	bool configurationApplied;         
    bool ProgramInList;                     // Is the Program we are looking at in the current list?
	int j;


	  // Convert the IP address to a string.
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);

    // Create a socket to communicate with the S75


#ifndef LINUX
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) {
#else
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) {
#endif
		last_error_code = ERR_PROGRAM_SOCKET_CREATE;

	} else {

        // Connect the newly created socket to the S75.

    	device_address.sin_family = AF_INET;

    	device_address.sin_port = htons( PROGRAM_CONFIG_PORT );

    	device_address.sin_addr.s_addr = inet_addr( temp );

    	if ( connect( s, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr_in ) ) != 0){

            last_error_code = ERR_PROGRAM_SOCKET_CONNECT;

		} else {

            // Create a buffer to hold the data we wish to send

            BYTE *Buffer1=NULL;
			Buffer1 = new BYTE[size];

            memcpy(Buffer1, ProgramSettingMsg, size); // copy the data in the buffer.
			 

            // Create a buffer the size of the Program Table to hold the
            // response we receive back.

			BYTE *Buffer2=NULL;
			Buffer2 = new BYTE[size];

            for (i = 0; (!success) && ( i < NUM_RETRIES ); i++) {

    			// Send the Program table to the S75

                if ( send( s, (const char *)Buffer1, size, 0 ) != size ) {

					last_error_code = ERR_PROGRAM_SEND;		
		
				} // if
	
#ifndef LINUX
    	        Sleep(30);
#endif

                // Set the socket options so that we now have a blocking socket.  Then, 
                // wait for a response packet or for a timeout.

    	        timeout.tv_sec = 1;

    	        timeout.tv_usec = 0;

    	        FD_ZERO( &readfs );

    	        FD_SET( s, &readfs );

                // Try to receive the response from S75.  Compare the response (if
                // received) to the values sent to see if they are the same.
#ifndef LINUX
                if ( ( result = select( 1, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#else
                if ( ( result = select( s+1, &readfs, NULL, NULL, &timeout ) ) != -1 ) {
#endif

    	        	if ( result != 0 ) {

#ifndef LINUX

        		    	if( (temp1 = recv( s, (char *)Buffer2, size, 0 ) ) == SOCKET_ERROR ) {
	        		    	temp1 = GetLastError();
		            		if(temp1 != WSAETIMEDOUT){
			            		 last_error_code = ERR_PROGRAM_RECEIVE;
					} // if

#else
        		    	if( (temp1 = recv( s, (char *)Buffer2, size, 0 ) ) == -1 ) {
					 last_error_code = ERR_PROGRAM_RECEIVE;

#endif

						}  // if

    			        // Check to make sure the data received back is the same data sent.

    			        if(temp1 ==  size){

							success = true;

	    	        		for(i = 0 ; i < size; i++ ){

		    	        		if(Buffer1[i] != Buffer2[i]) {

		    		        		last_error_code = ERR_PROGRAM_ACK_BAD;

									success = false;

								} // if

							} // for

				            last_error_code = ERR_SUCCESS;
	
						} // if

					} // if select result 0

				} // if select

			} // for retries

			delete [] Buffer1;
			delete [] Buffer2;

		} //.if socket connect

#ifndef LINUX
	  	closesocket( s );
#else
	  	close( s );
#endif

	} // if socket create

    // Check if we were able to send the configuration successfully
    if (success) {
        // Configuration sent to S75D

/*
		if ( m_usingStatusListener && ( GetDeviceType() != S75CA ) ) {

			// Flush the status packet buffer
			if ((last_error_code =FlushStatusBuffer(STATUS_PACKET_TIMEOUT)) !=ERR_SUCCESS ) {
				// Error flushing buffer
				// Don't need to define error code -- done by FlushStatusBuffer()
				return last_error_code;
			}
		    if ((last_error_code =FlushStatusBuffer(STATUS_PACKET_TIMEOUT)) !=ERR_SUCCESS ) {
				// Error flushing buffer
				// Don't need to define error code -- done by FlushStatusBuffer()
				return last_error_code;
			}

			// Wait for the next status packet
			if ((last_error_code = WaitForStatusPacket(STATUS_PACKET_TIMEOUT)) != ERR_SUCCESS) {
				// Error when waiting for status packet
				// Don't need to define error code -- done by WaitForStatusPacket()
				return last_error_code;
			}

		} else {
*/
		if ( ( last_error_code = PollStatus( m_StatusWrapper ) ) != ERR_SUCCESS ) {
				return last_error_code;
			}
//		}

        // Check to see if the values sent to the S75D are the same as those being
        // reported in the status


		// Check the Program list with the one we just downloaded.  Note that the 
		// Program list we just downloaded could be in a different order from ours.


		configurationApplied = true;


        // Go through the Program list downloaded and check to make sure that each Program we
        // uploaded is somewhere in that list.

		for (i = 0; i < max_programs; i++) {

			ProgramInList = false;


	        if (ProgramSettingMsg->Program[i] != 0) {     // Skip Program 0

		        for (j = 0; j < max_programs; j++) {

					if ( (((S75CAStatusPacket *)m_StatusWrapper)->ProgramListPointer())[j] == ntohs(ProgramSettingMsg->Program[i]) ) { // @@NS@@ Note that it was already inverted in prep !!!
						ProgramInList = true;
	                    break;
		            }

		        }

			} else {

				ProgramInList = true;

	        }

			if (!ProgramInList) {
				configurationApplied = false;

			}

		}

	    if (!configurationApplied) {

			// Configuration not successfully applied

			last_error_code = ERR_PROGRAMSET_NOTAPPLIED;

			success = false;

		} else {

			// Configuration successfully applied

			last_error_code = ERR_SUCCESS;

			success = true;

		}

	}

	return last_error_code;

}


int CS75CA::PollStatus( StatusPacket *status_packet_wrapper, int timeout )
{
	S75CA_One_Time_Status_Request_MSG message;
	TCHAR temp[16];
	SOCKET s;
	struct sockaddr_in device_address;
	int last_error_code ;
	bool success=false;
	struct timeval timeout_struct;
	fd_set readfs;
	int result;
	int temp1 = 1000;               // Temporary storage variable
	int i;
//	BYTE Buffer[sizeof(S75JLEO_Status_MSG)];
//	S75JLEO_Status_MSG *status = NULL;
	int use_again;
	/* S75CA_Status *response = NULL; */
	short firmware_version = 0x0000;
//	int     BERmantissa, BERexponent;   // Bit Error Rate mantissa and exponent

	S75CAStatusPacket *s75ca_status_packet_wrapper = (S75CAStatusPacket *)status_packet_wrapper;

#ifndef LINUX
	Sleep( 600 );   // To allow fresh status packet to become available on receiver and to prevent flooding devices with poll requests
#endif
#ifdef LINUX
        usleep( 600000 );
#endif


	message.Message[0] = 0x47;
	message.Message[1] = 0x47;
	message.Message[2] = 0x48;
	message.Message[3] = 0x48;
	message.Message[4] = 0x49;
	message.Message[5] = 0x49;

	sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);

	// Create a socket to communicate with the S75

	sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);

#ifndef LINUX
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) {
#else
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) {
#endif


        last_error_code = ERR_POLL_STATUS_SOCKET_CREATE;

	} else {

		setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (const char *)(&use_again), sizeof( use_again ) );
        // Connect the newly created socket to the S75.

    	device_address.sin_family = AF_INET;

    	device_address.sin_port = htons( STATUS_POLL_PORT );

    	device_address.sin_addr.s_addr = inet_addr( temp);

    	if ( connect( s, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr ) ) != 0){

    		last_error_code = ERR_POLL_STATUS_CONNECT;

		} else {


            for (i = 0; (!success) && ( i < NUM_RETRIES ); i++) {

                if ( send( s, (const char *)(&message), sizeof( S75CA_One_Time_Status_Request_MSG), 0 ) != sizeof( S75CA_One_Time_Status_Request_MSG ) ) {
                    last_error_code = ERR_STATUS_POLL_SEND;

				} else {
#ifndef LINUX
	                Sleep(30);
#endif

	                timeout_struct.tv_sec = timeout/1000;

	                timeout_struct.tv_usec = timeout % 1000;

        	        FD_ZERO( &readfs );

        	        FD_SET( s, &readfs );

                    // Try to receive the response from S75.  Check for errors.

#ifndef LINUX
                    if ( ( result = select( 1, &readfs, NULL, NULL, &timeout_struct ) ) != SOCKET_ERROR ) {
#else
                    if ( ( result = select( s+1, &readfs, NULL, NULL, &timeout_struct ) ) != -1 ) {
#endif
	        	        if ( result != 0 ) {
#ifndef LINUX
    		    	        if( (temp1 = recv( s, (char *)(status_packet_wrapper->data()), sizeof(S75CAStatus_MSG), 0 ) ) == SOCKET_ERROR ) {

	            	    		temp1 = GetLastError();

	                	        if(temp1 != WSAETIMEDOUT){

	        	        			last_error_code = ERR_STATUS_POLL;

								} // if


#else
    		    	        if( (temp1 = recv( s, (char *)(status_packet_wrapper->data()), sizeof(S75CAStatus_MSG), 0 ) ) == -1 ) {
								last_error_code = ERR_STATUS_POLL;
#endif

							
							} else {

		    					// Make sure we have received the right number of bytes back

				    		    if (temp1 == sizeof(S75CAStatus_MSG)) {

									success = true;

									if(m_MACAddressValid){
										if(memcmp(status_packet_wrapper->ReceiverMACPointer(), m_Device_MAC_Address,6)!=0) {

											last_error_code = ERR_WRONG_RECEIVER;
											success = false;
										}
									}

									firmware_version = ntohs((*(status_packet_wrapper->DSPFirmWarePointer()))) & 0xFF00;
									if( !(	( firmware_version == (short)0x6700 ) || ( firmware_version == (short)0x7600 ) ) ) {		// Communication rev. JSAT Leo
										success = false;
										last_error_code = ERR_WRONG_RECEIVER;		/// if the device does not follow communication rev JSAT Leo it is not supported.
									}

									if(m_Device_Type != S75CA) {
										success = false;
										last_error_code = ERR_DEVICE_TYPE_MISMATCH;	

									}
						
									if ( success ) {

										s75ca_status_packet_wrapper->ntoh();

										last_error_code = ERR_SUCCESS;

									}
		
								} else {

									last_error_code = ERR_STATUS_POLL_ACK_BAD;

								} // if

							} // if recv

						} else {

							last_error_code = ERR_STATUS_POLL;

						} // if select result 0

					} else {

						last_error_code = ERR_STATUS_POLL;

					} // if select

				} // if send
	
			} // for 4 retires



		} // if socket connect

#ifndef LINUX
        closesocket( s );
#else
        close( s );
#endif

	} // if socket create

	return last_error_code;

}


int CS75CA::GetStatus(void *Container)
{
	S75Plus_Status * pStatus;
	int i;
	/* WORD temp ; */

	pStatus = (S75Plus_Status *) Container;

	if(!IsStatusValid())  return ERR_FAILED ; // the status isn't valid. 

	if(m_Device_Type != S75CA) return ERR_DEVICE_TYPE_MISMATCH ;

	CSatelliteDevice::GetStatus( (SatelliteStatus *)(&(S75CAStatus( Container ))), true );

	switch(((*(((S75PlusStatusPacket *)m_StatusWrapper)->DSPFirmWarePointer())) & 0xFF00) >> 8){

	case 0x34:		// Older Hardware 
		{
			pStatus->SignalStrength = CalculateSignalStrength( AGC, PERCENTAGE, SIZE_ARRAY );
			break;
		}
	case 0x01:
	case 0x54:	// Newer Hardware 
		{
			pStatus->SignalStrength = (*(((S75PlusStatusPacket *)m_StatusWrapper)->SignalStrengthPointer()));
			break; 
		
		}

	default:

		break;

	}

	pStatus->DemodulatorGain = (*(((S75PlusStatusPacket *)m_StatusWrapper)->DemodulatorGainPointer()));

	// Convert demodulator status flags into something more readable

	pStatus->SignalLock  = (*(((S75PlusStatusPacket *)m_StatusWrapper)->DemodulatorStatusPointer())) & 0x01 ;

	pStatus->DataLock = ((*(((S75PlusStatusPacket *)m_StatusWrapper)->DemodulatorStatusPointer())) & 0x10) >> 4;

	pStatus->LNBFault = ((*(((S75PlusStatusPacket *)m_StatusWrapper)->LNBConfigurationPointer())) & 0x80) >> 7;


	pStatus->VBER = CalculateVBER();

	pStatus->BroadcastStatusPort = (*(((S75PlusStatusPacket *)m_StatusWrapper)->BroadcastStatusPortPointer()));

	pStatus->LocalOscFrequency	= (*(((S75PlusStatusPacket *)m_StatusWrapper)->LocalOscFrequencyPointer()));

	pStatus->Long_Line_Comp = ((*(((S75PlusStatusPacket *)m_StatusWrapper)->LNBConfigurationPointer())) & 0x10 ) >> 4;  // is Long Line Compa ON .

	pStatus->Hi_VoltageMode = ((*(((S75PlusStatusPacket *)m_StatusWrapper)->LNBConfigurationPointer())) & 0x8 ) >> 3;  // is High Voltage On. 

	pStatus->PolaritySwitchingVoltage = ((*(((S75PlusStatusPacket *)m_StatusWrapper)->LNBConfigurationPointer())) & 0x20 ) >> 5;

	pStatus->HiLoBandSwitchingTone = ((*(((S75PlusStatusPacket *)m_StatusWrapper)->LNBConfigurationPointer())) & 0x40 ) >> 6;

	pStatus->CardStatus = (*(((S75PlusStatusPacket *)m_StatusWrapper)->CardStatusPointer()));

	pStatus->AGCA = (*(((S75PlusStatusPacket *)m_StatusWrapper)->SignalStrengthPointer()));

	pStatus->AGCN = (*(((S75PlusStatusPacket *)m_StatusWrapper)->DemodulatorGainPointer())); 

	pStatus->GNYQA = ((*(((S75PlusStatusPacket *)m_StatusWrapper)->Digital_GainPointer())) &0x30) >>4 ;

	pStatus->GFARA = ((*(((S75PlusStatusPacket *)m_StatusWrapper)->Digital_GainPointer())) &0xf) ;

	pStatus->NEST = (*(((S75PlusStatusPacket *)m_StatusWrapper)->NESTPointer()));
	pStatus->Clock_Off = (*(((S75PlusStatusPacket *)m_StatusWrapper)->Clock_OffPointer()));
	pStatus->Freq_Err = (*(((S75PlusStatusPacket *)m_StatusWrapper)->Freq_ErrPointer()));
	pStatus->ADC_MID = (*(((S75PlusStatusPacket *)m_StatusWrapper)->ADC_MIDPointer()));
	pStatus->ADC_CLIP = (*(((S75PlusStatusPacket *)m_StatusWrapper)->ADC_CLIPPointer()));
	pStatus->Digital_Gain = (*(((S75PlusStatusPacket *)m_StatusWrapper)->Digital_GainPointer()));
	pStatus->AA_CLAMP = (*(((S75PlusStatusPacket *)m_StatusWrapper)->AA_CLAMPPointer()));

	pStatus->IGMP_Enable = ((*(((S75PlusStatusPacket *)m_StatusWrapper)->FilterPointer())) & 0x04) >> 2;

	if ( pStatus->ViterbiRate == THREE_QUARTERS ) {
		if ( pStatus->VBER < 1E-8 ) {
			pStatus->CarrierToNoise = 50.0;
		} else
		if ( pStatus->VBER > 1E-2 ) {
			pStatus->CarrierToNoise = 0.0;
		} else {
			pStatus->CarrierToNoise = (float)(4.0 - 0.625 * log10( pStatus->VBER ));
		}
	} else {
		pStatus->CarrierToNoise = 100.0;
	}
	
	i = (*(((S75PlusStatusPacket *)m_StatusWrapper)->RFENDPointer()));
	/*
	pStatus->TotalDVBPacketsAccepted[0] 	= (((S75PlusStatusPacket *)m_StatusWrapper)->TotalDVBPacketsAcceptedPointer())[0];
	pStatus->TotalDVBPacketsAccepted[1] 	= (((S75PlusStatusPacket *)m_StatusWrapper)->TotalDVBPacketsAcceptedPointer())[1];
	pStatus->TotalDVBPacketsRXInError[0] 	= (((S75PlusStatusPacket *)m_StatusWrapper)->TotalDVBPacketsRXInErrorPointer())[0];
	pStatus->TotalDVBPacketsRXInError[1] 	= (((S75PlusStatusPacket *)m_StatusWrapper)->TotalDVBPacketsRXInErrorPointer())[1];
	pStatus->TotalEthernetPacketsOut[0] 	= (((S75PlusStatusPacket *)m_StatusWrapper)->TotalEthernetPacketsOutPointer())[0];
	pStatus->TotalEthernetPacketsOut[1] 	= (((S75PlusStatusPacket *)m_StatusWrapper)->TotalEthernetPacketsOutPointer())[1];
	pStatus->TotalUncorrectableTSPackets[0] = (((S75PlusStatusPacket *)m_StatusWrapper)->TotalUncorrectableTSPacketsPointer())[0];
	pStatus->TotalUncorrectableTSPackets[1] = (((S75PlusStatusPacket *)m_StatusWrapper)->TotalUncorrectableTSPacketsPointer())[1];
*/
	// Copy the PID list
	pStatus->EthernetTransmit = *(((S75PlusStatusPacket *)m_StatusWrapper)->EthernetTXPointer());
	pStatus->DVBScrambled = *(((S75PlusStatusPacket *)m_StatusWrapper)->DVBScrambledPointer());
	pStatus->DVBDescrambled = *(((S75PlusStatusPacket *)m_StatusWrapper)->DVBDescrambledPointer());

	PIDRoute *pid_route;

	for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {

		pid_route = &(((PIDRoute *)(((S75PlusStatusPacket *)m_StatusWrapper)->PIDListPointer()))[i]);

		pStatus->PIDList.Entry[i] = pStatus->PIDRoutes[i].PID = pid_route->PID & 0x1fff;

		pStatus->PIDList.Control[i] = (pid_route->PID & 0xE000) >> 13 ;

		pStatus->PIDRoutes[i].DestinationMask = pid_route->DestinationMask;

	}

	TrimPIDList((LPBYTE)(&(pStatus->PIDList)), MAX_SUPP_PIDS );

	memcpy( pStatus->PIDDestinations, ((S75PlusStatusPacket *)m_StatusWrapper)->PIDDestinationsPointer(), NUMBER_OF_DESTINATIONS * sizeof( PIDDestination ) );
	memcpy( pStatus->ProgramList, ((S75PlusStatusPacket *)m_StatusWrapper)->ProgramListPointer(), NUMBER_OF_PROGRAMS * sizeof( WORD ) );
	memcpy( pStatus->ProgramStatus, ((S75PlusStatusPacket *)m_StatusWrapper)->ProgramStatusPointer(), NUMBER_OF_PROGRAMS * sizeof( WORD ) );

	return ERR_SUCCESS ;

}



int CS75CA::ResetCAM()
{
	BYTE dataBuffer[4] = {0x19, 0x65, 0x04, 0x05};  // Data buffer (data to be sent to S75)
    TCHAR temp[16]; 	// Temporary storage variable
	int LastError;

    // There are two possible methods of communicating -- by IP address or
    // by MAC address.  For the former, make sure the IP address is set.
    // For the later, make sure the network path is set
    if ((!m_Device_attached)&&(!m_NetPathSet)) {
        // Trying to communicate by IP with no IP address -- error
         return ERR_RESET_NONETPATH;
	}

    // Destination information properly set
    
    // Convert the IP address to a string
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);


    // Send the reset command to the S75CA.  If communication fails, retry up to four times.
    for (int i = 0; i < NUM_RETRIES; i++) {
        if ((LastError = SendWaitForConfirmation( temp, CAM_RESET_PORT, dataBuffer, 4 ))== ERR_SUCCESS) break;

        if (i == 3) {
            // Failed to reset CAM on S75CA.
            // Don't need to define error code -- done by SendWaitForConfirmation()
            return LastError;
        }        
    }

    // CAM on S75CA Reset successfully
    return LastError;

}


int CS75CA::setCAMWatchdogTimeout(WORD timeout)
{
	int last_error_code ;

	S75CA_CAM_Watchdog_Timeout_MSG CAM_Watchdog_Timeout_Msg;


    // Set the opcode in the message to Upload (0x0002)

    CAM_Watchdog_Timeout_Msg.Opcode = htons(0x0002);
    CAM_Watchdog_Timeout_Msg.Timeout = htons(timeout);

	srand( time(NULL) );
	CAM_Watchdog_Timeout_Msg.Random = rand();

	for ( int i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	digitalsignature( &m_sec, (uint8_t *)(&CAM_Watchdog_Timeout_Msg), sizeof( CAM_Watchdog_Timeout_Msg ) - 20 );

	memcpy( CAM_Watchdog_Timeout_Msg.Signature, m_sec.digest, 20 );

	last_error_code = SendCAMWatchdogTimeout( &CAM_Watchdog_Timeout_Msg, sizeof( CAM_Watchdog_Timeout_Msg ) );

    return last_error_code;
}


int CS75CA::getCAMWatchdogTimeout(WORD *cam_watchdog_timeout)
{
//	WORD opcode;                    // Opcode to send to S75
	int temp1 = 1000;               // Temporary storage variable
//	int count = 0 ;                 // Temporary storage variable
//	DWORD xx = 1;                   // Temporary storage variable
	SOCKET s;
	struct sockaddr_in device_address;
	fd_set readfs;
	struct timeval timeout;
	int result;
	bool success=false;
    BYTE Buffer2[sizeof(S75CAPAT_MSG)];
	int i;
	int last_error_code ;
	TCHAR temp[16];
	S75CA_CAM_Watchdog_Timeout_MSG CAM_Watchdog_Timeout_Request;
	S75CA_CAM_Watchdog_Timeout_MSG CAM_Watchdog_Timeout_Response;


    // Convert the IP address to a string.
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);

	memset( (void *)&CAM_Watchdog_Timeout_Response, 0, sizeof( CAM_Watchdog_Timeout_Response ) );
	memset( (void *)&CAM_Watchdog_Timeout_Request, 0, sizeof( CAM_Watchdog_Timeout_Request ) );

	// Set the opcode to send to the S75 to download (0x0001)

	CAM_Watchdog_Timeout_Request.Opcode = htons(0x0001);
	srand( time(NULL) );
	CAM_Watchdog_Timeout_Request.Random = rand();

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	digitalsignature( &m_sec, (uint8_t *)(&CAM_Watchdog_Timeout_Request), sizeof( CAM_Watchdog_Timeout_Request ) - 20 );

	memcpy( CAM_Watchdog_Timeout_Request.Signature, m_sec.digest, 20 );


	// Create a socket to communicate with the S75

#ifndef LINUX
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) {
#else
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) {
#endif


        last_error_code = ERR_CAM_WATCHDOG_TIMEOUT_SOCKET_CREATE;

	} else {

        // Connect the newly created socket to the S75.

    	device_address.sin_family = AF_INET;

    	device_address.sin_port = htons( CAM_WATCHDOG_TIMEOUT_CONFIG_PORT );

    	device_address.sin_addr.s_addr = inet_addr( temp);

    	if ( connect( s, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr_in ) ) != 0){

    		last_error_code = ERR_CAM_WATCHDOG_TIMEOUT_SOCKET_CONNECT;

		} else {

            // Create a buffer the size of the CAMWatchdogTimer Msg to hold the
            // response we receive back.

			
            // Download the data.  Retry (if required) up to 4 times

            for (i = 0; (!success) && ( i < NUM_RETRIES ); i++) {

            	// Send the CAM WATCHDOG TIMER reqest to the S75

                if ( send( s, (const char *)(&CAM_Watchdog_Timeout_Request), sizeof( CAM_Watchdog_Timeout_Request), 0 ) != sizeof( CAM_Watchdog_Timeout_Request ) ) {

                    last_error_code = ERR_CAM_WATCHDOG_TIMEOUT_SEND;

				} else {

#ifndef LINUX
	                Sleep(30);
#endif

	                timeout.tv_sec = 1;

	                timeout.tv_usec = 0;

        	        FD_ZERO( &readfs );

        	        FD_SET( s, &readfs );

                    // Try to receive the response from S75.  Check for errors.

#ifndef LINUX
                    if ( ( result = select( 1, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#else
                    if ( ( result = select( s+1, &readfs, NULL, NULL, &timeout ) ) != -1 ) {
#endif
	        	        if ( result != 0 ) {

#ifndef LINUX
    		    	        if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75CA_CAM_Watchdog_Timeout_MSG), 0 ) ) == SOCKET_ERROR ) {

	            	    		temp1 = GetLastError();

	                	        if(temp1 != WSAETIMEDOUT){

	        	        			last_error_code = ERR_CAM_WATCHDOG_TIMEOUT_RECEIVE;

								} // if


#else
    		    	        if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75CA_CAM_Watchdog_Timeout_MSG), 0 ) ) == -1 ) {
					last_error_code = ERR_CAM_WATCHDOG_TIMEOUT_RECEIVE;
#endif

							} else {

		    	    	        // Make sure we have received the right number of bytes back

	    		    	        if (temp1 == sizeof(S75CA_CAM_Watchdog_Timeout_MSG)) {

	            		    		memcpy(&CAM_Watchdog_Timeout_Response, Buffer2, sizeof(S75CA_CAM_Watchdog_Timeout_MSG));

									*cam_watchdog_timeout = ntohs( CAM_Watchdog_Timeout_Response.Timeout );

					                last_error_code = ERR_SUCCESS;

									success = true;

								} else {
                                    last_error_code = ERR_CAM_WATCHDOG_TIMEOUT_ACK_BAD;

								} // if

							} // if recv

						} else {

							last_error_code = ERR_CAM_WATCHDOG_TIMEOUT_RECEIVE;

						} // if select result 0

					} else {

						last_error_code = ERR_CAM_WATCHDOG_TIMEOUT_RECEIVE;

					} // if select

				} // if send

			} // for 4 retires



		} // if socket connect

#ifndef LINUX
        closesocket( s );
#else
        close( s );
#endif

	} // if socket create

	return last_error_code;
}



int CS75CA::SendCAMWatchdogTimeout( S75CA_CAM_Watchdog_Timeout_MSG  *CAMWatchdogTimeoutMsg, int size )
{
	bool success = false;
	TCHAR  temp[16];
	SOCKET s;
	int last_error_code ;
	struct sockaddr_in device_address;
	int i;
	struct timeval timeout;
	fd_set readfs;
	int result;
	int temp1 = 1000;               // Temporary storage variable
	/* bool configurationApplied;          */
	// bool ProgramInList;   Is the Program we are looking at in the current list?
	/* int j; */


  // Convert the IP address to a string.
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);

    // Create a socket to communicate with the S75


#ifndef LINUX
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) {
#else
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) {
#endif
		last_error_code = ERR_CAM_WATCHDOG_TIMEOUT_SOCKET_CREATE;

	} else {

        // Connect the newly created socket to the S75.

    	device_address.sin_family = AF_INET;

    	device_address.sin_port = htons( CAM_WATCHDOG_TIMEOUT_CONFIG_PORT );

    	device_address.sin_addr.s_addr = inet_addr( temp );

    	if ( connect( s, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr_in ) ) != 0){

            last_error_code = ERR_CAM_WATCHDOG_TIMEOUT_SOCKET_CONNECT;

		} else {

            // Create a buffer to hold the data we wish to send

            BYTE *Buffer1=NULL;
			Buffer1 = new BYTE[size];

            memcpy(Buffer1, CAMWatchdogTimeoutMsg, size); // copy the data in the buffer.
			 

            // Create a buffer to hold the response we receive back.

			BYTE *Buffer2=NULL;
			Buffer2 = new BYTE[size];

            for (i = 0; (!success) && ( i < NUM_RETRIES ); i++) {

    			// Send the message to the S75

                if ( send( s, (const char *)Buffer1, size, 0 ) != size ) {

					last_error_code = ERR_CAM_WATCHDOG_TIMEOUT_SEND;		
		
				} // if
	
#ifndef LINUX
    	        Sleep(30);
#endif

                // Set the socket options so that we now have a blocking socket.  Then, 
                // wait for a response packet or for a timeout.

    	        timeout.tv_sec = 1;

    	        timeout.tv_usec = 0;

    	        FD_ZERO( &readfs );

    	        FD_SET( s, &readfs );

                // Try to receive the response from S75.  Compare the response (if
                // received) to the values sent to see if they are the same.
#ifndef LINUX
                if ( ( result = select( 1, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#else
                if ( ( result = select( s+1, &readfs, NULL, NULL, &timeout ) ) != -1 ) {
#endif

    	        	if ( result != 0 ) {

#ifndef LINUX

        		    	if( (temp1 = recv( s, (char *)Buffer2, size, 0 ) ) == SOCKET_ERROR ) {
	        		    	temp1 = GetLastError();
		            		if(temp1 != WSAETIMEDOUT){
			            		 last_error_code = ERR_CAM_WATCHDOG_TIMEOUT_RECEIVE;
					} // if

#else
        		    	if( (temp1 = recv( s, (char *)Buffer2, size, 0 ) ) == -1 ) {
					 last_error_code = ERR_CAM_WATCHDOG_TIMEOUT_RECEIVE;

#endif

						}  // if

    			        // Check to make sure the data received back is the same data sent.

    			        if(temp1 ==  size){

							success = true;

	    	        		for(i = 0 ; i < size; i++ ){

		    	        		if(Buffer1[i] != Buffer2[i]) {

		    		        		last_error_code = ERR_CAM_WATCHDOG_TIMEOUT_ACK_BAD;

									success = false;

								} // if

							} // for

				            last_error_code = ERR_SUCCESS;
	
						} // if

					} // if select result 0

				} // if select

			} // for retries

			delete [] Buffer1;
			delete [] Buffer2;

		} //.if socket connect

#ifndef LINUX
	  	closesocket( s );
#else
	  	close( s );
#endif

	} // if socket create

	return last_error_code;

}
