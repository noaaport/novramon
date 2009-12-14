//************************************************************************
//*
//* Copyright Novra Technologies Incorporated, 2003
//* 1100-330 St. Mary Avenue
//* Winnipeg, Manitoba
//* Canada R3C 3Z5
//* Telephone (204) 989-4724
//*
//* All rights reserved. The copyright for the computer program(s) contained
//* herein is the property of Novra Technologies Incorporated, Canada.
//* No part of the computer program(s) contained herein may be reproduced or
//* transmitted in any form by any means without direct written permission
//* from Novra Technologies Inc. or in accordance with the terms and
//* conditions stipulated in the agreement/contract under which the
//* program(s) have been supplied.
//*
//***************************************************************************
//
//
// S75.cpp : main source file for the S75 DLL
//
//
// Defines the following ...
//
//
//
// Functions  : discoverDevices
//				b_openS75         (uses byte arrays for IP and MAC)
//				s_openS75         (uses strings for IP and MAC)
//				closeS75
//              configureNetwork
//              configureRF
//              setPIDList
//              getPIDList
//              setProgramList
//              getProgramList
//              setPATList
//              getPATList
//				setFixedKeys
//				getFixedKeys
//              getStatus
//              configureTraps
//              pollStatus
//				discoverRemoteDevice
//
//---------------------------------------------------------------------------


#include "stdafx.h"

// PREPROCESSOR DIRECTIVES

#define VERSION_MAJOR	2
#define VERSION_MINOR	31
#define VERSION_DAY		16
#define VERSION_MONTH	4
#define VERSION_YEAR	2009

#ifdef LINUX
    	#include <time.h>
    	#include <string.h>
        #include <sys/socket.h>
	#include <stdlib.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
	#include <net/if.h>
    	#include "linux_windows_types.h"
#endif

#ifndef LINUX
	#ifndef  _WinSock2Included_
		#define	 _WinSock2Included_
		#include <winsock2.h>
	#endif
	#ifndef  _WindowsIncluded_
	#define	 _WindowsIncluded_
		#include <windows.h>
	#endif
	#include <iphlpapi.h>
#endif


#include <stdio.h>

#include "DeviceIdentifier.h"   // S75 Device Identifier header file
#include "s75_v2.h"				//  Header file for S75 Version 2 Class.
#include "s75_v3.h"				//  Header file for S75 Version 3 Class.
#include "A75.h"				//  Header File for the A75 Class
#include "PesMaker.h"			//  Header File for the PesMaker Class
#include "S75Plus.h"
#include "S75PlusPro.h"
#include "S75FK.h"
#include "S75FKPro.h"
#include "S75CA.h"
#include "S75CAPro.h"
#include "A75PesMaker.h"
//#ifdef JSAT
	#include "s75_jleo.h"		//  Header file for S75 JLEO Version Class.
//#endif
#include "S75IPRemap.h"
#include "s75.h"


#ifdef S75_DLL_EXPORTS
#define S75_DLL_API __declspec(dllexport)
#endif


//#ifdef JSAT

//typedef struct Novra_DeviceEntry {	// The information that we need about the device

//	BYTE DeviceIP[4];
//	BYTE DeviceMAC[6];
//	WORD StatusPort;
//	WORD DeviceType;

//} Novra_DeviceEntry;

//#endif




// FUNCTION DEFINITIONS
//----------------------------------------------------------------------------------

// getVersion

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
void __stdcall getVersion( 	int *major,	///< pointer to storage for version major number
							int *minor,	///< pointer to storage for version minor number
							int *day,	///< pointer to storage for day of month that the library was released
							int *month,	///< pointer to storage for month that the library was released
							int *year )	///< pointer to storage for year that the library was released
#else
S75_DLL_API void getVersion( int *major, int *minor, int *day, int *month, int *year )
#endif
#else
void getVersion( int *major, int *minor, int *day, int *month, int *year )
#endif
{
	// Returns the version of the library and the release date

	*major	= VERSION_MAJOR;
	*minor	= VERSION_MINOR;
	*day	= VERSION_DAY;
	*month	= VERSION_MONTH;
	*year	= VERSION_YEAR;
}

//----------------------------------------------------------------------------------

#ifdef JSAT

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall findAdapters( PIP_ADAPTER_INFO *adapters, ULONG *buffer_len )
#else
S75_DLL_API int findAdapters( PIP_ADAPTER_INFO *adapters, ULONG *buffer_len )
#endif
#else
int findAdapters( struct ifconf *adapters )
#endif
{
	int result;

#ifndef LINUX

	PIP_ADAPTER_INFO	adapter_buffer;         // Pointer to a network adapter information buffer

	ULONG			buf_len=10000;          // Amount of memory allocated for the adapter buffer


	if ( ( adapter_buffer = (PIP_ADAPTER_INFO)( new char[buf_len] ) ) != NULL ) {

		// Acquire adapter information, if the buffer is too small increase it until it is big enough

	        while ( ( result = GetAdaptersInfo( adapter_buffer, &buf_len ) ) == ERROR_BUFFER_OVERFLOW ) {

			// Buffer too small, increase its size and try again

			delete [] adapter_buffer;

			buf_len += 10000;

       			adapter_buffer = (PIP_ADAPTER_INFO)( new char[buf_len] );

		} // while

	} // if

	*adapters = adapter_buffer;

#else

 	int sockfd;

	// Get list of Interfaces
	adapters->ifc_len = 50 * sizeof( struct ifreq );		// We have enough memory for 50 interfaces
	adapters->ifc_ifcu.ifcu_buf = (char *)malloc( adapters->ifc_len );
	sockfd = socket( AF_INET, SOCK_DGRAM, 0 );
	result = ioctl( sockfd, SIOCGIFCONF, adapters );

	close( sockfd );

#endif

	return result;

}

#endif

//----------------------------------------------------------------------------------

#ifdef JSAT

