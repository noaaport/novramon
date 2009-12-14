// S75_V2.cpp: implementation of the CS75_V2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "s75_v2.h"
#include "S75V2Status.h"
#include <stdio.h>
#include <math.h>



	const int LNB_HIGH = 1;                         // Possible values for 'band' parameter in TuneS75D() function
	const int LNB_LOW = 0;

	const int LNB_HORIZONTAL = 1;                   // Possible values for 'polarity' parameter in TuneS75D() function
	const int LNB_LEFT = 1;
	const int LNB_VERTICAL = 0;
	const int LNB_RIGHT = 0;
	const int	MAX_SUPP_PIDS = 32;
	
// CONSTANTS for converting AGC to percentage 

	const double  AGC[]        = { 254, 112, 105, 95 };  // Automatic Gain Control
	const double  PERCENTAGE[] = { 10, 65, 80, 100 };
	const int     SIZE_ARRAY   = 4;
	



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CS75_V2::CS75_V2()
{
	m_StatusWrapper = (StatusPacket *)(new S75V2StatusPacket( &m_Status ));
//	m_StatusStructure = (S75D_V2_Status_MSG *)(&m_Status);
	m_StatusPacketSize = sizeof(S75D_V2_Status_MSG);
	m_Device_Type = S75D_V21;  // when we instantiate this class, the device type will be enforced on attachment.
	m_pStatusBuffer = (LPBYTE) &m_Status;	// Initialize the Member variable of the parent.
	m_PID_HW_Filter = 1;
	
}

CS75_V2::~CS75_V2()
{

}

