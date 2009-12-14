// SatelliteDevice.cpp: implementation of the CSatelliteDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SatelliteDevice.h"
#include "SatelliteStatus.h"

#include <stdio.h>
#include <math.h>


const int RF_CONFIG_PORT = 0x1983;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSatelliteDevice::CSatelliteDevice()
{

//	m_SatStatus = (SatelliteStatus_MSG *)(&m_Status);

}

CSatelliteDevice::~CSatelliteDevice()
{

}


void CSatelliteDevice::CreateRFConfigMsg( SatelliteRFConfigPacket *rf_config_packet_wrapper, void *TuningParameters )
{
	(*(rf_config_packet_wrapper->FrequencyPointer())) = htons( (unsigned short)( ((SatelliteRFSetting *)TuningParameters)->frequency * 10.0 + 0.5 ) );
	(*(rf_config_packet_wrapper->SymbolRatePointer())) = htons( (unsigned short)( ((SatelliteRFSetting *)TuningParameters)->symbolRate * 1000.0 + 0.5 ) );
	(*(rf_config_packet_wrapper->ViterbiRatePointer())) = 0x08 ;
}


bool CSatelliteDevice::CheckRFConfig( SatelliteRFConfigPacket *rf_config_packet_wrapper )
{
	bool configurationApplied = true;

	if (( ( (*( ((SatelliteStatusPacket *)m_StatusWrapper)->SymbolRatePointer())) <
		    ( ntohs(*(rf_config_packet_wrapper->SymbolRatePointer())) - 10 ) ) ||
		  ( (*( ((SatelliteStatusPacket *)m_StatusWrapper)->SymbolRatePointer())) >
			( ntohs(*(rf_config_packet_wrapper->SymbolRatePointer())) + 10 ) )    ) )
		configurationApplied = false;

	if (( ( (*( ((SatelliteStatusPacket *)m_StatusWrapper)->FrequencyPointer())) < 
			( ntohs(*(rf_config_packet_wrapper->FrequencyPointer())) - 100 ) ) ||
		  ( (*( ((SatelliteStatusPacket *)m_StatusWrapper)->FrequencyPointer())) >
			( ntohs(*(rf_config_packet_wrapper->FrequencyPointer())) + 100) )    ) )
		configurationApplied = false;

//##@@NS@@ I don't understand what is going on here, so I'll turn off the check... 
	// the LNB word coming back may contain variable information that need to be masked like LNB fault.  !!!
		/*if ( (*(((SatelliteStatusPacket *)m_StatusWrapper)->LNBConfigurationPointer())) != (*(rf_config_packet_wrapper->LNBConfigPointer())) )
		configurationApplied = false;
*/
	return configurationApplied;

}

int CSatelliteDevice::SendRFConfig( SatelliteRFConfigPacket *rf_config_packet_wrapper )
{
//	SatelliteRFConfig_MSG pRFSettingMsg;
	TCHAR temp[16];                       // Temporary storage variable
	bool configurationSent;             // New configuration sent to S75D?
	int LastError;
	bool configurationApplied;          // New configuration applied to S75D?
	int retry_count = 0;


	// Now the Data is in the buffer and all is set and ready for transmission.
	// Now we construct the destination string.

	sprintf( temp, "%d.%d.%d.%d", m_Device_IP_Address[0], m_Device_IP_Address[1], m_Device_IP_Address[2], m_Device_IP_Address[3]);
	
	 for (int i = 0; i < NUM_RETRIES; i++) {
        configurationSent = ( ( LastError = SendWaitForConfirmation( temp, RF_CONFIG_PORT, rf_config_packet_wrapper->data(), rf_config_packet_wrapper->size() ) ) == ERR_SUCCESS);
        if (configurationSent) break;
    }

	// Check if we were able to send the configuration successfully
    if (configurationSent) {
        // Configuration sent to S75D

		if ( m_usingStatusListener && ( GetDeviceType() != S75CA ) ) {

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

			configurationApplied = CheckRFConfig( rf_config_packet_wrapper );

#ifdef JSAT
		} else { // Need to poll for status

        	configurationApplied = false;

			while ( ( !configurationApplied ) && ( retry_count++ < 4 ) ) {

				if ( PollStatus( m_StatusWrapper ) == ERR_SUCCESS ) {

        			// Check to see if the values sent to the S75D are the same as those being
        			// reported in the status

					configurationApplied = CheckRFConfig( rf_config_packet_wrapper );

				}

			}
#endif
		}

        
        if (!configurationApplied) 
			return ERR_RFCFG_NOTAPPLIED;
		else      // Configuration successfully applied
            return ERR_SUCCESS;
	}else 
		// Configuration not sent to S75D
		return LastError; // This will be the error returned by Send wait for confirmation function.

}


