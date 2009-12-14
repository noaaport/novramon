// SatelliteDevice.h: interface for the CSatelliteDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SATELLITEDEVICE_H__17E7E1EE_C3F3_4586_80B6_5D31172445B4__INCLUDED_)
#define AFX_SATELLITEDEVICE_H__17E7E1EE_C3F3_4586_80B6_5D31172445B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NovraDevice.h"
#include "SatelliteStatus.h"

class CSatelliteDevice : public CNovraDevice  
{
public:
	CSatelliteDevice();
	virtual ~CSatelliteDevice();
protected:
	int SendRFConfig( SatelliteRFConfigPacket *satellite_rf_config_packet );
	int GetStatus( SatelliteStatus *pStatus, bool forward_raw = false, int max_pids = 32 );
	BYTE CalculateSignalStrength( const double AGC[], const double PERCENTAGE[], int num_entries );
	float CalculateVBER( int exp_sign = 1 );
//	bool ParseStatusPacket();
	void CreateRFConfigMsg( SatelliteRFConfigPacket *rf_config_packet_wrapper, void *TuningParameters );
	bool ConfirmDeviceSpecificRFSettings( StatusPacket *status_wrapper, RFConfigPacket *rf_config_packet_wrapper );
	bool CheckRFConfig( SatelliteRFConfigPacket *rf_config_packet_wrapper );

//	SatelliteStatus_MSG *m_SatStatus;
};

#endif // !defined(AFX_SATELLITEDEVICE_H__17E7E1EE_C3F3_4586_80B6_5D31172445B4__INCLUDED_)
