// S75IPRemap.h: interface for the CS75IPRemap class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __S75IPREMAP
#define __S75IPREMAP

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Password.h"
#include "SatelliteDevice.h"
#include "S75FKPackets.h"
#include "PesMakerPIDList.h"
#include "S75Plus.h"
#include "S75IPRemapPackets.h"



class CS75IPRemap : public CS75Plus
{
public:
	int GetStatus(void * Container);
	int PollStatus( StatusPacket *pStatus, int timeout=2000 );

	CS75IPRemap( bool remote=false );
	virtual ~CS75IPRemap();

	int GetIPRemapTable(LPBYTE IPRemapTable);
	int SetIPRemapTable(LPBYTE IPRemapTable);

protected:

	int SendIPRemap( S75IPRemap_IPRemap_MSG *IPRemapMsg, int size );
	
	bool ParseStatusPacket();
	S75IPRemap_RemapTable * m_IPRemapTable;


};


#endif
