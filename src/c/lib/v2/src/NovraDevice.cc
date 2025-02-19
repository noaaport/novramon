// NovraDevice.cpp: implementation of the CNovraDevice class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "NovraDevice.h"
#include "Status.h"
#include "PIDList.h"

#include <memory.h>
#ifndef LINUX
#include <iphlpapi.h>
#endif

#include <stdio.h>
#include <math.h>
#include  "s75.h"
#ifdef LINUX
#include <time.h>
#define TRUE  true
#define FALSE false
#define BOOL bool
#endif


const int MAC_ADDRESS_CONFIG_PORT = 0xDCAB;
const int UID_CONFIG_PORT = 0x1984;
const int RESET_PORT = 0x9999;
const int NETWORK_CONFIG_PORT =0x1975;
const int PID_CONFIG_PORT = 0x1977;
const int DVB_MAC_CONFIG = 0x1979;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/****************************************************************************************
**  Constructor Initialize class variables.
*/

CNovraDevice::CNovraDevice(LPBYTE pStatusBuff)
{
	m_IPAddressValid = false;
	m_Device_attached = false;
	m_StatusTimeout = STATUS_PACKET_TIMEOUT;    // (default value)
	m_FilterOnIP = false ;
	m_Device_Status_Port = 0x1974;             // (default value)
	m_NetPathSet = false ;
	m_pStatusBuffer = pStatusBuff;		// If No Value is passed this parameter is NULL. to prevent Using
										// a buffer that is not initialized.
	//printf ("\n Parent Constructor \n");
	m_Status_Listen_Start_Time = 0;
	m_usingStatusListener = true;
	m_remote = false;
}

CNovraDevice::~CNovraDevice()
{

}
//-------------------------------------------------------------------------
// Get_Device_MAC_Address(BYTE * MAC)
//
// Purpose:     This function is used to return the S75D MAC address.
//
// Parameters:  MAC   [OUT]   The MAC address, assigned to the class.
//
// Returns:     True if the value returned is the valid MAC address.  False
//				If the Class was not attached to aphysical device prior to the call
//
//-------------------------------------------------------------------------

bool CNovraDevice::Get_Device_MAC_Address(BYTE * MAC)
{
	if(m_Device_attached)							// If the class was attached to a physical Unit by the attach routine
		memcpy(MAC, m_Device_MAC_Address, 6);		// then the MAC address Here is Valid

	return m_Device_attached;						// Return value of TRUE means the device is attached and the MAC is valid
												// False means the MAC address is not Set, And the class is not attached.
}

bool CNovraDevice::Set_Device_MAC_Address(BYTE * MAC)
{
	if((MAC[0] == 0x00) && (MAC[1] == 0x06) && (MAC[2] == 0x76)){	// Are we trying to Set it to a valid MAC Address
		memcpy(m_Device_MAC_Address, MAC, 6);							// Yes, Set the MAC address
		m_MACAddressValid = true;
		return TRUE;												// Return True, ALL OK
	}
	m_MACAddressValid  =false;
	return FALSE;												// The MAc address doesn't match Novra's devices..
}


bool CNovraDevice::Get_Device_IP_Address(BYTE *IP)
{
	if(m_Device_attached)							// If the class was attached to a physical Unit by the attach routine
		memcpy(IP, m_Device_IP_Address, 4);		// then the IP address Here is Valid

	return m_Device_attached;						// Return value of TRUE means the device is attached and the IP is valid
												// False means the IP address is not Set, And the class is not attached
}

bool CNovraDevice::Set_Device_IP_Address(BYTE *IP)
{
		memcpy(m_Device_IP_Address, IP, 4);			// Copy the data over to the IP address...
		m_IPAddressValid = true;
		return	TRUE;								// For now, All the calls here return true..
}

int CNovraDevice::Attach_Device(void * pDev)
{
	Novra_DeviceEntry * Dev;
	Dev = (Novra_DeviceEntry *)pDev;
	if(m_Device_Type != S75_ANY)		// Unless it is an all purpose discovery class... device types should match
		if(Dev->DeviceType != m_Device_Type) 
			return Err_Attach_Device_TypeMismatch ;

	if(m_Device_attached) 
		return Error_Already_Attached;			// Check if the class already attached to a device
	if(!Set_Device_MAC_Address(Dev->DeviceMAC)) 
		return Error_Attach_MAC;				//	Set MAC Address failed
	if(!Set_Device_IP_Address(Dev->DeviceIP)) 
		return Error_Attach_IP;					// Set IP Failed
	if(!Set_Status_Port(Dev->StatusPort))	
		return Error_Attach_Port;				// Set Port failed
	m_Device_attached = true ;
	return Attach_Success ;				// Now the class has all the device Info...

}

bool CNovraDevice::Set_Status_Port(WORD port)
{
	m_Device_Status_Port = port ;		// Copy the value to the member variable
	return TRUE	;					// So far anything goes.. always return true
}

bool CNovraDevice::Get_Status_Port(WORD * port)
{
	if(m_Device_attached)					// If the device has been attached 
		*(port) = m_Device_Status_Port ;	// we copy the port from the member variable. we used pointer instead of reference for consistency
	return m_Device_attached;				// return the attach status of the device.
}


int CNovraDevice::SendWaitForConfirmation(TCHAR device_ip[], UINT port, LPBYTE pData, int length)
{
	SOCKET s;
	struct sockaddr_in device_address;
	fd_set readfs;
	struct timeval timeout;
	int result;

    // Create a socket to communicate with the S75.
#ifndef LINUX
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) {
#else
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) {
#endif
        return ERR_SSC_SOCKET_CREATE;
    	
	}

	device_address.sin_family = AF_INET;
	device_address.sin_port = htons( port );
	device_address.sin_addr.s_addr = inet_addr( device_ip );
	if ( connect( s, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr_in ) ) != 0){
         return ERR_SSC_SOCKET_CONNECT;
		
	}

	LPBYTE Buffer2 = new BYTE[length];
	int temp1 = 1000;
	/* int count = 0 ; */
	/* DWORD xx = 1; */
	
    if ( send( s, (const char *)pData, length, 0 ) != length ) {
		delete [] Buffer2 ;
#ifndef LINUX
		closesocket( s );
#else
		close( s );
#endif
		return ERR_SSC_SEND;
	}
	
#ifndef LINUX
	Sleep(30);
#endif
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	FD_ZERO( &readfs );
	FD_SET( s, &readfs );
