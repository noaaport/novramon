#ifdef JSAT

// s75_jleo.cpp: implementation of the CS75_JLEO class.
//
//////////////////////////////////////////////////////////////////////
#include "s75_jleo.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <iostream>
using namespace std;
#include "S75JLEOStatus.h"

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

extern "C"
int __stdcall findAdapters( PIP_ADAPTER_INFO *adapters, ULONG *buffer_len );

#endif


#ifdef LINUX
    	#include <time.h>
    	#include <string.h>
	#include <stdlib.h>
	#include <net/if.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
    	#include "linux_windows_types.h"
#endif

	const int NETWORK_CONFIG_PORT =0x1975;
	const int RF_CONFIG_PORT = 0x1983;
	const int TRAP_CONFIG_PORT = 0x2005;
	const int DVB_MAC_CONFIG = 0x1979;
	const int RESET_PORT = 0x9999;
	const int PID_CONFIG_PORT = 0x1977;
	const int STATUS_POLL_PORT = 0x2011;
	const int UID_CONFIG_PORT = 0x1984;
	const int MAC_ADDRESS_CONFIG_PORT = 0xDCAB;
	const int PASSWORD_CHANGE_PORT = 0x2010;
	const int LNB_HIGH = 1;                         // Possible values for 'band' parameter in TuneS75D() function
	const int LNB_LOW = 0;

	const int LNB_HORIZONTAL = 1;                   // Possible values for 'polarity' parameter in TuneS75D() function
	const int LNB_LEFT = 1;
	const int LNB_VERTICAL = 0;
	const int LNB_RIGHT = 0;
	const int	MAX_SUPP_PIDS = 16;

