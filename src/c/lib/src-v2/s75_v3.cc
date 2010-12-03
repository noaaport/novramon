// S75_V3.cpp: implementation of the CS75_V3 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "s75_v3.h"
#include "S75V3Status.h"
#include <stdio.h>
#include <math.h>


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

CS75_V3::CS75_V3()
{
	m_StatusWrapper = (StatusPacket *)(new S75V3StatusPacket( &m_Status ));
//	m_StatusStructure = (S75D_V3_Status_MSG *)(&m_Status);
	m_StatusPacketSize = sizeof(S75D_V2_Status_MSG);
	m_Device_Type = S75D_V3;  // when we instantiate this class, the device type will be enforced on attachment.
	m_pStatusBuffer = (LPBYTE) &m_Status;	// Initialize the Member variable of the parent.

}

CS75_V3::~CS75_V3()
{

}

bool CS75_V3::ParseStatusPacket()
{
	BYTE temp[4];

	// If this function is called, the status Structure has been filled with data received by Status Listner.
	// We need to verify that the packet is valid, and that it matches the device to be valid.

	if ( !CNovraDevice::ParseStatusPacket() ) return false;

	((S75V3StatusPacket *)m_StatusWrapper)->ntoh();

//	for(int i=0;i<MAX_SUPP_PIDS;i++)
//		(((S75V3StatusPacket *)m_StatusWrapper)->PIDListPointer())[i] = ntohs((((S75V3StatusPacket *)m_StatusWrapper)->PIDListPointer())[i]);

	if(((*(((S75V3StatusPacket *)m_StatusWrapper)->DSPFirmWarePointer())) & 0xFF00) != 0x3300 )			// Communication rev. 3.0
	 return false ;		/// if the device does not follow communication rev 3.0 it is not supported.

	if(m_Device_Type != S75D_V3) return false;

	m_StatusListener.GetLastPacketSource(temp);

	memcpy(m_Device_IP_Address, temp,4); // Get the address from the Unit.

	return true;
}


int CS75_V3::ConfigureNetwork(void *pNetworkSetting)
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

	if ( !CreateNetConfigMsg( (NetworkConfigPacket *)(&network_config_packet_wrapper), (NetSetting *)pNetworkSetting, 4*(((S75D_NetSetting_V3 *) pNetworkSetting)->IGMP_Enable) ) )
		return ERR_BAD_IP_SUBNET_COMBO;

	// we should Check the Status Port to Make sure it is in the auto detection List.
	
	if ( !CheckStatusPort( ((NetSetting *)pNetworkSetting)->StatusPort ) )	// Broadcast Status Port
		return ERR_BAD_STATUS_PORT;

	if ( !CheckStatusPort( ((NetSetting *)pNetworkSetting)->DestUDP ) )		// Unicast Status Port
		return ERR_BAD_STATUS_PORT;

	return SendNetworkConfig( (NetworkConfigPacket *)(&network_config_packet_wrapper) );

}


