// A75.cpp: implementation of the CA75 class.
//
//////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "A75.h"
#include "A75Status.h"
//#include "linux_windows_types.h"
#include <stdio.h>
#include <math.h>


	const float _SYMBOL_RATE  = 10760000.0 ;
	const int	MAX_SUPP_PIDS = 16;

// CONSTANTS for converting AGC to percentage

	const double  AGC[]        = { 254, 112, 105, 95 };  // Automatic Gain Control
	const double  PERCENTAGE[] = { 10, 65, 80, 100 };
	const int     SIZE_ARRAY   = 4;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CA75::CA75()
{
	m_StatusWrapper = (StatusPacket *)(new A75StatusPacket( &m_Status ));
	m_StatusPacketSize = sizeof(A75Status_MSG);
	m_Device_Type = A75;  // when we instantiate this class, the device type will be enforced on attachment.
	m_pStatusBuffer = (LPBYTE) &m_Status;	// Initialize the Member variable of the parent.

}

CA75::~CA75()
{

}



bool CA75::GetDefaultConfigStruct(void *pConfig)
{

	if((!m_Device_attached) ||(!m_IPAddressValid && !m_MACAddressValid)) return false;

	CNovraDevice::GetDefaultConfigStruct( pConfig );
 
  	((A75_NetSetting *)pConfig)->UniCastStatusDestUDP =  m_Device_Status_Port;

	((A75_NetSetting *)pConfig)->IGMP_Enable = 0;
  
  	return true;	

}


void CA75::TranscodePIDList( PIDSettings_MSG *PidSettingMSG )
{
	
	for ( int i = 0; i < MAX_SUPP_PIDS; i++) {

		PidSettingMSG->PIDTableEntry[i].PID = ntohs(PidSettingMSG->PIDTableEntry[i].PID);
		m_pA75_PIDList->Entry[i] = PidSettingMSG->PIDTableEntry[i].PID & 0x1FFF;
//		m_pA75_PIDList->Control[i] = ((PidSettingMSG->PIDTableEntry[i].PID & 0x2000) > 1)? 1:0 ;
		m_pA75_PIDList->Control[i] = (PidSettingMSG->PIDTableEntry[i].PID & 0xE000) >> 13;

	}

	m_pA75_PIDList->NumberofPIDs = MAX_SUPP_PIDS ;

	TrimPIDList((LPBYTE)  m_pA75_PIDList, MAX_SUPP_PIDS) ;

}


int CA75::GetPIDTable(LPBYTE PIDTable)
{

	m_pA75_PIDList = (A75_PIDList *) PIDTable ;

	return GetPIDs( sizeof(A75PIDSettings_MSG) );

}


int CA75::SetPIDTable(LPBYTE PIDTable)
{
	int i;
	int j;
	int last_error_code ;

	A75PIDSettings_MSG PidSettingMsg;
	CommonPIDSettingsPacket pid_setting_packet_wrapper( &PidSettingMsg );
	

	m_pA75_PIDList = (A75_PIDList *) PIDTable ;
	m_PIDList = (PIDList *)(new StandardPIDList( (void *)PIDTable ));

    // Set the opcode in the PID table to Upload (0x0002)

    PidSettingMsg.Opcode = htons(0x0002);

    // Copy the PID table to download into the CPIDProgram object
	if(m_pA75_PIDList->NumberofPIDs > MAX_SUPP_PIDS) return ERR_PID_TOOLONG ;

    for (i = 0; i < MAX_SUPP_PIDS; i++) {

        for ( j =0; j < 4; j++ ) {

            PidSettingMsg.PIDTableEntry[i].DestinationIP[j] = 0;

		} // for j

        PidSettingMsg.PIDTableEntry[i].DestinationUDP = 0;
		if(i<m_pA75_PIDList->NumberofPIDs){
			PidSettingMsg.PIDTableEntry[i].PID = m_pA75_PIDList->Entry[i] ;
			if(m_pA75_PIDList->Entry[i] != NULL_PID)
				PidSettingMsg.PIDTableEntry[i].PID |= 0x2000;
		}else{
			PidSettingMsg.PIDTableEntry[i].PID = NULL_PID;
		}

    } // for i

	last_error_code = SendPIDs( (PIDSettingsPacket *)(&pid_setting_packet_wrapper), sizeof( A75PIDSettings_MSG ), MAX_SUPP_PIDS );

	delete m_PIDList;

    return last_error_code;

}




