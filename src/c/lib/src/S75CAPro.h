// S75CAPro.h: interface for the CS75CAPro class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __S75CAPRO
#define __S75CAPRP


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Password.h"
#include "SatelliteDevice.h"
#include "S75CAProPackets.h"
#include "PesMakerPIDList.h"
#include "S75CA.h"



class CS75CAPro : public CS75CA
{
public:
	int SetPAT(LPBYTE PAT);
	int GetPAT(LPBYTE PAT);
	int SetTraps(LPBYTE Traps);
	int GetTraps(LPBYTE Traps);
	int GetStatus(void * Container);
	int PollStatus( StatusPacket *pStatus, int timeout=2000 );

	CS75CAPro( bool remote=false );
	virtual ~CS75CAPro();

protected:
	int SendPAT( S75CAProPAT_MSG  *PATMsg, int size, int max_programs );
	bool ParseStatusPacket();

	S75_TrapSetting_JLEO * m_pS75CAPro_TrapSetting;
	S75CAPro_PAT	*m_pS75CAPro_PAT;
	S75CAPro_TrapSetting *m_pS75CAPro_Traps;

};


#endif

