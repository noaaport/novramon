// StatusPacket.h: interface for the StatusPacket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATUSPACKET_H__13511B3F_21D7_4D36_B0E5_2DAE95474E3B__INCLUDED_)
#define AFX_STATUSPACKET_H__13511B3F_21D7_4D36_B0E5_2DAE95474E3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef LINUX
	#include <Winsock2.h>
#endif
#ifdef LINUX
	#include <netinet/in.h>
#endif
#include "NovraDevicePackets.h"
#include "S75V2Packets.h"
#include "S75V3Packets.h"
#include "S75CAPackets.h"
#include "PesMakerPackets.h"
#include "A75MSGS.h"
#include "A75PesMakerPackets.h"
#include "S75JLEOPackets.h"


// This stuff is all an attempt to isolate the rest of the code from the differences
// in the status packet structures and hopefully reuse some code.  Whether it turns out being
// better than what we had before remains to be seen.

// First ... some abstract classes

class StatusPacket  
{
public:

	StatusPacket( void *Container ) { status_packet = (Status_MSG *)Container; }
	virtual ~StatusPacket() {};

	BYTE* ProductIDPointer()	{ return status_packet->ProductID;		}
	WORD* DSPFirmWarePointer()	{ return &(status_packet->DSPFirmWare);	}

	virtual BYTE* DestIPPointer()=0;
	virtual BYTE* SubnetMaskPointer()=0;
	virtual BYTE* DefaultGatewayIPPointer()=0;
	virtual WORD* StatusDestUDPPointer()=0;
	virtual BYTE* ReceiverMACPointer()=0;
	virtual WORD* RFStatusValidPointer()=0;
	virtual WORD* MicroFirmWarePointer()=0;
	virtual WORD* EthernetTXPointer()=0;
	virtual WORD* EthernetDroppedPointer()=0;
	virtual WORD* EthernetRXPointer()=0;
	virtual WORD* EthernetRXErrPointer()=0;
//	virtual WORD* EthernetTXErrPointer()=0;
	virtual WORD* TotalDVBAcceptedPointer()=0;
//	virtual WORD* FEL_LossPointer()=0;
	virtual WORD* Sync_LossPointer()=0;
	virtual WORD* PIDListPointer()=0;

	BYTE* data() { return (BYTE *)status_packet; };
	int   size() { return sizeof( Status_MSG );  }

	void ntoh();

protected :

	Status_MSG	*status_packet;
	int			MAX_SUPP_PIDS;

};


// Mapping for most common layouts

class CommonStatusPacket : public StatusPacket 
{
public:

	CommonStatusPacket( void *Container ) : StatusPacket( Container ) { common_status_packet = (CommonStatus_MSG *)Container; }
	virtual ~CommonStatusPacket() {};

	BYTE* DestIPPointer()			{ return   common_status_packet->DestIP;			}
	BYTE* SubnetMaskPointer()		{ return   common_status_packet->SubnetMask;		}
	BYTE* DefaultGatewayIPPointer()	{ return   common_status_packet->DefaultGatewayIP;	}
	WORD* StatusDestUDPPointer()	{ return &(common_status_packet->StatusDestUDP);	}
	BYTE* ReceiverMACPointer()		{ return   common_status_packet->ReceiverMAC;		}
	WORD* RFStatusValidPointer()	{ return &(common_status_packet->RFStatusValid);	}
	WORD* MicroFirmWarePointer()	{ return &(common_status_packet->MicroFirmWare);	}
	WORD* EthernetTXPointer()		{ return &(common_status_packet->EthernetTX);		}
	WORD* EthernetDroppedPointer()	{ return &(common_status_packet->EthernetDropped);	}
	WORD* EthernetRXPointer()		{ return &(common_status_packet->EthernetRX);		}
	WORD* EthernetRXErrPointer()	{ return &(common_status_packet->EthernetRXErr);	}
//	WORD* EthernetTXErrPointer()	{ return &(common_status_packet->EthernetTXErr);	}
	WORD* TotalDVBAcceptedPointer()	{ return &(common_status_packet->TotalDVBAccepted);	}
//	WORD* FEL_LossPointer()			{ return &(common_status_packet->FEL_Loss);			}
	WORD* Sync_LossPointer()		{ return &(common_status_packet->Sync_Loss);		}
	BYTE* FilterPointer()			{ return &(common_status_packet->Filter);			}
	BYTE* StatusIntervalPointer()	{ return &(common_status_packet->StatusInterval);	}

