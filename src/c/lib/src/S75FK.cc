// S75FK.cpp: implementation of the CS75FK class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S75FK.h"
#include "S75PlusStatus.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "Password.h"


#ifdef LINUX
	extern void digitalsignature(pstruct *p, uint8_t *text, uint32_t len);
#endif

	const int PID_CONFIG_PORT = 0x1977;
	const int RESET_PORT = 0x9999;
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



 //----------------------------------------------------------------------------
 //			Function:	  encryptCSAkeys()
 //	 Description:		Encrypt the CSA key(s).
 //    Arguments:		none
 //			 Returns:		none
 //----------------------------------------------------------------------------
 void
 encryptCSAkeys(unsigned short * ptrKeyTable)
 {
 unsigned char secretKey[] = {	0xfe, 0xc5, 0x5d, 0x1f,
								0xe0, 0x67, 0x71, 0xa4,
								0x28, 0xcd, 0x0c, 0x8e,
								0x6d, 0x6e, 0xd0, 0x8c };
 unsigned int keySchedule = 2264783973;
 unsigned int i, j;
 unsigned int key;
		
	 // encrypt the CSA keys
	 for ( i = 0; i < NUMBER_OF_FIXED_KEYS * 5; i += 4)	{
 		encrypt((unsigned char *)(&secretKey[0]), (unsigned char *)(&ptrKeyTable[i]));
		for ( j = 0; j < 4; j++ ) {
			key = secretKey[4*j+0];
			key = key << 8;
			key += secretKey[4*j+1];
			key = key << 8;
			key += secretKey[4*j+2];
			key = key << 8;
			key += secretKey[4*j+3];
			key += keySchedule;
			secretKey[4*j+3] = key & 0x000000ff;
			key = key >> 8;
			secretKey[4*j+2] = key & 0x000000ff;
			key = key >> 8;
			secretKey[4*j+1] = key & 0x000000ff;
			key = key >> 8;
			secretKey[4*j+0] = key & 0x000000ff;
		}
	}	 				 				 				 			
 } 
 

