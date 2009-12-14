// S75PlusStatus.h: interface for the S75PlusStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_S75CASTATUS_H__7421DA43_2A81_4DD6_A6EB_FE6F08B6B733__INCLUDED_)
#define AFX_S75CASTATUS_H__7421DA43_2A81_4DD6_A6EB_FE6F08B6B733__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>

#include "SatelliteStatus.h"

class S75PlusStatus : public SatelliteStatus  
{
public:
	S75PlusStatus( void *Container );
	virtual ~S75PlusStatus();

	// Status (base class) overrides

	BYTE* UIDPointer()						{ return status->UID;						}	
	BYTE* FirmWareVerPointer()				{ return &(status->FirmWareVer);			}
	BYTE* FirmWareRevPointer()				{ return &(status->FirmWareRev);			}
	WORD* RF_FIRMWAREPointer()				{ return &(status->RF_FIRMWARE);			}
	BYTE* DefaultGatewayIPPointer()			{ return status->DefaultGatewayIP;			}
	BYTE* DestinationIPPointer()			{ return status->UniCastStatusDestIP;		}
	BYTE* SubnetMaskPointer()				{ return status->SubnetMask;				}
	BYTE* ReceiverMACPointer()				{ return status->ReceiverMAC;				}
	BYTE* ReceiverIPPointer()				{ return status->ReceiverIP;				}
	WORD* DestinationUDPPointer()			{ return &(status->UniCastStatusDestUDP);	}
	WORD* EthernetPacketDroppedPointer()	{ return &(status->EthernetPacketDropped);	}
	WORD* EthernetReceivePointer()			{ return &(status->EthernetReceive);		}
	WORD* EthernetTransmitPointer()			{ return &(status->EthernetTransmit);		}
	WORD* DVBScrambledPointer()				{ return &(status->DVBScrambled);			}
	WORD* EthernetTransmitErrorPointer()	{ return NULL;								}
	WORD* WRESERVEDPointer()				{ return &(status->EthernetReceiveError);	}
	S75D_PIDList* PIDListPointer()			{ return &(status->PIDList);				}

	// SatelliteStatus (parent class) overrides

    BYTE* RFStatusValidPointer()			{ return &(status->RFStatusValid);	}
	float* SymbolRatePointer()				{ return &(status->SymbolRate);		}
	float* FrequencyPointer()				{ return &(status->Frequency);		}
	BYTE* PowerPointer()					{ return &(status->Power);			}
	BYTE* PolarityPointer()					{ return &(status->Polarity);		}
	BYTE* BandPointer()						{ return &(status->Band);			}
	BYTE* ViterbiRatePointer()				{ return &(status->ViterbiRate);	}
	BYTE* SignalLockPointer()				{ return &(status->SignalLock);		}
	BYTE* DataLockPointer()					{ return &(status->DataLock);		}
	BYTE* LNBFaultPointer()					{ return &(status->LNBFault);		}
	BYTE* UncorrectablesPointer()			{ return &(status->Uncorrectables);	}

	// Device Specific

	BYTE*			IGMP_EnablePointer()					{ return &(status->IGMP_Enable);				}
	BYTE*			Long_Line_CompPointer()					{ return &(status->Long_Line_Comp);				}
	BYTE*			Hi_VoltageModePointer()					{ return &(status->Hi_VoltageMode);				}
	BYTE*			PolaritySwitchingVoltagePointer()		{ return &(status->PolaritySwitchingVoltage);	}
	BYTE*			HiLoBandSwitchingTonePointer()			{ return &(status->HiLoBandSwitchingTone);		}
	BYTE*			SignalStrengthPointer()					{ return &(status->SignalStrength);				}
	float*			VBERPointer()							{ return &(status->VBER);						}
	float*			CarrierToNoisePointer()					{ return &(status->CarrierToNoise);				}
	BYTE*			DemodulatorGainPointer()				{ return &(status->DemodulatorGain);			}
	WORD*			EthernetReceiveErrorPointer()			{ return &(status->EthernetReceiveError);		}
	WORD*			DVBDescrambledPointer()					{ return &(status->DVBDescrambled);				}
	WORD*			DataSyncLossPointer()					{ return &(status->DataSyncLoss);				}
	WORD*			DVBAcceptedPointer()					{ return &(status->DVBAccepted);				}
	unsigned long*  TotalDVBPacketsAcceptedPointer()		{ return   status->TotalDVBPacketsAccepted;		}
	unsigned long*  TotalDVBPacketsRXInErrorPointer()		{ return   status->TotalDVBPacketsRXInError;	}
	unsigned long*  TotalEthernetPacketsOutPointer()		{ return   status->TotalEthernetPacketsOut;		}
	unsigned long*  TotalUncorrectableTSPacketsPointer()	{ return   status->TotalUncorrectableTSPackets;	}
	WORD*			BroadcastStatusPortPointer()			{ return &(status->BroadcastStatusPort);		}
	WORD*			LocalOscFrequencyPointer()				{ return &(status->LocalOscFrequency);			}

//	WORD* ProgramsPointer()					{ return status->Programs;			}

protected:
	S75Plus_Status *status;
};

#endif // !defined(AFX_S75CASTATUS_H__7421DA43_2A81_4DD6_A6EB_FE6F08B6B733__INCLUDED_)


