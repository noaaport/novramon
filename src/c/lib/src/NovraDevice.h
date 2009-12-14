// NovraDevice.h: interface for the CNovraDevice class.
//
//////////////////////////////////////////////////////////////////////


//#if !defined(AFX_NOVRADEVICE_H__ACBD51A7_E3A2_43C7_A331_CF04B6E8942B__INCLUDED_)
//#define AFX_NOVRADEVICE_H__ACBD51A7_E3A2_43C7_A331_CF04B6E8942B__INCLUDED_
//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000


#ifndef _NOVRA_Device
#define	_NOVRA_Device

#ifndef LINUX
	
	#ifndef  _WinSock2Included_
	#define	 _WinSock2Included_
	#include <winsock2.h>
	#endif
	
	#ifndef  _WindowsIncluded_
	#define	 _WindowsIncluded_
	#include <windows.h>
	#endif
	
#else
/*
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
*/
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
/* #include <iostream.h> */
#include "linux_windows_types.h"
#endif
#include"s75.h"
#include "NovraDevicePackets.h"
#include "Status.h"
#include "PIDList.h"
#include "ProgramList.h"
#include "StatusPacket.h"
#include "NetworkConfigPacket.h"
#include "RFConfigPacket.h"
#include "PIDPacket.h"


	const int STATUS_PACKET_TIMEOUT = 4000;         // Status packet time out used when configuring system
	const int NUM_RETRIES = 4;                      // Number of time to try to send configuration to S75D

#include "StatusListner.h"	// Added by ClassView
/// @@NS AUG 15 This structure carries the Last added route Information so we can reomve the Route
// The Implication is add network path and removenetwork path should be used as a pair.
typedef struct NetworkPath{
	BYTE	MAC[6];
	BYTE	IP[4];
	BYTE	IF_IP[4];
	BYTE	IF_NAME[100];
	bool	ARPSet;
	bool	RouteSet;
} NetworkPath;




class CNovraDevice  
{
public:
	CNovraDevice(LPBYTE pStatusBuff = NULL);
	virtual ~CNovraDevice();

	WORD GetDeviceType();
	bool DetachDevice(void);
	bool IsStatusValid();
	int WaitForStatusPacket(int timeout);
	int   StartStatusListener();
	void   StopStatusListener();
	int   FlushStatusBuffer(int timeout);
	void RemoveNetworkPath();
	int SetNetworkPath(BYTE MyIPAddress[4], bool SetARPTable, bool SetRoutingTable, BYTE ifname[] = NULL);
	void FilterStatus(bool filter);
	void SetStatusTimeout(UINT  Timeout);
	bool Get_Status_Port(WORD *);
	bool Set_Status_Port(WORD  port);
	int Attach_Device(void *);
	bool Set_Device_IP_Address(BYTE * IP);
	bool Get_Device_IP_Address(BYTE * IP);
	bool Set_Device_MAC_Address(BYTE*);
	bool Get_Device_MAC_Address(BYTE *);


	// OverRideable Functions - Device specific
	int ProgramDeviceUID(LPBYTE UID);
	int ProgramDeviceMACAddress(LPBYTE MAC);
	virtual int ResetDevice(void);
	virtual int ResetCAM(void);
	virtual int setCAMWatchdogTimeout(WORD timeout);
	virtual int getCAMWatchdogTimeout(WORD *timeout);
	virtual int TuneDevice(void * TuningParameters);
	virtual int ConfigureNetwork(void * pNetworkSetting);
	virtual int GetStatus( void *Container );
	int ConfigureDVBMAC(LPBYTE DVBMAC);
	bool GetDefaultConfigStruct(void *);
	virtual int GetPIDTable(LPBYTE PIDTable);
	virtual int SetPIDTable(LPBYTE PIDTable);
	virtual int SetProgramTable(LPBYTE ProgramTable);
	virtual int GetProgramTable(LPBYTE ProgramTable);
	virtual int SetPAT(LPBYTE PAT);
	virtual int GetPAT(LPBYTE PAT);
	virtual int SetFixedKeys(LPBYTE KeyTable);
	virtual int GetFixedKeys(LPBYTE KeyTable);
	virtual int SetTraps(LPBYTE Traps);
	virtual int GetTraps(LPBYTE Traps);
	int GetStatus( Status *pStatus, bool forward_raw = false, int max_pids = 32 );
#ifdef JSAT	
	virtual int ChangePassword(char password[8], unsigned char key[16]) { return ERR_FAILED; };
	virtual int SpecifyPassword(char password[8]) { return ERR_FAILED; };
//	virtual int SetTraps(void * TrapParameters) { return ERR_FAILED; };
//	virtual int GetTraps(void * TrapParameters) { return ERR_FAILED; };
	virtual int PollStatus(void *Container, int timeout=2000 ) { return ERR_FAILED; };
	virtual int PollStatus( StatusPacket *status_packet_wrapper, int timeout=2000 ) { return ERR_FAILED; };
#ifdef BROADCAST_POLL_FOR_STATUS
	virtual int BroadcastPollStatus(void *Container) { return ERR_FAILED; };
	int BroadcastPollStatus( StatusPacket *status_packet_wrapper ) { return ERR_FAILED; };
#endif
#endif


protected:
	Status_MSG	m_Status;
	StatusPacket *m_StatusWrapper;
	bool TrimPIDList( LPBYTE pList, int Max);
	bool CheckStatusPort( WORD port );
	bool CreateNetConfigMsg( NetworkConfigPacket *network_config_packet_wrapper, NetSetting *pNetworkSetting, BYTE filter );
	int SendNetworkConfig( NetworkConfigPacket *network_config_packet_wrapper );
	int SendPIDs( PIDSettingsPacket  *pid_setting_packet_wrapper, int size, int max_pids, bool forward_raw = false );
	int GetPIDs( int size );
	virtual PIDList* PIDListWrapper()=0;
	virtual void TranscodePIDList( PIDSettings_MSG *PidSettingMSG )=0;
	virtual BYTE* DVBMACPointer() { return NULL; }
	virtual bool ConfirmDeviceSpecificNetworkSettings( StatusPacket *status_wrapper, NetworkConfigPacket *network_config_packet_wrapper )=0;
	virtual bool ConfirmDeviceSpecificRFSettings( StatusPacket *status_wrapper, RFConfigPacket *rf_config_packet_wrapper )=0;

	DWORD m_Status_Listen_Start_Time;
	NetworkPath	m_LastNetPath;
	LPBYTE m_pStatusBuffer;
	CStatusListner m_StatusListener;
	bool m_usingStatusListener;
	bool m_remote;
	bool m_NetPathSet;
	DWORD m_LastStatus;
	int m_StatusTimeout;
	UINT m_StatusPacketSize;
	bool m_IPAddressValid;
	bool m_Device_attached;
	BYTE m_Device_MAC_Address[6];		// The Ethernet MAC Address of the Device.
	BYTE m_Device_IP_Address[4];		// The IP Address of the Device
	WORD m_Device_Status_Port ;		// The Port on which status are being Sent.
	NOVRA_DEVICES m_Device_Type;				// An Identifier to what type the device is.
	bool m_FilterOnIP;
	bool m_MACAddressValid;
	PIDList* m_PIDList;
	Program_List* m_ProgramList;
	int SendWaitForConfirmation( TCHAR device_ip[16],UINT port,LPBYTE pData, int length);
	virtual bool ParseStatusPacket(void);	// The way each device interperts status packet depends on its type





};
#endif
//#endif // !defined(AFX_NOVRADEVICE_H__ACBD51A7_E3A2_43C7_A331_CF04B6E8942B__INCLUDED_)