#ifdef BROADCAST_POLL_FOR_STATUS
#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall pollDevicesOnLocalAdapter( Novra_DeviceList *devices, char *local_address )
#else
S75_DLL_API int pollDevicesOnLocalAdapter( Novra_DeviceList *devices, char *local_address )
#endif
#else
int pollDevicesOnLocalAdapter( Novra_DeviceList *devices, char *local_address )
#endif
{
	BYTE Buffer[sizeof(S75JLEO_Status_MSG)];
	S75JLEO_Status_MSG *status = NULL;
	char message[2];
	SOCKET send_socket;
	SOCKET receive_socket;
	int use_again=1;
	int use_broadcast=1;
	struct sockaddr_in device_address;
	struct sockaddr_in receive_address;
#ifndef LINUX
	int len = sizeof( receive_address );
#else
	socklen_t len = sizeof( receive_address );
#endif
	struct timeval timeout_struct;
	fd_set readfs;
	int result;
	int temp1 = 1000;               // Temporary storage variable
	Novra_DeviceEntry device_info;
	time_t start_time;
	bool timed_out = false;
	int       device_index;       // Index into the devices array
	bool      device_found;       // Flag to indicate if a device can be found in the list
	bool	bound;
	int		i;

//#ifndef LINUX

//	WSADATA	  wsa_data;           // Results of Winsock Startup Request

//	const int winsock_version=2;  // Request Winsock 2.0
//#endif

//#ifndef LINUX
//	WSAStartup( winsock_version, &wsa_data );
//#endif

	status = (S75JLEO_Status_MSG *)Buffer;

	message[0] = 0x19;
	message[1] = 0x61;

#ifndef LINUX
	if ( ( send_socket = socket( AF_INET, SOCK_DGRAM, 17 ) ) != INVALID_SOCKET ) {
	if ( ( receive_socket = socket( AF_INET, SOCK_DGRAM, 17 ) ) != INVALID_SOCKET ) {
#else
	if ( ( send_socket = socket( AF_INET, SOCK_DGRAM, 17 ) ) != -1 ) {
	if ( ( receive_socket = socket( AF_INET, SOCK_DGRAM, 17 ) ) != -1 ) {
#endif

		setsockopt( send_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)(&use_again), sizeof( use_again ) );
		setsockopt( send_socket, SOL_SOCKET, SO_BROADCAST, (const char *)(&use_broadcast), sizeof( use_broadcast ) );
		setsockopt( receive_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)(&use_again), sizeof( use_again ) );

   		device_address.sin_family = AF_INET;

   		device_address.sin_addr.s_addr = inet_addr( "255.255.255.255" );

   		device_address.sin_port = htons( 0x1963 );

		receive_address.sin_family = AF_INET;

   		receive_address.sin_addr.s_addr = inet_addr( local_address );

		bound = false;
		i = 0;
		while ( ( !bound ) && ( i < 10 ) ) {

   			receive_address.sin_port = htons( auto_detect_status_ports[i] );

			if ( bind ( send_socket, (struct sockaddr *)(&receive_address), sizeof( struct sockaddr ) ) == 0 ) {
				bound = true;
			} else {
				i++;
			}

		}

   		if ( connect( send_socket, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr ) ) == 0){

			getsockname( send_socket, (struct sockaddr *)(&receive_address), &len );
			receive_address.sin_addr.s_addr = INADDR_ANY;

			if ( bind ( receive_socket, (struct sockaddr *)(&receive_address), sizeof( struct sockaddr ) ) == 0 ) {

				if ( send( send_socket, (const char *)(&message), sizeof(message), 0 ) == sizeof( message ) ) {

					start_time = time( NULL );

					while ( ( !timed_out ) && ( devices->num_devices < MAX_DEVICES ) ) {

			           	timeout_struct.tv_sec = 1;
	
				       	timeout_struct.tv_usec = 0;

 						FD_ZERO( &readfs );

 			    		FD_SET( receive_socket, &readfs );

           				// Try to receive the responses from S75s.  Check for errors.
#ifndef LINUX
       					if ( ( result = select( 1, &readfs, NULL, NULL, &timeout_struct ) ) != SOCKET_ERROR ) {
#else
       					if ( ( result = select( receive_socket+1, &readfs, NULL, NULL, &timeout_struct ) ) != -1 ) {
#endif
   	        				if ( result != 0 ) {

#ifndef LINUX
   	        					if( (temp1 = recv( receive_socket, (char *)Buffer, sizeof(S75JLEO_Status_MSG), 0 ) ) != SOCKET_ERROR ) {

#else
   	        					if( (temp1 = recv( receive_socket, (char *)Buffer, sizeof(S75JLEO_Status_MSG), 0 ) ) != -1 ) {
#endif

    			   					// Make sure we have received the right number of bytes back

	   	        					if ( temp1 == sizeof(S75JLEO_Status_MSG) ) {

										if((status->DSPFirmWare&0xff) == 0x55){

											device_info.DeviceType = S75_JLEO;
											device_info.StatusPort = ntohs( status->UniCastStatusDestUDP );
											memcpy(device_info.DeviceMAC, status->ReceiverMAC, 6);
											memcpy(device_info.DeviceIP, status->ReceiverIP, 4);

											device_index = 0;
											device_found = false;

											// Loop through the device list comparing MAC addresses with those of this device.

											while ( ( device_index < devices->num_devices ) && (!device_found) ) {

												// Compare MAC addresses

												if ( memcmp(	device_info.DeviceMAC,
																devices->Device[device_index].DeviceMAC, 6 ) == 0 ) {
											
													device_found = true;
											
												} else {

													device_index++;
												}

											} // while

											if ( !device_found ) {
												// The device is not in the list. Add it.
												// Copy device details
											
												memcpy( (void *)(&(devices->Device[devices->num_devices])),
														(void *)(&device_info),
														sizeof( Novra_DeviceEntry ) );

												// Increment number of devices
												(devices->num_devices)++;

											}

										}

									} // if

								} // if recv

							} else {

								timed_out = true;	

							}

						} // if select

						if ( ( time( NULL ) - start_time ) > 1 ) {

							timed_out = true;

						}

					} // while

				} // if send

			} // if bind
	
		} // if socket connect


#ifndef LINUX
       	closesocket( receive_socket );
#else
       	close( receive_socket );
#endif

	} // if receive_socket create
#ifndef LINUX
       	closesocket( send_socket );
#else
       	close( send_socket );
#endif
	} // if send_socket create

	return 1;

}
#endif


#endif

//----------------------------------------------------------------------------------

#ifdef JSAT

#ifdef BROADCAST_POLL_FOR_STATUS
#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall pollDevices( Novra_DeviceList *devices )
#else
S75_DLL_API int pollDevices( Novra_DeviceList *devices )
#endif
#else
int pollDevices( Novra_DeviceList *devices )
#endif
{
	int result;

//#ifndef LINUX

//	WSADATA	  wsa_data;           // Results of Winsock Startup Request

//	const int winsock_version=2;  // Request Winsock 2.0
//#endif

#ifndef LINUX
	PIP_ADAPTER_INFO	adapter_info;           // Pointer to an adapter info structure
	PIP_ADAPTER_INFO	adapter_buffer;         // Pointer to a network adapter information buffer
	PIP_ADDR_STRING		ip_address;             // A pointer to an adapter IP address
#endif

	ULONG			buf_len=10000;          // Amount of memory allocated for the adapter buffer
	BYTE			*end_of_buffer;         // A pointer to the last byte in the adapter buffer


//#ifndef LINUX
//	WSAStartup( winsock_version, &wsa_data );
//#endif

#ifndef LINUX

	result = findAdapters( &adapter_buffer, &buf_len );

	if ( result == ERROR_SUCCESS ) {

		// Adapter info has been acquired

		// Set pointer to first adapter in the list

           	adapter_info = adapter_buffer;

		// Set a pointer to the last byte in the buffer. Don't search past this pointer.

           	end_of_buffer = (BYTE *)adapter_buffer + buf_len;

           	// Loop through the adapter list until the end of the adapter buffer has been reached

          	while ( ( adapter_info != NULL ) && ( (BYTE *)adapter_info < end_of_buffer ) ) {

			// Set pointer to first IP address of this adapter

		    	ip_address = &(adapter_info->IpAddressList);

			pollDevicesOnLocalAdapter( devices, ip_address->IpAddress.String );

               		// Keep looking ... go to next adapter

               		adapter_info = adapter_info->Next;

		}  // while

	}  // if

	delete [] adapter_buffer;

#else

 	struct ifconf adapters;
 	int if_count;
 	struct ifreq *ifr;
 	int i;

	if ( findAdapters( &adapters ) >= 0 ) {

        	// Got List of Interfaces

		if_count = adapters.ifc_len / sizeof( struct ifreq );

		for ( int i = 0; i < if_count; i++ ) {

			ifr = adapters.ifc_ifcu.ifcu_req + i ;
			if(strcmp(ifr->ifr_name, "lo") != 0){

				pollDevicesOnLocalAdapter( devices, inet_ntoa( ((struct sockaddr_in *)(&(ifr->ifr_ifru.ifru_addr)))->sin_addr ) );

			}
		}

	}

	free( adapters.ifc_ifcu.ifcu_buf );

#endif

	return 1;

}
#endif

#endif

//----------------------------------------------------------------------------------

// discoverDevices

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall discoverDevices( Novra_DeviceList *devices )	///< pointer to a structure of type Novra_DeviceList
#else
S75_DLL_API int discoverDevices( Novra_DeviceList *devices )
#endif
#else
int discoverDevices( Novra_DeviceList *devices )
#endif
/**
*
* Results are returned in the Novra_DeviceList pointed to by devices
*
* Returns 0 on failure, 1 on success
*
*/
{

    // LOCAL VARIABLES

#ifndef LINUX

	WSADATA	  wsa_data;           // Results of Winsock Startup Request

	const int winsock_version=2;  // Request Winsock 2.0
#endif

	CDeviceIdentifier S75Lookup[10];          // S75 device Identifier Class

	int       port_index;         // Index into auto detection ports array

	BOOL      keep_looking=true;  // Flag used to indicate when no new devices
	                              // can be seen on a port

	const int timeout= 3000;       // Maximum time to wait for a status packet

	int       device_index;       // Index into the devices array

	BOOL      device_found;       // Flag to indicate if a device can be found in the list

	int       byte_index;         // Index into a byte array (IP and MAC addresses)

	unsigned long      last_time;          // Last time a new device was found

	DWORD	  up_time;			// Status Up Time

	int       success = 0;        // Return Value

	int		LastError;			// Error code from last operation.

	Novra_DeviceEntry temp_dev;		// temporary device Info container




	devices->num_devices = 0;
#ifndef LINUX
	WSAStartup( winsock_version, &wsa_data );
#endif

    // Set up the status ports of the classes to the elements of the auto detect array.
	for(port_index =0 ; port_index< num_auto_detect_ports; port_index ++){
		S75Lookup[port_index].Set_Status_Port(auto_detect_status_ports[port_index]);		// Configure the Status Port .
		S75Lookup[port_index].StartStatusListener();									// And start the listner.
	}

	for(port_index =0 ; port_index< num_auto_detect_ports; port_index ++){ // For each of our objects...

		keep_looking = true;

		// On each port, loop until a new device can not be found

		while ( keep_looking && ( devices->num_devices < MAX_DEVICES ) ) {
			up_time = S75Lookup[port_index].GetStatusUpTime();


 			if( (up_time > 3) && (up_time != ERR_INVALID_TIME) )	// if it has been more than one second since we are listening.
				LastError = S75Lookup[port_index].WaitForStatusPacket(1000);	// wait only 1 second for a packet.
																				// DO NOT DECREASE THIS BELOW 1 SECOND
																				// UNRELIABLE DEVICE DETECTION WAS OBSERVED
			else
				LastError = S75Lookup[port_index].WaitForStatusPacket(timeout);		// wait the prescribed time out;

			if(LastError == ERR_SUCCESS){

				//printf("\nSomeone is sending so something on port %d \n",auto_detect_status_ports[port_index]);
				// Found a device.  See if this device has been seen already.
				S75Lookup[port_index].GetdeviceEntry( &temp_dev);	// Get the device information .

				device_index = 0;

				device_found = false;

				// Loop through the device list comparing MAC addresses with those of this device.

				while ( ( device_index < devices->num_devices ) && (!device_found) ) {

					device_found = true;

					// Compare MAC addresses

					byte_index = 0;

					// Loop through the 6 bytes of the MAC address.

					while ( ( byte_index < 6 ) && device_found ) {

						if ( temp_dev.DeviceMAC[byte_index] !=
									 devices->Device[device_index].DeviceMAC[byte_index] ) {
							device_found = false;
						} // if
						byte_index++;
					} // while

					byte_index = 0;

					// Loop through the 4 bytes of the IP address.

					while ( ( byte_index < 4 ) && device_found ) {

						if ( temp_dev.DeviceIP[byte_index] !=
									 devices->Device[device_index].DeviceIP[byte_index] ) {
							device_found = false;
						} // if
						byte_index++;
					} // while

					device_index++;

				} // while

				if ( !device_found ) {
					// The device is not in the list. Add it.
					// Copy device details

					S75Lookup[port_index].GetdeviceEntry(&devices->Device[devices->num_devices]) ;

					// Increment number of devices
					(devices->num_devices)++;

					// Record the time that this device was found
					last_time = time(NULL);
				}
				else {
					// The device has been seen before
					// If it has been more than 3 seconds since a new device has been found then
					// stop looking on this port
					if ( time(NULL) > ( last_time + 3 ) ) {
					keep_looking = false;
					} //if
				} // else if
			}
			else {
				

				// No status packet received on this port
				keep_looking = false;
			} // else if (Was status Received
		} // while (looking on this port as long as there is packets and time)

				// If there are more ports to scan, go to the next one

		S75Lookup[port_index].StopStatusListener(); // we don't need this guy to listen

	} // for

	// Finished scanning ports

//#ifdef JSAT

	// Scan for non-chatty devices

	#ifdef BROADCAST_POLL_FOR_STATUS
		pollDevices( devices );
	#endif

//#endif

	success = 1;	   	

	return success;

} // discoverDevices

//----------------------------------------------------------------------------------

// openDevice

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
S75_Handle __stdcall openDevice(	Novra_DeviceEntry * pDev,	///< pointer to a Novra_DeviceEntry identifying the device to open (input)
									const int  timeout,			///< timeout in milliseconds (input)
									int & ErrorCode )			///< error code (output)
#else
S75_DLL_API S75_Handle openDevice( Novra_DeviceEntry * pDev, const int  timeout, int & ErrorCode)
#endif
#else
S75_Handle openDevice( Novra_DeviceEntry * pDev, const int  timeout, int & ErrorCode)
#endif
/**
*
* Returns a valid S75_Handle or a NULL pointer with ErrorCode set
*
*/
{

	// This function creates a handle for communicating with an S75 receiver


    // LOCAL VARIABLES

// @@@DBW	void	*S75=NULL;
	CNovraDevice	*S75=NULL;	// @@@DBW

#ifndef LINUX
	WSADATA	  wsa_data;               // Results of Winsock Startup Request

	const int winsock_version=2;      // Request Winsock 2.0

	WSAStartup( winsock_version, &wsa_data );
#endif

	switch (pDev->DeviceType)
	{
		case S75D_V21:
			S75 =  new CS75_V2();
			break;

		case S75D_V3:
			S75 = new CS75_V3();
			break;
		case A75:
			S75 =  new CA75();
			break;
		case PesMaker: 
			S75 = new CPesMaker();
			break;
		case S75PLUS:
			S75 = new CS75Plus();
			break;
		case S75CA:
			S75 = new CS75CA();
			break;
		case S75CAPRO:
			S75 = new CS75CAPro();
			break;
		case S75PLUSPRO:
			S75 = new CS75PlusPro();
			break;
		case S75FK:
			S75 = new CS75FK();
			break;
		case S75FKPRO:
			S75 = new CS75FKPro();
			break;
		case A75PesMaker:
			S75 = new CA75PesMaker();
			break;
//#ifdef JSAT
		case S75_JLEO:
			S75 = new CS75_JLEO();
			break;
//#endif
		case S75IPREMAP:
			S75 = new CS75IPRemap();
			break;

		default:
			{
				S75 = NULL;
				ErrorCode = ERR_DEVICE_TYPE_UNKNOWN;
				break;
			}
	}
	if ( S75 != NULL ) {

	    // Set information needed to communicate with the unit

		if( (ErrorCode= ( (CNovraDevice *) S75)->Attach_Device(pDev) ) != Attach_Success){
			delete S75;
			return NULL;
		}


		// Listen for the device

//#ifdef JSAT
#ifdef BROADCAST_POLL_FOR_STATUS
		if ( pDev->DeviceType != S75_JLEO ) {
#endif
//#endif


		if (( ErrorCode = ( (CNovraDevice *) S75)->StartStatusListener() ) == ERR_SUCCESS) {

			// Filter Status packets based on MAC address but not IP address

			((CNovraDevice *)S75)->FilterStatus(true);

    	    // If no status packet is successfully received before the request times out, return a NULL handle

			if ( ( ErrorCode = ( (CNovraDevice *) S75)->WaitForStatusPacket(timeout) ) != ERR_SUCCESS ) {

		        ((CNovraDevice *)S75)->StopStatusListener();

         		delete S75;

				S75 = NULL;

			} //if

		} else {

			// Could not start the status listener, return NULL handle

        	delete S75;

	      	S75 = NULL;

		} // else if

//#ifdef JSAT
#ifdef BROADCAST_POLL_FOR_STATUS
		}
#endif
//#endif

	} // if

	return S75;

} // openDevice





//----------------------------------------------------------------------------------

// b_openS75

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
S75_Handle __stdcall b_openS75(	const BYTE S75IPAddress[4],	///< a 4 byte array containing the IP address of the device
		 	    	const BYTE S75MACAddress[6],			///< a 6 byte array containing the MAC address of the device
			    	const int  *statusPort,					///< a pointer to an int containing the status port used by the device
			    	const int  *timeout          )			///< a pointer to an int containing a timeout (in milliseconds)
#else
S75_DLL_API S75_Handle b_openS75(	const BYTE S75IPAddress[4],
					const BYTE S75MACAddress[6],
					const int  *statusPort,
					const int  *timeout          )
#endif
#else
S75_Handle b_openS75(	const BYTE S75IPAddress[4],
			const BYTE S75MACAddress[6],
			const int  *statusPort,
			const int  *timeout          )
#endif
/**
*
* Returns a valid S75_Handle or a NULL pointer
*
* This function is deprecated and should not be used.  Use openDevice instead.
*
*/
{

	// This function creates a handle for communicating with an S75 receiver


    // LOCAL VARIABLES

	CDeviceIdentifier S75Lookup;          // S75 device Identifier Class
	int i;
	Novra_DeviceEntry temp_dev;		// temporary device Info container
	int Error;

#ifndef LINUX
	WSADATA	  wsa_data;               // Results of Winsock Startup Request
	const int winsock_version=2;      // Request Winsock 2.0
	WSAStartup( winsock_version, &wsa_data );
#endif

	//	Now we have to detect what type the devvice is...

	S75Lookup.Set_Device_IP_Address( (BYTE*) S75IPAddress);
	S75Lookup.Set_Device_MAC_Address( (BYTE*) S75MACAddress);
	S75Lookup.Set_Status_Port(*statusPort);
	S75Lookup.FilterStatus(true);

	if((Error = S75Lookup.StartStatusListener())!= ERR_SUCCESS) return NULL;
	if((Error = S75Lookup.WaitForStatusPacket(*timeout))!= ERR_SUCCESS) return NULL;
	S75Lookup.GetdeviceEntry(&temp_dev);
	return openDevice(&temp_dev, *timeout, i);

} // b_openS75

//----------------------------------------------------------------------------------

// s_openS75

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
S75_Handle __stdcall s_openS75(	const char *S75IPAddress,	///< a string containing the IP address of the device (in the form ddd.ddd.ddd.ddd)
								const char *S75MACAddress,	///< a string containing the MAC address of the device (in the form xx-xx-xx-xx-xx-xx)
			    				const int  *statusPort,         ///< a pointer to an int containing the status port used by the device
			    				const int  *timeout          )	///< a pointer to an int containing a timeout (in milliseconds)
#else
S75_DLL_API S75_Handle s_openS75(	const char *S75IPAddress,
					const char *S75MACAddress,
					const int  *statusPort,
					const int  *timeout        )
#endif
#else
S75_Handle s_openS75(	const char *S75IPAddress,
			const char *S75MACAddress,
			const int  *statusPort,
			const int  *timeout        )
#endif
/**
*
* Returns a valid S75_Handle or a NULL pointer
*
* This function is deprecated and should not be used.  Use openDevice instead.
*
*/
{

	// This function creates a handle for communicating with an S75 receiver


    // LOCAL VARIABLES

	int   b0, b1, b2, b3, b4, b5;    // Temporary storage used when reading bytes
	                                 // out of input strings

	BYTE  IPAddress[4];              // IP Address as byte array

	BYTE  MACAddress[6];             // MAC Address as byte array



	// Convert IP address string to byte array

	sscanf( S75IPAddress, "%d.%d.%d.%d", &b0, &b1, &b2, &b3 );
	IPAddress[0] = b0;
	IPAddress[1] = b1;
	IPAddress[2] = b2;
	IPAddress[3] = b3;
	
	// Convert MAC address string to byte array

	sscanf( S75MACAddress, "%x-%x-%x-%x-%x-%x", &b0, &b1, &b2, &b3, &b4, &b5 );
	MACAddress[0] = b0;
	MACAddress[1] = b1;
	MACAddress[2] = b2;
	MACAddress[3] = b3;
	MACAddress[4] = b4;
	MACAddress[5] = b5;

	// Call sister function and return

    return b_openS75( IPAddress, MACAddress, statusPort, timeout );

} // s_openS75

//----------------------------------------------------------------------------------

// configureNetwork

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall configureNetwork(	S75_Handle S75Unit,			///< The S75_Handle of the device whose network settings are being configured
								BYTE* networkSettings )	///< A pointer to a structure containing the network settings to use (one of S75D_NetSetting, S75D_NetSetting_V3, A75_NetSetting, A75PesMaker_NetSetting, PesMaker_NetSetting, S75_NetSetting_JLEO, S75Plus_NetSetting, S75PlusPro_NetSetting, S75CA_NetSetting, S75CAPro_NetSetting, S75FK_NetSetting, S75FKPro_NetSetting)
#else
S75_DLL_API int configureNetwork(	S75_Handle S75Unit,
					BYTE* networkSettings )
#endif
#else
int  configureNetwork(	S75_Handle S75Unit,
			BYTE* networkSettings )
#endif
/**
*
* Returns S75_OK or an ERROR code (see file s75.h)
*
* The following networkSettings types are all equivalent to the S75D_NetSetting_V3 structure
*
* - A75_NetSetting
*
* - A75PesMaker_NetSetting
*
* - PesMaker_NetSetting
*
* - S75_NetSetting_JLEO
*
* - S75Plus_NetSetting
*
* - S75PlusPro_NetSetting
*
* - S75CA_NetSetting
*
* - S75CAPro_NetSetting
*
* - S75FK_NetSetting
*
* - S75FKPro_NetSetting
*
*/
{

	// This function configures the network parameters belonging to an S75


    // LOCAL VARIABLES

	CNovraDevice		*S75;                   // Device handle as an CS75D object

	//ULONG			buf_len=10000;          Amount of memory allocated for the adapter buffer
	//DWORD			result;                 Result of the call to get adapter info
	// BYTE			*end_of_buffer;         A pointer to the last byte in the adapter buffer
	BOOL			config_success=false;   // Flag indicating if our device has been configured
	// BYTE			local_ip[4];            A byte array representing a local IP address
	// int			b0, b1, b2, b3;         Temporary values
	int			LastError;
	//BYTE		oldDeviceIP[4];
#ifndef LINUX
	PIP_ADAPTER_INFO	adapter_info;           // Pointer to an adapter info structure
	PIP_ADAPTER_INFO	adapter_buffer;         // Pointer to a network adapter information buffer
	PIP_ADDR_STRING		ip_address;             // A pointer to an adapter IP address
#endif
	

	S75 = (CNovraDevice *)S75Unit;



   // No assumption about local network interfaces or subnet addresses being matched
	// to the current IP address of the device is made and therefore configuration
	// is achieved by attempting to configure the device on each network adapter
	// until we are acknowledged by the device

	// Allocate some memory for adapter information
//@@@DBW April 16, 2009	if(S75->SetNetworkPath(NULL, false, false) == ERR_NETPATH_ALREADYSET){
		if ((LastError = S75->ConfigureNetwork((void *)networkSettings))==ERR_SUCCESS) {
			config_success = true;
			return S75_OK;
		} else {	//@@@DBW April 16, 2009
			return S75_FAILED;	//@@@DBW April 16, 2009
		}
//@@@DBW April 16, 2009	}
/*@@@DBW April 16, 2009
	S75->Get_Device_IP_Address(oldDeviceIP);
	if(oldDeviceIP[0]==0){
		oldDeviceIP[0]=10;
		oldDeviceIP[1]=10;
		oldDeviceIP[2]=10;
		oldDeviceIP[3]=10;
	
	}
	S75->Set_Device_IP_Address(oldDeviceIP);
#ifndef LINUX
	if ( ( adapter_buffer = (PIP_ADAPTER_INFO)( new char[buf_len] ) ) != NULL ) {

		// Acquire adapter information, if the buffer is too small increase it until it is big enough

        while ( ( result = GetAdaptersInfo( adapter_buffer, &buf_len ) ) == ERROR_BUFFER_OVERFLOW ) {

			// Buffer too small, increase its size and try again

			delete [] adapter_buffer;

			buf_len += 10000;

            adapter_buffer = (PIP_ADAPTER_INFO)( new char[buf_len] );

		} // while

		if ( result == ERROR_SUCCESS ) {

			// Adapter info has been acquired

			// Set pointer to first adapter in the list

            adapter_info = adapter_buffer;

			// Set a pointer to the last byte in the buffer. Don't search past this pointer.

            end_of_buffer = (BYTE *)adapter_buffer + buf_len;

            // Loop through the adapter list until we have successfully configured our device,
			// there are no more adapters to try, or the end of the adapter buffer has been reached

            while ( (!config_success) && ( adapter_info != NULL ) && ( (BYTE *)adapter_info < end_of_buffer ) ) {

				// Set pointer to first IP address of this adapter

		    	ip_address = &(adapter_info->IpAddressList);

				// Loop through all the IP addresses belonging to this adapter until our device
				// is configured or there are no more IP addresses associated with this adapter

                while( (!config_success) && ( ip_address != NULL ) ) {

					// Convert the address string to a byte array

				    sscanf( ip_address->IpAddress.String, "%d.%d.%d.%d", &b0, &b1, &b2, &b3 );
                    local_ip[0] = b0;
					local_ip[1] = b1;
					local_ip[2] = b2;
					local_ip[3] = b3;

					// Set up temporary route and ARP entries that force the network config packet out of this adapter

                    S75->SetNetworkPath( local_ip, true, true );

					// Try and configure our device

					if ((LastError = S75->ConfigureNetwork((void *)networkSettings))==ERR_SUCCESS) {

		        		config_success = true;

					} // if

					// Remove the temporary route and ARP entries we made

					S75->RemoveNetworkPath();

				    if ( !config_success ) {

					    // Keep Looking ... go to next IP address


						ip_address = ip_address->Next;

					} //  if

				} // while

                if ( !config_success ) {

                    // Keep looking ... go to next adapter

                    adapter_info = adapter_info->Next;

				}  // if

			}  // while

		}  // if

	    delete [] adapter_buffer;

	} // if

	// if the function is successful, set up permanent route and ARP entries for our device

	if (config_success) {

		// Set up route and ARP entries

		// @@NS
		// We Shouldn't enforce a Netowrk path Devices should be set according to network standards.
		// If the user wants to get nitty it is his job.
		//LastError = S75->SetNetworkPath( local_ip, true, true );

		return S75_OK;

	} else {

		return LastError;

	}//if
#else

 	int sockfd;
 	struct ifconf ifc;
 	struct ifreq *ifr;
 	int if_count;
 	int i;

    // Get list of Interfaces
    ifc.ifc_len = 50 * sizeof( struct ifreq );		// We have enough memory for 50 interfaces
    ifc.ifc_ifcu.ifcu_buf = (char *)malloc( ifc.ifc_len );
    sockfd = socket( AF_INET, SOCK_DGRAM, 0 );
    config_success = false ;
    if ( ioctl( sockfd, SIOCGIFCONF, &ifc ) >= 0 ) {
        // Got List of Interfaces

        if_count = ifc.ifc_len / sizeof( struct ifreq );
	//printf("\n the machine has %d Interface(s)", if_count);
		for ( int i = 0; i < if_count; i++ ) {
			ifr = ifc.ifc_ifcu.ifcu_req + i ;
			if(strcmp(ifr->ifr_name, "lo") != 0){

				S75->SetNetworkPath( local_ip, true, true, (BYTE *)ifr->ifr_name);
				if ((LastError = S75->ConfigureNetwork((void *)networkSettings))==ERR_SUCCESS) {

					S75->RemoveNetworkPath();
					config_success = true;

  					break;
				}else{
					S75->RemoveNetworkPath();
				}
			}
		}

	}else{
		return S75_FAILED ;
	}
	if(config_success){
		// @@NS
		// We Shouldn't enforce a Netowrk path Devices should be set according to network standards.
		// If the user wants to get nitty it is his job.
		//S75->SetNetworkPath( local_ip, true, true, (BYTE *)ifr->ifr_name);
		return S75_OK;
	}
	else
		return LastError;



    close( sockfd );
    free( ifc.ifc_ifcu.ifcu_buf );
#endif
@@@DBW April 16, 2009 */

} //configureNetwork

//----------------------------------------------------------------------------------

// forceDevice

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
S75_Handle __stdcall forceDevice(	const Novra_DeviceEntry *pDev,	///< pointer to the Novra_DeviceEntry of the device being forced
									WORD Port,						///< the status port the device should use
									const int timeout,				///< a pointer to an int containing a timeout (in milliseconds)
									int &ErrorCode               )	///< error code (output)
#else
S75_DLL_API S75_Handle forceDevice(	const Novra_DeviceEntry *pDev,
					WORD Port,
					const int timeout,
					int &ErrorCode                 )
#endif
#else
S75_Handle forceDevice(	const Novra_DeviceEntry *pDev,
			WORD Port,
			const int timeout,
			int &ErrorCode                 )
#endif
/**
*
* Resets a device's network settings to the default and provides an S75_Handle to the device.
* In general, use of this functionality should be avoided.  Use it when a device can't be reached using other means.
*
* Returns a valid S75_Handle or a NULL pointer and an Error code (see file s75.h)
*
*/
{
	bool StatusPortValid ;
	int i;
	CNovraDevice *S75;
	BYTE ConfigBuffer[1500] ; // generic buffer to hold any configuration structure
	StatusPortValid = false;
	// We need to make sure that the port will support auto detection.
	// The user should use get available port
	for(i=0; i< num_auto_detect_ports; i++){
		if(Port == auto_detect_status_ports[i]){
			StatusPortValid = true;
			break;
		}
	}
	if(!StatusPortValid){
		ErrorCode = ERR_BAD_STATUS_PORT;
		return NULL;
	}
	switch (pDev->DeviceType)
	{
		case S75D_V21:
			S75 = (CNovraDevice *) new CS75_V2();
			break;
		case S75D_V3:
			S75 = (CNovraDevice *) new CS75_V3();
			break;
		case A75:
			S75 = (CNovraDevice *) new CA75();
			break;
		case PesMaker:
			S75 = (CNovraDevice *) new CPesMaker();
			break;
		case A75PesMaker:
			S75 = (CNovraDevice *) new CA75PesMaker();
			break;
		case S75PLUS:
			S75 = (CNovraDevice *) new CS75Plus();
			break;
		case S75CA:
			S75 = (CNovraDevice *) new CS75CA();
			break;
		case S75CAPRO:
			S75 = (CNovraDevice *) new CS75CAPro();
			break;
		case S75PLUSPRO:
			S75 = (CNovraDevice *) new CS75PlusPro();
			break;
		case S75FK:
			S75 = (CNovraDevice *) new CS75FK();
			break;
		case S75FKPRO:
			S75 = (CNovraDevice *) new CS75FKPro();
			break;
//#ifdef JSAT
		case S75_JLEO:
			S75 = (CNovraDevice *) new CS75_JLEO();
			break;
//#endif
		case S75IPREMAP:
			S75 = (CNovraDevice *) new CS75IPRemap();
			break;

		default:
			{
				S75 = NULL;
				ErrorCode = ERR_DEVICE_TYPE_UNKNOWN;
				break;
			}
	}
	if(S75 == NULL) return S75;

	S75->Attach_Device((void *)pDev); // The value in the status port doesn't matter
	S75->Set_Status_Port( Port);	 // cause here we are changing it
	if(!	S75->GetDefaultConfigStruct(ConfigBuffer)){ // here we get a default configuration for the device
		ErrorCode = ERR_NO_DefaultConfig;
		delete S75;
		return NULL;
	}

	ErrorCode = configureNetwork(S75, (BYTE*)ConfigBuffer); // and we configure the device

	// Listen for the device
	if ( (ErrorCode = S75->StartStatusListener()) == ERR_SUCCESS) {
		// Filter Status packets based on MAC address but not IP address
		S75->FilterStatus( true);
		// If no status packet is successfully received before the request times out, return a NULL handle

		if ((ErrorCode = S75->WaitForStatusPacket( timeout))!=ERR_SUCCESS ) {

			S75->StopStatusListener();

			delete S75;

			S75 = NULL;

		} //if

	} else {

		// Could not start the status listener, return NULL handle

        delete S75;

	    S75 = NULL;

	} // else if

	return S75;
}

//----------------------------------------------------------------------------------

// b_forceS75

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
S75_Handle __stdcall b_forceS75(	const BYTE S75MACAddress[6],	///< a 6 byte array containing the MAC address of the device
									const BYTE S75IPAddress[4],	///< a 4 byte array containing the new IP address for the device
									const int  *statusPort,		///< a pointer to an int containing the status port the device is to use
									const int  *timeout          )	///< a timeout (in milliseconds)
#else
S75_DLL_API S75_Handle b_forceS75(	const BYTE S75MACAddress[6],
					const BYTE S75IPAddress[4],
					const int  *statusPort,
					const int  *timeout          )
#endif
#else
S75_Handle b_forceS75(	const BYTE S75MACAddress[6],
			const BYTE S75IPAddress[4],
			const int  *statusPort,
			const int  *timeout          )
#endif
/**
*
* This function is deprecated and should not be used.  Use ForceDevice instead.
*
* This function creates a handle for communicating with an S75 receiver
* by forcing the S75 with the specified MAC address to accept a new
* IP address and status port.  This is different from the open functions
* because they do not change any of the device network settings, whereas
* the force functions destroy whatever network configuration previously
* existed.
*
* Returns a valid S75_Handle or a NULL pointer
*
*/
{

	// This function creates a handle for communicating with an S75 receiver
	// by forcing the S75 with the specified MAC address to accept a new
	// IP address and status port.  This is different from the open functions
	// because they do not change any of the device network settings, whereas
	// the force functions destroy whatever network configuration previously
	// existed.


    // LOCAL VARIABLES

	CDeviceIdentifier S75Lookup;          // S75 device Identifier Class

	int i;
	Novra_DeviceEntry temp_dev;		// temporary device Info container
	int Error;

#ifndef LINUX
	WSADATA	  wsa_data;               // Results of Winsock Startup Request

	const int winsock_version=2;      // Request Winsock 2.0

	WSAStartup( winsock_version, &wsa_data );
#endif
	//	Now we have to detect what type the devvice is...

	S75Lookup.Set_Device_IP_Address( (BYTE*) S75IPAddress);
	S75Lookup.Set_Device_MAC_Address( (BYTE*) S75MACAddress);
	S75Lookup.Set_Status_Port(*statusPort);
	S75Lookup.FilterStatus(true);

	if((Error = S75Lookup.StartStatusListener())!= ERR_SUCCESS) return NULL;
	if((Error = S75Lookup.WaitForStatusPacket(*timeout))!= ERR_SUCCESS) return NULL;
	S75Lookup.GetdeviceEntry(&temp_dev);
	return forceDevice(&temp_dev, *statusPort,*timeout, i);

} // b_forceS75

//----------------------------------------------------------------------------------

// s_forceS75

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
S75_Handle __stdcall s_forceS75(	const char *S75MACAddress,	///< a string containing the MAC address of the device (in the form xx-xx-xx-xx-xx-xx)
									const char *S75IPAddress,	///< a string containing the IP address of the device (in the form ddd.ddd.ddd.ddd)
									const int  *statusPort,		///< a pointer to an int containing the status port used by the device
									const int  *timeout        )	///< a pointer to an int containing a timeout (in milliseconds)
#else
S75_DLL_API S75_Handle s_forceS75(	const char *S75MACAddress,
					const char *S75IPAddress,
					const int  *statusPort,
					const int  *timeout        )
#endif
#else
S75_Handle  s_forceS75(	const char *S75MACAddress,
			const char *S75IPAddress,
			const int  *statusPort,
			const int  *timeout        )
#endif
/**
*
* This function is deprecated and should not be used.  Use ForceDevice instead.
*
* This function creates a handle for communicating with an S75 receiver
* by forcing the S75 with the specified MAC address to accept a new
* IP address and status port.  This is different from the open functions
* because they do not change any of the device network settings, whereas
* the force functions destroy whatever network configuration previously
* existed.
*
* Returns a valid S75_Handle or a NULL pointer
*
*/

{


    // LOCAL VARIABLES

	int   b0, b1, b2, b3, b4, b5;    // Temporary storage used when reading bytes
	                                 // out of input strings

	BYTE  IPAddress[4];              // IP Address as byte array

	BYTE  MACAddress[6];             // MAC Address as byte array



	// Convert IP address string to byte array

	sscanf( S75IPAddress, "%d.%d.%d.%d", &b0, &b1, &b2, &b3 );
	IPAddress[0] = b0;
	IPAddress[1] = b1;
	IPAddress[2] = b2;
	IPAddress[3] = b3;

	// Convert MAC address string to byte array

	sscanf( S75MACAddress, "%x-%x-%x-%x-%x-%x", &b0, &b1, &b2, &b3, &b4, &b5 );
	MACAddress[0] = b0;
	MACAddress[1] = b1;
	MACAddress[2] = b2;
	MACAddress[3] = b3;
	MACAddress[4] = b4;
	MACAddress[5] = b5;

	// Call sister function and return

    return b_forceS75( MACAddress, IPAddress, statusPort, timeout );

} // s_forceS75

//----------------------------------------------------------------------------------

// closeS75

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall closeS75( S75_Handle S75Unit )	///< the S75_Handle of the device to be closed
#else
S75_DLL_API int closeS75( S75_Handle S75Unit )
#endif
#else
int closeS75( S75_Handle S75Unit )
#endif
{

	// This function disposes of a handle for communicating with an S75 receiver


    // LOCAL VARIABLES

	CNovraDevice * S75;



	if ( ( S75 = (CNovraDevice *)S75Unit ) != NULL ) {

		// Close the status port
		//S75->RemoveNetworkPath();
		S75->StopStatusListener();

	    delete S75;

		S75 = NULL;

	}

	return S75_OK;

}//closeS75

//----------------------------------------------------------------------------------

// configureRF

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall configureRF(	S75_Handle S75Unit,	///< The S75_Handle of the device whose network settings are being configured
							BYTE*  RFSettings )	///< A pointer to a structure containing the RF settings to use (one of S75D_RFSetting, S75D_RFSetting_V3, A75_RFSetting, A75PesMaker_RFSetting, PesMaker_RFSetting, S75_RFSetting_JLEO, S75Plus_RFSetting, S75PlusPro_RFSetting, S75CA_RFSetting, S75CAPro_RFSetting, S75FK_RFSetting, S75FKPro_RFSetting)
#else
S75_DLL_API int configureRF(	S75_Handle S75Unit,
				BYTE*  RFSettings )
#endif
#else
int configureRF(	S75_Handle S75Unit,
			BYTE*  RFSettings )
#endif

/**
*
* Returns S75_OK or an ERROR code (see file s75.h)
*
* The following networkSettings types are all equivalent to the S75D_RFSetting_V3 structure
*
* - PesMaker_RFSetting
*
* The following networkSettings types are all equivalent to the S75Plus_RFSetting structure
*
* - S75PlusPro_RFSetting
*
* - S75CA_RFSetting
*
* - S75CAPro_RFSetting
*
* - S75FK_RFSetting
*
* - S75FKPro_RFSetting
*
* The following networkSettings types are all equivalent to the A75_RFSetting structure
*
* - A75PesMaker_RFSetting
*
*/
{ 

	// This function configures the RF parameters belonging to an S75


    // LOCAL VARIABLES

	CNovraDevice *S75;    // Device handle as an CS75D object
	int LastError;



	S75 = (CNovraDevice *)S75Unit;

	// Send the RF settings to the unit, if the function is successful, return OK

	if ( (LastError = S75->TuneDevice( (void *)RFSettings)) == ERR_SUCCESS) {
		return S75_OK;
	} 
	else{
		return LastError;
	}//if

} //configureRF

//----------------------------------------------------------------------------------

// getProgramList

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall getProgramList(	S75_Handle S75Unit,	///< The S75_Handle of the device whose programs are being obtained
								BYTE *Programs          )	///< A pointer to a structure to receive the Programs (either an S75CA_ProgramList or an S75CAPro_ProgramList)
#else
S75_DLL_API int getProgramList(	S75_Handle S75Unit,
								BYTE *Programs          )
#endif
#else
int  getProgramList(	S75_Handle S75Unit,
						BYTE *Programs          )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
* S75CA_ProgramList and S75CAPro_ProgramList are equivalent structure types.
*
*/
{

	// This function gets the list of Programs in the Program filter of a device


    // LOCAL VARIABLES

	CNovraDevice  *S75;    // Device handle as an CS75D object

	int    LastError;    // Indices



	S75 = (CNovraDevice *)S75Unit;

	// Retrieve the Program list from the unit

	if ( (LastError = S75->GetProgramTable((BYTE *)Programs)) == ERR_SUCCESS ) {

		return S75_OK;

	} else {

		return LastError;

	}//if

} //getProgramList

//----------------------------------------------------------------------------------

// setProgramList

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall setProgramList(	S75_Handle S75Unit,	///< The S75_Handle of the device whose programs are being configured
								BYTE *Programs      )	///< A pointer to a structure to receive the Programs (either an S75CA_ProgramList or an S75CAPro_ProgramList)
#else
S75_DLL_API int setProgramList(	S75_Handle S75Unit,
								BYTE *Programs     )
#endif
#else
int setProgramList(	S75_Handle S75Unit,
					BYTE *Programs      )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
* S75CA_ProgramList and S75CAPro_ProgramList are equivalent structure types.
*
*/
{

	// This function sets the Program filter on an S75CA


    // LOCAL VARIABLES

	CNovraDevice *S75;                        // Device handle as a CS75CA object

    int   i, j;                        // Indices

	bool  duplicates_exist = false;

	/* bool  program_out_of_range = false; */

	int   result = S75_FAILED;



	S75 = (CNovraDevice *)S75Unit;

	if( ( S75->GetDeviceType() == S75CA ) || ( S75->GetDeviceType() == S75CAPRO ) ) {

		S75CA_ProgramList  *PList = NULL; 
//		S75CA_ProgramList	*pProgramList = NULL;


		PList = (S75CA_ProgramList * ) Programs ;
		for ( i = 0; ( i < SIZE_PROGRAM_LIST ) && ( !duplicates_exist ); i++ ) {
			
			// Check for duplicate Program
				
			if ( PList->ProgramNumber[i] ) {
	
				for ( j = 0; ( j < SIZE_PROGRAM_LIST ) && ( !duplicates_exist ); j++ ) {
					
					if ( i != j ) {
						
						if ( PList->ProgramNumber[i] == PList->ProgramNumber[j] ) {
							
							duplicates_exist = true;
							
						} // if
						
					} // if
					
				} // for

			}
			
		} // for
		
		
		// If the list is OK then proceed
		
		if ( !duplicates_exist )
			result = S75->SetProgramTable((BYTE *)Programs);
		else
			result = ERR_PROGRAMSET_NOTAPPLIED;

	}

	return  result;

} //setProgramList

//----------------------------------------------------------------------------------

// getPAT

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall getPAT(	S75_Handle S75Unit,		///< The S75_Handle of the device whose PAT is being obtained
						BYTE *PAT          )	///< A pointer to a structure to receive the PAT (one of S75PlusPro_PAT, S75CAPro_PAT or S75FKPro_PAT)
#else
S75_DLL_API int getPAT(	S75_Handle S75Unit,
						BYTE *PAT          )
#endif
#else
int  getPAT(	S75_Handle S75Unit,
				BYTE *PAT          )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
* S75PlusPro_PAT, S75CAPro_PAT and S75FKPro_PAT are equivalent stucture types
*
*/
{

	// This function gets the PAT from a device


    // LOCAL VARIABLES

	CNovraDevice  *S75;    // Device handle as an CS75D object

	int    LastError;    // Indices



	S75 = (CNovraDevice *)S75Unit;

	// Retrieve the PAT from the unit

	if ( (LastError = S75->GetPAT((BYTE *)PAT)) == ERR_SUCCESS ) {

		return S75_OK;

	} else {

		return LastError;

	}//if

} //getPAT

//----------------------------------------------------------------------------------

// setPAT

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall setPAT(	S75_Handle S75Unit,	///< The S75_Handle of the device whose PAT is being configured
						BYTE *PAT      )	///< A pointer to a structure to receive the PAT (one of S75PlusPro_PAT, S75CAPro_PAT or S75FKPro_PAT)
#else
S75_DLL_API int setPAT(	S75_Handle S75Unit,
						BYTE *PAT     )
#endif
#else
int setPAT(	S75_Handle S75Unit,
			BYTE *PAT      )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
* S75PlusPro_PAT, S75CAPro_PAT and S75FKPro_PAT are equivalent stucture types
*
*/
{

	// This function sets the PAT on an S75CA


    // LOCAL VARIABLES

	CNovraDevice *S75;                        // Device handle as a CS75CA object

    int   i, j;                        // Indices

	bool  duplicates_exist = false;

	/* bool  program_out_of_range = false;*/

	int   result = S75_FAILED;



	S75 = (CNovraDevice *)S75Unit;

	S75PlusPro_PAT *PPAT = NULL; 
	/* S75PlusPro_PAT *pPAT = NULL; */

	if( ( S75->GetDeviceType() == S75PLUSPRO ) ||
		( S75->GetDeviceType() == S75CAPRO )   ||
		( S75->GetDeviceType() == S75FKPRO )      ){

		PPAT = (S75PlusPro_PAT * ) PAT;
		for ( i = 0; ( i < 16 ) && ( !duplicates_exist ); i++ ) {
			
			// Check for duplicate Program

			if ( PPAT->ProgramNumber[i] != 0 ) {
				
				for ( j = i + 1; ( j < 16 ) && ( !duplicates_exist ); j++ ) {
					
					if ( i != j ) {
						
						if ( PPAT->ProgramNumber[i] == PPAT->ProgramNumber[j] ) {
							
							duplicates_exist = true;
							
						} // if
						
					} // if
					
				} // for

			}
			
		} // for
		
		
		// If the list is OK then proceed
		
		if ( !duplicates_exist )
			result = S75->SetPAT((BYTE *)PAT);
		else
			result = ERR_PAT_NOTAPPLIED;

	}

	return  result;

} //setPAT

//----------------------------------------------------------------------------------

// getFixedKeys

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall getFixedKeys(	S75_Handle S75Unit,		///< The S75_Handle of the device whose fixed keys are being obtained
						BYTE *KeyTable          )	///< A pointer to a structure to receive the fixed keys (should be of type S75FK_KeyList or S75FKPro_KeyList)
#else
S75_DLL_API int getFixedKeys(	S75_Handle S75Unit,
						BYTE *KeyTable          )
#endif
#else
int  getFixedKeys(	S75_Handle S75Unit,
				BYTE *KeyTable          )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
* S75FK_KeyList and S75FKPro_KeyList are equivalent stucture types
*
* Fixed keys receivers do not send keys back to the host application.  The stucture contents will tell you which PIDs have keys assigned and whether those keys are odd or even.  These entries are to be used as placeholders for these keys when sending them back to the receiver using setFixedKeys().  If the placeholders are removed, then the keys will be removed from the receiver.  If the placeholder key value is changed then the key on the receiver will be changed.  If the placeholders are left alone, then the key on the receiver will be left alone also.
*
*/
{

	// This function gets the PAT from a device


    // LOCAL VARIABLES

	CNovraDevice  *S75;    // Device handle as an CS75D object

	int    LastError;    // Indices



	S75 = (CNovraDevice *)S75Unit;

	// Retrieve the PAT from the unit

	if ( (LastError = S75->GetFixedKeys((BYTE *)KeyTable)) == ERR_SUCCESS ) {

		return S75_OK;

	} else {

		return LastError;

	}//if

} //getFixedKeys

//----------------------------------------------------------------------------------

// setFixedKeys

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall setFixedKeys(	S75_Handle S75Unit,	///< The S75_Handle of the device whose fixed keys are being configured
						BYTE *KeyTable      )	///< A pointer to a structure to receive the fixed keys (should be of type S75FK_KeyList or S75FKPro_KeyList)
#else
S75_DLL_API int setFixedKeys(	S75_Handle S75Unit,
						BYTE *KeyTable     )
#endif
#else
int setFixedKeys(	S75_Handle S75Unit,
			BYTE *KeyTable      )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
* S75FK_KeyList and S75FKPro_KeyList are equivalent stucture types
*
*/
{

	// This function sets the Fixed Keys on an S75FK


    // LOCAL VARIABLES

	CNovraDevice *S75;                        // Device handle as a CS75FK object

    int   i, j;                        // Indices

	bool  duplicates_exist = false;

	/* bool  pid_out_of_range = false; */

	int   result = S75_FAILED;



	S75 = (CNovraDevice *)S75Unit;

	S75FK_KeyList *pFK = NULL; 

	if ( ( S75->GetDeviceType() == S75FK ) || ( S75->GetDeviceType() == S75FKPRO ) ) {

		pFK = (S75FK_KeyList * ) KeyTable;
		for ( i = 0; ( i < NUMBER_OF_FIXED_KEYS ) && ( !duplicates_exist ); i++ ) {
			
			// Check for duplicates (same PID and odd/even flag)

			if ( pFK->PID[i] != 0x1fff ) {
				
				for ( j = i + 1; ( j < NUMBER_OF_FIXED_KEYS ) && ( !duplicates_exist ); j++ ) {
					
					if ( i != j ) {
						
						if ( ( pFK->PID[i] == pFK->PID[j] ) && ( pFK->odd[i] == pFK->odd[j] ) ) {
							
							duplicates_exist = true;
							
						} // if
						
					} // if
					
				} // for

			}
			
		} // for
		
		
		// If the list is OK then proceed
		
		if ( !duplicates_exist )
			result = S75->SetFixedKeys((BYTE *)KeyTable);
		else
			result = ERR_FK_NOTAPPLIED;

	}

	return  result;

} //setFixedKeys

//----------------------------------------------------------------------------------

// getTraps

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall getTraps(	S75_Handle S75Unit,		///< The S75_Handle of the device whose Traps are being obtained
						BYTE *Traps          )	///< A pointer to a structure to receive the Traps (one of S75_TrapSetting_JLEO, S75PlusPro_TrapSetting, S75CAPro_TrapSetting, S75FKPro_TrapSetting)

#else
S75_DLL_API int getTraps(	S75_Handle S75Unit,
							BYTE *Traps          )
#endif
#else
int  getTraps(	S75_Handle S75Unit,
				BYTE *Traps          )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
* S75PlusPro_TrapSetting, S75CAPro_TrapSetting and S75FKPro_TrapSetting are all equivalent structure types.  However, this functionality is not yet supported on the corresponding devices.
*
*/
{

	// This function gets the Traps from a device


    // LOCAL VARIABLES

	CNovraDevice  *S75;    // Device handle as an CS75D object

	int    LastError;    // Indices



	S75 = (CNovraDevice *)S75Unit;

	// Retrieve the Traps Settings from the unit

	if ( (LastError = S75->GetTraps((BYTE *)Traps)) == ERR_SUCCESS ) {

		return S75_OK;

	} else {

		return LastError;

	}//if

} //getTraps

//----------------------------------------------------------------------------------

// setTraps

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall setTraps(	S75_Handle S75Unit,	///< The S75_Handle of the device whose Traps are being configured
						BYTE *Traps      )	///< A pointer to a structure to receive the Traps (one of S75_TrapSetting_JLEO, S75PlusPro_TrapSetting, S75CAPro_TrapSetting, S75FKPro_TrapSetting)
#else
S75_DLL_API int setTraps(	S75_Handle S75Unit,
							BYTE *Traps     )
#endif
#else
int setTraps(	S75_Handle S75Unit,
				BYTE *Traps      )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
* S75PlusPro_TrapSetting, S75CAPro_TrapSetting and S75FKPro_TrapSetting are all equivalent structure types.  However, this functionality is not yet supported on the corresponding devices.
*
*/
{

	// This function sets the Traps on an S75CA


    // LOCAL VARIABLES

	CNovraDevice *S75;                        // Device handle as a CS75CA object

	int   result = S75_FAILED;


	S75 = (CNovraDevice *)S75Unit;

	if( ( S75->GetDeviceType() == S75PLUSPRO )  ||
		( S75->GetDeviceType() == S75CAPRO )    ||
		( S75->GetDeviceType() == S75FKPRO )        ) {

		result = S75->SetTraps((BYTE *)Traps);

	}

	return  result;

} //setTraps

//----------------------------------------------------------------------------------

// setPIDList

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall setPIDList(	S75_Handle S75Unit,	///< The S75_Handle of the device whose PIDs are being configured
							BYTE *PIDs          )	///< A pointer to a structure to receive the PIDs (one of S75D_PIDList, S75D_PIDList_V3, A75_PIDList, S75_PIDList_JLEO, PesMaker_PIDList, A75PesMaker_PIDList, S75Plus_PIDList, S75PlusPro_PIDList, S75CA_PIDList, S75CAPro_PIDList, S75FK_PIDList, S75FKPro_PIDList)
#else
S75_DLL_API int setPIDList(	S75_Handle S75Unit,
				BYTE *PIDs          )
#endif
#else
int setPIDList(	S75_Handle S75Unit,
		BYTE *PIDs          )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
* The following PIDs types are all equivalent to the S75D_PIDList structure
*
* - S75D_PIDList_V3
* - A75_PIDList
* - S75_PIDList_JLEO
*
* The following PIDs types are all equivalent to the PesMaker_PIDList structure
*
* - A75PesMaker_PIDList
*
* The following PIDs types are all equivalent to the S75Plus_PIDList structure
*
* - S75PlusPro_PIDList
*
* - S75CA_PIDList
*
* - S75CAPro_PIDList
*
* - S75FK_PIDList
*
* - S75FKPro_PIDList
*
*/
{


    // LOCAL VARIABLES

	CNovraDevice *S75;                        // Device handle as a CS75D object

    int   i, j;                        // Indices

	bool  duplicates_exist = false;

	bool  pid_out_of_range = false;

	int   result = S75_OK;

	WORD dev_type;



	S75 = (CNovraDevice *)S75Unit;

	PesMaker_PIDList  *PList = NULL; 
	S75D_PIDList	*pPIDList = NULL;
	S75Plus_PIDList	*pS75PlusPIDList = NULL;

	dev_type = S75->GetDeviceType();

	if ( ( dev_type == S75PLUS )    ||
		 ( dev_type == S75PLUSPRO ) ||
		 ( dev_type == S75CA )      ||
		 ( dev_type == S75CAPRO )   ||
		 ( dev_type == S75FK )      ||
		 ( dev_type == S75FKPRO )   ||
		 ( dev_type == S75IPREMAP )       ) {
		pS75PlusPIDList = (S75Plus_PIDList * ) PIDs ;

		if ( ( dev_type == S75PLUS ) || ( dev_type == S75FK ) || ( dev_type == S75CA ) || ( dev_type == S75IPREMAP ) ) {
			for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {
				pS75PlusPIDList->Control[i] = (pS75PlusPIDList->PID[i] != NULL_PID) ? 1 : 0 ;
				pS75PlusPIDList->DestinationMask[i] = 0;
			}
		}

		for ( i = 0; ( i < NUMBER_OF_PIDS ) && (!pid_out_of_range) && ( !duplicates_exist); i++ ) {
			if ( pS75PlusPIDList->PID[i] != NULL_PID ) {
				
				// Do range check on PID
				
				if ( ( pS75PlusPIDList->PID[i] > NULL_PID ) || ( pS75PlusPIDList->PID[i] < 0 ) ) {
					
					pid_out_of_range = true;
					
				} // if
				
				// Check for duplicate PID
				
				for ( j = 0; ( j < NUMBER_OF_PIDS ) && ( !duplicates_exist ); j++ ) {
					
					if ( i != j ) {
						
						if ( pS75PlusPIDList->PID[i] == pS75PlusPIDList->PID[j] ) {
							
							duplicates_exist = true;
							
						} // if
						
					} // if
					
				} // for
				
			} // if
			
		} // for

		if ( ( pS75PlusPIDList->Control[0] ==3 ) || ( pS75PlusPIDList->Control[0] == 4 ) ) {
			pS75PlusPIDList->DestinationMask[0] = 0x8000;
		}
		
		
		// If the list is OK then proceed

		if ( (!pid_out_of_range) && (!duplicates_exist) )
			result = S75->SetPIDTable((BYTE *)PIDs);
		else
			result = ERR_PIDSET_NOTAPPLIED;

	} else
	if( ( S75->GetDeviceType() == PesMaker ) ||
		( S75->GetDeviceType() == A75PesMaker ) ) {

		PList = (PesMaker_PIDList * ) PIDs ;
		for ( i = 0; ( i < PList->NumberofPIDs ) && (!pid_out_of_range) && ( !duplicates_exist ); i++ ) {
			
			// Don't check NULL_PID
			
			if ( PList->Entry[i] != NULL_PID ) {
				
				// Do range check on PID
				
				if ( ( PList->Entry[i] > NULL_PID ) || ( PList->Entry[i] < 0 ) ) {
					
					pid_out_of_range = true;
					
				} // if
				
				// Check for duplicate PID
				
				for ( j = 0; ( j < PList->NumberofPIDs ) && ( !duplicates_exist ); j++ ) {
					
					if ( i != j ) {
						
						if ( PList->Entry[i] == PList->Entry[j] ) {
							
							duplicates_exist = true;
							
						} // if
						
					} // if
					
				} // for
				
			} // if
			
		} // for
		
		
		// If the list is OK then proceed
		

		for ( i = PList->NumberofPIDs; i < SIZE_PID_LIST; i++ ) {
			PList->Entry[i] = 0x1fff;
		}

		if ( (!pid_out_of_range) && (!duplicates_exist) )
			result = S75->SetPIDTable((BYTE *)PIDs);
		else
			result = ERR_PIDSET_NOTAPPLIED;

	}else{

		pPIDList = (S75D_PIDList *) PIDs;
		for ( i = 0; ( i < pPIDList->NumberofPIDs ) && (!pid_out_of_range) && ( !duplicates_exist ); i++ ) {
			
			// Don't check NULL_PID
			
			if ( pPIDList->Entry[i] != NULL_PID ) {
				
				// Do range check on PID
				
				if ( ( pPIDList->Entry[i] > NULL_PID ) || ( pPIDList->Entry[i] < 0 ) ) {
					
					pid_out_of_range = true;
					
				} // if
				
				// Check for duplicate PID
				
				for ( j = 0; ( j < pPIDList->NumberofPIDs ) && ( !duplicates_exist ); j++ ) {
					
					if ( i != j ) {
						
						if ( pPIDList->Entry[i] == pPIDList->Entry[j] ) {
							
							duplicates_exist = true;
							
						} // if
						
					} // if
					
				} // for
				
			} // if
			
		} // for
		
		
		// If the list is OK then proceed
		
		for ( i = pPIDList->NumberofPIDs; i < SIZE_PID_LIST; i++ ) {
			pPIDList->Entry[i] = 0x1fff;
		}

		if ( (!pid_out_of_range) && (!duplicates_exist) ) {
			result = S75->SetPIDTable((BYTE *)PIDs);
		} else
			result = ERR_PIDSET_NOTAPPLIED;
	}
	
	
    

	return  result;

} //setPIDList

//----------------------------------------------------------------------------------

// getPIDList

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall getPIDList(	S75_Handle S75Unit,	///< The S75_Handle of the device whose PIDs are being obtained
				BYTE *PIDs          )	///< A pointer to a structure to receive the PIDs (one of S75D_PIDList, S75D_PIDList_V3, A75_PIDList, S75_PIDList_JLEO, PesMaker_PIDList, A75PesMaker_PIDList, S75Plus_PIDList, S75PlusPro_PIDList, S75CA_PIDList, S75CAPro_PIDList, S75FK_PIDList, S75FKPro_PIDList)
#else
S75_DLL_API int getPIDList(	S75_Handle S75Unit,
				BYTE *PIDs          )
#endif
#else
int  getPIDList(	S75_Handle S75Unit,
			BYTE *PIDs          )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
* The following PIDs types are all equivalent to the S75D_PIDList structure
*
* - S75D_PIDList_V3
* - A75_PIDList
* - S75_PIDList_JLEO
*
* The following PIDs types are all equivalent to the PesMaker_PIDList structure
*
* - A75PesMaker_PIDList
*
* The following PIDs types are all equivalent to the S75Plus_PIDList structure
*
* - S75PlusPro_PIDList
*
* - S75CA_PIDList
*
* - S75CAPro_PIDList
*
* - S75FK_PIDList
*
* - S75FKPro_PIDList
*
*/
{

	// This function gets the list of PIDs in the PID filter of a device


    // LOCAL VARIABLES

	CNovraDevice  *S75;    // Device handle as an CS75D object

	int    LastError;    // Indices



	S75 = (CNovraDevice *)S75Unit;

	// Retrieve the PID list from the unit

	if ( (LastError = S75->GetPIDTable((BYTE *)PIDs)) == ERR_SUCCESS ) {

		return S75_OK;

	} else {

		return LastError;

	}//if

} //getPIDList

//----------------------------------------------------------------------------------

// setIPRemapTable

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall setIPRemapTable(	S75_Handle S75Unit,	///< The S75_Handle of the device whose IP Remap Table is being configured
								BYTE *IPRemapTable  )	///< A pointer to a structure to receive the IP Remap Table
#else
S75_DLL_API int setIPRemapTable(	S75_Handle S75Unit,
									BYTE *IPRemapTable   )
#endif
#else
int setIPRemapTable(	S75_Handle S75Unit,
						BYTE *IPRemapTable   )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
*/
{


    // LOCAL VARIABLES

	CNovraDevice *S75;                        // Device handle as a CS75D object

    int   i, j;                        // Indices
	S75IPRemap_RemapTable *remap_table;

	int   result = S75_OK;

	  /* WORD dev_type; */

	unsigned long mask_check;
	bool check_set = false;
	bool mask_ok = true;



	S75 = (CNovraDevice *)S75Unit;
	remap_table = (S75IPRemap_RemapTable *)IPRemapTable;


	try {

		for ( i = 0; i < NUM_REMAPPING_RULES-1; i++ ) {
			for ( j = i + 1; j < NUM_REMAPPING_RULES; j++ ) {
				if ( remap_table->remapping_rules[i].originalIP[0] != 0 ) {
					if ( ( memcmp( (void *)(remap_table->remapping_rules[i].originalIP),
						           (void *)(remap_table->remapping_rules[j].originalIP), 4 ) == 0 ) &&
 					     ( memcmp( (void *)(remap_table->remapping_rules[i].newIP),
						           (void *)(remap_table->remapping_rules[j].newIP), 4 ) == 0 ) &&
 					     ( memcmp( (void *)(remap_table->remapping_rules[i].mask),
 						           (void *)(remap_table->remapping_rules[j].mask), 4 ) == 0 ) ) {
						throw 1;;
					}
				}
			}
		}


		for ( i = 0; i < NUM_REMAPPING_RULES; i++ ) {
			if ( remap_table->remapping_rules[i].originalIP[0] == 0 ) {
				if ( !( ( remap_table->remapping_rules[i].originalIP[1] == 0 ) &&
					    ( remap_table->remapping_rules[i].originalIP[2] == 0 ) &&
						( remap_table->remapping_rules[i].originalIP[3] == 0 )    ) ) {
					throw 2;
				}
			}
			if ( remap_table->remapping_rules[i].newIP[0] == 0 ) {
				if ( !( ( remap_table->remapping_rules[i].newIP[1] == 0 ) &&
					    ( remap_table->remapping_rules[i].newIP[2] == 0 ) &&
						( remap_table->remapping_rules[i].newIP[3] == 0 )    ) ) {
					throw 3;
				}
			}

			mask_check = (int)256 * 256 *256 * remap_table->remapping_rules[i].mask[0] + 
				         (int)256 * 256 * remap_table->remapping_rules[i].mask[1] + 
						 (int)256 * remap_table->remapping_rules[i].mask[2] + 
						 remap_table->remapping_rules[i].mask[3];

			check_set = false;
			mask_ok = true;

			// Check mask from right to left making sure 0's and 1's are contiguous
			for ( j = 0; ( j < 32 ) && mask_ok; j++ ) {
				if ( !check_set ) {
					// Traversing 0's
					if ( mask_check & 0x1 ) {
						// Found a 1
						check_set = true;
					}
					mask_check = mask_check >> 1;
				} else {
					// Traversing 1's
					if ( !( mask_check & 0x1 ) ) {
						// 0 that should not be here
						mask_ok = false;
					}
					mask_check = mask_check >> 1;
				}
			}
			if ( !mask_ok ) {
				throw 4;
			}

			if ( ( remap_table->remapping_rules[i].ttl < 0 ) || ( remap_table->remapping_rules[i].ttl > 255 ) ) {
				throw 5;
			}

			if ( ( remap_table->remapping_rules[i].action < 0 ) || ( remap_table->remapping_rules[i].action > 3 ) ) {
				throw 6;
			}
		}

		if ( ( remap_table->enable < 0 ) || ( remap_table->enable > 1 ) ) {
			throw 7;
		}


		result = ((CS75IPRemap *)S75)->SetIPRemapTable((BYTE *)IPRemapTable);

	}
	
	catch ( int e ) {

		result = ERR_FAILED;

	}
	
    
	return  result;

} //setIPRemapTable

//----------------------------------------------------------------------------------

// getIPRemapTable

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall getIPRemapTable(	S75_Handle S75Unit,	///< The S75_Handle of the device whose IP Remap Table is being obtained
				BYTE *IPRemapTable          )	///< A pointer to a structure to receive the IP Remap Table (IPRemap Device Only)
#else
S75_DLL_API int getIPRemapTable(	S75_Handle S75Unit,
				BYTE *IPRemapTable          )
#endif
#else
int  getIPRemapTable(	S75_Handle S75Unit,
			BYTE *IPRemapTable          )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
*/
{

	// This function gets the IP Remap Table of a device


    // LOCAL VARIABLES

	CNovraDevice  *S75;    // Device handle as an CS75D object

	int    LastError;    // Indices



	S75 = (CNovraDevice *)S75Unit;

	// Retrieve the IP Remap Table from the unit

	if ( (LastError = ((CS75IPRemap *)S75)->GetIPRemapTable((BYTE *)IPRemapTable)) == ERR_SUCCESS ) {

		return S75_OK;

	} else {

		return LastError;

	}//if

} //getIPRemapTable

//----------------------------------------------------------------------------------

// getStatus

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall getStatus(	S75_Handle S75Unit,		///< The S75_Handle of the device whose status is being obtained
							BYTE *status        )	///< A pointer to a structure to receive the status (one of S75D_Status, S75D_Status_V3, A75_Status, A75PesMaker_Status, PesMaker_Status, S75_Status_JLEO, S75Plus_Status, S75PlusPro_Status, S75CA_Status, S75CAPro_Status, S75FK_Status, S75FKPro_Status)
#else
S75_DLL_API int getStatus(	S75_Handle S75Unit,
				BYTE *status        )
#endif
#else
int getStatus(	S75_Handle S75Unit,
		BYTE *status        )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
* A75_Status and A75PesMaker_Status are equivalent structure types
*
* The following structure types are all equiavlent to the S75Plus_Status structure
*
* - S75PlusPro_Status
*
* - S75CA_Status
*
* - S75CAPro_Status
*
* - S75FK_Status
*
* - S75FKPro_Status
*
*/
{

    // This function acquires a status packet from a device


    // LOCAL VARIABLES

	CNovraDevice   *S75;                       // Device handle as an CS75D object

// @@@DBW DEBUG	int     timeout = 2000;
	int     timeout = 2000;  // @@@DBW Debug

	S75 = (CNovraDevice *)S75Unit;

//#ifdef JSAT
#ifdef BROADCAST_POLL_FOR_STATUS
	if ( S75->GetDeviceType() == S75_JLEO ) {

		if ( S75->GetStatus((void*)status) == ERR_SUCCESS )
			return S75_OK;

	} else {
#endif
//#endif

	if(S75->FlushStatusBuffer(timeout)==ERR_SUCCESS){ // The Default behaviour of a Novra device is Flush the buffer and receive one packet.
		// Copy last status packet into our structure
		if (S75->GetStatus((void*)status )== ERR_SUCCESS) {
			return S75_OK;
                }
	}

//#ifdef JSAT
#ifdef BROADCAST_POLL_FOR_STATUS
	}
#endif
//#endif

	return S75_FAILED;

} //getStatus

//----------------------------------------------------------------------------------

// resetS75

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall resetS75( S75_Handle  S75Unit )	///< The S75_Handle of the device being reset
#else
S75_DLL_API int resetS75( S75_Handle  S75Unit )
#endif
#else
int  resetS75( S75_Handle  S75Unit )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
*/
{

    // This function resets the S75


	return (int)(((CNovraDevice *)S75Unit)->ResetDevice());

}  // resetS75

//----------------------------------------------------------------------------------

// resetCAM

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall resetCAM( S75_Handle  S75Unit )	///< The S75_Handle of the device being reset
#else
S75_DLL_API int resetCAM( S75_Handle  S75Unit )
#endif
#else
int  resetCAM( S75_Handle  S75Unit )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
*/
{

    // This function resets the S75


	return (int)(((CNovraDevice *)S75Unit)->ResetCAM());

}  // resetCAM

//----------------------------------------------------------------------------------

// setCAMWatchdogTimeout

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall setCAMWatchdogTimeout(	S75_Handle  S75Unit,	///< The S75_Handle of the device whose CAM watchdog timeout is being set
										WORD timeout )			///< The timeout value in seconds
#else
S75_DLL_API int setCAMWatchdogTimeout( S75_Handle  S75Unit, WORD timeout )
#endif
#else
int  setCAMWatchdogTimeout( S75_Handle  S75Unit, WORD timeout )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
*/
{

	return (int)(((CNovraDevice *)S75Unit)->setCAMWatchdogTimeout(timeout));

}  // setCAMWatchdogTimeout

//----------------------------------------------------------------------------------

// getCAMWatchdogTimeout

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall getCAMWatchdogTimeout(	S75_Handle  S75Unit,	///< The S75_Handle of the device whose CAM watchdog timeout is being read
										WORD *timeout )			///< A pointer to variable where the timeout value (in seconds) will be stored
#else
S75_DLL_API int getCAMWatchdogTimeout( S75_Handle  S75Unit, WORD *timeout )
#endif
#else
int  getCAMWatchdogTimeout( S75_Handle  S75Unit, WORD *timeout )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
*/
{

	return (int)(((CNovraDevice *)S75Unit)->getCAMWatchdogTimeout(timeout));

}  // getCAMWatchdogTimeout

//----------------------------------------------------------------------------------

// GetAvailablePort

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall GetAvailablePort( WORD &Port)	///< Pointer to a WORD that will receive a port number
#else
S75_DLL_API int GetAvailablePort( WORD &Port)
#endif
#else
int GetAvailablePort( WORD &Port)
#endif
/**
*
* It tries to find one of the 10 auto detect status ports that can be
* successfully used on the host machine.  It does this by creating
* sockets experimentally until one works.  Once found, it closes the
* experimental socket and returns the port number to the caller via the
* Port parameter.
*
* Returns S75_OK or an Error code (see file s75.h)
*
*/
{
#ifndef LINUX
	const int winsock_version=2;  // Request Winsock 2.0
	WSADATA	  wsa_data;           // Results of Winsock Startup Request
#endif
	SOCKET	s;
	int i;
	struct sockaddr_in address;
#ifndef LINUX
	bool use_again=true;
	WSAStartup( winsock_version, &wsa_data );
#endif
	/* int use_again; */

#ifndef LINUX
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) {
#else	
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) {
#endif
		s=0;
        	return ERR_SS_SOCKET_CREATE;
	}
	//setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (const char *)(&use_again), sizeof( use_again ) );
	for(i = 0; i<num_auto_detect_ports; i++){
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons( (unsigned short)auto_detect_status_ports[i]);
		if ( bind ( s, (struct sockaddr *)(&address), sizeof( struct sockaddr ) ) == 0 ) {
			Port = auto_detect_status_ports[i];
#ifndef LINUX
			closesocket( s );
			WSACleanup();
#else
			close( s );
#endif
			return S75_OK;
		}
	}
#ifndef LINUX
	closesocket( s );
	WSACleanup();
#else
	close( s );
#endif


	return S75_FAILED;
}

