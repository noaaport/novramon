// S75_V2.h: interface for the CS75_V2 class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_S75_V2_H__73C2A014_78F1_421A_9DA6_4ADFEDEEB8C6__INCLUDED_)
//#define AFX_S75_V2_H__73C2A014_78F1_421A_9DA6_4ADFEDEEB8C6__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#include "SatelliteDevice.h"
#include "S75V2Packets.h"
#include "StandardPIDList.h"
#include "s75.h"



class CS75_V2 : public CSatelliteDevice
{
public:
	bool GetDefaultConfigStruct(void * pConfig);
	int GetStatus(void *Container);

	int GetPIDTable(LPBYTE PIDTable);
	int SetPIDTable(LPBYTE PIDTable);
//	int ResetDevice(void);
	int ConfigureNetwork(void * pNetworkSetting);
	int TuneDevice(void * TuningParameters);
	int ConfigureDVBMAC(LPBYTE DVBMAC) { return ERR_METHODNOTSUPPORTED; }
	bool CreateNetConfigMsg( NetworkConfigPacket *network_config_packet_wrapper, NetSetting *pNetworkSetting, BYTE filter );
	bool ConfirmDeviceSpecificNetworkSettings( StatusPacket *status_wrapper, NetworkConfigPacket *network_config_packet_wrapper );
	void CreateRFConfigMsg( SatelliteRFConfigPacket *rf_config_packet_wrapper, void *TuningParameters );

	CS75_V2();
	virtual ~CS75_V2();
	//int TuneDevice(void *TuningParameters);

protected:
	bool ParseStatusPacket();
	PIDList* PIDListWrapper()	{ return new StandardPIDList( new S75D_PIDList ); }
	void TranscodePIDList( PIDSettings_MSG *PidSettingMSG );

	BYTE m_PID_HW_Filter;
//	S75D_V2_Status_MSG *m_StatusStructure;
//	S75D_Status * m_pS75D_Status;		// pointers to the external structures used by the calling functions
	S75D_PIDList * m_pS75D_PIDList;		// totally different from the structures used by the device ...
//	S75D_RFSetting * m_pS75D_RFSetting;	// we need these constructs to so we can pass the data back in a different format
//	S75D_NetSetting * m_pS75D_NetSetting;

	
};

//#endif // !defined(AFX_S75_V2_H__73C2A014_78F1_421A_9DA6_4ADFEDEEB8C6__INCLUDED_)
