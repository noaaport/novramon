// ATSCDevice.cpp: implementation of the ATSCDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ATSCDevice.h"

#include <stdio.h>
#include <math.h>


const int RF_CONFIG_PORT = 0x1983;

const float _SYMBOL_RATE  = 10760000.0 ;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ATSCDevice::ATSCDevice()
{

//	m_ATSCStatus = (ATSCStatus_MSG *)(&m_Status);

}

ATSCDevice::~ATSCDevice()
{

}


int ATSCDevice::TuneDevice(void *TuningParameters)
{
	bool configurationSent;             // New configuration sent to S75D?
    bool configurationApplied;          // New configuration applied to S75D?
//    BYTE Buffer[50];                    // Buffer to hold data to send to S75D
	ATSCRFConfig_MSG pRFSettings;
    TCHAR temp[16];                       // Temporary storage variable
	int LastError;
//	m_pA75_RFSetting  = (ATSC_RFSetting *)TuningParameters ;		// Type Cast the parameters Passed into the structure 
//	pRFSettings = (A75RFConfig_MSG *) Buffer;		// Initialize the structure pointer to the Buffer

    // There are two possible methods of communicating -- by IP address or
    // by MAC address.  For the former, make sure the IP address is set.
    // For the later, make sure the network path is set

    if (!m_Device_attached)		    // Trying to communicate and the class not attached to physical device
		return ERR_RFCFG_NOIP;

	pRFSettings.ChannelNumber = htons( (unsigned short)( ((ATSC_RFSetting *)TuningParameters)->ChannelNumber) );
	pRFSettings.Reserved = 0;
	pRFSettings.BRESERVED = 0;

	// Now the Data is in the buffer and all is set and ready for transmission.
	// Now we construct the destination string.
	 sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);
	
	 for (int i = 0; i < NUM_RETRIES; i++) {
        configurationSent = (LastError = SendWaitForConfirmation( temp, RF_CONFIG_PORT, (BYTE *)(&pRFSettings), sizeof(ATSCRFConfig_MSG)) == ERR_SUCCESS);
        if (configurationSent) break;
    }

    // Check if we were able to send the configuration successfully
    if (configurationSent) {
        // Configuration sent to S75D

        // Flush the status packet buffer
        if ((LastError =FlushStatusBuffer(STATUS_PACKET_TIMEOUT)) !=ERR_SUCCESS ) {
            // Error flushing buffer
            // Don't need to define error code -- done by FlushStatusBuffer()
            return LastError;
        }
		    if ((LastError =FlushStatusBuffer(STATUS_PACKET_TIMEOUT)) !=ERR_SUCCESS ) {
            // Error flushing buffer
            // Don't need to define error code -- done by FlushStatusBuffer()
            return LastError;
        }

        // Wait for the next status packet
        if ((LastError = WaitForStatusPacket(STATUS_PACKET_TIMEOUT)) != ERR_SUCCESS) {
            // Error when waiting for status packet
            // Don't need to define error code -- done by WaitForStatusPacket()
            return LastError;
        }

        // Check to see if the values sent to the S75D are the same as those being
        // reported in the status
        configurationApplied = true;

        if ( ((CommonATSCStatus_MSG *)(&m_Status))->ChannelNumber != ((ATSC_RFSetting *)TuningParameters)->ChannelNumber ) 
			configurationApplied = false;

        
        if (!configurationApplied) 
			return ERR_RFCFG_NOTAPPLIED;
		else      // Configuration successfully applied
            return ERR_SUCCESS;
	}else 
		// Configuration not sent to S75D
		return LastError; // This will be the error returned by Send wait for confirmation function.


}


/*
bool ATSCDevice::ParseStatusPacket()
{

//	*(((ATSCStatusPacket *)m_StatusWrapper)->ChannelNumberPointer())	= ntohs(*(((ATSCStatusPacket *)m_StatusWrapper)->ChannelNumberPointer()));
//	*(((ATSCStatusPacket *)m_StatusWrapper)->Carrier_OffPointer())		= ntohs(*(((ATSCStatusPacket *)m_StatusWrapper)->Carrier_OffPointer()));
//	*(((ATSCStatusPacket *)m_StatusWrapper)->SNR_AEPointer())			= ntohs(*(((ATSCStatusPacket *)m_StatusWrapper)->SNR_AEPointer()));
//	*(((ATSCStatusPacket *)m_StatusWrapper)->SNR_BEPointer())			= ntohs(*(((ATSCStatusPacket *)m_StatusWrapper)->SNR_BEPointer()));
//	*(((ATSCStatusPacket *)m_StatusWrapper)->FEC_CorrPointer())			= ntohs(*(((ATSCStatusPacket *)m_StatusWrapper)->FEC_CorrPointer()));
//	*(((ATSCStatusPacket *)m_StatusWrapper)->FEC_UnCorrPointer())		= ntohs(*(((ATSCStatusPacket *)m_StatusWrapper)->FEC_UnCorrPointer()));
//	*(((ATSCStatusPacket *)m_StatusWrapper)->FEC_UnErrPointer())		= ntohs(*(((ATSCStatusPacket *)m_StatusWrapper)->FEC_UnErrPointer()));
//	*(((ATSCStatusPacket *)m_StatusWrapper)->IF_AGCPointer())			= ntohs(*(((ATSCStatusPacket *)m_StatusWrapper)->IF_AGCPointer()));
//	*(((ATSCStatusPacket *)m_StatusWrapper)->RFAGCPointer())			= ntohs(*(((ATSCStatusPacket *)m_StatusWrapper)->RFAGCPointer()));

	if ( !CNovraDevice::ParseStatusPacket() ) return false;

	return true;

}
*/

