// StatusPacket.cpp: implementation of the StatusPacket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StatusPacket.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


void StatusPacket::ntoh()
{
	*(DSPFirmWarePointer())			= ntohs(*(DSPFirmWarePointer()));
	*(StatusDestUDPPointer())		= ntohs(*(StatusDestUDPPointer()));
	*(EthernetDroppedPointer())		= ntohs(*(EthernetDroppedPointer()));
	*(EthernetRXPointer())			= ntohs(*(EthernetRXPointer()));
	*(EthernetRXErrPointer())		= ntohs(*(EthernetRXErrPointer()));
	*(EthernetTXPointer())			= ntohs(*(EthernetTXPointer()));
//	*(EthernetTXErrPointer())		= ntohs(*(EthernetTXErrPointer()));
//	*(FEL_LossPointer())			= ntohs(*(FEL_LossPointer()));
	*(MicroFirmWarePointer())		= ntohs(*(MicroFirmWarePointer()));
	*(Sync_LossPointer())			= ntohs(*(Sync_LossPointer()));
	*(TotalDVBAcceptedPointer())	= ntohs(*(TotalDVBAcceptedPointer()));
}


void ATSCStatusPacket::ntoh()
{
	CommonStatusPacket::ntoh();

	*(ChannelNumberPointer())	= ntohs(*(ChannelNumberPointer()));
	*(Carrier_OffPointer())		= ntohs(*(Carrier_OffPointer()));
	*(SNR_AEPointer())			= ntohs(*(SNR_AEPointer()));
	*(SNR_BEPointer())			= ntohs(*(SNR_BEPointer()));
	*(FEC_CorrPointer())		= ntohs(*(FEC_CorrPointer()));
	*(FEC_UnCorrPointer())		= ntohs(*(FEC_UnCorrPointer()));
	*(FEC_UnErrPointer())		= ntohs(*(FEC_UnErrPointer()));
	*(IF_AGCPointer())			= ntohs(*(IF_AGCPointer()));
	*(RFAGCPointer())			= ntohs(*(RFAGCPointer()));
	*(RFStatusValidPointer())	= ntohs(*(RFStatusValidPointer()));
	*(EthernetTXErrPointer())	= ntohs(*(EthernetTXErrPointer()));
	*(FEL_LossPointer())		= ntohs(*(FEL_LossPointer()));
}


void SatelliteStatusPacket::ntoh()
{
	CommonStatusPacket::ntoh();

	*(FrequencyPointer())		= ntohs(*(FrequencyPointer()));
	*(SymbolRatePointer())		= ntohs(*(SymbolRatePointer()));
}


void CommonStatusPacket::ntoh()
{
	StatusPacket::ntoh();
}


void CommonATSCStatusPacket::ntoh()
{
	ATSCStatusPacket::ntoh();
}


void CommonSatelliteStatusPacket::ntoh()
{
	SatelliteStatusPacket::ntoh();

	*(EthernetTXErrPointer()) = ntohs(*(EthernetTXErrPointer()));
	*(FEL_LossPointer()) = ntohs(*(FEL_LossPointer()));
}


void A75StatusPacket::ntoh()
{
	CommonATSCStatusPacket::ntoh();

	for(int i=0;i<MAX_SUPP_PIDS;i++)
		(PIDListPointer())[i] = ntohs((PIDListPointer())[i]);
}


void A75PesMakerStatusPacket::ntoh()
{
	CommonATSCStatusPacket::ntoh();

	for(int i=0;i<MAX_SUPP_PIDS;i++)
		(PIDListPointer())[i] = ntohs((PIDListPointer())[i]);
}


void S75V2StatusPacket::ntoh()
{
	CommonSatelliteStatusPacket::ntoh();

	for(int i=0;i<MAX_SUPP_PIDS;i++) 
		(PIDListPointer())[i] = ntohs((PIDListPointer())[i]);
}


void S75V3StatusPacket::ntoh()
{
	CommonSatelliteStatusPacket::ntoh();

	for(int i=0;i<MAX_SUPP_PIDS;i++)
		(PIDListPointer())[i] = ntohs((PIDListPointer())[i]);
}


void PesMakerStatusPacket::ntoh()
{
	CommonSatelliteStatusPacket::ntoh();

	for(int i=0;i<MAX_SUPP_PIDS;i++)
		(PIDListPointer())[i] = ntohs((PIDListPointer())[i]);
}


