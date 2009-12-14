// A75PesMakerStatus.h: interface for the A75PesMakerStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_A75PESMAKERSTATUS_H__7CF4AB59_140C_4596_A4E9_60CAD03A5DBE__INCLUDED_)
#define AFX_A75PESMAKERSTATUS_H__7CF4AB59_140C_4596_A4E9_60CAD03A5DBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ATSCStatus.h"
#include "s75.h"

class A75PesMakerStatus : public ATSCStatus  
{
public:

	A75PesMakerStatus( void *Container );
	virtual ~A75PesMakerStatus();

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

	BYTE* RFStatusValidPointer()			{ return &(status->RFStatusValid); }
	WORD* ChannelNumberPointer()			{ return &(status->ChannelNumber); }
	BYTE* DeModStatsPointer()				{ return &(status->DeModStats); }
	BYTE* VSBModePointer()					{ return &(status->VSBMode); }
	BYTE* SER_10Pointer()					{ return &(status->SER_10); }
	BYTE* SER_1Pointer()					{ return &(status->SER_1); }
	float* SNR_BEPointer()					{ return &(status->SNR_BE); }
	float* SNR_AEPointer()					{ return &(status->SNR_AE); }
	float* Carrier_OffPointer()				{ return &(status->Carrier_Off); }
	float* IF_AGCPointer()					{ return &(status->IF_AGC); }
	float* RFAGCPointer()					{ return &(status->RFAGC); }
	WORD* FEC_UnErrPointer()				{ return &(status->FEC_UnErr); }
	WORD* FEC_CorrPointer()					{ return &(status->FEC_Corr); }
	WORD* FEC_UnCorrPointer()				{ return &(status->FEC_UnCorr); }
	bool* FieldLockedPointer()				{ return &(status->FieldLocked); }
	bool* EqualizerStatePointer()			{ return &(status->EqualizerState); }
	BYTE* ChannelStatePointer()				{ return &(status->ChannelState); }
	bool* NTSC_FilterPointer()				{ return &(status->NTSC_Filter); }
	bool* InverseSpectrumPointer()			{ return &(status->InverseSpectrum); }
	BYTE* DVB_MACADDRESSPointer()			{ return status->DVB_MACADDRESS; }

	BYTE* IGMP_EnablePointer()				{ return &(status->IGMP_Enable); }

private:

	A75PesMaker_Status *status;

};

#endif // !defined(AFX_A75PESMAKERSTATUS_H__7CF4AB59_140C_4596_A4E9_60CAD03A5DBE__INCLUDED_)

