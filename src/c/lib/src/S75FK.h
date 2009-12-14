// S75FK.h: interface for the CS75FK class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __S75FK
#define __S75FK

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Password.h"
#include "SatelliteDevice.h"
#include "S75FKPackets.h"
#include "PesMakerPIDList.h"
#include "S75Plus.h"



class CS75FK : public CS75Plus
{
public:
	int GetStatus(void * Container);
	int PollStatus( StatusPacket *pStatus, int timeout=2000 );
	int SetFixedKeys(LPBYTE KeyTable);
	int GetFixedKeys(LPBYTE KeyTable);

	CS75FK( bool remote=false );
	virtual ~CS75FK();

protected:
	int SendFixedKeys( S75FKeys_MSG  *FKMsg, int size, int max_fixed_keys );
	bool ParseStatusPacket();

	S75FK_KeyList	*m_pS75FK_keys;

};


#endif