//----------------------------------------------------------------------------------

// SetDVBMACAddress

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall SetDVBMACAddress(	S75_Handle S75Unit,	///< The S75_Handle of the device whose DVB MAC address is being set
				BYTE* DVBMAC       )	///< A pointer to a 6 byte array containing the DVB MAC
#else
S75_DLL_API int SetDVBMACAddress(	S75_Handle S75Unit,
					BYTE* DVBMAC       )
#endif
#else
int SetDVBMACAddress(	S75_Handle S75Unit,
			BYTE* DVBMAC       )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
*/
{
	CNovraDevice * S75;
	S75=(CNovraDevice *)S75Unit ;
	if(S75 == NULL) return S75_FAILED ;

	return S75->ConfigureDVBMAC(DVBMAC);
}

//----------------------------------------------------------------------------------

// GetDeviceType

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall GetDeviceType(	S75_Handle S75Unit,	///< The S75_Handle of the device whose type is being retrieved
				WORD &DevType       )	///< A pointer to a WORD that will receive the device type
#else
S75_DLL_API int GetDeviceType(	S75_Handle S75Unit,
				WORD &DevType       )
#endif
#else
int  GetDeviceType(	S75_Handle S75Unit,
			WORD &DevType       )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
* The device type can be one of the members of the enumeration NOVRA_DEVICES ...
*
* - S75_ANY (used internally during auto discovery)
*
* - S75_UNKNOWN (a device with firmware that is not recognized or sending bad status packets)
*
* - S75_V_1 (S75 Version 1 device, not supported by this library)
*
* - S75D_V21 (S75 Version 2.x device)
*
* - S75D_V3 (S75 Version 3 device)
*
* - A75 (ATSC Device)
*
* - PesMaker (S75 Version 5 PesMaker)
*
* - A75PesMaker (ATSC PesMaker)
*
* - S75_JLEO (S75 JSAT Leo-Palace device)
*
* - S75PLUS (S75 Plus device)
*
* - S75PLUSPRO (S75 Plus device with support for the PRO feature set)
*
* - S75CA (S75 Conditional Access device)
*
* - S75CAPRO (S75 Conditional Access device with support for the PRO feature set)
*
* - S75FK (S75 Fixed Key device)
*
* - S75FKPRO (S75 Fixed Key device with support for the PRO feature set)
*
*/
{
	CNovraDevice * S75;
	S75 = (CNovraDevice *) S75Unit;
	if((DevType = S75->GetDeviceType()) == S75_UNKNOWN)
		return S75_FAILED;

	return S75_OK;


}

