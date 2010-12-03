// SatelliteStatus.h: interface for the SatelliteStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SATELLITESTATUS_H__32B0AFC8_A6B1_4A78_8B58_4CDF3F5D16BF__INCLUDED_)
#define AFX_SATELLITESTATUS_H__32B0AFC8_A6B1_4A78_8B58_4CDF3F5D16BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Status.h"

class SatelliteStatus : public Status  
{
public:
	SatelliteStatus();
	virtual ~SatelliteStatus();
    virtual BYTE* RFStatusValidPointer()=0;
	virtual float* SymbolRatePointer()=0;
	virtual float* FrequencyPointer()=0;
	virtual BYTE* PowerPointer()=0;
	virtual BYTE* PolarityPointer()=0;
	virtual BYTE* BandPointer()=0;
	virtual BYTE* ViterbiRatePointer()=0;
	virtual BYTE* SignalLockPointer()=0;
	virtual BYTE* DataLockPointer()=0;
	virtual BYTE* LNBFaultPointer()=0;
	virtual BYTE* UncorrectablesPointer()=0;
};

#endif // !defined(AFX_SATELLITESTATUS_H__32B0AFC8_A6B1_4A78_8B58_4CDF3F5D16BF__INCLUDED_)