void CS75_V2::CreateRFConfigMsg( SatelliteRFConfigPacket *rf_config_packet_wrapper, void *TuningParameters )
{

	CSatelliteDevice::CreateRFConfigMsg( rf_config_packet_wrapper, TuningParameters );

	(*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) = 0;

    if ( ((S75D_RFSetting *) TuningParameters)->band == LNB_HIGH)
		(*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 4;

    if ( ( ((S75D_RFSetting *) TuningParameters)->polarity == LNB_HORIZONTAL ) ||
		 ( ((S75D_RFSetting *) TuningParameters)->polarity == LNB_LEFT ) )
		(*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 2;

    if ( ((S75D_RFSetting *) TuningParameters)->powerOn )
		(*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 1;

}


int CS75_V2::TuneDevice(void *TuningParameters)
{

	SatelliteRFConfig_MSG	rf_config_msg;
	SatelliteRFConfigPacket rf_config_packet_wrapper( &rf_config_msg ); 

    // There are two possible methods of communicating -- by IP address or
    // by MAC address.  For the former, make sure the IP address is set.
    // For the later, make sure the network path is set

    if (!m_Device_attached)		    // Trying to communicate and the class not attached to physical device
		return ERR_RFCFG_NOIP;

	CreateRFConfigMsg( &rf_config_packet_wrapper, TuningParameters );

    return SendRFConfig( &rf_config_packet_wrapper );

}


int CS75_V2::ConfigureNetwork(void *pNetworkSetting)
{
	CommonNetConfig_MSG			net_setting_msg;
	CommonNetworkConfigPacket	network_config_packet_wrapper( &net_setting_msg );


    // There are two possible methods of communicating -- by IP address or
    // by MAC address.  For the former, make sure the IP address is set.
    // For the later, make sure the network path is set

    if ((!m_Device_attached) && (!m_NetPathSet))
    
		// Trying to communicate by IP with no network path -- error
        return ERR_NETCFG_NONETPATH;


    // Destination information properly set

	if ( !CreateNetConfigMsg( (NetworkConfigPacket *)(&network_config_packet_wrapper), (NetSetting *)pNetworkSetting, m_PID_HW_Filter ) )
		return ERR_BAD_IP_SUBNET_COMBO;

	// we should Check the Status Port to Make sure it is in teh auto detection List.

	if ( !CheckStatusPort( ((NetSetting *)pNetworkSetting)->StatusPort ) )	// Broadcast Status Port
		return ERR_BAD_STATUS_PORT;

	return SendNetworkConfig( (NetworkConfigPacket *)(&network_config_packet_wrapper) );

}


int CS75_V2::SetPIDTable(LPBYTE PIDTable)
{
	int i;
	int j;
	int last_error_code ;
	S75V2_PIDSetting_MSG PidSettingMsg;
	CommonPIDSettingsPacket pid_setting_packet_wrapper( &PidSettingMsg );


	m_pS75D_PIDList = (S75D_PIDList *) PIDTable ;

	m_PIDList = (PIDList *)(new StandardPIDList( (void *)PIDTable ));


    // Set the opcode in the PID table to Upload (0x0002)

    PidSettingMsg.Opcode = htons(0x0002);

	if(m_pS75D_PIDList->NumberofPIDs > MAX_SUPP_PIDS) return ERR_PID_TOOLONG ;

    

    for (i = 0; i < MAX_SUPP_PIDS; i++) {

        for ( j =0; j < 4; j++ ) {

            PidSettingMsg.PIDTableEntry[i].DestinationIP[j] = 0;

		} // for j

        PidSettingMsg.PIDTableEntry[i].DestinationUDP = 0;

		if(i<m_pS75D_PIDList->NumberofPIDs){
			
			PidSettingMsg.PIDTableEntry[i].PID = m_pS75D_PIDList->Entry[i] ;
		
			if((m_pS75D_PIDList->Control[i] & 0x01) && (m_pS75D_PIDList->Entry[i] != NULL_PID))
				PidSettingMsg.PIDTableEntry[i].PID |= 0x2000;
		}else{
			PidSettingMsg.PIDTableEntry[i].PID = NULL_PID;
		}

    } // for i

	last_error_code = SendPIDs( (PIDSettingsPacket *)(&pid_setting_packet_wrapper), sizeof( S75V2_PIDSettings_MSG ), MAX_SUPP_PIDS );

	delete m_PIDList;

    return last_error_code;
}


void CS75_V2::TranscodePIDList( PIDSettings_MSG *PidSettingMSG )
{
	
	for ( int i = 0; i < MAX_SUPP_PIDS; i++) {

		PidSettingMSG->PIDTableEntry[i].PID = ntohs(PidSettingMSG->PIDTableEntry[i].PID);
		m_pS75D_PIDList->Entry[i] = PidSettingMSG->PIDTableEntry[i].PID & 0x1FFF;
		m_pS75D_PIDList->Control[i] = ((PidSettingMSG->PIDTableEntry[i].PID & 0x2000) > 1)? 1:0 ;

	}

	m_pS75D_PIDList->NumberofPIDs = MAX_SUPP_PIDS ;

	TrimPIDList((LPBYTE)  m_pS75D_PIDList, MAX_SUPP_PIDS) ;

}


int CS75_V2::GetPIDTable(LPBYTE PIDTable)
{

	m_pS75D_PIDList = (S75D_PIDList *) PIDTable ;

	return GetPIDs( sizeof(S75V2_PIDSettings_MSG) );

}


bool CS75_V2::ParseStatusPacket()
{
	BYTE temp[4];

	// If this function is called, the status Structure has been filled with data received by Status Listner.
	// We need to verify that the packet is valid, and that it matches the device to be valid.

	if ( !CNovraDevice::ParseStatusPacket() ) return false;

	((S75V2StatusPacket *)m_StatusWrapper)->ntoh();

//	for(int i=0;i<MAX_SUPP_PIDS;i++) 
//		(((S75V2StatusPacket *)m_StatusWrapper)->PIDListPointer())[i] = ntohs((((S75V2StatusPacket *)m_StatusWrapper)->PIDListPointer())[i]);
	
	if( ( (*(((S75V2StatusPacket *)m_StatusWrapper)->DSPFirmWarePointer())) & 0x0F00) != 0x0200 )			// Communication rev. 2.0
	 return false ;		/// if the device does not follow communication rev 2.0 it is not supported.

	if(m_Device_Type != S75D_V21) return false;

	m_StatusListener.GetLastPacketSource(temp);
	
	memcpy(m_Device_IP_Address, temp,4); // Get the address from the Unit.

	return true;
}


int CS75_V2::GetStatus(void *Container)
{
	S75D_Status * pStatus;
	WORD *PIDList = (WORD *)(m_StatusWrapper->PIDListPointer());


	pStatus = (S75D_Status *) Container;

	if(!IsStatusValid())  return ERR_FAILED ; // the status isn't valid. 

	if(m_Device_Type != S75D_V21) return ERR_DEVICE_TYPE_MISMATCH ;

	CSatelliteDevice::GetStatus( (SatelliteStatus *)(&(S75V2Status( Container ))) );

	pStatus->SignalStrength = CalculateSignalStrength( AGC, PERCENTAGE, SIZE_ARRAY );

	pStatus->VBER = CalculateVBER( -1 );

	memcpy(pStatus->Reserved, ((S75V2StatusPacket *)m_StatusWrapper)->ReservedPointer(),8);

	pStatus->EthernetTransmitError	= (*(((S75V2StatusPacket *)m_StatusWrapper)->EthernetTXErrPointer()));

	pStatus->WRESERVED[1]	= (*(((S75V2StatusPacket *)m_StatusWrapper)->FEL_LossPointer()));
	
	// Copy the PID list

	for ( int i = 0; i < MAX_SUPP_PIDS; i++ ) {

		pStatus->PIDList.Entry[i] = PIDList[i] & 0x1fff;

		pStatus->PIDList.Control[i] = (PIDList[i] & 0x2000) >> 13 ;	

	}

	TrimPIDList((LPBYTE)(&(pStatus->PIDList)), MAX_SUPP_PIDS );

	return ERR_SUCCESS ;

}


bool CS75_V2::GetDefaultConfigStruct(void *pConfig)
{

	if((!m_Device_attached) ||(!m_IPAddressValid && !m_MACAddressValid)) return false;

	CNovraDevice::GetDefaultConfigStruct( pConfig );

  	((S75D_NetSetting *)pConfig)->DestinationUDP = 9000;
  		
  	return true;	

}


bool CS75_V2::CreateNetConfigMsg( NetworkConfigPacket *network_config_packet_wrapper, NetSetting *pNetworkSetting, BYTE filter )
{    

	if ( CNovraDevice::CreateNetConfigMsg( network_config_packet_wrapper, pNetworkSetting, filter ) ) {

		*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->StatusIntervalPointer()) = 5;
		*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer()) = filter;

		return true;

	} else {

		return false;

	}

}



bool CS75_V2::ConfirmDeviceSpecificNetworkSettings( StatusPacket *status_wrapper, NetworkConfigPacket *network_config_packet_wrapper )
{

	if ( ( (*(((CommonStatusPacket *)m_StatusWrapper)->FilterPointer())) &
		   (*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer())) ) ==
		 (*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer())) ) 
		return true;
	else
		return false;

}
