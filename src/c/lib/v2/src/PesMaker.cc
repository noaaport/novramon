// PesMaker.cpp: implementation of the CPesMaker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PesMaker.h"
#include "PesMakerStatus.h"

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

	const double  AGC[]        = { 254, 112, 105, 85 };  // Automatic Gain Control
	const double  PERCENTAGE[] = { 10, 65, 80, 100 };
	const int     SIZE_ARRAY   = 4;




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPesMaker::CPesMaker()
{
	m_StatusWrapper = (StatusPacket *)(new PesMakerStatusPacket( &m_Status ));
	m_StatusPacketSize = sizeof(PesMakerStatus_MSG);
	m_Device_Type = PesMaker;  // when we instantiate this class, the device type will be enforced on attachment.
	m_pStatusBuffer = (LPBYTE) &m_Status;	// Initialize the Member variable of the parent.



}

CPesMaker::~CPesMaker()
{

}


int CPesMaker::ConfigureNetwork(void *pNetworkSetting)
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

	if ( !CreateNetConfigMsg( (NetworkConfigPacket *)(&network_config_packet_wrapper), (NetSetting *)pNetworkSetting, 4*(((PesMaker_NetSetting *) pNetworkSetting)->IGMP_Enable) ) )
		return ERR_BAD_IP_SUBNET_COMBO;

	// we should Check the Status Port to Make sure it is in the auto detection List.
	
	if ( !CheckStatusPort( ((NetSetting *)pNetworkSetting)->StatusPort ) )	// Broadcast Status Port
		return ERR_BAD_STATUS_PORT;

	if ( !CheckStatusPort( ((NetSetting *)pNetworkSetting)->DestUDP ) )		// Unicast Status Port
		return ERR_BAD_STATUS_PORT;

	return SendNetworkConfig( (NetworkConfigPacket *)(&network_config_packet_wrapper) );

}


bool CPesMaker::GetDefaultConfigStruct(void *pConfig)
{

	if((!m_Device_attached) ||(!m_IPAddressValid && !m_MACAddressValid)) return false;

	CNovraDevice::GetDefaultConfigStruct( pConfig );
 
  	((PesMaker_NetSetting *)pConfig)->UniCastStatusDestUDP =  m_Device_Status_Port;

	((PesMaker_NetSetting *)pConfig)->IGMP_Enable = 0;
  	
  	return true;	

}


void CPesMaker::TranscodePIDList( PIDSettings_MSG *PidSettingMSG )
{

	for ( int i = 0; i < MAX_SUPP_PIDS; i++) {

		PidSettingMSG->PIDTableEntry[i].PID = ntohs(PidSettingMSG->PIDTableEntry[i].PID);
		PidSettingMSG->PIDTableEntry[i].DestinationUDP = ntohs(PidSettingMSG->PIDTableEntry[i].DestinationUDP);
		m_pS75D_PIDList->Entry[i] = PidSettingMSG->PIDTableEntry[i].PID & 0x1FFF;
		m_pS75D_PIDList->Control[i] = ((PidSettingMSG->PIDTableEntry[i].PID & 0xE000) >> 13) ;
		memcpy( m_pS75D_PIDList->ForwardInformation[i].Destination_IP, PidSettingMSG->PIDTableEntry[i].DestinationIP, 4);
		m_pS75D_PIDList->ForwardInformation[i].Destination_Port =  PidSettingMSG->PIDTableEntry[i].DestinationUDP ;

	}
	
	m_pS75D_PIDList->NumberofPIDs = MAX_SUPP_PIDS ;
	TrimPIDList((LPBYTE)  m_pS75D_PIDList, MAX_SUPP_PIDS) ;

}


int CPesMaker::GetPIDTable(BYTE * PIDTable)
{

	m_pS75D_PIDList = (PesMaker_PIDList *) PIDTable ;

	return GetPIDs( sizeof(PesMakerPIDSettings_MSG) );

}