	BYTE* data() { return (BYTE *)common_status_packet; };
	int   size() { return sizeof( CommonStatus_MSG );  }

	void ntoh();

protected :

	CommonStatus_MSG *common_status_packet;

};


class ATSCStatusPacket : public CommonStatusPacket
{
public:

	ATSCStatusPacket( void *Container ) : CommonStatusPacket( Container ) {};
	virtual ~ATSCStatusPacket() {};

	virtual WORD* ChannelNumberPointer()=0;
	virtual BYTE* DeModStatsPointer()=0;
	virtual BYTE* VSBModePointer()=0;
	virtual BYTE* SER_1Pointer()=0;
	virtual BYTE* SER_10Pointer()=0;
	virtual WORD* SNR_AEPointer()=0;
	virtual WORD* SNR_BEPointer()=0;
	virtual WORD* Carrier_OffPointer()=0;
	virtual WORD* IF_AGCPointer()=0;
	virtual WORD* RFAGCPointer()=0;
	virtual WORD* FEC_UnErrPointer()=0;
	virtual WORD* FEC_CorrPointer()=0;
	virtual WORD* FEC_UnCorrPointer()=0;
	virtual WORD* RFENDPointer()=0;
	virtual WORD* EthernetTXErrPointer()=0;
	virtual WORD* FEL_LossPointer()=0;

	void ntoh();

};


class SatelliteStatusPacket : public CommonStatusPacket
{
public:

	SatelliteStatusPacket( void *Container ) : CommonStatusPacket( Container ) {};
	virtual ~SatelliteStatusPacket() {};

	virtual WORD* SymbolRatePointer()=0;
	virtual WORD* FrequencyPointer()=0;
	virtual BYTE* LNBConfigurationPointer()=0;
	virtual BYTE* ViterbiRatePointer()=0;
	virtual BYTE* SignalStrengthPointer()=0;
	virtual BYTE* DemodulatorStatusPointer()=0;
	virtual BYTE* VBER_ManPointer()=0;
	virtual BYTE* VBER_ExpPointer()=0;
	virtual BYTE* UncorrectablesPointer()=0;
	virtual BYTE* DemodulatorGainPointer()=0;
	virtual WORD* RFENDPointer()=0;

	void ntoh();

};


class CommonATSCStatusPacket : public ATSCStatusPacket
{
public :

	CommonATSCStatusPacket( void *Container ) : ATSCStatusPacket( Container ) { common_atsc_status_packet = (CommonATSCStatus_MSG *)Container; }
	~CommonATSCStatusPacket() {};

	WORD* ChannelNumberPointer()	{ return &(common_atsc_status_packet->ChannelNumber);	}
	BYTE* DeModStatsPointer()		{ return &(common_atsc_status_packet->DeModStats);		}
	BYTE* VSBModePointer()			{ return &(common_atsc_status_packet->VSBMode);			}
	BYTE* SER_1Pointer()			{ return &(common_atsc_status_packet->SER_1);			}
	BYTE* SER_10Pointer()			{ return &(common_atsc_status_packet->SER_10);			}
	WORD* SNR_AEPointer()			{ return &(common_atsc_status_packet->SNR_AE);			}
	WORD* SNR_BEPointer()			{ return &(common_atsc_status_packet->SNR_BE);			}
	WORD* Carrier_OffPointer()		{ return &(common_atsc_status_packet->Carrier_Off);		}
	WORD* IF_AGCPointer()			{ return &(common_atsc_status_packet->IF_AGC);			}
	WORD* RFAGCPointer()			{ return &(common_atsc_status_packet->RFAGC);			}
	WORD* FEC_UnErrPointer()		{ return &(common_atsc_status_packet->FEC_UnErr);		}
	WORD* FEC_CorrPointer()			{ return &(common_atsc_status_packet->FEC_Corr);		}
	WORD* FEC_UnCorrPointer()		{ return &(common_atsc_status_packet->FEC_UnCorr);		}
	WORD* RFENDPointer()			{ return &(common_atsc_status_packet->RFEND);			}
	WORD* EthernetTXErrPointer()	{ return &(common_atsc_status_packet->EthernetTXErr);	}
	WORD* FEL_LossPointer()			{ return &(common_atsc_status_packet->FEL_Loss);		}