#ifndef LINUX
    	if ( ( result = select( 1, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#else
    	if ( ( result = select( s+1, &readfs, NULL, NULL, &timeout ) ) != -1 ) {
#endif
		if ( result != 0 ) {

#ifndef LINUX
			if( (temp1 = recv( s, (char *)Buffer2, length, 0 ) ) == SOCKET_ERROR ) {
				temp1 = GetLastError();
				if(temp1 != WSAETIMEDOUT) {
					delete [] Buffer2;
					return ERR_SSC_RECEIVE;
				}

#else
			if( (temp1 = recv( s, (char *)Buffer2, length, 0 ) ) == -1 ) {
				delete [] Buffer2;
				return ERR_SSC_RECEIVE;
#endif


			}
			if(temp1 ==  length){
				for(int i =0 ; i < length; i++ ){
					if(pData[i] != Buffer2[i]) {
						delete [] Buffer2 ;
#ifndef LINUX
						closesocket( s );
#else
						close( s );
#endif
						return  ERR_SSC_ACK_BAD ;
					}
				}

#ifndef LINUX
				closesocket( s );
#else
				close( s );
#endif
				delete [] Buffer2 ;
				return  ERR_SUCCESS;

			}

		}
	}

	delete [] Buffer2 ;
#ifndef LINUX
	closesocket( s );
#else
	close( s );
#endif

	return ERR_SSC_ACK_NONE;

}

void CNovraDevice::SetStatusTimeout(UINT Timeout)
{
	m_StatusTimeout = Timeout;
}

void CNovraDevice::FilterStatus(bool filter)
{
	m_FilterOnIP = filter && m_IPAddressValid ;
}

int CNovraDevice::SetNetworkPath(BYTE MyIPAddress[], bool SetARPTable, bool SetRoutingTable, BYTE ifname[]  )
{
	TCHAR S75DIP[16];                     // S75D IP address in string form
	TCHAR MASK[16];
	TCHAR S75DMAC[18];                    // S75D MAC address in string form
	char network_interface_ip[16];                       // My IP address in string form
	unsigned char *buffer;
	ULONG buflen = 10000;
#ifndef LINUX
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADDR_STRING pIPAddress;
	MIB_IPFORWARDROW Route;
	MIB_IPNETROW ArpEntry;
#else
	/* char *pAdapterInfo; */
	/* char *pIPAddress; */
	char system_command[100];
#endif
//	int i;
	/* ULONG result;*/
	/* BOOL adapter_found=false; */

#ifdef JSAT
	if ( !m_remote ) {
#endif

	if(m_NetPathSet) return ERR_NETPATH_ALREADYSET ;

	if(MyIPAddress == NULL) return ERR_CHECK;

	m_NetPathSet = false;

    // Check to see if the MAC address and the IP address has already been
    // set.  If not, return an error.
    if ((!m_IPAddressValid) && (!m_MACAddressValid)) {
        // Can't set network path -- no MAC or IP addresses
        return ERR_NETPATH_NODESTINFO;
    }

	buffer = new unsigned char[buflen];
#ifndef LINUX
    while ( ( result = GetAdaptersInfo((PIP_ADAPTER_INFO)buffer,&buflen) ) == ERROR_BUFFER_OVERFLOW ) {
		delete [] buffer;
		buflen += 10000;
    	buffer = new unsigned char[buflen];
	}

	if ( result == ERROR_SUCCESS ) {
#else
	if(1){		// @@NS to maintain most of the code intact a dummy IF is added ..
#endif

		sprintf( S75DIP, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);
		sprintf( MASK, "255.255.255.255");
#ifndef LINUX
		sprintf( S75DMAC, "%02X-%02X-%02X-%02X-%02X-%02X", m_Device_MAC_Address[0], m_Device_MAC_Address[1], m_Device_MAC_Address[2], m_Device_MAC_Address[3], m_Device_MAC_Address[4], m_Device_MAC_Address[5]);
#else
		sprintf( S75DMAC, "%02X:%02X:%02X:%02X:%02X:%02X", m_Device_MAC_Address[0], m_Device_MAC_Address[1], m_Device_MAC_Address[2], m_Device_MAC_Address[3], m_Device_MAC_Address[4], m_Device_MAC_Address[5]);
#endif
		sprintf( network_interface_ip, "%d.%d.%d.%d", MyIPAddress[0], MyIPAddress[1], MyIPAddress[2], MyIPAddress[3]);
		//@@ns These values are updated here if The configuration is changed these values are preserved for removal
		memcpy(m_LastNetPath.MAC, m_Device_MAC_Address,6);
		memcpy(m_LastNetPath.IP, m_Device_IP_Address,4);
		memcpy(m_LastNetPath.IF_IP, MyIPAddress,4);
		
#ifdef LINUX
		strcpy((char*)m_LastNetPath.IF_NAME, (const char*)ifname);		// Copies the interface Name for Linux Purposes
//@@@SP Delete any existing route that is the same as the route we are about to add. This is necessary to ensure that
//@@@SP the route we are about to add is not preceeded by another route to the same IP, that we may have added previously.
//@@@SP We're not going to get into deleting any route to that IP to avoid messing around with the customer's route table,
//@@@SP so it will be their responsibility to have a "clean" route table for the receiver IP they have selected.

	//@@NS route should be deleted by the remove network path
//		memset( system_command, 0, 100 );
//		sprintf( system_command, "route del -host %s gw 0.0.0.0 dev %s metric 3", S75DIP, ifname );
//		printf("\n %s \n", system_command);
//		system( system_command );
//		// ---Add Route
		memset( system_command, 0, 100 );
		sprintf( system_command, "route add -host %s dev %s metric 3", S75DIP, ifname );
	//	printf("\n %s \n", system_command);
		if ( system( system_command ) != 0 ) {	//@@@SP wrapped system() call in if block
			delete [] buffer;
			return NetPath_Failed;
		}
		m_LastNetPath.RouteSet = true;

		sleep(2); //@@@SP - Add a small delay to ensure the route is entered into the kernel's route table before the call returns.
		// --- Add ARP
		memset( system_command, 0, 100 );
		sprintf( system_command, "arp -i %s -s %s %s temp", ifname, S75DIP, S75DMAC );
		//printf("\n %s \n", system_command);
		if ( system( system_command ) != 0 ) {
			delete [] buffer;
			return NetPath_Failed;
		}
		m_LastNetPath.ARPSet = true ;
#else	// Windows Calls for adding routes and ARP
		pAdapterInfo = (PIP_ADAPTER_INFO)buffer;
		while ( ( pAdapterInfo != NULL ) && (!adapter_found) ){
			pIPAddress = &(pAdapterInfo->IpAddressList);
			while ( ( pIPAddress != NULL ) && (!adapter_found ) ) {
				if ( strcmp( pIPAddress->IpAddress.String, network_interface_ip) == 0 ) {
				adapter_found = true;
				} else {
				pIPAddress = pIPAddress->Next;
				}
			}
			if ( !adapter_found ) {
				pAdapterInfo = pAdapterInfo->Next;
			}
		}

		if ( adapter_found ) {
			RemoveNetworkPath();
		// Put the IP and MAC addresses into strings
			Route.dwForwardDest = inet_addr((LPCTSTR)S75DIP);
			Route.dwForwardMask = inet_addr((LPCTSTR)MASK);
			Route.dwForwardPolicy = 0;
			Route.dwForwardNextHop = 0;
			Route.dwForwardNextHop = inet_addr( network_interface_ip );
			Route.dwForwardIfIndex = pAdapterInfo->Index;
			Route.dwForwardType = 3;
			Route.dwForwardProto = 3;
			Route.dwForwardAge = 0;
			Route.dwForwardNextHopAS = 0;
			Route.dwForwardMetric1 = 3;
			Route.dwForwardMetric2 = -1;
			Route.dwForwardMetric3 = -1;
			Route.dwForwardMetric4 = -1;
			Route.dwForwardMetric5 = -1;
			CreateIpForwardEntry(&Route);
			ArpEntry.dwIndex = pAdapterInfo->Index;
			ArpEntry.dwPhysAddrLen = 6;
			memcpy( ArpEntry.bPhysAddr, m_Device_MAC_Address, 6 );
			ArpEntry.dwAddr = inet_addr((LPCTSTR)S75DIP);
			ArpEntry.dwType = 4;
			CreateIpNetEntry(&ArpEntry);
			m_LastNetPath.ARPSet = true;
			m_LastNetPath.RouteSet = true;

		} else {
			delete [] buffer;
			return NetPath_Failed;
		}
#endif
	} else {
		delete [] buffer;
		return NetPath_Failed;
	}

	delete [] buffer;
#ifdef JSAT
	}
#endif

    return NetPath_Success;
}

void CNovraDevice::RemoveNetworkPath()
{

  // TCHAR temp[16];               Temporary storage variable.
  /* unsigned char *buffer; */
  /* ULONG buflen = 10000; */
  /* ULONG result; */
  /* unsigned int i; */
#ifdef LINUX
	char command[120];
#endif
#ifndef LINUX
	MIB_IPNETROW arpInfo;       // Structure for sending information to create ARP entry
	IPAddr thisDevice;          // IP address of the device we wish to add an entry for.
	PIP_ADAPTER_INFO pAdapterInfo;
	buffer = new unsigned char[buflen];
    while ( ( result = GetAdaptersInfo((PIP_ADAPTER_INFO)buffer,&buflen) ) == ERROR_BUFFER_OVERFLOW ) {
		delete [] buffer;
		buflen += 10000;
    	buffer = new unsigned char[buflen];
	}

    if ( result == ERROR_SUCCESS ) {
		if(m_LastNetPath.ARPSet){

			pAdapterInfo = (PIP_ADAPTER_INFO)buffer;

        // Convert the device IP address to the right format
        
			sprintf( temp, "%d.%d.%d.%d", m_LastNetPath.IP[0], m_LastNetPath.IP[1], m_LastNetPath.IP[2],m_LastNetPath.IP[3]);
        
			thisDevice = inet_addr((LPCTSTR)temp);

        
			arpInfo.dwAddr = thisDevice;

		
			while ( pAdapterInfo != NULL ) {
				arpInfo.dwIndex = pAdapterInfo->Index;
				DeleteIpNetEntry(&arpInfo);
				pAdapterInfo = pAdapterInfo->Next;
			}
	
		}
	}

	while ( ( result = GetIpForwardTable( (PMIB_IPFORWARDTABLE)buffer, &buflen, TRUE) ) == ERROR_INSUFFICIENT_BUFFER ) {
		delete [] buffer;
		buflen += 10000;
    	buffer = new unsigned char[buflen];
	}

	if ( result == NO_ERROR ) {
		if(m_LastNetPath.RouteSet){
			
			for ( i = 0; i < ((PMIB_IPFORWARDTABLE)buffer)->dwNumEntries; i++ ) {
				
				if ( ((PMIB_IPFORWARDTABLE)buffer)->table[i].dwForwardDest == thisDevice ) {
					
					DeleteIpForwardEntry(&(((PMIB_IPFORWARDTABLE)buffer)->table[i]));
					
				}
				
			}
		}
	}
	m_LastNetPath.ARPSet = false;
	m_LastNetPath.RouteSet = false;
	delete [] buffer;
#else
	if(m_LastNetPath.RouteSet){
		sprintf(command,"route del -host %d.%d.%d.%d ",m_LastNetPath.IP[0], m_LastNetPath.IP[1], m_LastNetPath.IP[2], m_LastNetPath.IP[3]);
		//printf("\n %s \n", command);
		system(command);
	}
	sleep(2);
	if(m_LastNetPath.ARPSet){
		sprintf(command,"arp  -i %s -d  %d.%d.%d.%d ",m_LastNetPath.IF_NAME,m_LastNetPath.IP[0], m_LastNetPath.IP[1], m_LastNetPath.IP[2], m_LastNetPath.IP[3]);
		//printf("\n %s \n", command);
		system(command);
	}
	m_LastNetPath.ARPSet = false;
	m_LastNetPath.RouteSet = false;
#endif
}

int CNovraDevice::StartStatusListener() {
    // Check to see if a status listener is already up.  If so, close it
	int LastError ;
    if (m_StatusListener.IsListening()) {
        StopStatusListener();
    }
	//printf("Starting status listner .. ");
    // Check to make sure the given status port is in the right range
    /* JFN - comparison is always false due to limited range of data type
     *
    if ((m_Device_Status_Port <= 0) || (m_Device_Status_Port >= 65536)) {
        return ERR_STATUS_INVALID_PORT;
    }
    */
	if(m_pStatusBuffer == NULL){
		//printf(" No Buffer ");
		return ERR_STATUS_BufferNotInitialized ;
		}
	// Initialize the status listener
	m_StatusListener.Initialize(&m_FilterOnIP,m_Device_IP_Address, m_pStatusBuffer, &m_LastStatus, m_StatusPacketSize);
	    // Start the status listener
    if ((LastError = m_StatusListener.StartListening(m_Device_Status_Port))!= SL_SUCCESS)         
        return LastError;
    
	m_Status_Listen_Start_Time = time(NULL);

    // Success
    
    return ERR_SUCCESS;
}

//-------------------------------------------------------------------------
// StopStatusListener()
//
// Purpose:     This function is used to stop the status listener and close
//              the S75D status socket.
//
// Parameters:  None
//
// Returns:     None
//
// Notes:       None
//-------------------------------------------------------------------------
void CNovraDevice::StopStatusListener() {
    // Make sure a status listener is up before trying to close it

    if (!m_StatusListener.IsListening()) return;

    // Close the status listener
    m_StatusListener.EndListening();


} // CNovraDevice::StopStatusListener()

//-------------------------------------------------------------------------
// FlushStatusBuffer()
//
// Purpose:     This function is used to flush any incoming status packets
//              from the packet buffer.  This function will then wait up
//              to STATUS_PACKET_TIMEOUT milliseconds for a new status packet
//              arrive.
//
// Parameters:  timeout     [IN]    Maximum amount of time to wait for a new
//                                  status packet to arrive.
//
// Returns:     True if the buffer was flushed successfully and a new status
//              packets was received.  False otherwise.
//  
// Notes:       None
//-------------------------------------------------------------------------
int CNovraDevice::FlushStatusBuffer(int timeout) {
	int LastError;
    // Make sure the status listener is up before attempting flush
    if (!m_StatusListener.IsListening()) 
        return  ERR_STATUS_NOT_UP ;
    // Flush the buffer
    if (!m_StatusListener.FlushBuffer())
        return ERR_FAILED;

    // Wait for the next status packet to arrive so that we can
	// update the status structure.  If we timeout waiting for a packet,
	// return a value of false.
    if ((LastError = WaitForStatusPacket(timeout))!= ERR_SUCCESS) {
        // Don't need to define error code -- done in WaitForStatusPacket()
		return ERR_FAILED;			// Time out or error
    }

    // Success
    
	return ERR_SUCCESS;

} // CNovraDevice::FlushStatusBuffer()


bool CNovraDevice::IsStatusValid()
{
	if ((time(NULL) - m_LastStatus) < (DWORD)m_StatusTimeout/1000) {
        // Valid status
        return true;
    } else {
        // Invalid status
        return false;
    }

}


bool CNovraDevice::ParseStatusPacket()
{

//	*(m_StatusWrapper->DSPFirmWarePointer())		= ntohs(*(m_StatusWrapper->DSPFirmWarePointer()));
//	*(m_StatusWrapper->StatusDestUDPPointer())		= ntohs(*(m_StatusWrapper->StatusDestUDPPointer()));
//	*(m_StatusWrapper->EthernetDroppedPointer())	= ntohs(*(m_StatusWrapper->EthernetDroppedPointer()));
//	*(m_StatusWrapper->EthernetRXPointer())			= ntohs(*(m_StatusWrapper->EthernetRXPointer()));
//	*(m_StatusWrapper->EthernetRXErrPointer())		= ntohs(*(m_StatusWrapper->EthernetRXErrPointer()));
//	*(m_StatusWrapper->EthernetTXPointer())			= ntohs(*(m_StatusWrapper->EthernetTXPointer()));
//	*(m_StatusWrapper->EthernetTXErrPointer())		= ntohs(*(m_StatusWrapper->EthernetTXErrPointer()));
//	*(m_StatusWrapper->FEL_LossPointer())			= ntohs(*(m_StatusWrapper->FEL_LossPointer()));
//	*(m_StatusWrapper->MicroFirmWarePointer())		= ntohs(*(m_StatusWrapper->MicroFirmWarePointer()));
//	*(m_StatusWrapper->Sync_LossPointer())			= ntohs(*(m_StatusWrapper->Sync_LossPointer()));
//	*(m_StatusWrapper->TotalDVBAcceptedPointer())	= ntohs(*(m_StatusWrapper->TotalDVBAcceptedPointer()));
//	*(m_StatusWrapper->RFStatusValidPointer())		= ntohs(*(m_StatusWrapper->RFStatusValidPointer()));

	if ( m_MACAddressValid ) {

		if ( memcmp( m_StatusWrapper->ReceiverMACPointer(), m_Device_MAC_Address, 6 ) != 0 )

			return false;

	}

	return true;

}

int CNovraDevice::WaitForStatusPacket(int timeout)
{
	int LastError ;

	if ( m_usingStatusListener ) {

		// Make sure the status listener is up before waiting for a status packet
		if (!m_StatusListener.IsListening())

			if((LastError=StartStatusListener())!= ERR_SUCCESS)
				return LastError;
	
		// Wait for a status packet
				
		if ((LastError = m_StatusListener.ReceiveSinglePacket(timeout))!= ERR_SUCCESS)
			return LastError;
				
		if (!ParseStatusPacket())
			return ERR_FAILED;		// This will ensure the Packet is Parsed and all the variables were updated..
				
		// Update the last time stamp
		m_LastStatus = time(NULL) ;

		return ERR_SUCCESS;

	} else {

		return ERR_FAILED;

	}

}



int CNovraDevice::GetStatus( Status *pStatus, bool forward_raw, int max_pids )
{

	memcpy( pStatus->UIDPointer(), m_StatusWrapper->ProductIDPointer(), 32 );


	*(pStatus->FirmWareVerPointer()) = ((*(m_StatusWrapper->DSPFirmWarePointer())) & 0x00f0) >> 4; 
	*(pStatus->FirmWareRevPointer()) =	(*(m_StatusWrapper->DSPFirmWarePointer())) & 0xf ;
	*(pStatus->RF_FIRMWAREPointer()) =	(*(m_StatusWrapper->MicroFirmWarePointer())) ;


	// Copy the Network settings.

	memcpy( pStatus->DefaultGatewayIPPointer(), m_StatusWrapper->DefaultGatewayIPPointer(),4 );
	memcpy( pStatus->DestinationIPPointer(), m_StatusWrapper->DestIPPointer(),4 );
	memcpy( pStatus->SubnetMaskPointer(), m_StatusWrapper->SubnetMaskPointer(), 4 );
	memcpy( pStatus->ReceiverMACPointer(), m_StatusWrapper->ReceiverMACPointer(), 6 );
	memcpy( pStatus->ReceiverIPPointer(), m_Device_IP_Address,4 );
	*(pStatus->DestinationUDPPointer()) = (*(m_StatusWrapper->StatusDestUDPPointer()));
	
		
	// Copy the Ethernet Statistics...
	*(pStatus->EthernetPacketDroppedPointer())	= (*(m_StatusWrapper->EthernetDroppedPointer()));
	*(pStatus->EthernetReceivePointer())		= (*(m_StatusWrapper->EthernetRXPointer()));
	*(pStatus->EthernetTransmitPointer())		= (*(m_StatusWrapper->EthernetTXPointer()));
//	*(pStatus->EthernetTransmitErrorPointer())	= (*(m_StatusWrapper->EthernetTXErrPointer()));

	(pStatus->WRESERVEDPointer())[0]			= (*(m_StatusWrapper->EthernetRXErrPointer()));
//	(pStatus->WRESERVEDPointer())[1]			= (*(m_StatusWrapper->FEL_LossPointer()));
	(pStatus->WRESERVEDPointer())[2]			= (*(m_StatusWrapper->Sync_LossPointer()));
	(pStatus->WRESERVEDPointer())[3]			= (*(m_StatusWrapper->TotalDVBAcceptedPointer()));

	return ERR_SUCCESS ;

}


int CNovraDevice::GetStatus( void *Container )
{
	return ERR_METHODNOTSUPPORTED;
}


int CNovraDevice::ConfigureNetwork(void * pNetworkSetting)
{
	return ERR_METHODNOTSUPPORTED;
}



int CNovraDevice::TuneDevice(void *TuningParameters)
{
	return ERR_METHODNOTSUPPORTED;
}


int CNovraDevice::ResetDevice()
{
	BYTE dataBuffer[4] = {0x19, 0x75, 0x14, 0x03};  // Data buffer (data to be sent to S75)
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


    // Send the reset command to the S75D.  If communication fails, retry up to four times.
    for (int i = 0; i < NUM_RETRIES; i++) {
        if ((LastError = SendWaitForConfirmation( temp, RESET_PORT, dataBuffer, 4 ))== ERR_SUCCESS) break;

        if (i == 3) {
            // Failed to reset S75D.
            // Don't need to define error code -- done by SendWaitForConfirmation()
            return LastError;
        }        
    }

    // S75D Reset successfully
    return LastError;

}


int CNovraDevice::ResetCAM()
{
	return ERR_METHODNOTSUPPORTED;
}


int CNovraDevice::setCAMWatchdogTimeout( WORD timeout )
{
	return ERR_METHODNOTSUPPORTED;
}


int CNovraDevice::getCAMWatchdogTimeout( WORD *timeout )
{
	return ERR_METHODNOTSUPPORTED;
}


int CNovraDevice::ProgramDeviceMACAddress(LPBYTE MACAddress)
{
	bool configurationSent;             // New configuration sent to S75D?
    bool configurationApplied;          // New configuration applied to S75D?
    BYTE Buffer[10];                    // Buffer to hold data to send to S75D
    TCHAR temp[16];                       // Temporary storage variables.
	int LastError;

    // Check the MAC address provided to confirm that it is valid
    // (that is, make sure the first 3 bytes are 00-06-76)
    if ((MACAddress[0] != 0x00) || (MACAddress[1] != 0x06) || (MACAddress[2] != 0x76)) {
        // Invalid MAC address
		return	ERR_MACPGM_INVALIDMAC;
    }

    // Check to see if the device has a MAC address already.  Do this
    // by looking at the receiver MAC addres in the status.  If we are not
    // receiving status, return false.
    if ((m_StatusListener.IsListening()) && (IsStatusValid())) {
        // Receiving status, check MAC address.
        // If we don't have a MAC address, the last three bytes will be zero
        if (((m_StatusWrapper->ReceiverMACPointer()[3]) != 0x00) || ((m_StatusWrapper->ReceiverMACPointer())[4] != 0x00) || ((m_StatusWrapper->ReceiverMACPointer())[5] != 0x00)) {
            // This unit already has a MAC address
            return ERR_MACPGM_HASMAC;
		}
	} else {
        // Not receiving status
        return ERR_MACPGM_NOSTATUS;
    }

    // There are two possible methods of communicating -- by IP address or
    // by MAC address.  For the former, make sure the IP address is set.
    // For the later, make sure the network path is set
	if ((!m_Device_attached)&&(!m_NetPathSet)) {
        // Trying to communicate by IP with no IP address -- error
         return ERR_MACPGM_NONETPATH;
	}

    // Destination information properly set

    // Copy the data to sent into a buffer
	memcpy(Buffer, MACAddress, 6);

    // Convert the IP address to a string
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);


    // Send the configuration to the S75D.  If communication fails, retry up to four times.
    for (int i = 0; i < NUM_RETRIES; i++) {
        configurationSent =((LastError = SendWaitForConfirmation( temp, MAC_ADDRESS_CONFIG_PORT, Buffer, 6 ))== ERR_SUCCESS);
        if (configurationSent) break;
    }

    // Check if we were able to send the configuration successfully
    if (configurationSent) {
        // Configuration sent to S75D

        // Update the MAC address member variable
        memcpy(&m_Device_MAC_Address, &MACAddress, 6);

//		if ( m_usingStatusListener ) {
		if ( m_usingStatusListener && ( GetDeviceType() != S75CA ) ) {

	        // Flush the status packet buffer
		    if ((LastError = FlushStatusBuffer(10000))!= ERR_SUCCESS) {
			    // Error flushing buffer
				// Don't need to define error code -- done by FlushStatusBuffer()
				return LastError;
			}

			// Wait for the next status packet
			if ((LastError = WaitForStatusPacket(STATUS_PACKET_TIMEOUT))!=ERR_SUCCESS) {
				// Error when waiting for status packet
				// Don't need to define error code -- done by WaitForStatusPacket()
				return LastError;
			}

			// Check to see if the values sent to the S75D are the same as those being
			// reported in the status
			configurationApplied = true;

			if (memcmp(MACAddress, m_StatusWrapper->ReceiverMACPointer(), 6) != 0) configurationApplied = false;

#ifdef JSAT

		} else { // Need to poll for status

			if ( PollStatus( m_StatusWrapper ) == ERR_SUCCESS ) {
        	
				// Check to see if the values sent to the S75D are the same as those being
        		// reported in the status

        		configurationApplied = true;

        		if (memcmp(MACAddress, m_StatusWrapper->ReceiverMACPointer(), 6) != 0) configurationApplied = false;
			
			}
#endif
		}

        if (!configurationApplied) 
            // Configuration not successfully applied
			return ERR_MACPGM_NOTAPPLIED;
		else 
            // Configuration successfully applied
            return ERR_SUCCESS;
	} else {
        // Configuration not sent to S75D
        return LastError;
    }



}



int CNovraDevice::ProgramDeviceUID(LPBYTE UID)
{
	bool configurationSent;             // New configuration sent to S75D?
    bool configurationApplied;          // New configuration applied to S75D?
    BYTE Buffer[40];                    // Buffer to hold data to send to S75D
    TCHAR temp[16];                       // Temporary storage variables.
	int LastError;

  
    // There are two possible methods of communicating -- by IP address or
    // by MAC address.  For the former, make sure the IP address is set.
    // For the later, make sure the network path is set
	if ((!m_Device_attached)&&(!m_NetPathSet)) {
        // Trying to communicate by IP with no IP address -- error
         return ERR_UIDPGM_NONETPATH;
	}

    // Destination information properly set

    // Copy the data to sent into a buffer
	memcpy(Buffer, UID, 32);

    // Convert the IP address to a string
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);


    // Send the configuration to the S75D.  If communication fails, retry up to four times.
    for (int i = 0; i < NUM_RETRIES; i++) {
        configurationSent =((LastError = SendWaitForConfirmation( temp, UID_CONFIG_PORT, Buffer, 32 ))== ERR_SUCCESS);
        if (configurationSent) break;
    }

    // Check if we were able to send the configuration successfully
    if (configurationSent) {
        // Configuration sent to S75D

//		if ( m_usingStatusListener ) {
		if ( m_usingStatusListener && ( GetDeviceType() != S75CA ) ) {
        
	        // Flush the status packet buffer
		    if ((LastError = FlushStatusBuffer(10000))!= ERR_SUCCESS) {
			    // Error flushing buffer
				// Don't need to define error code -- done by FlushStatusBuffer()
				return LastError;
			}

	        // Wait for the next status packet
		    if ((LastError = WaitForStatusPacket(STATUS_PACKET_TIMEOUT))!=ERR_SUCCESS) {
			    // Error when waiting for status packet
				// Don't need to define error code -- done by WaitForStatusPacket()
				return LastError;
			}

#ifdef JSAT
		} else { // Need to poll for status

			if ( ( LastError = PollStatus( m_StatusWrapper ) ) != ERR_SUCCESS ) {
				return LastError;
			}
#endif
		}

        // Check to see if the values sent to the S75D are the same as those being
        // reported in the status
        configurationApplied = true;

        if (memcmp(UID, m_StatusWrapper->ProductIDPointer(), 32) != 0) configurationApplied = false;

        if (!configurationApplied) 
            // Configuration not successfully applied
			return ERR_UIDPGM_NOTAPPLIED;
		else 
            // Configuration successfully applied
            return ERR_SUCCESS;
	} else {
        // Configuration not sent to S75D
        return LastError;
    }

}



int CNovraDevice::SetPIDTable(LPBYTE PIDTable)
{
		return ERR_METHODNOTSUPPORTED;

}

int CNovraDevice::GetPIDTable(LPBYTE PIDTable)
{
		return ERR_METHODNOTSUPPORTED;

}

int CNovraDevice::SetProgramTable(LPBYTE ProgramTable)
{
	return ERR_METHODNOTSUPPORTED;
}


int CNovraDevice::GetProgramTable(LPBYTE ProgramTable)
{
	return ERR_METHODNOTSUPPORTED;
}

int CNovraDevice::SetPAT(LPBYTE PAT)
{
	return ERR_METHODNOTSUPPORTED;
}


int CNovraDevice::GetPAT(LPBYTE PAT)
{
	return ERR_METHODNOTSUPPORTED;
}


int CNovraDevice::SetFixedKeys(LPBYTE KeyTable)
{
	return ERR_METHODNOTSUPPORTED;
}


int CNovraDevice::GetFixedKeys(LPBYTE KeyTable)
{
	return ERR_METHODNOTSUPPORTED;
}

int CNovraDevice::SetTraps(LPBYTE Traps)
{
	return ERR_METHODNOTSUPPORTED;
}


int CNovraDevice::GetTraps(LPBYTE Traps)
{
	return ERR_METHODNOTSUPPORTED;
}


bool CNovraDevice::DetachDevice()
{
	if(m_Device_attached) m_Device_attached = false;
	return !m_Device_attached;

}


bool CNovraDevice::TrimPIDList( LPBYTE pList, int Max)
{
	S75D_PIDList *pL;

	int j=0,k=0,l=0,m=0;

	pL = (S75D_PIDList*) pList;

	for(j=0; j<Max; j++){
		if(pL->Entry[j] != NULL_PID){ // if it is not a NULL PID
			// Store the entry
			l=pL->Entry[j] ;
			m=pL->Control[j];
			// delete it
			pL->Entry[j] = NULL_PID;
			pL->Control[j] = 0;
			// Write it at the top of the list.
			pL->Entry[k] = l;
			pL->Control[k] = m;
			// Increment the counter tht point to the top
			k++;
		}
	// other wise do nothing
	}

	// Now the non-NULL PID should be at the top..
	pL->NumberofPIDs = k;
	return true;

}


WORD CNovraDevice::GetDeviceType()
{
	if(m_Device_attached)
		return m_Device_Type ;
	else
		return S75_UNKNOWN;

}


bool CNovraDevice::CheckStatusPort( WORD port )
{

	bool StatusPortValid = false ;

	for( int i=0; i< num_auto_detect_ports; i++ ) {
		if ( port == auto_detect_status_ports[i] ) {
			StatusPortValid = true;
			break;
		}
	}

	return StatusPortValid;

}


bool CNovraDevice::CreateNetConfigMsg( NetworkConfigPacket *network_config_packet_wrapper, NetSetting *pNetworkSetting, BYTE filter )
{    
	// Switch all multibyte values to network order

    *(network_config_packet_wrapper->DestinationUDPPointer()) = htons(pNetworkSetting->DestUDP);
    *(network_config_packet_wrapper->StatusPortPointer())     = htons(pNetworkSetting->StatusPort);
	memcpy(network_config_packet_wrapper->DefaultGatewayIPPointer() , pNetworkSetting->DefaultGatewayIP, 4);
	memcpy(network_config_packet_wrapper->DestinationIPPointer(), pNetworkSetting->StatusDestIP, 4);
	memcpy(network_config_packet_wrapper->ReceiverIPPointer(), pNetworkSetting->ReceiverIP, 4);
	memcpy(network_config_packet_wrapper->SubnetMaskPointer(), pNetworkSetting->SubnetMask, 4);
//	*(network_config_packet_wrapper->StatusIntervalPointer()) = 5;
//	*(network_config_packet_wrapper->FilterPointer()) = filter;

	if (	( ( (network_config_packet_wrapper->ReceiverIPPointer())[0] & ( 255 - (network_config_packet_wrapper->SubnetMaskPointer())[0] ) ) == 0 ) &&
			( ( (network_config_packet_wrapper->ReceiverIPPointer())[1] & ( 255 - (network_config_packet_wrapper->SubnetMaskPointer())[1] ) ) == 0 ) &&
			( ( (network_config_packet_wrapper->ReceiverIPPointer())[2] & ( 255 - (network_config_packet_wrapper->SubnetMaskPointer())[2] ) ) == 0 ) &&
			( ( (network_config_packet_wrapper->ReceiverIPPointer())[3] & ( 255 - (network_config_packet_wrapper->SubnetMaskPointer())[3] ) ) == 0 )    ) {

		return false;	// All Hosts Address on the subnet

	} else if ( ( (network_config_packet_wrapper->ReceiverIPPointer())[0] == 0 ) || ( (network_config_packet_wrapper->ReceiverIPPointer())[0] >= 224 ) ) {

		return false;	// Leading byte of address out of valid range

	} else {
		
		// Check for broadcast address

		// Exclusive OR the host part of the address with the 1's compliment of the netmask
		// (must have at least one zero bit)

		for ( int i = 0; i < 4; i++ ) {

			if ( (network_config_packet_wrapper->SubnetMaskPointer())[i] < 255 ) { // This byte has part of the host address

				if ( ( ( (network_config_packet_wrapper->ReceiverIPPointer())[i] ) & ( 255 - (network_config_packet_wrapper->SubnetMaskPointer())[i] ) ) ^
					( 255 - (network_config_packet_wrapper->SubnetMaskPointer())[i] ) ) {

					return true;	// Address is OK

				}

			}

		}

		return false;	// Address is the broadcast address on the subnet

	}

}


int CNovraDevice::SendNetworkConfig( NetworkConfigPacket *network_config_packet_wrapper )
{
    TCHAR temp[16];                     // Temporary storage variables.
	bool configurationSent;             // New configuration sent to S75D?
	int LastError;
    bool configurationApplied;          // New configuration applied to S75D?
	int status_recieved;


    // Convert the IP address to a string
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);


    // Send the configuration to the S75D.  If communication fails, retry up to four times.
    for ( int i = 0; i < NUM_RETRIES; i++) {
        configurationSent = ((LastError = SendWaitForConfirmation( temp, NETWORK_CONFIG_PORT, network_config_packet_wrapper->data(), network_config_packet_wrapper->size() )) == ERR_SUCCESS);
        if (configurationSent) break;
    }


    // Check if we were able to send the configuration successfully
    if (configurationSent) {

		if ( m_usingStatusListener ) {
//		if ( m_usingStatusListener && ( GetDeviceType() != S75CA ) ) {

	        FlushStatusBuffer(STATUS_PACKET_TIMEOUT);
			// @@ NS / There is no need for return wihpout comparing against status ... return ERR_SUCCESS;
			// Configuration sent to S75D

	        // Update the IP address member variable
			memcpy(m_Device_IP_Address, network_config_packet_wrapper->ReceiverIPPointer(), 4);

	        // Switch to the new status port
		    if (m_StatusListener.IsListening()) {	 // If the status Listener is Up... 
				StopStatusListener();
				Set_Status_Port(ntohs(*(network_config_packet_wrapper->StatusPortPointer())));
				LastError = ERR_FAILED;
				if ( m_usingStatusListener )
					LastError = StartStatusListener();
				if (LastError != ERR_SUCCESS) return LastError;            
			}
			else {
				Set_Status_Port(ntohs(*(network_config_packet_wrapper->StatusPortPointer())));
				LastError = ERR_FAILED;
				if ( m_usingStatusListener )
					LastError = StartStatusListener();
				if (LastError != ERR_SUCCESS) return LastError;            
			}
        
			// Flush the status packet buffer
			if ((LastError = FlushStatusBuffer(STATUS_PACKET_TIMEOUT))!= ERR_SUCCESS) {
				// Error flushing buffer
				// Don't need to define error code -- done by FlushStatusBuffer()
				return LastError;
			}

			// Wait for the next status packet
			if ((LastError = WaitForStatusPacket(STATUS_PACKET_TIMEOUT))!=ERR_SUCCESS) {
				// Error when waiting for status packet
				// Don't need to define error code -- done by WaitForStatusPacket()
				return LastError;
			}

			status_recieved = ERR_SUCCESS;

#ifdef JSAT
		} else { // Need to poll for status

#ifdef BROADCAST_POLL_FOR_STATUS
			if ( m_remote ) {
#endif
				status_recieved = PollStatus( m_StatusWrapper );
#ifdef BROADCAST_POLL_FOR_STATUS
			} else {
				status_recieved = BroadcastPollStatus( m_StatusWrapper );
			}
#endif
			if ( status_recieved == ERR_SUCCESS ) {
				memcpy(m_Device_IP_Address, network_config_packet_wrapper->ReceiverIPPointer(), 4);

			}
#endif
		}

		if ( status_recieved == ERR_SUCCESS ) {

			// Check to see if the values sent to the S75D are the same as those being
			// reported in the status

			configurationApplied = true;

			if (memcmp(network_config_packet_wrapper->DefaultGatewayIPPointer(), m_StatusWrapper->DefaultGatewayIPPointer() , 4) != 0) 
				configurationApplied = false;
			if (memcmp(network_config_packet_wrapper->DestinationIPPointer(), m_StatusWrapper->DestIPPointer(), 4) != 0) 
				configurationApplied = false;
			if (ntohs(*(network_config_packet_wrapper->DestinationUDPPointer())) != (*(m_StatusWrapper->StatusDestUDPPointer()))) 
				configurationApplied = false;
			if (memcmp(network_config_packet_wrapper->SubnetMaskPointer(), m_StatusWrapper->SubnetMaskPointer(), 4) != 0) 
				configurationApplied = false;

			if ( !ConfirmDeviceSpecificNetworkSettings( m_StatusWrapper, network_config_packet_wrapper ) )
				configurationApplied = false;

			if (!configurationApplied) 
				// Configuration not successfully applied
				return ERR_NETCFG_NOTAPPLIED;
			else 
				// Configuration successfully applied
				return ERR_SUCCESS;

		} else {

			return status_recieved;

		}

	} else {
		// Configuration not sent to S75D
		// Don't need to define error code -- done by SendWaitForConfirmation()
		return LastError;
	}

}


int CNovraDevice::SendPIDs( PIDSettingsPacket  *pid_setting_packet_wrapper, int size, int max_pids, bool forward_raw )
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
	int PIDsUploaded;                   // Number of PIDs uploaded to the S75
    int PIDsDownloaded;                 // Number of PIDs downloaded to the S75
    bool PIDInList;                     // Is the PID we are looking at in the current list?
	PIDList *verification_pid_table;
	int j;


	  // Convert the IP address to a string.
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);

    // Create a socket to communicate with the S75