//----------------------------------------------------------------------------------

// calcSignalStrengthPercentage

//@@@NTL moved following function into library 20050906
#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall calcSignalStrengthPercentage(	S75_Handle S75Unit,
						BYTE AutomaticGainControl,		///< AGC value from device status
						BYTE DemodulatorGain,			///< Demodulator gain from device status
						BYTE SignalLock,				///< Signal lock flag from device statuis
						BYTE DataLock              )	///< Data lock flag from device status
#else
S75_DLL_API int calcSignalStrengthPercentage(	S75_Handle S75Unit,
												BYTE AutomaticGainControl,
												BYTE DemodulatorGain,
												BYTE SignalLock,
												BYTE DataLock             )
#endif
#else
int calcSignalStrengthPercentage(	S75_Handle S75Unit,
									BYTE AutomaticGainControl,
									BYTE DemodulatorGain,
									BYTE SignalLock,
									BYTE DataLock              )
#endif
/**
*
* This functions should only be used for satellite receivers
*
* Returns a number between 0 and 100.
*
*/

{
	WORD device_type;
	double ss = 0.0;                // Values for calculating signal strength

	device_type = ((CNovraDevice *)S75Unit)->GetDeviceType();

	if ( ( device_type == S75PLUS )    ||
		 ( device_type == S75PLUSPRO ) ||
		 ( device_type == S75CA )      ||
		 ( device_type == S75CAPRO )   ||
		 ( device_type == S75FK )      ||
		 ( device_type == S75FKPRO )   ||
		 ( device_type == S75IPREMAP ) ) {

		if ( ( AutomaticGainControl == 255 ) && ( SignalLock ) ) {
			ss = 0.0;
		} else {
			ss = 140.0 - ( AutomaticGainControl / 2.0 );
		}

	} else {


		const double AGC[] = { 250, 220, 140,120, 105,80, 65 }; // Automatic Gain Control
		const double PERCENTAGE[] = { 4, 35,40,60,75, 95,100 };
		const int SIZE_ARRAY = 7;
		const int OffsetByGain[] = {0, 8, 16 , 25};

		double slope = 0.0;             // Values for calculating signal strength
		double offset = 0.0;            // Values for calculating signal strength

		// Signal Strength
		if (AutomaticGainControl >= 250) {
			ss=3;
			if(! DataLock) ss= 0;

			switch( DemodulatorGain) {
				case 1:
					ss += 2.2 * OffsetByGain[3];
					break;
				case 2:
					ss += 2 *OffsetByGain[2];
					break ;
				case 4:
					ss += 10+2* OffsetByGain[1];
					break ;
				case 8:
					ss += OffsetByGain[0];
					break;
			}
		} else if (AutomaticGainControl < AGC[SIZE_ARRAY - 1]) {
			ss = 100;

		} else {
			for (int i = 0; i < SIZE_ARRAY - 1; i++ ) {
				if( ( AGC[i] >= AutomaticGainControl) && 
// @@@DBW            ( AGC[i + 1] < AutomaticGainControl) ) {
					( AGC[i + 1] <= AutomaticGainControl) ) {	// @@@DBW
					slope = ( PERCENTAGE[i + 1] - PERCENTAGE[i] ) / ( AGC[i + 1] - AGC[i] );
					offset = PERCENTAGE[i] - slope * AGC[i];
					ss = slope * (double)AutomaticGainControl + offset;
					switch( DemodulatorGain){
						case 1:
								{
								ss=1.2 *ss;
								ss += OffsetByGain[3];
								if (ss < (3 + 2.2 * OffsetByGain[3]) )
									ss=(3 + 2.2 * OffsetByGain[3]) ;
								break;
								}
						case 2:
								{
								ss =1.1 *ss;
								ss += OffsetByGain[2];
								if (ss < (3+2 *OffsetByGain[2]) )
									ss = (3+2 *OffsetByGain[2]) ;
								break ;
								}
						case 4:
								{
								ss = 0.8 *ss ;
								ss += OffsetByGain[1];
								if(ss<(13+2* OffsetByGain[1]))
									ss = 13+2* OffsetByGain[1] ;
								break ;
								}
						case 8:
								{
								ss = 0.7*ss;
								break;
								}
					}

					break;
				}
			}
		}
	}
	return ((int)ss > 100) ? 100 : (int)ss ;
}