//----------------------------------------------------------------------------
 //			Function:	  decryptCSAkeys()
 //	 Description:		Decrypt the CSA key(s).
 //    Arguments:		pointer to keytable
 //			 Returns:		none
 //----------------------------------------------------------------------------
 void
 decryptCSAkeys(unsigned short * ptrKeyTable)
 {
  unsigned char secretKey[] = {	0xfe, 0xc5, 0x5d, 0x1f,
								0xe0, 0x67, 0x71, 0xa4,
								0x28, 0xcd, 0x0c, 0x8e,
								0x6d, 0x6e, 0xd0, 0x8c };
 unsigned int keySchedule = 2264783973;
 unsigned int i, j;
 unsigned int key;
		
	 // decrypt the CSA keys
	 for ( i = 0; i < NUMBER_OF_FIXED_KEYS * 5; i += 4)	{
		decrypt((unsigned char *)(&secretKey[0]), (unsigned char *)(&ptrKeyTable[i]));
		for ( j = 0; j < 4; j++ ) {
			key = secretKey[4*j+0];
			key = key << 8;
			key += secretKey[4*j+1];
			key = key << 8;
			key += secretKey[4*j+2];
			key = key << 8;
			key += secretKey[4*j+3];
			key += keySchedule;
			secretKey[4*j+3] = key & 0x000000ff;
			key = key >> 8;
			secretKey[4*j+2] = key & 0x000000ff;
			key = key >> 8;
			secretKey[4*j+1] = key & 0x000000ff;
			key = key >> 8;
			secretKey[4*j+0] = key & 0x000000ff;
		}
	}	 				 				 				 				 				 				 			
 }



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS75FK::CS75FK( bool remote )
{
	m_StatusPacketSize = sizeof(S75FKStatus_MSG);
	m_Device_Type = S75FK;  // when we instantiate this class, the device type will be enforced on attachment.
	m_StatusWrapper = (StatusPacket *)(new S75FKStatusPacket( &m_Status ));
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


CS75FK::~CS75FK()
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


int CS75FK::SetFixedKeys( LPBYTE KeyTable )
{
	int i;

	int last_error_code ;

	S75FKeys_MSG FKMsg;


	m_pS75FK_keys = (S75FK_KeyList *) KeyTable ;


    // Set the opcode in the FixedKey Msg to Upload (0x0002)

    FKMsg.Opcode = htons(0x0002);

    for (i = 0; i < NUMBER_OF_FIXED_KEYS; i++) {

		if ( m_pS75FK_keys->odd[i] ) {
			FKMsg.FK[i].PID = htons( m_pS75FK_keys->PID[i] | 0x2000 );	
		} else {
			FKMsg.FK[i].PID = htons( m_pS75FK_keys->PID[i] );
		}
		memcpy( FKMsg.FK[i].key, m_pS75FK_keys->key[i], 8 );

    } // for i

//FILE *fp = NULL;
//fp = fopen( "C:\\FKDebug.hex", "wb" );
//fwrite( (void *)(&FKMsg), 1, sizeof( FKMsg), fp );
//fclose( fp );
 	encryptCSAkeys( (unsigned short *)(FKMsg.FK) );


	srand( time(NULL) );
	FKMsg.Random = rand();

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	digitalsignature( &m_sec, (uint8_t *)(&FKMsg), sizeof( FKMsg ) - 20 );

	
	memcpy( FKMsg.Signature, m_sec.digest, 20 );

	last_error_code = SendFixedKeys( &FKMsg, sizeof( S75FKeys_MSG ), 64 );

    return last_error_code;

}



int CS75FK::GetFixedKeys( LPBYTE KeyTable )
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
    BYTE Buffer2[sizeof(S75FKeys_MSG)];
	int i;
	int last_error_code ;
	TCHAR temp[16];
	S75FKeys_MSG FKResponseMsg;
	S75FKeys_MSG FKRequest;
//

	m_pS75FK_keys = (S75FK_KeyList *) KeyTable ;

	  // Convert the IP address to a string.
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);

	memset( (void *)&FKResponseMsg, 0, sizeof( FKResponseMsg ) );
	memset( (void *)&FKRequest, 0, sizeof( FKRequest ) );

	// Set the opcode to send to the S75 to download (0x0001)

	FKRequest.Opcode = htons(0x0001);
	srand( time(NULL) );
	FKRequest.Random = rand();

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	digitalsignature( &m_sec, (uint8_t *)(&FKRequest), sizeof( S75FKeys_MSG ) - 20 );

	memcpy( FKRequest.Signature, m_sec.digest, 20 );


	// Create a socket to communicate with the S75

