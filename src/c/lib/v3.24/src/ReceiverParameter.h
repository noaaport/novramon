#ifndef _RECEIVER_PARAMETER

#define _RECEIVER_PARAMETER

#undef DEVICE_TYPE

typedef enum ParameterSet	/// Used to enumerate the receiver parameter sets
{

	CURRENT, 
	PENDING

} ParameterSet;


typedef enum ReceiverParameter	/// Used to identify receiver parameters by name.  Not all parameters have meaning with all receivers.
{

	ALL_PARAMETERS,	///< represents all of the receiver's parameters

	// READ ONLY SECTION
	//--------------------------------

	// FIXED READ ONLY VALUES
	//---------------------------------------
	
	// DEVICE INFO
	
	DEVICE_TYPE,	///< the device type
	UNIT_ID_CODE,	///< unit identification code
	DSP_VERSION,	///< Digital Signal Processoer firmware version
	DSP_REVISION,	///< Digital Signal Processoer firmware revision
	RF_FIRMWARE_VERSION,	///< RF chip firmware version
	CAM_VERSION,	///< Conditional Access Module controller firmware version
	CAM_REVISION,	///< Conditional Access Module controller firmware revision
	FPGA_VERSION,	///< Field Programmable Gate Array firmware version
	FPGA_REVISION,	///< Field Programmable Gate Array firmware revision
	RECEIVER_MAC,	///< receiver Media Access Control address
	SIZE_PID_LIST,	///< the maximum number of PIDS that can be processed by the receiver
	SIZE_CA_PROGRAM_LIST,	///< the maximum number of Conditional Access programs supported by the receiver
	SIZE_DESTINATION_LIST,	///< the maximum number of destinations that raw data can be forwarded to
	SIZE_MINI_PAT_LIST,	///< the number of mini Program Allocation Tables the receiver can send
	SIZE_FIXED_KEY_LIST,	///< the number of Fixed Keys the receiver can hold
	SIZE_IP_REMAP_LIST,	///< the size of the Internet Protocol remapping table
	SIZE_VPROGRAM_LIST,	///< the size of the program list in a V receiver
	SIZE_PROGRAM_GUIDE,	///< the size of the program guide (in entries)
	SIZE_AUDIO_PID_LIST,	///< the maximum number of audio pids the receiver can list for each program
	MPE,			///< a flag that identifies whether the receiver supports Multi-Protocol Encapsulation
	CODE_DOWNLOAD,	///< a flag that identifies whether the receiver supports firmware code downloads
	NETWORK_CODE_DOWNLOAD,  ///< a flag that identifies whether the receiver supports firmware code download over a WAN
	FACTORY_RESET,	///< a flag that identifies whether the receiver has a factory reset option
	AUTO_SYMBOL_RATE,	///< a flag that identifes whether the receiver supports automatic symbol rate detection
	AUTO_DVBS_TYPE,	///< a flag that identifes whether the receiver supports the ability to automatically search for different types of DVBS signals at the same time
	
	// DYNAMIC READ ONLY VALUES
	//--------------------------------------------
	
	STATUS_TIMESTAMP,	///< the time when the last status update was aquired

	// SIGNAL
	
	FRONT_END_LOCK_LOSS,	///< a count of how many times Front End Lock was lost in the last half second
	DATA_SYNC_LOSS,	///< a count of how many times data sync was lost in the last half second
	CARRIER_FREQUENCY,	///< the carrier frequency
	
	// RF

	DVB_SIGNAL_TYPE,	///< the DVB signal type (DVBS or DVBS2)
	VITERBI_RATE,	///< the viterbi rate
	MODCOD,	///< the modulation code
	SIGNAL_STRENGTH_AS_DBM,	///< signal strength measured in dBm
	SIGNAL_STRENGTH_AS_PERCENTAGE,	///< sigmal strength as a percentage
	SIGNAL_LOCK,	///< a flag indicating whether the receiver is locked on to the carrier
	DATA_LOCK,	///< a flag indicating whether the receiver is locked onto the symbol rate
	LNB_FAULT,	///< a flag indicating whether a fault has been detected with the Low Noise Block downconverter
	VBER,	///< the viterbi bit error rate
	PER,
	UNCORRECTABLES,	///< the number of uncorrectable errors detected in the last half second
	AGC,	///< the automatic gain control being used by the tuner
	CARRIER_TO_NOISE,	///< the signal carrier to noise ratio
	DEMODULATOR_GAIN,	///< the gain being used by the demodulator chip
	AGCA,
	AGCN,
	GNYQA,
	GFARA,
	NEST,	///< noise estimate
	CLOCK_OFFSET,	
	FREQUENCY_OFFSET,	///< the difference between the specified frequency and the locked frequency
	FREQUENCY_ERROR,	///< the difference between the carrier frequency and the locked frequency
	LOCKED_SYMBOL_RATE,	///< the symbol rate to which the receiver is locked
	ADC_MID,
	ADC_CLIP,
	DIGITAL_GAIN,
	AA_CLAMP,
	SPECTRAL_INVERSION_FLAG,	///< spectral inversion
	PILOT_SYMBOL_FLAG,	
	FRAME_LENGTH,

	// ATSC
	
	DEMOD_STATS,
	VSB_MODE,
	SER_1,
	SER_10,
	SNR_BE,
	SNR_AE,
	CARRIER_OFFSET,
	IF_AGC,
	RF_AGC,
	FEC_UNERR,
	FEC_CORR,
	FEC_UNCORR,
	FIELD_LOCKED,
	EQUALIZER_STATE,
	CHANNEL_STATE,
	NTSC_FILTER,
	INVERSE_SPECTRUM,
	
	// CONTENT

	// DVB
	
	PID_COUNT,	///< the number of PIDs programmed into the receiver
	PROGRAM_GUIDE_PROGRAM_NUMBER,	///< a program number in the program guide
	PROGRAM_GUIDE_PROGRAM_NAME,	///< a program name in the program guide
	DVB_ACCEPTED,	///< the number of DVB packets accepted in the last half second
	TOTAL_DVB_PACKETS_ACCEPTED,	///< the total number of DVB packets received since startup
	TOTAL_DVB_PACKETS_RX_IN_ERROR,	///< the total number of DVB packets received with errors since startup
	TOTAL_UNCORRECTABLE_TS_PACKETS,	///< the total number of uncorrectable transport stream packets since startup
	PMT_VERSION,	///< the version number of a program map table
	PROGRAM_STREAM_TYPE, ///< the type of information in a program stream
	DATA_PID,	///< The PID carrying data within a program
	VIDEO_PID,	///< the PID carrying video within a program
	PCR_PID,	///< the PID carrying the program clock reference within a program
	AUDIO_PID,	///< an audio pid within a program
	AUDIO_PID_LANGUAGE,	///< the language used in an audio PID within a program
	AUDIO_TYPE,	///< the audio type used in an audio PID within a program

	// ETHERNET
		
	ETHERNET_TRANSMIT,	///< the number of ethernet packets transitted in the last half second
	ETHERNET_RECEIVE,	///< the number of ethernet packets received in the last half second
	ETHERNET_TRANSMIT_ERROR,	///< the number of ethernet transmission errors in the last half second
	ETHERNET_PACKET_DROPPED,	///< the number of ethernet packets dropped in the last half second
	ETHERNET_RECEIVE_ERROR,	///< the number of ethernet packets received in error in the last half second
	TOTAL_ETHERNET_PACKETS_OUT,	///< the total number of ethernet packets sent since startup

	// CONDITIONAL ACCESS
	
	CAM_STATUS,	///< the status of the Conditional Access Module
	DVB_SCRAMBLED,	///< the number of DVB packets scrambled
	DVB_CLEAR,	///< the number of DVB packets in the clear
	CA_PROGRAM_STATUS,	///< the status of a conditional access program
	CA_SYSTEM_ID,	///< conditional access system ID for a program
	CA_PROGRAM_SCRAMBLED_FLAG,	///< the original state of a received program (0=clear, 1=scrambled)
	
	// READ/WRITE SECTION
	//-----------------------------------

	// ETHERNET
	
	RECEIVER_IP,	///< the receiver's Internet Protocol address
	SUBNET_MASK,	///< the receivers subnet mask
	DEFAULT_GATEWAY_IP,	///< the internet protocol address the receiver is using for a gateway
	RAW_TRAFFIC_DEST_IP,	///< an internet protocol address where raw traffic is sent
	RAW_TRAFFIC_DEST_PORT,	///< a port number where raw traffic is sent
	UNICAST_STATUS_IP,	///< the internet protocol address the receiver is sending status packets to
	UNICAST_STATUS_PORT,	///< the port that the receiver is unicasting status packets to
	BROADCAST_STATUS_PORT,	///< the port to which the receiver is broadcasting status packets to
	IGMP_ENABLE,	///< a flag to enable or disable Internet Group Management Protocol on the receiver

	// RF

	LBAND_FREQUENCY,	///< the LBand frequency the receiver is tuned to
	LO_FREQUENCY,	///< the Local Oscillator frequency on the Low Noise Block downconverter
	SYMBOL_RATE,	///< the symbol rate the receiver is programmed to lock to (0 for Auto-lock)
	GOLD_CODE,	///< the modulation gold code
	ISI_ENABLE,	///< a flag indicating whether or not the receiver should use ISI (Input Stream Identifier) filtering
	INPUT_STREAM_IDENTIFIER, ///> an 8 bit number identifying an input stream (or stream group)
	INPUT_STREAM_IDENTIFIER_MASK, ///> an 8 bit mask, that when applied to an ISI number can be used to match a group of streams
	TARGET_MODCOD, ///> the modulation code that should be used for reception
	LNB_POWER,	///< a flag indicating whether the receiver should provide power to the Low Noise Block downconverter
	BAND,	///< the signal band
	POLARITY,	///< the signal polarity
	HI_VOLTAGE_SWITCH,	///< a flag indicating whether to switch to a broader LNB voltage range
	LONG_LINE_COMPENSATION_SWITCH,	///< a switch indicating that the receiver should compensate for a long RF cable attached
	POLARITY_SWITCHING_VOLTAGE,	///< a switch indicating what polarity switching volatges to apply to the LNB
	BAND_SWITCHING_TONE_FREQUENCY,	///< a switch indicating what band switching frequencies to use
	DVB_SIGNAL_TYPE_CONTROL,	///< the DVB signal type to search for (DVBS, DVBS2, or AUTO if supported)

	// ATSC
	
	CHANNEL_NUMBER,	///< an ATSC channel number
	
	// CONTENT SELECTION, DESCRAMBLING AND ROUTING

    DVB_MAC,	///< the Digital Video Broadcasting Media Access Control address
	PID,	///< packet identifier in the filter list
	RAW_FORWARD_FLAG,	///< a flag associated with a PID specifying whether its packets are to be forwared as raw data
	DESTINATION_MASK,	///< a mask associated with a PID that identfies which IP destinations raw data should be sent to
	PID_DESTINATION_IP,	///< an IP destination for raw PID data
	PID_DESTINATION_PORT,	///< a UDP destination port for raw PID data
	PROGRAM_NUMBER,	///< a DVB program number
	PMT_PID,	///< a Program Map Table PID
	CA_PROGRAM_NUMBER,	///< a program that is to be descrambled
	VPROGRAM_NUMBER, ///< a video program on a V receiver
	PROGRAM_NAME,	///< a program name from the SI tables
	VPROGRAM_DESTINATION_IP, ///< an IP destination for a program on a V receiver
	VPROGRAM_DESTINATION_PORT,	///< a UDP destination port for a program on a V receiver
	CAM_WATCHDOG_INTERVAL,	///< the timeout to be applied to the CAM watchdog
	FORWARD_ALL_FLAG,	///< a flag that tells the receiver whether to forward an entire transport stream or ot filter
	FORWARD_NULLS_FLAG,	///< a flag that tells the receiver whether to include NULL packets when forwarding an entire transport stream
	STREAM_DESTINATION_IP,	///< the destination IP when the entire transport stream is being forwared
	STREAM_DESTINATION_PORT,	///< the destination port when the entire transport stream is being forwarded
//	CIPHER_SABER_ENABLE,	///< a flag indicating whether or not the receiver should encrypt output using Cipher Saber
	CIPHER_SABER_ITERATIONS, ///> number of iterations to perform when using Cipher Saber
	CIPHER_SABER_KEY_FILE, ///> Cipher Saber key file
	IP_REMAP_ENABLE,	///< a flag indicating whether or not IP remapping is enabled
	ORIGINAL_IP,	///< an original IP in a remap rule
	NEW_IP,	///< the new IP in a remap rule
	IP_REMAP_MASK,	///< a network remap mask in a remap rule
	IP_REMAP_TTL,	///< the time to live component of a remap rule
	IP_REMAP_ACTION,	///< the action component of a remap rule

	// TRAP CONTROL
	
	TRAP_MONITORING_INTERVAL,	///< the trap monitoring interval
	TRAP_NOTIFICATION_IP,	///< the IP destination to send trap packets to
	TRAP_NOTIFICATION_PORT,	///< the UDP port to send trap packets to
	DATA_LOCK_MONITOR_TRAP_SWITCH,	///< switch to turn on/off data lock monitor traps
	ETHERNET_TX_ERROR_TRAP_SWITCH,	///< switch to turn on/off ethernet transmission error traps 
	TS_ERROR_TRAP_SWITCH,	///< switch to turn on/off transport stream error traps
	VBER_TRAP_SWITCH,	///< switch to turn on/off viterbi bit error rate traps
	VBER_TRAP_THRESHOLD,	///< the threshold for triggering the viterbi bit error rate trap
	UNCORRECTABLE_TS_ERROR_TRAP_THRESHOLD, ///< the threshold for triggering the transport stream error trap
	ETHERNET_TX_ERROR_TRAP_THRESHOLD,	///< the threshold for triggering the ethernet TX error trap


	// FIXED KEY

	FIXED_KEY_PID,	///< a PID having a fixed key

	// WRITE ONLY SECTION

	PASSWORD,	///< the receiver password
	FIXED_KEY,	///< a fixed key
	FIXED_KEY_SIGN,	///< the sign of a fixed key

	MAX_RECEIVER_PARAM	///< the number of parameters in this enumeration
	
} ReceiverParameter;



/*
typedef enum NOVRA_DEVICE		/// Used to enumerate the Novra device types
{ 
	UNKNOWN,		///< A device that reports firmware that is not recognised or bad status packet.
	S75_V_1,		///< S75 Version 1 device, if we ever decide to support them.
	S75D_V21,		///< S75 Version 2.X device.
	S75D_V3,		///< S75 Version 3.X device.
	A75,			///< A75 device.
	PesMaker,		///< S75 Version 5.x PesMaker device.
	S75CA,			///< S75 Conditional Access device.
	A75PesMaker,	///< A75 PesMaker device.
	S75J,			///< S75 JSAT Leo-Palace device.
	S75FK,			///< S75 Fixed Key device.
	S75PLUS,		///< S75 Plus device.
	S75CAPRO,		///< S75 Conditional Access device with support for the PRO feature set
	S75FKPRO,		///< S75 Fixed Key device with support for the PRO feature set
	S75PLUSPRO,		///< S75 Plus device with support for the PRO feature set
	S75IPREMAP,
	S200
};
*/

#endif

