//#include "stdafx.h"

#ifdef WINDOWS
#ifndef  _WinSock2Included_
	#define	 _WinSock2Included_
	#include <winsock2.h>
#endif
#endif

#include "tinyxml.h"

#ifndef WINDOWS
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <unistd.h>

	typedef unsigned char   BYTE;
	typedef unsigned char * LPBYTE;
#endif

#include "Receiver.h"
#include "Password.h"
//#include "ParameterValue.h"
#include <time.h>
#ifndef WINDOWS
	#include <sys/time.h>
#endif



typedef ParameterValue* ParameterValue_ptr;


pstruct m_sec;


Receiver::Receiver()
{
	parameterMask = new unsigned short[MAX_RECEIVER_PARAM];
	pendingParameters = new ParameterValue_ptr[MAX_RECEIVER_PARAM];
	currentParameters = new ParameterValue_ptr[MAX_RECEIVER_PARAM];
	dirtyFlags = new bool[MAX_RECEIVER_PARAM];
	memset( parameterMask, 0, MAX_RECEIVER_PARAM * sizeof( unsigned short ) );
	memset( pendingParameters, 0, MAX_RECEIVER_PARAM * sizeof( ParameterValue_ptr) );
	memset( currentParameters, 0, MAX_RECEIVER_PARAM * sizeof( ParameterValue_ptr) );
	memset( dirtyFlags, 0, MAX_RECEIVER_PARAM * sizeof( bool ) );
	m_remote = false;
//	s = 0;
}


Receiver::~Receiver()
{
	int i;

	if ( parameterMask ) {
		delete [] parameterMask;
	}
	for ( i = 0; i < MAX_RECEIVER_PARAM; i++ ) {
		if ( pendingParameters[i] ) delete pendingParameters[i];
		if ( currentParameters[i] ) delete currentParameters[i];
	}
	delete [] pendingParameters;
	delete [] currentParameters;
	delete [] dirtyFlags;

}


void Receiver::initAttributes()
{

	for ( int i = 0; i < MAX_RECEIVER_PARAM; i++ ) {
		if ( parameterMask[i] & F_EXISTS ) {
			if ( ( parameterMask[i] & F_READ ) || ( parameterMask[i] & F_WRITE ) ) {
				if ( parameterMask[i] & F_ENUM ) {
					currentParameters[i] = new EnumeratedReceiverParameter( (ReceiverParameter)i );
				} else {
					currentParameters[i] = new ParameterValue();
				}
			}
			if ( parameterMask[i] & F_WRITE ) {
				if ( parameterMask[i] & F_ENUM ) {
					pendingParameters[i] = new EnumeratedReceiverParameter( (ReceiverParameter)i );
				} else {
					pendingParameters[i] = new ParameterValue();
				}
				dirtyFlags[i] = false;
			}
		}
	}

}


void Receiver::cancel()
{

	for ( int i = 0; i < MAX_RECEIVER_PARAM; i++ ) {
		if ( parameterMask[i] & F_EXISTS ) {
			if ( parameterMask[i] & F_WRITE ) {
				if ( currentParameters[i]->isSet() ) {
					pendingParameters[i]->setM_value( currentParameters[i]->getM_value() );
				} else {
					pendingParameters[i]->setM_isSet( false );
				}
				dirtyFlags[i] = false;
			}
		}
	}
	cancelIndexedParameters();

}


void Receiver::clear()
{

	for ( int i = 0; i < MAX_RECEIVER_PARAM; i++ ) {
		if ( parameterMask[i] & F_EXISTS ) {
			if ( parameterMask[i] & F_WRITE ) {
				pendingParameters[i]->setM_value( "0" );
				dirtyFlags[i] = true;
			}
		}
	}
	clearIndexedParameters();

}