void CS75_V3::CreateRFConfigMsg( SatelliteRFConfigPacket *rf_config_packet_wrapper, void *TuningParameters )
{

	CSatelliteDevice::CreateRFConfigMsg( rf_config_packet_wrapper, TuningParameters );


	(*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) = 0;

	if ( ((S75D_RFSetting_V3 *)TuningParameters)->band == LNB_HIGH )
		(*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 4;

    if ( ( ((S75D_RFSetting_V3 *)TuningParameters)->polarity == LNB_HORIZONTAL ) ||
		 ( ((S75D_RFSetting_V3 *)TuningParameters)->polarity == LNB_LEFT))
		(*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 2;

    if ( ((S75D_RFSetting_V3 *)TuningParameters)->powerOn )
		(*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 1;

	if ( ((S75D_RFSetting_V3 *)TuningParameters)->LongLineCompensation )
		(*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 16 ; // Shall we enable Long line Compensation  ?

	if ( ((S75D_RFSetting_V3 *)TuningParameters)->HighVoltage )
		(*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 8 ;  // Account for High Voltage Selection .


}


int CS75_V3::TuneDevice(void *TuningParameters)
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


int CS75_V3::SetPIDTable(LPBYTE PIDTable)
{

	int i;
	int j;
	int last_error_code ;
	S75V3_PIDSetting_MSG PidSettingMsg;
	CommonPIDSettingsPacket pid_setting_packet_wrapper( &PidSettingMsg );

	
	m_pS75D_PIDList = (S75D_PIDList_V3 *) PIDTable ;

	m_PIDList = (PIDList *)(new StandardPIDList( (void *)PIDTable ));



    // Set the opcode in the PID table to Upload (0x0002)

    PidSettingMsg.Opcode = htons(0x0002);

    // Copy the PID table to download into the CPIDProgram object
	if(m_pS75D_PIDList->NumberofPIDs > MAX_SUPP_PIDS) return ERR_PID_TOOLONG ;

    for (i = 0; i < MAX_SUPP_PIDS; i++) {

        for ( j =0; j < 4; j++ ) {

            PidSettingMsg.PIDTableEntry[i].DestinationIP[j] = 0;

		} // for j

        PidSettingMsg.PIDTableEntry[i].DestinationUDP = 0;
		if(i<m_pS75D_PIDList->NumberofPIDs){
			PidSettingMsg.PIDTableEntry[i].PID = m_pS75D_PIDList->Entry[i] ;
			if(m_pS75D_PIDList->Entry[i] != NULL_PID)
				PidSettingMsg.PIDTableEntry[i].PID |= 0x2000;
		}else{
			PidSettingMsg.PIDTableEntry[i].PID = NULL_PID;
		}

    } // for i

	last_error_code = SendPIDs( (PIDSettingsPacket *)(&pid_setting_packet_wrapper), sizeof( S75V3_PIDSettings_MSG ), MAX_SUPP_PIDS );

	delete m_PIDList;

    return last_error_code;


}


void CS75_V3::TranscodePIDList( PIDSettings_MSG *PidSettingMSG )
{
	
	for ( int i = 0; i < MAX_SUPP_PIDS; i++ ) {

   		PidSettingMSG->PIDTableEntry[i].PID = ntohs(PidSettingMSG->PIDTableEntry[i].PID);
		m_pS75D_PIDList->Entry[i] = PidSettingMSG->PIDTableEntry[i].PID & 0x1FFF;
		m_pS75D_PIDList->Control[i] = ((PidSettingMSG->PIDTableEntry[i].PID & 0x2000) > 1)? 1:0 ;
									
	}

	m_pS75D_PIDList->NumberofPIDs = MAX_SUPP_PIDS ;

	TrimPIDList((LPBYTE)  m_pS75D_PIDList, MAX_SUPP_PIDS) ;

}


int CS75_V3::GetPIDTable(LPBYTE PIDTable)
{

	m_pS75D_PIDList = (S75D_PIDList_V3 *) PIDTable ;

	return GetPIDs( sizeof(S75V3_PIDSettings_MSG) );

}


int CS75_V3::GetStatus(void *Container)
{

	S75D_Status_V3 * pStatus;
	WORD *PIDList = (WORD *)(m_StatusWrapper->PIDListPointer());


	pStatus = (S75D_Status_V3 *) Container;

	if(!IsStatusValid())  return ERR_FAILED ; // the status isn't valid. 

	if(m_Device_Type != S75D_V3) return ERR_DEVICE_TYPE_MISMATCH ;

	CSatelliteDevice::GetStatus( (SatelliteStatus *)(&(S75V3Status( Container ))), false, 16 );

	pStatus->SignalStrength = CalculateSignalStrength( AGC, PERCENTAGE, SIZE_ARRAY );
	
	pStatus->VBER = CalculateVBER();

	memcpy(pStatus->DVB_MACADDRESS, ((S75V3StatusPacket *)m_StatusWrapper)->DVBMACPointer(),6);

	memcpy(pStatus->Reserved, ((S75V3StatusPacket *)m_StatusWrapper)->ReservedPointer(),8);

	pStatus->Long_Line_Comp = ((*(((S75V3StatusPacket *)m_StatusWrapper)->LNBConfigurationPointer())) &0x10) >> 4;  // is Long Line Compa ON .

	pStatus->Hi_VoltageMode = ((*(((S75V3StatusPacket *)m_StatusWrapper)->LNBConfigurationPointer())) &0x8) >> 3;  // is High Voltage On. 

	pStatus->EthernetTransmitError	= (*(((S75V3StatusPacket *)m_StatusWrapper)->EthernetTXErrPointer()));

	pStatus->WRESERVED[1]	= (*(((S75V3StatusPacket *)m_StatusWrapper)->FEL_LossPointer()));

	// Copy the PID list

	for ( int i = 0; i < MAX_SUPP_PIDS; i++ ) {

		pStatus->PIDList.Entry[i] = PIDList[i] & 0x1fff;

		pStatus->PIDList.Control[i] = (PIDList[i] & 0x2000) >> 13 ;	

	}

	TrimPIDList((LPBYTE)(&(pStatus->PIDList)), MAX_SUPP_PIDS );

	return ERR_SUCCESS ;

}


bool CS75_V3::GetDefaultConfigStruct(void *pConfig)
{

	if((!m_Device_attached) ||(!m_IPAddressValid && !m_MACAddressValid)) return false;

	CNovraDevice::GetDefaultConfigStruct( pConfig );
  
  	((S75D_NetSetting_V3 *)pConfig)->UniCastStatusDestUDP =  m_Device_Status_Port;

	((S75D_NetSetting_V3 *)pConfig)->IGMP_Enable = 0;
    		
  	return true;	

}


bool CS75_V3::CreateNetConfigMsg( NetworkConfigPacket *network_config_packet_wrapper, NetSetting *pNetworkSetting, BYTE filter )
{    

	if ( CNovraDevice::CreateNetConfigMsg( network_config_packet_wrapper, pNetworkSetting, filter ) ) {

		*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->StatusIntervalPointer()) = 5;
		*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer()) = filter;

		return true;

	} else {

		return false;

	}

}


bool CS75_V3::ConfirmDeviceSpecificNetworkSettings( StatusPacket *status_wrapper, NetworkConfigPacket *network_config_packet_wrapper )
{

	if ( ( (*(((CommonStatusPacket *)m_StatusWrapper)->FilterPointer())) &
		   (*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer())) ) ==
		 (*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer())) ) 
		return true;
	else
		return false;

}