#ifndef LINUX
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) {
#else
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) {
#endif


        last_error_code = ERR_FK_SOCKET_CREATE;

	} else {

        // Connect the newly created socket to the S75.

    	device_address.sin_family = AF_INET;

    	device_address.sin_port = htons( FK_CONFIG_PORT );

    	device_address.sin_addr.s_addr = inet_addr( temp);

    	if ( connect( s, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr_in ) ) != 0){

    		last_error_code = ERR_FK_SOCKET_CONNECT;

		} else {

            // Create a buffer the size of the Key table to hold the
            // response we receive back.

			
            // Download the Key table.  Retry (if required) up to 4 times

            for (i = 0; (!success) && ( i < NUM_RETRIES ); i++) {

            	// Send the Fixed Key reqest to the S75

                if ( send( s, (const char *)(&FKRequest), sizeof( S75FKeys_MSG), 0 ) != sizeof( S75FKeys_MSG ) ) {

                    last_error_code = ERR_FK_SEND;

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
    		    	        if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75FKeys_MSG), 0 ) ) == SOCKET_ERROR ) {

	            	    		temp1 = GetLastError();

	                	        if(temp1 != WSAETIMEDOUT){

	        	        			last_error_code = ERR_FK_RECEIVE;

								} // if


#else
    		    	        if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75FKeys_MSG), 0 ) ) == -1 ) {
					last_error_code = ERR_FK_RECEIVE;
#endif

							} else {

		    	    	        // Make sure we have received the right number of bytes back

	    		    	        if (temp1 == sizeof(S75FKeys_MSG)) {

	            		    		memcpy(&FKResponseMsg, Buffer2, sizeof(S75FKeys_MSG));

									// Go through the Fixed Key table and change all the 
    				                // PIDs from network order to host order.
					                for ( i = 0; i < NUMBER_OF_FIXED_KEYS; i++) {
   						                m_pS75FK_keys->PID[i] = ntohs(FKResponseMsg.FK[i].PID );
										memcpy( m_pS75FK_keys->key[i], FKResponseMsg.FK[i].key, 8 );
										if ( m_pS75FK_keys->PID[i] & 0x2000 ) {
											m_pS75FK_keys->odd[i] = true;
											m_pS75FK_keys->PID[i] = 0x1fff & m_pS75FK_keys->PID[i];
										} else {
											m_pS75FK_keys->odd[i] = false;
										}
									}

					                last_error_code = ERR_SUCCESS;

									success = true;

								} else {
                                    last_error_code = ERR_FK_ACK_BAD;

								} // if

							} // if recv

						} else {

							last_error_code = ERR_FK_RECEIVE;

						} // if select result 0

					} else {

						last_error_code = ERR_FK_RECEIVE;

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


int CS75FK::SendFixedKeys( S75FKeys_MSG  *FKMsg, int size, int max_fixed_keys )
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


	  // Convert the IP address to a string.
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);

    // Create a socket to communicate with the S75


