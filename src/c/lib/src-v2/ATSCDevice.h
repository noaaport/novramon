// ATSCDevice.h: interface for the ATSCDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATSCDEVICE_H__C3A3C1E2_B950_4FC1_A0E4_AD7D8C6B302F__INCLUDED_)
#define AFX_ATSCDEVICE_H__C3A3C1E2_B950_4FC1_A0E4_AD7D8C6B302F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NovraDevice.h"
#include "ATSCStatus.h"


class ATSCDevice : public CNovraDevice  
{
public:
	ATSCDevice();
	virtual ~ATSCDevice();
	int TuneDevice(void * TuningParameters);
protected:
//	bool ParseStatusPacket();
	int GetStatus( ATSCStatus *pStatus, bool forward_raw = false, int max_pids = 32 );
	bool ConfirmDeviceSpecificRFSettings( StatusPacket *status_wrapper, RFConfigPacket *rf_config_packet_wrapper ) { return true; };

//	ATSCStatus_MSG *m_ATSCStatus;

};

#endif // !defined(AFX_ATSCDEVICE_H__C3A3C1E2_B950_4FC1_A0E4_AD7D8C6B302F__INCLUDED_)
