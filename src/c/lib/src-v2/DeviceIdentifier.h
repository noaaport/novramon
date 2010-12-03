// DeviceIdentifier.h: interface for the CDeviceIdentifier class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_DEVICEIDENTIFIER_H__35C128D4_5AA4_43FF_974E_8824601E3753__INCLUDED_)
//#define AFX_DEVICEIDENTIFIER_H__35C128D4_5AA4_43FF_974E_8824601E3753__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

#include "NovraDevice.h"
#include "S75V2Packets.h"
#include "S75V3Packets.h"
#include "PesMakerPackets.h"
#include "A75PesMakerPackets.h"
#include "S75CAPackets.h"
#ifdef JSAT
	#include "S75JLEOPackets.h"
#endif
#include "A75MSGS.h"
#include <time.h>

class CDeviceIdentifier : public CNovraDevice  
{
public:
	DWORD GetStatusUpTime();
	void GetdeviceEntry(Novra_DeviceEntry * pDev); // This function returns the discovered device information
	
	CDeviceIdentifier();
	virtual ~CDeviceIdentifier();

protected:
	PIDList* PIDListWrapper() { return NULL; }
	void TranscodePIDList( PIDSettings_MSG *PidSettingsMSG ) {}
	bool ParseStatusPacket();
	bool ConfirmDeviceSpecificNetworkSettings( StatusPacket *status_wrapper, NetworkConfigPacket *network_config_packet_wrapper ) { return false; }
	bool ConfirmDeviceSpecificRFSettings( StatusPacket *status_wrapper, RFConfigPacket *rf_config_packet_wrapper ) { return false; }

	S75D_V2_Status_MSG* m_pStatusV2;
	S75D_V3_Status_MSG* m_pStatusV3;
	A75Status_MSG*	m_pStatusA75;
	A75PesMakerStatus_MSG*	m_pStatusA75PesMaker;
	PesMakerStatus_MSG* m_pStatusPesMaker ;
	S75CAStatus_MSG* m_pStatusS75CA ;
#ifdef JSAT
	S75JLEO_Status_MSG* m_pStatusJLEO;
	BYTE m_StatusBuffer[sizeof(S75JLEO_Status_MSG)];
#else
	BYTE m_StatusBuffer[sizeof(S75D_V2_Status_MSG)];
#endif
};

//#endif // !defined(AFX_DEVICEIDENTIFIER_H__35C128D4_5AA4_43FF_974E_8824601E3753__INCLUDED_)
