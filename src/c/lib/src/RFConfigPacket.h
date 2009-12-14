// RFConfigPacket.h: interface for the RFConfigPacket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RFCONFIGPACKET_H__6C35215F_C8F5_4823_8BBF_BE9C89A73CD1__INCLUDED_)
#define AFX_RFCONFIGPACKET_H__6C35215F_C8F5_4823_8BBF_BE9C89A73CD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NovraDevicePackets.h"


class RFConfigPacket  
{
public:

	RFConfigPacket();
	virtual ~RFConfigPacket();

	virtual BYTE* data()=0;
	virtual int   size()=0;

};


class ATSCRFConfigPacket : public RFConfigPacket  
{
public:

	ATSCRFConfigPacket( void *Container ) { rf_config_packet = (ATSCRFConfig_MSG *)Container; }
	virtual ~ATSCRFConfigPacket();

	WORD* ChannelNumberPointer()	{ return &(rf_config_packet->ChannelNumber);	}
	WORD* ReservedPointer()			{ return &(rf_config_packet->Reserved);			}
	BYTE* BRESERVEDPointer()		{ return &(rf_config_packet->BRESERVED);		}

	BYTE* data()					{ return (BYTE *)rf_config_packet;				}
	int   size()					{ return sizeof( ATSCRFConfig_MSG );			}

protected:

	ATSCRFConfig_MSG *rf_config_packet;

};


class SatelliteRFConfigPacket : public RFConfigPacket  
{
public:

	SatelliteRFConfigPacket( void *Container ) { rf_config_packet = (SatelliteRFConfig_MSG *)Container; }
	virtual ~SatelliteRFConfigPacket() {};

	WORD* SymbolRatePointer()		{ return &(rf_config_packet->SymbolRate);	}
	WORD* FrequencyPointer()		{ return &(rf_config_packet->Frequency);	}
	BYTE* LNBConfigPointer()		{ return &(rf_config_packet->LNBConfig);	}
	BYTE* ViterbiRatePointer()		{ return &(rf_config_packet->ViterbiRate);	}

	BYTE* data()					{ return (BYTE *)rf_config_packet;			}
	int   size()					{ return sizeof( SatelliteRFConfig_MSG );	}

protected:

	SatelliteRFConfig_MSG *rf_config_packet;

};


class S75JLEORFConfigPacket : public SatelliteRFConfigPacket  
{
public:

	S75JLEORFConfigPacket( void *Container ) : SatelliteRFConfigPacket( Container ) { s75_jleo_rf_config_packet = (S75JLEORFConfig_MSG*)Container; }
	virtual ~S75JLEORFConfigPacket() {};

	WORD* LocalOscFrequencyPointer()	{ return &(s75_jleo_rf_config_packet->LocalOscFrequency);	}
	WORD* RandomPointer()				{ return &(s75_jleo_rf_config_packet->Random);				}
	BYTE* SignaturePointer()			{ return   s75_jleo_rf_config_packet->Signature;			}

	BYTE* data()						{ return (BYTE *)s75_jleo_rf_config_packet;					}
	int   size()						{ return sizeof( S75JLEORFConfig_MSG );						}

protected:

	S75JLEORFConfig_MSG *s75_jleo_rf_config_packet;

};


class S75PlusRFConfigPacket : public SatelliteRFConfigPacket  
{
public:

	S75PlusRFConfigPacket( void *Container ) : SatelliteRFConfigPacket( Container ) { s75plus_rf_config_packet = (S75PlusRFConfig_MSG*)Container; }
	virtual ~S75PlusRFConfigPacket() {};

	WORD* LocalOscFrequencyPointer()	{ return &(s75plus_rf_config_packet->LocalOscFrequency);	}
	WORD* RandomPointer()				{ return &(s75plus_rf_config_packet->Random);				}
	BYTE* SignaturePointer()			{ return   s75plus_rf_config_packet->Signature;				}

	BYTE* data()						{ return (BYTE *)s75plus_rf_config_packet;					}
	int   size()						{ return sizeof( S75PlusRFConfig_MSG );						}

protected:

	S75PlusRFConfig_MSG *s75plus_rf_config_packet;

};


typedef S75PlusRFConfigPacket S75PlusProRFConfigPacket;
typedef S75PlusRFConfigPacket S75FKRFConfigPacket;
typedef S75PlusRFConfigPacket S75FKProRFConfigPacket;
typedef S75PlusRFConfigPacket S75CARFConfigPacket;
typedef S75PlusRFConfigPacket S75CAProRFConfigPacket;


#endif // !defined(AFX_RFCONFIGPACKET_H__6C35215F_C8F5_4823_8BBF_BE9C89A73CD1__INCLUDED_)