string Receiver::enumToString( ReceiverParameter ParameterName, int enumeratedValue )
{
        char temp[10];
	string return_str = "";
	int masked_id;

	if ( ( ParameterName == ALL_PARAMETERS ) || ( (  parameterMask[ParameterName] & F_EXISTS ) != 0 ) ) {
		switch ( ParameterName ) {

		case ALL_PARAMETERS:	switch ( enumeratedValue ) {
					case ALL_PARAMETERS				:	return_str = "ALL_PARAMETERS";
												break;
					case STATUS_TIMESTAMP				:	return_str = "STATUS_TIMESTAMP";
												break;
					case DEVICE_TYPE				:	return_str = "DEVICE_TYPE";
												break;
					case UNIT_ID_CODE				:	return_str = "UNIT_ID_CODE";
												break;
					case DSP_VERSION 				:	return_str = "DSP_VERSION";
												break;
					case DSP_REVISION 				:	return_str = "DSP_REVISION";
												break;
					case RF_FIRMWARE_VERSION			:	return_str = "RF_FIRMWARE_VERSION";
												break;
					case CAM_VERSION				:	return_str = "CAM_VERSION";
												break;
					case CAM_REVISION				:	return_str = "CAM_REVISION";
												break;
					case FPGA_VERSION				:	return_str = "FPGA_VERSION";
												break;
					case FPGA_REVISION				:	return_str = "FPGA_REVISION";
												break;
					case RECEIVER_MAC				:	return_str = "RECEIVER_MAC";
												break;
					case SIZE_PID_LIST				:	return_str = "SIZE_PID_LIST";
												break;
					case SIZE_CA_PROGRAM_LIST			:	return_str = "SIZE_CA_PROGRAM_LIST";
												break;
					case SIZE_DESTINATION_LIST			:	return_str = "SIZE_DESTINATION_LIST";
												break;
					case SIZE_MINI_PAT_LIST 			:	return_str = "SIZE_MINI_PAT_LIST";
												break;
					case SIZE_FIXED_KEY_LIST 			:	return_str = "SIZE_FIXED_KEY_LIST";
												break;
					case SIZE_IP_REMAP_LIST 			:	return_str = "SIZE_IP_REMAP_LIST";
												break;
					case SIZE_VPROGRAM_LIST 			:	return_str = "SIZE_VPROGRAM_LIST";
												break;
					case SIZE_PROGRAM_GUIDE 			:	return_str = "SIZE_PROGRAM_GUIDE";
												break;
					case SIZE_AUDIO_PID_LIST 			:	return_str = "SIZE_AUDIO_PID_LIST";
												break;
					case CODE_DOWNLOAD 				:	return_str = "CODE_DOWNLOAD";
												break;
					case FACTORY_RESET 				:	return_str = "FACTORY_RESET";
												break;
					case AUTO_SYMBOL_RATE	 			:	return_str = "AUTO_SYMBOL_RATE";
												break;
					case AUTO_DVBS_TYPE 				:	return_str = "AUTO_DVBS_TYPE";
												break;
					case FRONT_END_LOCK_LOSS 			:	return_str = "FRONT_END_LOCK_LOSS";
												break;
					case DATA_SYNC_LOSS				:	return_str = "DATA_SYNC_LOSS";
												break;
					case CARRIER_FREQUENCY 				:	return_str = "CARRIER_FREQUENCY";
												break;
					case DVB_SIGNAL_TYPE 				:	return_str = "DVB_SIGNAL_TYPE";
												break;
					case VITERBI_RATE 				:	return_str = "VITERBI_RATE";
												break;
					case MODCOD					:	return_str = "MODCOD";
												break;
					case SIGNAL_STRENGTH_AS_DBM 			:	return_str = "SIGNAL_STRENGTH_AS_DBM";
												break;
					case SIGNAL_STRENGTH_AS_PERCENTAGE 		:	return_str = "SIGNAL_STRENGTH_AS_PERCENTAGE";
												break;
					case SIGNAL_LOCK				:	return_str = "SIGNAL_LOCK";
												break;
					case DATA_LOCK 					:	return_str = "DATA_LOCK";
												break;
					case LNB_FAULT 					:	return_str = "LNB_FAULT";
												break;
					case VBER					:	return_str = "VBER";
												break;
					case PER 					:	return_str = "PER";
												break;
					case UNCORRECTABLES 				:	return_str = "UNCORRECTABLES";
												break;
					case AGC					:	return_str = "AGC";
												break;
					case CARRIER_TO_NOISE				:	return_str = "CARRIER_TO_NOISE";
												break;
					case DEMODULATOR_GAIN 				:	return_str = "DEMODULATOR_GAIN";
												break;
					case AGCA 					:	return_str = "AGCA";
												break;
					case AGCN 					:	return_str = "AGCN";
												break;
					case GNYQA 					:	return_str = "GNYQA";
												break;
					case GFARA 					:	return_str = "GFARA";
												break;
					case NEST 					:	return_str = "NEST";
												break;
					case CLOCK_OFFSET 				:	return_str = "CLOCK_OFFSET";
												break;
					case FREQUENCY_OFFSET 				:	return_str = "FREQUENCY_OFFSET";
												break;
					case FREQUENCY_ERROR 				:	return_str = "FREQUENCY_ERROR";
												break;
					case LOCKED_SYMBOL_RATE				:	return_str = "LOCKED_SYMBOL_RATE";
												break;
					case ADC_MID 					:	return_str = "ADC_MID";
												break;
					case ADC_CLIP 					:	return_str = "ADC_CLIP";
												break;
					case DIGITAL_GAIN 				:	return_str = "DIGITAL_GAIN";
												break;
					case AA_CLAMP 					:	return_str = "AA_CLAMP";
												break;
					case SPECTRAL_INVERSION_FLAG 			:	return_str = "SPECTRAL_INVERSION_FLAG";
												break;
					case PILOT_SYMBOL_FLAG 				:	return_str = "PILOT_SYMBOL_FLAG";
												break;
					case FRAME_LENGTH 				:	return_str = "FRAME_LENGTH";
												break;
					case DEMOD_STATS 				:	return_str = "DEMOD_STATS";
												break;
					case VSB_MODE 					:	return_str = "VSB_MODE";
												break;
					case SER_1 					:	return_str = "SER_1";
												break;
					case SER_10 					:	return_str = "SER_10";
												break;
					case SNR_BE 					:	return_str = "SNR_BE";
												break;
					case SNR_AE 					:	return_str = "SNR_AE";
												break;
					case CARRIER_OFFSET 				:	return_str = "CARRIER_OFFSET";
												break;
					case IF_AGC 					:	return_str = "IF_AGC";
												break;
					case RF_AGC 					:	return_str = "RF_AGC";
												break;
					case FEC_UNERR 					:	return_str = "FEC_UNERR";
												break;
					case FEC_CORR 					:	return_str = "FEC_CORR";
												break;
					case FEC_UNCORR 				:	return_str = "FEC_UNCORR";
												break;
					case FIELD_LOCKED 				:	return_str = "FIELD_LOCKED";
												break;
					case EQUALIZER_STATE 				:	return_str = "EQUALIZER_STATE";
												break;
					case CHANNEL_STATE 				:	return_str = "CHANNEL_STATE";
												break;
					case NTSC_FILTER 				:	return_str = "NTSC_FILTER";
												break;
					case INVERSE_SPECTRUM 				:	return_str = "INVERSE_SPECTRUM";
												break;
					case PID_COUNT 					:	return_str = "PID_COUNT";
												break;
					case PROGRAM_GUIDE_PROGRAM_NUMBER 		:	return_str = "PROGRAM_GUIDE_PROGRAM_NUMBER";
												break;
					case PROGRAM_GUIDE_PROGRAM_NAME 		:	return_str = "PROGRAM_GUIDE_PROGRAM_NAME";
												break;
					case DVB_ACCEPTED 				:	return_str = "DVB_ACCEPTED";
												break;
					case TOTAL_DVB_PACKETS_ACCEPTED 		:	return_str = "TOTAL_DVB_PACKETS_ACCEPTED";
												break;
					case TOTAL_DVB_PACKETS_RX_IN_ERROR 		:	return_str = "TOTAL_DVB_PACKETS_RX_IN_ERROR";
												break;
					case TOTAL_UNCORRECTABLE_TS_PACKETS 		:	return_str = "TOTAL_UNCORRECTABLE_TS_PACKETS";
												break;
					case PMT_VERSION 				:	return_str = "PMT_VERSION";
												break;
					case PROGRAM_STREAM_TYPE 			:	return_str = "PROGRAM_STREAM_TYPE";
												break;
					case DATA_PID 					:	return_str = "DATA_PID";
												break;
					case VIDEO_PID 					:	return_str = "VIDEO_PID";
												break;
					case PCR_PID 					:	return_str = "PCR_PID";
												break;
					case AUDIO_PID 					:	return_str = "AUDIO_PID";
												break;
					case AUDIO_PID_LANGUAGE 			:	return_str = "AUDIO_PID_LANGUAGE";
												break;
					case AUDIO_TYPE 				:	return_str = "AUDIO_TYPE";
												break;
					case ETHERNET_TRANSMIT 				:	return_str = "ETHERNET_TRANSMIT";
												break;
					case ETHERNET_RECEIVE 				:	return_str = "ETHERNET_RECEIVE";
												break;
					case ETHERNET_TRANSMIT_ERROR 			:	return_str = "ETHERNET_TRANSMIT_ERROR";
												break;
					case ETHERNET_PACKET_DROPPED 			:	return_str = "ETHERNET_PACKET_DROPPED";
												break;
					case ETHERNET_RECEIVE_ERROR 			:	return_str = "ETHERNET_RECEIVE_ERROR";
												break;
					case TOTAL_ETHERNET_PACKETS_OUT			:	return_str = "TOTAL_ETHERNET_PACKETS_OUT";
												break;
					case CAM_STATUS 				:	return_str = "CAM_STATUS";
												break;
					case DVB_SCRAMBLED 				:	return_str = "DVB_SCRAMBLED";
												break;
					case DVB_CLEAR 					:	return_str = "DVB_CLEAR";
												break;
					case CA_PROGRAM_STATUS 				:	return_str = "CA_PROGRAM_STATUS";
												break;
					case CA_SYSTEM_ID 				:	return_str = "CA_SYSTEM_ID";
												break;
					case CA_PROGRAM_SCRAMBLED_FLAG 			:	return_str = "CA_PROGRAM_SCRAMBLED_FLAG";
												break;
					case RECEIVER_IP 				:	return_str = "RECEIVER_IP";
												break;
					case SUBNET_MASK				:	return_str = "SUBNET_MASK";
												break;
					case DEFAULT_GATEWAY_IP 			:	return_str = "DEFAULT_GATEWAY_IP";
												break;
					case RAW_TRAFFIC_DEST_IP 			:	return_str = "RAW_TRAFFIC_DEST_IP";
												break;
					case RAW_TRAFFIC_DEST_PORT			:	return_str = "RAW_TRAFFIC_DEST_PORT";
												break;
					case UNICAST_STATUS_IP				:	return_str = "UNICAST_STATUS_IP";
												break;
					case UNICAST_STATUS_PORT 			:	return_str = "UNICAST_STATUS_PORT";
												break;
					case BROADCAST_STATUS_PORT 			:	return_str = "BROADCAST_STATUS_PORT";
												break;
					case IGMP_ENABLE				:	return_str = "IGMP_ENABLE";
												break;
					case LBAND_FREQUENCY				:	return_str = "LBAND_FREQUENCY";
												break;
					case LO_FREQUENCY				:	return_str = "LO_FREQUENCY";
												break;
					case SYMBOL_RATE				:	return_str = "SYMBOL_RATE";
												break;
					case GOLD_CODE 					:	return_str = "GOLD_CODE";
												break;
					case LNB_POWER			 		:	return_str = "LNB_POWER";
												break;
					case BAND			 		:	return_str = "BAND";
												break;
					case POLARITY 					:	return_str = "POLARITY";
												break;
					case HI_VOLTAGE_SWITCH 				:	return_str = "HI_VOLTAGE_SWITCH";
												break;
					case LONG_LINE_COMPENSATION_SWITCH 		:	return_str = "LONG_LINE_COMPENSATION_SWITCH";
												break;
					case POLARITY_SWITCHING_VOLTAGE 		:	return_str = "POLARITY_SWITCHING_VOLTAGE";
												break;
					case BAND_SWITCHING_TONE_FREQUENCY 		:	return_str = "BAND_SWITCHING_TONE_FREQUENCY";
												break;
					case DVB_SIGNAL_TYPE_CONTROL 			:	return_str = "DVB_SIGNAL_TYPE_CONTROL";
												break;
					case CHANNEL_NUMBER 				:	return_str = "CHANNEL_NUMBER";
												break;
					case DVB_MAC 					:	return_str = "DVB_MAC";
												break;
					case PID 					:	return_str = "PID";
												break;
					case RAW_FORWARD_FLAG 				:	return_str = "RAW_FORWARD_FLAG";
												break;
					case DESTINATION_MASK 				:	return_str = "DESTINATION_MASK";
												break;
					case PID_DESTINATION_IP 			:	return_str = "PID_DESTINATION_IP";
												break;
					case PID_DESTINATION_PORT 			:	return_str = "PID_DESTINATION_PORT";
												break;
					case PROGRAM_NUMBER 				:	return_str = "PROGRAM_NUMBER";
												break;
					case PMT_PID 					:	return_str = "PMT_PID";
												break;
					case CA_PROGRAM_NUMBER 				:	return_str = "CA_PROGRAM_NUMBER";
												break;
					case VPROGRAM_NUMBER				:	return_str = "VPROGRAM_NUMBER";
												break;
					case PROGRAM_NAME 				:	return_str = "PROGRAM_NAME";
												break;
					case VPROGRAM_DESTINATION_IP 			:	return_str = "VPROGRAM_DESTINATION_IP";
												break;
					case VPROGRAM_DESTINATION_PORT 			:	return_str = "VPROGRAM_DESTINATION_PORT";
												break;
					case CAM_WATCHDOG_INTERVAL 			:	return_str = "CAM_WATCHDOG_INTERVAL";
												break;
					case FORWARD_ALL_FLAG 				:	return_str = "FORWARD_ALL_FLAG";
												break;
					case FORWARD_NULLS_FLAG				:	return_str = "FORWARD_NULLS_FLAG";
												break;
					case STREAM_DESTINATION_IP 			:	return_str = "STREAM_DESTINATION_IP";
												break;
					case STREAM_DESTINATION_PORT 			:	return_str = "STREAM_DESTINATION_PORT";
												break;
					case IP_REMAP_ENABLE 				:	return_str = "IP_REMAP_ENABLE";
												break;
					case ORIGINAL_IP 				:	return_str = "ORIGINAL_IP";
												break;
					case NEW_IP 					:	return_str = "NEW_IP";
												break;
					case IP_REMAP_MASK 				:	return_str = "IP_REMAP_MASK";
												break;
					case IP_REMAP_TTL 				:	return_str = "IP_REMAP_TTL";
												break;
					case IP_REMAP_ACTION 				:	return_str = "IP_REMAP_ACTION";
												break;
					case TRAP_MONITORING_INTERVAL 			:	return_str = "TRAP_MONITORING_INTERVAL";
												break;
					case TRAP_NOTIFICATION_IP 			:	return_str = "TRAP_NOTIFICATION_IP";
												break;
					case TRAP_NOTIFICATION_PORT 			:	return_str = "TRAP_NOTIFICATION_PORT";
												break;
					case DATA_LOCK_MONITOR_TRAP_SWITCH 		:	return_str = "DATA_LOCK_MONITOR_TRAP_SWITCH";
												break;
					case ETHERNET_TX_ERROR_TRAP_SWITCH 		:	return_str = "ETHERNET_TX_ERROR_TRAP_SWITCH";
												break;
					case TS_ERROR_TRAP_SWITCH 			:	return_str = "TS_ERROR_TRAP_SWITCH";
												break;
					case VBER_TRAP_SWITCH 				:	return_str = "VBER_TRAP_SWITCH";
												break;
					case VBER_TRAP_THRESHOLD 			:	return_str = "VBER_TRAP_THRESHOLD";
												break;
					case UNCORRECTABLE_TS_ERROR_TRAP_THRESHOLD	:	return_str = "UNCORRECTABLE_TS_ERROR_TRAP_THRESHOLD";
												break;
					case ETHERNET_TX_ERROR_TRAP_THRESHOLD 		:	return_str = "ETHERNET_TX_ERROR_TRAP_THRESHOLD";
												break;
					case FIXED_KEY_PID 				:	return_str = "FIXED_KEY_PID";
												break;
					case PASSWORD 					:	return_str = "PASSWORD";
												break;
					case FIXED_KEY 					:	return_str = "FIXED_KEY";
												break;
					case FIXED_KEY_SIGN 				:	return_str = "FIXED_KEY_SIGN";
												break;
					case MAX_RECEIVER_PARAM 			:	return_str = "MAX_RECEIVER_PARAM";
												break;
					default						:	return_str = string( itoa( enumeratedValue, temp, 10 ) );
												break;
				}
				break;

		case DEVICE_TYPE : switch ( enumeratedValue ) {

								case R_ANY			:	return_str = "Any";
														break;
								case R_UNKNOWN		:	return_str = "Unknown";
														break;
								case R_S75_V1		:	return_str = "S75 V1";
														break;
								case R_S75_V2_1		:	return_str = "S75 V2.1";
														break;
								case R_S75_V3		:	return_str = "S75 V3";
														break;
								case R_A75			:	return_str = "A75";
														break;
								case R_S75V			:	return_str = "S75V";
														break;
								case R_S75CA		:	return_str = "S75CA";
														break;
								case R_A75V			:	return_str = "A75V";
														break;
								case R_S75J			:	return_str = "S75J";
														break;
								case R_S75FK		:	return_str = "S75FK";
														break;
								case R_S75PLUS		:	return_str = "S75+";
														break;
								case R_S75CAPRO		:	return_str = "S75CA Pro";
														break;
								case R_S75FKPRO		:	return_str = "S75FK Pro";
														break;
								case R_S75PLUSPRO	:	return_str = "S75+ Pro";
														break;
								case R_S75IPREMAP	:	return_str = "S75+ IP Remap";
														break;
								case R_S200			:	return_str = "S200";
														break;
								case R_S200PRO		:	return_str = "S200 Pro";
														break;
								case R_S200CA		:	return_str = "S200CA";
														break;
								case R_S200CAPRO	:	return_str = "S200CA Pro";
														break;
								case R_S200FK		:	return_str = "S200FK";
														break;
								case R_S200FKPRO	:	return_str = "S200FK Pro";
														break;
								case R_S200V		:	return_str = "S200V";
														break;
								case R_S200VCA		:	return_str = "S200VCA";
														break;
								case R_S200J		:	return_str = "S200J";
														break;
								case R_SJ300		:	return_str = "S300";
														break;
								default				:	return_str = "Unknown";
														break;

						   }

						   break;


		case LNB_POWER	:		if ( enumeratedValue ) {
							return_str = "On";
						} else {
							return_str = "Off";
						}
						break;


		case BAND 	:		if ( enumeratedValue ) {
							return_str = "High";
						} else {
							return_str = "Low";
						}
						break;


		case POLARITY 	:		if ( enumeratedValue ) {
							return_str = "Horizontal/Left";
						} else {
							return_str = "Vertical/Right";
						}
						break;


		case HI_VOLTAGE_SWITCH 	:	if ( enumeratedValue ) {
							return_str = "On";
						} else {
							return_str = "Off";
						}
						break;


		case LONG_LINE_COMPENSATION_SWITCH 	:	if ( enumeratedValue ) {
									return_str = "On";
								} else {
									return_str = "Off";
								}
								break;


		case BAND_SWITCHING_TONE_FREQUENCY 	:	if ( enumeratedValue ) {
									return_str = "44 kHz";
								} else {
									return_str = "22 kHz";
								}
								break;


		case POLARITY_SWITCHING_VOLTAGE	:	if ( enumeratedValue ) {
								return_str = "11-15V";
							} else {
								if ( hasParameter( HI_VOLTAGE_SWITCH ) && getParameter( HI_VOLTAGE_SWITCH ).asFlag() ) {
									return_str = "12-20V";
								} else {
									return_str = "13-18V";
								}
							}
							break;


		case SIGNAL_LOCK	:	if ( enumeratedValue ) {
							return_str = "Locked";
						} else {
							return_str = "No Lock";
						}
						break;


		case DATA_LOCK		:	if ( enumeratedValue ) {
							return_str = "Locked";
						} else {
							return_str = "No Lock";
						}
						break;


		case LNB_FAULT		:	if ( enumeratedValue ) {
							return_str = "Fault";
						} else {
							return_str = "No Fault";
						}
						break;


		case IGMP_ENABLE	:	if ( enumeratedValue ) {
							return_str = "Enabled";
						} else {
							return_str = "Disabled";
						}
						break;


		case VITERBI_RATE : switch ( enumeratedValue ) {

								case VITERBI_ERROR	:	return_str = "Unknown";
														break;
								case HALF			:	return_str = "1/2 QPSK";
														break;
								case TWO_THIRDS		:	return_str = "2/3 QPSK";
														break;
								case THREE_QUARTERS	:	return_str = "3/4 QPSK";
														break;
								case FOUR_FIFTHS	:	return_str = "4/5 QPSK";
														break;
								case FIVE_SIXTHS	:	return_str = "5/6 QPSK";
														break;
								case SIX_SEVENTHS	:	return_str = "6/7 QPSK";
														break;
								case SEVEN_EIGTHS	:	return_str = "7/8 QPSK";
														break;
								case EIGHT_NINTHS	:	return_str = "8/9 QPSK";
														break;
								default				:	return_str = "Unknown";
														break;
							}

							break;

		case MODCOD		: switch ( enumeratedValue ) {

							case MODCOD_ERROR			:	return_str = "Unknown";
															break;
							case QUARTER_QPSK			:	return_str = "1/4 QPSK";
															break;
							case THIRD_QPSK				:	return_str = "1/3 QPSK";
															break;
							case TWO_FIFTHS_QPSK		:	return_str = "2/5 QPSK";
															break;
							case HALF_QPSK				:	return_str = "1/2 QPSK";
															break;
							case THREE_FIFTHS_QPSK		:	return_str = "3/5 QPSK";
															break;
							case TWO_THIRDS_QPSK		:	return_str = "2/3 QPSK";
															break;
							case THREE_QUARTERS_QPSK	:	return_str = "3/4 QPSK";
															break;
							case FOUR_FIFTHS_QPSK		:	return_str = "4/5 QPSK";
															break;
							case FIVE_SIXTHS_QPSK		:	return_str = "5/6 QPSK";
															break;
							case EIGHT_NINTHS_QPSK		:	return_str = "8/9 QPSK";
															break;
							case NINE_TENTHS_QPSK		:	return_str = "9/10 QPSK";
															break;
							case THREE_FIFTHS_8PSK		:	return_str = "3/5 8PSK";
															break;
							case TWO_THIRDS_8PSK		:	return_str = "2/3 8PSK";
															break;
							case THREE_QUARTERS_8PSK	:	return_str = "3/4 8PSK";
															break;
							case FIVE_SIXTHS_8PSK		:	return_str = "5/6 8PSK";
															break;
							case EIGHT_NINTHS_8PSK		:	return_str = "8/9 8PSK";
															break;
							case NINE_TENTHS_8PSK		:	return_str = "9/10 8PSK";
															break;
							case TWO_THIRDS_16PSK		:	return_str = "2/3 16PSK";
															break;
							case THREE_QUARTERS_16PSK	:	return_str = "3/4 16PSK";
															break;
							case FOUR_FIFTHS_16PSK		:	return_str = "4/5 16PSK";
															break;
							case FIVE_SIXTHS_16PSK		:	return_str = "5/6 16PSK";
															break;
							case EIGHT_NINTHS_16PSK		:	return_str = "8/9 16PSK";
															break;
							case NINE_TENTHS_16PSK		:	return_str = "9/10 16PSK";
															break;
							case THREE_QUARTERS_32PSK	:	return_str = "3/4 32PSK";
															break;
							case FOUR_FIFTHS_32PSK		:	return_str = "4/5 32PSK";
															break;
							case FIVE_SIXTHS_32PSK		:	return_str = "5/6 32PSK";
															break;
							case EIGHT_NINTHS_32PSK		:	return_str = "8/9 32PSK";
															break;
							case NINE_TENTHS_32PSK		:	return_str = "9/10 32PSK";
															break;
							case THIRD_BPSK				:	return_str = "1/3 BPSK";
															break;
							case QUARTER_BPSK			:	return_str = "1/4 BPSK";
															break;
							default						:	return_str = "Unknown";
															break;
						}

						break;

		case DVB_SIGNAL_TYPE	:	if ( enumeratedValue ) {
												return_str = "DVBS2";
											} else {
												return_str = "DVBS";
											}
											break;

		case DVB_SIGNAL_TYPE_CONTROL	:	switch ( enumeratedValue ) {
					
								case AUTO		:	return_str = "Auto";
												break;
								case DVBS		:	return_str = "DVBS";
												break;
								case DVBS2		:	return_str = "DVBS2";
												break;
								default			:	return_str = "None";
												break;

							}
							break;


		case SPECTRAL_INVERSION_FLAG	:	if ( enumeratedValue ) {
												return_str = "Normal";
											} else {
												return_str = "Inverted";
											}
											break;

		case PILOT_SYMBOL_FLAG			:	if ( enumeratedValue ) {
												return_str = "On";
											} else {
												return_str = "Off";
											}
											break;

		case FRAME_LENGTH				:	if ( enumeratedValue ) {
												return_str = "Short";
											} else {
												return_str = "Long";
											}
											break;

		case PROGRAM_STREAM_TYPE		:	switch ( enumeratedValue ) {
					
												case VIDEO_1		:	return_str = "MPEG-1 Video";
																		break;
												case VIDEO_2		:	return_str = "MPEG-2 Video";
																		break;
												case AUDIO_3		:	return_str = "MPEG-1 Audio";
																		break;
												case AUDIO_4		:	return_str = "MPEG-2 Audio";
																		break;
												case AUDIO_DOLBY	:	return_str = "MPEG-2 A/V";
																		break;
												case TYPE_A_MPE		:	return_str = "ISO/IEC 13818-6 type A, MPE";
																		break;
												case TYPE_D_MPE		:	return_str = "ISO/IEC 13818-6 type D, DSM-CC Sections";
																		break;
												case VIDEO_H264		:	return_str = "H.264 Video";
																		break;
												default				:	return_str = "Unknown Stream Type";
																		break;
											}

											break;


		case CAM_STATUS				:	switch ( enumeratedValue ) {
					
									case 0		:	return_str = "Not Inserted";
												break;

									case 1		:	return_str = "Initializing";
												break;

									case 3		:	return_str = "Initialized";
												break;

									case 7		:	return_str = "Ready";
												break;

									default		:	return_str = "Unknown";
												break;
								}

								break;


		case CA_PROGRAM_STATUS			:	switch ( enumeratedValue ) {
					
									case 0		:	
									case 1		:	return_str = "No Program";
												break;

									case 2		:	return_str = "Searching";
												break;

									case 3		:	
									case 4		:	return_str = "Decoding";
												break;

									default		:	return_str = "Unknown";
												break;
								}

								break;


		case CA_PROGRAM_SCRAMBLED_FLAG	:	if ( enumeratedValue ) {
								return_str = "Scrambled";
							} else {
								return_str = "Clear";
							}
							break;


		case CA_SYSTEM_ID	:	masked_id = enumeratedValue & 0xff00;
								if ( enumeratedValue != 0 ) {
									switch ( masked_id ) {

										case 0		:	return_str = "Standardized";
														break;
										case 0x0100	:	return_str = "Canal Plus";
														break;
										case 0x0500	:	return_str = "France Telecom";
														break;
										case 0x0600	:	return_str = "Irdeto";
														break;
										case 0x0b00	:	return_str = "Conax";
														break;
										case 0x0d00	:	return_str = "Philips";
														break;
										case 0x1700	:	return_str = "Beta Technik";
														break;
										default		:	char temp[10];
														sprintf( temp, "0x%04x", enumeratedValue );
														return_str = string( temp );
														break;
													
									}

								} else {

									return_str = "No CA System";

								}

								break;


		case RAW_FORWARD_FLAG	:	if ( enumeratedValue ) {
							return_str = "RAW";
						} else {
							return_str = "MPE";
						}
						break;


		case FORWARD_ALL_FLAG	:	if ( enumeratedValue ) {
							return_str = "Filters Off";
						} else {
							return_str = "Filtering";
						}
						break;


		case FORWARD_NULLS_FLAG	:	if ( enumeratedValue ) {
							return_str = "NULLS On";
						} else {
							return_str = "No NULLS";
						}
						break;


		case FIXED_KEY_SIGN	:	if ( enumeratedValue ) {
							return_str = "Odd";
						} else {
							return_str = "Even";
						}
						break;


		case IP_REMAP_ENABLE	:	if ( enumeratedValue ) {
							return_str = "Enabled";
						} else {
							return_str = "Disabled";
						}
						break;


		case IP_REMAP_ACTION	:	switch ( enumeratedValue ) {
				
							case NO_RULE	:	return_str = "No Rule";
										break;
							case NORMAL	:	return_str = "Normal";
										break;
							case FORWARD	:	return_str = "Forward";
										break;
							case DISCARD	:	return_str = "Discard";
										break;

						}
						break;

		default			:	break;

		}

	}
		
	return return_str;
}


