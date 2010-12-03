#include "EnumeratedReceiverParameter.h"

#include "ReceiverParameter.h"


EnumeratedReceiverParameter::EnumeratedReceiverParameter( )
			:ParameterValue()
{
	m_p = (ReceiverParameter)0;
}


EnumeratedReceiverParameter::EnumeratedReceiverParameter( ReceiverParameter p )
			:ParameterValue()
{
	m_p = p;
}


EnumeratedReceiverParameter::EnumeratedReceiverParameter( ReceiverParameter p, string value )
			:ParameterValue( (DataType)F_SHORT, value )
{
	m_p = p;
}



EnumeratedReceiverParameter::~EnumeratedReceiverParameter()
{
}


void EnumeratedReceiverParameter::setEnumType( ReceiverParameter p )
{
	m_p = p;
}


string EnumeratedReceiverParameter::asString( )
{
        char temp[10];
	string return_str = "";
	int masked_id;
	unsigned short val;

	val = asShort();

	switch ( m_p ) {

		case ALL_PARAMETERS:	switch ( val ) {
					case ALL_PARAMETERS				:	return_str = "ALL_PARAMETERS";
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
					default						:	return_str = string( itoa( val, temp, 10 ) );
												break;
				}
				break;

		case DEVICE_TYPE : switch ( val ) {

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


		case LNB_POWER	:		if ( val ) {
							return_str = "On";
						} else {
							return_str = "Off";
						}
						break;


		case BAND 	:		if ( val ) {
							return_str = "High";
						} else {
							return_str = "Low";
						}
						break;


		case POLARITY 	:		if ( val ) {
							return_str = "Horizontal/Left";
						} else {
							return_str = "Vertical/Right";
						}
						break;


		case HI_VOLTAGE_SWITCH 	:	if ( val ) {
							return_str = "On";
						} else {
							return_str = "Off";
						}
						break;


		case LONG_LINE_COMPENSATION_SWITCH 	:	if ( val ) {
									return_str = "On";
								} else {
									return_str = "Off";
								}
								break;


		case BAND_SWITCHING_TONE_FREQUENCY 	:	if ( val ) {
									return_str = "44 kHz";
								} else {
									return_str = "22 kHz";
								}
								break;


		case POLARITY_SWITCHING_VOLTAGE	:	if ( val ) {
								return_str = "11-15V";
							} else {
								return_str = "13-18V";
							}
							break;


		case SIGNAL_LOCK	:	if ( val ) {
							return_str = "Locked";
						} else {
							return_str = "No Lock";
						}
						break;


		case DATA_LOCK		:	if ( val ) {
							return_str = "Locked";
						} else {
							return_str = "No Lock";
						}
						break;


		case LNB_FAULT		:	if ( val ) {
							return_str = "Fault";
						} else {
							return_str = "No Fault";
						}
						break;


		case IGMP_ENABLE	:	if ( val ) {
							return_str = "Enabled";
						} else {
							return_str = "Disabled";
						}
						break;


		case VITERBI_RATE : switch ( val ) {

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

		case MODCOD		: switch ( val ) {

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

		case DVB_SIGNAL_TYPE	:	if ( val ) {
												return_str = "DVBS2";
											} else {
												return_str = "DVBS";
											}
											break;

		case DVB_SIGNAL_TYPE_CONTROL	:	switch ( val ) {
					
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


		case SPECTRAL_INVERSION_FLAG	:	if ( val ) {
												return_str = "Normal";
											} else {
												return_str = "Inverted";
											}
											break;

		case PILOT_SYMBOL_FLAG			:	if ( val ) {
												return_str = "On";
											} else {
												return_str = "Off";
											}
											break;

		case FRAME_LENGTH				:	if ( val ) {
												return_str = "Short";
											} else {
												return_str = "Long";
											}
											break;

		case PROGRAM_STREAM_TYPE		:	switch ( val ) {
					
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


		case CAM_STATUS				:	switch ( val ) {
					
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


		case CA_PROGRAM_STATUS			:	switch ( val ) {
					
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


		case CA_PROGRAM_SCRAMBLED_FLAG	:	if ( val ) {
								return_str = "Scrambled";
							} else {
								return_str = "Clear";
							}
							break;


		case CA_SYSTEM_ID	:	masked_id = val & 0xff00;
								if ( val != 0 ) {
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
														sprintf( temp, "0x%04x", val );
														return_str = string( temp );
														break;
													
									}

								} else {

									return_str = "No CA System";

								}

								break;


		case RAW_FORWARD_FLAG	:	if ( val ) {
							return_str = "RAW";
						} else {
							return_str = "MPE";
						}
						break;


		case FORWARD_ALL_FLAG	:	if ( val ) {
							return_str = "Filters Off";
						} else {
							return_str = "Filtering";
						}
						break;


		case FORWARD_NULLS_FLAG	:	if ( val ) {
							return_str = "NULLS On";
						} else {
							return_str = "No NULLS";
						}
						break;


		case FIXED_KEY_SIGN	:	if ( val ) {
							return_str = "Odd";
						} else {
							return_str = "Even";
						}
						break;


		case IP_REMAP_ENABLE	:	if ( val ) {
							return_str = "Enabled";
						} else {
							return_str = "Disabled";
						}
						break;


		case IP_REMAP_ACTION	:	switch ( val ) {
				
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
		
	return return_str;
}


