// S75JLEOStatus.h: interface for the S75JLEOStatus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_S75JLEOSTATUS_H__0A21EC18_B621_4222_9073_E63247F5AF7B__INCLUDED_)
#define AFX_S75JLEOSTATUS_H__0A21EC18_B621_4222_9073_E63247F5AF7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SatelliteStatus.h"

class S75JLEOStatus : public SatelliteStatus  
{
public:
	S75JLEOStatus( void *Container );
	virtual ~S75JLEOStatus();

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
	WORD* EthernetTransmitErrorPointer()	{ return &(status->EthernetTransmitError);	}
	S75D_PIDList* PIDListPointer()			{ return &(status->PIDList);				}

	// SatelliteStatus (parent class) overrides

    BYTE*  RFStatusValidPointer()			{ return &(status->RFStatusValid);	}
	float* SymbolRatePointer()				{ return &(status->SymbolRate);		}
	float* FrequencyPointer()				{ return &(status->Frequency);		}
	BYTE*  PowerPointer()					{ return &(status->Power);			}
	BYTE*  PolarityPointer()				{ return &(status->Polarity);		}
	BYTE*  BandPointer()					{ return &(status->Band);			}
	BYTE*  ViterbiRatePointer()				{ return &(status->ViterbiRate);	}
	BYTE*  SignalLockPointer()				{ return &(status->SignalLock);		}
	BYTE*  DataLockPointer()				{ return &(status->DataLock);		}
	BYTE*  LNBFaultPointer()				{ return &(status->LNBFault);		}
	BYTE*  UncorrectablesPointer()			{ return &(status->Uncorrectables); }

	// Device Specific

	BYTE*			IGMP_EnablePointer()					{ return &(status->IGMP_Enable);				}
	BYTE*			Long_Line_CompPointer()					{ return &(status->Long_Line_Comp);				}
	BYTE*			Hi_VoltageModePointer()					{ return &(status->Hi_VoltageMode);				}
	BYTE*			AutomaticGainControlPointer()			{ return &(status->AutomaticGainControl);		}
	float*			VBERPointer()							{ return &(status->VBER);						}
	float*			CarrierToNoisePointer()					{ return &(status->CarrierToNoise);				}
	BYTE*			DemodulatorGainPointer()				{ return &(status->DemodulatorGain);			}
	BYTE*			DVB_MACADDRESSPointer()					{ return   status->DVB_MACADDRESS;				}
	BYTE*			ReservedPointer()						{ return   status->Reserved;					}
	WORD*			WRESERVEDPointer()						{ return &(status->EthernetReceiveError);		}
	WORD*			EthernetReceiveErrorPointer()			{ return &(status->EthernetReceiveError);		}
	WORD*			FrontEndLockLossPointer()				{ return &(status->FrontEndLockLoss);			}
	WORD*			DataSyncLossPointer()					{ return &(status->DataSyncLoss);				}
	WORD*			DVBAcceptedPointer()					{ return &(status->DVBAccepted);				}
	unsigned long*  TotalDVBPacketsAcceptedPointer()		{ return   status->TotalDVBPacketsAccepted;		}
	unsigned long*  TotalDVBPacketsRXInErrorPointer()		{ return   status->TotalDVBPacketsRXInError;	}
	unsigned long*  TotalEthernetPacketsOutPointer()		{ return   status->TotalEthernetPacketsOut;		}
	unsigned long*  TotalUncorrectableTSPacketsPointer()	{ return   status->TotalUncorrectableTSPackets;	}
	WORD*			BroadcastStatusPortPointer()			{ return &(status->BroadcastStatusPort);		}
	WORD*			LocalOscFrequencyPointer()				{ return &(status->LocalOscFrequency);			}

protected:

	S75_Status_JLEO *status;

};

#endif // !defined(AFX_S75JLEOSTATUS_H__0A21EC18_B621_4222_9073_E63247F5AF7B__INCLUDED_)
