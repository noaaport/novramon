// S75Plus.h: interface for the CS75Plus class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_S75PLUS_H__730B9E62_1FFF_4ECE_9D21_4457A1116EF3__INCLUDED_)
//#define AFX_S75CA_H__730B9E62_1FFF_4ECE_9D21_4457A1116EF3__INCLUDED_
#ifndef __S75PLUS
#define __S75PLUS

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Password.h"
#include "SatelliteDevice.h"
#include "S75PlusPackets.h"
#include "PesMakerPIDList.h"



class CS75Plus : public CSatelliteDevice
{
public:
	int TuneDevice(void * TuningParameters);
	int ResetDevice();

	int GetStatus(void * Container);
	int PollStatus(void *Container, int timeout=2000 );
	int PollStatus( StatusPacket *pStatus, int timeout=2000 );
	int GetPIDTable(BYTE * PIDTable);
	int SetPIDTable(LPBYTE PIDTable);
//	int SetProgramTable(LPBYTE ProgramTable);
//	int GetProgramTable(LPBYTE ProgramTable);
//	int SetPAT(LPBYTE PAT);
//	int GetPAT(LPBYTE PAT);
//	int SetFixedKeys(LPBYTE KeyTable);
//	int GetFixedKeys(LPBYTE KeyTable);
//	int SetTraps(LPBYTE Traps);
//	int GetTraps(LPBYTE Traps);

	bool GetDefaultConfigStruct( void *pConfig);

	int ConfigureNetwork( void * pNetworkSetting);

	int ChangePassword(char password[8], unsigned char key[16]);
	int SpecifyPassword( char password[8] );

	CS75Plus( bool remote=false );
	virtual ~CS75Plus();

protected:
	PIDList* PIDListWrapper()	{ return new PesMakerPIDList( new S75Plus_PIDList ); }
	void TranscodePIDList( PIDSettings_MSG *PidSettingMSG );
//	int SendPrograms( S75PlusProgramSettings_MSG  *ProgramSettingMsg, int size, int max_programs );
//	int SendPAT( S75PlusPAT_MSG  *PATMsg, int size, int max_programs );
//	int SendFixedKeys( S75FKeys_MSG  *FKMsg, int size, int max_fixed_keys );
	bool CreateNetConfigMsg( NetworkConfigPacket *network_config_packet_wrapper, NetSetting *pNetworkSetting, BYTE filter );
	bool ConfirmDeviceSpecificNetworkSettings( StatusPacket *status_wrapper, NetworkConfigPacket *network_config_packet_wrapper );
	void CreateRFConfigMsg( S75PlusRFConfigPacket *rf_config_packet_wrapper, void *TuningParameters );
	void TranscodeStatus( S75Plus_Status *response, S75PlusStatusPacket *status_packet_wrapper );

	bool TrimPIDList( LPBYTE PL, int Max);
	bool ParseStatusPacket();
	BYTE m_IGMP_Filter;
	pstruct m_sec;
	S75Plus_RFSetting * m_pS75Plus_RFSetting;	// we need these constructs to so we can pass the data back in a different format
//	S75_TrapSetting_JLEO * m_pS75Plus_TrapSetting;
	S75Plus_PIDList	* m_pS75Plus_PIDList;		// totally different from the structures used by the device ...
//	S75Plus_ProgramList	* m_pS75Plus_ProgramList;		// totally different from the structures used by the device ...
//	S75PlusPro_PAT	*m_pS75Plus_PAT;
//	S75FK_KeyList	*m_pS75FK_keys;
//	S75PlusPro_TrapSetting *m_pS75Plus_Traps;
	

};


#endif // !defined(AFX_S75CA_H__730B9E62_1FFF_4ECE_9D21_4457A1116EF3__INCLUDED_)