int ATSCDevice::GetStatus( ATSCStatus *pStatus, bool forward_raw, int max_pids )
{

	WORD *PIDList = (WORD *)(m_StatusWrapper->PIDListPointer());
	double Ex = 0.0;
	double ss = 0.0;
	signed short temp1;


	CNovraDevice::GetStatus( (Status *)pStatus, forward_raw, max_pids );

	// If the RF valid == 0x0400 then RFStatusvalid =1 (easier to read for external programmers...:P)
	*(pStatus->RFStatusValidPointer())	= ((*(m_StatusWrapper->RFStatusValidPointer())) == 0x0400)? 1:0;

temp1 = (*(((CommonATSCStatusPacket *)m_StatusWrapper)->ChannelNumberPointer()));
	*(pStatus->ChannelNumberPointer())	= (*(((CommonATSCStatusPacket *)m_StatusWrapper)->ChannelNumberPointer()));
	*(pStatus->FEC_CorrPointer())		= (*(((CommonATSCStatusPacket *)m_StatusWrapper)->FEC_CorrPointer()));
	*(pStatus->FEC_UnCorrPointer())		= (*(((CommonATSCStatusPacket *)m_StatusWrapper)->FEC_UnCorrPointer()));
	*(pStatus->FEC_UnErrPointer())		= (*(((CommonATSCStatusPacket *)m_StatusWrapper)->FEC_UnErrPointer()));
	*(pStatus->EthernetTransmitErrorPointer())	= (*(((CommonATSCStatusPacket *)m_StatusWrapper)->EthernetTXErrPointer()));
	(pStatus->WRESERVEDPointer())[1]	= (*(((CommonATSCStatusPacket *)m_StatusWrapper)->FEL_LossPointer()));

	temp1					= (*(((CommonATSCStatusPacket *)m_StatusWrapper)->Carrier_OffPointer()));
	*(pStatus->Carrier_OffPointer()) = (float)((temp1* _SYMBOL_RATE) / 65535.0);

	*(pStatus->IF_AGCPointer())			= (float)((*(((CommonATSCStatusPacket *)m_StatusWrapper)->IF_AGCPointer())) / 1024.0);
	*(pStatus->RFAGCPointer())			= (float)((*(((CommonATSCStatusPacket *)m_StatusWrapper)->RFAGCPointer())) / 1024.0);

	*(pStatus->DeModStatsPointer())		= (*(((CommonATSCStatusPacket *)m_StatusWrapper)->DeModStatsPointer()));
	
	if((*(((CommonATSCStatusPacket *)m_StatusWrapper)->VSBModePointer())) == 0x06){
		*(pStatus->VSBModePointer()) = 8 ;
		Ex = 21.0/64.0; //actually Ex squared ...!!!
	} else if((*(((CommonATSCStatusPacket *)m_StatusWrapper)->VSBModePointer())) == 0x05){
		*(pStatus->VSBModePointer()) = 16 ;
		Ex = 85.0/256.0; ////actually Ex squared ...!!!
	} else {
		*(pStatus->VSBModePointer()) = 0;
		Ex = 1.0;
	}


	ss = (Ex)/(((*(((CommonATSCStatusPacket *)m_StatusWrapper)->SNR_AEPointer()))/65535.0) * ((*(((CommonATSCStatusPacket *)m_StatusWrapper)->SNR_AEPointer()))/65535.0) *3.14/2.0);

	*(pStatus->SNR_AEPointer())			= (float)(10*log10(ss));

	ss = (Ex)/(((*(((CommonATSCStatusPacket *)m_StatusWrapper)->SNR_BEPointer()))/65535.0) * ((*(((CommonATSCStatusPacket *)m_StatusWrapper)->SNR_BEPointer()))/65535.0) *3.14/2.0);
	*(pStatus->SNR_BEPointer())			=	(float)(10*log10(ss));
	
	if( (*(pStatus->DeModStatsPointer())) &0x10) *(pStatus->SNR_AEPointer()) -= 3;


	*(pStatus->SER_10Pointer())			= (*(((CommonATSCStatusPacket *)m_StatusWrapper)->SER_10Pointer()));
	*(pStatus->SER_1Pointer())			= (*(((CommonATSCStatusPacket *)m_StatusWrapper)->SER_1Pointer()));

	
	*(pStatus->FieldLockedPointer())	= *(pStatus->DeModStatsPointer()) &0x01 ? 1 : 0;
	*(pStatus->EqualizerStatePointer()) = *(pStatus->DeModStatsPointer()) &0x2 ? 1 : 0;
	*(pStatus->ChannelStatePointer())	= (*(pStatus->DeModStatsPointer()) &0x0C)>>2 ;
	*(pStatus->NTSC_FilterPointer())	= *(pStatus->DeModStatsPointer()) &0x10 ? 1 : 0;
	*(pStatus->InverseSpectrumPointer()) = *(pStatus->DeModStatsPointer()) & 0x80 ? 1 : 0;

	
	*(pStatus->IGMP_EnablePointer()) = ((*(((CommonStatusPacket *)m_StatusWrapper)->FilterPointer())) & 0x04) >> 2;

	// Copy the PID list

	for ( int i = 0; i < max_pids; i++ ) {

    	pStatus->PIDListPointer()->Entry[i] = PIDList[i] & 0x1fff;

		if ( forward_raw ) {
			pStatus->PIDListPointer()->Control[i] = (PIDList[i] & 0xE000) >> 13 ;	
		} else {
			pStatus->PIDListPointer()->Control[i] = (PIDList[i] & 0x2000) >> 13 ;	
		}

	}

	TrimPIDList((LPBYTE)(pStatus->PIDListPointer()), max_pids );

	return ERR_SUCCESS ;
}