	BYTE* data() { return (BYTE *)common_atsc_status_packet; };
	int   size() { return sizeof( CommonATSCStatus_MSG );  }

	void ntoh();

protected :

	CommonATSCStatus_MSG *common_atsc_status_packet;

};


class CommonSatelliteStatusPacket : public SatelliteStatusPacket
{
public :

	CommonSatelliteStatusPacket( void *Container ) : SatelliteStatusPacket( Container ) { common_satellite_status_packet = (CommonSatelliteStatus_MSG *)Container; }
	~CommonSatelliteStatusPacket() {};

	WORD* SymbolRatePointer()			{ return &(common_satellite_status_packet->SymbolRate);			}
	WORD* FrequencyPointer()			{ return &(common_satellite_status_packet->Frequency);			}
	BYTE* LNBConfigurationPointer()		{ return &(common_satellite_status_packet->LNBConfiguration);	}
	BYTE* ViterbiRatePointer()			{ return &(common_satellite_status_packet->ViterbiRate);		}
	BYTE* SignalStrengthPointer()		{ return &(common_satellite_status_packet->SignalStrength);		}
	BYTE* DemodulatorStatusPointer()	{ return &(common_satellite_status_packet->DemodulatorStatus);	}
	BYTE* VBER_ManPointer()				{ return   common_satellite_status_packet->VBER_Man;			}
	BYTE* VBER_ExpPointer()				{ return &(common_satellite_status_packet->VBER_Exp);			}
	BYTE* UncorrectablesPointer()		{ return &(common_satellite_status_packet->Uncorrectables);		}
	BYTE* DemodulatorGainPointer()		{ return &(common_satellite_status_packet->DemodulatorGain);	}
	BYTE* NESTPointer()					{ return &(common_satellite_status_packet->NEST);				}
	BYTE* Clock_OffPointer()			{ return &(common_satellite_status_packet->Clock_Off);			}
	WORD* Freq_ErrPointer()				{ return &(common_satellite_status_packet->Freq_Err);			}
	BYTE* ADC_MIDPointer()				{ return &(common_satellite_status_packet->ADC_MID);			}
	BYTE* ADC_CLIPPointer()				{ return &(common_satellite_status_packet->ADC_CLIP);			}
	BYTE* Digital_GainPointer()			{ return &(common_satellite_status_packet->Digital_Gain);		}
	BYTE* AA_CLAMPPointer()				{ return &(common_satellite_status_packet->AA_CLAMP);			}
	WORD* RFENDPointer()				{ return &(common_satellite_status_packet->RFEND);				}
	WORD* EthernetTXErrPointer()		{ return &(common_satellite_status_packet->EthernetTXErr);		}
	WORD* FEL_LossPointer()				{ return &(common_satellite_status_packet->FEL_Loss);			}

	BYTE* data() { return (BYTE *)common_satellite_status_packet; };
	int   size() { return sizeof( CommonSatelliteStatus_MSG );  }

	void ntoh();

protected :

	CommonSatelliteStatus_MSG *common_satellite_status_packet;

};


// Now classes for specific receivers ...


class A75StatusPacket : public CommonATSCStatusPacket 
{
public:

	A75StatusPacket( void *Container ) : CommonATSCStatusPacket( Container ) { a75_status_packet = (A75Status_MSG *)Container; MAX_SUPP_PIDS = 16; }
	~A75StatusPacket() {};

