// StatusListner.cpp: implementation of the CStatusListner class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#ifndef LINUX
	#ifndef  _WinSock2Included_
	#define	 _WinSock2Included_
	#include <winsock2.h>
	#endif
	#ifndef  _WindowsIncluded_
	#define	 _WindowsIncluded_
	#include <windows.h>
	#endif
#endif
#include "StatusListner.h"
#include <time.h>
#include <stdio.h>
#ifdef LINUX
#define SOCKET_ERROR  -1

#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatusListner::CStatusListner()
{
	m_IsListening = false ;
	m_pFilterOnIP = NULL;
	m_pFilterIP = NULL;
	m_MaxRcvSize = 0;
	m_pTimeStamp = NULL ;
	m_pDestinationBuffer = NULL;
	s=0;

}

CStatusListner::~CStatusListner()
{
	if(m_IsListening)
		EndListening();

}

bool CStatusListner::Initialize(bool* FilterOnIP, LPBYTE pFilterIP, LPBYTE pBuffer, DWORD *TimeStamp, UINT MaxRcv)
{
	if(m_IsListening){			// If an attempt to Initialize while we are already listinging 
		return false;	
	}
	m_IsListening = false ;
	m_pFilterOnIP = FilterOnIP;
	m_pFilterIP = pFilterIP;
	m_MaxRcvSize = MaxRcv;
	m_pTimeStamp = TimeStamp ;
	m_pDestinationBuffer = pBuffer ;
	s=0;
	return true;

}

int CStatusListner::StartListening(WORD Port)
{

		// Make sure init was called.  Do this by making sure that the pointer
	// to the status buffer is not NULL.  If it is, return false.  We don't
	// care about the other pointer, since if they are NULL, we just won't
	// filter the status packets

    struct sockaddr_in address;
#ifndef LINUX
	BOOL use_again=true;
#else
	int use_again;
#endif

	if (m_pDestinationBuffer == NULL) return Err_SL_Not_Initialized;


	if ( m_IsListening ) {		// If the Socket is already opened,
		EndListening();
	}

#ifndef LINUX
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) {
#else
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) {
#endif

		s=0;
		return ERR_SS_SOCKET_CREATE;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( (unsigned short)Port );

	setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (const char *)(&use_again), sizeof( use_again ) );

    if ( bind ( s, (struct sockaddr *)(&address), sizeof( struct sockaddr ) ) != 0 ) {
#ifndef LINUX
		closesocket( s );
#else
		close( s );
#endif
		s = 0;
		return ERR_SS_SOCKET_CREATE;
	}

	m_IsListening = true ;
	
    return SL_SUCCESS;



}

void CStatusListner::EndListening()
{
	if ( s > 0 ) {
#ifndef LINUX
		closesocket( s );
#else
		close( s );
#endif
		s = 0;
		m_IsListening = false ;
	}
}

bool CStatusListner::IsListening()
{
	return m_IsListening;

}

