// A75PesMaker.cpp: implementation of the CA75PesMaker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "A75PesMaker.h"
#include "A75PesMakerStatus.h"
#include <math.h>


const int	MAX_SUPP_PIDS = 32;
const float _SYMBOL_RATE  = 10760000.0 ;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CA75PesMaker::CA75PesMaker()
{

	m_StatusWrapper = (StatusPacket *)(new A75PesMakerStatusPacket( &m_Status ));
	m_StatusPacketSize = sizeof(A75PesMakerStatus_MSG);
	m_Device_Type = A75PesMaker;  // when we instantiate this class, the device type will be enforced on attachment.
	m_pStatusBuffer = (LPBYTE) &m_Status;	// Initialize the Member variable of the parent.

}

CA75PesMaker::~CA75PesMaker()
{

}


void CA75PesMaker::TranscodePIDList( PIDSettings_MSG *PidSettingMSG )
{

	for ( int i = 0; i < MAX_SUPP_PIDS; i++) {

		PidSettingMSG->PIDTableEntry[i].PID = ntohs(PidSettingMSG->PIDTableEntry[i].PID);
		PidSettingMSG->PIDTableEntry[i].DestinationUDP = ntohs(PidSettingMSG->PIDTableEntry[i].DestinationUDP);
		m_pA75_PIDList->Entry[i] = PidSettingMSG->PIDTableEntry[i].PID & 0x1FFF;
		m_pA75_PIDList->Control[i] = ((PidSettingMSG->PIDTableEntry[i].PID & 0xE000) >> 13) ;
		memcpy( m_pA75_PIDList->ForwardInformation[i].Destination_IP, PidSettingMSG->PIDTableEntry[i].DestinationIP, 4);
		m_pA75_PIDList->ForwardInformation[i].Destination_Port =  PidSettingMSG->PIDTableEntry[i].DestinationUDP ;

	}
	
	m_pA75_PIDList->NumberofPIDs = MAX_SUPP_PIDS ;
	TrimPIDList((LPBYTE)  m_pA75_PIDList, MAX_SUPP_PIDS) ;

}


int CA75PesMaker::SetPIDTable(LPBYTE PIDTable)
{
	int i;

	int last_error_code ;
	A75PesMakerPIDSettings_MSG PidSettingMsg;
	CommonPIDSettingsPacket pid_setting_packet_wrapper( &PidSettingMsg );

	
	m_pA75_PIDList = (PesMaker_PIDList *) PIDTable ;
	m_PIDList = (PIDList *)(new PesMakerPIDList( (void *)PIDTable ));


    // Set the opcode in the PID table to Upload (0x0002)

    PidSettingMsg.Opcode = htons(0x0002);

    // Copy the PID table to download into the CPIDProgram object
	// Does The PID list have too many entries !!? 
	if(m_pA75_PIDList->NumberofPIDs > MAX_SUPP_PIDS) return ERR_PID_TOOLONG ;

    for (i = 0; i < MAX_SUPP_PIDS; i++) {

		memcpy(PidSettingMsg.PIDTableEntry[i].DestinationIP, (m_pA75_PIDList->ForwardInformation[i].Destination_IP), 4) ;

        PidSettingMsg.PIDTableEntry[i].DestinationUDP = m_pA75_PIDList->ForwardInformation[i].Destination_Port;
		if(i<m_pA75_PIDList->NumberofPIDs){
			PidSettingMsg.PIDTableEntry[i].PID = m_pA75_PIDList->Entry[i] ;
			PidSettingMsg.PIDTableEntry[i].PID |= (m_pA75_PIDList->Control[i] << 13);
		}else{
			PidSettingMsg.PIDTableEntry[i].PID = NULL_PID;
		}

    } // for i

	last_error_code = SendPIDs( (PIDSettingsPacket *)(&pid_setting_packet_wrapper), sizeof( A75PesMakerPIDSettings_MSG ), MAX_SUPP_PIDS, true );

	delete m_PIDList;

    return last_error_code;


}


int CA75PesMaker::GetPIDTable(BYTE * PIDTable)
{

	m_pA75_PIDList = (A75PesMaker_PIDList *) PIDTable ;

	return GetPIDs( sizeof(A75PesMakerPIDSettings_MSG) );

}


