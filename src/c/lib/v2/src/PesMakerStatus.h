// PesMakerStatus.h: interface for the PesMakerStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PESMAKERSTATUS_H__92F817B3_5EEA_4E72_AFA8_FC5562B020B0__INCLUDED_)
#define AFX_PESMAKERSTATUS_H__92F817B3_5EEA_4E72_AFA8_FC5562B020B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SatelliteStatus.h"

class PesMakerStatus : public SatelliteStatus  
{
public:
	PesMakerStatus( void *Container );
	virtual ~PesMakerStatus();

	// Status (base class) overrides

	BYTE* UIDPointer()						{ return status->UID; }
	BYTE* FirmWareVerPointer()				{ return &(status->FirmWareVer); }
	BYTE* FirmWareRevPointer()				{ return &(status->FirmWareRev); }
	WORD* RF_FIRMWAREPointer()				{ return &(status->RF_FIRMWARE); }
	BYTE* DefaultGatewayIPPointer()			{ return status->DefaultGatewayIP; }
	BYTE* DestinationIPPointer()			{ return status->UniCastStatusDestIP; }
	BYTE* SubnetMaskPointer()				{ return status->SubnetMask; }
	BYTE* ReceiverMACPointer()				{ return status->ReceiverMAC; }
	BYTE* ReceiverIPPointer()				{ return status->ReceiverIP; }
	WORD* DestinationUDPPointer()			{ return &(status->UniCastStatusDestUDP); }
	WORD* EthernetPacketDroppedPointer()	{ return &(status->EthernetPacketDropped); }
	WORD* EthernetReceivePointer()			{ return &(status->EthernetReceive); }
	WORD* EthernetTransmitPointer()			{ return &(status->EthernetTransmit); }
	WORD* EthernetTransmitErrorPointer()	{ return &(status->EthernetTransmitError); }
	WORD* WRESERVEDPointer()				{ return status->WRESERVED; }
	S75D_PIDList* PIDListPointer()			{ return &(status->PIDList); }

	// SatelliteStatus (parent class) overrides

    BYTE* RFStatusValidPointer()			{ return &(status->RFStatusValid); }
	float* SymbolRatePointer()				{ return &(status->SymbolRate); }
	float* FrequencyPointer()				{ return &(status->Frequency); }
	BYTE* PowerPointer()					{ return &(status->Power); }
	BYTE* PolarityPointer()					{ return &(status->Polarity); }
	BYTE* BandPointer()						{ return &(status->Band); }
	BYTE* ViterbiRatePointer()				{ return &(status->ViterbiRate); }
	BYTE* SignalLockPointer()				{ return &(status->SignalLock); }
	BYTE* DataLockPointer()					{ return &(status->DataLock); }
	BYTE* LNBFaultPointer()					{ return &(status->LNBFault); }
	BYTE* UncorrectablesPointer()			{ return &(status->Uncorrectables); }

private:
	PesMaker_Status *status;

};

#endif // !defined(AFX_PESMAKERSTATUS_H__92F817B3_5EEA_4E72_AFA8_FC5562B020B0__INCLUDED_)
