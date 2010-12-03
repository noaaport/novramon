// NetworkConfigPacket.h: interface for the NetworkConfigPacket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKCONFIGPACKET_H__4A2D4FE3_79C6_4823_BC9B_52740E924CA8__INCLUDED_)
#define AFX_NETWORKCONFIGPACKET_H__4A2D4FE3_79C6_4823_BC9B_52740E924CA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NovraDevicePackets.h"
#include "S75JLEOPackets.h"

class NetworkConfigPacket  
{
public:

	NetworkConfigPacket() {}
	virtual ~NetworkConfigPacket();

	virtual BYTE* DestinationIPPointer()=0;
	virtual BYTE* SubnetMaskPointer()=0;
	virtual BYTE* DefaultGatewayIPPointer()=0;
	virtual WORD* DestinationUDPPointer()=0;
	virtual BYTE* ReceiverIPPointer()=0;
	virtual WORD* StatusPortPointer()=0;

	virtual BYTE* data()=0;
	virtual int   size()=0;

};


class CommonNetworkConfigPacket : public NetworkConfigPacket  
{
public:

	CommonNetworkConfigPacket( void *Container ) { net_config_packet = (CommonNetConfig_MSG *)Container; }
	virtual ~CommonNetworkConfigPacket() {};

	BYTE* DestinationIPPointer()	{ return net_config_packet->DestinationIP;		}
	BYTE* SubnetMaskPointer()		{ return net_config_packet->SubnetMask;			}
	BYTE* DefaultGatewayIPPointer()	{ return net_config_packet->DefaultGatewayIP;	}
	WORD* DestinationUDPPointer()	{ return &(net_config_packet->DestinationUDP);	}
	BYTE* ReceiverIPPointer()		{ return net_config_packet->ReceiverIP;			}
	WORD* StatusPortPointer()		{ return &(net_config_packet->StatusPort);		}
	BYTE* StatusIntervalPointer()	{ return &(net_config_packet->StatusInterval);	}
	BYTE* FilterPointer()			{ return &(net_config_packet->Filter);			}

	BYTE* data()					{ return (BYTE *)net_config_packet;				}
	int   size()					{ return sizeof( CommonNetConfig_MSG );			}

protected:

	CommonNetConfig_MSG *net_config_packet;

};


class S75JLEONetworkConfigPacket : public NetworkConfigPacket  
{
public:

	S75JLEONetworkConfigPacket( void *Container ) { s75_jleo_net_config_packet = (S75JLEONetConfig_MSG*)Container; }
	virtual ~S75JLEONetworkConfigPacket() {};

	BYTE* DestinationIPPointer()	{ return   s75_jleo_net_config_packet->DestinationIP;		}
	BYTE* SubnetMaskPointer()		{ return   s75_jleo_net_config_packet->SubnetMask;			}
	BYTE* DefaultGatewayIPPointer()	{ return   s75_jleo_net_config_packet->DefaultGatewayIP;	}
	WORD* DestinationUDPPointer()	{ return &(s75_jleo_net_config_packet->DestinationUDP);		}
	BYTE* ReceiverIPPointer()		{ return   s75_jleo_net_config_packet->ReceiverIP;			}
	WORD* StatusPortPointer()		{ return &(s75_jleo_net_config_packet->StatusPort);			}
	WORD* IGMPPointer()				{ return &(s75_jleo_net_config_packet->IGMP);				}
	WORD* RandomPointer()			{ return &(s75_jleo_net_config_packet->Random);				}
	BYTE* SignaturePointer()		{ return   s75_jleo_net_config_packet->Signature;			}

	BYTE* data()					{ return (BYTE *)s75_jleo_net_config_packet;				}
	int   size()					{ return sizeof( S75JLEONetConfig_MSG );					}

protected:

	S75JLEONetConfig_MSG *s75_jleo_net_config_packet;

};


class S75PlusNetworkConfigPacket : public NetworkConfigPacket  
{
public:

	S75PlusNetworkConfigPacket( void *Container ) { s75plus_net_config_packet = (S75PlusNetConfig_MSG*)Container; }
	virtual ~S75PlusNetworkConfigPacket() {};

	BYTE* DestinationIPPointer()	{ return   s75plus_net_config_packet->DestinationIP;	}
	BYTE* SubnetMaskPointer()		{ return   s75plus_net_config_packet->SubnetMask;		}
	BYTE* DefaultGatewayIPPointer()	{ return   s75plus_net_config_packet->DefaultGatewayIP;	}
	WORD* DestinationUDPPointer()	{ return &(s75plus_net_config_packet->DestinationUDP);	}
	BYTE* ReceiverIPPointer()		{ return   s75plus_net_config_packet->ReceiverIP;		}
	WORD* StatusPortPointer()		{ return &(s75plus_net_config_packet->StatusPort);		}
	WORD* IGMPPointer()				{ return &(s75plus_net_config_packet->IGMP);			}
	WORD* RandomPointer()			{ return &(s75plus_net_config_packet->Random);			}
	BYTE* SignaturePointer()		{ return   s75plus_net_config_packet->Signature;		}

	BYTE* data()					{ return (BYTE *)s75plus_net_config_packet;				}
	int   size()					{ return sizeof( S75PlusNetConfig_MSG );				}

protected:

	S75PlusNetConfig_MSG *s75plus_net_config_packet;

};


typedef S75PlusNetworkConfigPacket S75PlusProNetworkConfigPacket;
typedef S75PlusNetworkConfigPacket S75FKNetworkConfigPacket;
typedef S75PlusNetworkConfigPacket S75FKProNetworkConfigPacket;
typedef S75PlusNetworkConfigPacket S75CANetworkConfigPacket;
typedef S75PlusNetworkConfigPacket S75CAProNetworkConfigPacket;


#endif // !defined(AFX_NETWORKCONFIGPACKET_H__4A2D4FE3_79C6_4823_BC9B_52740E924CA8__INCLUDED_)
