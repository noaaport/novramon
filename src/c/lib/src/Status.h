// Status.h: interface for the Status class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATUS_H__2B9989A8_2BCD_47CE_9390_D4ED9CB0119B__INCLUDED_)
#define AFX_STATUS_H__2B9989A8_2BCD_47CE_9390_D4ED9CB0119B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "s75.h"


class Status  
{
public:
	Status();
	virtual ~Status();

	virtual BYTE* UIDPointer()=0;
	virtual BYTE* FirmWareVerPointer()=0;
	virtual BYTE* FirmWareRevPointer()=0;
	virtual WORD* RF_FIRMWAREPointer()=0;
	virtual BYTE* DefaultGatewayIPPointer()=0;
	virtual BYTE* DestinationIPPointer()=0;
	virtual BYTE* SubnetMaskPointer()=0;
	virtual BYTE* ReceiverMACPointer()=0;
	virtual BYTE* ReceiverIPPointer()=0;
	virtual WORD* DestinationUDPPointer()=0;
	virtual WORD* EthernetPacketDroppedPointer()=0;
	virtual WORD* EthernetReceivePointer()=0;
	virtual WORD* EthernetTransmitPointer()=0;
	virtual WORD* EthernetTransmitErrorPointer()=0;
	virtual WORD* WRESERVEDPointer()=0;
	virtual S75D_PIDList* PIDListPointer()=0;
};

#endif // !defined(AFX_STATUS_H__2B9989A8_2BCD_47CE_9390_D4ED9CB0119B__INCLUDED_)
