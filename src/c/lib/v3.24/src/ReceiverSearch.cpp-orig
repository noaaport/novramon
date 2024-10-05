//#include "stdafx.h"

#include "ReceiverSearch.h"
#include "ReceiverFactory.h"
#include "time.h"

#ifdef WINDOWS
#ifndef  _WinSock2Included_
	#define	 _WinSock2Included_
	#include <winsock2.h>
#endif
#endif

#ifndef WINDOWS
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>

    typedef unsigned short WORD;
    typedef unsigned long DWORD;
    typedef int SOCKET;
    typedef bool BOOL;
#endif


#define MAX_DEVICES 10

const int num_auto_detect_ports = 10;	///< Novra's device discovery strategy assumes receivers will be sending status on a predefined list of ports

const WORD auto_detect_status_ports[num_auto_detect_ports]	/// Novra's device discovery strategy assumes receivers will be sending status on a predefined list of ports.  This is the list of auto-detection ports... already compiled into the library
= {	

	0x1974,
	0x2075,
	0x2175,
	0x2275,
	0x2375,
	0x2475,
	0x3475,
	0x4475,
	0x4575,
	0x4675

};



// Constructors/Destructors
//  


ReceiverSearch::ReceiverSearch ( ) {
}

ReceiverSearch::~ReceiverSearch ( ) { }


int ReceiverSearch::numAutoDetectPorts()
{
	return num_auto_detect_ports; 
}


unsigned short ReceiverSearch::autoDetectPort( int index )
{
	if ( index < num_auto_detect_ports ) {
		return auto_detect_status_ports[index];
	} else {
		return 0;
	}
}


int ReceiverSearch::portIndex( unsigned short port )
{
	int index = -1;

	for ( int i = 0; ( i < num_auto_detect_ports ) && ( index == -1 ); i++ ) {
		if ( auto_detect_status_ports[i] == port ) {
			index = i;
		}
	}

	return index;
}


NOVRA_ERRORS ReceiverSearch::GetAvailablePort( WORD &port)
{
#ifdef WINDOWS
	const int winsock_version=2;  // Request Winsock 2.0
	WSADATA	  wsa_data;           // Results of Winsock Startup Request
#endif
	SOCKET	s;
	int i;
	struct sockaddr_in address;
#ifdef WINDOWS
	bool use_again=true;
	WSAStartup( winsock_version, &wsa_data );
#else
	int use_again;
#endif

#ifdef WINDOWS
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) {
#else	
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) {
#endif
		s=0;
        	return N_ERROR_SOCKET_CREATE;
	}
	//setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (const char *)(&use_again), sizeof( use_again ) );
	for(i = 0; i<num_auto_detect_ports; i++){
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons( (unsigned short)auto_detect_status_ports[i]);
		if ( bind ( s, (struct sockaddr *)(&address), sizeof( struct sockaddr ) ) == 0 ) {
			port = auto_detect_status_ports[i];
#ifdef WINDOWS
			closesocket( s );
			WSACleanup();
#else
			close( s );
#endif
			return N_ERROR_SUCCESS;
		}
	}
#ifdef WINDOWS
	closesocket( s );
	WSACleanup();
#else
	close( s );
#endif


	return N_ERROR_FAILED;
}


Receiver* ReceiverSearch::getRemote( string ip, int timeout, int retries )
{

#ifdef WINDOWS

	WSADATA	  wsa_data;           // Results of Winsock Startup Request

	const int winsock_version=2;  // Request Winsock 2.0
#endif

#ifdef WINDOWS
	BOOL use_again=true;
#else
	int use_again;
#endif

	ReceiverFactory receiver_factory;

	SOCKET s;
    struct sockaddr_in address;
//	int addr_size;
	bool success = false;
	unsigned char message[6] = { 0x47, 0x47, 0x48, 0x48, 0x49, 0x49 };
	struct timeval timeout_struct;
	fd_set readfs;
	int result;
	int recv_len;
	unsigned char packet[1500];
	Receiver *r = NULL;


	try {

#ifdef WINDOWS
		WSAStartup( winsock_version, &wsa_data );
#endif


		memset( (void *)&address, 0, sizeof( address ) );
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = inet_addr( ip.c_str() );
		address.sin_port = htons( 0x2011 );

#ifdef WINDOWS
		if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) throw 1;
#else
		if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) throw 1;
#endif

		setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (const char *)(&use_again), sizeof( use_again ) );

		if ( ::connect( s, (const struct sockaddr *)(&address), sizeof( struct sockaddr ) ) != 0) throw 2;