void S75PlusStatusPacket::ntoh()
{
	int i;

//	CommonSatelliteStatusPacket::ntoh();
	SatelliteStatusPacket::ntoh();

//	for(i=0;i<16;i++)
//		(ProgramsPointer())[i] = ntohs((ProgramsPointer())[i]);

	*(Freq_ErrPointer()) = ntohs(*(Freq_ErrPointer()));
/*
	(TotalDVBPacketsAcceptedPointer())[0]	 	= ntohl((TotalDVBPacketsAcceptedPointer())[0]);
	(TotalDVBPacketsAcceptedPointer())[1]		= ntohl((TotalDVBPacketsAcceptedPointer())[1]);
	(TotalDVBPacketsRXInErrorPointer())[0]		= ntohl((TotalDVBPacketsRXInErrorPointer())[0]);
	(TotalDVBPacketsRXInErrorPointer())[1]		= ntohl((TotalDVBPacketsRXInErrorPointer())[1]);
	(TotalEthernetPacketsOutPointer())[0]		= ntohl((TotalEthernetPacketsOutPointer())[0]);
	(TotalEthernetPacketsOutPointer())[1]		= ntohl((TotalEthernetPacketsOutPointer())[1]);
	(TotalUncorrectableTSPacketsPointer())[0]	= ntohl((TotalUncorrectableTSPacketsPointer())[0]);
	(TotalUncorrectableTSPacketsPointer())[1]	= ntohl((TotalUncorrectableTSPacketsPointer())[1]);
*/
	*(DVBScrambledPointer())					= ntohs(*(DVBScrambledPointer()));
	*(DVBDescrambledPointer())					= ntohs(*(DVBDescrambledPointer()));
	*(BroadcastStatusPortPointer())				= ntohs(*(BroadcastStatusPortPointer()));
	*(LocalOscFrequencyPointer())				= ntohs(*(LocalOscFrequencyPointer()));

	for(i=0;i<2*MAX_SUPP_PIDS;i++)
		(PIDListPointer())[i] = ntohs((PIDListPointer())[i]);

	for ( i=0; i < NUMBER_OF_DESTINATIONS; i++ ) {
		(PIDDestinationsPointer())[i].DestinationUDP = ntohs((PIDDestinationsPointer())[i].DestinationUDP);
	}

	for ( i=0; i < NUMBER_OF_PROGRAMS; i++ ) {
		(ProgramListPointer())[i] = ntohs((ProgramListPointer())[i]);
	}

	for ( i=0; i < NUMBER_OF_PROGRAMS; i++ ) {
		(ProgramStatusPointer())[i] = ntohs((ProgramStatusPointer())[i]);
	}
}


void S75JLEOStatusPacket::ntoh()
{
	SatelliteStatusPacket::ntoh();

	*(FilterPointer()) = ntohs(*(FilterPointer())); 

	*(RFStatusValidPointer())	= ntohs(*(RFStatusValidPointer()));

	*(EthernetTXErrPointer())	= ntohs(*(EthernetTXErrPointer()));
	*(FEL_LossPointer())		= ntohs(*(FEL_LossPointer()));

	(TotalDVBPacketsAcceptedPointer())[0]	 	= ntohl((TotalDVBPacketsAcceptedPointer())[0]);
	(TotalDVBPacketsAcceptedPointer())[1]		= ntohl((TotalDVBPacketsAcceptedPointer())[1]);
	(TotalDVBPacketsRXInErrorPointer())[0]		= ntohl((TotalDVBPacketsRXInErrorPointer())[0]);
	(TotalDVBPacketsRXInErrorPointer())[1]		= ntohl((TotalDVBPacketsRXInErrorPointer())[1]);
	(TotalEthernetPacketsOutPointer())[0]		= ntohl((TotalEthernetPacketsOutPointer())[0]);
	(TotalEthernetPacketsOutPointer())[1]		= ntohl((TotalEthernetPacketsOutPointer())[1]);
	(TotalUncorrectableTSPacketsPointer())[0]	= ntohl((TotalUncorrectableTSPacketsPointer())[0]);
	(TotalUncorrectableTSPacketsPointer())[1]	= ntohl((TotalUncorrectableTSPacketsPointer())[1]);

	*(BroadcastStatusPortPointer())				= ntohs(*(BroadcastStatusPortPointer()));
	*(LocalOscFrequencyPointer())				= ntohs(*(LocalOscFrequencyPointer()));

	for(int i=0;i<MAX_SUPP_PIDS;i++)
		(PIDListPointer())[i] = ntohs((PIDListPointer())[i]);
}
