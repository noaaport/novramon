// PesMaker.h: interface for the CPesMaker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PESMAKER_H__40DF9DA1_7E57_4E6C_B166_9276FAAE7ECA__INCLUDED_)
#define AFX_PESMAKER_H__40DF9DA1_7E57_4E6C_B166_9276FAAE7ECA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SatelliteDevice.h"
#include "PesMakerPackets.h"
#include "PesMakerPIDList.h"


class CPesMaker : public CSatelliteDevice
{
public:
	int TuneDevice(void * TuningParameters);
	int SetPIDTable(LPBYTE PIDTable);
//	int ResetDevice();

	int GetStatus(void * Container);
	int GetPIDTable(BYTE * PIDTable);

	bool GetDefaultConfigStruct( void *pConfig);

	int ConfigureNetwork( void * pNetworkSetting);

	CPesMaker();
	virtual ~CPesMaker();

protected:
	PIDList* PIDListWrapper()	{ return new PesMakerPIDList( new PesMaker_PIDList ); }
	void TranscodePIDList( PIDSettings_MSG *PidSettingMSG );
	BYTE* DVBMACPointer() { return ((PesMakerStatusPacket *)m_StatusWrapper)->DVBMACPointer(); }
	bool CreateNetConfigMsg( NetworkConfigPacket *network_config_packet_wrapper, NetSetting *pNetworkSetting, BYTE filter );
	bool ConfirmDeviceSpecificNetworkSettings( StatusPacket *status_wrapper, NetworkConfigPacket *network_config_packet_wrapper );
	void CreateRFConfigMsg( SatelliteRFConfigPacket *rf_config_packet_wrapper, void *TuningParameters );

	bool TrimPIDList( LPBYTE PL, int Max);
	bool ParseStatusPacket();
	BYTE m_IGMP_Filter;
//	PesMakerStatus_MSG *m_StatusStructure;
//	PesMaker_Status		* m_pS75D_Status;		// pointers to the external structures used by the calling functions
	PesMaker_PIDList	* m_pS75D_PIDList;		// totally different from the structures used by the device ...
//	PesMaker_RFSetting	* m_pS75D_RFSetting;	// we need these constructs to so we can pass the data back in a different format
//	PesMaker_NetSetting * m_pS75D_NetSetting;

//	PesMaker_Status * m_pSStatus;
	

};

#endif // !defined(AFX_PESMAKER_H__40DF9DA1_7E57_4E6C_B166_9276FAAE7ECA__INCLUDED_)
