#ifdef JSAT

// s75_jleo.h: interface for the CS75_JLEO class.
//
//////////////////////////////////////////////////////////////////////

#include "SatelliteDevice.h"
#include "S75V2Packets.h"
#include "S75V3Packets.h"
#include "S75JLEOPackets.h"
#include "Password.h"
#include "StandardPIDList.h"




class CS75_JLEO : public CSatelliteDevice  
{
public:
	bool GetDefaultConfigStruct(void * pConfig);
	int GetStatus(void *Container);
	int PollStatus(void *Container, int timeout=2000 );
	int PollStatus( StatusPacket *pStatus, int timeout=2000 );
#ifdef BROADCAST_POLL_FOR_STATUS
	int BroadcastPollStatus(void *Container);
	int BroadcastPollStatus( StatusPacket *pStatus );
#endif
	int GetPIDTable(LPBYTE PIDTable);
	int SetPIDTable(LPBYTE PIDTable);
	int ChangePassword(char password[8], unsigned char key[16]);
	int ResetDevice(void);
	int TuneDevice(void * TuningParameters);
	int ConfigureNetwork(void * pNetworkSetting);
	int SetTraps( void *TrapParameters );
	int SpecifyPassword( char password[8] );
	CS75_JLEO( bool remote=false);
	virtual ~CS75_JLEO();

protected:

	void TranscodeStatus( S75_Status_JLEO *response, S75JLEOStatusPacket *status_packet_wrapper );
	bool ParseStatusPacket();
	PIDList* PIDListWrapper()	{ return new StandardPIDList( new S75_PIDList_JLEO ); }
	void TranscodePIDList( PIDSettings_MSG *PidSettingMSG );
#ifdef BROADCAST_POLL_FOR_STATUS
	bool PollForDeviceOnLocalAdapter( char *local_address );
	bool CreateBroadcastLink();
#endif
	bool CreateNetConfigMsg( NetworkConfigPacket *network_config_packet_wrapper, NetSetting *pNetworkSetting, BYTE filter );
	bool ConfirmDeviceSpecificNetworkSettings( StatusPacket *status_wrapper, NetworkConfigPacket *network_config_packet_wrapper );
	void CreateRFConfigMsg( S75JLEORFConfigPacket *rf_config_packet_wrapper, void *TuningParameters );

	BYTE m_IGMP_Filter;
//	S75JLEO_Status_MSG m_StatusStructure;
//	S75_Status_JLEO * m_pS75JLEO_Status;		// pointers to the external structures used by the calling functions
	S75_PIDList_JLEO * m_pS75JLEO_PIDList;		// totally different from the structures used by the device ...
	S75_RFSetting_JLEO * m_pS75JLEO_RFSetting;	// we need these constructs to so we can pass the data back in a different format
//	S75_NetSetting_JLEO * m_pS75JLEO_NetSetting;
	S75_TrapSetting_JLEO * m_pS75JLEO_TrapSetting;

	pstruct m_sec;
#ifdef BROADCAST_POLL_FOR_STATUS
	SOCKET m_broadcast_send_socket;
	SOCKET m_broadcast_receive_socket;
	bool	m_broadcast_linked;
#endif

};

//#endif // !defined(AFX_S75_V3_H__12828402_1462_4536_9485_39A39684D694__INCLUDED_)

#endif