bool Receiver::connect(	const int timeout,
			NOVRA_ERRORS &ErrorCode,
			bool passive )
{

//	if ( passive == true ) m_remote = false;
	return _connect( timeout, ErrorCode, CURRENT, passive );

}


bool Receiver::_connect(	const int timeout,
				NOVRA_ERRORS &ErrorCode,
				ParameterSet parameter_set,
				bool passive )
{

    struct sockaddr_in address;
	int addr_size;

#ifdef WINDOWS
	BOOL use_again=true;
#else
	int use_again;
#endif

	int num_config_ports;
	int i;
	unsigned short port = 0;


	num_config_ports = numConfigPorts();

	if ( parameter_set == PENDING ) {
#ifdef WINDOWS
		closesocket( status_socket );
		status_socket = 0;
#else
		close( status_socket );
		status_socket = 0;
#endif


		for ( i = 0; i < num_config_ports; i++ ) {
#ifdef WINDOWS
			closesocket( config_sockets[i] );
#else
			close( config_sockets[i] );
#endif

		}
	}


	try {

		addr_size = sizeof( struct sockaddr_in );
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;

		address.sin_port = htons( (unsigned short)getParameter( BROADCAST_STATUS_PORT, parameter_set ).asShort() );

#ifdef WINDOWS
		if ( ( status_socket = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) throw 1;
#else
		if ( ( status_socket = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) throw 1;
#endif

		setsockopt( status_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)(&use_again), sizeof( use_again ) );


	    if ( bind ( status_socket, (struct sockaddr *)(&address), sizeof( struct sockaddr ) ) != 0 ) throw 2;

// OCT 2, 2009		
//		if ( !getStatus() ) throw 3;

// OCT 2, 2009		
//		updateStatus(); 

		num_config_ports = numConfigPorts();
		config_sockets = new int[num_config_ports];

		if ( !passive ) {

			for ( i = 0; i < num_config_ports; i++ ) {
				port = configPort(i);
				if ( port != 0 ) {
					memset( (void *)&address, 0, sizeof( address ) );
					address.sin_family = AF_INET;
			    	address.sin_addr.s_addr = inet_addr( getParameter( RECEIVER_IP, parameter_set ).asString().c_str() );
			    	address.sin_port = htons( port );
#ifdef WINDOWS
					if ( ( config_sockets[i] = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) throw 4 + i;
#else
					if ( ( config_sockets[i] = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) throw 4 + i;
#endif

					setsockopt( config_sockets[i], SOL_SOCKET, SO_REUSEADDR, (const char *)(&use_again), sizeof( use_again ) );

					if ( ::connect( config_sockets[i], (const struct sockaddr *)(&address), sizeof( struct sockaddr ) ) != 0) throw 4 + num_config_ports + i;

				}
			}
		}

		if ( getStatus() ) { // OCT 2, 2009

			updateStatus();		// OCT 2, 2009

		} else {
			if ( !passive ) {
				pollStatus( m_status_packet, timeout );	// OCT 2, 2009
				updateStatus();		
			}

		}

	}

	catch ( int e ) {

		switch ( e ) {

			case 1	:	status_socket = 0;
						ErrorCode = N_ERROR_SOCKET_CREATE;
						break;

			case 2	: 
#ifdef WINDOWS
						closesocket( status_socket );
#else
						close( status_socket );
#endif
						status_socket = 0;
						ErrorCode = N_ERROR_SOCKET_BIND;
						break;

			case 3	:	ErrorCode = N_ERROR_NO_STATUS;
						break;

//			case 4	:	ErrorCode = N_ERROR_READ_CURRENT;
//						break;

			default	:	if ( e % 2 ) {
							ErrorCode = N_ERROR_SOCKET_BIND;
						} else {
							ErrorCode = N_ERROR_SOCKET_CREATE;
						}
						for ( i = ( e - 3 ) / 2 - 1; i >= 0; i-- ) {
#ifdef WINDOWS
							closesocket( config_sockets[i] );
#else
							close( config_sockets[i] );
#endif

						}
						for ( i = 0; i < num_config_ports; i++ ) {
							config_sockets[i] = 0;
						}
						break;

			}

		return false;

	}

	ErrorCode = N_ERROR_SUCCESS;

	return true;

}


void Receiver::disconnect()
{

	int num_config_ports;

#ifdef WINDOWS
	closesocket( status_socket );
	status_socket = 0;
#else
	close( status_socket );
	status_socket = 0;
#endif


	num_config_ports = numConfigPorts();
	for ( int i = 0; i < num_config_ports; i++ ) {
#ifdef WINDOWS
		closesocket( config_sockets[i] );
#else
		close( config_sockets[i] );
#endif

	}
	delete [] config_sockets;

}


bool Receiver::getStatus()
{
	bool success = false;
	struct timeval timeout;
	fd_set readfs;
	bool got_packet = true;
	int result;
	int length;
	struct sockaddr_in from_address;
	int addr_size = sizeof( from_address );
	const int max_receive = 1500;
	time_t time_mark;
	time_t now;


	if ( m_remote ) return pollStatus( m_status_packet, 2000 );

	// Make sure buffer is flushed

	timeout.tv_sec = 0;
	timeout.tv_usec = 10;
	FD_ZERO( &readfs );
	FD_SET( status_socket, &readfs );

	while ( got_packet ) {

		got_packet = false;

#ifdef WINDOWS
		if ( ( result = select( 1, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#else
		if ( ( result = select( status_socket+1, &readfs, NULL, NULL, &timeout ) ) > 0 ) {
#endif

			if ( result != 0 ) {
				if ( FD_ISSET( status_socket, &readfs ) ) {
#ifdef WINDOWS
					length = recvfrom( status_socket, (char *)m_status_packet, max_receive, 0, (sockaddr *)(&from_address), &addr_size );
#else
					length = recvfrom( status_socket, (char *)m_status_packet, max_receive, 0, (sockaddr *)(&from_address), (socklen_t *)(&addr_size) );
#endif
					got_packet = true;
				}
			}
		}
	}

	// Buffer is now empty

	timeout.tv_sec = 0;
	timeout.tv_usec = 600;
	got_packet = false;
	now = time_mark = time( NULL );

	while ( !success && ( ( now - time_mark ) < 5 ) ) {

		FD_ZERO( &readfs );
		FD_SET( status_socket, &readfs );

#ifdef WINDOWS
		if ( ( result = select( 1, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#else
		if ( ( result = select( status_socket+1, &readfs, NULL, NULL, &timeout ) ) > 0 ) {
#endif
			if ( result != 0 ) {
				if ( FD_ISSET( status_socket, &readfs ) ) {
#ifdef WINDOWS
					length = recvfrom( status_socket, (char *)m_status_packet, max_receive, 0, (sockaddr *)(&from_address), &addr_size );
#else
					length = recvfrom( status_socket, (char *)m_status_packet, max_receive, 0, (sockaddr *)(&from_address), (socklen_t *)(&addr_size) );
#endif
					if ( checkStatusPacket( string( inet_ntoa( from_address.sin_addr ) ), m_status_packet ) ) {
						success = true;
					}

				}
			}
		}

		now = time( NULL );

	}

	return success;
}



bool Receiver::acceptStatus( string ip, unsigned char *packet )
{
	bool success = false;

	if ( checkStatusPacket( ip, packet ) ) {
		memcpy( m_status_packet, packet, 1500 );
		success = true;
	}

	return success;
}



//  
// Methods
//  


// Accessor methods
//  


// Other methods
//  

bool Receiver::updateStatus()
{
	string param_str;
	int i, num_ca_programs;

	for (	ReceiverParameter p = (ReceiverParameter)0;
			p < MAX_RECEIVER_PARAM;
			p = (ReceiverParameter)((int)p+1) ) {

		if ( hasParameter( p ) ) {
			param_str = dynamicParamFromStatus( p, m_status_packet );
			if ( param_str != "" ) {
				currentParameters[p]->setM_value( param_str );
			}
		}

	}

	if ( hasParameter( CA_PROGRAM_STATUS ) ) {

		num_ca_programs = getParameter( SIZE_CA_PROGRAM_LIST ).asShort();

		for ( i = 0; i < num_ca_programs; i++ ) {

			setIndexedParameter( CA_PROGRAM_STATUS, i, dynamicParamFromStatus( CA_PROGRAM_STATUS, m_status_packet, i ) );

		}

	}

	return true;
}



bool Receiver::sendConfig( int socket_index, unsigned char *packet, int packet_size )
{
//	bool configurationSent;           
//	int LastError;
//    bool configurationApplied;         
//	int status_recieved;
	struct timeval timeout;
	fd_set readfs;
	int result;
	int i; // j;
	bool success = false;
	int length_received;


	LPBYTE Buffer2 = new BYTE[packet_size];


	// Try up to 4 times

	for ( i = 0; !success && ( i < 4 ); i++ ) {

		if ( send( config_sockets[socket_index], (const char *)packet, packet_size, 0 ) != packet_size ) continue;	

#ifdef WINDOWS
		Sleep(30);
#endif
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		FD_ZERO( &readfs );
		FD_SET( config_sockets[socket_index], &readfs );

#ifdef WINDOWS
   		if ( ( result = select( 1, &readfs, NULL, NULL, &timeout ) ) == SOCKET_ERROR ) continue;
#else
   		if ( ( result = select( config_sockets[socket_index]+1, &readfs, NULL, NULL, &timeout ) ) == -1 ) continue;
#endif
		if ( result == 0 ) continue;

#ifdef WINDOWS
		if( ( length_received = recv( config_sockets[socket_index], (char *)Buffer2, packet_size, 0 ) ) == SOCKET_ERROR ) {
			result = GetLastError();
			if ( result != WSAETIMEDOUT ) continue;
		}
#else
		if( ( length_received = recv( config_sockets[socket_index], (char *)Buffer2, packet_size, 0 ) ) == -1 ) continue;
#endif

		if ( length_received != packet_size ) continue;

		for ( int j = 0 ; j < packet_size; j++ ) {

			if ( packet[j] != Buffer2[j] ) continue;

		}

		success = true;
	
	}


	delete [] Buffer2;

	return success;

}


bool Receiver::getConfig(	int socket_index,
							unsigned char *request_packet,
							unsigned char *reply_packet,
							int request_packet_size, 
							int reply_packet_size			)
{
//	bool configurationSent;             // New configuration sent to S75D?
//	int LastError;
//    bool configurationApplied;          // New configuration applied to S75D?
//	int status_recieved;
	struct timeval timeout;
	fd_set readfs;
	int result;
	int i;
	bool success = false;
	int length_received;
	int expected_packet_size = 1500;


	if ( reply_packet_size != 0 ) {
		expected_packet_size = reply_packet_size;
	}

	// Try up to 4 times

	for ( i = 0; !success && ( i < 4 ); i++ ) {

		if ( send( config_sockets[socket_index], (const char *)request_packet, request_packet_size, 0 ) != request_packet_size ) continue;	

#ifdef WINDOWS
		Sleep(30);
#endif
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		FD_ZERO( &readfs );
		FD_SET( config_sockets[socket_index], &readfs );

#ifdef WINDOWS
   		if ( ( result = select( 1, &readfs, NULL, NULL, &timeout ) ) == SOCKET_ERROR ) continue;
#else
   		if ( ( result = select( config_sockets[socket_index]+1, &readfs, NULL, NULL, &timeout ) ) == -1 ) continue;
#endif

		if ( result == 0 ) continue;

#ifdef WINDOWS
		if( ( length_received = recv( config_sockets[socket_index], (char *)reply_packet, expected_packet_size, 0 ) ) == SOCKET_ERROR ) {
			result = GetLastError();
			if ( result != WSAETIMEDOUT ) continue;
		}
#else
		if( ( length_received = recv( config_sockets[socket_index], (char *)reply_packet, expected_packet_size, 0 ) ) == -1 ) continue;
#endif

		if ( ( reply_packet_size != 0 ) && ( length_received != reply_packet_size ) ) continue;

		success = true;
	
	}

	return success;

}


void Receiver::countParameter( ReceiverParameter ParameterName )
{
	int i;
	int count=0;
	int max_count;
	char temp[10];

	switch ( ParameterName ) {
		case PID_COUNT	:	max_count = getParameter( SIZE_PID_LIST ).asShort();
					for ( i = 0; i < max_count; i++ ) {
						if ( getIndexedParameter( PID, i ).asShort() != 8191 ) {
							count++;
						}
					}
					currentParameters[PID_COUNT]->setM_value( itoa( count, temp, 10 ) );
					break;
		default		:	break;
	}
}


int Receiver::loadConfigFromXML( string file_path, bool load_network )
{
	TiXmlDocument *doc=NULL;
	TiXmlNode *xml_config=NULL;
	TiXmlNode *xml_network=NULL;
	TiXmlNode *xml_content=NULL;
	TiXmlNode *xml_cam=NULL;
	TiXmlNode *xml_program=NULL;
	TiXmlNode *xml_pats=NULL;
	TiXmlNode *xml_pat=NULL;
	TiXmlNode *xml_transport=NULL;
	TiXmlNode *xml_pid=NULL;
	TiXmlNode *xml_destination=NULL;
	TiXmlNode *xml_satellite=NULL;
	TiXmlNode *xml_lnb=NULL;
	TiXmlNode *xml_lnb_spec=NULL;
	TiXmlNode *xml_lnb_control=NULL;
	TiXmlNode *xml_signal=NULL;
	TiXmlNode *xml_ip_remap_table;
	TiXmlNode *xml_ip_remap_rule;

	string new_receiver_ip;
	string subnet_mask;
	string default_gateway;
	string status_destination_ip;
	string status_destination_port;
	string IGMP_filter;
	string watchdog_timeout_str;;
	string program_number;
	string pmt_pid;
	string pid_number;
	string processing;
	string ip_address;
	string port_str;
	string rf_str;
	string symbol_rate_str;
	string gold_code_str;
	string lo_frequency_str;
	string switching_voltage_str;
	string hi_voltage_mode_str;
	string dataStr;

	string bcast_status_destination_port = "6516";
	char temp[20];
	int i, j;
	bool cam_read = false;
	bool pat_read = false;
	short port;
	bool found;
	int numDestinations;
	unsigned short mask;
	bool ip_remap_table_read = false;


	try {

		if ( !( doc = new TiXmlDocument( file_path.c_str() ) ) ) throw 1;

		if ( !doc->LoadFile() ) throw 2;

		bcast_status_destination_port = getParameter( BROADCAST_STATUS_PORT ).asString();

		clear();

		if ( !( xml_config = doc->FirstChild( "S75_CONFIG" ) ) ) throw 3;

		if ( !( xml_content	= xml_config->FirstChild( "CONTENT" ) ) ) throw 4;

		if ( !( xml_satellite = xml_config->FirstChild( "SATELLITE" ) ) ) throw 5;

		if ( !( xml_lnb	= xml_satellite->FirstChild( "LNB" ) ) ) throw 6;

		if ( !( xml_lnb_spec = xml_lnb->FirstChild( "LNBSpec" ) ) ) throw 7;

		if ( !( xml_lnb_control	= xml_lnb->FirstChild( "LNBControl" ) ) ) throw 8;

		if ( !( xml_signal = xml_satellite->FirstChild( "SIGNAL" ) ) ) throw 9;

		if ( load_network ) {

			if ( !( xml_network = xml_config->FirstChild( "NETWORK" ) ) ) throw 13;

			new_receiver_ip = xml_network->ToElement()->Attribute( "ReceiverIP" );
			subnet_mask = xml_network->ToElement()->Attribute( "SubnetMask" );
			default_gateway = xml_network->ToElement()->Attribute( "DefaultGateway" );
			status_destination_ip = xml_network->ToElement()->Attribute( "StatusDestinationIP" );
			status_destination_port = xml_network->ToElement()->Attribute( "StatusDestinationPort" );
			IGMP_filter = xml_network->ToElement()->Attribute( "IGMPFilter" );

			setParameter( RECEIVER_IP, string( new_receiver_ip ) );
			setParameter( SUBNET_MASK, string( subnet_mask ) );
			setParameter( DEFAULT_GATEWAY_IP, string( default_gateway ) );
			setParameter( UNICAST_STATUS_IP, string( status_destination_ip ) );
			setParameter( UNICAST_STATUS_PORT, string( status_destination_port ) );


			if ( strcmp( IGMP_filter.c_str(), "ON" ) == 0 ) {
				setParameter( IGMP_ENABLE, "1" );
			} else {
				setParameter( IGMP_ENABLE, "0" );
			}

//@@@JUNE 8, 2009			ReceiverSearch::GetAvailablePort( (unsigned short &)port );
//@@@JUNE 8, 2009			m_receiver->setParameter( BROADCAST_STATUS_PORT, string( itoa( port, temp, 10 ) ) );

			setParameter( BROADCAST_STATUS_PORT, bcast_status_destination_port );

		}

		rf_str = xml_signal->ToElement()->Attribute( "RF" );
		if ( hasParameter( GOLD_CODE ) ) {
			gold_code_str = xml_signal->ToElement()->Attribute( "GoldCode" );
		}
		symbol_rate_str	= xml_signal->ToElement()->Attribute( "SymbolRate" );

		setParameter( LBAND_FREQUENCY, string( rf_str ) );
		if ( parameterWriteable( SYMBOL_RATE ) ) {
			setParameter( SYMBOL_RATE, string( symbol_rate_str ) );
		}
		if ( hasParameter( GOLD_CODE ) ) {
			setParameter( GOLD_CODE, string( gold_code_str ) );
		}

		if ( hasParameter( DVB_SIGNAL_TYPE_CONTROL ) ) {
			if ( xml_signal->ToElement()->Attribute( "SearchType" ) ) {
				if ( strcmp( xml_signal->ToElement()->Attribute( "SearchType" ), "DVBS2" ) == 0 ) {
					setParameter( DVB_SIGNAL_TYPE_CONTROL, string( itoa( DVBS2, temp, 10 ) ) );
				}
				if ( strcmp( xml_signal->ToElement()->Attribute( "SearchType" ), "DVBS" ) == 0 ) {
					setParameter( DVB_SIGNAL_TYPE_CONTROL, string( itoa( DVBS, temp, 10 ) ) );
				}
				if ( strcmp( xml_signal->ToElement()->Attribute( "SearchType" ), "AUTO" ) == 0 ) {
					setParameter( DVB_SIGNAL_TYPE_CONTROL, string( itoa( AUTO, temp, 10 ) ) );
				}
			} else {
				setParameter( DVB_SIGNAL_TYPE_CONTROL, string( itoa( AUTO, temp, 10 ) ) );
			}
		}

		if ( hasParameter( LO_FREQUENCY ) ) {

			lo_frequency_str = xml_lnb_spec->ToElement()->Attribute( "LOFrequency" );
			setParameter( LO_FREQUENCY, string( lo_frequency_str ) );

		}
	
		if ( strcmp( xml_lnb_control->ToElement()->Attribute( "Power" ), "ON" ) == 0 ) {
			setParameter( LNB_POWER, "1" );
		} else {
			setParameter( LNB_POWER, "0" );
		}
		if ( strcmp( xml_lnb_control->ToElement()->Attribute( "Band" ), "High" ) == 0 ) {
			setParameter( BAND, "1" );
		} else {
			setParameter( BAND, "0" );
		}
		if ( strcmp( xml_lnb_control->ToElement()->Attribute( "Polarization" ), "Horizontal/Left" ) == 0 ) {
			setParameter( POLARITY, "1" );
		} else {
			setParameter( POLARITY, "0" );
		}
		if ( hasParameter( LONG_LINE_COMPENSATION_SWITCH ) ) {
			if ( strcmp( xml_lnb_control->ToElement()->Attribute( "LongLineCompensation" ), "ON" ) == 0 ) {
				setParameter( LONG_LINE_COMPENSATION_SWITCH, "1" );
			} else {
				setParameter( LONG_LINE_COMPENSATION_SWITCH, "0" );
			}
		}
		if ( hasParameter( BAND_SWITCHING_TONE_FREQUENCY ) ) {
			if ( strcmp( xml_lnb_spec->ToElement()->Attribute( "HiLowBandTone" ), "22" ) == 0 ) {
				setParameter( BAND_SWITCHING_TONE_FREQUENCY, "0" );
			} else {
				setParameter( BAND_SWITCHING_TONE_FREQUENCY, "1" );
			}
		}

//		hi_voltage_mode_str = xml_lnb_control->ToElement()->Attribute( "HiVoltageMode" );
//		if ( ( hi_voltage_mode_str = xml_lnb_control->ToElement()->Attribute( "HiVoltageMode" ) ) != "" ) {
		if ( xml_lnb_control->ToElement()->Attribute( "HiVoltageMode" ) &&
			 ( ( hi_voltage_mode_str = xml_lnb_control->ToElement()->Attribute( "HiVoltageMode" ) ) != "" )) {

			if ( strcmp( hi_voltage_mode_str.c_str(), "ON" ) == 0 ) {
				if ( hasParameter( HI_VOLTAGE_SWITCH ) ) {
					setParameter( HI_VOLTAGE_SWITCH, "1" );
				}
			} else {
				if ( hasParameter( HI_VOLTAGE_SWITCH ) ) {
					setParameter( HI_VOLTAGE_SWITCH, "0" );
				}
			}

		} else {

			switching_voltage_str = xml_lnb_spec->ToElement()->Attribute( "PolaritySwitchingVoltages" );

			if ( strcmp( switching_voltage_str.c_str(), "11 & 15" ) == 0 ) {
				if ( hasParameter( HI_VOLTAGE_SWITCH ) ) {
					setParameter( HI_VOLTAGE_SWITCH, "0" );
				}
				if ( hasParameter( POLARITY_SWITCHING_VOLTAGE ) ) {
					setParameter( POLARITY_SWITCHING_VOLTAGE, "1" );
				}
			} else
			if ( strcmp( switching_voltage_str.c_str(), "13 & 18" ) == 0 ) {
				if ( hasParameter( HI_VOLTAGE_SWITCH ) ) {
					setParameter( HI_VOLTAGE_SWITCH, "0" );
				}
				if ( hasParameter( POLARITY_SWITCHING_VOLTAGE ) ) {
					setParameter( POLARITY_SWITCHING_VOLTAGE, "0" );
				}
			} else
			if ( strcmp( switching_voltage_str.c_str(), "12 & 20" ) == 0 ) {
				if ( hasParameter( HI_VOLTAGE_SWITCH ) ) {
					setParameter( HI_VOLTAGE_SWITCH, "1" );
				}
				if ( hasParameter( POLARITY_SWITCHING_VOLTAGE ) ) {
					setParameter( POLARITY_SWITCHING_VOLTAGE, "0" );
				}
			}

		}

		if ( hasParameter( SIZE_CA_PROGRAM_LIST ) ) {
			if ( ( xml_cam = xml_content->FirstChild( "CAM" ) ) ) {
				cam_read = true;
				watchdog_timeout_str = xml_cam->ToElement()->Attribute( "WatchdogTimeout" );
				setParameter( CAM_WATCHDOG_INTERVAL, string( watchdog_timeout_str ) );
				xml_program = xml_cam->FirstChild( "Program" );
				i = 0;
				while ( xml_program ) {
					program_number = xml_program->ToElement()->Attribute( "Number" );
					setIndexedParameter( CA_PROGRAM_NUMBER, i++, string( program_number ) );
					xml_program = xml_program->NextSibling( "Program" );
				}
			}
		}	


		if ( hasParameter( SIZE_MINI_PAT_LIST ) ) {
			if ( ( xml_pats = xml_content->FirstChild( "PATS" ) ) ) {
				pat_read = true;
				xml_pat = xml_pats->FirstChild( "PAT" );
				i = 0;
				while ( xml_pat ) {
					program_number = xml_pat->ToElement()->Attribute( "ProgramNumber" );
					pmt_pid = xml_pat->ToElement()->Attribute( "PMTPID" );
					setIndexedParameter( PROGRAM_NUMBER, i, string( program_number ) );
					setIndexedParameter( PMT_PID, i++, string( pmt_pid ) );
					xml_pat = xml_pat->NextSibling( "PAT" );
				}
			}
		}


		if ( !( xml_transport = xml_content->FirstChild( "TRANSPORT_STREAM" ) ) ) throw 12;

		if ( ( xml_transport->ToElement()->Attribute( "PIDS" ) ) &&
			 ( strncmp( xml_transport->ToElement()->Attribute( "PIDS" ), "ALL", 3 ) == 0 ) ) {
			if ( hasParameter( FORWARD_ALL_FLAG ) ) {
				setParameter( FORWARD_ALL_FLAG, "1" );
			}
			if ( strcmp( xml_transport->ToElement()->Attribute( "PIDS" ), "ALL Minus NULL" ) == 0 ) {
				setParameter( FORWARD_NULLS_FLAG, "0" );
			} else {
				setParameter( FORWARD_NULLS_FLAG, "1" );
			}
			xml_destination = xml_transport->FirstChild( "DESTINATION" );
			if ( xml_destination ) {
				ip_address = xml_destination->ToElement()->Attribute( "IPAddress" );
				port_str = xml_destination->ToElement()->Attribute( "Port" );
				setParameter( STREAM_DESTINATION_IP, string( ip_address ) );
				setParameter( STREAM_DESTINATION_PORT, string( port_str ) );
			}

		} else {

			if ( hasParameter( FORWARD_ALL_FLAG ) ) {
				setParameter( FORWARD_ALL_FLAG, "0" );
			}
			if ( hasParameter( FORWARD_NULLS_FLAG ) ) {
				setParameter( FORWARD_NULLS_FLAG, "0" );
			}

		} //	if ( strcmp( xml_transport->ToElement()->Attribute( "PIDS" ), "Selected" ) == 0 ) {

		if ( hasParameter( SIZE_PID_LIST ) ) {

			xml_pid = xml_transport->FirstChild( "PID" );
			i = 0;
			while ( xml_pid ) {

				pid_number = xml_pid->ToElement()->Attribute( "Number" );
				processing = xml_pid->ToElement()->Attribute( "Processing" );

				if ( strcmp( processing.c_str(), "RAW" ) == 0 ) {
					if ( hasParameter( RAW_FORWARD_FLAG ) ) {
						if ( pid_number != "NULL" ) {
							setIndexedParameter( PID, i, string( pid_number ) );
							setIndexedParameter( RAW_FORWARD_FLAG, i, "1" );
						}
						xml_destination = xml_pid->FirstChild( "DESTINATION" );
						if ( hasParameter( SIZE_MINI_PAT_LIST ) ) {
							while ( xml_destination ) {
								ip_address = xml_destination->ToElement()->Attribute( "IPAddress" );
								port_str = xml_destination->ToElement()->Attribute( "Port" );
								sscanf( port_str.c_str(), "%hd", &port );
								j = 0;
								found = false;
								numDestinations = getParameter( SIZE_DESTINATION_LIST ).asShort();
								while ( ( j < numDestinations ) && (!found) ) {
									if ( ( getIndexedParameter( PID_DESTINATION_IP, j, PENDING ).asString() == string( ip_address ) ) &&
										 ( getIndexedParameter( PID_DESTINATION_PORT, j, PENDING ).asShort() == port ) ) {
										found = true;
									} else {
										j++;
									}
								}
								if ( !found ) {
									j = 0;
									while ( !found ) {
										if ( ( getIndexedParameter( PID_DESTINATION_IP, j, PENDING ).asString() == string( "0.0.0.0" ) ) &&
											 ( getIndexedParameter( PID_DESTINATION_PORT, j, PENDING ).asShort() == 0 ) ) {
											found = true;
										} else {
											j++;
										}
									}
									if ( found ) {
										setIndexedParameter( PID_DESTINATION_IP, j, string( ip_address ) );
										setIndexedParameter( PID_DESTINATION_PORT, j, string( port_str ) );
									}
								}
								if ( ( pid_number != "NULL" ) && ( found ) ) { 
									mask = getIndexedParameter( DESTINATION_MASK, i, PENDING ).asShort();
									mask = mask | ( 0x8000 >> j );
									setIndexedParameter( DESTINATION_MASK, i, string( itoa( mask, temp, 10 ) ) );
								}
								xml_destination = xml_destination->NextSibling( "DESTINATION" );
							}
						} else {
							if ( xml_destination ) {
								ip_address = xml_destination->ToElement()->Attribute( "IPAddress" );
								port_str = xml_destination->ToElement()->Attribute( "Port" );
								sscanf( port_str.c_str(), "%hd", &port );
								setIndexedParameter( PID_DESTINATION_IP, i, string( ip_address ) );
								setIndexedParameter( PID_DESTINATION_PORT, i, string( port_str ) );
							} else {
								setIndexedParameter( PID_DESTINATION_IP, i, string( "0.0.0.0" ) );
								setIndexedParameter( PID_DESTINATION_PORT, i, "0" );
							}
						}

					}

				} else {
					setIndexedParameter( PID, i, string( pid_number ) );
					setIndexedParameter( RAW_FORWARD_FLAG, i, "0" );
				}
				i++;
				xml_pid = xml_pid->NextSibling( "PID" );
			}
		}


		if ( hasParameter( SIZE_VPROGRAM_LIST ) ) {

			xml_program = xml_transport->FirstChild( "PROGRAM" );
			i = 0;
			while ( xml_program && ( i < getParameter( SIZE_VPROGRAM_LIST ).asShort() ) ) {

				program_number = xml_program->ToElement()->Attribute( "Number" );
				setIndexedParameter( VPROGRAM_NUMBER, i, string( program_number ) );
				ip_address = xml_program->ToElement()->Attribute( "IPAddress" );
				port_str = xml_program->ToElement()->Attribute( "Port" );					

				if ( ip_address == "" ) ip_address = "0.0.0.0";
				if ( port_str == "" ) port_str = "0";

				setIndexedParameter( VPROGRAM_DESTINATION_IP, i, string( ip_address ) );
				setIndexedParameter( VPROGRAM_DESTINATION_PORT, i, string( port_str ) );

				i++;
				xml_program = xml_program->NextSibling( "PROGRAM" );
			}

		}


		if ( hasParameter( SIZE_IP_REMAP_LIST ) ) {
			if ( ( xml_ip_remap_table = xml_content->FirstChild( "IP_REMAP_TABLE" ) ) ) {
				ip_remap_table_read = true;
				dataStr = xml_ip_remap_table->ToElement()->Attribute( "Enabled" );
				if ( dataStr == "true" ) {
					setParameter( IP_REMAP_ENABLE, "1" );
				} else {
					setParameter( IP_REMAP_ENABLE, "0" );
				}
				xml_ip_remap_rule = xml_ip_remap_table->FirstChild( "IP_Remap_Rule" );
				i = 0;
				while ( xml_ip_remap_rule ) {

					dataStr = xml_ip_remap_rule->ToElement()->Attribute( "Original_IP" );
					setIndexedParameter( ORIGINAL_IP, i, string( dataStr ) );

					dataStr = xml_ip_remap_rule->ToElement()->Attribute( "New_IP" );
					setIndexedParameter( NEW_IP, i, string( dataStr ) );

					dataStr = xml_ip_remap_rule->ToElement()->Attribute( "Mask" );
					setIndexedParameter( IP_REMAP_MASK, i, string( dataStr ) );

					dataStr = xml_ip_remap_rule->ToElement()->Attribute( "TTL" );
					setIndexedParameter( IP_REMAP_TTL, i, string( dataStr ) );

					dataStr = xml_ip_remap_rule->ToElement()->Attribute( "Action" );
					setIndexedParameter( IP_REMAP_ACTION, i, string( dataStr ) );
		
					xml_ip_remap_rule = xml_ip_remap_rule->NextSibling( "IP_Remap_Rule" );
					i++;

				}
			}
		}

	}

	catch ( int e ) {

		return e;

	}

	return 0;

}



int Receiver::saveConfigToXML( string file_path ) 
{
	
/*
	CString fileFilter;
    CFile outFile;                          // CFile object to save current settings
//    CString temp;                           // Temporary storage variables
	WORD watchdog_timeout=0;
*/
	TiXmlDocument *doc=NULL;
	TiXmlElement root_node( "S75_CONFIG" );
	TiXmlElement network_node( "NETWORK" );
	TiXmlElement satellite_node( "SATELLITE" );
	TiXmlElement signal_node( "SIGNAL" );
	TiXmlElement lnb_node( "LNB" );
	TiXmlElement lnb_spec_node( "LNBSpec" );
	TiXmlElement lnb_control_node( "LNBControl" );
	TiXmlElement content_node( "CONTENT" );
	TiXmlElement ts_node( "TRANSPORT_STREAM" );
	TiXmlElement pats_node( "PATS" );
	TiXmlElement cam_node( "CAM" );
//	TiXmlElement trap_node( "TRAPS" );
	TiXmlElement ip_remap_table_node( "IP_REMAP_TABLE" );
	TiXmlElement *pid_node=NULL;
	TiXmlElement *destination_node=NULL;
	TiXmlElement *pat_node=NULL;
	TiXmlElement *program_node=NULL;
	TiXmlElement *vprogram_node=NULL;
	TiXmlElement *ip_remap_node=NULL;

	string attribute_value;
	int i, j;
	int numPids=0;
	int numPrograms=0;
	int numDestinations=0;
	unsigned short mask=0;
	unsigned short used_destinations_mask = 0;
	int numPATs=0;
	int numCAPrograms=0;
	int numRemapRules=0;
//	bool pids_are_selected = false;


//    fileFilter.Format("S75 Configuration Files (*.xml)|*.xml|All Files (*.*)|*.*||");

	// Construct a CFileDialog object to get the name of the file to save
//    CFileDialog fileWindow(false, "*.xml", "*.xml", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, (LPCTSTR)fileFilter, this);

    // Open the dialog box to get the name of the file to save.
//    if (fileWindow.DoModal() == IDCANCEL) return;
	
    // Get the IP address from the control on the main dialog box
//    UpdateData(true);

	doc = new TiXmlDocument( file_path.c_str() );


	attribute_value = getParameter( RECEIVER_IP ).asString();
	network_node.SetAttribute( "ReceiverIP", attribute_value.c_str() );

	attribute_value = getParameter( SUBNET_MASK ).asString();
	network_node.SetAttribute( "SubnetMask", attribute_value.c_str() );

	attribute_value = getParameter( DEFAULT_GATEWAY_IP ).asString();
	network_node.SetAttribute( "DefaultGateway", attribute_value.c_str() );

	attribute_value = getParameter( UNICAST_STATUS_IP ).asString();
	network_node.SetAttribute( "StatusDestinationIP", attribute_value.c_str() );

	attribute_value = getParameter( UNICAST_STATUS_PORT ).asString();
	network_node.SetAttribute( "StatusDestinationPort", attribute_value.c_str() );

	if ( getParameter( IGMP_ENABLE ).asFlag() ) {
		network_node.SetAttribute( "IGMPFilter", "ON" );
	} else {
		network_node.SetAttribute( "IGMPFilter", "OFF" );
	}

	if ( hasParameter( LO_FREQUENCY ) ) {
		attribute_value = getParameter( LO_FREQUENCY ).asString();
		lnb_spec_node.SetAttribute( "LOFrequency", attribute_value.c_str() );
	}

	if ( hasParameter( HI_VOLTAGE_SWITCH ) ) {
		if ( hasParameter( POLARITY_SWITCHING_VOLTAGE ) ) {
			if ( getParameter( HI_VOLTAGE_SWITCH ).asFlag() ) {
				if ( !getParameter( POLARITY_SWITCHING_VOLTAGE ).asFlag() ) {
					lnb_spec_node.SetAttribute( "PolaritySwitchingVoltages", "12 & 20" );
				}
			} else {
				if ( getParameter( POLARITY_SWITCHING_VOLTAGE ).asFlag() ) {
					lnb_spec_node.SetAttribute( "PolaritySwitchingVoltages", "11 & 15" );
				} else {
					lnb_spec_node.SetAttribute( "PolaritySwitchingVoltages", "13 & 18" );
				}
			}
		} else {
			if ( getParameter( HI_VOLTAGE_SWITCH ).asFlag() ) {
				lnb_control_node.SetAttribute( "HiVoltageMode", "ON" );
			} else {
				lnb_control_node.SetAttribute( "HiVoltageMode", "OFF" );
			}
		}
	}

	if ( hasParameter( BAND_SWITCHING_TONE_FREQUENCY ) ) {
		if ( getParameter( BAND_SWITCHING_TONE_FREQUENCY ).asFlag() ) {
			lnb_spec_node.SetAttribute( "HiLowBandTone", "44" );
		} else {
			lnb_spec_node.SetAttribute( "HiLowBandTone", "22" );
		}
	}

	if ( getParameter( LNB_POWER ).asFlag() ) {
		lnb_control_node.SetAttribute( "Power", "ON" );
	} else {
		lnb_control_node.SetAttribute( "Power", "OFF" );
	}

	if ( getParameter( POLARITY ).asFlag() ) {
		lnb_control_node.SetAttribute( "Polarization", "Horizontal/Left" );
	} else {
		lnb_control_node.SetAttribute( "Polarization", "Vertical/Right" );
	}

	if ( getParameter( BAND ).asFlag() ) {
		lnb_control_node.SetAttribute( "Band", "High" );
	} else {
		lnb_control_node.SetAttribute( "Band", "Low" );
	}

	if ( getParameter( LONG_LINE_COMPENSATION_SWITCH ).asFlag() ) {
		lnb_control_node.SetAttribute( "LongLineCompensation", "ON" );
	} else {
		lnb_control_node.SetAttribute( "LongLineCompensation", "OFF" );
	}

	attribute_value = getParameter( LBAND_FREQUENCY ).asString();
	signal_node.SetAttribute( "RF", attribute_value.c_str() );

	attribute_value = getParameter( SYMBOL_RATE ).asString();
	signal_node.SetAttribute( "SymbolRate", attribute_value.c_str() );

	if ( hasParameter( GOLD_CODE ) ) {
		attribute_value = getParameter( GOLD_CODE ).asString();
		signal_node.SetAttribute( "GoldCode", attribute_value.c_str() );
	}

	switch ( getParameter( DVB_SIGNAL_TYPE_CONTROL ).asShort() ) {

		case DVBS	:	signal_node.SetAttribute( "SearchType", "DVBS" );
						break;
		case DVBS2	:	signal_node.SetAttribute( "SearchType", "DVBS2" );
						break;
		default		:	signal_node.SetAttribute( "SearchType", "AUTO" );
						break;

	}

	satellite_node.InsertEndChild( signal_node );
	lnb_node.InsertEndChild( lnb_spec_node );
	lnb_node.InsertEndChild( lnb_control_node );
	satellite_node.InsertEndChild( lnb_node );

	if ( hasParameter( FORWARD_ALL_FLAG ) ) {
		if ( getParameter( FORWARD_ALL_FLAG ).asFlag() ) {
			if ( hasParameter( FORWARD_NULLS_FLAG ) ) {
				if ( getParameter( FORWARD_NULLS_FLAG ).asFlag() ) {
					ts_node.SetAttribute( "PIDS", "ALL" );
				} else {
					ts_node.SetAttribute( "PIDS", "ALL Minus NULL" );
				}
			} else {
				ts_node.SetAttribute( "PIDS", "ALL Minus NULL" );
			}
			destination_node = new TiXmlElement( "DESTINATION" );
			attribute_value = getParameter( STREAM_DESTINATION_IP ).asString();
			destination_node->SetAttribute( "IPAddress", attribute_value.c_str() );
			attribute_value = getParameter( STREAM_DESTINATION_PORT ).asString();
			destination_node->SetAttribute( "Port", attribute_value.c_str() );
			ts_node.InsertEndChild( *destination_node );
		} else {
			if ( hasParameter( SIZE_PID_LIST ) ) {
				ts_node.SetAttribute( "PIDS", "Selected" );
			} else {
				ts_node.SetAttribute( "PIDS", "By Program" );
			}
//			pids_are_selected = true;
		}
	} else {
		if ( hasParameter( SIZE_PID_LIST ) ) {
			ts_node.SetAttribute( "PIDS", "Selected" );
		} else {
			ts_node.SetAttribute( "PIDS", "By Program" );
		}

//		pids_are_selected = true;
	}

//	if ( pids_are_selected ) {
		numPids = getParameter( SIZE_PID_LIST ).asShort();
		if ( hasParameter( SIZE_DESTINATION_LIST ) ) {
			numDestinations = getParameter( SIZE_DESTINATION_LIST ).asShort();
		}
		for ( i = 0; i < numPids; i++ ) {
			if ( getIndexedParameter( PID, i ).asShort() != 0x1fff ) {
				attribute_value = getIndexedParameter( PID, i ).asString();
				pid_node = new TiXmlElement( "PID" );
				pid_node->SetAttribute( "Number", attribute_value.c_str() );
				if ( hasParameter( RAW_FORWARD_FLAG ) ) {
					if ( getIndexedParameter( RAW_FORWARD_FLAG, i ).asFlag() ) {
						pid_node->SetAttribute( "Processing", "RAW" );
						if ( hasParameter( DESTINATION_MASK ) ) {
							mask = getIndexedParameter( DESTINATION_MASK, i ).asShort();
							if ( mask != 0 ) {
								for ( j = 0; j < numDestinations; j++ ) {
									if ( mask & ( 0x8000 >> j ) ) {
										used_destinations_mask = used_destinations_mask | ( 0x8000 >> j );
										destination_node = new TiXmlElement( "DESTINATION" );
										attribute_value = getIndexedParameter( PID_DESTINATION_IP, j ).asString();
										destination_node->SetAttribute( "IPAddress", attribute_value.c_str() );
										attribute_value = getIndexedParameter( PID_DESTINATION_PORT, j ).asString();
										destination_node->SetAttribute( "Port", attribute_value.c_str() );
										pid_node->InsertEndChild( *destination_node );
									}
								}
							}
						} else {
							destination_node = new TiXmlElement( "DESTINATION" );
							attribute_value = getIndexedParameter( PID_DESTINATION_IP, 0 ).asString();
							destination_node->SetAttribute( "IPAddress", attribute_value.c_str() );
							attribute_value = getIndexedParameter( PID_DESTINATION_PORT, 0 ).asString();
							destination_node->SetAttribute( "Port", attribute_value.c_str() );
							pid_node->InsertEndChild( *destination_node );
						}
					} else {
						pid_node->SetAttribute( "Processing", "MPE" );
					}
				} else {
					pid_node->SetAttribute( "Processing", "MPE" );
				}
				ts_node.InsertEndChild( *pid_node );
			}
		}
		if ( used_destinations_mask != 0xffff ) {
			pid_node = new TiXmlElement( "PID" );
			pid_node->SetAttribute( "Number", "NULL" );
			pid_node->SetAttribute( "Processing", "RAW" );
			for ( j = 0; j < numDestinations; j++ ) {
				if ( !( used_destinations_mask & ( 0x8000 >> j ) )  ) {
					if ( getIndexedParameter( PID_DESTINATION_PORT, j ).asShort() != 0 ) {
						destination_node = new TiXmlElement( "DESTINATION" );
						attribute_value = getIndexedParameter( PID_DESTINATION_IP, j ).asString();
						destination_node->SetAttribute( "IPAddress", attribute_value.c_str() );
						attribute_value = getIndexedParameter( PID_DESTINATION_PORT, j ).asString();
						destination_node->SetAttribute( "Port", attribute_value.c_str() );
						pid_node->InsertEndChild( *destination_node );
					}
				}
			}
			ts_node.InsertEndChild( *pid_node );
		}
//	}

	numPrograms = getParameter( SIZE_VPROGRAM_LIST ).asShort();
	for ( i = 0; i < numPrograms; i++ ) {
		if ( getIndexedParameter( VPROGRAM_NUMBER, i ).asShort() ) {
			attribute_value = getIndexedParameter( VPROGRAM_NUMBER, i ).asString();
			vprogram_node = new TiXmlElement( "PROGRAM" );
			vprogram_node->SetAttribute( "Number", attribute_value.c_str() );
			if ( getIndexedParameter( VPROGRAM_DESTINATION_PORT, i ).asShort() ) {
				attribute_value = getIndexedParameter( VPROGRAM_DESTINATION_IP, j ).asString();
				vprogram_node->SetAttribute( "IPAddress", attribute_value.c_str() );
				attribute_value = getIndexedParameter( VPROGRAM_DESTINATION_PORT, j ).asString();
				vprogram_node->SetAttribute( "Port", attribute_value.c_str() );
			}
			ts_node.InsertEndChild( *vprogram_node );
		}
	}


	if ( hasParameter( SIZE_MINI_PAT_LIST ) ) {
		numPATs = getParameter( SIZE_MINI_PAT_LIST ).asShort();
		for ( i = 0; i < numPATs; i++ ) {
			if ( getIndexedParameter( PROGRAM_NUMBER, i ).asShort() ) {
				pat_node = new TiXmlElement( "PAT" );
				attribute_value = getIndexedParameter( PROGRAM_NUMBER, i ).asString();
				pat_node->SetAttribute( "ProgramNumber", attribute_value.c_str() );
				attribute_value = getIndexedParameter( PMT_PID, i ).asString();
				pat_node->SetAttribute( "PMTPID", attribute_value.c_str() );
				pats_node.InsertEndChild( *pat_node );
			}
		}
	}

	if ( hasParameter( CAM_WATCHDOG_INTERVAL ) ) {
		attribute_value = getParameter( CAM_WATCHDOG_INTERVAL ).asString();
		cam_node.SetAttribute( "WatchdogTimeout", attribute_value.c_str() );
	}

	if ( hasParameter( SIZE_CA_PROGRAM_LIST ) ) {
		numCAPrograms = getParameter( SIZE_CA_PROGRAM_LIST ).asShort();
		for ( i = 0; i < numCAPrograms; i++ ) {
			if ( getIndexedParameter( CA_PROGRAM_NUMBER, i ).asShort() ) {
				program_node = new TiXmlElement( "Program" );
				attribute_value = getIndexedParameter( CA_PROGRAM_NUMBER, i ).asString();
				program_node->SetAttribute( "Number", attribute_value.c_str() );
				cam_node.InsertEndChild( *program_node );
			}
		}
	}

	if ( hasParameter( SIZE_IP_REMAP_LIST ) ) {
		if ( getParameter( IP_REMAP_ENABLE ).asFlag() ) {
			attribute_value = "true";
		} else {
			attribute_value = "false";
		}
		ip_remap_table_node.SetAttribute( "Enabled", attribute_value.c_str() );
		numRemapRules = getParameter( SIZE_IP_REMAP_LIST ).asShort();
		for ( i = 0; i < numRemapRules; i++ ) {
			if ( getIndexedParameter( ORIGINAL_IP, i ).asString() != "0.0.0.0" ) {
				ip_remap_node = new TiXmlElement( "IP_Remap_Rule" );
				attribute_value = getIndexedParameter( ORIGINAL_IP, i ).asString();
				ip_remap_node->SetAttribute( "Original_IP", attribute_value.c_str() );
				attribute_value = getIndexedParameter( NEW_IP, i ).asString();
				ip_remap_node->SetAttribute( "New_IP", attribute_value.c_str() );
				attribute_value = getIndexedParameter( IP_REMAP_MASK, i ).asString();
				ip_remap_node->SetAttribute( "Mask", attribute_value.c_str() );
				attribute_value = getIndexedParameter( IP_REMAP_TTL, i ).asString();
				ip_remap_node->SetAttribute( "TTL", attribute_value.c_str() );
				attribute_value = getIndexedParameter( IP_REMAP_ACTION, i ).asString();
				ip_remap_node->SetAttribute( "Action", attribute_value.c_str() );
				ip_remap_table_node.InsertEndChild( *ip_remap_node );
			}
		}
	}

	content_node.InsertEndChild( ts_node );

	if ( hasParameter( SIZE_MINI_PAT_LIST ) ) { 
		content_node.InsertEndChild( pats_node );
	}

	if ( hasParameter( SIZE_CA_PROGRAM_LIST ) ) {
		content_node.InsertEndChild( cam_node );
	}

	if ( hasParameter( SIZE_IP_REMAP_LIST ) ) {	
		content_node.InsertEndChild( ip_remap_table_node );
	}

	root_node.InsertEndChild( network_node );
	root_node.InsertEndChild( satellite_node );
	root_node.InsertEndChild( content_node );

	doc->InsertEndChild( root_node );

	doc->SaveFile();

	delete doc;

	return 0;
}



int Receiver::statusToXML( string file_path ) 
{
	TiXmlDocument *doc=NULL;

	doc = new TiXmlDocument( file_path.c_str() );
	statusToXML( doc );
	doc->SaveFile();

	delete doc;

	return 0;
}



int Receiver::definitionToXML( string file_path ) 
{
	TiXmlDocument *doc=NULL;

	doc = new TiXmlDocument( file_path.c_str() );
	definitionToXML( doc );
	doc->SaveFile();

	delete doc;

	return 0;
}


int Receiver::configToXML( string file_path ) 
{
	TiXmlDocument *doc=NULL;

	doc = new TiXmlDocument( file_path.c_str() );
	configToXML( doc );
	doc->SaveFile();

	delete doc;

	return 0;
}


int Receiver::toXML( string file_path ) 
{
	TiXmlDocument *doc=NULL;

	doc = new TiXmlDocument( file_path.c_str() );
	toXML( doc );
	doc->SaveFile();

	delete doc;

	return 0;
}


int Receiver::statusToXML( FILE *f ) 
{
	TiXmlDocument *doc=NULL;


	doc = new TiXmlDocument();
	statusToXML( doc );
	doc->SaveFile( f );

	delete doc;

	return 0;
}



int Receiver::definitionToXML( FILE *f ) 
{
	TiXmlDocument *doc=NULL;


	doc = new TiXmlDocument();
	definitionToXML( doc );
	doc->SaveFile( f );

	delete doc;

	return 0;
}


int Receiver::configToXML( FILE *f ) 
{
	TiXmlDocument *doc=NULL;


	doc = new TiXmlDocument();
	configToXML( doc );
	doc->SaveFile( f );

	delete doc;

	return 0;
}


int Receiver::toXML( FILE *f ) 
{
	TiXmlDocument *doc=NULL;


	doc = new TiXmlDocument();
	toXML( doc );
	doc->SaveFile( f );

	delete doc;

	return 0;
}


void Receiver::statusToXML( TiXmlDocument *doc ) 
{
	TiXmlElement root_node( "RECEIVER_STATUS" );
	TiXmlElement *param_node = NULL;
	TiXmlElement *indexed_param_node = NULL;
	TiXmlText *param_value = NULL;
	TiXmlElement *program_number_node = NULL;
	TiXmlElement *program_status = NULL;
	unsigned short program_number;
	int i;
//	struct timeval tv;
//	struct tm *time_date;;
//	char time_str[30];
	char temp[20];
	

	
//	memset( time_str, 0, sizeof( time_str ) );
//	gettimeofday( &tv, NULL );
//	time_date = gmtime( &(tv.tv_sec) );
//	sprintf( time_str, "%d/%02d/%02d %d:%02d:%02d.%03d",
//		 1900+time_date->tm_year, time_date->tm_mon, time_date->tm_mday, time_date->tm_hour, time_date->tm_min, time_date->tm_sec, (int)( tv.tv_usec / 1000 ) );
//	root_node.SetAttribute( "TIME_STAMP", time_str );
	root_node.SetAttribute( "TIME_STAMP", timestamp().c_str() );
//	root_node.SetAttribute( "RECEIVER_IP", getParameter( RECEIVER_IP ).asString().c_str() );
//	root_node.SetAttribute( "RECEIVER_MAC", getParameter( RECEIVER_MAC ).asString().c_str() );
//	root_node.SetAttribute( "DEVICE_TYPE", getParameter( DEVICE_TYPE ).asString().c_str() );

	for ( i = 0; i < (int)MAX_RECEIVER_PARAM; i++ ) {
		if ( hasParameter( (ReceiverParameter)i ) && parameterIsIdentifier( (ReceiverParameter)i ) ) {
			param_node = new TiXmlElement( enumToString( (ReceiverParameter)0, i ).c_str() );
			param_value = new TiXmlText( getParameter( (ReceiverParameter)i ).asString().c_str() );
			param_node->InsertEndChild( *param_value );
			root_node.InsertEndChild( *param_node );
			delete param_node;
			delete param_value;
		}
	}
	for ( i = 0; i < (int)MAX_RECEIVER_PARAM; i++ ) {
		if ( hasParameter( (ReceiverParameter)i ) && parameterIsStatus( (ReceiverParameter)i ) &&
		     !parameterIsIdentifier( (ReceiverParameter)i ) && !parameterIsIndexed( (ReceiverParameter)i ) ) {
			param_node = new TiXmlElement( enumToString( (ReceiverParameter)0, i ).c_str() );
			param_value = new TiXmlText( getParameter( (ReceiverParameter)i ).asString().c_str() );
			param_node->InsertEndChild( *param_value );
			root_node.InsertEndChild( *param_node );
			delete param_node;
			delete param_value;
		}
	}
	if ( hasParameter( SIZE_CA_PROGRAM_LIST ) &&  hasParameter( CA_PROGRAM_STATUS ) ) {
		param_node = new TiXmlElement( "CA_PROGRAM_STATUS_LIST" );
		for ( i = 0; i < getParameter( SIZE_CA_PROGRAM_LIST ).asShort(); i++ ) {
			if ( getIndexedParameter( CA_PROGRAM_STATUS, i ).isSet() ) {
				program_number = 0;
				if ( hasParameter( CA_PROGRAM_NUMBER ) ) {
					program_number = getIndexedParameter( CA_PROGRAM_NUMBER, i ).asShort();
				} else {
					if ( hasParameter( VPROGRAM_NUMBER ) ) {
						program_number = getIndexedParameter( VPROGRAM_NUMBER, i ).asShort();
					}
				}
				if ( program_number != 0 ) {
					indexed_param_node = new TiXmlElement( "CA_PROGRAM_STATUS" );
					indexed_param_node->SetAttribute( "Index", itoa( i, temp, 10 ) );
					program_number_node = new TiXmlElement( "PROGRAM_NUMBER" );
					param_value = new TiXmlText( itoa( program_number, temp, 10 ) );
					program_number_node->InsertEndChild( *param_value );
					indexed_param_node->InsertEndChild( *program_number_node );
					delete param_value;
					delete program_number_node;
					program_status = new TiXmlElement( "PROGRAM_STATUS" );
					param_value = new TiXmlText( getIndexedParameter( CA_PROGRAM_STATUS, i ).asString().c_str() );
					program_status->InsertEndChild( *param_value );
					indexed_param_node->InsertEndChild( *program_status );
				 
					delete program_status;
					delete param_value;

					param_node->InsertEndChild( *indexed_param_node );
					delete indexed_param_node;
				}
			}
		}
		root_node.InsertEndChild( *param_node );
		delete param_node;
	}
	
	
	doc->InsertEndChild( root_node );
}


void Receiver::definitionToXML( TiXmlDocument *doc ) 
{
	TiXmlElement root_node( "RECEIVER_DEFINITION" );
	TiXmlElement *param_node = NULL;
	TiXmlText *param_value = NULL;
	int i;

//	root_node.SetAttribute( "RECEIVER_IP", getParameter( RECEIVER_IP ).asString().c_str() );
//	root_node.SetAttribute( "RECEIVER_MAC", getParameter( RECEIVER_MAC ).asString().c_str() );
//	root_node.SetAttribute( "DEVICE_TYPE", getParameter( DEVICE_TYPE ).asString().c_str() );

	for ( i = 0; i < (int)MAX_RECEIVER_PARAM; i++ ) {
		if ( hasParameter( (ReceiverParameter)i ) && getParameter( (ReceiverParameter)i ).isSet() && parameterIsIdentifier( (ReceiverParameter)i ) ) {
			param_node = new TiXmlElement( enumToString( (ReceiverParameter)0, i ).c_str() );
			param_value = new TiXmlText( getParameter( (ReceiverParameter)i ).asString().c_str() );
			param_node->InsertEndChild( *param_value );
			root_node.InsertEndChild( *param_node );
			delete param_node;
			delete param_value;
		}
	}
	for ( i = 0; i < (int)MAX_RECEIVER_PARAM; i++ ) {
		if ( hasParameter( (ReceiverParameter)i ) && getParameter( (ReceiverParameter)i ).isSet() && parameterIsDefinition( (ReceiverParameter)i ) &&
		     !parameterIsIdentifier( (ReceiverParameter)i ) && !parameterIsIndexed( (ReceiverParameter)i ) ) {
			param_node = new TiXmlElement( enumToString( (ReceiverParameter)0, i ).c_str() );
			param_value = new TiXmlText( getParameter( (ReceiverParameter)i ).asString().c_str() );
			param_node->InsertEndChild( *param_value );
			root_node.InsertEndChild( *param_node );
			delete param_node;
			delete param_value;
		}
	}
	
	doc->InsertEndChild( root_node );
}


void Receiver::configToXML( TiXmlDocument *doc ) 
{
	TiXmlElement root_node( "RECEIVER_CONFIGURATION" );
	TiXmlElement *param_node = NULL;
	TiXmlText *param_value = NULL;
	int i;

//	root_node.SetAttribute( "RECEIVER_IP", getParameter( RECEIVER_IP ).asString().c_str() );
//	root_node.SetAttribute( "RECEIVER_MAC", getParameter( RECEIVER_MAC ).asString().c_str() );
//	root_node.SetAttribute( "DEVICE_TYPE", getParameter( DEVICE_TYPE ).asString().c_str() );

	for ( i = 0; i < (int)MAX_RECEIVER_PARAM; i++ ) {
		if ( hasParameter( (ReceiverParameter)i ) && getParameter( (ReceiverParameter)i ).isSet() && parameterIsIdentifier( (ReceiverParameter)i ) ) {
			param_node = new TiXmlElement( enumToString( (ReceiverParameter)0, i ).c_str() );
			param_value = new TiXmlText( getParameter( (ReceiverParameter)i ).asString().c_str() );
			param_node->InsertEndChild( *param_value );
			root_node.InsertEndChild( *param_node );
			delete param_node;
			delete param_value;
		}
	}
	for ( i = 0; i < (int)MAX_RECEIVER_PARAM; i++ ) {
		if ( hasParameter( (ReceiverParameter)i ) && getParameter( (ReceiverParameter)i ).isSet() && parameterIsConfig( (ReceiverParameter)i ) &&
		     !parameterIsIdentifier( (ReceiverParameter)i ) && !parameterIsIndexed( (ReceiverParameter)i ) ) {
			param_node = new TiXmlElement( enumToString( (ReceiverParameter)0, i ).c_str() );
			param_value = new TiXmlText( getParameter( (ReceiverParameter)i ).asString().c_str() );
			param_node->InsertEndChild( *param_value );
			root_node.InsertEndChild( *param_node );
			delete param_node;
			delete param_value;
		}
	}
	
	doc->InsertEndChild( root_node );
}


/*
void Receiver::configToXML( TiXmlDocument *doc )
{
	TiXmlElement root_node( "RECEIVER_CONFIG" );
	TiXmlElement network_node( "NETWORK" );
	TiXmlElement satellite_node( "SATELLITE" );
	TiXmlElement signal_node( "SIGNAL" );
	TiXmlElement lnb_node( "LNB" );
	TiXmlElement lnb_spec_node( "LNBSpec" );
	TiXmlElement lnb_control_node( "LNBControl" );
	TiXmlElement content_node( "CONTENT" );
	TiXmlElement ts_node( "TRANSPORT_STREAM" );
	TiXmlElement pats_node( "PATS" );
	TiXmlElement cam_node( "CAM" );
//	TiXmlElement trap_node( "TRAPS" );
	TiXmlElement ip_remap_table_node( "IP_REMAP_TABLE" );
	TiXmlElement *pid_node=NULL;
	TiXmlElement *destination_node=NULL;
	TiXmlElement *pat_node=NULL;
	TiXmlElement *program_node=NULL;
	TiXmlElement *vprogram_node=NULL;
	TiXmlElement *ip_remap_node=NULL;

	string attribute_value;
	int i, j;
	int numPids=0;
	int numPrograms=0;
	int numDestinations=0;
	unsigned short mask=0;
	unsigned short used_destinations_mask = 0;
	int numPATs=0;
	int numCAPrograms=0;
	int numRemapRules=0;


	attribute_value = getParameter( RECEIVER_IP ).asString();
	network_node.SetAttribute( "ReceiverIP", attribute_value.c_str() );

	attribute_value = getParameter( SUBNET_MASK ).asString();
	network_node.SetAttribute( "SubnetMask", attribute_value.c_str() );

	attribute_value = getParameter( DEFAULT_GATEWAY_IP ).asString();
	network_node.SetAttribute( "DefaultGateway", attribute_value.c_str() );

	attribute_value = getParameter( UNICAST_STATUS_IP ).asString();
	network_node.SetAttribute( "StatusDestinationIP", attribute_value.c_str() );

	attribute_value = getParameter( UNICAST_STATUS_PORT ).asString();
	network_node.SetAttribute( "StatusDestinationPort", attribute_value.c_str() );

	if ( getParameter( IGMP_ENABLE ).asFlag() ) {
		network_node.SetAttribute( "IGMPFilter", "ON" );
	} else {
		network_node.SetAttribute( "IGMPFilter", "OFF" );
	}

	if ( hasParameter( LO_FREQUENCY ) ) {
		attribute_value = getParameter( LO_FREQUENCY ).asString();
		lnb_spec_node.SetAttribute( "LOFrequency", attribute_value.c_str() );
	}

	if ( hasParameter( HI_VOLTAGE_SWITCH ) ) {
		if ( hasParameter( POLARITY_SWITCHING_VOLTAGE ) ) {
			if ( getParameter( HI_VOLTAGE_SWITCH ).asFlag() ) {
				if ( !getParameter( POLARITY_SWITCHING_VOLTAGE ).asFlag() ) {
					lnb_spec_node.SetAttribute( "PolaritySwitchingVoltages", "12 & 20" );
				}
			} else {
				if ( getParameter( POLARITY_SWITCHING_VOLTAGE ).asFlag() ) {
					lnb_spec_node.SetAttribute( "PolaritySwitchingVoltages", "11 & 15" );
				} else {
					lnb_spec_node.SetAttribute( "PolaritySwitchingVoltages", "13 & 18" );
				}
			}
		} else {
			if ( getParameter( HI_VOLTAGE_SWITCH ).asFlag() ) {
				lnb_control_node.SetAttribute( "HiVoltageMode", "ON" );
			} else {
				lnb_control_node.SetAttribute( "HiVoltageMode", "OFF" );
			}
		}
	}

	if ( hasParameter( BAND_SWITCHING_TONE_FREQUENCY ) ) {
		if ( getParameter( BAND_SWITCHING_TONE_FREQUENCY ).asFlag() ) {
			lnb_spec_node.SetAttribute( "HiLowBandTone", "44" );
		} else {
			lnb_spec_node.SetAttribute( "HiLowBandTone", "22" );
		}
	}

	if ( getParameter( LNB_POWER ).asFlag() ) {
		lnb_control_node.SetAttribute( "Power", "ON" );
	} else {
		lnb_control_node.SetAttribute( "Power", "OFF" );
	}

	if ( getParameter( POLARITY ).asFlag() ) {
		lnb_control_node.SetAttribute( "Polarization", "Horizontal/Left" );
	} else {
		lnb_control_node.SetAttribute( "Polarization", "Vertical/Right" );
	}

	if ( getParameter( BAND ).asFlag() ) {
		lnb_control_node.SetAttribute( "Band", "High" );
	} else {
		lnb_control_node.SetAttribute( "Band", "Low" );
	}

	if ( getParameter( LONG_LINE_COMPENSATION_SWITCH ).asFlag() ) {
		lnb_control_node.SetAttribute( "LongLineCompensation", "ON" );
	} else {
		lnb_control_node.SetAttribute( "LongLineCompensation", "OFF" );
	}

	attribute_value = getParameter( LBAND_FREQUENCY ).asString();
	signal_node.SetAttribute( "RF", attribute_value.c_str() );

	attribute_value = getParameter( SYMBOL_RATE ).asString();
	signal_node.SetAttribute( "SymbolRate", attribute_value.c_str() );

	if ( hasParameter( GOLD_CODE ) ) {
		attribute_value = getParameter( GOLD_CODE ).asString();
		signal_node.SetAttribute( "GoldCode", attribute_value.c_str() );
	}

	switch ( getParameter( DVB_SIGNAL_TYPE_CONTROL ).asShort() ) {

		case DVBS	:	signal_node.SetAttribute( "SearchType", "DVBS" );
						break;
		case DVBS2	:	signal_node.SetAttribute( "SearchType", "DVBS2" );
						break;
		default		:	signal_node.SetAttribute( "SearchType", "AUTO" );
						break;

	}

	satellite_node.InsertEndChild( signal_node );
	lnb_node.InsertEndChild( lnb_spec_node );
	lnb_node.InsertEndChild( lnb_control_node );
	satellite_node.InsertEndChild( lnb_node );

	if ( hasParameter( FORWARD_ALL_FLAG ) ) {
		if ( getParameter( FORWARD_ALL_FLAG ).asFlag() ) {
			if ( hasParameter( FORWARD_NULLS_FLAG ) ) {
				if ( getParameter( FORWARD_NULLS_FLAG ).asFlag() ) {
					ts_node.SetAttribute( "PIDS", "ALL" );
				} else {
					ts_node.SetAttribute( "PIDS", "ALL Minus NULL" );
				}
			} else {
				ts_node.SetAttribute( "PIDS", "ALL Minus NULL" );
			}
			destination_node = new TiXmlElement( "DESTINATION" );
			attribute_value = getParameter( STREAM_DESTINATION_IP ).asString();
			destination_node->SetAttribute( "IPAddress", attribute_value.c_str() );
			attribute_value = getParameter( STREAM_DESTINATION_PORT ).asString();
			destination_node->SetAttribute( "Port", attribute_value.c_str() );
			ts_node.InsertEndChild( *destination_node );
		} else {
			if ( hasParameter( SIZE_PID_LIST ) ) {
				ts_node.SetAttribute( "PIDS", "Selected" );
			} else {
				ts_node.SetAttribute( "PIDS", "By Program" );
			}
//			pids_are_selected = true;
		}
	} else {
		if ( hasParameter( SIZE_PID_LIST ) ) {
			ts_node.SetAttribute( "PIDS", "Selected" );
		} else {
			ts_node.SetAttribute( "PIDS", "By Program" );
		}

//		pids_are_selected = true;
	}

//	if ( pids_are_selected ) {
		numPids = getParameter( SIZE_PID_LIST ).asShort();
		if ( hasParameter( SIZE_DESTINATION_LIST ) ) {
			numDestinations = getParameter( SIZE_DESTINATION_LIST ).asShort();
		}
		for ( i = 0; i < numPids; i++ ) {
			if ( getIndexedParameter( PID, i ).asShort() != 0x1fff ) {
				attribute_value = getIndexedParameter( PID, i ).asString();
				pid_node = new TiXmlElement( "PID" );
				pid_node->SetAttribute( "Number", attribute_value.c_str() );
				if ( hasParameter( RAW_FORWARD_FLAG ) ) {
					if ( getIndexedParameter( RAW_FORWARD_FLAG, i ).asFlag() ) {
						pid_node->SetAttribute( "Processing", "RAW" );
						if ( hasParameter( DESTINATION_MASK ) ) {
							mask = getIndexedParameter( DESTINATION_MASK, i ).asShort();
							if ( mask != 0 ) {
								for ( j = 0; j < numDestinations; j++ ) {
									if ( mask & ( 0x8000 >> j ) ) {
										used_destinations_mask = used_destinations_mask | ( 0x8000 >> j );
										destination_node = new TiXmlElement( "DESTINATION" );
										attribute_value = getIndexedParameter( PID_DESTINATION_IP, j ).asString();
										destination_node->SetAttribute( "IPAddress", attribute_value.c_str() );
										attribute_value = getIndexedParameter( PID_DESTINATION_PORT, j ).asString();
										destination_node->SetAttribute( "Port", attribute_value.c_str() );
										pid_node->InsertEndChild( *destination_node );
									}
								}
							}
						} else {
							destination_node = new TiXmlElement( "DESTINATION" );
							attribute_value = getIndexedParameter( PID_DESTINATION_IP, 0 ).asString();
							destination_node->SetAttribute( "IPAddress", attribute_value.c_str() );
							attribute_value = getIndexedParameter( PID_DESTINATION_PORT, 0 ).asString();
							destination_node->SetAttribute( "Port", attribute_value.c_str() );
							pid_node->InsertEndChild( *destination_node );
						}
					} else {
						pid_node->SetAttribute( "Processing", "MPE" );
					}
				} else {
					pid_node->SetAttribute( "Processing", "MPE" );
				}
				ts_node.InsertEndChild( *pid_node );
			}
		}
		if ( used_destinations_mask != 0xffff ) {
			pid_node = new TiXmlElement( "PID" );
			pid_node->SetAttribute( "Number", "NULL" );
			pid_node->SetAttribute( "Processing", "RAW" );
			for ( j = 0; j < numDestinations; j++ ) {
				if ( !( used_destinations_mask & ( 0x8000 >> j ) )  ) {
					if ( getIndexedParameter( PID_DESTINATION_PORT, j ).asShort() != 0 ) {
						destination_node = new TiXmlElement( "DESTINATION" );
						attribute_value = getIndexedParameter( PID_DESTINATION_IP, j ).asString();
						destination_node->SetAttribute( "IPAddress", attribute_value.c_str() );
						attribute_value = getIndexedParameter( PID_DESTINATION_PORT, j ).asString();
						destination_node->SetAttribute( "Port", attribute_value.c_str() );
						pid_node->InsertEndChild( *destination_node );
					}
				}
			}
			ts_node.InsertEndChild( *pid_node );
		}
//	}

	numPrograms = getParameter( SIZE_VPROGRAM_LIST ).asShort();
	for ( i = 0; i < numPrograms; i++ ) {
		if ( getIndexedParameter( VPROGRAM_NUMBER, i ).asShort() ) {
			attribute_value = getIndexedParameter( VPROGRAM_NUMBER, i ).asString();
			vprogram_node = new TiXmlElement( "PROGRAM" );
			vprogram_node->SetAttribute( "Number", attribute_value.c_str() );
			if ( getIndexedParameter( VPROGRAM_DESTINATION_PORT, i ).asShort() ) {
				attribute_value = getIndexedParameter( VPROGRAM_DESTINATION_IP, j ).asString();
				vprogram_node->SetAttribute( "IPAddress", attribute_value.c_str() );
				attribute_value = getIndexedParameter( VPROGRAM_DESTINATION_PORT, j ).asString();
				vprogram_node->SetAttribute( "Port", attribute_value.c_str() );
			}
			ts_node.InsertEndChild( *vprogram_node );
		}
	}


	if ( hasParameter( SIZE_MINI_PAT_LIST ) ) {
		numPATs = getParameter( SIZE_MINI_PAT_LIST ).asShort();
		for ( i = 0; i < numPATs; i++ ) {
			if ( getIndexedParameter( PROGRAM_NUMBER, i ).asShort() ) {
				pat_node = new TiXmlElement( "PAT" );
				attribute_value = getIndexedParameter( PROGRAM_NUMBER, i ).asString();
				pat_node->SetAttribute( "ProgramNumber", attribute_value.c_str() );
				attribute_value = getIndexedParameter( PMT_PID, i ).asString();
				pat_node->SetAttribute( "PMTPID", attribute_value.c_str() );
				pats_node.InsertEndChild( *pat_node );
			}
		}
	}

	if ( hasParameter( CAM_WATCHDOG_INTERVAL ) ) {
		attribute_value = getParameter( CAM_WATCHDOG_INTERVAL ).asString();
		cam_node.SetAttribute( "WatchdogTimeout", attribute_value.c_str() );
	}

	if ( hasParameter( SIZE_CA_PROGRAM_LIST ) ) {
		numCAPrograms = getParameter( SIZE_CA_PROGRAM_LIST ).asShort();
		for ( i = 0; i < numCAPrograms; i++ ) {
			if ( getIndexedParameter( CA_PROGRAM_NUMBER, i ).asShort() ) {
				program_node = new TiXmlElement( "Program" );
				attribute_value = getIndexedParameter( CA_PROGRAM_NUMBER, i ).asString();
				program_node->SetAttribute( "Number", attribute_value.c_str() );
				cam_node.InsertEndChild( *program_node );
			}
		}
	}

	if ( hasParameter( SIZE_IP_REMAP_LIST ) ) {
		if ( getParameter( IP_REMAP_ENABLE ).asFlag() ) {
			attribute_value = "true";
		} else {
			attribute_value = "false";
		}
		ip_remap_table_node.SetAttribute( "Enabled", attribute_value.c_str() );
		numRemapRules = getParameter( SIZE_IP_REMAP_LIST ).asShort();
		for ( i = 0; i < numRemapRules; i++ ) {
			if ( getIndexedParameter( ORIGINAL_IP, i ).asString() != "0.0.0.0" ) {
				ip_remap_node = new TiXmlElement( "IP_Remap_Rule" );
				attribute_value = getIndexedParameter( ORIGINAL_IP, i ).asString();
				ip_remap_node->SetAttribute( "Original_IP", attribute_value.c_str() );
				attribute_value = getIndexedParameter( NEW_IP, i ).asString();
				ip_remap_node->SetAttribute( "New_IP", attribute_value.c_str() );
				attribute_value = getIndexedParameter( IP_REMAP_MASK, i ).asString();
				ip_remap_node->SetAttribute( "Mask", attribute_value.c_str() );
				attribute_value = getIndexedParameter( IP_REMAP_TTL, i ).asString();
				ip_remap_node->SetAttribute( "TTL", attribute_value.c_str() );
				attribute_value = getIndexedParameter( IP_REMAP_ACTION, i ).asString();
				ip_remap_node->SetAttribute( "Action", attribute_value.c_str() );
				ip_remap_table_node.InsertEndChild( *ip_remap_node );
			}
		}
	}

	content_node.InsertEndChild( ts_node );

	if ( hasParameter( SIZE_MINI_PAT_LIST ) ) { 
		content_node.InsertEndChild( pats_node );
	}

	if ( hasParameter( SIZE_CA_PROGRAM_LIST ) ) {
		content_node.InsertEndChild( cam_node );
	}

	if ( hasParameter( SIZE_IP_REMAP_LIST ) ) {	
		content_node.InsertEndChild( ip_remap_table_node );
	}

	root_node.InsertEndChild( network_node );
	root_node.InsertEndChild( satellite_node );
	root_node.InsertEndChild( content_node );

	doc->InsertEndChild( root_node );
}
*/


void Receiver::toXML( TiXmlDocument *doc ) 
{
	TiXmlElement root_node( "RECEIVER" );
	TiXmlElement *param_node = NULL;
	TiXmlElement *indexed_param_node = NULL;
	TiXmlText *param_value = NULL;
	TiXmlElement *program_number_node = NULL;
	TiXmlElement *program_status = NULL;
	unsigned short program_number;
	int i;
//	struct timeval tv;
//	struct tm *time_date;;
//	char time_str[30];
	char temp[20];
	

	
//	memset( time_str, 0, sizeof( time_str ) );
//	gettimeofday( &tv, NULL );
//	time_date = gmtime( &(tv.tv_sec) );
//	sprintf( time_str, "%d/%02d/%02d %d:%02d:%02d.%03d",
//		 1900+time_date->tm_year, time_date->tm_mon, time_date->tm_mday, time_date->tm_hour, time_date->tm_min, time_date->tm_sec, (int)( tv.tv_usec / 1000 ) );
//	root_node.SetAttribute( "TIME_STAMP", time_str );
	root_node.SetAttribute( "TIME_STAMP", timestamp().c_str() );
//	root_node.SetAttribute( "RECEIVER_IP", getParameter( RECEIVER_IP ).asString().c_str() );
//	root_node.SetAttribute( "RECEIVER_MAC", getParameter( RECEIVER_MAC ).asString().c_str() );
//	root_node.SetAttribute( "DEVICE_TYPE", getParameter( DEVICE_TYPE ).asString().c_str() );

	for ( i = 0; i < (int)MAX_RECEIVER_PARAM; i++ ) {
		if ( hasParameter( (ReceiverParameter)i ) && getParameter( (ReceiverParameter)i ).isSet() && parameterIsIdentifier( (ReceiverParameter)i ) ) {
			param_node = new TiXmlElement( enumToString( (ReceiverParameter)0, i ).c_str() );
			param_value = new TiXmlText( getParameter( (ReceiverParameter)i ).asString().c_str() );
			param_node->InsertEndChild( *param_value );
			root_node.InsertEndChild( *param_node );
			delete param_node;
			delete param_value;
		}
	}
	for ( i = 0; i < (int)MAX_RECEIVER_PARAM; i++ ) {
		if ( hasParameter( (ReceiverParameter)i ) && getParameter( (ReceiverParameter)i ).isSet() && !parameterIsIdentifier( (ReceiverParameter)i ) && !parameterIsIndexed( (ReceiverParameter)i ) ) {
			param_node = new TiXmlElement( enumToString( (ReceiverParameter)0, i ).c_str() );
			param_value = new TiXmlText( getParameter( (ReceiverParameter)i ).asString().c_str() );
			param_node->InsertEndChild( *param_value );
			root_node.InsertEndChild( *param_node );
			delete param_node;
			delete param_value;
		}
	}
	
	doc->InsertEndChild( root_node );
}

