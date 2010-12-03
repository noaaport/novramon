// A75.h: interface for the CA75 class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_A75_H__5BC2EEC9_2319_4A45_B5D8_2009E7B0BE6C__INCLUDED_)
//#define AFX_A75_H__5BC2EEC9_2319_4A45_B5D8_2009E7B0BE6C__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000


#include "ATSCDevice.h"
#include "S75V2Packets.h"
#include "S75V3Packets.h"
#include "A75MSGS.h"
#include "StandardPIDList.h"
#include "StatusPacket.h"

class CA75 : public ATSCDevice  
{
public:
	int GetStatus(void *Container);
	int ConfigureNetwork(void * pNetworkSetting);
//	int ResetDevice(void);
	int SetPIDTable(LPBYTE PIDTable);
	int GetPIDTable(LPBYTE PIDTable);
	bool GetDefaultConfigStruct(void * pConfig);
	CA75();
	virtual ~CA75();

protected:
	bool ParseStatusPacket();
	PIDList* PIDListWrapper()	{ return new StandardPIDList( new S75D_PIDList ); }
	void TranscodePIDList( PIDSettings_MSG *PidSettingMSG );
	BYTE* DVBMACPointer() { return ((A75StatusPacket *)m_StatusWrapper)->DVBMACPointer(); }
	bool CreateNetConfigMsg( NetworkConfigPacket *network_config_packet_wrapper, NetSetting *pNetworkSetting, BYTE filter );
	bool ConfirmDeviceSpecificNetworkSettings( StatusPacket *status_wrapper, NetworkConfigPacket *network_config_packet_wrapper );

	BYTE m_IGMP_Filter;
//	A75Status_MSG  *m_StatusStructure;		// The Buffer that holds the status messages.
//	A75_Status * m_pA75_Status;		// pointers to the external structures used by the calling functions
	A75_PIDList * m_pA75_PIDList;		// totally different from the structures used by the device ...
//	A75_RFSetting * m_pA75_RFSetting;	// we need these constructs to so we can pass the data back in a different format
//	A75_NetSetting * m_pA75_NetSetting;


};

//#endif // !defined(AFX_A75_H__5BC2EEC9_2319_4A45_B5D8_2009E7B0BE6C__INCLUDED_)