bool CPesMaker::TrimPIDList(LPBYTE PL, int Max)
{
	PesMaker_PIDList *pL;

	int j=0,k=0,l=0,m=0;

	pL = (PesMaker_PIDList*) PL;

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


int CPesMaker::GetStatus(void *Container)
{
	PesMaker_Status *pStatus;
	WORD *PIDList = (WORD *)(m_StatusWrapper->PIDListPointer());


	if(!IsStatusValid())  return ERR_FAILED ; // the status isn't valid. 

	if(m_Device_Type != PesMaker) return ERR_DEVICE_TYPE_MISMATCH ;

	pStatus = (PesMaker_Status *)Container;

	CSatelliteDevice::GetStatus( (SatelliteStatus *)(&(PesMakerStatus( Container ))), true );

	pStatus->SignalStrength = CalculateSignalStrength( AGC, PERCENTAGE, SIZE_ARRAY );

	pStatus->VBER = CalculateVBER();

	memcpy(pStatus->DVB_MACADDRESS, ((PesMakerStatusPacket *)m_StatusWrapper)->DVBMACPointer(),6);

	memcpy(pStatus->Reserved, ((PesMakerStatusPacket *)m_StatusWrapper)->ReservedPointer(),8);

	pStatus->Long_Line_Comp = ((*(((PesMakerStatusPacket *)m_StatusWrapper)->LNBConfigurationPointer())) &0x10) >> 4;  // is Long Line Compa ON .

	pStatus->Hi_VoltageMode = ((*(((PesMakerStatusPacket *)m_StatusWrapper)->LNBConfigurationPointer())) &0x8) >> 3;  // is High Voltage On. 

	pStatus->IGMP_Enable = ((*(((PesMakerStatusPacket *)m_StatusWrapper)->FilterPointer())) & 0x04) >> 2;

	pStatus->EthernetTransmitError	= (*(((PesMakerStatusPacket *)m_StatusWrapper)->EthernetTXErrPointer()));

	pStatus->WRESERVED[1]	= (*(((PesMakerStatusPacket *)m_StatusWrapper)->FEL_LossPointer()));
	
	
	// Copy the PID list

	for ( int i = 0; i < MAX_SUPP_PIDS; i++ ) {

		pStatus->PIDList.Entry[i] = PIDList[i] & 0x1fff;

		pStatus->PIDList.Control[i] = (PIDList[i] & 0xE000) >> 13 ;	

	}

	TrimPIDList((LPBYTE)(&(pStatus->PIDList)), MAX_SUPP_PIDS );

	return ERR_SUCCESS ;

}



bool CPesMaker::ParseStatusPacket()
{
	BYTE temp[4];

	// If this function is called, the status Structure has been filled with data received by Status Listner.
	// We need to verify that the packet is valid, and that it matches the device to be valid.

	if ( !CNovraDevice::ParseStatusPacket() ) return false;

	((PesMakerStatusPacket *)m_StatusWrapper)->ntoh();

//	for(int i=0;i<MAX_SUPP_PIDS;i++)
//		(((PesMakerStatusPacket *)m_StatusWrapper)->PIDListPointer())[i] = ntohs((((PesMakerStatusPacket *)m_StatusWrapper)->PIDListPointer())[i]);

	if ( ( (*(((PesMakerStatusPacket *)m_StatusWrapper)->DSPFirmWarePointer())) & 0xFF00) != 0x3400 )			// Communication rev. 3.0
	 return false ;		/// if the device does not follow communication rev 3.0 it is not supported.

	if(m_Device_Type != PesMaker) return false;

	m_StatusListener.GetLastPacketSource(temp);

	memcpy(m_Device_IP_Address, temp,4); // Get the address from the Unit.

	return true;
}



int CPesMaker::SetPIDTable(LPBYTE PIDTable)
{
	int i;

	int last_error_code ;
	PesMakerPIDSettings_MSG PidSettingMsg;
	CommonPIDSettingsPacket pid_setting_packet_wrapper( &PidSettingMsg );

	
	m_pS75D_PIDList = (PesMaker_PIDList *) PIDTable ;
	m_PIDList = (PIDList *)(new PesMakerPIDList( (void *)PIDTable ));


    // Set the opcode in the PID table to Upload (0x0002)

    PidSettingMsg.Opcode = htons(0x0002);

    // Copy the PID table to download into the CPIDProgram object
	// Does The PID list have too many entries !!? 
	if(m_pS75D_PIDList->NumberofPIDs > MAX_SUPP_PIDS) return ERR_PID_TOOLONG ;

    for (i = 0; i < MAX_SUPP_PIDS; i++) {

		memcpy(PidSettingMsg.PIDTableEntry[i].DestinationIP, (m_pS75D_PIDList->ForwardInformation[i].Destination_IP), 4) ;

        PidSettingMsg.PIDTableEntry[i].DestinationUDP = m_pS75D_PIDList->ForwardInformation[i].Destination_Port;
		if(i<m_pS75D_PIDList->NumberofPIDs){
			PidSettingMsg.PIDTableEntry[i].PID = m_pS75D_PIDList->Entry[i] ;
			PidSettingMsg.PIDTableEntry[i].PID |= (m_pS75D_PIDList->Control[i] << 13);
		}else{
			PidSettingMsg.PIDTableEntry[i].PID = NULL_PID;
		}

    } // for i

	last_error_code = SendPIDs( (PIDSettingsPacket *)(&pid_setting_packet_wrapper), sizeof( PesMakerPIDSettings_MSG ), MAX_SUPP_PIDS, true );

	delete m_PIDList;

    return last_error_code;


}


void CPesMaker::CreateRFConfigMsg( SatelliteRFConfigPacket *rf_config_packet_wrapper, void *TuningParameters )
{

	CSatelliteDevice::CreateRFConfigMsg( rf_config_packet_wrapper, TuningParameters );

	(*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) = 0;

	if ( ((PesMaker_RFSetting *)TuningParameters)->band == LNB_HIGH )
		(*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 4;

    if ( ( ((PesMaker_RFSetting *)TuningParameters)->polarity == LNB_HORIZONTAL ) ||
		 ( ((PesMaker_RFSetting *)TuningParameters)->polarity == LNB_LEFT ) )
		(*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 2;

    if ( ((PesMaker_RFSetting *)TuningParameters)->powerOn )
		(*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 1;

	if ( ((PesMaker_RFSetting *)TuningParameters)->LongLineCompensation )
		(*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 16 ; // Shall we enable Long line Compensation  ?

	if ( ((PesMaker_RFSetting *)TuningParameters)->HighVoltage )
		(*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) += 8 ;  // Account for High Voltage Selection .

}


int CPesMaker::TuneDevice(void *TuningParameters)
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


bool CPesMaker::CreateNetConfigMsg( NetworkConfigPacket *network_config_packet_wrapper, NetSetting *pNetworkSetting, BYTE filter )
{    

	if ( CNovraDevice::CreateNetConfigMsg( network_config_packet_wrapper, pNetworkSetting, filter ) ) {

		*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->StatusIntervalPointer()) = 5;
		*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer()) = filter;

		return true;

	} else {

		return false;

	}

}



bool CPesMaker::ConfirmDeviceSpecificNetworkSettings( StatusPacket *status_wrapper, NetworkConfigPacket *network_config_packet_wrapper )
{

	if ( ( (*(((CommonStatusPacket *)m_StatusWrapper)->FilterPointer())) &
		   (*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer())) ) ==
		 (*(((CommonNetworkConfigPacket *)network_config_packet_wrapper)->FilterPointer())) ) 
		return true;
	else
		return false;

}