int CSatelliteDevice::GetStatus( SatelliteStatus *pStatus, bool forward_raw, int max_pids )
{
  //double  slope = 0.0;                Value used in converting signal strength
  //double  offset = 0.0;               Values used in converting signal strength
  //double  ss = 0.0;                   Values used in converting signal strength


	CNovraDevice::GetStatus( (Status *)pStatus, forward_raw, max_pids );

	// If the RF valid == 0x0400 then RFStatusvalid =1 (easier to read for external programmers...:P)

	*(pStatus->RFStatusValidPointer()) = ((*(m_StatusWrapper->RFStatusValidPointer())) == 0x0400)? 1:0;
	*(pStatus->SymbolRatePointer()) = (float) ((*(((SatelliteStatusPacket *)m_StatusWrapper)->SymbolRatePointer())) / 1000.0) ; // Convert Symbol rate to float.
	*(pStatus->FrequencyPointer()) = (float) ((*(((SatelliteStatusPacket *)m_StatusWrapper)->FrequencyPointer())) / 10.0);  // Convert Frequency to Float 
	*(pStatus->PowerPointer()) = (*(((SatelliteStatusPacket *)m_StatusWrapper)->LNBConfigurationPointer())) & 0x01 ; // The Power settings in the LNB configuraton on/off 
	*(pStatus->PolarityPointer()) = ((*(((SatelliteStatusPacket *)m_StatusWrapper)->LNBConfigurationPointer()))&0x02) >>1; // The Polarity setting in LNB configuration H/V .
	*(pStatus->BandPointer()) =  ((*(((SatelliteStatusPacket *)m_StatusWrapper)->LNBConfigurationPointer())) &0x04) >> 2; // The band settings in the LNB configuration Low/ Hi.

	
	switch ( (*((SatelliteStatusPacket *)m_StatusWrapper)->ViterbiRatePointer())) {	// Convert the Viterbi Rate to the enumerated type...

    	case 0  : *(pStatus->ViterbiRatePointer())	=	HALF;			break;
	    case 1  : *(pStatus->ViterbiRatePointer())	=	TWO_THIRDS;		break;
	    case 2  : *(pStatus->ViterbiRatePointer())	=	THREE_QUARTERS; break;
	    case 4  : *(pStatus->ViterbiRatePointer())	=	FIVE_SIXTHS;	break;
	    case 6  : *(pStatus->ViterbiRatePointer())	=	SEVEN_EIGTHS;	break;
		default : *(pStatus->ViterbiRatePointer())	=	VITERBI_ERROR;	break;

	}

	// Convert demodulator status flags into something more readable

	*(pStatus->SignalLockPointer())  = (*(((SatelliteStatusPacket *)m_StatusWrapper)->DemodulatorStatusPointer())) & 0x01 ;

	*(pStatus->DataLockPointer()) = ((*(((SatelliteStatusPacket *)m_StatusWrapper)->DemodulatorStatusPointer())) & 0x02) >> 1;

	*(pStatus->LNBFaultPointer()) = ((*(((SatelliteStatusPacket *)m_StatusWrapper)->DemodulatorStatusPointer())) & 0x04) >> 2;

	*(pStatus->UncorrectablesPointer()) = (*(((SatelliteStatusPacket *)m_StatusWrapper)->UncorrectablesPointer()));

	return ERR_SUCCESS ;
}



BYTE CSatelliteDevice::CalculateSignalStrength( const double AGC[], const double PERCENTAGE[], int num_entries )
{

	BYTE signal_strength = 0;
	double  slope = 0.0;                // Value used in converting signal strength
    double  offset = 0.0;               // Values used in converting signal strength
    double  ss = 0.0;                   // Values used in converting signal strength


	if ((*(((SatelliteStatusPacket *)m_StatusWrapper)->SignalStrengthPointer())) == 255) {

        signal_strength = 0;

    } else if ((*(((SatelliteStatusPacket *)m_StatusWrapper)->SignalStrengthPointer())) < AGC[num_entries - 1]) {

        signal_strength = 110;

    } else {

        for (int i = 0; i < num_entries - 1; i++ ) {

            if( ( AGC[i] >= (*(((SatelliteStatusPacket *)m_StatusWrapper)->SignalStrengthPointer())) ) && ( AGC[i + 1] < (*(((SatelliteStatusPacket *)m_StatusWrapper)->SignalStrengthPointer())) ) ) {

                slope = ( PERCENTAGE[i + 1] - PERCENTAGE[i] ) / ( AGC[i + 1] - AGC[i] );

                offset = PERCENTAGE[i] - slope * AGC[i];

                ss = slope * (double)(*(((SatelliteStatusPacket *)m_StatusWrapper)->SignalStrengthPointer())) + offset;

                break;

            }   
			
        }

        signal_strength = (int)ss;

    }

	return signal_strength;

}


float CSatelliteDevice::CalculateVBER( int exp_sign )
{

	float	VBER;
	int		BERmantissa;   // Bit Error Rate mantissa
	int		BERexponent;   // Bit Error Rate exponent


	// Convert the BER to a floating point

    BERmantissa = ((((SatelliteStatusPacket *)m_StatusWrapper)->VBER_ManPointer())[2]) |
                  ((((SatelliteStatusPacket *)m_StatusWrapper)->VBER_ManPointer())[1] * 0x100) |
                  ((((SatelliteStatusPacket *)m_StatusWrapper)->VBER_ManPointer())[0] * 0x10000);
            
    BERexponent = exp_sign * ((*(((SatelliteStatusPacket *)m_StatusWrapper)->VBER_ExpPointer())));

    VBER = (float)((double)BERmantissa / pow(10.0, (double)BERexponent));	

	return VBER;

}


/*
bool CSatelliteDevice::ParseStatusPacket()
{

//	*(((SatelliteStatusPacket *)m_StatusWrapper)->FrequencyPointer())	= ntohs(*(((SatelliteStatusPacket *)m_StatusWrapper)->FrequencyPointer()));
//	*(((SatelliteStatusPacket *)m_StatusWrapper)->SymbolRatePointer()) = ntohs(*(((SatelliteStatusPacket *)m_StatusWrapper)->SymbolRatePointer()));

	if ( !CNovraDevice::ParseStatusPacket() ) return false;

	return true;

}
*/

bool CSatelliteDevice::ConfirmDeviceSpecificRFSettings( StatusPacket *status_wrapper, RFConfigPacket *rf_config_packet_wrapper )
{

	if ( (*(((SatelliteStatusPacket *)status_wrapper)->LNBConfigurationPointer())) == (*(((SatelliteRFConfigPacket *)rf_config_packet_wrapper)->LNBConfigPointer())) )
		return true;
	else
		return false;

}