#ifndef LINUX
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) {
#else
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) {
#endif
		last_error_code = ERR_PID_SOCKET_CREATE;

	} else {

        // Connect the newly created socket to the S75.

    	device_address.sin_family = AF_INET;

    	device_address.sin_port = htons( PID_CONFIG_PORT );

    	device_address.sin_addr.s_addr = inet_addr( temp );

    	if ( connect( s, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr_in ) ) != 0){

            last_error_code = ERR_PID_SOCKET_CONNECT;

		} else {

            // Go through the PID table to upload and change all the PIDs and
            // UDP ports from host order to network order.

			pid_setting_packet_wrapper->hton( max_pids, forward_raw );

            // Create a buffer to hold the data we wish to send

            BYTE *Buffer1=NULL;
			Buffer1 = new BYTE[size];

            memcpy(Buffer1, pid_setting_packet_wrapper->data(), size); // copy the data in the buffer.
			 

            // Create a buffer the size of the PID Table to hold the
            // response we receive back.

			BYTE *Buffer2=NULL;
			Buffer2 = new BYTE[size];

            for (i = 0; (!success) && ( i < NUM_RETRIES ); i++) {

    			// Send the PID table to the S75

                if ( send( s, (const char *)Buffer1, size, 0 ) != size ) {

					last_error_code = ERR_PID_SEND;		
		
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
			            		 last_error_code = ERR_PID_RECEIVE;
					} // if

#else
        		    	if( (temp1 = recv( s, (char *)Buffer2, size, 0 ) ) == -1 ) {
					 last_error_code = ERR_PID_RECEIVE;

#endif

						}  // if

    			        // Check to make sure the data received back is the same data sent.

    			        if(temp1 ==  size){

							success = true;

	    	        		for(i = 0 ; i < size; i++ ){

		    	        		if(Buffer1[i] != Buffer2[i]) {

		    		        		last_error_code = ERR_PID_ACK_BAD;

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

	if ( success ) {

		success = false;

		verification_pid_table = PIDListWrapper();

        // Try to download the PID table

        for ( i = 0; (!success) && ( i < NUM_RETRIES ); i++ ) {

            if ( GetPIDTable((LPBYTE)(verification_pid_table->DataPointer())) == ERR_SUCCESS ) {

				success = true;

			} else {

                if (i == 3) {

                    last_error_code = ERR_PIDSET_NOTAPPLIED;
				}

            }

        }

		if ( success ) {

			// Check the PID list with the one we just downloaded.  Note that the 
			// PID list we just downloaded could be in a different order from ours.

			// Start by counting the number of PIDs uploaded and the number of PIDs 
			// downloaded and comparing the two numbers.

			configurationApplied = true;

			PIDsDownloaded = 0;

			PIDsUploaded = 0;

			for (i = 0; i < max_pids; i++) {

				if ((verification_pid_table->EntryPointer())[i] != NULL_PID) {
					PIDsDownloaded++;

				}


				if ((m_PIDList->EntryPointer())[i] != NULL_PID) {
					
					PIDsUploaded++;
				}

			}

            if (PIDsUploaded != PIDsDownloaded) {
				
				configurationApplied = false;

			}

            // Go through the PID list downloaded and check to make sure that each PID we
            // uploaded is somewhere in that list.

			for (i = 0; i < max_pids; i++) {

				PIDInList = false;


	            if ((m_PIDList->EntryPointer())[i] != NULL_PID) {     // Skip null PIDs

		            for (j = 0; j < max_pids; j++) {

						if (((verification_pid_table->EntryPointer())[j]== (m_PIDList->EntryPointer())[i])&&((verification_pid_table->ControlPointer())[j]== (m_PIDList->ControlPointer())[i])) {
							PIDInList = true;
	                        break;
		                }

			        }

				} else {

					PIDInList = true;

	            }

				if (!PIDInList) {
					configurationApplied = false;

				}

			}

	        if (!configurationApplied) {

				// Configuration not successfully applied

				last_error_code = ERR_PIDSET_NOTAPPLIED;

				success = false;

			} else {

				// Configuration successfully applied

				last_error_code = ERR_SUCCESS;

				success = true;

			}

		}

		verification_pid_table->DeletePIDList();

		delete verification_pid_table;

	}

	return last_error_code;

}



int CNovraDevice::GetPIDs( int size )
{
	TCHAR temp[16];
	WORD opcode;                    // Opcode to send to S75
	SOCKET s;
	int last_error_code ;
	struct sockaddr_in device_address;
	int i;
	bool success=false;
	struct timeval timeout;
	fd_set readfs;
	int result;
	int temp1 = 1000;               // Temporary storage variable
    BYTE *Buffer2 = NULL;
	PIDSettings_MSG  PidSettingMSG;


	// Convert the IP address to a string.
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);

	// Set the opcode to send to the S75 to download (0x0001)

    opcode = htons(0x0001);




	// Create a socket to communicate with the S75

#ifndef LINUX
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) {
#else
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) {
#endif


        last_error_code = ERR_PID_SOCKET_CREATE;

	} else {

        // Connect the newly created socket to the S75.

    	device_address.sin_family = AF_INET;

    	device_address.sin_port = htons( PID_CONFIG_PORT );

    	device_address.sin_addr.s_addr = inet_addr( temp);

    	if ( connect( s, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr_in ) ) != 0){

    		last_error_code = ERR_PID_SOCKET_CONNECT;

		} else {

            // Create a buffer the size of the PID Table to hold the
            // response we receive back.

			Buffer2 = new BYTE[size];

			
            // Download the PID table.  Retry (if required) up to 4 times

            for (i = 0; (!success) && ( i < NUM_RETRIES ); i++) {

            	// Send the PID table to the S75

                if ( send( s, (const char *)(&opcode), sizeof( WORD), 0 ) != sizeof( WORD ) ) {

                    last_error_code = ERR_PID_SEND;

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
    		    	        if( (temp1 = recv( s, (char *)Buffer2, size, 0 ) ) == SOCKET_ERROR ) {

	            	    		temp1 = GetLastError();

	                	        if(temp1 != WSAETIMEDOUT){

	        	        			last_error_code = ERR_PID_RECEIVE;

								} // if


#else
    		    	        if( (temp1 = recv( s, (char *)Buffer2, size, 0 ) ) == -1 ) {
					last_error_code = ERR_PID_RECEIVE;
#endif

							} else {

		    	    	        // Make sure we have received the right number of bytes back

	    		    	        if (temp1 == size) {

	            		    		memcpy(&PidSettingMSG, Buffer2, size);

									TranscodePIDList( &PidSettingMSG );

									last_error_code = ERR_SUCCESS;

									success = true;

								} else {
                                    last_error_code = ERR_PID_ACK_BAD;

								} // if

							} // if recv

						} else {

							last_error_code = ERR_PID_RECEIVE;

						} // if select result 0

					} else {

						last_error_code = ERR_PID_RECEIVE;

					} // if select

				} // if send
	
			} // for 4 retires

			delete [] Buffer2;

		} // if socket connect

#ifndef LINUX
        closesocket( s );
#else
        close( s );
#endif

	} // if socket create


	return last_error_code;

}