/*
	char temp[20];
*/



		for ( int i = 0; (!success) && ( i < retries ); i++ ) {

			try {
        
				if ( send( s, (const char *)(&message), 6, 0 ) != 6 ) throw 1;

#ifdef WINDOWS
				Sleep(30);
#endif
				timeout_struct.tv_sec = timeout/1000;
				timeout_struct.tv_usec = timeout % 1000;

				FD_ZERO( &readfs );
				FD_SET( s, &readfs );

				// Try to receive the response from receiver.  Check for errors.

#ifdef WINDOWS
				if ( ( result = select( 1, &readfs, NULL, NULL, &timeout_struct ) ) == SOCKET_ERROR ) throw 2;
#else
				if ( ( result = select( s+1, &readfs, NULL, NULL, &timeout_struct ) ) == -1 ) throw 2;
#endif

				if ( result == 0 ) throw 3;

#ifdef WINDOWS
				if ( ( recv_len = recv( s, (char *)packet, 1500, 0 ) ) == SOCKET_ERROR ) {

					result = GetLastError();

					if ( result != WSAETIMEDOUT ) throw 4;

				}
#else
				if ( ( recv_len = recv( s, (char *)packet, 1500, 0 ) ) == -1 ) throw 4;
#endif

				// Make sure we have received the right number of bytes back

/*
				if ( recv_len != sizeof( StatusMSG ) ) throw 5;

				sprintf( temp, "%02x-%02x-%02x-%02x-%02x-%02x",
						 ((StatusMSG *)packet)->ReceiverMAC[0],
						 ((StatusMSG *)packet)->ReceiverMAC[1],
						 ((StatusMSG *)packet)->ReceiverMAC[2],
						 ((StatusMSG *)packet)->ReceiverMAC[3],
						 ((StatusMSG *)packet)->ReceiverMAC[4],
						 ((StatusMSG *)packet)->ReceiverMAC[5] );

				if ( !matches( RECEIVER_MAC, string( temp ) ) ) throw 6;

				if ( ((StatusMSG *)packet)->DeviceType != typeIDCode() ) throw 7;
//			if ( ((StatusMSG *)packet)->DeviceType != ID_S200Base ) throw 7;
*/

				r = receiver_factory.Create( packet, ip );

				success = true;

			}

			catch ( int e ) {
				switch ( e ) {
					case 1 : //cout << "Send Failed" << endl;
						break;
					case 2 : //cout << "Select Failed" << endl;
						break;
					case 3 : //cout << "Select Failed with 0 result" << endl;
						break;
				}
				success = false;
			}
	
		} // for 4 retires

	}

	catch ( int e ) {

		switch ( e ) {
			case 1 : //cout<<"INVALID SOCKET"<<endl;
				break;
			case 2 : //cout<<"SOCKET Connect Failed"<<endl;
				break;
		}
	}

	if ( s > 0 ) {
#ifdef WINDOWS
		closesocket( s );
#else
		close( s );
#endif
	}

	return r;
}


