// ATSCStatus.h: interface for the ATSCStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATSCSTATUS_H__7F1D3F4A_6E4B_414F_904D_8AEB19145B15__INCLUDED_)
#define AFX_ATSCSTATUS_H__7F1D3F4A_6E4B_414F_904D_8AEB19145B15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Status.h"

class ATSCStatus : public Status  
{
public:
	ATSCStatus();
	virtual ~ATSCStatus();

    virtual BYTE* RFStatusValidPointer()=0;
	virtual WORD* ChannelNumberPointer()=0;
	virtual WORD* FEC_CorrPointer()=0;
	virtual WORD* FEC_UnCorrPointer()=0;
	virtual WORD* FEC_UnErrPointer()=0;
	virtual float* Carrier_OffPointer()=0;
	virtual float* IF_AGCPointer()=0;
	virtual float* RFAGCPointer()=0;
	virtual BYTE* DeModStatsPointer()=0;
	virtual BYTE* VSBModePointer()=0;
	virtual float* SNR_AEPointer()=0;
	virtual float* SNR_BEPointer()=0;
	virtual BYTE* SER_10Pointer()=0;
	virtual BYTE* SER_1Pointer()=0;
	virtual bool* FieldLockedPointer()=0;
	virtual bool* EqualizerStatePointer()=0;
	virtual BYTE* ChannelStatePointer()=0;
	virtual bool* NTSC_FilterPointer()=0;
	virtual bool* InverseSpectrumPointer()=0;
	virtual BYTE* DVB_MACADDRESSPointer()=0;
	virtual BYTE* IGMP_EnablePointer()=0;

};

#endif // !defined(AFX_ATSCSTATUS_H__7F1D3F4A_6E4B_414F_904D_8AEB19145B15__INCLUDED_)