int CA75PesMaker::GetStatus(void *Container)
{
	A75_Status * pStatus;


	pStatus = (A75_Status *) Container;

	if(!IsStatusValid())  return ERR_FAILED ; // the status isn't valid. 

	if(m_Device_Type != A75PesMaker) return ERR_DEVICE_TYPE_MISMATCH ;

	ATSCDevice::GetStatus( (ATSCStatus *)(&(A75PesMakerStatus( Container ))) );

	memcpy(pStatus->DVB_MACADDRESS, ((A75PesMakerStatusPacket *)m_StatusWrapper)->DVBMACPointer(),6);

	return ERR_SUCCESS ;

}


int CA75PesMaker::ConfigureNetwork(void *pNetworkSetting)
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

	if ( !CreateNetConfigMsg( (NetworkConfigPacket *)(&network_config_packet_wrapper), (NetSetting *)pNetworkSetting, 4*(((A75PesMaker_NetSetting *) pNetworkSetting)->IGMP_Enable) ) )
		return ERR_BAD_IP_SUBNET_COMBO;

	// we should Check the Status Port to Make sure it is in the auto detection List.
	
	if ( !CheckStatusPort( ((NetSetting *)pNetworkSetting)->StatusPort ) )	// Broadcast Status Port
		return ERR_BAD_STATUS_PORT;

	if ( !CheckStatusPort( ((NetSetting *)pNetworkSetting)->DestUDP ) )		// Unicast Status Port
		return ERR_BAD_STATUS_PORT;

	return SendNetworkConfig( (NetworkConfigPacket *)(&network_config_packet_wrapper) );

}


bool CA75PesMaker::TrimPIDList(LPBYTE PL, int Max)
{
	A75PesMaker_PIDList *pL;

	int j=0,k=0,l=0,m=0;

	pL = (A75PesMaker_PIDList*) PL;

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
			memcpy( &(pL->ForwardInformation[k]), &(pL->ForwardInformation[j]), sizeof(FWD_Info));
			// Increment the counter that points to the top
			k++;
		}
	// other wise do nothing
	}

	// Now the non-NULL PID should be at the top..
	pL->NumberofPIDs = k;
	return true;


}


bool CA75PesMaker::GetDefaultConfigStruct(void *pConfig)
{

	if((!m_Device_attached) ||(!m_IPAddressValid && !m_MACAddressValid)) return false;

	CNovraDevice::GetDefaultConfigStruct( pConfig );
 
  	((A75PesMaker_NetSetting *)pConfig)->UniCastStatusDestUDP =  m_Device_Status_Port;

	((A75PesMaker_NetSetting *)pConfig)->IGMP_Enable = 0;
  	
  	return true;	

}


bool CA75PesMaker::ParseStatusPacket()
{
		
	BYTE temp[4];

	// If this function is called, the status Structure has been filled with data received by Status Listner.
	// We need to verify that the packet is valid, and that it matches the device to be valid.

	if ( !CNovraDevice::ParseStatusPacket() ) return false;

	((A75PesMakerStatusPacket *)m_StatusWrapper)->ntoh();

//	for(int i=0;i<MAX_SUPP_PIDS;i++)
//		(((A75PesMakerStatusPacket *)m_StatusWrapper)->PIDListPointer())[i] = ntohs((((A75PesMakerStatusPacket *)m_StatusWrapper)->PIDListPointer())[i]);

	if( ( (*(((A75PesMakerStatusPacket *)m_StatusWrapper)->DSPFirmWarePointer())) & 0xFF00) != 0x4400 )		
	 return false ;		/// if the device does not follow communication rev 3.0 it is not supported.

	if(m_Device_Type != A75PesMaker) return false;

	m_StatusListener.GetLastPacketSource(temp);

	memcpy(m_Device_IP_Address, temp,4); // Get the address from the Unit.

	return true;


}


bool CA75PesMaker::CreateNetConfigMsg( NetworkConfigPacket *network_config_packet_wrapper, NetSetting *pNetworkSetting, BYTE filter )
{    

	if ( CNovraDevice::CreateNetConfigMsg( network_config_packet_wrapper, pNetworkSetting, filter ) ) {

		*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->StatusIntervalPointer()) = 5;
		*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer()) = filter;

		return true;

	} else {

		return false;

	}

}


bool CA75PesMaker::ConfirmDeviceSpecificNetworkSettings( StatusPacket *status_wrapper, NetworkConfigPacket *network_config_packet_wrapper )
{

	if ( ( (*(((CommonStatusPacket *)m_StatusWrapper)->FilterPointer())) &
		   (*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer())) ) ==
		 (*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer())) ) 
		return true;
	else
		return false;

}