bool CStatusListner::FlushBuffer()
{
	int length;
	/* int errorValue = 0; */
	struct sockaddr_in from_address;
	int addr_size;
	fd_set readfs;
	struct timeval timeout;
	int result;
	bool got_packet=true;

	if(!m_IsListening) return false; // If not listening there is no meaning of flushing the socket buffers
	addr_size = sizeof( struct sockaddr_in );

	// Receive packets until the buffer is empty
	while ( got_packet ) {
		// Receive a packet from the buffer
        got_packet = false;
		timeout.tv_sec = 0;
		timeout.tv_usec = 400;
		FD_ZERO( &readfs );
		FD_SET( s, &readfs );
#ifndef LINUX
		if ( ( result = select( 1, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#else
		if ( ( result = select( s+1, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#endif
			//printf(" \n Select in Flush returned %d \n",result);

			if ( result != 0 ) {
#ifndef LINUX
				length = recvfrom( s, (char *)m_RcvBuffer, m_MaxRcvSize, 0, (sockaddr *)(&from_address), &addr_size );
#else
				length = recvfrom( s, (char *)m_RcvBuffer, m_MaxRcvSize, 0, (sockaddr *)(&from_address), (socklen_t *)(&addr_size) );
#endif
				got_packet = true;
				
			}
		}
	}

	return true;	// Buffer now empty

}


int CStatusListner::ReceiveSinglePacket(int TimeOut)
{

	TCHAR temp[16];
	int length = m_MaxRcvSize;
	LPBYTE Buffer2 = new BYTE[length];
	int temp1 = 1000;
	/* int count = 0 ; */
	DWORD xx = 0;
	unsigned long startTime;
	unsigned long now;
	//bool switchModeSuccess = true;	   Mode switch successful?
	bool rcvOK = true;					// Status packet received OK?
	struct sockaddr_in from_address;
	int addr_size;
	fd_set readfs;
	struct timeval timeout;
	bool continue_looping = true;

	int result,	b1, b2, b3, b4, junk;
	int last_error_code;


	temp1 = TimeOut;

	if(!m_IsListening) return Err_SL_NotListening; // If not listening there is no meaning of flushing the socket buffers

    // Loop, receiving packets.  Break out of the loop if we reach the timeout or if
    // we receive a packet from the right IP address.
	//printf(" \n Wait for Single Packet started ");
    startTime = time(NULL);
    while (continue_looping) {
		memset( (void *)(&from_address), 0, sizeof( from_address ) );
        rcvOK = true;

        // Receive a packet
		addr_size = sizeof( sockaddr_in );
		timeout.tv_sec = TimeOut / 1000;
		timeout.tv_usec = TimeOut % 1000;
		FD_ZERO( &readfs );
		FD_SET( s, &readfs );

#ifndef LINUX
		if ( ( result = select( 1, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#else
		if ( ( result = select( s+1, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#endif
//	    	printf(" \n Select in receive Single returned %d \n",result);
			if ( result != 0 ) {
#ifndef LINUX
			    if ( ( temp1 = recvfrom ( s, (char *)Buffer2, length, 0, (sockaddr *)(&from_address), &addr_size ) ) == SOCKET_ERROR ) {
					temp1 = GetLastError();
					if(temp1 != WSAETIMEDOUT){
						if(temp1 == WSAEMSGSIZE)
							last_error_code = ERR_SS_MSG_TOO_BIG;
						else
							last_error_code = ERR_SS_RECEIVE;

					   //@@NS Having this will make packets from other devices bloack status packet for our device
						// So if we receive one or two stray packets we through them away.. big deal..!!
						//rcvOK = false;
					    } else {
						last_error_code = ERR_SS_TIMEOUT;

						rcvOK = false;
					}
#else
				if ( ( temp1 = recvfrom ( s, (char *)Buffer2, length, 0, (sockaddr *)(&from_address), (socklen_t *)(&addr_size) ) ) == -1 ) {
					
					rcvOK=false ;
					last_error_code = ERR_SS_TIMEOUT;

#endif

				}
				// Check for errors
				if (!rcvOK) {
					continue_looping = false;
				} else {

					strcpy( temp, inet_ntoa( from_address.sin_addr ) );
					sscanf( temp, "%d%c%d%c%d%c%d", &b1, (char*)&junk, &b2, (char*)&junk, &b3, (char*)&junk,&b4 );
					m_LastPacketSource[0] = b1;
					m_LastPacketSource[1] = b2;
					m_LastPacketSource[2] = b3;
					m_LastPacketSource[3] = b4;


					// Process the incoming packet
					if (rcvOK) {
						if (temp1 > (int)m_MaxRcvSize) temp1 = m_MaxRcvSize;
						if( (*m_pFilterOnIP) == true ){	

							if (memcmp(m_LastPacketSource, m_pFilterIP,4)!=0) {
								// Packet filtered out.
								rcvOK = false;
							
							}
							else {
								// Packet was OK
								last_error_code = ERR_SUCCESS;
								xx = time(NULL);
								memcpy(m_pTimeStamp, &xx, sizeof(DWORD));
								memcpy(m_pDestinationBuffer , Buffer2 , temp1);
							
								continue_looping = false;
							}
						}
						else{
							last_error_code = ERR_SUCCESS;
							xx = time(NULL);
							memcpy(m_pTimeStamp, &xx, sizeof(DWORD));
							memcpy(m_pDestinationBuffer , Buffer2 , temp1);
							
							continue_looping = false;
						}

					}
				}
			}
		}

		// Check if we have timed out
		//@@NS added the Equal Sign... not having it makes the timeoput double when there is no packets.
		now = time( NULL );
		if ((( 1000 * (now - startTime)) >= (unsigned long)TimeOut)) { // If the time out is less thatn a 1000 it is useless to
			
			last_error_code = ERR_SS_TIMEOUT;	// check this way so we exit
			rcvOK = false;
			continue_looping = false;
		}
		if(TimeOut < 1000) {
			last_error_code = ERR_SS_TIMEOUT;
			
			rcvOK = false;
			continue_looping = false;
		}

	}

    delete [] Buffer2;

	//------------------------------------

/*
	int k;
	for ( k = 0; k < temp1; k++ ) {
		if ( ( k != 0x3A ) && ( k != 0x3B ) && ( k != 0x43 ) && ( k != 84 ) && ( k != 85 ) ) {
			*(m_pDestinationBuffer+k) = (unsigned char)(k % 256);
		}
	}
	*(m_pDestinationBuffer+32) = (unsigned char)0x01;
	*(m_pDestinationBuffer+33) = (unsigned char)0x33;

	*(m_pDestinationBuffer+34) = (unsigned char)0;
	*(m_pDestinationBuffer+35) = (unsigned char)6;
	*(m_pDestinationBuffer+36) = (unsigned char)0x76;
	*(m_pDestinationBuffer+37) = (unsigned char)0;
	*(m_pDestinationBuffer+38) = (unsigned char)0;
	*(m_pDestinationBuffer+39) = (unsigned char)0;

	*(m_pDestinationBuffer+66) = (unsigned char)0x1F;
	*(m_pDestinationBuffer+67) = (unsigned char)0x4;
	*(m_pDestinationBuffer+69) = (unsigned char)0x0;

	*(m_pDestinationBuffer+70) = (unsigned char)0x00;
	*(m_pDestinationBuffer+71) = (unsigned char)0x03;
	*(m_pDestinationBuffer+72) = (unsigned char)0xD8;
	*(m_pDestinationBuffer+73) = (unsigned char)0x02;

	*(m_pDestinationBuffer+102) = (unsigned char)0x00;
//	*(m_pDestinationBuffer+103) = (unsigned char)0x00;
*/

/*
	int k;

	for ( k = 0; k < 32; k++ ) {
		if ( k % 2 ) {
			*(m_pDestinationBuffer+140+k*4) = (unsigned char)0x20;
		} else {
			*(m_pDestinationBuffer+140+k*4) = (unsigned char)0x0;
		}
		*(m_pDestinationBuffer+140+k*4+1) = (unsigned char)(100+k);
		*(m_pDestinationBuffer+140+k*4+2) = (unsigned char)k;
		*(m_pDestinationBuffer+140+k*4+3) = (unsigned char)(k+1);
	}

	for ( k = 0; k < 16; k++ ) {
		*(m_pDestinationBuffer+268+k*6+0) = (unsigned char)192;
		*(m_pDestinationBuffer+268+k*6+1) = (unsigned char)168;
		*(m_pDestinationBuffer+268+k*6+2) = (unsigned char)254;
		*(m_pDestinationBuffer+268+k*6+3) = (unsigned char)k;
		*(m_pDestinationBuffer+268+k*6+4) = (unsigned char)3;
		*(m_pDestinationBuffer+268+k*6+5) = (unsigned char)232+k;
	}

	for ( k = 0; k < 16; k++ ) {
		*(m_pDestinationBuffer+364+k*2+0) = (unsigned char)0;
		*(m_pDestinationBuffer+364+k*2+1) = (unsigned char)(k+1);
	}

	for ( k = 0; k < 16; k++ ) {
		*(m_pDestinationBuffer+396+k*2+0) = (unsigned char)0;
		*(m_pDestinationBuffer+396+k*2+1) = (unsigned char)(k+1);
	}
*/
	//-------------------------------------

	// Everything is OK
	return last_error_code;

}

void CStatusListner::GetLastPacketSource(LPBYTE Src)
{
	memcpy(Src, m_LastPacketSource, 4);
}
