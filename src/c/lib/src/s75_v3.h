// S75_V3.h: interface for the CS75_V3 class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_S75_V3_H__12828402_1462_4536_9485_39A39684D694__INCLUDED_)
//#define AFX_S75_V3_H__12828402_1462_4536_9485_39A39684D694__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#include "SatelliteDevice.h"
#include "S75V2Packets.h"
#include "S75V3Packets.h"
#include "StandardPIDList.h"
#include "s75.h"




class CS75_V3 : public CSatelliteDevice
{
public:
	bool GetDefaultConfigStruct(void * pConfig);
	int GetStatus(void *Container);
	int GetPIDTable(LPBYTE PIDTable);
	int SetPIDTable(LPBYTE PIDTable);
//	int ResetDevice(void);
	int TuneDevice(void * TuningParameters);
	int ConfigureNetwork(void * pNetworkSetting);
	CS75_V3();
	virtual ~CS75_V3();
protected:
	bool ParseStatusPacket();
	PIDList* PIDListWrapper()	{ return new StandardPIDList( new S75D_PIDList_V3 ); }
	void TranscodePIDList( PIDSettings_MSG *PidSettingMSG );
	BYTE* DVBMACPointer() { return ((S75V3StatusPacket *)m_StatusWrapper)->DVBMACPointer(); }
	bool CreateNetConfigMsg( NetworkConfigPacket *network_config_packet_wrapper, NetSetting *pNetworkSetting, BYTE filter );
	bool ConfirmDeviceSpecificNetworkSettings( StatusPacket *status_wrapper, NetworkConfigPacket *network_config_packet_wrapper );
	void CreateRFConfigMsg( SatelliteRFConfigPacket *rf_config_packet_wrapper, void *TuningParameters );

	BYTE m_IGMP_Filter;
//	S75D_V3_Status_MSG *m_StatusStructure;
//	S75D_Status_V3 * m_pS75D_Status;		// pointers to the external structures used by the calling functions
	S75D_PIDList_V3 * m_pS75D_PIDList;		// totally different from the structures used by the device ...
//	S75D_RFSetting_V3 * m_pS75D_RFSetting;	// we need these constructs to so we can pass the data back in a different format
//	S75D_NetSetting_V3 * m_pS75D_NetSetting;


};

//#endif // !defined(AFX_S75_V3_H__12828402_1462_4536_9485_39A39684D694__INCLUDED_)