#ifndef LINUX
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) {
#else
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) {
#endif
		last_error_code = ERR_FK_SOCKET_CREATE;

	} else {

        // Connect the newly created socket to the S75.

    	device_address.sin_family = AF_INET;

    	device_address.sin_port = htons( FK_CONFIG_PORT );

    	device_address.sin_addr.s_addr = inet_addr( temp );

    	if ( connect( s, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr_in ) ) != 0){

            last_error_code = ERR_FK_SOCKET_CONNECT;

		} else {

            // Create a buffer to hold the data we wish to send

            BYTE *Buffer1=NULL;
			Buffer1 = new BYTE[size];

            memcpy(Buffer1, FKMsg, size); // copy the data in the buffer.
			 

            // Create a buffer the size of the Fixed Key Message to hold the
            // response we receive back.

			BYTE *Buffer2=NULL;
			Buffer2 = new BYTE[size];

            for (i = 0; (!success) && ( i < NUM_RETRIES ); i++) {

    			// Send the Fixed Keys to the S75

                if ( send( s, (const char *)Buffer1, size, 0 ) != size ) {

					last_error_code = ERR_FK_SEND;		
		
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
			            		 last_error_code = ERR_FK_RECEIVE;
					} // if

#else
        		    	if( (temp1 = recv( s, (char *)Buffer2, size, 0 ) ) == -1 ) {
					 last_error_code = ERR_FK_RECEIVE;

#endif

						}  // if

    			        // Check to make sure the data received back is the same data sent.

    			        if(temp1 ==  size){

							success = true;

	    	        		for(i = 0 ; i < size; i++ ){

		    	        		if(Buffer1[i] != Buffer2[i]) {

		    		        		last_error_code = ERR_FK_ACK_BAD;

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
/*
    if (success) {
        // Configuration sent to S75D

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

        // Check to see if the values sent to the S75D are the same as those being
        // reported in the status


		// Check the PAT with the one we just downloaded.  Note that the 
		// PAT we just downloaded could be in a different order from ours.


		configurationApplied = true;


        // Go through the PAT downloaded and check to make sure that each Program we
        // uploaded is somewhere in that list.

		for (i = 0; i < max_programs; i++) {

			ProgramInList = false;


	        if (PATMsg->PATTableEntry[i].ProgramNumber != 0) {     // Skip Program 0

		        for (j = 0; j < max_programs; j++) {

					if ( (((S75FKStatusPacket *)m_StatusWrapper)->ProgramListPointer())[j] == PATMsg->PATTableEntry[i].ProgramNumber ) {
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
*/
	return last_error_code;

}


bool CS75FK::ParseStatusPacket()
{
	BYTE temp[4];

	// If this function is called, the status Structure has been filled with data received by Status Listner.
	// We need to verify that the packet is valid, and that it matches the device to be valid.

	if ( !CNovraDevice::ParseStatusPacket() ) return false;

	((S75FKStatusPacket *)m_StatusWrapper)->ntoh();

	if( ((*(((S75FKStatusPacket *)m_StatusWrapper)->DSPFirmWarePointer())) & 0xFF00) != 0x6B00 )
		 return false ;		

	if(m_Device_Type != S75FK) return false;

	m_StatusListener.GetLastPacketSource(temp);

	memcpy(m_Device_IP_Address, temp,4); // Get the address from the Unit.

	return true;
}



int CS75FK::PollStatus( StatusPacket *status_packet_wrapper, int timeout )
// This function should be factored into a base class (there is a small section of non-common code) that is different
// from receiver type to receiver type.  The rest of the code is common among all current S75+ based receivers
{
	S75Plus_One_Time_Status_Request_MSG message;
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
	/* S75Plus_Status *response = NULL; */
	short firmware_version = 0x0000;
//	int     BERmantissa, BERexponent;   // Bit Error Rate mantissa and exponent

	S75PlusStatusPacket *s75plus_status_packet_wrapper = (S75PlusStatusPacket *)status_packet_wrapper;

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

                if ( send( s, (const char *)(&message), sizeof( S75Plus_One_Time_Status_Request_MSG), 0 ) != sizeof( S75Plus_One_Time_Status_Request_MSG ) ) {
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
    		    	        if( (temp1 = recv( s, (char *)(status_packet_wrapper->data()), sizeof(S75PlusStatus_MSG), 0 ) ) == SOCKET_ERROR ) {

	            	    		temp1 = GetLastError();

	                	        if(temp1 != WSAETIMEDOUT){

	        	        			last_error_code = ERR_STATUS_POLL;

								} // if


#else
    		    	        if( (temp1 = recv( s, (char *)(status_packet_wrapper->data()), sizeof(S75PlusStatus_MSG), 0 ) ) == -1 ) {
								last_error_code = ERR_STATUS_POLL;
#endif

							
							} else {

		    					// Make sure we have received the right number of bytes back

				    		    if (temp1 == sizeof(S75PlusStatus_MSG)) {

									success = true;

									if(m_MACAddressValid){
										if(memcmp(status_packet_wrapper->ReceiverMACPointer(), m_Device_MAC_Address,6)!=0) {

											last_error_code = ERR_WRONG_RECEIVER;
											success = false;
										}
									}

									// This is the non-common part of code starting here
									firmware_version = ntohs((*(status_packet_wrapper->DSPFirmWarePointer()))) & 0xFF00;
									if( firmware_version != (short)0x6B00 ) {		
										success = false;
										last_error_code = ERR_WRONG_RECEIVER;		
									}

									if(m_Device_Type != S75FK) {
										success = false;
										last_error_code = ERR_DEVICE_TYPE_MISMATCH;	

									}
									// This is the end of the non-common code (all the rest is the same for all S75+ receivers so far)
						
									if ( success ) {

										s75plus_status_packet_wrapper->ntoh();

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


int CS75FK::GetStatus(void *Container)
{
	S75Plus_Status * pStatus;
	int i;
//	WORD temp ;

	pStatus = (S75Plus_Status *) Container;

	if(!IsStatusValid())  return ERR_FAILED ; // the status isn't valid. 

	if(m_Device_Type != S75FK) return ERR_DEVICE_TYPE_MISMATCH ;

	CSatelliteDevice::GetStatus( (SatelliteStatus *)(&(S75PlusStatus( Container ))), true );

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

