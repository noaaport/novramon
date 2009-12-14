// DeviceIdentifier.cpp: implementation of the CDeviceIdentifier class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "DeviceIdentifier.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeviceIdentifier::CDeviceIdentifier()
{
#ifdef JSAT
	m_StatusPacketSize = sizeof(S75CAStatus_MSG);		// The longest status message
#else	
	m_StatusPacketSize = sizeof(Status_MSG);		// The longest status message
#endif
	m_Device_Type = S75_ANY;  // when we instantiate this class, the device type will be enforced on attachment.
	m_pStatusBuffer = (LPBYTE) &m_Status;	// Initialize the Member variable of the parent.
	memset((void *)(&m_Status),0,m_StatusPacketSize);
	//printf ("\n CHILD Constructor \n");
	
}

CDeviceIdentifier::~CDeviceIdentifier()
{

}

bool CDeviceIdentifier::ParseStatusPacket()
{
	m_pStatusV2 = (S75D_V2_Status_MSG *) (&m_Status) ;
	
	if(((m_pStatusV2->DSPFirmWare & 0xff) ==0x32)){		// The Inverted version of the DSP firmware signature in V 2.1
		m_Device_Type = S75D_V21;
		m_StatusListener.GetLastPacketSource(m_Device_IP_Address);
		memcpy(m_Device_MAC_Address, m_pStatusV2->ReceiverMAC, 6);
		return true;
	}
	m_pStatusV3 = (S75D_V3_Status_MSG *) (&m_Status) ;
	if((m_pStatusV3->DSPFirmWare&0xff) == 0x33){
		m_Device_Type = S75D_V3;
		m_StatusListener.GetLastPacketSource(m_Device_IP_Address);
		memcpy(m_Device_MAC_Address, m_pStatusV3->ReceiverMAC, 6);
		return true;
	}

	m_pStatusA75 = (A75Status_MSG *) (&m_Status);
	if((m_pStatusA75->DSPFirmWare & 0xFF) == 0x43){
		m_Device_Type = A75;
		m_StatusListener.GetLastPacketSource(m_Device_IP_Address);
		memcpy(m_Device_MAC_Address, m_pStatusA75->ReceiverMAC, 6);
		return true;
	
	}

	m_pStatusPesMaker = (PesMakerStatus_MSG*) (&m_Status) ;
	if((m_pStatusPesMaker->DSPFirmWare&0xff) == 0x34){
		m_Device_Type = PesMaker;
		m_StatusListener.GetLastPacketSource(m_Device_IP_Address);
		memcpy(m_Device_MAC_Address, m_pStatusPesMaker->ReceiverMAC, 6);
		return true;
	}
	
	m_pStatusA75PesMaker = (A75PesMakerStatus_MSG *) (&m_Status);
	if((m_pStatusA75PesMaker->DSPFirmWare & 0xFF) == 0x44){
		m_Device_Type = A75PesMaker;
		m_StatusListener.GetLastPacketSource(m_Device_IP_Address);
		memcpy(m_Device_MAC_Address, m_pStatusA75PesMaker->ReceiverMAC, 6);
		return true;
	
	}

	m_pStatusS75CA = (S75CAStatus_MSG*)(&m_Status);
//	if((m_pStatusS75CA->DSPFirmWare&0xff) == 0x54){
	if(((m_pStatusS75CA->DSPFirmWare&0xff) == 0x66)||((m_pStatusS75CA->DSPFirmWare&0xff) == 0x76)){
//	if( ( m_pStatusS75CA->DSPFirmWare & 0xff ) == 0x66 ){
		m_Device_Type = S75CAPRO;
		m_StatusListener.GetLastPacketSource(m_Device_IP_Address);
		memcpy(m_Device_MAC_Address, m_pStatusS75CA->ReceiverMAC, 6);
		return true;
	}
	if( ( m_pStatusS75CA->DSPFirmWare & 0xff ) == 0x67 ){
		m_Device_Type = S75CA;
		m_StatusListener.GetLastPacketSource(m_Device_IP_Address);
		memcpy(m_Device_MAC_Address, m_pStatusS75CA->ReceiverMAC, 6);
		return true;
	}
	if( ( m_pStatusS75CA->DSPFirmWare & 0xff ) == 0x68 ){
		m_Device_Type = S75PLUSPRO;
		m_StatusListener.GetLastPacketSource(m_Device_IP_Address);
		memcpy(m_Device_MAC_Address, m_pStatusS75CA->ReceiverMAC, 6);
		return true;
	}
	if( ( m_pStatusS75CA->DSPFirmWare & 0xff ) == 0x69 ) {
		m_Device_Type = S75PLUS;
		m_StatusListener.GetLastPacketSource(m_Device_IP_Address);
		memcpy(m_Device_MAC_Address, m_pStatusS75CA->ReceiverMAC, 6);
		return true;
	}
	if( ( m_pStatusS75CA->DSPFirmWare & 0xff ) == 0x6A ){
		m_Device_Type = S75FKPRO;
		m_StatusListener.GetLastPacketSource(m_Device_IP_Address);
		memcpy(m_Device_MAC_Address, m_pStatusS75CA->ReceiverMAC, 6);
		return true;
	}
	if( ( m_pStatusS75CA->DSPFirmWare & 0xff ) == 0x6B ){
		m_Device_Type = S75FK;
		m_StatusListener.GetLastPacketSource(m_Device_IP_Address);
		memcpy(m_Device_MAC_Address, m_pStatusS75CA->ReceiverMAC, 6);
		return true;
	}
	if( ( m_pStatusS75CA->DSPFirmWare & 0xff ) == 0x6C ) {
		m_Device_Type = S75IPREMAP;
		m_StatusListener.GetLastPacketSource(m_Device_IP_Address);
		memcpy(m_Device_MAC_Address, m_pStatusS75CA->ReceiverMAC, 6);
		return true;
	}
	
#ifdef JSAT
	m_pStatusJLEO = (S75JLEO_Status_MSG *)(&m_Status);
	if((m_pStatusJLEO->DSPFirmWare&0xff) == 0x55){
		m_Device_Type = S75_JLEO;
		m_StatusListener.GetLastPacketSource(m_Device_IP_Address);
		memcpy(m_Device_MAC_Address, m_pStatusJLEO->ReceiverMAC, 6);
		return true;
	}
#endif

	m_Device_Type = S75_UNKNOWN;
	return false;


}

void CDeviceIdentifier::GetdeviceEntry(Novra_DeviceEntry *pDev)
{
	pDev->DeviceType = m_Device_Type;
	pDev->StatusPort = m_Device_Status_Port;
	memcpy(pDev->DeviceMAC, m_Device_MAC_Address, 6);
	memcpy(pDev->DeviceIP, m_Device_IP_Address, 4);
}

DWORD CDeviceIdentifier::GetStatusUpTime()
{
	if(m_StatusListener.IsListening())
		return (time(NULL) - m_Status_Listen_Start_Time) ;
	else 
		return ERR_INVALID_TIME; 

}