// CONSTANTS for converting AGC to percentage

	const double  AGC[]        = { 254, 112, 105, 95 };  // Automatic Gain Control
	const double  PERCENTAGE[] = { 10, 65, 80, 100 };
	const int     SIZE_ARRAY   = 4;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS75_JLEO::CS75_JLEO( bool remote )
{

	m_StatusPacketSize = sizeof(S75JLEO_Status_MSG);
	m_Device_Type = S75_JLEO;  // when we instantiate this class, the device type will be enforced on attachment.
	m_StatusWrapper = (StatusPacket *)(new S75JLEOStatusPacket( &m_Status ));
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

CS75_JLEO::~CS75_JLEO()
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


int CS75_JLEO::SpecifyPassword( char password[8] )
{
	memcpy( m_sec.password, password, 8 );

	return ERR_SUCCESS;
}


bool CS75_JLEO::ParseStatusPacket()
{
	BYTE temp[4];
	/* bool  FWMatch = false ; */

	// If this function is called, the status Structure has been filled with data received by Status Listner.
	// We need to verify that the packet is valid, and that it matches the device to be valid.

	if ( !CNovraDevice::ParseStatusPacket() ) return false;

	((S75JLEOStatusPacket *)m_StatusWrapper)->ntoh();

//	*(((S75JLEOStatusPacket *)m_StatusWrapper)->FilterPointer())		= ntohs(*(((S75JLEOStatusPacket *)m_StatusWrapper)->FilterPointer()));

//	(((S75JLEOStatusPacket *)m_StatusWrapper)->TotalDVBPacketsAcceptedPointer())[0]	 	= ntohl((((S75JLEOStatusPacket *)m_StatusWrapper)->TotalDVBPacketsAcceptedPointer())[0]);
//	(((S75JLEOStatusPacket *)m_StatusWrapper)->TotalDVBPacketsAcceptedPointer())[1]		= ntohl((((S75JLEOStatusPacket *)m_StatusWrapper)->TotalDVBPacketsAcceptedPointer())[1]);
//	(((S75JLEOStatusPacket *)m_StatusWrapper)->TotalDVBPacketsRXInErrorPointer())[0]	= ntohl((((S75JLEOStatusPacket *)m_StatusWrapper)->TotalDVBPacketsRXInErrorPointer())[0]);
//	(((S75JLEOStatusPacket *)m_StatusWrapper)->TotalDVBPacketsRXInErrorPointer())[1]	= ntohl((((S75JLEOStatusPacket *)m_StatusWrapper)->TotalDVBPacketsRXInErrorPointer())[1]);
//	(((S75JLEOStatusPacket *)m_StatusWrapper)->TotalEthernetPacketsOutPointer())[0]		= ntohl((((S75JLEOStatusPacket *)m_StatusWrapper)->TotalEthernetPacketsOutPointer())[0]);
//	(((S75JLEOStatusPacket *)m_StatusWrapper)->TotalEthernetPacketsOutPointer())[1]		= ntohl((((S75JLEOStatusPacket *)m_StatusWrapper)->TotalEthernetPacketsOutPointer())[1]);
//	(((S75JLEOStatusPacket *)m_StatusWrapper)->TotalUncorrectableTSPacketsPointer())[0]	= ntohl((((S75JLEOStatusPacket *)m_StatusWrapper)->TotalUncorrectableTSPacketsPointer())[0]);
//	(((S75JLEOStatusPacket *)m_StatusWrapper)->TotalUncorrectableTSPacketsPointer())[1]	= ntohl((((S75JLEOStatusPacket *)m_StatusWrapper)->TotalUncorrectableTSPacketsPointer())[1]);
//	*(((S75JLEOStatusPacket *)m_StatusWrapper)->BroadcastStatusPortPointer())			= ntohs(*(((S75JLEOStatusPacket *)m_StatusWrapper)->BroadcastStatusPortPointer()));
//	*(((S75JLEOStatusPacket *)m_StatusWrapper)->LocalOscFrequencyPointer())				= ntohs(*(((S75JLEOStatusPacket *)m_StatusWrapper)->LocalOscFrequencyPointer()));

	if(m_MACAddressValid){
		if(memcmp(m_StatusWrapper->ReceiverMACPointer(), m_Device_MAC_Address,6)!=0)
			return false;
	}
//	for(int i=0;i<MAX_SUPP_PIDS;i++)
//		(((S75JLEOStatusPacket *)m_StatusWrapper)->PIDListPointer())[i] = ntohs((((S75JLEOStatusPacket *)m_StatusWrapper)->PIDListPointer())[i]);


	if(((*(m_StatusWrapper->DSPFirmWarePointer())) & 0xFF00) != 0x5500 )			// Communication rev. JSAT Leo
	 return false ;		/// if the device does not follow communication rev JSAT Leo it is not supported.


	if(m_Device_Type != S75_JLEO) return false;

	m_StatusListener.GetLastPacketSource(temp);

	memcpy(m_Device_IP_Address, temp,4); // Get the address from the Unit.

	return true;
}


bool CS75_JLEO::CreateNetConfigMsg( NetworkConfigPacket *network_config_packet_wrapper, NetSetting *pNetworkSetting, BYTE filter )
{
	for ( int i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	if ( CNovraDevice::CreateNetConfigMsg( network_config_packet_wrapper, pNetworkSetting, filter ) ) {

		*(((S75JLEONetworkConfigPacket *)network_config_packet_wrapper)->IGMPPointer()) = filter;
		*(((S75JLEONetworkConfigPacket *)network_config_packet_wrapper)->IGMPPointer()) = htons( *(((S75JLEONetworkConfigPacket *)network_config_packet_wrapper)->IGMPPointer()) );

		srand( time(NULL) );
		*(((S75JLEONetworkConfigPacket *)network_config_packet_wrapper)->RandomPointer()) = rand();

		digitalsignature( &m_sec, (uint8_t *)(network_config_packet_wrapper->data()), sizeof( S75JLEONetConfig_MSG ) - 20 );
		memcpy( ((S75JLEONetworkConfigPacket *)network_config_packet_wrapper)->SignaturePointer(), m_sec.digest, 20 );

		return true;

	} else {

		return false;

	}

}


int CS75_JLEO::ConfigureNetwork(void *pNetworkSetting)
{

	S75JLEONetConfig_MSG  net_setting_msg;
	S75JLEONetworkConfigPacket network_config_packet_wrapper( &net_setting_msg );


   	// There are two possible methods of communicating -- by IP address or
   	// by MAC address.  For the former, make sure the IP address is set.
   	// For the later, make sure the network path is set

   	if ((!m_Device_attached) && (!m_remote) && (!m_NetPathSet) )
       // Trying to communicate by IP with no network path -- error
       return ERR_NETCFG_NONETPATH;
    
    // Destination information properly set

	if ( !CreateNetConfigMsg( (NetworkConfigPacket *)(&network_config_packet_wrapper), (NetSetting *)pNetworkSetting, 4 * (((S75_NetSetting_JLEO *)pNetworkSetting)->IGMP_Enable) ) )

	// we should Check the Status Port to Make sure it is in the auto detection List.
	
#ifndef BROADCAST_POLL_FOR_STATUS
	if ( !CheckStatusPort( ((NetSetting *)pNetworkSetting)->StatusPort ) )
		return ERR_BAD_STATUS_PORT;
#endif

	if ( !CheckStatusPort( ((NetSetting *)pNetworkSetting)->DestUDP ) )
		return ERR_BAD_STATUS_PORT;

	return SendNetworkConfig( (NetworkConfigPacket *)(&network_config_packet_wrapper) );

}


void CS75_JLEO::CreateRFConfigMsg( S75JLEORFConfigPacket *rf_config_packet_wrapper, void *TuningParameters )
{

	CSatelliteDevice::CreateRFConfigMsg( rf_config_packet_wrapper, TuningParameters );

	(*(((S75JLEORFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) = 0;

	if ( ((S75_RFSetting_JLEO *)TuningParameters)->band == LNB_HIGH )
		(*(((S75JLEORFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 4;

    if ( ( ((S75_RFSetting_JLEO *)TuningParameters)->polarity == LNB_HORIZONTAL ) ||
		 ( ((S75_RFSetting_JLEO *)TuningParameters)->polarity == LNB_LEFT ) )
		(*(((S75JLEORFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 2;

    if ( ((S75_RFSetting_JLEO *)TuningParameters)->powerOn )
		(*(((S75JLEORFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 1;

	if ( ((S75_RFSetting_JLEO *)TuningParameters)->LongLineCompensation )
		(*(((S75JLEORFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 16 ; // Shall we enable Long line Compensation  ?

	if ( ((S75_RFSetting_JLEO *)TuningParameters)->HighVoltage )
		(*(((S75JLEORFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 8 ;  // Account for High Voltage Selection .

	(*(((S75JLEORFConfigPacket *)rf_config_packet_wrapper)->LocalOscFrequencyPointer())) = htons( ((S75_RFSetting_JLEO *)TuningParameters)->LocalOscFrequency );

	srand( time(NULL) );
	(*(((S75JLEORFConfigPacket *)rf_config_packet_wrapper)->RandomPointer())) = rand();

	for ( int i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	digitalsignature( &m_sec, (uint8_t *)(((S75JLEORFConfigPacket *)rf_config_packet_wrapper)->data()), sizeof( S75JLEORFConfig_MSG ) - 20 );

	memcpy( ((S75JLEORFConfigPacket *)rf_config_packet_wrapper)->SignaturePointer(), m_sec.digest, 20 );

}


int CS75_JLEO::TuneDevice(void *TuningParameters)
{

	S75JLEORFConfig_MSG		rf_config_msg;
	S75JLEORFConfigPacket	rf_config_packet_wrapper( &rf_config_msg ); 


	// There are two possible methods of communicating -- by IP address or
	// by MAC address.  For the former, make sure the IP address is set.
	// For the later, make sure the network path is set

	if (!m_Device_attached)		    // Trying to communicate and the class not attached to physical device
		return ERR_RFCFG_NOIP;

	CreateRFConfigMsg( &rf_config_packet_wrapper, TuningParameters );

    return SendRFConfig( &rf_config_packet_wrapper );

}


int CS75_JLEO::SetTraps(void *TrapParameters)
{
	bool			configurationSent;             // New configuration sent to S75?
	bool			configurationApplied=false;          // New configuration applied to S75?
	BYTE			Buffer[sizeof(S75JLEO_Event_Trap_Set_MSG)];                    // Buffer to hold data to send to S75
	S75JLEO_Event_Trap_Set_MSG *	pTrapSettings;
	TCHAR			temp[16];                      // Temporary storage variable
	int			LastError;
	int			i;


	m_pS75JLEO_TrapSetting  = (S75_TrapSetting_JLEO *) TrapParameters ;		// Type Cast the parameters Passed into the structure 
	pTrapSettings = (S75JLEO_Event_Trap_Set_MSG *) Buffer;					// Initialize the structure pointer to the Buffer

	// There are two possible methods of communicating -- by IP address or
	// by MAC address.  For the former, make sure the IP address is set.
	// For the later, make sure the network path is set

	if (!m_Device_attached)		    // Trying to communicate and the class not attached to physical device
		return ERR_TRAPCFG_NOIP;

	memset( pTrapSettings, 0, sizeof( S75JLEO_Event_Trap_Set_MSG ) );
	pTrapSettings->MonitoringInterval = htons( (unsigned short)( m_pS75JLEO_TrapSetting->MonitoringInterval  ) );
	memcpy( pTrapSettings->NotificationIPAddress, m_pS75JLEO_TrapSetting->NotificationIP, 4 );
	pTrapSettings->NotificationPort = htons( (unsigned short)( m_pS75JLEO_TrapSetting->NotificationPort  ) );
	pTrapSettings->DataLockMonitorOn = htons( (unsigned short)( m_pS75JLEO_TrapSetting->DataLockMonitorOn  ) );
	pTrapSettings->VBERThreshold = htons( (unsigned short)( m_pS75JLEO_TrapSetting->VBERThreshold  ) );
	*((unsigned long *)(pTrapSettings->UncorrectableTSErrorThreshold)) = htonl( (unsigned long)( m_pS75JLEO_TrapSetting->UncorrectableTSErrorThreshold  ) );
	pTrapSettings->EthernetTxErrorThreshold = htons( (unsigned short)( m_pS75JLEO_TrapSetting->EthernetTxErrorThreshold  ) );
	pTrapSettings->Mask = 0;
	if ( m_pS75JLEO_TrapSetting->DataLockMonitorOn ) {
		pTrapSettings->Mask += 1;
	}
	if ( m_pS75JLEO_TrapSetting->VBEROn ) {
		pTrapSettings->Mask += 2;
	}
	if ( m_pS75JLEO_TrapSetting->TSErrorOn ) {
		pTrapSettings->Mask += 4;
	}
	if ( m_pS75JLEO_TrapSetting->EthernetTxErrorOn ) {
		pTrapSettings->Mask += 8;
	}
	pTrapSettings->Mask = htons( (unsigned short)( pTrapSettings->Mask  ) );

	srand( time(NULL) );
	pTrapSettings->Random = rand();

//	memset( (void *)(&m_sec), 0, sizeof( m_sec ) );

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}


	digitalsignature( &m_sec, (uint8_t *)pTrapSettings, sizeof( S75JLEO_Event_Trap_Set_MSG ) - 20 );

	
	memcpy( pTrapSettings->Signature, m_sec.digest, 20 );

//	for ( i = 0; i < 20; i++ ) {
//		pRFSettings->Signature[i] = m_sec.digest[i];
//	}

	// Now the Data is in the buffer and all is set and ready for transmission.
	// Now we construct the destination string.
	sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);
	
	for (i = 0; i < NUM_RETRIES; i++) {
        configurationSent = ((LastError = SendWaitForConfirmation( temp, TRAP_CONFIG_PORT, Buffer, sizeof(S75JLEO_Event_Trap_Set_MSG))) == ERR_SUCCESS);
	if (configurationSent) break;
	}


	// Check if we were able to send the configuration successfully
	if (configurationSent) {
/*
        // Configuration sent to S75D

	// Flush the status packet buffer
        if ((LastError =FlushStatusBuffer(STATUS_PACKET_TIMEOUT)) !=ERR_SUCCESS ) {
            // Error flushing buffer
            // Don't need to define error code -- done by FlushStatusBuffer()
            return LastError;
        }
		    if ((LastError =FlushStatusBuffer(STATUS_PACKET_TIMEOUT)) !=ERR_SUCCESS ) {
            // Error flushing buffer
            // Don't need to define error code -- done by FlushStatusBuffer()
            return LastError;
        }

        // Wait for the next status packet
        if ((LastError = WaitForStatusPacket(STATUS_PACKET_TIMEOUT)) != ERR_SUCCESS) {
            // Error when waiting for status packet
            // Don't need to define error code -- done by WaitForStatusPacket()
            return LastError;
        }

        // Check to see if the values sent to the S75D are the same as those being
        // reported in the status
*/
        configurationApplied = true;

//        if ((m_StatusStructure.SymbolRate < ((m_pS75JLEO_RFSetting->symbolRate * 1000) - 10)) ||
//            (m_StatusStructure.SymbolRate> ((m_pS75JLEO_RFSetting->symbolRate * 1000) + 10))) configurationApplied = false;
//        if ((m_StatusStructure.Frequency < ((m_pS75JLEO_RFSetting->frequency * 10) - 100)) ||
//            (m_StatusStructure.Frequency > ((m_pS75JLEO_RFSetting->frequency * 10) + 100))) configurationApplied = false;
//        if ((m_StatusStructure.LNBConfiguration & 0x01) != m_pS75JLEO_RFSetting->powerOn) configurationApplied = false;
//        if (((m_StatusStructure.LNBConfiguration & 0x02)>>1) != m_pS75JLEO_RFSetting->polarity) configurationApplied = false;
//        if (((m_StatusStructure.LNBConfiguration & 0x04)>>2) != m_pS75JLEO_RFSetting->band) configurationApplied = false;
//        if (((m_StatusStructure.LNBConfiguration & 0x08)>>3) != m_pS75JLEO_RFSetting->HighVoltage) configurationApplied = false;
//        if (((m_StatusStructure.LNBConfiguration & 0x010)>>4) != m_pS75JLEO_RFSetting->LongLineCompensation) configurationApplied = false;
        
        if (!configurationApplied) 
			return ERR_RFCFG_NOTAPPLIED;
		else      // Configuration successfully applied
            return ERR_SUCCESS;
	}else{ 
		// Configuration not sent to S75
		return LastError; // This will be the error returned by Send wait for confirmation function.
	}

}


int CS75_JLEO::ResetDevice()
{
//	BYTE dataBuffer[26] = {0x19, 0x75, 0x14, 0x03};  // Data buffer (data to be sent to S75)
	S75JLEO_Reset_MSG reset_message;
    TCHAR temp[16]; 	// Temporary storage variable
	int LastError;
	int i;

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

	reset_message.ResetKey[0] = 0x19;
	reset_message.ResetKey[1] = 0x75;
	reset_message.ResetKey[2] = 0x14;
	reset_message.ResetKey[3] = 0x03;

	srand( time(NULL) );
	reset_message.Random = rand();

//	memset( (void *)(&m_sec), 0, sizeof( m_sec ) );

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	digitalsignature( &m_sec, (uint8_t *)(&reset_message), sizeof( S75JLEO_Reset_MSG ) - 20 );

	
	memcpy( reset_message.Signature, m_sec.digest, 20 );

//	for ( i = 0; i < 20; i++ ) {

    // Send the reset command to the S75.  If communication fails, retry up to four times.
    for (i = 0; i < NUM_RETRIES; i++) {
        if ((LastError = SendWaitForConfirmation( temp, RESET_PORT, (BYTE *)(&reset_message), sizeof( S75JLEO_Reset_MSG ) ))== ERR_SUCCESS) break;

        if (i == 3) {
            // Failed to reset S75
            // Don't need to define error code -- done by SendWaitForConfirmation()
            return LastError;
        }        
    }

    // S75 Reset successfully
    return LastError;

}


int CS75_JLEO::SetPIDTable(LPBYTE PIDTable)
{
	int temp1 = 1000;               // Temporary storage variable
	//int count = 0 ;               Temporary storage variable
	// DWORD xx = 1;                   Temporary storage variable
	int i;
	int j;
	SOCKET s;
	struct sockaddr_in device_address;
	fd_set readfs;
	struct timeval timeout;
	TCHAR  temp[16];
	int result;
	bool success = false;
	S75_PIDList_JLEO verification_pid_table;
	bool configurationApplied=false;          // New configuration applied to S75?
    bool PIDInList;                     // Is the PID we are looking at in the current list?
	int PIDsUploaded;                   // Number of PIDs uploaded to the S75
    int PIDsDownloaded;                 // Number of PIDs downloaded to the S75
	int last_error_code ;
	S75JLEO_PIDSetting_MSG PidSettingMsg;
	
	m_pS75JLEO_PIDList = (S75_PIDList_JLEO *) PIDTable ;



    // Set the opcode in the PID table to Upload (0x0002)

	PidSettingMsg.Opcode = htons(0x0002);

    // Copy the PID table to download into the CPIDProgram object
	if(m_pS75JLEO_PIDList->NumberofPIDs > MAX_SUPP_PIDS) return ERR_PID_TOOLONG ;

    for (i = 0; i < MAX_SUPP_PIDS; i++) {

//        for ( j =0; j < 4; j++ ) {

//            PidSettingMsg.PIDTableEntry[i].DestinationIP[j] = 0;

//		} // for j

//        PidSettingMsg.PIDTableEntry[i].DestinationUDP = 0;
		if(i<m_pS75JLEO_PIDList->NumberofPIDs){
			PidSettingMsg.PIDTableEntry[i] = m_pS75JLEO_PIDList->Entry[i] ;
			if(m_pS75JLEO_PIDList->Entry[i] != NULL_PID)
				PidSettingMsg.PIDTableEntry[i] |= 0x2000;
		}else{
			PidSettingMsg.PIDTableEntry[i] = NULL_PID;
		}

    } // for i
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

            for (i = 0; i < MAX_SUPP_PIDS; i++) {

                PidSettingMsg.PIDTableEntry[i] = htons(PidSettingMsg.PIDTableEntry[i]);

                // @@NS  V2 does not support individual route
				/// Sve the processing m_PIDTable.PIDTableEntry[i].DestinationUDP = htons(m_PIDTable.PIDTableEntry[i].DestinationUDP);

			} // for


	srand( time(NULL) );
	PidSettingMsg.Random = rand();

//	memset( (void *)(&m_sec), 0, sizeof( m_sec ) );

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	digitalsignature( &m_sec, (uint8_t *)(&PidSettingMsg), sizeof( S75JLEO_PIDSetting_MSG ) - 20 );

	
	memcpy( PidSettingMsg.Signature, m_sec.digest, 20 );


//	for ( i = 0; i < 20; i++ ) {
            // Create a buffer to hold the data we wish to send

            BYTE Buffer1[sizeof(S75JLEO_PIDSetting_MSG)];
            memcpy(Buffer1, &PidSettingMsg, sizeof(S75JLEO_PIDSetting_MSG)); // copy the data in the buffer.
			 


            // Create a buffer the size of the PID Table to hold the
            // response we receive back.

			BYTE Buffer2[sizeof(S75JLEO_PIDSetting_MSG)];

            for (i = 0; (!success) && ( i < NUM_RETRIES ); i++) {

    			// Send the PID table to the S75

                if ( send( s, (const char *)Buffer1, sizeof( S75JLEO_PIDSetting_MSG), 0 ) != sizeof( S75JLEO_PIDSetting_MSG ) ) {

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

        		    	if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75JLEO_PIDSetting_MSG), 0 ) ) == SOCKET_ERROR ) {
	        		    	temp1 = GetLastError();
		            		if(temp1 != WSAETIMEDOUT){
			            		 last_error_code = ERR_PID_RECEIVE;
					} // if

#else
        		    	if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75JLEO_PIDSetting_MSG), 0 ) ) == -1 ) {
					 last_error_code = ERR_PID_RECEIVE;

#endif

						}  // if

    			        // Check to make sure the data received back is the same data sent.

    			        if(temp1 ==  sizeof(S75JLEO_PIDSetting_MSG)){

							success = true;

	    	        		for(i = 0 ; i < (int)sizeof(S75JLEO_PIDSetting_MSG); i++ ){

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

		} //.if socket connect

#ifndef LINUX
	  	closesocket( s );
#else
	  	close( s );
#endif

	} // if socket create

	if ( success ) {

		success = false;

        // Try to download the PID table

        for ( i = 0; (!success) && ( i < NUM_RETRIES ); i++ ) {

            if ( GetPIDTable((LPBYTE)&verification_pid_table) == ERR_SUCCESS ) {

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

			for (i = 0; i < MAX_SUPP_PIDS; i++) {

				if (verification_pid_table.Entry[i] != NULL_PID) {

					PIDsDownloaded++;

				}

				if (m_pS75JLEO_PIDList->Entry[i] != NULL_PID) {
					
					PIDsUploaded++;
				}

			}

            if (PIDsUploaded != PIDsDownloaded) {
				
				configurationApplied = false;

			}

            // Go through the PID list downloaded and check to make sure that each PID we
            // uploaded is somewhere in that list.

			for (i = 0; i < MAX_SUPP_PIDS; i++) {

				PIDInList = false;

	            if (m_pS75JLEO_PIDList->Entry[i] != NULL_PID) {     // Skip null PIDs

		            for (j = 0; j < MAX_SUPP_PIDS; j++) {

						if ((verification_pid_table.Entry[j]== m_pS75JLEO_PIDList->Entry[i])&&(verification_pid_table.Control[j]== m_pS75JLEO_PIDList->Control[i])) {
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

	}

    return last_error_code;


}


void CS75_JLEO::TranscodeStatus( S75_Status_JLEO *response, S75JLEOStatusPacket *status_packet_wrapper )
{
	int BERmantissa, BERexponent;   // Bit Error Rate mantissa and exponent
	int j;

	// Copy the Network settings.
	memcpy(response->DefaultGatewayIP, status_packet_wrapper->DefaultGatewayIPPointer(),4);
	memcpy(response->UniCastStatusDestIP, status_packet_wrapper->DestIPPointer(),4);
	memcpy(response->SubnetMask, status_packet_wrapper->SubnetMaskPointer(), 4);
	memcpy(response->ReceiverMAC, status_packet_wrapper->ReceiverMACPointer(), 6);
	memcpy(response->ReceiverIP, status_packet_wrapper->ReceiverIPPointer(),4 );
	memcpy(response->DVB_MACADDRESS, status_packet_wrapper->DVBMACPointer(),6);
	memcpy(response->Reserved, status_packet_wrapper->ReservedPointer(),8);
	memcpy(response->UID, status_packet_wrapper->ProductIDPointer(),32);

	response->UniCastStatusDestUDP = (*(status_packet_wrapper->StatusDestUDPPointer()));
	response->BroadcastStatusPort = (*(status_packet_wrapper->BroadcastStatusPortPointer()));
		
	// If the RF valid == 0x0400 then RFStatusvalid =1 (easier to read for external programmers...:P)
	response->RFStatusValid = ((*(status_packet_wrapper->RFStatusValidPointer())) == 0x0400)? 1:0;
	response->SymbolRate = (float) ((*(status_packet_wrapper->SymbolRatePointer())) / 1000.0) ; // Convert Symbol rate to float.
	response->Frequency = (float) ((*(status_packet_wrapper->FrequencyPointer())) / 10.0);  // Convert Frequency to Float 
	response->LocalOscFrequency = (*(status_packet_wrapper->LocalOscFrequencyPointer()));
	response->Power = (*(status_packet_wrapper->LNBConfigurationPointer())) & 0x01 ; // The Power settings in the LNB configuraton on/off 
	response->Polarity = ((*(status_packet_wrapper->LNBConfigurationPointer()))&0x02) >>1; // The Polarity setting in LNB configuration H/V .
	response->Band =  ((*(status_packet_wrapper->LNBConfigurationPointer())) &0x04) >> 2; // The band settings in the LNB configuration Low/ Hi.
	response->Long_Line_Comp = ((*(status_packet_wrapper->LNBConfigurationPointer())) &0x10) >> 4;  // is Long Line Compa ON .
	response->Hi_VoltageMode = ((*(status_packet_wrapper->LNBConfigurationPointer())) &0x8) >> 3;  // is High Voltage On. 
	response->FirmWareVer = ((*(status_packet_wrapper->DSPFirmWarePointer())) & 0x00f0) >> 4; 
	response->FirmWareRev =	(*(status_packet_wrapper->DSPFirmWarePointer())) & 0xf;
	response->RF_FIRMWARE =	(*(status_packet_wrapper->MicroFirmWarePointer()));
	response->IGMP_Enable = (unsigned char)((*(status_packet_wrapper->FilterPointer())));

	// Copy the Ethernet Statistics...
	response->EthernetPacketDropped	= (*(status_packet_wrapper->EthernetDroppedPointer()));
	response->EthernetReceive		= (*(status_packet_wrapper->EthernetRXPointer()));
	response->EthernetTransmit		= (*(status_packet_wrapper->EthernetTXPointer()));
	response->EthernetTransmitError	= (*(status_packet_wrapper->EthernetTXErrPointer()));
	response->EthernetReceiveError	= (*(status_packet_wrapper->EthernetRXErrPointer()));
	response->FrontEndLockLoss		= (*(status_packet_wrapper->FEL_LossPointer()));
	response->DataSyncLoss			= (*(status_packet_wrapper->Sync_LossPointer()));
	response->DVBAccepted			= (*(status_packet_wrapper->TotalDVBAcceptedPointer()));


	switch ( *(status_packet_wrapper->ViterbiRatePointer()) ) {	// Convert the Viterbi Rate to the enumerated type...

    	case 0  : response->ViterbiRate	=	HALF;			break;
		case 1  : response->ViterbiRate	=	TWO_THIRDS;		break;
		case 2  : response->ViterbiRate	=	THREE_QUARTERS; break;
		case 4  : response->ViterbiRate	=	FIVE_SIXTHS;	break;
		case 6  : response->ViterbiRate	=	SEVEN_EIGTHS;	break;
		default : response->ViterbiRate	=	VITERBI_ERROR;	break;

	}

    response->AutomaticGainControl = (*(status_packet_wrapper->SignalStrengthPointer()));

	response->DemodulatorGain = (*(status_packet_wrapper->DemodulatorGainPointer()));

	// Convert demodulator status flags into something more readable

	response->SignalLock  = (*(status_packet_wrapper->DemodulatorStatusPointer())) & 0x01 ;

	response->DataLock = ((*(status_packet_wrapper->DemodulatorStatusPointer())) & 0x02) >> 1;

	response->LNBFault = ((*(status_packet_wrapper->DemodulatorStatusPointer())) & 0x04) >> 2;


	// Convert the BER to a floating point
							
	BERmantissa = ((status_packet_wrapper->VBER_ManPointer())[2]) |
	   		      ((status_packet_wrapper->VBER_ManPointer())[1] * 0x100) |
                  ((status_packet_wrapper->VBER_ManPointer())[0] * 0x10000);
            
	BERexponent = (*(status_packet_wrapper->VBER_ExpPointer()));

	response->VBER = (float)((double)BERmantissa / pow(10.0, (double)BERexponent));	

	if ( response->ViterbiRate == THREE_QUARTERS ) {
		if ( response->VBER < 1E-8 ) {
			response->CarrierToNoise = 50.0;
		} else
			if ( response->VBER > 1E-2 ) {
				response->CarrierToNoise = 0.0;
			} else {
				response->CarrierToNoise = (float)(4.0 - 0.625 * log10( response->VBER ));
			}
	} else {
		response->CarrierToNoise = 100.0;
	}

	response->Uncorrectables = (*(status_packet_wrapper->UncorrectablesPointer()));

	// Copy the PID list

	for ( j = 0; j < MAX_SUPP_PIDS; j++ ) {

		response->PIDList.Entry[j] = (status_packet_wrapper->PIDListPointer())[j] & 0x1fff;
		response->PIDList.Control[j] = ((status_packet_wrapper->PIDListPointer())[j] & 0x2000) >> 13 ;	
		
	}
		
	TrimPIDList((LPBYTE)&(response->PIDList), MAX_SUPP_PIDS);

	response->TotalDVBPacketsAccepted[0] 	= (status_packet_wrapper->TotalDVBPacketsAcceptedPointer())[0];
	response->TotalDVBPacketsAccepted[1] 	= (status_packet_wrapper->TotalDVBPacketsAcceptedPointer())[1];
	response->TotalDVBPacketsRXInError[0] 	= (status_packet_wrapper->TotalDVBPacketsRXInErrorPointer())[0];
	response->TotalDVBPacketsRXInError[1] 	= (status_packet_wrapper->TotalDVBPacketsRXInErrorPointer())[1];
	response->TotalEthernetPacketsOut[0] 	= (status_packet_wrapper->TotalEthernetPacketsOutPointer())[0];
	response->TotalEthernetPacketsOut[1] 	= (status_packet_wrapper->TotalEthernetPacketsOutPointer())[1];
	response->TotalUncorrectableTSPackets[0] 	= (status_packet_wrapper->TotalUncorrectableTSPacketsPointer())[0];
	response->TotalUncorrectableTSPackets[1] 	= (status_packet_wrapper->TotalUncorrectableTSPacketsPointer())[1];

}


int CS75_JLEO::PollStatus(void *Container, int timeout )
{
	int last_error_code ;
	S75JLEO_Status_MSG status_msg;
	S75JLEOStatusPacket status_packet_wrapper( &status_msg );
	S75_Status_JLEO *response = NULL;


	response = (S75_Status_JLEO *)Container;

	last_error_code = PollStatus( &status_packet_wrapper, timeout );

	if ( last_error_code == ERR_SUCCESS ) {

		TranscodeStatus( response, &status_packet_wrapper );

	}

	return last_error_code;

}


int CS75_JLEO::PollStatus( StatusPacket *status_packet_wrapper, int timeout )
{
	S75JLEO_One_Time_Status_Request_MSG message;
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
	/* S75_Status_JLEO *response = NULL; */
//	int     BERmantissa, BERexponent;   // Bit Error Rate mantissa and exponent

	S75JLEOStatusPacket *s75jleo_status_packet_wrapper = (S75JLEOStatusPacket *)status_packet_wrapper;


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

                if ( send( s, (const char *)(&message), sizeof( S75JLEO_One_Time_Status_Request_MSG), 0 ) != sizeof( S75JLEO_One_Time_Status_Request_MSG ) ) {
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
    		    	        if( (temp1 = recv( s, (char *)(status_packet_wrapper->data()), sizeof(S75JLEO_Status_MSG), 0 ) ) == SOCKET_ERROR ) {

	            	    		temp1 = GetLastError();

	                	        if(temp1 != WSAETIMEDOUT){

	        	        			last_error_code = ERR_STATUS_POLL;

					} // if


#else
    		    	        if( (temp1 = recv( s, (char *)(status_packet_wrapper->data()), sizeof(S75JLEO_Status_MSG), 0 ) ) == -1 ) {
					last_error_code = ERR_STATUS_POLL;
#endif

				} else {

		    	    // Make sure we have received the right number of bytes back

	    		    if (temp1 == sizeof(S75JLEO_Status_MSG)) {

						success = true;

						if(m_MACAddressValid){
							if(memcmp(status_packet_wrapper->ReceiverMACPointer(), m_Device_MAC_Address,6)!=0) {

								last_error_code = ERR_WRONG_RECEIVER;
								success = false;
							}
						}

						if((ntohs((*(status_packet_wrapper->DSPFirmWarePointer()))) & 0xFF00) != 0x5500 )	{		// Communication rev. JSAT Leo
							success = false;
							last_error_code = ERR_WRONG_RECEIVER;		/// if the device does not follow communication rev JSAT Leo it is not supported.
						}

						if(m_Device_Type != S75_JLEO) {
							success = false;
							last_error_code = ERR_DEVICE_TYPE_MISMATCH;	

						}
						
						if ( success ) {

							s75jleo_status_packet_wrapper->ntoh();

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


#ifdef BROADCAST_POLL_FOR_STATUS
bool CS75_JLEO::PollForDeviceOnLocalAdapter( char *local_address )
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
//	Novra_DeviceEntry device_info;
	time_t start_time;
	bool timed_out = false;
//	int       device_index;       // Index into the devices array
	bool      device_found=false;       // Flag to indicate if a device can be found in the list
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

					while ( !timed_out ) {

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

											if ( memcmp( m_Device_MAC_Address, status->ReceiverMAC, 6) == 0 ) {

												device_found = true;

											}

										}

									} // if

								} // if recv

							} else {

								timed_out = true;	

							}

						} // if select

						if ( ( time( NULL ) - start_time ) > 5 ) {

							timed_out = true;

						}

					} // while

				} // if send

			} // if bind
	
		} // if socket connect


		if ( device_found ) {

			m_broadcast_receive_socket = receive_socket;

		} else {

			#ifndef LINUX
	       		closesocket( receive_socket );
			#else
		   		close( receive_socket );
			#endif

		}

	} // if receive_socket create

	if ( device_found ) {

		m_broadcast_send_socket = send_socket;

	} else {

		#ifndef LINUX
       			closesocket( send_socket );
		#else
			close( send_socket );
		#endif
	}

	} // if send_socket create

	return device_found;

}
#endif


#ifdef BROADCAST_POLL_FOR_STATUS
bool CS75_JLEO::CreateBroadcastLink()
{
	int result;
	bool link_established=false;

#ifndef LINUX

	WSADATA	  wsa_data;           // Results of Winsock Startup Request

	const int winsock_version=2;  // Request Winsock 2.0
#endif

#ifndef LINUX
	PIP_ADAPTER_INFO	adapter_info;           // Pointer to an adapter info structure
	PIP_ADAPTER_INFO	adapter_buffer;         // Pointer to a network adapter information buffer
	PIP_ADDR_STRING		ip_address;             // A pointer to an adapter IP address
#endif

	ULONG			buf_len=10000;          // Amount of memory allocated for the adapter buffer
	BYTE			*end_of_buffer;         // A pointer to the last byte in the adapter buffer


#ifndef LINUX
	WSAStartup( winsock_version, &wsa_data );
#endif

#ifndef LINUX

	result = findAdapters( &adapter_buffer, &buf_len );

	if ( result == ERROR_SUCCESS ) {

		// Adapter info has been acquired

		// Set pointer to first adapter in the list

        	adapter_info = adapter_buffer;

		// Set a pointer to the last byte in the buffer. Don't search past this pointer.

        	end_of_buffer = (BYTE *)adapter_buffer + buf_len;

        	// Loop through the adapter list until linkage is established or the end of the
		// adapter buffer has been reached

        	while ( (!link_established) && ( adapter_info != NULL ) && ( (BYTE *)adapter_info < end_of_buffer ) ) {

			// Set pointer to first IP address of this adapter

			ip_address = &(adapter_info->IpAddressList);

			link_established = PollForDeviceOnLocalAdapter( ip_address->IpAddress.String );

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

		for ( int i = 0; (!link_established) && ( i < if_count ); i++ ) {

			ifr = adapters.ifc_ifcu.ifcu_req + i ;
			if(strcmp(ifr->ifr_name, "lo") != 0){

				link_established = PollForDeviceOnLocalAdapter( inet_ntoa( ((struct sockaddr_in *)(&(ifr->ifr_ifru.ifru_addr)))->sin_addr ) );

if ( link_established ) {
printf( "Linked on %s\n", ifr->ifr_name );
}

			}
		}

	}

	free( adapters.ifc_ifcu.ifcu_buf );


#endif
if ( !link_established ) {
printf( "NO LINK \n" );
}

	return link_established;

}
#endif


#ifdef BROADCAST_POLL_FOR_STATUS
int CS75_JLEO::BroadcastPollStatus( StatusPacket *status_packet_wrapper )
{
	char message[2];
	int use_again=1;
	int use_broadcast=1;
	struct sockaddr_in device_address;
	struct sockaddr_in receive_address;
#ifndef LINUX
	typedef int socklen_t;
#endif
	socklen_t len = sizeof( receive_address );
	struct timeval timeout_struct;
	fd_set readfs;
	int result;
	int temp1 = 1000;               // Temporary storage variable
	time_t start_time;
	bool timed_out = false;
	bool device_found = false;
	bool success = false;
	int last_error_code ;


	if(!m_MACAddressValid){
		return ERR_STATUS_POLL;
	}

	if ( (!m_broadcast_send_socket) && (!m_broadcast_receive_socket) ) {

		// Need to setup broadcast linkage to the receiver

		#ifndef LINUX

			WSADATA	  wsa_data;           // Results of Winsock Startup Request

			const int winsock_version=2;  // Request Winsock 2.0

			WSAStartup( winsock_version, &wsa_data );

		#endif

		#ifndef LINUX
			if ( ( m_broadcast_send_socket = socket( AF_INET, SOCK_DGRAM, 17 ) ) != INVALID_SOCKET ) {
			if ( ( m_broadcast_receive_socket = socket( AF_INET, SOCK_DGRAM, 17 ) ) != INVALID_SOCKET ) {
		#else
			if ( ( m_broadcast_send_socket = socket( AF_INET, SOCK_DGRAM, 17 ) ) != -1 ) {
			if ( ( m_broadcast_receive_socket = socket( AF_INET, SOCK_DGRAM, 17 ) ) != -1 ) {
		#endif

				setsockopt( m_broadcast_send_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)(&use_again), sizeof( use_again ) );
				setsockopt( m_broadcast_send_socket, SOL_SOCKET, SO_BROADCAST, (const char *)(&use_broadcast), sizeof( use_broadcast ) );
				setsockopt( m_broadcast_receive_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)(&use_again), sizeof( use_again ) );

		   		device_address.sin_family = AF_INET;

   				device_address.sin_addr.s_addr = inet_addr( "255.255.255.255" );

		   		device_address.sin_port = htons( 0x1963 );


		   		if ( connect( m_broadcast_send_socket, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr ) ) == 0){

					getsockname( m_broadcast_send_socket, (struct sockaddr *)(&receive_address), (socklen_t *)( &len) );
					receive_address.sin_addr.s_addr = INADDR_ANY;

					if ( bind ( m_broadcast_receive_socket, (struct sockaddr *)(&receive_address), sizeof( struct sockaddr ) ) == 0 ) {

						m_broadcast_linked = true;

					}
				}
			}
			}

	}

	if ( m_broadcast_linked ) {

		message[0] = 0x19;
		message[1] = 0x61;

		if ( send( m_broadcast_send_socket, (const char *)(&message), sizeof(message), 0 ) == sizeof( message ) ) {

			start_time = time( NULL );

			while ( ( !timed_out ) && ( !device_found ) ) {

	           	timeout_struct.tv_sec = 1;
	
		       	timeout_struct.tv_usec = 0;

				FD_ZERO( &readfs );

 			    FD_SET( m_broadcast_receive_socket, &readfs );

           		// Try to receive the responses from S75s.  Check for errors.
#ifndef LINUX
       			if ( ( result = select( 1, &readfs, NULL, NULL, &timeout_struct ) ) != SOCKET_ERROR ) {
#else
       			if ( ( result = select( m_broadcast_receive_socket+1, &readfs, NULL, NULL, &timeout_struct ) ) != -1 ) {
#endif
   	        		if ( result != 0 ) {

#ifndef LINUX
   	        			if( (temp1 = recv( m_broadcast_receive_socket, (char *)(status_packet_wrapper->data()), status_packet_wrapper->size(), 0 ) ) != SOCKET_ERROR ) {

#else
   	        			if( (temp1 = recv( m_broadcast_receive_socket, (char *)(status_packet_wrapper->data()), status_packet_wrapper->size(), 0 ) ) != -1 ) {
#endif
	
    						// Make sure we have received the right number of bytes back

	   	        			if ( temp1 == sizeof(S75JLEO_Status_MSG) ) {

								if( memcmp( status_packet_wrapper->ReceiverMACPointer(), m_Device_MAC_Address, 6 ) == 0 ) {

									device_found = true;

									success = true;

									if((ntohs((*(status_packet_wrapper->DSPFirmWarePointer()))) & 0xFF00) != 0x5500 )	{		// Communication rev. JSAT Leo
										success = false;
										last_error_code = ERR_WRONG_RECEIVER;		/// if the device does not follow communication rev JSAT Leo it is not supported.
									}

									if(m_Device_Type != S75_JLEO) {
										success = false;
										last_error_code = ERR_DEVICE_TYPE_MISMATCH;	
									}
						
									if ( success ) {

										status_packet_wrapper->ntoh();

										last_error_code = ERR_SUCCESS;

									}

								} else {

               						 last_error_code = ERR_STATUS_POLL_ACK_BAD;

								}

							} // if

						} // if recv

					} else {

						timed_out = true;	

					}

				} // if select

				if ( ( time( NULL ) - start_time ) > 5 ) {

					timed_out = true;

				}

			} // while

		} // if send

	}

	return last_error_code;
}

	
int CS75_JLEO::BroadcastPollStatus(void *Container )
{
	S75JLEO_Status_MSG status_msg;
	S75JLEOStatusPacket status_packet_wrapper( &status_msg );
#ifndef LINUX
//	typedef int socklen_t;
#endif
	int last_error_code ;
	S75_Status_JLEO *response = NULL;


	response = (S75_Status_JLEO *)Container;

	last_error_code = BroadcastPollStatus( &status_packet_wrapper );

	if ( last_error_code == ERR_SUCCESS ) {

		TranscodeStatus( response, &status_packet_wrapper );

	}

	return last_error_code;

}
#endif


int CS75_JLEO::GetPIDTable(LPBYTE PIDTable)
{
//	WORD opcode;                    // Opcode to send to S75
	int temp1 = 1000;               // Temporary storage variable
	// int count = 0 ;               Temporary storage variable
	// DWORD xx = 1;                  Temporary storage variable
	SOCKET s;
	struct sockaddr_in device_address;
	fd_set readfs;
	struct timeval timeout;
	int result;
	bool success=false;
    BYTE Buffer2[sizeof(S75JLEO_PIDResponse_MSG)];
	int i;
	S75JLEO_PIDResponse_MSG  PidResponseMSG;
	int last_error_code ;
	TCHAR temp[16];
	S75JLEO_PIDRequest_MSG pidRequest;

	m_pS75JLEO_PIDList = (S75_PIDList_JLEO *) PIDTable ;

	  // Convert the IP address to a string.
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);

	// Set the opcode to send to the S75 to download (0x0001)

	pidRequest.Opcode = htons(0x0001);
	srand( time(NULL) );
	pidRequest.Random = rand();

//	memset( (void *)(&m_sec), 0, sizeof( m_sec ) );

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	digitalsignature( &m_sec, (uint8_t *)(&pidRequest), sizeof( S75JLEO_PIDRequest_MSG ) - 20 );

	memcpy( pidRequest.Signature, m_sec.digest, 20 );



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

			
            // Download the PID table.  Retry (if required) up to 4 times

            for (i = 0; (!success) && ( i < NUM_RETRIES ); i++) {

            	// Send the PID table to the S75

//                if ( send( s, (const char *)(&opcode), sizeof( WORD), 0 ) != sizeof( WORD ) ) {
                if ( send( s, (const char *)(&pidRequest), sizeof( S75JLEO_PIDRequest_MSG), 0 ) != sizeof( S75JLEO_PIDRequest_MSG ) ) {

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
    		    	        if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75JLEO_PIDSettings_MSG), 0 ) ) == SOCKET_ERROR ) {

	            	    		temp1 = GetLastError();

	                	        if(temp1 != WSAETIMEDOUT){

	        	        			last_error_code = ERR_PID_RECEIVE;

								} // if


#else
    		    	        if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75JLEO_PIDResponse_MSG), 0 ) ) == -1 ) {
					last_error_code = ERR_PID_RECEIVE;
#endif

							} else {

		    	    	        // Make sure we have received the right number of bytes back

	    		    	        if (temp1 == sizeof(S75JLEO_PIDResponse_MSG)) {

	            		    		memcpy(&PidResponseMSG, Buffer2, sizeof(S75JLEO_PIDResponse_MSG));

      		        		        // Go through the PID table to upload and change all the PIDs and
    				                // UDP ports from network order to host order.
					                for ( i = 0; i < MAX_SUPP_PIDS; i++) {
   						                PidResponseMSG.PIDTableEntry[i] = ntohs(PidResponseMSG.PIDTableEntry[i]);
   						             // @@NS not needed for V2   PidSettingMSG.PIDTableEntry[i].DestinationUDP = ntohs(m_PIDTable.PIDTableEntry[i].DestinationUDP);
									}

                                    for (i = 0; i < MAX_SUPP_PIDS; i++) {

                                        m_pS75JLEO_PIDList->Entry[i] = PidResponseMSG.PIDTableEntry[i] & 0x1FFF;
										m_pS75JLEO_PIDList->Control[i] = ((PidResponseMSG.PIDTableEntry[i] & 0x2000) > 1)? 1:0 ;
									}
									m_pS75JLEO_PIDList->NumberofPIDs = MAX_SUPP_PIDS ;
									TrimPIDList((LPBYTE)  m_pS75JLEO_PIDList, MAX_SUPP_PIDS) ;

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



		} // if socket connect

#ifndef LINUX
        closesocket( s );
#else
        close( s );
#endif

	} // if socket create

	return last_error_code;
}


int CS75_JLEO::GetStatus(void *Container)
{
	WORD *PIDList = (WORD *)(m_StatusWrapper->PIDListPointer());

#ifdef BROADCAST_POLL_FOR_STATUS
	return BroadcastPollStatus( Container );
#else
//  This code will not work with non-chatty devices, use PollStatus instead

	S75_Status_JLEO * pStatus;
	//double  slope = 0.0;                Value used in converting signal strength
	//double  offset = 0.0;               Values used in converting signal strength
	//double  ss = 0.0;                   Values used in converting signal strength
	int     BERmantissa, BERexponent;   // Bit Error Rate mantissa and exponent


	pStatus = (S75_Status_JLEO *) Container;

	if(!IsStatusValid())  return ERR_FAILED ; // the status isn't valid. 

	if(m_Device_Type != S75_JLEO) return ERR_DEVICE_TYPE_MISMATCH ;

	//@@@NTL CSatelliteDevice::GetStatus( (SatelliteStatus *)(&(S75JLEOStatus( Container ))), false, MAX_SUPP_PIDS );
        S75JLEOStatus tmpJLEOStatus(Container) ;
	CSatelliteDevice::GetStatus( (SatelliteStatus *)(&tmpJLEOStatus), false, MAX_SUPP_PIDS );

	memcpy(pStatus->DVB_MACADDRESS, ((S75JLEOStatusPacket *)m_StatusWrapper)->DVBMACPointer(),6);
	memcpy(pStatus->Reserved, ((S75JLEOStatusPacket *)m_StatusWrapper)->ReservedPointer(),8);

	pStatus->BroadcastStatusPort = (*(((S75JLEOStatusPacket *)m_StatusWrapper)->BroadcastStatusPortPointer()));
	// If the RF valid == 0x0400 then RFStatusvalid =1 (easier to read for external programmers...:P)
	pStatus->LocalOscFrequency	= (*(((S75JLEOStatusPacket *)m_StatusWrapper)->LocalOscFrequencyPointer()));
	pStatus->Long_Line_Comp = ((*(((SatelliteStatusPacket *)m_StatusWrapper)->LNBConfigurationPointer())) &0x10) >> 4;  // is Long Line Compa ON .
	pStatus->Hi_VoltageMode = ((*(((SatelliteStatusPacket *)m_StatusWrapper)->LNBConfigurationPointer())) &0x8) >> 3;  // is High Voltage On. 
	pStatus->IGMP_Enable = (unsigned char)(*(((S75JLEOStatusPacket *)m_StatusWrapper)->FilterPointer()) );

	
	pStatus->AutomaticGainControl = (*(((SatelliteStatusPacket *)m_StatusWrapper)->SignalStrengthPointer()));

	pStatus->DemodulatorGain = (*(((SatelliteStatusPacket *)m_StatusWrapper)->DemodulatorGainPointer()));


	// Convert the BER to a floating point

    BERmantissa = ((((SatelliteStatusPacket *)m_StatusWrapper)->VBER_ManPointer())[2]) |
                  ((((SatelliteStatusPacket *)m_StatusWrapper)->VBER_ManPointer())[1] * 0x100) |
                  ((((SatelliteStatusPacket *)m_StatusWrapper)->VBER_ManPointer())[0] * 0x10000);
            
    BERexponent = (*(((SatelliteStatusPacket *)m_StatusWrapper)->VBER_ExpPointer()));

    pStatus->VBER = (float)((double)BERmantissa / pow(10.0, (double)BERexponent));
	
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

	pStatus->EthernetTransmitError	= (*(((S75JLEOStatusPacket *)m_StatusWrapper)->EthernetTXErrPointer()));
	pStatus->FrontEndLockLoss	= (*(((S75JLEOStatusPacket *)m_StatusWrapper)->FEL_LossPointer()));

	pStatus->TotalDVBPacketsAccepted[0] 	= (((S75JLEOStatusPacket *)m_StatusWrapper)->TotalDVBPacketsAcceptedPointer())[0];
	pStatus->TotalDVBPacketsAccepted[1] 	= (((S75JLEOStatusPacket *)m_StatusWrapper)->TotalDVBPacketsAcceptedPointer())[1];
	pStatus->TotalDVBPacketsRXInError[0] 	= (((S75JLEOStatusPacket *)m_StatusWrapper)->TotalDVBPacketsRXInErrorPointer())[0];
	pStatus->TotalDVBPacketsRXInError[1] 	= (((S75JLEOStatusPacket *)m_StatusWrapper)->TotalDVBPacketsRXInErrorPointer())[1];
	pStatus->TotalEthernetPacketsOut[0] 	= (((S75JLEOStatusPacket *)m_StatusWrapper)->TotalEthernetPacketsOutPointer())[0];
	pStatus->TotalEthernetPacketsOut[1] 	= (((S75JLEOStatusPacket *)m_StatusWrapper)->TotalEthernetPacketsOutPointer())[1];
	pStatus->TotalUncorrectableTSPackets[0] = (((S75JLEOStatusPacket *)m_StatusWrapper)->TotalUncorrectableTSPacketsPointer())[0];
	pStatus->TotalUncorrectableTSPackets[1] = (((S75JLEOStatusPacket *)m_StatusWrapper)->TotalUncorrectableTSPacketsPointer())[1];

	// Copy the PID list

	for ( int i = 0; i < MAX_SUPP_PIDS; i++ ) {

		pStatus->PIDList.Entry[i] = PIDList[i] & 0x1fff;

		pStatus->PIDList.Control[i] = (PIDList[i] & 0x2000) >> 13 ;	

	}

	TrimPIDList((LPBYTE)(&(pStatus->PIDList)), MAX_SUPP_PIDS );

	return ERR_SUCCESS ;
#endif
}


bool CS75_JLEO::GetDefaultConfigStruct(void *pConfig)
{

	if((!m_Device_attached) ||(!m_IPAddressValid && !m_MACAddressValid)) return false;

	CNovraDevice::GetDefaultConfigStruct( pConfig );
  
  	((S75_NetSetting_JLEO *)pConfig)->UniCastStatusDestUDP =  m_Device_Status_Port;

	((S75_NetSetting_JLEO *)pConfig)->IGMP_Enable = 0;
    		
  	return true;	

}


int CS75_JLEO::ChangePassword( char password[8], unsigned char key[16] )
{
	S75JLEO_Password_Change_MSG password_message;
	TCHAR temp[16];
	bool configurationSent=false;             // New configuration sent to S75?
	int LastError = ERR_SUCCESS;
	int i;
	char old_password[8];


	sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);

//	memset( (void *)(&m_sec), 0, sizeof( m_sec ) );

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	memcpy( old_password, m_sec.password, 8 );
	memcpy( m_sec.password, password, 8 );

	encryptpassword( &m_sec, (uint8_t *)(&password_message), key );

	// Send to the S75.  If communication fails, retry up to four times.

	for (i = 0; i < NUM_RETRIES; i++) {

		configurationSent = (( LastError = SendWaitForConfirmation( temp, PASSWORD_CHANGE_PORT, (BYTE *)(&password_message), sizeof( S75JLEO_Password_Change_MSG ))) == ERR_SUCCESS );
		if (configurationSent) break;

	}

	if ( !configurationSent ) memcpy( m_sec.password, old_password, 8 );

	return LastError;

}


void CS75_JLEO::TranscodePIDList( PIDSettings_MSG *PidSettingMSG )
{
	
	for ( int i = 0; i < MAX_SUPP_PIDS; i++ ) {

   		((S75JLEO_PIDSettings_MSG *)PidSettingMSG)->PIDTableEntry[i] = ntohs(((S75JLEO_PIDSettings_MSG *)PidSettingMSG)->PIDTableEntry[i]);
		m_pS75JLEO_PIDList->Entry[i] = ((S75JLEO_PIDSettings_MSG *)PidSettingMSG)->PIDTableEntry[i] & 0x1FFF;
		m_pS75JLEO_PIDList->Control[i] = ((((S75JLEO_PIDSettings_MSG *)PidSettingMSG)->PIDTableEntry[i] & 0x2000) > 1)? 1:0 ;
									
	}

	m_pS75JLEO_PIDList->NumberofPIDs = MAX_SUPP_PIDS ;

	TrimPIDList((LPBYTE)  m_pS75JLEO_PIDList, MAX_SUPP_PIDS) ;

}


bool CS75_JLEO::ConfirmDeviceSpecificNetworkSettings( StatusPacket *status_wrapper, NetworkConfigPacket *network_config_packet_wrapper )
{

	bool configurationApplied = true;

    if (memcmp(network_config_packet_wrapper->ReceiverIPPointer(), ((S75JLEOStatusPacket *)m_StatusWrapper)->ReceiverIPPointer() , 4) != 0) { 
		configurationApplied = false;
cerr << endl << endl << "Receiver IP Pointer value is different" << endl << endl ;
   }
    
#ifndef BROADCAST_POLL_FOR_STATUS
	if ( *(network_config_packet_wrapper->StatusPortPointer()) != ntohs(*(((S75JLEOStatusPacket *)m_StatusWrapper)->BroadcastStatusPortPointer()))) { 
		configurationApplied = false; 
     }
#endif

	if ( ( (*(((S75JLEOStatusPacket *)m_StatusWrapper)->FilterPointer())) &
		   (*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer())) ) !=
		 (*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer())) ) {

		configurationApplied = false;
        }

//cerr << endl << endl << (int)ntohs(*(((S75JLEOStatusPacket *)m_StatusWrapper)->FilterPointer())) << endl ;
//cerr << endl << (int)(*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer())) << endl ;

	return configurationApplied;

}


#endif


