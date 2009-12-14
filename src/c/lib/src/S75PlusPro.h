// S75PlusPro.h: interface for the CS75PlusPro class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __S75PLUSPRO
#define __S75PLUSPRO


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Password.h"
#include "SatelliteDevice.h"
#include "S75PlusProPackets.h"
#include "PesMakerPIDList.h"
#include "S75Plus.h"



class CS75PlusPro : public CS75Plus
{
public:
	int SetPAT(LPBYTE PAT);
	int GetPAT(LPBYTE PAT);
	int SetTraps(LPBYTE Traps);
	int GetTraps(LPBYTE Traps);
	int GetStatus(void * Container);
	int PollStatus( StatusPacket *pStatus, int timeout=2000 );

	CS75PlusPro( bool remote=false );
	virtual ~CS75PlusPro();

protected:
	int SendPAT( S75PlusProPAT_MSG  *PATMsg, int size, int max_programs );
	bool ParseStatusPacket();

	S75_TrapSetting_JLEO * m_pS75PlusPro_TrapSetting;
	S75PlusPro_PAT	*m_pS75PlusPro_PAT;
	S75PlusPro_TrapSetting *m_pS75PlusPro_Traps;

};


#endif