	WORD* PIDListPointer()	{ return a75_status_packet->PIDList; }
	BYTE* DVBMACPointer()	{ return a75_status_packet->DVBMAC; }

	BYTE* data() { return (BYTE *)a75_status_packet; };
	int   size() { return sizeof( A75Status_MSG );  }

	void ntoh();

protected:

	A75Status_MSG *a75_status_packet;

};


class A75PesMakerStatusPacket : public CommonATSCStatusPacket 
{
public:

	A75PesMakerStatusPacket( void *Container ) : CommonATSCStatusPacket( Container ) { a75_pes_maker_status_packet = (A75PesMakerStatus_MSG *)Container; MAX_SUPP_PIDS = 32; }
	~A75PesMakerStatusPacket() {};

	WORD* PIDListPointer()	{ return a75_pes_maker_status_packet->PIDList; }
	BYTE* DVBMACPointer()	{ return a75_pes_maker_status_packet->DVBMAC; }

	BYTE* data() { return (BYTE *)a75_pes_maker_status_packet; };
	int   size() { return sizeof( A75PesMakerStatus_MSG );  }

	void ntoh();

protected:

	A75PesMakerStatus_MSG *a75_pes_maker_status_packet;

};


class S75V2StatusPacket : public CommonSatelliteStatusPacket 
{
public:

	S75V2StatusPacket( void *Container ) : CommonSatelliteStatusPacket( Container ) { s75_v2_status_packet = (S75D_V2_Status_MSG *)Container; MAX_SUPP_PIDS = 32; }
	~S75V2StatusPacket() {};

	BYTE* ReservedPointer()	{ return s75_v2_status_packet->Reserved; }
	WORD* PIDListPointer()	{ return s75_v2_status_packet->PIDList; }

	BYTE* data() { return (BYTE *)s75_v2_status_packet; };
	int   size() { return sizeof( S75D_V2_Status_MSG );  }

	void ntoh();

protected:

	S75D_V2_Status_MSG *s75_v2_status_packet;

};


class S75V3StatusPacket : public CommonSatelliteStatusPacket 
{
public:

	S75V3StatusPacket( void *Container ) : CommonSatelliteStatusPacket( Container ) { s75_v3_status_packet = (S75D_V3_Status_MSG *)Container; MAX_SUPP_PIDS = 16; }
	~S75V3StatusPacket() {};

	BYTE* ReservedPointer()	{ return s75_v3_status_packet->Reserved; }
	WORD* PIDListPointer()	{ return s75_v3_status_packet->PIDList; }
	BYTE* DVBMACPointer()	{ return s75_v3_status_packet->DVBMAC; }

	BYTE* data() { return (BYTE *)s75_v3_status_packet; };
	int   size() { return sizeof( S75D_V3_Status_MSG );  }

	void ntoh();

protected:

	S75D_V3_Status_MSG *s75_v3_status_packet;

};


class PesMakerStatusPacket : public CommonSatelliteStatusPacket 
{
public:

	PesMakerStatusPacket( void *Container ) : CommonSatelliteStatusPacket( Container ) { pes_maker_status_packet = (PesMakerStatus_MSG *)Container; MAX_SUPP_PIDS = 32; }
	~PesMakerStatusPacket() {};

	BYTE* ReservedPointer()	{ return pes_maker_status_packet->Reserved; }
	WORD* PIDListPointer()	{ return pes_maker_status_packet->PIDList; }
	BYTE* DVBMACPointer()	{ return pes_maker_status_packet->DVBMAC; }

	BYTE* data() { return (BYTE *)pes_maker_status_packet; };
	int   size() { return sizeof( PesMakerStatus_MSG );  }

	void ntoh();

protected:

	PesMakerStatus_MSG *pes_maker_status_packet;

};