bool ReceiverSearch::discoverLocal( ReceiverList *receiverList )
{

    // LOCAL VARIABLES

#ifdef WINDOWS

	WSADATA	  wsa_data;           // Results of Winsock Startup Request

	const int winsock_version=2;  // Request Winsock 2.0
#endif

	int       port_index;         // Index into auto detection ports array

	BOOL      keep_looking=true;  // Flag used to indicate when no new devices
	                              // can be seen on a port

//	const int timeout= 3000;       // Maximum time to wait for a status packet

//	int       device_index;       // Index into the devices array

//	BOOL      device_found;       // Flag to indicate if a device can be found in the list

//	int       byte_index;         // Index into a byte array (IP and MAC addresses)

//	unsigned long      last_time;          // Last time a new device was found

//	DWORD	  up_time;			// Status Up Time

	bool       success = false;        // Return Value

//	int		LastError;			// Error code from last operation.

//	Novra_DeviceEntry temp_dev;		// temporary device Info container
//	Novra_DeviceList devices;

	SOCKET s[num_auto_detect_ports];
    struct sockaddr_in address;
	struct timeval timeout;
	fd_set readfs;
	int result;
	int length;
	struct sockaddr_in from_address;
	int addr_size;
//	bool got_packet=true;
	const int max_receive = 1500;
	unsigned char receive_buffer[max_receive];
	string mac_str;
	ReceiverFactory receiver_factory;
	Receiver *receiver = NULL;
	time_t last_added;


#ifdef WINDOWS
	BOOL use_again=true;
#else
	int use_again;
#endif



//	devices.num_devices = 0;
#ifdef WINDOWS
	WSAStartup( winsock_version, &wsa_data );
#endif

    // Set up the status ports of the classes to the elements of the auto detect array.
	addr_size = sizeof( struct sockaddr_in );
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	for(port_index =0 ; port_index< num_auto_detect_ports; port_index ++){
		address.sin_port = htons( (unsigned short)auto_detect_status_ports[port_index] );
#ifdef WINDOWS
		if ( ( s[port_index] = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) {
#else
		if ( ( s[port_index] = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) {
#endif

			s[port_index]=0;
		} else {
			setsockopt( s[port_index], SOL_SOCKET, SO_REUSEADDR, (const char *)(&use_again), sizeof( use_again ) );
		}


		if ( s[port_index] != 0 ) {
			if ( bind ( s[port_index], (struct sockaddr *)(&address), sizeof( struct sockaddr ) ) != 0 ) {
#ifdef WINDOWS
				closesocket( s[port_index] );
#else
				close( s[port_index] );
#endif
				s[port_index] = 0;
			}
		}
	}


//	for(port_index =0 ; port_index< num_auto_detect_ports; port_index ++){ // For each of our objects...

	keep_looking = true;
	last_added = time( NULL );

	// On each port, loop until a new device can not be found

	while ( keep_looking ) {
		keep_looking = false;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		FD_ZERO( &readfs );
#ifndef WINDOWS
		int max_port;
		max_port = 0;
#endif
		for(port_index =0 ; port_index < num_auto_detect_ports; port_index ++){
			FD_SET( s[port_index], &readfs );
#ifndef WINDOWS
			if ( s[port_index] > max_port ) max_port = s[port_index];
#endif
		}
#ifdef WINDOWS
		if ( ( result = select( num_auto_detect_ports, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#else
		if ( ( result = select( max_port+1, &readfs, NULL, NULL, &timeout ) ) > 0 ) {
#endif
			//printf(" \n Select in Flush returned %d \n",result);

			if ( result != 0 ) {
				for( port_index = 0; port_index < num_auto_detect_ports; port_index++ ) {
					if ( FD_ISSET( s[port_index], &readfs ) ) {
#ifdef WINDOWS
						length = recvfrom( s[port_index], (char *)receive_buffer, max_receive, 0, (sockaddr *)(&from_address), &addr_size );
#else
						length = recvfrom( s[port_index], (char *)receive_buffer, max_receive, 0, (sockaddr *)(&from_address), (socklen_t *)(&addr_size) );
#endif
						if ( length > 0 ) {

							mac_str = receiver_factory.MACFromStatus( (unsigned char *)receive_buffer );

							if ( mac_str != "" ) {
	
								if ( receiverList->findReceiver( mac_str, string( inet_ntoa( from_address.sin_addr ) ) ) ) {

									if ( time( NULL ) < ( last_added + 3 ) ) {
										keep_looking = true;
									}

								} else {

									receiver = receiver_factory.Create( receive_buffer, 
																		string( inet_ntoa( from_address.sin_addr ) ), 
																		auto_detect_status_ports[port_index] );
									
									if ( receiver ) {
									
										receiverList->addReceiver( receiver );
										last_added = time( NULL );
										keep_looking = true;
									
									}

								}
							}
						}
					}
				}
			}
		}
	}

	for( port_index = 0; port_index < num_auto_detect_ports; port_index++ ) {

		if ( s[port_index] > 0 ) {
#ifdef WINDOWS
			closesocket( s[port_index] );
#else
			close( s[port_index] );
#endif
		}
	}

	success = true;	   	

	return success;

} // discoverDevices



/*
int CStatusListner::StartListening(WORD Port)
{

		// Make sure init was called.  Do this by making sure that the pointer
	// to the status buffer is not NULL.  If it is, return false.  We don't
	// care about the other pointer, since if they are NULL, we just won't
	// filter the status packets

    struct sockaddr_in address;
#ifdef WINDOWS
	BOOL use_again=true;
#else
	int use_again;
#endif

	if (m_pDestinationBuffer == NULL) return Err_SL_Not_Initialized;


	if ( m_IsListening ) {		// If the Socket is already opened,
		EndListening();
	}

#ifdef WINDOWS
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
#ifdef WINDOWS
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
*/
/*
bool CStatusListner::FlushBuffer()
{
	int length;
	int errorValue = 0;
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
#ifdef WINDOWS
		if ( ( result = select( 1, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#else
		if ( ( result = select( s+1, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#endif
			//printf(" \n Select in Flush returned %d \n",result);

			if ( result != 0 ) {
#ifdef WINDOWS
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
*/
