// S75V3Status.h: interface for the S75V3Status class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_S75V3STATUS_H__9976A67F_1CB5_4981_86A9_CB38C4BB3A2E__INCLUDED_)
#define AFX_S75V3STATUS_H__9976A67F_1CB5_4981_86A9_CB38C4BB3A2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SatelliteStatus.h"

class S75V3Status : public SatelliteStatus  
{
public:
	S75V3Status( void *Container );
	virtual ~S75V3Status();

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

protected:
	S75D_Status_V3 *status;
};

#endif // !defined(AFX_S75V3STATUS_H__9976A67F_1CB5_4981_86A9_CB38C4BB3A2E__INCLUDED_)