/*
class S75CAStatusPacket : public CommonSatelliteStatusPacket 
{
public:

	S75CAStatusPacket( void *Container ) : CommonSatelliteStatusPacket( Container ) { s75ca_status_packet = (S75CAStatus_MSG *)Container; MAX_SUPP_PIDS = 32; }
	~S75CAStatusPacket() {};

	WORD* ProgramListPointer()		{ return   s75ca_status_packet->ProgramList;		}
	BYTE* UncorrectablesPointer()	{ return &(s75ca_status_packet->Uncorrectables);	}
	BYTE* DemodulatorGainPointer()	{ return &(s75ca_status_packet->DemodulatorGain);	}
	WORD* PIDListPointer()			{ return (unsigned short *)(s75ca_status_packet->PIDList); }
	BYTE* DVBMACPointer()			{ return s75ca_status_packet->DVBMAC; }

	BYTE* data() { return (BYTE *)s75ca_status_packet; };
	int   size() { return sizeof( S75CAStatus_MSG );  }

	void ntoh();

protected:

	S75CAStatus_MSG *s75ca_status_packet;

};
*/
class S75PlusStatusPacket : public SatelliteStatusPacket
{
public:

	S75PlusStatusPacket( void *Container ) : SatelliteStatusPacket( Container ) { s75plus_status_packet = (S75PlusStatus_MSG *)Container; MAX_SUPP_PIDS = 32; }
	virtual ~S75PlusStatusPacket() {};

	BYTE* DestIPPointer()						{ return   s75plus_status_packet->UniCastStatusDestIP;			}
	BYTE* ReceiverIPPointer()					{ return   s75plus_status_packet->ReceiverIP;					}
	BYTE* SubnetMaskPointer()					{ return   s75plus_status_packet->SubnetMask;					}
	BYTE* DefaultGatewayIPPointer()				{ return   s75plus_status_packet->DefaultGatewayIP;				}
	WORD* StatusDestUDPPointer()				{ return &(s75plus_status_packet->UniCastStatusDestUDP);		}
	BYTE* ReceiverMACPointer()					{ return   s75plus_status_packet->ReceiverMAC;					}
//	WORD* RFStatusValidPointer()				{ return &(s75plus_status_packet->RFStatusValid);				}
	WORD* MicroFirmWarePointer()				{ return &(s75plus_status_packet->MicroFirmWare);				}
	WORD* SymbolRatePointer()					{ return &(s75plus_status_packet->SymbolRate);					}
	WORD* FrequencyPointer()					{ return &(s75plus_status_packet->Frequency);					}
	BYTE* LNBConfigurationPointer()				{ return &(s75plus_status_packet->LNBConfiguration);			}
	BYTE* ViterbiRatePointer()					{ return &(s75plus_status_packet->ViterbiRate);					}
	BYTE* SignalStrengthPointer()				{ return &(s75plus_status_packet->SignalStrength);				}
	BYTE* DemodulatorStatusPointer()			{ return &(s75plus_status_packet->DemodulatorStatus);			}
	BYTE* VBER_ManPointer()						{ return   s75plus_status_packet->VBER_Man;						}
	BYTE* VBER_ExpPointer()						{ return &(s75plus_status_packet->VBER_Exp);					}
	BYTE* UncorrectablesPointer()				{ return &(s75plus_status_packet->Uncorrectables);				}
	BYTE* DemodulatorGainPointer()				{ return &(s75plus_status_packet->DemodulatorGain);				}
	BYTE* NESTPointer()							{ return &(s75plus_status_packet->NEST);						}
	BYTE* Clock_OffPointer()					{ return &(s75plus_status_packet->Clock_Off);					}
	WORD* Freq_ErrPointer()						{ return &(s75plus_status_packet->Freq_Err);					}
	BYTE* ADC_MIDPointer()						{ return &(s75plus_status_packet->ADC_MID);						}
	BYTE* ADC_CLIPPointer()						{ return &(s75plus_status_packet->ADC_CLIP);					}
	BYTE* Digital_GainPointer()					{ return &(s75plus_status_packet->Digital_Gain);				}
	BYTE* AA_CLAMPPointer()						{ return &(s75plus_status_packet->AA_CLAMP);					}
	WORD* RFENDPointer()						{ return &(s75plus_status_packet->RFEND);						}
	WORD* EthernetTXPointer()					{ return &(s75plus_status_packet->EthernetTX);					}
	WORD* EthernetDroppedPointer()				{ return &(s75plus_status_packet->EthernetDropped);				}
	WORD* EthernetRXPointer()					{ return &(s75plus_status_packet->EthernetRX);					}
	WORD* EthernetRXErrPointer()				{ return &(s75plus_status_packet->EthernetRXErr);				}
	WORD* DVBScrambledPointer()					{ return &(s75plus_status_packet->DVBScrambled);				}
	WORD* TotalDVBAcceptedPointer()				{ return &(s75plus_status_packet->DVBAccepted);					}
	WORD* DVBDescrambledPointer()				{ return &(s75plus_status_packet->DVBDescrambled);				}
	WORD* Sync_LossPointer()					{ return &(s75plus_status_packet->Sync_Loss);					}
	BYTE* FilterPointer()						{ return &(s75plus_status_packet->Filter);						}
	BYTE* CardStatusPointer()					{ return &(s75plus_status_packet->CardStatus);					}
//	BYTE* DVBMACPointer()						{ return   s75plus_status_packet->DVBMAC;						} 