int CA75::ConfigureNetwork(void *pNetworkSetting)
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


	if ( !CreateNetConfigMsg( (NetworkConfigPacket *)(&network_config_packet_wrapper), (NetSetting *)pNetworkSetting, 4*(((A75_NetSetting *) pNetworkSetting)->IGMP_Enable) ) )
		return ERR_BAD_IP_SUBNET_COMBO;

	// We should Check the Status Ports to Make sure they are in the auto detection List.
	
	if ( !CheckStatusPort( ((NetSetting *)pNetworkSetting)->StatusPort ) )	// Broadcast Status Port
		return ERR_BAD_STATUS_PORT;

	if ( !CheckStatusPort( ((NetSetting *)pNetworkSetting)->DestUDP ) )		// Unicast Status Port
		return ERR_BAD_STATUS_PORT;

	return SendNetworkConfig( (NetworkConfigPacket *)(&network_config_packet_wrapper) );

}


bool CA75::ParseStatusPacket()
{
		
	BYTE temp[4];

	// If this function is called, the status Structure has been filled with data received by Status Listner.
	// We need to verify that the packet is valid, and that it matches the device to be valid.

	if ( !CNovraDevice::ParseStatusPacket() ) return false;

	((A75StatusPacket *)m_StatusWrapper)->ntoh();

//	for(int i=0;i<MAX_SUPP_PIDS;i++)
//		(((A75StatusPacket *)m_StatusWrapper)->PIDListPointer())[i] = ntohs((((A75StatusPacket *)m_StatusWrapper)->PIDListPointer())[i]);

	if ( ( (*(m_StatusWrapper->DSPFirmWarePointer())) & 0xFF00) != 0x4300 )			// Communication rev. 3.0
	 return false ;		/// if the device does not follow communication rev 3.0 it is not supported.

	if(m_Device_Type != A75) return false;

	m_StatusListener.GetLastPacketSource(temp);

	memcpy(m_Device_IP_Address, temp,4); // Get the address from the Unit.

	return true;


}

int CA75::GetStatus(void *Container)
{
	A75_Status * pStatus;


	pStatus = (A75_Status *) Container;

	if(!IsStatusValid())  return ERR_FAILED ; // the status isn't valid. 

	if(m_Device_Type != A75) return ERR_DEVICE_TYPE_MISMATCH ;

	ATSCDevice::GetStatus( (ATSCStatus *)(&(A75Status( Container ))), false, 16  );

	memcpy(pStatus->DVB_MACADDRESS, ((A75StatusPacket *)m_StatusWrapper)->DVBMACPointer(),6);

	return ERR_SUCCESS ;

}


bool CA75::CreateNetConfigMsg( NetworkConfigPacket *network_config_packet_wrapper, NetSetting *pNetworkSetting, BYTE filter )
{    

	if ( CNovraDevice::CreateNetConfigMsg( network_config_packet_wrapper, pNetworkSetting, filter ) ) {

		*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->StatusIntervalPointer()) = 5;
		*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer()) = filter;

		return true;

	} else {

		return false;

	}

}


bool CA75::ConfirmDeviceSpecificNetworkSettings( StatusPacket *status_wrapper, NetworkConfigPacket *network_config_packet_wrapper )
{

	if ( ( (*(((CommonStatusPacket *)m_StatusWrapper)->FilterPointer())) &
		   (*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer())) ) ==
		 (*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer())) ) 
		return true;
	else
		return false;

}
