// S75CA.h: interface for the CS75CA class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_S75CA_H__730B9E62_1FFF_4ECE_9D21_4457A1116EF3__INCLUDED_)
#define AFX_S75CA_H__730B9E62_1FFF_4ECE_9D21_4457A1116EF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Password.h"
#include "SatelliteDevice.h"
#include "S75CAPackets.h"
#include "PesMakerPIDList.h"
#include "S75Plus.h"



class CS75CA : public CS75Plus
{
public:

	int GetStatus(void * Container);
	int PollStatus( StatusPacket *pStatus, int timeout=2000 );
	int SetProgramTable(LPBYTE ProgramTable);
	int GetProgramTable(LPBYTE ProgramTable);

	int ResetCAM();
	int setCAMWatchdogTimeout(WORD timeout);
	int getCAMWatchdogTimeout(WORD *timeout);

	CS75CA( bool remote=false );
	virtual ~CS75CA();

protected:
	int SendCAMWatchdogTimeout( S75CA_CAM_Watchdog_Timeout_MSG  *CAMWatchdogTimeoutMsg, int size );
	int SendPrograms( S75CAProgramSettings_MSG  *ProgramSettingMsg, int size, int max_programs );

	bool ParseStatusPacket();

	S75CA_ProgramList	* m_pS75CA_ProgramList;		// totally different from the structures used by the device ...

};


#endif // !defined(AFX_S75CA_H__730B9E62_1FFF_4ECE_9D21_4457A1116EF3__INCLUDED_)