	WORD* TotalDVBPacketsAcceptedPointer()		{ return   s75plus_status_packet->TotalDVBPacketsAccepted;		} 
	WORD* TotalDVBPacketsRXInErrorPointer()		{ return   s75plus_status_packet->TotalDVBPacketsRXInError;		} 
	WORD* TotalEthernetPacketsOutPointer()		{ return   s75plus_status_packet->TotalEthernetPacketsOut;		} 
	WORD* TotalUncorrectableTSPacketsPointer()	{ return   s75plus_status_packet->TotalUncorrectableTSPackets;	} 

	WORD* BroadcastStatusPortPointer()			{ return &(s75plus_status_packet->BroadcastStatusPort);			} 
	WORD* LocalOscFrequencyPointer()			{ return &(s75plus_status_packet->LocalOscFrequency);			} 

	WORD* PIDListPointer()						{ return   (unsigned short *)s75plus_status_packet->PIDList;	} 
	PIDDestination* PIDDestinationsPointer()	{ return s75plus_status_packet->PIDDestinations;				}
	WORD* ProgramListPointer()					{ return s75plus_status_packet->ProgramList;					}
	WORD* ProgramStatusPointer()				{ return s75plus_status_packet->ProgramStatus;					}

	BYTE* data() { return (BYTE *)s75plus_status_packet; };
	int   size() { return sizeof( S75PlusStatus_MSG );  }

	void ntoh();

protected:

	S75PlusStatus_MSG *s75plus_status_packet;

};


typedef S75PlusStatusPacket S75PlusProStatusPacket;
typedef S75PlusStatusPacket S75FKStatusPacket;
typedef S75PlusStatusPacket S75FKProStatusPacket;
typedef S75PlusStatusPacket S75CAStatusPacket;
typedef S75PlusStatusPacket S75CAProStatusPacket;


class S75JLEOStatusPacket : public SatelliteStatusPacket
{
public:

	S75JLEOStatusPacket( void *Container ) : SatelliteStatusPacket( Container ) { s75_jleo_status_packet = (S75JLEO_Status_MSG *)Container; MAX_SUPP_PIDS = 16; }
	virtual ~S75JLEOStatusPacket() {};

