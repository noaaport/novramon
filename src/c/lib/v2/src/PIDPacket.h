// PIDPacket.h: interface for the PIDPacket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIDPACKET_H__C8B793D0_6DAF_4A56_8BB6_91F6D2ED94F3__INCLUDED_)
#define AFX_PIDPACKET_H__C8B793D0_6DAF_4A56_8BB6_91F6D2ED94F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "NovraDevicePackets.h"
#include "S75JLEOPackets.h"
#ifndef LINUX
	#include <Winsock2.h>
#endif
#ifdef LINUX
	#include <netinet/in.h>
#endif


class PIDPacket  
{
public:

	PIDPacket();
	virtual ~PIDPacket();

	virtual WORD* OpcodePointer()=0;
	virtual BYTE* data()=0;

};


class PIDSettingsPacket : public PIDPacket
{
public:
	virtual void hton( int max_pids, bool forward_raw = false )=0;
};


class CommonPIDSettingsPacket : public PIDSettingsPacket  
{
public:

	CommonPIDSettingsPacket( void *Container ) { pid_setting_packet = (PIDSettings_MSG *)Container; }
	virtual ~CommonPIDSettingsPacket() {};

	WORD*		OpcodePointer()		{ return &(pid_setting_packet->Opcode);		}
	BYTE*		data()				{ return (BYTE *)pid_setting_packet;		}
	PIDEntry*	PIDEntriesPointer()	{ return pid_setting_packet->PIDTableEntry;	}
	void		hton( int max_pids, bool forward_raw = false );

protected:

	PIDSettings_MSG *pid_setting_packet;

};


inline void CommonPIDSettingsPacket::hton( int max_pids, bool forward_raw )
{
	for ( int i = 0; i < max_pids; i++) {
		pid_setting_packet->PIDTableEntry[i].PID = htons(pid_setting_packet->PIDTableEntry[i].PID);
		if ( forward_raw ) {
			pid_setting_packet->PIDTableEntry[i].DestinationUDP = htons(pid_setting_packet->PIDTableEntry[i].DestinationUDP);
		}
	} // for
}


class S75JLEOPIDSettingsPacket : public PIDSettingsPacket  
{
public:

	S75JLEOPIDSettingsPacket( void *Container ) { pid_setting_packet = (S75JLEO_PIDSettings_MSG *)Container; }
	virtual ~S75JLEOPIDSettingsPacket();

	WORD* OpcodePointer()		{ return &(pid_setting_packet->Opcode);		}
	WORD* PIDEntriesPointer()	{ return pid_setting_packet->PIDTableEntry;	}
	BYTE* data()				{ return (BYTE *)pid_setting_packet;		}
	void  hton();

protected:

	S75JLEO_PIDSettings_MSG *pid_setting_packet;

};


#endif // !defined(AFX_PIDPACKET_H__C8B793D0_6DAF_4A56_8BB6_91F6D2ED94F3__INCLUDED_)