//----------------------------------------------------------------------------------

//#ifdef JSAT

extern "C"
#ifndef LINUX
#ifndef S75_DLL_EXPORTS
S75_Handle __stdcall discoverRemoteDevice(	BYTE* ip,		///< IP address of receiver
											int timeout )	///< A timeout value in seconds
#else
S75_DLL_API S75_Handle discoverRemoteDevice( BYTE* ip, int timeout )
#endif
#else
S75_Handle discoverRemoteDevice( BYTE* ip, int timeout )  //@@@NTL added timeout
#endif
/**
*
* Returns a valid S75_Handle or a NULL pointer
*
*/
{
	S75JLEO_One_Time_Status_Request_MSG message;
	TCHAR temp[16];
	SOCKET s;
	int use_again;
	struct sockaddr_in device_address;
	int i;
	short STATUS_POLL_PORT = 0x2011;
	struct timeval timeout_struct;
	fd_set readfs;
	int result;
	int temp1 = 1000;               // Temporary storage variable
	BYTE Buffer[sizeof(S75JLEO_Status_MSG)];
	Novra_DeviceEntry device_info;
	S75JLEO_Status_MSG *statusJLEO = NULL;
	S75CAStatus_MSG *statusPlus = NULL;
	int Errcode;
	S75_Handle	S75=NULL;	// @@@DBW

#ifndef LINUX

	WSADATA	  wsa_data;           // Results of Winsock Startup Request

	const int winsock_version=2;  // Request Winsock 2.0
#endif


#ifndef LINUX
	WSAStartup( winsock_version, &wsa_data );
#endif


	message.Message[0] = 0x47;
	message.Message[1] = 0x47;
	message.Message[2] = 0x48;
	message.Message[3] = 0x48;
	message.Message[4] = 0x49;
	message.Message[5] = 0x49;

	// Create a socket to communicate with the S75

	sprintf( temp, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

#ifndef LINUX
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) != INVALID_SOCKET ) {
#else
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) != -1 ) {
#endif

		setsockopt( s, SOL_SOCKET, SO_REUSEADDR, (const char *)(&use_again), sizeof( use_again ) );
        	// Connect the newly created socket to the S75.

    		device_address.sin_family = AF_INET;

    		device_address.sin_port = htons( STATUS_POLL_PORT );

    		device_address.sin_addr.s_addr = inet_addr( temp);

    		if ( connect( s, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr ) ) == 0){


            		for (i = 0; (S75==NULL) && ( i < NUM_RETRIES ); i++) {

                		if ( send( s, (const char *)(&message), sizeof( S75JLEO_One_Time_Status_Request_MSG), 0 ) == sizeof( S75JLEO_One_Time_Status_Request_MSG ) ) {

#ifndef LINUX
		                	Sleep(30);  //@@@NTL should this be here?
#endif

		                	timeout_struct.tv_sec = timeout;
	
		                	timeout_struct.tv_usec = 0;

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
    		    	        			if( (temp1 = recv( s, (char *)Buffer, sizeof(S75JLEO_Status_MSG), 0 ) ) != SOCKET_ERROR ) {


#else
    		    	        			if( (temp1 = recv( s, (char *)Buffer, sizeof(S75JLEO_Status_MSG), 0 ) ) != -1 ) {
#endif
		    	    	        			// Make sure we have received the right number of bytes back

	    		    	        			if ( temp1 == sizeof(S75JLEO_Status_MSG) ||
								     temp1 == sizeof(S75PlusStatus_MSG) ) {

									// try JLEO v1 type first, then JLEO v2 (based on S75+)

									statusJLEO = (S75JLEO_Status_MSG *)Buffer;
									memcpy( device_info.DeviceIP, ip, 4 );
									memcpy( device_info.DeviceMAC, statusJLEO->ReceiverMAC, 6 );
                   							device_info.StatusPort = ntohs( statusJLEO->UniCastStatusDestUDP );
                   							device_info.DeviceType = S75_JLEO;
									
									S75 = openRemoteDevice(&device_info,1000, Errcode );

									if ( S75 == NULL ) { // try JLEO v2 (S75+ based JLEO)

										statusPlus = (S75PlusStatus_MSG *)Buffer;
										memcpy( device_info.DeviceMAC, statusPlus->ReceiverMAC, 6 );
                   								device_info.StatusPort = ntohs( statusPlus->UniCastStatusDestUDP );
                   								device_info.DeviceType = S75PLUS;
										S75 = openRemoteDevice(&device_info,1000, Errcode );
									}

								} // if temp1 == sizeof...

							} // if recv

						} // if select result 0

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

	return S75;

}

//#endif

//----------------------------------------------------------------------------------

#ifdef JSAT

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
S75_Handle __stdcall openRemoteDevice(	Novra_DeviceEntry * pDev,
										const int  timeout,
										int &ErrorCode)
#else
S75_DLL_API S75_Handle openRemoteDevice( Novra_DeviceEntry * pDev, const int  timeout, int & ErrorCode)
#endif
#else
S75_Handle openRemoteDevice( Novra_DeviceEntry * pDev, const int  timeout, int & ErrorCode)
#endif

// input   : Device Entry as returned by the Discover Devices, timeout (in ms)
// output  : none
// returns : valid handle or NULL pointer

{

	BYTE status[sizeof( S75CA_Status )]; 

	// This function creates a handle for communicating with an S75 receiver


    // LOCAL VARIABLES

// @@@DBW	void	*S75=NULL;
	CNovraDevice	*S75=NULL;	// @@@DBW

#ifndef LINUX
	WSADATA	  wsa_data;               // Results of Winsock Startup Request

	const int winsock_version=2;      // Request Winsock 2.0

	WSAStartup( winsock_version, &wsa_data );
#endif

	switch (pDev->DeviceType)
	{
		case S75_JLEO:
			S75 = new CS75_JLEO(true);
			break;
		case S75PLUS:
			S75 = new CS75Plus(true);
			break;
		case S75PLUSPRO:
			S75 = new CS75PlusPro(true);
			break;
		case S75CA:
			S75 = new CS75CA(true);
			break;
		case S75CAPRO:
			S75 = new CS75CAPro(true);
			break;
		case S75FK:
			S75 = new CS75FK(true);
			break;
		case S75FKPRO:
			S75 = new CS75FKPro(true);
			break;
		case S75IPREMAP:
			S75 = new CS75IPRemap(true);
			break;
		default:
			{
				S75 = NULL;
				ErrorCode = ERR_DEVICE_TYPE_UNKNOWN;
				break;
			}
	}
	if ( S75 != NULL ) {
	    // Set information needed to communicate with the unit

		if( (ErrorCode= ( (CNovraDevice *) S75)->Attach_Device(pDev) ) != Attach_Success){
			delete S75;
			return NULL;
		}

		if ( S75->PollStatus( status ) != ERR_SUCCESS ) {


         		delete S75;

			S75 = NULL;

		} //if

	} // if

	return S75;
}

#endif

//----------------------------------------------------------------------------------

#ifdef JSAT

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall configureTraps( 	S75_Handle S75Unit,
			  	BYTE*  TrapSettings )
#else
S75_DLL_API int configureTraps( 	S75_Handle S75Unit,
			  	BYTE*  TrapSettings )
#endif
#else
int configureTraps( S75_Handle S75Unit,
		  BYTE*  TrapSettings )
#endif

// input   : S75Unit, TrapSettings
// output  : none
// returns : S75_OK or ERROR (see S75.h for error definitions)

{ 

	// This function configures the Trap parameters belonging to an S75


    // LOCAL VARIABLES

	CNovraDevice *S75;    // Device handle as an CS75D object
	int LastError;



	S75 = (CNovraDevice *)S75Unit;

	// Send the Trap settings to the unit, if the function is successful, return OK

	if ( (LastError = S75->SetTraps( TrapSettings)) == ERR_SUCCESS) {
		return S75_OK;
	} 
	else{
		return LastError;
	}//if

} //configureTraps

#endif

//----------------------------------------------------------------------------------

//#ifdef JSAT

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall pollStatus(	S75_Handle S75Unit,	///< The S75_Handle of the device whose status is being sought
							BYTE *status,		///< A pointer to a buffer to receive the status (one of S75_Status_JLEO, S75Plus_Status, S75PlusPro_Status, S75CA_Status, S75CAPro_Status, S75FK_Status, S75FKPro_Status)
							int timeout )		///< A timeout value (in milliseconds)
#else
S75_DLL_API int pollStatus( S75_Handle S75Unit, BYTE *status, int timeout )
#endif
#else
int pollStatus( S75_Handle S75Unit, BYTE *status, int timeout )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
* Unlike the getStatus function, which relies on the passive delivery of status packets, this function will send a message to an S75 asking it to send status back.  This enables status to be acquired from a device that may not be on the same subnet as the host application.
*
* Only the following devices support this functionality ...
*
* - S75 JLeo
*
* - S75+
*
* - S75+ Pro
*
* - S75CA
*
* - S75CA Pro
*
* - S75FK
*
* - S75FK Pro
*
* The following structure types are all equiavlent to the S75Plus_Status structure
*
* - S75PlusPro_Status
*
* - S75CA_Status
*
* - S75CAPro_Status
*
* - S75FK_Status
*
* - S75FKPro_Status
*
*/

// input   : S75Unit, timeout
// output  : status
// returns : S75_OK or ERROR (see S75.h for error definitions)

{

    // This function acquires a status packet from a device


    // LOCAL VARIABLES

	CNovraDevice  *S75;    // Device handle as an CS75D object

	int    LastError;    // Indices



	S75 = (CNovraDevice *)S75Unit;


	if ( (LastError = S75->PollStatus((void *)status,timeout)) == ERR_SUCCESS ) {

		return S75_OK;

	} else {

		return LastError;

	}//if

} //pollStatus

//#endif

//----------------------------------------------------------------------------------

//#ifdef JSAT

#ifdef BROADCAST_POLL_FOR_STATUS
#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall broadcastPollStatus( S75_Handle S75Unit, BYTE *status )
#else
S75_DLL_API int broadcastPollStatus( S75_Handle S75Unit, BYTE *status )
#endif
#else
int broadcastPollStatus( S75_Handle S75Unit, BYTE *status )
#endif

// input   : S75Unit
// output  : status
// returns : S75_OK or ERROR (see S75.h for error definitions)

{

    // This function acquires a status packet from a device by MAC when IP is not known


    // LOCAL VARIABLES

	CNovraDevice  *S75;    // Device handle as an CS75D object

	int    LastError;    // Indices



	S75 = (CNovraDevice *)S75Unit;


	if ( (LastError = S75->BroadcastPollStatus((void *)status)) == ERR_SUCCESS ) {

		return S75_OK;

	} else {

		return LastError;

	}//if

} //broadcastPollStatus
#endif

//#endif

//----------------------------------------------------------------------------------

#ifdef JSAT

extern "C"
#ifndef LINUX
#ifndef S75_DLL_EXPORTS
int __stdcall changePassword(	S75_Handle S75Unit,
								char *password,
								unsigned char key[16] )
#else
S75_DLL_API int changePassword(S75_Handle S75Unit, char *password, unsigned char key[16] )
#endif
#else
int changePassword(S75_Handle S75Unit, char *password, unsigned char key[16] )
#endif
{
	char p[8];
	CNovraDevice * S75;
	S75=(CNovraDevice *)S75Unit ;
	if(S75 == NULL) return S75_FAILED ;

	memset( p, 0, 8 );
	for ( int i = 0; ( i < 8 ) && ( i < (int)strlen( password ) ); i++ ) {
		p[i] = password[i];
	}
	return S75->ChangePassword(p, key);
}

#endif

//----------------------------------------------------------------------------------

//#ifdef JSAT

extern "C"
#ifndef LINUX
#ifndef S75_DLL_EXPORTS
int __stdcall specifyPassword(	S75_Handle S75Unit,	///< The S75_Handle of the device
								char *password )	///< The password (up to 8 characters)
#else
S75_DLL_API int specifyPassword(S75_Handle S75Unit, char *password )
#endif
#else
int specifyPassword(S75_Handle S75Unit, char *password )
#endif
/**
*
* Returns S75_OK or an Error code (see file s75.h)
*
*/
{
	char p[8];
	CNovraDevice * S75;
	S75=(CNovraDevice *)S75Unit ;
	if(S75 == NULL) return S75_FAILED ;

	memset( p, 0, 8 );
	for ( int i = 0; ( i < 8 ) && ( i < (int)strlen( password ) ); i++ ) {
		p[i] = password[i];
	}
	S75->SpecifyPassword(p);

	return ERR_SUCCESS;
}

//#endif

//----------------------------------------------------------------------------------

#ifdef JSAT

extern "C"
#ifndef LINUX
#ifndef S75_DLL_EXPORTS
S75_Handle __stdcall forceRemoteDevice( const Novra_DeviceEntry *pDev,WORD	Port,const int timeout, int &ErrorCode )
#else
S75_DLL_API S75_Handle forceRemoteDevice( const Novra_DeviceEntry *pDev,WORD	Port,const int timeout, int &ErrorCode )
#endif
#else
S75_Handle forceRemoteDevice( const Novra_DeviceEntry *pDev,WORD	Port,const int timeout, int &ErrorCode )
#endif
// input   : S75IPAddress, S75MACAddress, statusPort, timeout (in ms)
// output  : none
// returns : valid handle or NULL pointer

{
	BYTE status[sizeof( S75_Status_JLEO )]; 


	bool StatusPortValid ;
	int i;
	CNovraDevice *S75;
	BYTE ConfigBuffer[1500] ; // generic buffer to hold any configuration structure
	StatusPortValid = false;
	// We need to make sure that the port will support auto detection.
	// The user should use get available port
	for(i=0; i< num_auto_detect_ports; i++){
		if(Port == auto_detect_status_ports[i]){
			StatusPortValid = true;
			break;
		}
	}
	if(!StatusPortValid){
		ErrorCode = ERR_BAD_STATUS_PORT;
		return NULL;
	}
	switch (pDev->DeviceType)
	{
		case S75_JLEO:
			S75 = (CNovraDevice *) new CS75_JLEO(true);
			break;

		default:
			{
				S75 = NULL;
				ErrorCode = ERR_DEVICE_TYPE_UNKNOWN;
				break;
			}
	}
	if(S75 == NULL) return S75;

	S75->Attach_Device((void *)pDev); // The value in the status port doesn't matter
	S75->Set_Status_Port( Port);	 // cause here we are changing it
	if(!	S75->GetDefaultConfigStruct(ConfigBuffer)){ // here we get a default configuration for the device
		ErrorCode = ERR_NO_DefaultConfig;
		delete S75;
		return NULL;
	}

	ErrorCode = configureNetwork(S75, (BYTE*)ConfigBuffer); // and we configure the device

	if ( S75->PollStatus( status ) != ERR_SUCCESS ) {


       		delete S75;

		S75 = NULL;

	} //if


	return S75;

}

#endif

//----------------------------------------------------------------------------------


//**********************************************************************************

#ifdef _NOVRA_INTERNAL_BUILD

//----------------------------------------------------------------------------------

// ProgramMACAddress

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall ProgramMACAddress(	S75_Handle S75Unit,	///< The S75_Handle of the device whose MAC is being programmed
					BYTE* MAC          )	///< A pointer to a 6 byte array containing the MAC being programmed
#else
S75_DLL_API int ProgramMACAddress(	S75_Handle S75Unit,
					BYTE* MAC          )
#endif
#else
int ProgramMACAddress(	S75_Handle S75Unit,
			BYTE* MAC          )
#endif
// This function is for Novra use only.
{
	CNovraDevice * S75;
	S75=(CNovraDevice *)S75Unit ;
	if(S75 == NULL) return S75_FAILED ;

	return S75->ProgramDeviceMACAddress(MAC);
}

//----------------------------------------------------------------------------------

// ProgramUnitIdentifier

#ifndef LINUX
#ifndef S75_DLL_EXPORTS
extern "C"
int __stdcall ProgramUnitIdentifier(	S75_Handle S75Unit,	///< The S75_Handle of the device whose UID is being programmed
					BYTE* UID          )	///< A pointer to a 32 byte array containing the UID
#else
S75_DLL_API int ProgramUnitIdentifier(	S75_Handle S75Unit,
					BYTE* UID	    )
#endif
#else
int  ProgramUnitIdentifier(	S75_Handle S75Unit,
				BYTE* UID	    )
// This function is for Novra use only.
#endif
{
	CNovraDevice * S75;
	S75=(CNovraDevice *)S75Unit ;
	if(S75 == NULL) return S75_FAILED ;

	return S75->ProgramDeviceUID(UID);
}

#endif
