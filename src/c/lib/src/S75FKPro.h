// S75FKPro.h: interface for the CS75FKPro class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __S75FKPRO
#define __S75FKPRO


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Password.h"
#include "SatelliteDevice.h"
#include "S75FKProPackets.h"
#include "PesMakerPIDList.h"
#include "S75FK.h"



class CS75FKPro : public CS75FK
{
public:
	int SetPAT(LPBYTE PAT);
	int GetPAT(LPBYTE PAT);
	int SetTraps(LPBYTE Traps);
	int GetTraps(LPBYTE Traps);
	int GetStatus(void * Container);
	int PollStatus( StatusPacket *pStatus, int timeout=2000 );

	CS75FKPro( bool remote=false );
	virtual ~CS75FKPro();

protected:
	int SendPAT( S75FKProPAT_MSG  *PATMsg, int size, int max_programs );
	bool ParseStatusPacket();

	S75_TrapSetting_JLEO * m_pS75FKPro_TrapSetting;
	S75FKPro_PAT	*m_pS75FKPro_PAT;
	S75FKPro_TrapSetting *m_pS75FKPro_Traps;

};


#endif