bool CNovraDevice::GetDefaultConfigStruct( void *pConfig )
{
	NetSetting *networkSettings;


	networkSettings = (NetSetting *) pConfig;

	// Setup network config structure

	networkSettings->StatusDestIP[0] = 10;
	networkSettings->StatusDestIP[1] = 10;
	networkSettings->StatusDestIP[2] = 10;
	networkSettings->StatusDestIP[3] = 10;

    networkSettings->SubnetMask[0] = 255;
  	networkSettings->SubnetMask[1] = 255;
  	networkSettings->SubnetMask[2] = 255;
  	networkSettings->SubnetMask[3] = 0;
  
    networkSettings->DefaultGatewayIP[0] = 10;
    networkSettings->DefaultGatewayIP[1] = 10;
    networkSettings->DefaultGatewayIP[2] = 10;
    networkSettings->DefaultGatewayIP[3] = 10;
  
	memcpy( networkSettings->ReceiverIP, m_Device_IP_Address, 4 );
  		
  	networkSettings->StatusPort = m_Device_Status_Port;
  		
  	return true;	

}


int CNovraDevice::ConfigureDVBMAC(LPBYTE DVBMAC)
{
	bool configurationSent;             // New configuration sent to S75D?
    bool configurationApplied;          // New configuration applied to S75D?
    BYTE Buffer[10];                    // Buffer to hold data to send to S75D
    TCHAR temp[16];                     // Temporary storage variables.
	int LastError;



    // There are two possible methods of communicating -- by IP address or
    // by MAC address.  For the former, make sure the IP address is set.
    // For the later, make sure the network path is set
	if ((!m_Device_attached)&&(!m_NetPathSet)) {
        // Trying to communicate by IP with no IP address -- error
         return ERR_MACPGM_NONETPATH;
	}

    // Destination information properly set

    // Copy the data to sent into a buffer
	memcpy(Buffer, DVBMAC, 6);

    // Convert the IP address to a string
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);


    // Send the configuration to the S75D.  If communication fails, retry up to four times.
    for (int i = 0; i < NUM_RETRIES; i++) {
        configurationSent =((LastError = SendWaitForConfirmation( temp, DVB_MAC_CONFIG, Buffer, 6 ))== ERR_SUCCESS);
        if (configurationSent) break;
    }

    // Check if we were able to send the configuration successfully
    if (configurationSent) {
        // Configuration sent to S75D

        // Flush the status packet buffer
        if ((LastError = FlushStatusBuffer(10000))!= ERR_SUCCESS) {
            // Error flushing buffer
            // Don't need to define error code -- done by FlushStatusBuffer()
            return LastError;
        }

        // Wait for the next status packet
        if ((LastError = WaitForStatusPacket(STATUS_PACKET_TIMEOUT))!=ERR_SUCCESS) {
            // Error when waiting for status packet
            // Don't need to define error code -- done by WaitForStatusPacket()
            return LastError;
        }

        // Check to see if the values sent to the S75D are the same as those being
        // reported in the status
        configurationApplied = true;

//        if (memcmp(DVBMAC, m_Status->DVBMAC, 6) != 0) configurationApplied = false;
        if ( !DVBMACPointer() || memcmp(DVBMAC, DVBMACPointer(), 6) ) configurationApplied = false;

        if (!configurationApplied) 
            // Configuration not successfully applied
			return ERR_MACPGM_NOTAPPLIED;
		else 
            // Configuration successfully applied
            return ERR_SUCCESS;
	} else {
        // Configuration not sent to S75D
        return LastError;
    }


}
