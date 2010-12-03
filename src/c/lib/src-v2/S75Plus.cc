// S75Plus.cpp: implementation of the CS75Plus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S75Plus.h"
#include "S75PlusStatus.h"
#include "ProgramList.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

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




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS75Plus::CS75Plus( bool remote )
{
	m_StatusPacketSize = sizeof(S75PlusStatus_MSG);
	m_Device_Type = S75PLUS;  // when we instantiate this class, the device type will be enforced on attachment.
	m_StatusWrapper = (StatusPacket *)(new S75PlusStatusPacket( &m_Status ));
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


CS75Plus::~CS75Plus()
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


int CS75Plus::ConfigureNetwork(void *pNetworkSetting)
{
	
	S75PlusNetConfig_MSG			net_setting_msg;
	S75PlusNetworkConfigPacket	network_config_packet_wrapper( &net_setting_msg );


    // There are two possible methods of communicating -- by IP address or
    // by MAC address.  For the former, make sure the IP address is set.
    // For the later, make sure the network path is set

    if ((!m_Device_attached) && (!m_NetPathSet))
    
		// Trying to communicate by IP with no network path -- error
        return ERR_NETCFG_NONETPATH;
    

    // Destination information properly set

	if ( !CreateNetConfigMsg( (NetworkConfigPacket *)(&network_config_packet_wrapper), (NetSetting *)pNetworkSetting, 4*(((PesMaker_NetSetting *) pNetworkSetting)->IGMP_Enable) ) )
		return ERR_BAD_IP_SUBNET_COMBO;

	// we should Check the Status Port to Make sure it is in the auto detection List.
	
	if ( !CheckStatusPort( ((NetSetting *)pNetworkSetting)->StatusPort ) )	// Broadcast Status Port
		return ERR_BAD_STATUS_PORT;

	if ( !CheckStatusPort( ((NetSetting *)pNetworkSetting)->DestUDP ) )		// Unicast Status Port
		return ERR_BAD_STATUS_PORT;

	return SendNetworkConfig( (NetworkConfigPacket *)(&network_config_packet_wrapper) );

}


bool CS75Plus::GetDefaultConfigStruct(void *pConfig)
{

	if((!m_Device_attached) ||(!m_IPAddressValid && !m_MACAddressValid)) return false;

	CNovraDevice::GetDefaultConfigStruct( pConfig );
 
  	((PesMaker_NetSetting *)pConfig)->UniCastStatusDestUDP =  m_Device_Status_Port;

	((PesMaker_NetSetting *)pConfig)->IGMP_Enable = 0;
    		
  	return true;	

}


void CS75Plus::TranscodePIDList( PIDSettings_MSG *PidSettingMSG )
{
	
	for ( int i = 0; i < MAX_SUPP_PIDS; i++ ) {

		PidSettingMSG->
			PIDTableEntry[i].PID = ntohs(PidSettingMSG->PIDTableEntry[i].PID);
		PidSettingMSG->PIDTableEntry[i].DestinationUDP = ntohs(PidSettingMSG->PIDTableEntry[i].DestinationUDP);
//		m_pS75D_PIDList->Entry[i] = PidSettingMSG->PIDTableEntry[i].PID & 0x1FFF;
		m_pS75Plus_PIDList->Control[i] = ((PidSettingMSG->PIDTableEntry[i].PID & 0xE000) >> 13) ;
//		memcpy( m_pS75D_PIDList->ForwardInformation[i].Destination_IP, PidSettingMSG->PIDTableEntry[i].DestinationIP, 4);
//		m_pS75D_PIDList->ForwardInformation[i].Destination_Port =  PidSettingMSG->PIDTableEntry[i].DestinationUDP ;
									
	}
									
//	m_pS75CA_PIDList->NumberofPIDs = MAX_SUPP_PIDS ;
									
	TrimPIDList((LPBYTE) m_pS75Plus_PIDList, MAX_SUPP_PIDS) ;

}


int CS75Plus::GetPIDTable(LPBYTE PIDTable)
{
//	WORD opcode;                    // Opcode to send to S75
	int temp1 = 1000;               // Temporary storage variable
	//int count = 0 ;                 Temporary storage variable
	// DWORD xx = 1;                   Temporary storage variable
	SOCKET s;
	struct sockaddr_in device_address;
	fd_set readfs;
	struct timeval timeout;
	int result;
	bool success=false;
    BYTE Buffer2[sizeof(S75PlusPIDSettings_MSG)];
	int i;
	S75PlusPIDSettings_MSG  PidResponseMSG;
	int last_error_code ;
	TCHAR temp[16];
	S75PlusPIDSettings_MSG pidRequest;

	m_pS75Plus_PIDList = (S75Plus_PIDList *) PIDTable ;

	  // Convert the IP address to a string.
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);

	memset( (void *)&PidResponseMSG, 0, sizeof( PidResponseMSG ) );
	memset( (void *)&pidRequest, 0, sizeof( pidRequest ) );

	// Set the opcode to send to the S75 to download (0x0001)

	pidRequest.Opcode = htons(0x0001);
	srand( time(NULL) );
	pidRequest.Random = rand();

//	memset( (void *)(&m_sec), 0, sizeof( m_sec ) );

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	digitalsignature( &m_sec, (uint8_t *)(&pidRequest), sizeof( S75PlusPIDSettings_MSG ) - 20 );

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
                if ( send( s, (const char *)(&pidRequest), sizeof( S75PlusPIDSettings_MSG), 0 ) != sizeof( S75PlusPIDSettings_MSG ) ) {

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
    		    	        if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75PlusPIDSettings_MSG), 0 ) ) == SOCKET_ERROR ) {

	            	    		temp1 = GetLastError();

	                	        if(temp1 != WSAETIMEDOUT){

	        	        			last_error_code = ERR_PID_RECEIVE;

								} // if


#else
    		    	        if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75PlusPIDSettings_MSG), 0 ) ) == -1 ) {
					last_error_code = ERR_PID_RECEIVE;
#endif

							} else {

		    	    	        // Make sure we have received the right number of bytes back

	    		    	        if (temp1 == sizeof(S75PlusPIDSettings_MSG)) {

	            		    		memcpy(&PidResponseMSG, Buffer2, sizeof(S75PlusPIDSettings_MSG));

      		        		        // Go through the PID table to upload and change all the PIDs and
    				                // UDP ports from network order to host order.
					                for ( i = 0; i < MAX_SUPP_PIDS; i++) {
   						                PidResponseMSG.PIDTableEntry[i].PID = ntohs(PidResponseMSG.PIDTableEntry[i].PID);
										PidResponseMSG.PIDTableEntry[i].DestinationMask = ntohs(PidResponseMSG.PIDTableEntry[i].DestinationMask);
   						             // @@NS not needed for V2   PidSettingMSG.PIDTableEntry[i].DestinationUDP = ntohs(m_PIDTable.PIDTableEntry[i].DestinationUDP);
									}
					                for ( i = 0; i < MAX_SUPP_DESTINATIONS; i++) {
   						                PidResponseMSG.PIDDestinations[i].DestinationUDP = ntohs(PidResponseMSG.PIDDestinations[i].DestinationUDP);
									}

                                    for (i = 0; i < MAX_SUPP_PIDS; i++) {

										m_pS75Plus_PIDList->PID[i] = PidResponseMSG.PIDTableEntry[i].PID & 0x1FFF;
//										m_pS75Plus_PIDList->Control[i] = ((PidResponseMSG.PIDTableEntry[i].PID & 0x2000) > 1)? 1:0 ;
										m_pS75Plus_PIDList->Control[i] = (PidResponseMSG.PIDTableEntry[i].PID & 0xE000) >> 13;


										m_pS75Plus_PIDList->DestinationMask[i] = PidResponseMSG.PIDTableEntry[i].DestinationMask;

									}
                                    for (i = 0; i < MAX_SUPP_DESTINATIONS; i++) {

										memcpy( m_pS75Plus_PIDList->Destination[i].DestinationIP, PidResponseMSG.PIDDestinations[i].DestinationIP, 4 );
										m_pS75Plus_PIDList->Destination[i].DestinationUDP = PidResponseMSG.PIDDestinations[i].DestinationUDP;

									}

//									m_pS75Plus_PIDList->NumberofPIDs = MAX_SUPP_PIDS ;
//									TrimPIDList((LPBYTE)  m_pS75Plus_PIDList, MAX_SUPP_PIDS) ;

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


bool CS75Plus::TrimPIDList(LPBYTE PL, int Max)
{
	S75D_PIDList *pL;

	int j=0,k=0,l=0,m=0;

	pL = (S75D_PIDList*) PL;

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


int CS75Plus::GetStatus(void *Container)
{
	S75Plus_Status * pStatus;
	int i;
	/* WORD temp ; */
	/* S75PlusStatus *pstatus; */

	pStatus = (S75Plus_Status *) Container;

	if(!IsStatusValid())  return ERR_FAILED ; // the status isn't valid. 

	if(m_Device_Type != S75PLUS) return ERR_DEVICE_TYPE_MISMATCH ;

	CSatelliteDevice::GetStatus( (SatelliteStatus *)(&(S75PlusStatus( Container ))), true );

	/* XXX
	pstatus = new S75PlusStatus(Container);
	CSatelliteDevice::GetStatus((SatelliteStatus*)pstatus, true);
	delete pstatus;
	*/

	/* XXX
	fprintf(stdout, "%#x\n", ((*(((S75PlusStatusPacket *)m_StatusWrapper)->DSPFirmWarePointer())) & 0xFF00) >> 8);
	*/

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

	pStatus->SignalLock = (*(((S75PlusStatusPacket *)m_StatusWrapper)->DemodulatorStatusPointer())) & 0x01 ;

	pStatus->DataLock = ((*(((S75PlusStatusPacket *)m_StatusWrapper)->DemodulatorStatusPointer())) & 0x10) >> 4;

	pStatus->LNBFault = ((*(((S75PlusStatusPacket *)m_StatusWrapper)->LNBConfigurationPointer())) & 0x80) >> 7;

	pStatus->DemodulatorGain = (*(((SatelliteStatusPacket *)m_StatusWrapper)->DemodulatorGainPointer()));

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


bool CS75Plus::ParseStatusPacket()
{
	BYTE temp[4];

	// If this function is called, the status Structure has been filled with data received by Status Listner.
	// We need to verify that the packet is valid, and that it matches the device to be valid.

	if ( !CNovraDevice::ParseStatusPacket() ) return false;

	((S75PlusStatusPacket *)m_StatusWrapper)->ntoh();

//	for(i=0;i<16;i++)
//		(((S75PlusStatusPacket *)m_StatusWrapper)->ProgramsPointer())[i] = ntohs((((S75PlusStatusPacket *)m_StatusWrapper)->ProgramsPointer())[i]);

//	for(int i=0;i<MAX_SUPP_PIDS;i++)
//		(((S75PlusStatusPacket *)m_StatusWrapper)->PIDListPointer())[i] = ntohs((((S75PlusStatusPacket *)m_StatusWrapper)->PIDListPointer())[i]);

//	if( (((*(((S75PlusStatusPacket *)m_StatusWrapper)->DSPFirmWarePointer())) & 0xFF00) != 0x3400) && (((*(((S75PlusStatusPacket *)m_StatusWrapper)->DSPFirmWarePointer())) & 0xFF00) != 0x5400))	// Both 2M and New Hardware 
	if( ((* ( ( (S75PlusStatusPacket *)m_StatusWrapper)->DSPFirmWarePointer()) ) & 0xFF00) != 0x6900 )
	 return false ;		/// if the device does not follow communication rev 3.0 it is not supported.

	if(m_Device_Type != S75PLUS) return false;

	m_StatusListener.GetLastPacketSource(temp);

	memcpy(m_Device_IP_Address, temp,4); // Get the address from the Unit.

	return true;
}



int CS75Plus::SetPIDTable(LPBYTE PIDTable)
{
	int i, j;
	SOCKET s;
	TCHAR  temp[16];
	struct sockaddr_in device_address;
	int last_error_code ;
	S75PlusPIDSettings_MSG PidSettingMsg;
	bool success = false;
	struct timeval timeout;
	fd_set readfs;
	int result;
	int temp1 = 1000;               // Temporary storage variable
//	S75PlusPIDSettings_MSG verification_pid_table;
	S75Plus_PIDList verification_pid_table;
	bool configurationApplied=false;          // New configuration applied to S75?
    bool PIDInList;                     // Is the PID we are looking at in the current list?
	bool pidFound;
	bool destinationFound;
	WORD mask;
	int k, m;
//	CommonPIDSettingsPacket pid_setting_packet_wrapper( &PidSettingMsg );
//	bool	ProgramList = false ;

	
	
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

			m_pS75Plus_PIDList = (S75Plus_PIDList *) PIDTable ;


			// Set the opcode in the PID table to Upload (0x0002)

		    PidSettingMsg.Opcode = htons(0x0002);

			for (i = 0; i < MAX_SUPP_PIDS; i++) {

				PidSettingMsg.PIDTableEntry[i].PID = ( m_pS75Plus_PIDList->Control[i] << 13 ) + m_pS75Plus_PIDList->PID[i];
				PidSettingMsg.PIDTableEntry[i].PID = htons( PidSettingMsg.PIDTableEntry[i].PID );
				PidSettingMsg.PIDTableEntry[i].DestinationMask =  htons( m_pS75Plus_PIDList->DestinationMask[i] );

		    } // for i

			for (i = 0; i < 16; i++) {
				memcpy( PidSettingMsg.PIDDestinations[i].DestinationIP, m_pS75Plus_PIDList->Destination[i].DestinationIP, 4 );
				PidSettingMsg.PIDDestinations[i].DestinationUDP = htons( m_pS75Plus_PIDList->Destination[i].DestinationUDP );
			}

			srand( time(NULL) );
			PidSettingMsg.Random = rand();

			for ( i = 0; i < 6; i++ ) {
				m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
			}

			digitalsignature( &m_sec, (uint8_t *)(&PidSettingMsg), sizeof( S75PlusPIDSettings_MSG ) - 20 );

	
			memcpy( PidSettingMsg.Signature, m_sec.digest, 20 );


            // Create a buffer to hold the data we wish to send

            BYTE Buffer1[sizeof(S75PlusPIDSettings_MSG)];
            memcpy(Buffer1, &PidSettingMsg, sizeof(S75PlusPIDSettings_MSG)); // copy the data in the buffer.
			 


            // Create a buffer the size of the PID Table to hold the
            // response we receive back.

			BYTE Buffer2[sizeof(S75PlusPIDSettings_MSG)];

            for (i = 0; (!success) && ( i < NUM_RETRIES ); i++) {

    			// Send the PID table to the S75

                if ( send( s, (const char *)Buffer1, sizeof( S75PlusPIDSettings_MSG), 0 ) != sizeof( S75PlusPIDSettings_MSG ) ) {

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

        		    	if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75PlusPIDSettings_MSG), 0 ) ) == SOCKET_ERROR ) {
	        		    	temp1 = GetLastError();
		            		if(temp1 != WSAETIMEDOUT){
			            		 last_error_code = ERR_PID_RECEIVE;
					} // if

#else
        		    	if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75PlusPIDSettings_MSG), 0 ) ) == -1 ) {
					 last_error_code = ERR_PID_RECEIVE;

#endif

						}  // if

    			        // Check to make sure the data received back is the same data sent.

    			        if(temp1 ==  sizeof(S75PlusPIDSettings_MSG)){

							success = true;

		       for(i = 0 ; i < (int)sizeof(S75PlusPIDSettings_MSG); i++ ){

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

			configurationApplied = true;

            // Go through the PID list downloaded and check to make sure that each PID we
            // uploaded is somewhere in that list.

			for (i = 0; (i < MAX_SUPP_PIDS) && configurationApplied; i++) {

	            if (m_pS75Plus_PIDList->PID[i] != NULL_PID) {     // Skip null PIDs

					pidFound = false;
					j = 0;

					while ( (!pidFound) && ( j < MAX_SUPP_PIDS ) && configurationApplied ) {

//						if ( (verification_pid_table.PIDTableEntry[j].PID & 0x1fff) == m_pS75Plus_PIDList->PID[i] ) {
						if ( (verification_pid_table.PID[j] & 0x1fff) == m_pS75Plus_PIDList->PID[i] ) {
							pidFound = true;
//							if ( ((verification_pid_table.PIDTableEntry[j].PID & 0xe000 ) >> 13 ) == m_pS75Plus_PIDList->Control[i] ) {
							if ( verification_pid_table.Control[j] == m_pS75Plus_PIDList->Control[i] ) {
								PIDInList = true;
								mask = 0x8000;
								for ( k = 0; (k < 16) && configurationApplied; k++ ) {
									if ( m_pS75Plus_PIDList->DestinationMask[i] & ( mask >> k ) ) {
										m = 0;
										destinationFound = false;
										while ( (!destinationFound) && ( m < 16 ) ) {
//											if ( verification_pid_table.PIDTableEntry[j].DestinationMask * (mask >> m) ) {
											if ( verification_pid_table.DestinationMask[j] * (mask >> m) ) {
//												if ( m_pS75Plus_PIDList->Destination[k].DestinationUDP == verification_pid_table.PIDDestinations[m].DestinationUDP ) {
												if ( m_pS75Plus_PIDList->Destination[k].DestinationUDP == verification_pid_table.Destination[m].DestinationUDP ) {
													if ( memcmp(	m_pS75Plus_PIDList->Destination[k].DestinationIP,
//														verification_pid_table.PIDDestinations[m].DestinationIP, 4 ) == 0 ) {
														verification_pid_table.Destination[m].DestinationIP, 4 ) == 0 ) {
														destinationFound = true;
													}
												}
											}
											if ( !destinationFound ) {
												m++;
											}
										}
										if ( !destinationFound ) configurationApplied = false;
									}
								}
							} else {
								configurationApplied = false;  // Wrong Control Code
							}
						} else {
							j++;
						}

			        }

					if ( !pidFound ) configurationApplied = false;
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


/*
int CS75Plus::GetProgramTable(LPBYTE ProgramTable)
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
    BYTE Buffer2[sizeof(S75PlusProgramResponse_MSG)];
	int i;
	S75PlusProgramResponse_MSG programResponseMSG;
	int last_error_code ;
	TCHAR temp[16];
	S75PlusProgramSettings_MSG programRequest;
//

	m_pS75Plus_ProgramList = (S75Plus_ProgramList *) ProgramTable ;

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

	digitalsignature( &m_sec, (uint8_t *)(&programRequest), sizeof( S75PlusProgramSettings_MSG ) - 20 );

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

                if ( send( s, (const char *)(&programRequest), sizeof( S75PlusProgramSettings_MSG), 0 ) != sizeof( S75PlusProgramSettings_MSG ) ) {

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
    		    	        if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75PlusProgramResponse_MSG), 0 ) ) == SOCKET_ERROR ) {

	            	    		temp1 = GetLastError();

	                	        if(temp1 != WSAETIMEDOUT){

	        	        			last_error_code = ERR_PROGRAM_RECEIVE;

								} // if


#else
    		    	        if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75PlusProgramResponse_MSG), 0 ) ) == -1 ) {
					last_error_code = ERR_PROGRAM_RECEIVE;
#endif

							} else {

		    	    	        // Make sure we have received the right number of bytes back

	    		    	        if (temp1 == sizeof(S75PlusProgramResponse_MSG)) {

	            		    		memcpy(&programResponseMSG, Buffer2, sizeof(S75PlusProgramResponse_MSG));

									// Go through the Program table and change all the Programs and
    				                // status words from network order to host order.
					                for ( i = 0; i < MAX_SUPP_PROGRAMS; i++) {
   						                m_pS75Plus_ProgramList->ProgramNumber[i] = ntohs(programResponseMSG.Program[i]);
										m_pS75Plus_ProgramList->ProgramStatus[i] = ntohs(programResponseMSG.Status[i]);
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
*/


/*
int CS75Plus::SetProgramTable(LPBYTE ProgramTable)
{
	int i;

	int last_error_code ;

	S75PlusProgramSettings_MSG ProgramSettingMsg;

	bool	ProgramList = false ;

	m_pS75Plus_ProgramList = (S75Plus_ProgramList *) ProgramTable ;
	m_ProgramList = new Program_List( (void *)ProgramTable );


    // Set the opcode in the Program table to Upload (0x0002)

    ProgramSettingMsg.Opcode = htons(0x0002);

    // Copy the Program table to download into the CPIDProgram object
	// Does The Program list have too many entries !!? 

//	if(m_pS75D_ProgramList->NumberofPrograms > MAX_SUPP_PROGRAMS) return ERR_PROGRAM_TOOLONG ;

    for (i = 0; i < MAX_SUPP_PROGRAMS; i++) {

		ProgramSettingMsg.Program[i] = htons( m_pS75Plus_ProgramList->ProgramNumber[i] );

    } // for i

	srand( time(NULL) );
	ProgramSettingMsg.Random = rand();

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	digitalsignature( &m_sec, (uint8_t *)(&ProgramSettingMsg), sizeof( ProgramSettingMsg ) - 20 );

	
	memcpy( ProgramSettingMsg.Signature, m_sec.digest, 20 );

	last_error_code = SendPrograms( &ProgramSettingMsg, sizeof( S75PlusProgramSettings_MSG ), MAX_SUPP_PROGRAMS );

	delete m_ProgramList;

    return last_error_code;

}
*/


/*
int CS75Plus::SetPAT(LPBYTE PAT)
{
	int i;

	int last_error_code ;

	S75PlusPAT_MSG PATMsg;

//	bool ProgramList = false ;

	m_pS75Plus_PAT = (S75PlusPro_PAT *) PAT ;
//	m_PAT = new s75Plus_PAT( (void *)PAT );


    // Set the opcode in the PAT Msg to Upload (0x0002)

    PATMsg.Opcode = htons(0x0002);

    for (i = 0; i < 16; i++) {

		PATMsg.PATTableEntry[i].ProgramNumber = htons( m_pS75Plus_PAT->ProgramNumber[i] );
		PATMsg.PATTableEntry[i].PMTPID = htons( m_pS75Plus_PAT->PMTPID[i] );

    } // for i

	srand( time(NULL) );
	PATMsg.Random = rand();

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	digitalsignature( &m_sec, (uint8_t *)(&PATMsg), sizeof( PATMsg ) - 20 );

	
	memcpy( PATMsg.Signature, m_sec.digest, 20 );

	last_error_code = SendPAT( &PATMsg, sizeof( S75PlusPAT_MSG ), 16 );

//	delete m_PAT;

    return last_error_code;

}
*/


/*
int CS75Plus::GetPAT(LPBYTE PAT)
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
    BYTE Buffer2[sizeof(S75PlusPAT_MSG)];
	int i;
	int last_error_code ;
	TCHAR temp[16];
	S75PlusPAT_MSG PATResponseMsg;
	S75PlusPAT_MSG PATRequest;
//

	m_pS75Plus_PAT = (S75PlusPro_PAT *) PAT ;

	  // Convert the IP address to a string.
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);

	memset( (void *)&PATResponseMsg, 0, sizeof( PATResponseMsg ) );
	memset( (void *)&PATRequest, 0, sizeof( PATRequest ) );

	// Set the opcode to send to the S75 to download (0x0001)

	PATRequest.Opcode = htons(0x0001);
	srand( time(NULL) );
	PATRequest.Random = rand();

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	digitalsignature( &m_sec, (uint8_t *)(&PATRequest), sizeof( S75PlusPAT_MSG ) - 20 );

	memcpy( PATRequest.Signature, m_sec.digest, 20 );


	// Create a socket to communicate with the S75

#ifndef LINUX
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) {
#else
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) {
#endif


        last_error_code = ERR_PAT_SOCKET_CREATE;

	} else {

        // Connect the newly created socket to the S75.

    	device_address.sin_family = AF_INET;

    	device_address.sin_port = htons( PAT_CONFIG_PORT );

    	device_address.sin_addr.s_addr = inet_addr( temp);

    	if ( connect( s, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr_in ) ) != 0){

    		last_error_code = ERR_PAT_SOCKET_CONNECT;

		} else {

            // Create a buffer the size of the PAT to hold the
            // response we receive back.

			
            // Download the PAT.  Retry (if required) up to 4 times

            for (i = 0; (!success) && ( i < NUM_RETRIES ); i++) {

            	// Send the PAT reqest to the S75

                if ( send( s, (const char *)(&PATRequest), sizeof( S75PlusPAT_MSG), 0 ) != sizeof( S75PlusPAT_MSG ) ) {

                    last_error_code = ERR_PAT_SEND;

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
    		    	        if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75PlusPAT_MSG), 0 ) ) == SOCKET_ERROR ) {

	            	    		temp1 = GetLastError();

	                	        if(temp1 != WSAETIMEDOUT){

	        	        			last_error_code = ERR_PAT_RECEIVE;

								} // if


#else
    		    	        if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75PlusPAT_MSG), 0 ) ) == -1 ) {
					last_error_code = ERR_PAT_RECEIVE;
#endif

							} else {

		    	    	        // Make sure we have received the right number of bytes back

	    		    	        if (temp1 == sizeof(S75PlusPAT_MSG)) {

	            		    		memcpy(&PATResponseMsg, Buffer2, sizeof(S75PlusPAT_MSG));

									// Go through the PAT and change all the Programs and
    				                // PMT PIDs from network order to host order.
					                for ( i = 0; i < 16; i++) {
   						                m_pS75Plus_PAT->ProgramNumber[i] = ntohs(PATResponseMsg.PATTableEntry[i].ProgramNumber);
										m_pS75Plus_PAT->PMTPID[i] = ntohs(PATResponseMsg.PATTableEntry[i].PMTPID);
									}

					                last_error_code = ERR_SUCCESS;

									success = true;

								} else {
                                    last_error_code = ERR_PAT_ACK_BAD;

								} // if

							} // if recv

						} else {

							last_error_code = ERR_PAT_RECEIVE;

						} // if select result 0

					} else {

						last_error_code = ERR_PAT_RECEIVE;

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
*/


/*
int CS75Plus::SetFixedKeys( LPBYTE KeyTable )
{
	int i;

	int last_error_code ;

	S75FKeys_MSG FKMsg;


	m_pS75FK_keys = (S75FK_KeyList *) KeyTable ;


    // Set the opcode in the FixedKey Msg to Upload (0x0002)

    FKMsg.Opcode = htons(0x0002);

    for (i = 0; i < NUMBER_OF_FIXED_KEYS; i++) {

		if ( m_pS75FK_keys->odd[i] ) {
			FKMsg.FK[i].PID = htons( m_pS75FK_keys->PID[i] | 0x4000 );	
		} else {
			FKMsg.FK[i].PID = htons( m_pS75FK_keys->PID[i] );
		}
		memcpy( FKMsg.FK[i].key, m_pS75FK_keys->key[i], 8 );

    } // for i

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
*/


/*
int CS75Plus::GetFixedKeys( LPBYTE KeyTable )
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
										if ( m_pS75FK_keys->PID[i] & 0x4000 ) {
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
*/


void CS75Plus::CreateRFConfigMsg( S75PlusRFConfigPacket *rf_config_packet_wrapper, void *TuningParameters )
{

	CSatelliteDevice::CreateRFConfigMsg( rf_config_packet_wrapper, TuningParameters );

	(*(((S75PlusRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) = 0;

	if ( ((S75Plus_RFSetting *)TuningParameters)->band == LNB_HIGH )
		(*(((S75PlusRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 4;

    if ( ( ((S75Plus_RFSetting *)TuningParameters)->polarity == LNB_HORIZONTAL ) ||
		 ( ((S75Plus_RFSetting *)TuningParameters)->polarity == LNB_LEFT ) )
		(*(((S75PlusRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 2;

    if ( ((S75Plus_RFSetting *)TuningParameters)->powerOn )
		(*(((S75PlusRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 1;

	if ( ((S75Plus_RFSetting *)TuningParameters)->LongLineCompensation )
		(*(((S75PlusRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 16 ; // Shall we enable Long line Compensation  ?

	if ( ((S75Plus_RFSetting *)TuningParameters)->HighVoltage )
		(*(((S75PlusRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 8 ;  // Account for High Voltage Selection .

	if ( ((S75Plus_RFSetting *)TuningParameters)->PolaritySwitchingVoltage )
		(*(((S75PlusRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 32;

	if ( ((S75Plus_RFSetting *)TuningParameters)->HiLoBandSwitchingTone )
		(*(((S75PlusRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 64;

	(*(((S75PlusRFConfigPacket *)rf_config_packet_wrapper)->LocalOscFrequencyPointer())) = htons( ((S75Plus_RFSetting *)TuningParameters)->LocalOscFrequency );

	srand( time(NULL) );
	(*(((S75PlusRFConfigPacket *)rf_config_packet_wrapper)->RandomPointer())) = rand();

	for ( int i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	digitalsignature( &m_sec, (uint8_t *)(((S75PlusRFConfigPacket *)rf_config_packet_wrapper)->data()), sizeof( S75PlusRFConfig_MSG ) - 20 );

	memcpy( ((S75PlusRFConfigPacket *)rf_config_packet_wrapper)->SignaturePointer(), m_sec.digest, 20 );

}



int CS75Plus::TuneDevice(void *TuningParameters)
{

	S75PlusRFConfig_MSG	rf_config_msg;
	S75PlusRFConfigPacket rf_config_packet_wrapper( &rf_config_msg ); 

    // There are two possible methods of communicating -- by IP address or
    // by MAC address.  For the former, make sure the IP address is set.
    // For the later, make sure the network path is set

    if (!m_Device_attached)		    // Trying to communicate and the class not attached to physical device
		return ERR_RFCFG_NOIP;

	CreateRFConfigMsg( &rf_config_packet_wrapper, TuningParameters );

    return SendRFConfig( &rf_config_packet_wrapper );

}


/*
int CS75Plus::SendPrograms( S75PlusProgramSettings_MSG  *ProgramSettingMsg, int size, int max_programs )
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


//		if ( m_usingStatusListener && ( GetDeviceType() != S75Plus ) ) {
//
//			// Flush the status packet buffer
//			if ((last_error_code =FlushStatusBuffer(STATUS_PACKET_TIMEOUT)) !=ERR_SUCCESS ) {
//				// Error flushing buffer
//				// Don't need to define error code -- done by FlushStatusBuffer()
//				return last_error_code;
//			}
//		    if ((last_error_code =FlushStatusBuffer(STATUS_PACKET_TIMEOUT)) !=ERR_SUCCESS ) {
//				// Error flushing buffer
//				// Don't need to define error code -- done by FlushStatusBuffer()
//				return last_error_code;
//			}
//
//			// Wait for the next status packet
//			if ((last_error_code = WaitForStatusPacket(STATUS_PACKET_TIMEOUT)) != ERR_SUCCESS) {
//				// Error when waiting for status packet
//				// Don't need to define error code -- done by WaitForStatusPacket()
//				return last_error_code;
//			}
//
//		} else {

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

					if ( (((S75PlusStatusPacket *)m_StatusWrapper)->ProgramListPointer())[j] == ntohs(ProgramSettingMsg->Program[i]) ) { // @@NS@@ Note that it was already inverted in prep !!!
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
*/


/*
int CS75Plus::SendPAT( S75PlusPAT_MSG  *PATMsg, int size, int max_programs )
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
		last_error_code = ERR_PAT_SOCKET_CREATE;

	} else {

        // Connect the newly created socket to the S75.

    	device_address.sin_family = AF_INET;

    	device_address.sin_port = htons( PAT_CONFIG_PORT );

    	device_address.sin_addr.s_addr = inet_addr( temp );

    	if ( connect( s, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr_in ) ) != 0){

            last_error_code = ERR_PAT_SOCKET_CONNECT;

		} else {

            // Create a buffer to hold the data we wish to send

            BYTE *Buffer1=NULL;
			Buffer1 = new BYTE[size];

            memcpy(Buffer1, PATMsg, size); // copy the data in the buffer.
			 

            // Create a buffer the size of the PAT to hold the
            // response we receive back.

			BYTE *Buffer2=NULL;
			Buffer2 = new BYTE[size];

            for (i = 0; (!success) && ( i < NUM_RETRIES ); i++) {

    			// Send the PAT to the S75

                if ( send( s, (const char *)Buffer1, size, 0 ) != size ) {

					last_error_code = ERR_PAT_SEND;		
		
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
			            		 last_error_code = ERR_PAT_RECEIVE;
					} // if

#else
        		    	if( (temp1 = recv( s, (char *)Buffer2, size, 0 ) ) == -1 ) {
					 last_error_code = ERR_PAT_RECEIVE;

#endif

						}  // if

    			        // Check to make sure the data received back is the same data sent.

    			        if(temp1 ==  size){

							success = true;

	    	        		for(i = 0 ; i < size; i++ ){

		    	        		if(Buffer1[i] != Buffer2[i]) {

		    		        		last_error_code = ERR_PAT_ACK_BAD;

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

//    if (success) {
//        // Configuration sent to S75D

//        // Flush the status packet buffer
//        if ((last_error_code =FlushStatusBuffer(STATUS_PACKET_TIMEOUT)) !=ERR_SUCCESS ) {
//            // Error flushing buffer
//            // Don't need to define error code -- done by FlushStatusBuffer()
//            return last_error_code;
//        }
//		    if ((last_error_code =FlushStatusBuffer(STATUS_PACKET_TIMEOUT)) !=ERR_SUCCESS ) {
//            // Error flushing buffer
//            // Don't need to define error code -- done by FlushStatusBuffer()
//            return last_error_code;
//        }

//        // Wait for the next status packet
//        if ((last_error_code = WaitForStatusPacket(STATUS_PACKET_TIMEOUT)) != ERR_SUCCESS) {
//            // Error when waiting for status packet
//            // Don't need to define error code -- done by WaitForStatusPacket()
//            return last_error_code;
//        }

//        // Check to see if the values sent to the S75D are the same as those being
//        // reported in the status


//		// Check the PAT with the one we just downloaded.  Note that the 
//		// PAT we just downloaded could be in a different order from ours.


//		configurationApplied = true;


//        // Go through the PAT downloaded and check to make sure that each Program we
//        // uploaded is somewhere in that list.

//		for (i = 0; i < max_programs; i++) {

//			ProgramInList = false;


//	        if (PATMsg->PATTableEntry[i].ProgramNumber != 0) {     // Skip Program 0

//		        for (j = 0; j < max_programs; j++) {

//					if ( (((S75PlusStatusPacket *)m_StatusWrapper)->ProgramListPointer())[j] == PATMsg->PATTableEntry[i].ProgramNumber ) {
//						ProgramInList = true;
//	                    break;
//		            }

//		        }

//			} else {

//				ProgramInList = true;

//	        }

//			if (!ProgramInList) {
//				configurationApplied = false;

//			}

//		}

//	    if (!configurationApplied) {

//			// Configuration not successfully applied

//			last_error_code = ERR_PROGRAMSET_NOTAPPLIED;

//			success = false;

//		} else {

//			// Configuration successfully applied

//			last_error_code = ERR_SUCCESS;

//			success = true;

//		}

//	}

	return last_error_code;

}
*/


/*
int CS75Plus::SendFixedKeys( S75FKeys_MSG  *FKMsg, int size, int max_fixed_keys )
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

//	if (success) {
//        // Configuration sent to S75D

//        // Flush the status packet buffer
//        if ((last_error_code =FlushStatusBuffer(STATUS_PACKET_TIMEOUT)) !=ERR_SUCCESS ) {
//            // Error flushing buffer
//            // Don't need to define error code -- done by FlushStatusBuffer()
//            return last_error_code;
//        }
//		    if ((last_error_code =FlushStatusBuffer(STATUS_PACKET_TIMEOUT)) !=ERR_SUCCESS ) {
//            // Error flushing buffer
//            // Don't need to define error code -- done by FlushStatusBuffer()
//            return last_error_code;
//        }

//        // Wait for the next status packet
//        if ((last_error_code = WaitForStatusPacket(STATUS_PACKET_TIMEOUT)) != ERR_SUCCESS) {
//            // Error when waiting for status packet
//            // Don't need to define error code -- done by WaitForStatusPacket()
//            return last_error_code;
//        }

//        // Check to see if the values sent to the S75D are the same as those being
//        // reported in the status


//		// Check the PAT with the one we just downloaded.  Note that the 
//		// PAT we just downloaded could be in a different order from ours.


//		configurationApplied = true;


//        // Go through the PAT downloaded and check to make sure that each Program we
//        // uploaded is somewhere in that list.

//		for (i = 0; i < max_programs; i++) {

//			ProgramInList = false;


//	        if (PATMsg->PATTableEntry[i].ProgramNumber != 0) {     // Skip Program 0

//		        for (j = 0; j < max_programs; j++) {

//					if ( (((S75PlusStatusPacket *)m_StatusWrapper)->ProgramListPointer())[j] == PATMsg->PATTableEntry[i].ProgramNumber ) {
//						ProgramInList = true;
//	                    break;
//		            }

//		        }

//			} else {

//				ProgramInList = true;

//	        }

//			if (!ProgramInList) {
//				configurationApplied = false;

//			}

//		}

//	    if (!configurationApplied) {

//			// Configuration not successfully applied

//			last_error_code = ERR_PROGRAMSET_NOTAPPLIED;

//			success = false;

//		} else {

//			// Configuration successfully applied

//			last_error_code = ERR_SUCCESS;

//			success = true;

//		}

//	}

	return last_error_code;

}
*/


int CS75Plus::ResetDevice()
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



bool CS75Plus::CreateNetConfigMsg( NetworkConfigPacket *network_config_packet_wrapper, NetSetting *pNetworkSetting, BYTE filter )
{    
	for ( int i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	if ( CNovraDevice::CreateNetConfigMsg( network_config_packet_wrapper, pNetworkSetting, filter ) ) {

		*(((S75PlusNetworkConfigPacket *)network_config_packet_wrapper)->IGMPPointer()) = filter;
		*(((S75PlusNetworkConfigPacket *)network_config_packet_wrapper)->IGMPPointer()) = htons( *(((S75PlusNetworkConfigPacket *)network_config_packet_wrapper)->IGMPPointer()) );

		srand( time(NULL) );
		*(((S75PlusNetworkConfigPacket *)network_config_packet_wrapper)->RandomPointer()) = rand();

		digitalsignature( &m_sec, (uint8_t *)(network_config_packet_wrapper->data()), sizeof( S75PlusNetConfig_MSG ) - 20 );
		memcpy( ((S75PlusNetworkConfigPacket *)network_config_packet_wrapper)->SignaturePointer(), m_sec.digest, 20 );

		return true;

	} else {

		return false;

	}

}


bool CS75Plus::ConfirmDeviceSpecificNetworkSettings( StatusPacket *status_wrapper, NetworkConfigPacket *network_config_packet_wrapper )
{

	bool configurationApplied = true;

    if (memcmp(network_config_packet_wrapper->ReceiverIPPointer(), ((S75PlusStatusPacket *)m_StatusWrapper)->ReceiverIPPointer() , 4) != 0) 
		configurationApplied = false;

	if ( ( (*(((S75PlusStatusPacket *)m_StatusWrapper)->FilterPointer())) &
		   (*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer())) ) !=
		 (*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer())) ) 
		configurationApplied = false;

	return configurationApplied;

}


int CS75Plus::PollStatus(void *Container, int timeout )
{
	int last_error_code ;
	S75PlusStatus_MSG status_msg;
	S75PlusStatusPacket status_packet_wrapper( &status_msg );
	S75Plus_Status *response = NULL;


#ifndef LINUX
	Sleep( 600 );   // To allow fresh status packet to become available on receiver and to prevent flooding devices with poll requests
#endif
#ifdef LINUX
        usleep( 600000 ); 
#endif

	response = (S75Plus_Status *)Container;

	last_error_code = PollStatus( &status_packet_wrapper, timeout );

	if ( last_error_code == ERR_SUCCESS ) {
/*
		fprintf(stderr, "\nStatus Message:\n") ;
		for ( int i = 0 ; i < sizeof(S75PlusStatus_MSG) ; i++ ) {
				if ( i%48 == 0 ) fprintf(stderr, "\n") ;
				fprintf(stderr, "%02.2X ", *(((BYTE *)&status_msg)+i)) ;
		}
*/
		TranscodeStatus( response, &status_packet_wrapper );

	}

	return last_error_code;

}


int CS75Plus::PollStatus( StatusPacket *status_packet_wrapper, int timeout )
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

									firmware_version = ntohs((*(status_packet_wrapper->DSPFirmWarePointer()))) & 0xFF00;
									if( firmware_version != (short)0x6900 ) {		
										success = false;
										last_error_code = ERR_WRONG_RECEIVER;		
									}

									if(m_Device_Type != S75PLUS) {
										success = false;
										last_error_code = ERR_DEVICE_TYPE_MISMATCH;	

									}
						
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


/*
int CS75Plus::GetTraps(LPBYTE TrapParameters)
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
    BYTE Buffer2[sizeof(S75Plus_Event_Trap_MSG)];
	int i;
	int last_error_code ;
	TCHAR temp[16];
	S75Plus_Event_Trap_MSG TrapResponse;
	S75Plus_Event_Trap_MSG TrapRequest;
	WORD monitor_mask;
//


	m_pS75Plus_Traps = (S75PlusPro_TrapSetting *)TrapParameters;

	  // Convert the IP address to a string.
    sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);

	memset( (void *)&TrapResponse, 0, sizeof( TrapResponse ) );
	memset( (void *)&TrapRequest, 0, sizeof( TrapRequest ) );

	// Set the opcode to send to the S75 to download (0x0001)

	TrapRequest.Opcode = htons(0x0001);
	srand( time(NULL) );
	TrapRequest.Random = rand();

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

	digitalsignature( &m_sec, (uint8_t *)(&TrapRequest), sizeof( S75Plus_Event_Trap_MSG ) - 20 );

	memcpy( TrapRequest.Signature, m_sec.digest, 20 );


	// Create a socket to communicate with the S75

#ifndef LINUX
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) {
#else
	if ( ( s = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) {
#endif


        last_error_code = ERR_TRAP_SOCKET_CREATE;

	} else {

        // Connect the newly created socket to the S75.

    	device_address.sin_family = AF_INET;

    	device_address.sin_port = htons( TRAP_CONFIG_PORT );

    	device_address.sin_addr.s_addr = inet_addr( temp);

    	if ( connect( s, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr_in ) ) != 0){

    		last_error_code = ERR_TRAP_SOCKET_CONNECT;

		} else {

            // Create a buffer the size of the Trap Message to hold the
            // response we receive back.

			
            // Download the Trap Settings.  Retry (if required) up to 4 times

            for (i = 0; (!success) && ( i < NUM_RETRIES ); i++) {

            	// Send the Trap reqest to the S75

                if ( send( s, (const char *)(&TrapRequest), sizeof( S75Plus_Event_Trap_MSG), 0 ) != sizeof( S75Plus_Event_Trap_MSG ) ) {

                    last_error_code = ERR_TRAP_SEND;

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
    		    	        if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75Plus_Event_Trap_MSG), 0 ) ) == SOCKET_ERROR ) {

	            	    		temp1 = GetLastError();

	                	        if(temp1 != WSAETIMEDOUT){

	        	        			last_error_code = ERR_TRAP_RECEIVE;

								} // if


#else
    		    	        if( (temp1 = recv( s, (char *)Buffer2, sizeof(S75Plus_Event_Trap_MSG), 0 ) ) == -1 ) {
					last_error_code = ERR_TRAP_RECEIVE;
#endif

							} else {

		    	    	        // Make sure we have received the right number of bytes back

	    		    	        if (temp1 == sizeof(S75Plus_Event_Trap_MSG)) {

	            		    		memcpy(&TrapResponse, Buffer2, sizeof(S75Plus_Event_Trap_MSG));

									// Go through the TrapSettings and change all values from network order to host order.
  						            m_pS75Plus_Traps->MonitoringInterval = ntohs(TrapResponse.MonitoringInterval);
  						            memcpy( m_pS75Plus_Traps->NotificationIP, TrapResponse.NotificationIPAddress, 4 );
  						            m_pS75Plus_Traps->NotificationPort = ntohs(TrapResponse.NotificationPort);
									monitor_mask = ntohs(TrapResponse.Mask);
									m_pS75Plus_Traps->DataLockMonitorOn = monitor_mask & 0x0001 ? 1 : 0;
									m_pS75Plus_Traps->EthernetTxErrorOn = monitor_mask & 0x0002 ? 1 : 0;
									m_pS75Plus_Traps->TSErrorOn = monitor_mask & 0x0004 ? 1 : 0;
									m_pS75Plus_Traps->VBEROn = monitor_mask & 0x0008 ? 1 : 0;
  						            m_pS75Plus_Traps->VBERThreshold = ntohs(TrapResponse.VBERThreshold);
  						            m_pS75Plus_Traps->UncorrectableTSErrorThreshold = ntohl(*((unsigned long *)(&(TrapResponse.UncorrectableTSErrorThreshold[0]))));
  						            m_pS75Plus_Traps->EthernetTxErrorThreshold = ntohs(TrapResponse.EthernetTxErrorThreshold);

					                last_error_code = ERR_SUCCESS;

									success = true;

								} else {
                                    last_error_code = ERR_TRAP_ACK_BAD;

								} // if

							} // if recv

						} else {

							last_error_code = ERR_TRAP_RECEIVE;

						} // if select result 0

					} else {

						last_error_code = ERR_TRAP_RECEIVE;

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
*/


/*
int CS75Plus::SetTraps(LPBYTE TrapParameters)
{
	bool			configurationSent;             // New configuration sent to S75?
	bool			configurationApplied=false;          // New configuration applied to S75?
	BYTE			Buffer[sizeof(S75Plus_Event_Trap_MSG)];                    // Buffer to hold data to send to S75
	S75Plus_Event_Trap_MSG *	pTrapSettings;
	TCHAR			temp[16];                      // Temporary storage variable
	int			LastError;
	int			i;


	m_pS75Plus_TrapSetting  = (S75_TrapSetting_JLEO *) TrapParameters ;		// Type Cast the parameters Passed into the structure 
	pTrapSettings = (S75Plus_Event_Trap_MSG *) Buffer;					// Initialize the structure pointer to the Buffer

	// There are two possible methods of communicating -- by IP address or
	// by MAC address.  For the former, make sure the IP address is set.
	// For the later, make sure the network path is set

	if (!m_Device_attached)		    // Trying to communicate and the class not attached to physical device
		return ERR_TRAPCFG_NOIP;

	memset( pTrapSettings, 0, sizeof( S75Plus_Event_Trap_MSG ) );

    pTrapSettings->Opcode = htons(0x0002);

	pTrapSettings->MonitoringInterval = htons( (unsigned short)( m_pS75Plus_TrapSetting->MonitoringInterval  ) );
	memcpy( pTrapSettings->NotificationIPAddress, m_pS75Plus_TrapSetting->NotificationIP, 4 );
	pTrapSettings->NotificationPort = htons( (unsigned short)( m_pS75Plus_TrapSetting->NotificationPort  ) );
	pTrapSettings->DataLockMonitorOn = htons( (unsigned short)( m_pS75Plus_TrapSetting->DataLockMonitorOn  ) );
	pTrapSettings->VBERThreshold = htons( (unsigned short)( m_pS75Plus_TrapSetting->VBERThreshold  ) );
	*((unsigned long *)(pTrapSettings->UncorrectableTSErrorThreshold)) = htonl( (unsigned long)( m_pS75Plus_TrapSetting->UncorrectableTSErrorThreshold  ) );
	pTrapSettings->EthernetTxErrorThreshold = htons( (unsigned short)( m_pS75Plus_TrapSetting->EthernetTxErrorThreshold  ) );
	pTrapSettings->Mask = 0;
	if ( m_pS75Plus_TrapSetting->DataLockMonitorOn ) {
		pTrapSettings->Mask += 1;
	}
	if ( m_pS75Plus_TrapSetting->VBEROn ) {
		pTrapSettings->Mask += 2;
	}
	if ( m_pS75Plus_TrapSetting->TSErrorOn ) {
		pTrapSettings->Mask += 4;
	}
	if ( m_pS75Plus_TrapSetting->EthernetTxErrorOn ) {
		pTrapSettings->Mask += 8;
	}
	pTrapSettings->Mask = htons( (unsigned short)( pTrapSettings->Mask  ) );

	srand( time(NULL) );
	pTrapSettings->Random = rand();

//	memset( (void *)(&m_sec), 0, sizeof( m_sec ) );

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}


	digitalsignature( &m_sec, (uint8_t *)pTrapSettings, sizeof( S75Plus_Event_Trap_MSG ) - 20 );

	
	memcpy( pTrapSettings->Signature, m_sec.digest, 20 );

//	for ( i = 0; i < 20; i++ ) {
//		pRFSettings->Signature[i] = m_sec.digest[i];
//	}

	// Now the Data is in the buffer and all is set and ready for transmission.
	// Now we construct the destination string.
	sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);
	
	for (i = 0; i < NUM_RETRIES; i++) {
        configurationSent = ((LastError = SendWaitForConfirmation( temp, TRAP_CONFIG_PORT, Buffer, sizeof(S75Plus_Event_Trap_MSG))) == ERR_SUCCESS);
	if (configurationSent) break;
	}


	// Check if we were able to send the configuration successfully
	if (configurationSent) {

//        // Configuration sent to S75D

//	// Flush the status packet buffer
//        if ((LastError =FlushStatusBuffer(STATUS_PACKET_TIMEOUT)) !=ERR_SUCCESS ) {
//            // Error flushing buffer
//            // Don't need to define error code -- done by FlushStatusBuffer()
//            return LastError;
//        }
//		    if ((LastError =FlushStatusBuffer(STATUS_PACKET_TIMEOUT)) !=ERR_SUCCESS ) {
//            // Error flushing buffer
//            // Don't need to define error code -- done by FlushStatusBuffer()
//            return LastError;
//        }

//        // Wait for the next status packet
//        if ((LastError = WaitForStatusPacket(STATUS_PACKET_TIMEOUT)) != ERR_SUCCESS) {
//            // Error when waiting for status packet
//            // Don't need to define error code -- done by WaitForStatusPacket()
//            return LastError;
//        }

//        // Check to see if the values sent to the S75D are the same as those being
//        // reported in the status

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
*/


int CS75Plus::SpecifyPassword( char password[8] )
{
	memcpy( m_sec.password, password, 8 );

	return ERR_SUCCESS;
}


int CS75Plus::ChangePassword( char password[8], unsigned char key[16] )
{
	S75Plus_Password_Change_MSG password_message;
	TCHAR temp[16];
	bool configurationSent=false;             // New configuration sent to S75?
	int LastError = ERR_SUCCESS;
	int i;
	/* char old_password[8]; */


	sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);

//	memset( (void *)(&m_sec), 0, sizeof( m_sec ) );

	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)m_Device_MAC_Address[i];
	}

//	memcpy( old_password, m_sec.password, 8 );
	memcpy( m_sec.password, password, 8 );

	// MAC address to msg buffer
	memmove(password_message.Password, m_sec.macaddress, sizeof(m_sec.macaddress));
	// pad MAC address to 8-bytes
	password_message.Password[6] = 0;
	password_message.Password[7] = 0;
	// copy password to message buffer
	memmove(&(password_message.Password[8]), password, 8 );

	srand( time(NULL) );
	password_message.Random = rand();
	
	// compute digital signature on clear text
	// password and MAC address
	digitalsignature(&m_sec, (unsigned char *)(&password_message), 18);
	memmove(password_message.Signature, m_sec.digest, sizeof(m_sec.digest));
	// encrypt MAC address
	encrypt(key, &(password_message.Password[0]));
	// encrypt password
	encrypt(key, &(password_message.Password[8]));

	// Send to the S75.  If communication fails, retry up to four times.

	for (i = 0; i < NUM_RETRIES; i++) {

		configurationSent = (( LastError = SendWaitForConfirmation( temp, PASSWORD_CHANGE_PORT, (BYTE *)(&password_message), sizeof( S75Plus_Password_Change_MSG ))) == ERR_SUCCESS );
		if (configurationSent) break;

	}

	if ( configurationSent ) memcpy( m_sec.password, password, 8 );

	return LastError;

}


void CS75Plus::TranscodeStatus( S75Plus_Status *response, S75PlusStatusPacket *status_packet_wrapper )
{
	int BERmantissa, BERexponent;   // Bit Error Rate mantissa and exponent
	int i;
	WORD bigNum[4] ;
	int b;

	// Copy the Network settings.
	memcpy(response->DefaultGatewayIP, status_packet_wrapper->DefaultGatewayIPPointer(),4);
	memcpy(response->UniCastStatusDestIP, status_packet_wrapper->DestIPPointer(),4);
	memcpy(response->SubnetMask, status_packet_wrapper->SubnetMaskPointer(), 4);
	memcpy(response->ReceiverMAC, status_packet_wrapper->ReceiverMACPointer(), 6);
	memcpy(response->ReceiverIP, status_packet_wrapper->ReceiverIPPointer(),4 );
	memcpy(response->UID, status_packet_wrapper->ProductIDPointer(),32);

	response->UniCastStatusDestUDP = (*(status_packet_wrapper->StatusDestUDPPointer()));
	response->BroadcastStatusPort = (*(status_packet_wrapper->BroadcastStatusPortPointer()));
		
	// If the RF valid == 0x0400 then RFStatusvalid =1 (easier to read for external programmers...:P)
// @@@DBW	response->RFStatusValid = ((*(status_packet_wrapper->RFStatusValidPointer())) == 0x0400)? 1:0;
	response->RFStatusValid = 1;
	response->SymbolRate = (float) ((*(status_packet_wrapper->SymbolRatePointer())) / 1000.0) ; // Convert Symbol rate to float.
	response->Frequency = (float) ((*(status_packet_wrapper->FrequencyPointer())) / 10.0);  // Convert Frequency to Float 
	response->LocalOscFrequency = (*(status_packet_wrapper->LocalOscFrequencyPointer()));
	response->Power = (*(status_packet_wrapper->LNBConfigurationPointer())) & 0x01 ; // The Power settings in the LNB configuraton on/off 
	response->Polarity = ((*(status_packet_wrapper->LNBConfigurationPointer()))&0x02) >>1; // The Polarity setting in LNB configuration H/V .
	response->Band =  ((*(status_packet_wrapper->LNBConfigurationPointer())) &0x04) >> 2; // The band settings in the LNB configuration Low/ Hi.
	response->Long_Line_Comp = ((*(status_packet_wrapper->LNBConfigurationPointer())) &0x10) >> 4;  // is Long Line Compa ON .
	response->Hi_VoltageMode = ((*(status_packet_wrapper->LNBConfigurationPointer())) &0x8) >> 3;  // is High Voltage On. 
	response->PolaritySwitchingVoltage = ((*(status_packet_wrapper->LNBConfigurationPointer())) &0x20) >> 5;  // is Long Line Compa ON .
	response->HiLoBandSwitchingTone = ((*(status_packet_wrapper->LNBConfigurationPointer())) &0x40) >> 6;  // is Long Line Compa ON .
	response->FirmWareVer = ((*(status_packet_wrapper->DSPFirmWarePointer())) & 0x00f0) >> 4; 
	response->FirmWareRev =	(*(status_packet_wrapper->DSPFirmWarePointer())) & 0xf;
	response->RF_FIRMWARE =	(*(status_packet_wrapper->MicroFirmWarePointer()));
	response->IGMP_Enable = (((unsigned char)((*(status_packet_wrapper->FilterPointer())))) & 0x04 ) >> 2;

	// Copy the Ethernet Statistics...
	response->EthernetPacketDropped	= (*(status_packet_wrapper->EthernetDroppedPointer()));
	response->EthernetReceive		= (*(status_packet_wrapper->EthernetRXPointer()));
	response->EthernetTransmit		= (*(status_packet_wrapper->EthernetTXPointer()));
	response->DVBScrambled			= (*(status_packet_wrapper->DVBScrambledPointer()));
	response->EthernetReceiveError	= (*(status_packet_wrapper->EthernetRXErrPointer()));
	response->DVBDescrambled		= (*(status_packet_wrapper->DVBDescrambledPointer()));
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


	response->DemodulatorGain = (*(status_packet_wrapper->DemodulatorGainPointer()));


	// Convert demodulator status flags into something more readable

	response->SignalLock  = (*(status_packet_wrapper->DemodulatorStatusPointer())) & 0x01 ;

	response->DataLock = ((*(status_packet_wrapper->DemodulatorStatusPointer())) & 0x10) >> 4;

	response->LNBFault = ((*(status_packet_wrapper->LNBConfigurationPointer())) & 0x80) >> 7;


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

    for ( b = 0 ; b < 4 ; b++ ) {
      bigNum[b] = ntohs( *(status_packet_wrapper->TotalDVBPacketsAcceptedPointer()+b) ) ;
	  //fprintf(stderr, "\nbigNum[%d]=0x%X\n", b, bigNum[b]) ;
	}
	memcpy( response->TotalDVBPacketsAccepted, bigNum, 4*sizeof(WORD) ) ;

    for ( b = 0 ; b < 4 ; b++ ) {
      bigNum[b] = ntohs( *(status_packet_wrapper->TotalDVBPacketsRXInErrorPointer()+b) ) ;
	  //fprintf(stderr, "\nbigNum[%d]=0x%X\n", b, bigNum[b]) ;
	}
	memcpy( response->TotalDVBPacketsRXInError, bigNum, 4*sizeof(WORD) ) ;

    for ( b = 0 ; b < 4 ; b++ ) {
      bigNum[b] = ntohs( *(status_packet_wrapper->TotalEthernetPacketsOutPointer()+b) ) ;
	  //fprintf(stderr, "\nbigNum[%d]=0x%X\n", b, bigNum[b]) ;
	}
	memcpy( response->TotalEthernetPacketsOut, bigNum, 4*sizeof(WORD) ) ;

    for ( b = 0 ; b < 4 ; b++ ) {
      bigNum[b] = ntohs( *((WORD*)(status_packet_wrapper->TotalUncorrectableTSPacketsPointer())+b) ) ;
	  //fprintf(stderr, "\nbigNum[%d]=0x%X\n", b, bigNum[b]) ;
	}
	memcpy( response->TotalUncorrectableTSPackets, bigNum, 4*sizeof(WORD) ) ;

	switch(((*(status_packet_wrapper->DSPFirmWarePointer())) & 0xFF00) >> 8){

	case 0x34:		// Older Hardware 
		{
			response->SignalStrength = CalculateSignalStrength( AGC, PERCENTAGE, SIZE_ARRAY );
			break;
		}
	case 0x01:
	case 0x54:	// Newer Hardware 
		{
			response->SignalStrength = (*(status_packet_wrapper->SignalStrengthPointer()));
			break; 
		
		}

	default:

		break;

	}

	response->CardStatus = (*(status_packet_wrapper->CardStatusPointer()));

	response->AGCA = (*(status_packet_wrapper->SignalStrengthPointer()));

	response->AGCN = (*(status_packet_wrapper->DemodulatorGainPointer())); 

	response->GNYQA = ((*(status_packet_wrapper->Digital_GainPointer())) &0x30) >>4 ;

	response->GFARA = ((*(status_packet_wrapper->Digital_GainPointer())) &0xf) ;

	response->NEST = (*(status_packet_wrapper->NESTPointer()));
	response->Clock_Off = (*(status_packet_wrapper->Clock_OffPointer()));
	response->Freq_Err = (*(status_packet_wrapper->Freq_ErrPointer()));
	response->ADC_MID = (*(status_packet_wrapper->ADC_MIDPointer()));
	response->ADC_CLIP = (*(status_packet_wrapper->ADC_CLIPPointer()));
	response->Digital_Gain = (*(status_packet_wrapper->Digital_GainPointer()));
	response->AA_CLAMP = (*(status_packet_wrapper->AA_CLAMPPointer()));

	// Copy the PID list

	PIDRoute *pid_route;

	for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {

		pid_route = &(((PIDRoute *)(status_packet_wrapper->PIDListPointer()))[i]);

		response->PIDList.Entry[i] = response->PIDRoutes[i].PID = pid_route->PID & 0x1fff;

		response->PIDList.Control[i] = (pid_route->PID & 0x2000) >> 13 ;

		response->PIDRoutes[i].DestinationMask = pid_route->DestinationMask;

	}

	TrimPIDList((LPBYTE)(&(response->PIDList)), MAX_SUPP_PIDS );

	memcpy( response->PIDDestinations, status_packet_wrapper->PIDDestinationsPointer(), NUMBER_OF_DESTINATIONS * sizeof( PIDDestination ) );
	memcpy( response->ProgramList, status_packet_wrapper->ProgramListPointer(), NUMBER_OF_PROGRAMS * sizeof( WORD ) );
	memcpy( response->ProgramStatus, status_packet_wrapper->ProgramStatusPointer(), NUMBER_OF_PROGRAMS * sizeof( WORD ) );
}
