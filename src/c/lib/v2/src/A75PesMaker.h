// A75PesMaker.h: interface for the CA75PesMaker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_A75PESMAKER_H__952852CE_8DEB_4C69_A21B_22BE093742B4__INCLUDED_)
#define AFX_A75PESMAKER_H__952852CE_8DEB_4C69_A21B_22BE093742B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ATSCDevice.h"
#include "A75PesMakerPackets.h"
#include "PesMakerPIDList.h"


class CA75PesMaker : public ATSCDevice  
{
public:

	CA75PesMaker();
	virtual ~CA75PesMaker();

	int  GetStatus( void *Container );

	int  ConfigureNetwork( void *pNetworkSetting );

//	int  ResetDevice( void );

	int  SetPIDTable( LPBYTE PIDTable );

	int  GetPIDTable( LPBYTE PIDTable );

	bool GetDefaultConfigStruct( void *pConfig );

protected:

	bool TrimPIDList( LPBYTE PL, int Max);

	bool ParseStatusPacket();

	PIDList* PIDListWrapper() { return new PesMakerPIDList( new PesMaker_PIDList ); }

	void TranscodePIDList( PIDSettings_MSG *PidSettingMSG );

	BYTE* DVBMACPointer() { return ((A75PesMakerStatusPacket *)m_StatusWrapper)->DVBMACPointer(); }

	bool CreateNetConfigMsg( NetworkConfigPacket *network_config_packet_wrapper, NetSetting *pNetworkSetting, BYTE filter );

	bool ConfirmDeviceSpecificNetworkSettings( StatusPacket *status_wrapper, NetworkConfigPacket *network_config_packet_wrapper );


	BYTE m_IGMP_Filter;
//	A75PesMakerStatus_MSG	*m_StatusStructure;	// The Buffer that holds the status messages.
//	A75PesMaker_Status		*m_pA75_Status;		// pointers to the external structures used by the calling functions
	A75PesMaker_PIDList		*m_pA75_PIDList;	// totally different from the structures used by the device ...
//	A75PesMaker_RFSetting	*m_pA75_RFSetting;	// we need these constructs to so we can pass the data back in a different format
//	A75PesMaker_NetSetting	*m_pA75_NetSetting;

};

#endif // !defined(AFX_A75PESMAKER_H__952852CE_8DEB_4C69_A21B_22BE093742B4__INCLUDED_)