	BYTE* DestIPPointer()						{ return   s75_jleo_status_packet->UniCastStatusDestIP;			}
	BYTE* ReceiverIPPointer()					{ return   s75_jleo_status_packet->ReceiverIP;					}
	BYTE* SubnetMaskPointer()					{ return   s75_jleo_status_packet->SubnetMask;					}
	BYTE* DefaultGatewayIPPointer()				{ return   s75_jleo_status_packet->DefaultGatewayIP;			}
	WORD* StatusDestUDPPointer()				{ return &(s75_jleo_status_packet->UniCastStatusDestUDP);		}
	BYTE* ReceiverMACPointer()					{ return   s75_jleo_status_packet->ReceiverMAC;					}
	WORD* RFStatusValidPointer()				{ return &(s75_jleo_status_packet->RFStatusValid);				}
	WORD* MicroFirmWarePointer()				{ return &(s75_jleo_status_packet->MicroFirmWare);				}
	WORD* SymbolRatePointer()					{ return &(s75_jleo_status_packet->SymbolRate);					}
	WORD* FrequencyPointer()					{ return &(s75_jleo_status_packet->Frequency);					}
	BYTE* LNBConfigurationPointer()				{ return &(s75_jleo_status_packet->LNBConfiguration);			}
	BYTE* ViterbiRatePointer()					{ return &(s75_jleo_status_packet->ViterbiRate);				}
	BYTE* SignalStrengthPointer()				{ return &(s75_jleo_status_packet->SignalStrength);				}
	BYTE* DemodulatorStatusPointer()			{ return &(s75_jleo_status_packet->DemodulatorStatus);			}
	BYTE* VBER_ManPointer()						{ return   s75_jleo_status_packet->VBER_Man;					}
	BYTE* VBER_ExpPointer()						{ return &(s75_jleo_status_packet->VBER_Exp);					}
	BYTE* UncorrectablesPointer()				{ return &(s75_jleo_status_packet->Uncorrectables);				}
	BYTE* DemodulatorGainPointer()				{ return &(s75_jleo_status_packet->DemodulatorGain);			}
	BYTE* ReservedPointer()						{ return   s75_jleo_status_packet->Reserved;					}
	WORD* RFENDPointer()						{ return &(s75_jleo_status_packet->RFEND);						}
	WORD* EthernetTXPointer()					{ return &(s75_jleo_status_packet->EthernetTX);					}
	WORD* EthernetDroppedPointer()				{ return &(s75_jleo_status_packet->EthernetDropped);			}
	WORD* EthernetRXPointer()					{ return &(s75_jleo_status_packet->EthernetRX);					}
	WORD* EthernetRXErrPointer()				{ return &(s75_jleo_status_packet->EthernetRXErr);				}
	WORD* EthernetTXErrPointer()				{ return &(s75_jleo_status_packet->EthernetTXErr);				}
	WORD* TotalDVBAcceptedPointer()				{ return &(s75_jleo_status_packet->DVBAccepted);				}
	WORD* FEL_LossPointer()						{ return &(s75_jleo_status_packet->FEL_Loss);					}
	WORD* Sync_LossPointer()					{ return &(s75_jleo_status_packet->Sync_Loss);					}
	WORD* FilterPointer()						{ return &(s75_jleo_status_packet->Filter);						}
	WORD* PIDListPointer()						{ return   s75_jleo_status_packet->PIDList;						} 
	BYTE* DVBMACPointer()						{ return   s75_jleo_status_packet->DVBMAC;						} 
	
	unsigned long* TotalDVBPacketsAcceptedPointer()		{ return   s75_jleo_status_packet->TotalDVBPacketsAccepted;		} 
	unsigned long* TotalDVBPacketsRXInErrorPointer()		{ return   s75_jleo_status_packet->TotalDVBPacketsRXInError;	} 
	unsigned long* TotalEthernetPacketsOutPointer()		{ return   s75_jleo_status_packet->TotalEthernetPacketsOut;		} 
	unsigned long* TotalUncorrectableTSPacketsPointer()	{ return   s75_jleo_status_packet->TotalUncorrectableTSPackets;	} 

	WORD* BroadcastStatusPortPointer()			{ return &(s75_jleo_status_packet->BroadcastStatusPort);		} 
	WORD* LocalOscFrequencyPointer()			{ return &(s75_jleo_status_packet->LocalOscFrequency);			} 

	BYTE* data() { return (BYTE *)s75_jleo_status_packet; };
	int   size() { return sizeof( S75JLEO_Status_MSG );  }

	void ntoh();

protected:

	S75JLEO_Status_MSG *s75_jleo_status_packet;

};


#endif // !defined(AFX_STATUSPACKET_H__13511B3F_21D7_4D36_B0E5_2DAE95474E3B__INCLUDED_)
