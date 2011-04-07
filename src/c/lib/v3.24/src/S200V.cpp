//#include "stdafx.h"

#include "S200V.h"

#ifdef WINDOWS
    #include "Winsock2.h"
#endif
#ifndef WINDOWS
   #include <netinet/in.h>
   #include <sys/socket.h>
   #include <arpa/inet.h>
   #include <unistd.h>

   typedef int SOCKET;
#endif
#include "math.h"
#include "Password.h"
#include "Keycrypt.h"
#include "ConfigPorts.h"

#include <time.h>

#define NUMBER_OF_PIDS						0
#define NUMBER_OF_DESTINATIONS				16
#define NUMBER_OF_PATS						0
#define NUMBER_OF_PROGRAMS					0
#define NUMBER_OF_FIXED_KEYS				0
#define NUMBER_OF_IP_REMAP_RULES			0
#define NUMBER_OF_VPROGRAMS					12
#define NUMBER_OF_PROGRAM_GUIDE_ENTRIES		300
#define NUMBER_OF_AUDIO_PIDS_PER_PROGRAM	16

const int PAT_CONFIG_PORT		= 0x1952;
const int PROGRAM_CONFIG_PORT	= 0x1958;
const int NETWORK_CONFIG_PORT	= 0x1975;
const int PID_CONFIG_PORT		= 0x1977;
const int RF_CONFIG_PORT		= 0x1983;
const int PRODUCTID_CONFIG_PORT	= 0x1984;
const int TRAP_CONFIG_PORT		= 0x2005;
const int SET_PASSWORD_PORT		= 0x2010;
const int STATUS_POLL_PORT		= 0x2011;
const int FK_CONFIG_PORT		= 0x2022;
const int CIPHER_SABER_PORT		= 0x9899;
const int PROGRAM_INFO_PORT		= 0x9992;
const int VIDEO_PROGRAM_PORT	= 0x9993;
const int IP_REMAP_PORT			= 0x9994;
const int DEFAULT_CONFIG_PORT	= 0x9995;
const int PSI_TABLE_PORT		= 0x9996;
const int CAM_WATCHDOG_PORT		= 0x9997;
const int RESET_PORT			= 0x9999;




/*
enum CONFIG_PORT_INDEXES {	I_PAT_CONFIG_PORT,
							I_PROGRAM_CONFIG_PORT,
							I_NETWORK_PORT,
							I_PID_PORT,
							I_RF_PORT,
							I_PRODUCTID_CONFIG_PORT,
							I_TRAP_CONFIG_PORT,
							I_SET_PASSWORD_PORT,
							I_STATUS_POLL_PORT,
							I_FK_CONFIG_PORT,
							I_PROGRAM_INFO_PORT,
							I_VIDEO_PROGRAM_PORT,
							I_IP_REMAP_CONFIG_PORT,
							I_DEFAULT_CONFIG_PORT,
							I_PSI_TABLE_PORT,
							I_CAM_WATCHDOG_PORT,
							I_RESET_PORT			};
*/


/*
static const unsigned short config_ports[] = {	PAT_CONFIG_PORT,
												PROGRAM_CONFIG_PORT,
												NETWORK_CONFIG_PORT,
												PID_CONFIG_PORT,
												RF_CONFIG_PORT,
												PRODUCTID_CONFIG_PORT,
												TRAP_CONFIG_PORT,
												SET_PASSWORD_PORT,
												STATUS_POLL_PORT,												
												FK_CONFIG_PORT,
												PROGRAM_INFO_PORT,
												VIDEO_PROGRAM_PORT,
												IP_REMAP_PORT,
												DEFAULT_CONFIG_PORT,
												PSI_TABLE_PORT,
												CAM_WATCHDOG_PORT,
												RESET_PORT			};
*/

static ReceiverParameter networkParams[] = {	RECEIVER_IP,
												SUBNET_MASK,
												DEFAULT_GATEWAY_IP,
												UNICAST_STATUS_IP,
												UNICAST_STATUS_PORT,
												BROADCAST_STATUS_PORT,
												IGMP_ENABLE				};


static ReceiverParameter rfParams[] = {	LBAND_FREQUENCY,
										SYMBOL_RATE,
										DVB_SIGNAL_TYPE_CONTROL,
										LNB_POWER,
										BAND,
										POLARITY,
										HI_VOLTAGE_SWITCH,
										LONG_LINE_COMPENSATION_SWITCH,
										POLARITY_SWITCHING_VOLTAGE,
										BAND_SWITCHING_TONE_FREQUENCY,
										LO_FREQUENCY  	 				};

static ReceiverParameter vprogramParams[] = {	VPROGRAM_NUMBER,
												VPROGRAM_DESTINATION_IP,
												VPROGRAM_DESTINATION_PORT,
												STREAM_DESTINATION_IP,
												STREAM_DESTINATION_PORT,
												FORWARD_ALL_FLAG,
												FORWARD_NULLS_FLAG		};

typedef unsigned char BYTE;
typedef unsigned short WORD;

typedef struct S200VStatusRequestMSG {
	BYTE Message[6];	 
} S200VStatusRequestMSG;


typedef struct S200VStatusMsg {
	short Temperature;		  				// Circuit board temperature
	unsigned short  unused;
	BYTE			FPGAVersion;					// FKCA device ID, firmware version
	BYTE			FPGARevision;					// FKCA device ID, firmware revision
	short			LNBvoltage;							// LNB voltage
	unsigned short	Uncorrectables;					// Uncorrectable packet count (TEI bit set)
	unsigned short	sequenceNumber;					// Status message sequence number
	unsigned short	reserved[2];							// Reserved for later use
	BYTE			ProductID[16];						// Product Identification
	BYTE			DeviceType;
	BYTE			DSPFirmWare;					// Firmware Version Revision.
	BYTE			ReceiverMAC[6] ;				// Receiver MAC Address.
	BYTE			ReceiverIP[4];
    BYTE			SubnetMask[4];					// Receiver Subnet Mask
    BYTE			DefaultGatewayIP[4];			// Default Gateway Address
	BYTE			UnicastStatusIP[4];				// Destination IP Address for Unicast Status
	WORD			UnicastStatusPort;				// Destination UDP Port.	
	BYTE			CAMVersion;						// CAM controller firmware version
	BYTE			CAMRevision;					// CAM controller firmware revision
	unsigned short	SymbolRate;						// Symbol rate in KSPS
	unsigned short	Frequency;						// Frequency in 100 KHz
	unsigned short	LNBConfiguration;				// LNB control settings
	unsigned short	DVBSStatus;						// DVB-S ViterbiRate[15:8],LNBerror[2],SpectalInversion[1],Lock[0]
													// DVB-S2 ModCode[15:8],LNBerror[5],SpectalInversion[4],Length[3],Pilots[2],UWP&CSM&FEC[1],AGCLock[0]
	short		  	FrequencyOffset;				// Frequency offset				
	short		  	SignalLevel;					// RF signal level
	unsigned short	CNratio;						// Carrier to noise ratio
	unsigned short	VBER_or_PacketErrorRate;		// DVB-S Viterbi bit error rate
													// DVB-S2 CRC-8 error detected		
	unsigned short	BCHerrorRate;					// BCH error rate
	unsigned short	LDPCerrorRate;					// LDPC error rate
	unsigned short	DvbsSignature;					// DVB-S mode = 0x5555
	unsigned short	EthernetTX;						// Ethernet Transmitted Packets Counter
	unsigned short	EthernetDropped;				// Number of Ethernet Packets Dropped by the Ethernet chip
	unsigned short	EthernetRX;						// Number of Ethernet Packets Received
	unsigned short	EthernetRXErr;					// Count of Ethernet Packets Received In Error.
	unsigned short	DVBScrambled;					// DVB packets forwarded in a scrambled state
	unsigned short	DVBAccepted;					// Total Number of DVB Packets that passed the Filter.
	unsigned short	DVBDescrambled;					// DVB packets descrambled or not scrambled
	unsigned short	Sync_Loss;						// Packets Received with FEL high and No Sync Byte.	
	BYTE			Filter;							// IGMP On/Off flag[2]
	BYTE			CardStatus;						// CA Controller Status
	unsigned short	TotalDVBPacketsAccepted[4];		// Statistics counters
	unsigned short	TotalDVBPacketsRXInError[4];
	unsigned short	TotalEthernetPacketsOut[4];
	unsigned short	TotalUncorrectableTSPackets[4];
	unsigned short	BroadcastStatusPort;			// Broadcast status UDP port
	unsigned short	LocalOscFrequency;				// LNB Local Oscillator Frequency
} S200VStatusMSG;


typedef struct S200VDefaultConfigMSG
{

	unsigned short configVerificationCode[2];
	WORD Random;		
	BYTE  Signature[20];		// HMAC-SHA1

} S200VDefaultConfigMSG;


typedef struct S200VNetConfigMSG {

	BYTE  ReceiverIP[4];		// the IP address to be assumed by the device
    BYTE  SubnetMask[4];		// the Subnet Mask to be used by the device
    BYTE  DefaultGatewayIP[4];	// the default gateway to be used by the device
	BYTE  UnicastStatusIP[4];
	WORD  UnicastStatusPort;  
	WORD  BroadcastStatusPort;			// the status port 
	WORD  IGMP;                 // 0 = filter off, 1= IGMP Filter on
	WORD  Checksum; 
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1

} S200VNetConfigMSG;


typedef struct S200VRFConfigMSG {

	WORD  SymbolRate;              // in ksps
	WORD  Frequency;               // in units of 10 Hz
	BYTE  LNBConfig;               // bit0 - 0 = power off, 1 = power on
                                   // bit1 - 0 = vertical or right polarization
	                               //        1 = horizontal or left polarization
	                               // bit2 - 0 = low band, 1 = high band
								   // bit3 - Hi voltage switch
								   // bit4 - Long line compensation switch
								   // bit5 - Polarity Switching Voltage, 0 = 13/18, 1 = 11/15
								   // bit6 - Tone switching frequency, 0 = 22kHz, 1 = 44kHz
								   // bit7 - DVB Mode, 0 - DVB-S, 1 = DVB-S2
	BYTE  ViterbiRate;             // 0x08 = auto
	WORD  LocalOscFrequency;
	WORD  Random;				   // 
	BYTE  Signature[20];		   // HMAC-SHA1

} S200VRFConfigMSG;



typedef struct S200VResetMSG {

	BYTE Message[4];	 
	WORD  Random;				// 
	BYTE  Signature[20];		// HMAC-SHA1

} S200VResetMSG;


typedef struct S200VSetPasswordMSG {

	unsigned char newPassword[16];
	WORD Random;		
	BYTE Signature[20];				

} S200VSetPasswordMSG;


typedef struct S200VVProgramEntry {

	unsigned short programNumber;
	unsigned char  destinationIP[4];
	unsigned short destinationUDP;

} S200VVProgramEntry;



typedef struct S200VVProgramListMSG {

	WORD Opcode;
	unsigned short mode;
	unsigned char destinationIP[4];
	unsigned short destinationUDP;
	S200VVProgramEntry ProgramList[NUMBER_OF_VPROGRAMS];
	WORD Random;		
	BYTE Signature[20];				

} S200VVProgramListMSG;


typedef struct S200VGetPSIMSG
{

	unsigned short TableID;
	unsigned short SectionNumber;
	WORD Random;	  
	BYTE Signature[20];				

} S200VGetPSIMSG;


typedef struct S200VSITable {

	unsigned short tableID;
	unsigned short version;
	unsigned short current;
	union u_tagContext{
		unsigned short programNumber;
		unsigned short networkID;
	} context;
	unsigned short length;
	unsigned short sectionNumber;
	unsigned short lastSectionNumber;
	unsigned char sectionHeader[8];
	union u_tagSiTable{
		BYTE PATtable[1024];
		BYTE NITtable[1024];
		BYTE SDTtable[1024];
	} SiTable;

} S200VSITable;


typedef struct S200VGetProgramInfoMSG {

	WORD Opcode;
	WORD Random;	  
	BYTE Signature[20];				

} S200VGetProgramInfoMSG;


typedef struct S200VLanguageDescriptor {

	unsigned char languageCode[3];
	unsigned char audioType;

} S200VLanguageDescriptor;


typedef struct S200VProgramInfo {

	unsigned short programNumber;
	unsigned short programScramblingState;
	unsigned short CaSystemId;
	unsigned short PMTPID;
	unsigned short PMTversion;
	unsigned short streamType;
	unsigned short videoPID;
	unsigned short PCRPID;
	unsigned short audioPID[NUMBER_OF_AUDIO_PIDS_PER_PROGRAM];
	S200VLanguageDescriptor langList[NUMBER_OF_AUDIO_PIDS_PER_PROGRAM];

} S200VProgramInfo;


typedef struct S200VProgramInfoReplyMSG {

	S200VProgramInfo Info[NUMBER_OF_VPROGRAMS];

} S200VProgramInfoReplyMSG;


static pstruct m_sec;


///////////////////////////////////////////////////////////////////////////////
//
// ParseServiceDescriptor()
//
// Purpose:     Parse the service descriptor into strings.
//
//
// Parameters:  char *strA			- pointer to service provider name
//							char *strB			- pointer to service name
//							char *ptr				- pointer to the service descriptor
//
// Returns:     int		          - status of descriptor parsing
//							strA						- service provider name string
//							strB						- service name string
//
static int
ParseServiceDescriptor(char * strA, char * strB, unsigned char * ptrDesc)
{
	unsigned descriptor_tag;
	unsigned descriptor_length;
	unsigned service_type;
	unsigned service_provider_name_length;
	unsigned service_name_length;
	unsigned offset, i;
	char c;

		// fill in the service descriptor parameters
		descriptor_tag = ptrDesc[0];
		descriptor_length = ptrDesc[1];
		service_type = ptrDesc[2];
		service_provider_name_length = ptrDesc[3];
		service_name_length = ptrDesc[4 + service_provider_name_length];

		// check descriptor tag
		if (descriptor_tag != 0x48)
			return(false);

		// read provider name into string
		for ( i = 0; i < service_provider_name_length; i++)
		{			
			// get char
			c = ptrDesc[4 + i];
			// discard non ascii stuff
			if (isascii(c))
				*strA++ = c;
		}
			// terminate string
			*strA = 0;

		// buffer offset to string
		offset = service_provider_name_length + 5;
		// read service name into string
		for ( i = 0; i < service_name_length; i++)
		{
			c = ptrDesc[offset + i];
			// discard non ascii stuff
			if (isascii(c) && isprint(c))
				*strB++ = c;
		}
			// terminate string
			*strB = 0;

	// parse okay
	 return(true);
}



S200V::S200V()
{
	config_ports = new unsigned short[I_NUM_CONFIG_PORTS];

	SETUP_PORT_TABLE;

	currentPIDs = NULL;
	pendingPIDs = NULL;
	currentRawForwardFlags = NULL;
	pendingRawForwardFlags = NULL;
	currentDestinationMasks = NULL;
	pendingDestinationMasks = NULL;
	currentPIDDestinationIPs = NULL;
	pendingPIDDestinationIPs = NULL;
	currentPIDDestinationPorts = NULL;
	pendingPIDDestinationPorts = NULL;
	currentProgramNumbers = NULL;
	pendingProgramNumbers = NULL;
//	currentPMTPIDs = NULL;
//	pendingPMTPIDs = NULL;
	currentCAProgramNumbers = NULL;
	pendingCAProgramNumbers = NULL;
	CAProgramStatus = NULL;
	currentFKPIDs = NULL;
	pendingFKPIDs = NULL;
	currentFKSigns = NULL;
	pendingFKSigns = NULL;
	currentFKs = NULL;
	pendingFKs = NULL;
	currentOriginalIPs = NULL;
	pendingOriginalIPs = NULL;
	currentNewIPs = NULL;
	pendingNewIPs = NULL;
	currentIPRemapMasks = NULL;
	pendingIPRemapMasks = NULL;
	currentIPRemapTTLs = NULL;
	pendingIPRemapTTLs = NULL;
	currentIPRemapActions = NULL;
	pendingIPRemapActions = NULL;
	currentVProgramNumbers = NULL;
	pendingVProgramNumbers = NULL;
	programNames = NULL;
	guideProgramNumbers = NULL;
	guideProgramNames = NULL;
	currentVProgramDestinationIPs = NULL;
	pendingVProgramDestinationIPs = NULL;
	currentVProgramDestinationPorts = NULL;
	pendingVProgramDestinationPorts = NULL;
	CASystemIDs = NULL;
	CAProgramScrambledFlags = NULL;
	PMTPIDs = NULL;
	PMTVersions = NULL;
	ProgramStreamTypes = NULL;
	VideoPIDs = NULL;
	PCRPIDs = NULL;
	AudioPIDs = NULL;
	AudioPIDLanguages = NULL;
	AudioPIDTypes = NULL;
	initAttributes();
}


S200V::S200V( unsigned char *buffer )
{
	config_ports = new unsigned short[I_NUM_CONFIG_PORTS];

	SETUP_PORT_TABLE;

	currentPIDs = NULL;
	pendingPIDs = NULL;
	currentRawForwardFlags = NULL;
	pendingRawForwardFlags = NULL;
	currentDestinationMasks = NULL;
	pendingDestinationMasks = NULL;
	currentPIDDestinationIPs = NULL;
	pendingPIDDestinationIPs = NULL;
	currentPIDDestinationPorts = NULL;
	pendingPIDDestinationPorts = NULL;
	currentProgramNumbers = NULL;
	pendingProgramNumbers = NULL;
//	currentPMTPIDs = NULL;
//	pendingPMTPIDs = NULL;
//	initAttributes();
	currentCAProgramNumbers = NULL;
	pendingCAProgramNumbers = NULL;
	CAProgramStatus = NULL;
	currentFKPIDs = NULL;
	pendingFKPIDs = NULL;
	currentFKSigns = NULL;
	pendingFKSigns = NULL;
	currentFKs = NULL;
	pendingFKs = NULL;
	currentOriginalIPs = NULL;
	pendingOriginalIPs = NULL;
	currentNewIPs = NULL;
	pendingNewIPs = NULL;
	currentIPRemapMasks = NULL;
	pendingIPRemapMasks = NULL;
	currentIPRemapTTLs = NULL;
	pendingIPRemapTTLs = NULL;
	currentIPRemapActions = NULL;
	pendingIPRemapActions = NULL;
	currentVProgramNumbers = NULL;
	pendingVProgramNumbers = NULL;
	programNames = NULL;
	guideProgramNumbers = NULL;
	guideProgramNames = NULL;
	currentVProgramDestinationIPs = NULL;
	pendingVProgramDestinationIPs = NULL;
	currentVProgramDestinationPorts = NULL;
	pendingVProgramDestinationPorts = NULL;
	CASystemIDs = NULL;
	CAProgramScrambledFlags = NULL;
	PMTPIDs = NULL;
	PMTVersions = NULL;
	ProgramStreamTypes = NULL;
	VideoPIDs = NULL;
	PCRPIDs = NULL;
	AudioPIDs = NULL;
	AudioPIDLanguages = NULL;
	AudioPIDTypes = NULL;

	initAttributes();
	setFixedParameters( buffer );
	createIndexedParameters();
}



S200V::~S200V()
{
	int i;

	if ( currentPIDs != NULL ) {
		delete [] currentPIDs;
	}
	if ( pendingPIDs != NULL ) {
		delete [] pendingPIDs;
	}
	if ( currentRawForwardFlags != NULL ) {
		delete [] currentRawForwardFlags;
	}
	if ( pendingRawForwardFlags != NULL ) {
		delete [] pendingRawForwardFlags;
	}
	if ( currentDestinationMasks != NULL ) {
		delete [] currentDestinationMasks;
	}
	if ( pendingDestinationMasks != NULL ) {
		delete [] pendingDestinationMasks;
	}
	if ( currentPIDDestinationIPs != NULL ) {
		delete [] currentPIDDestinationIPs;
	}
	if ( pendingPIDDestinationIPs != NULL ) {
		delete [] pendingPIDDestinationIPs;
	}
	if ( currentPIDDestinationPorts != NULL ) {
		delete [] currentPIDDestinationPorts;
	}
	if ( pendingPIDDestinationPorts != NULL ) {
		delete [] pendingPIDDestinationPorts;
	}
	if ( currentProgramNumbers != NULL ) {
		delete [] currentProgramNumbers;
	}
	if ( pendingProgramNumbers != NULL ) {
		delete [] pendingProgramNumbers;
	}
//	if ( currentPMTPIDs != NULL ) {
//		delete [] currentPMTPIDs;
//	}
//	if ( pendingPMTPIDs != NULL ) {
//		delete [] pendingPMTPIDs;
//	}
	if ( currentCAProgramNumbers != NULL ) {
		delete [] currentCAProgramNumbers;
	}
	if ( pendingCAProgramNumbers != NULL ) {
		delete [] pendingCAProgramNumbers;
	}
	if ( CAProgramStatus != NULL ) {
		delete [] CAProgramStatus;
	}
	if ( currentFKPIDs != NULL ) {
		delete [] currentFKPIDs;
	}
	if ( pendingFKPIDs != NULL ) {
		delete [] pendingFKPIDs;
	}
	if ( currentFKSigns != NULL ) {
		delete [] currentFKSigns;
	}
	if ( pendingFKSigns != NULL ) {
		delete [] pendingFKSigns;
	}
	if ( currentFKs != NULL ) {
		delete [] currentFKs;
	}
	if ( pendingFKs != NULL ) {
		delete [] pendingFKs;
	}
	if ( currentOriginalIPs != NULL ) {
		delete [] currentOriginalIPs;
	}
	if ( pendingOriginalIPs != NULL ) {
		delete [] pendingOriginalIPs;
	}
	if ( currentNewIPs != NULL ) {
		delete [] currentNewIPs;
	}
	if ( pendingNewIPs != NULL ) {
		delete [] pendingNewIPs;
	}
	if ( currentIPRemapMasks != NULL ) {
		delete [] currentIPRemapMasks;
	}
	if ( pendingIPRemapMasks != NULL ) {
		delete [] pendingIPRemapMasks;
	}
	if ( currentIPRemapTTLs != NULL ) {
		delete [] currentIPRemapTTLs;
	}
	if ( pendingIPRemapTTLs != NULL ) {
		delete [] pendingIPRemapTTLs;
	}
	if ( currentIPRemapActions != NULL ) {
		delete [] currentIPRemapActions;
	}
	if ( pendingIPRemapActions != NULL ) {
		delete [] pendingIPRemapActions;
	}
	if ( currentVProgramNumbers!= NULL ) {
		delete [] currentVProgramNumbers;
	}
	if ( pendingVProgramNumbers!= NULL ) {
		delete [] pendingVProgramNumbers;
	}
	if ( programNames!= NULL ) {
		delete [] programNames;
	}
	if ( guideProgramNumbers!= NULL ) {
		delete [] guideProgramNumbers;
	}
	if ( guideProgramNames!= NULL ) {
		delete [] guideProgramNames;
	}
	if ( currentVProgramDestinationIPs!= NULL ) {
		delete [] currentVProgramDestinationIPs;
	}
	if ( pendingVProgramDestinationIPs!= NULL ) {
		delete [] pendingVProgramDestinationIPs;
	}
	if ( currentVProgramDestinationPorts!= NULL ) {
		delete [] currentVProgramDestinationPorts;
	}
	if ( pendingVProgramDestinationPorts!= NULL ) {
		delete [] pendingVProgramDestinationPorts;
	}
	if ( CASystemIDs != NULL ) {
		delete [] CASystemIDs;
	}
	if ( CAProgramScrambledFlags != NULL ) {
		delete [] CAProgramScrambledFlags;
	}
	if ( PMTPIDs != NULL ) {
		delete [] PMTPIDs;
	}
	if ( PMTVersions != NULL ) {
		delete [] PMTVersions;
	}
	if ( ProgramStreamTypes != NULL ) {
		delete [] ProgramStreamTypes;
	}
	if ( VideoPIDs != NULL ) {
		delete [] VideoPIDs;
	}
	if ( PCRPIDs != NULL ) {
		delete [] PCRPIDs;
	}
	if ( AudioPIDs != NULL ) {
		for ( i = 0; i < NUMBER_OF_VPROGRAMS; i++ ) {
			delete [] AudioPIDs[i];
		}
		delete [] AudioPIDs;
	}
	if ( AudioPIDLanguages != NULL ) {
		for ( i = 0; i < NUMBER_OF_VPROGRAMS; i++ ) {
			delete [] AudioPIDLanguages[i];
		}
		delete [] AudioPIDLanguages;
	}
	if ( AudioPIDTypes != NULL ) {
		for ( i = 0; i < NUMBER_OF_VPROGRAMS; i++ ) {
			delete [] AudioPIDTypes[i];
		}
		delete [] AudioPIDTypes;
	}

	delete [] config_ports;

}



void S200V::initAttributes()
{
	memset(	parameterMask, 0, MAX_RECEIVER_PARAM * sizeof( unsigned short ) );


	// FIXED READ ONLY VALUES
	//---------------------------------------
	
	// DEVICE INFO
	
	parameterMask[ DEVICE_TYPE						] = F_EXISTS | F_ID | F_DEF | F_READ | F_SHORT | F_ENUM;
	parameterMask[ DSP_VERSION						] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ DSP_REVISION						] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ FPGA_VERSION						] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ FPGA_REVISION					] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ RECEIVER_MAC						] = F_EXISTS | F_ID | F_READ | F_STRING;
	parameterMask[ SIZE_VPROGRAM_LIST				] = F_EXISTS | F_DEF | F_READ | F_SHORT;
//	parameterMask[ SIZE_PID_LIST					] = F_EXISTS | F_READ | F_SHORT;
//	parameterMask[ SIZE_IP_REMAP_LIST				] = F_EXISTS | F_READ | F_SHORT;
//	parameterMask[ SIZE_DESTINATION_LIST			] = F_EXISTS | F_READ | F_SHORT;
//	parameterMask[ SIZE_MINI_PAT_LIST				] = F_EXISTS | F_READ | F_SHORT;
	parameterMask[ CODE_DOWNLOAD					] = F_EXISTS | F_DEF | F_READ | F_FLAG;
	parameterMask[ FACTORY_RESET					] = F_EXISTS | F_DEF | F_READ | F_FLAG;
	parameterMask[ SIZE_PROGRAM_GUIDE				] = F_EXISTS | F_DEF | F_READ | F_SHORT;
	parameterMask[ SIZE_AUDIO_PID_LIST				] = F_EXISTS | F_DEF | F_READ | F_SHORT;

	// DYNAMIC READ ONLY VALUES
	//----------------------------------------

	parameterMask[ STATUS_TIMESTAMP					] = F_EXISTS | F_STATUS | F_READ | F_STRING;

	// SIGNAL

	parameterMask[ DATA_SYNC_LOSS					] = F_EXISTS | F_STATUS | F_READ | F_SHORT;

	// RF

	parameterMask[ VITERBI_RATE						] = F_EXISTS | F_STATUS | F_READ | F_STRING | F_ENUM;
	parameterMask[ MODCOD							] = F_EXISTS | F_STATUS | F_READ | F_STRING | F_ENUM;
	parameterMask[ SIGNAL_STRENGTH_AS_DBM			] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ SIGNAL_STRENGTH_AS_PERCENTAGE	] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ SIGNAL_LOCK						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ DATA_LOCK						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ LNB_FAULT						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ VBER								] = F_EXISTS | F_STATUS | F_READ | F_FLOAT;
	parameterMask[ PER								] = F_EXISTS | F_STATUS | F_READ | F_FLOAT;
	parameterMask[ UNCORRECTABLES					] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ CARRIER_TO_NOISE					] = F_EXISTS | F_STATUS | F_READ | F_FLOAT;
	parameterMask[ CARRIER_FREQUENCY				] = F_EXISTS | F_STATUS | F_READ | F_STRING;
	parameterMask[ FREQUENCY_OFFSET					] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ SPECTRAL_INVERSION_FLAG			] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ PILOT_SYMBOL_FLAG				] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ FRAME_LENGTH						] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;
	parameterMask[ DVB_SIGNAL_TYPE					] = F_EXISTS | F_STATUS | F_READ | F_FLAG | F_ENUM;


	// CONTENT

	// DVB
	
	parameterMask[ DVB_ACCEPTED						] = F_EXISTS | F_STATUS | F_READ | F_SHORT;
	parameterMask[ TOTAL_DVB_PACKETS_ACCEPTED		] = F_EXISTS | F_STATUS | F_READ | F_I64;
	parameterMask[ TOTAL_UNCORRECTABLE_TS_PACKETS	] = F_EXISTS | F_STATUS | F_READ | F_I64;
	parameterMask[ DVB_SCRAMBLED					] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ DVB_CLEAR						] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ PROGRAM_GUIDE_PROGRAM_NUMBER		] = F_EXISTS | F_READ | F_SHORT | F_INDEXED;
	parameterMask[ PROGRAM_GUIDE_PROGRAM_NAME		] = F_EXISTS | F_READ | F_STRING | F_INDEXED;
	parameterMask[ CA_PROGRAM_SCRAMBLED_FLAG		] = F_EXISTS | F_READ | F_FLAG | F_ENUM | F_INDEXED;
	parameterMask[ CA_SYSTEM_ID						] = F_EXISTS | F_READ | F_SHORT | F_ENUM | F_INDEXED;
	parameterMask[ PMT_PID							] = F_EXISTS | F_READ | F_SHORT | F_INDEXED;
	parameterMask[ PMT_VERSION						] = F_EXISTS | F_READ | F_SHORT | F_INDEXED;
	parameterMask[ PROGRAM_STREAM_TYPE				] = F_EXISTS | F_READ | F_SHORT | F_ENUM | F_INDEXED;
	parameterMask[ DATA_PID							] = F_EXISTS | F_READ | F_SHORT | F_INDEXED;
	parameterMask[ VIDEO_PID						] = F_EXISTS | F_READ | F_SHORT | F_INDEXED;
	parameterMask[ PCR_PID							] = F_EXISTS | F_READ | F_SHORT | F_INDEXED;
	parameterMask[ AUDIO_PID						] = F_EXISTS | F_READ | F_SHORT | F_INDEXED;
	parameterMask[ AUDIO_PID_LANGUAGE				] = F_EXISTS | F_READ | F_LONG | F_INDEXED;
	parameterMask[ AUDIO_TYPE						] = F_EXISTS | F_READ | F_SHORT | F_INDEXED;


	// ETHERNET
		
	parameterMask[ ETHERNET_TRANSMIT				] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ ETHERNET_RECEIVE					] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ ETHERNET_PACKET_DROPPED			] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ ETHERNET_RECEIVE_ERROR			] = F_EXISTS | F_STATUS | F_READ | F_LONG;
	parameterMask[ TOTAL_ETHERNET_PACKETS_OUT		] = F_EXISTS | F_STATUS | F_READ | F_I64;


	// READ/WRITE SECTION
	//-----------------------------------

	// Network Parameters

	parameterMask[ RECEIVER_IP						] = F_EXISTS | F_ID | F_CONFIG | F_READ | F_WRITE | F_STRING;
	parameterMask[ SUBNET_MASK						] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_STRING;
	parameterMask[ DEFAULT_GATEWAY_IP				] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_STRING;
	parameterMask[ UNICAST_STATUS_IP				] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_STRING;
	parameterMask[ UNICAST_STATUS_PORT				] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT;
	parameterMask[ BROADCAST_STATUS_PORT			] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT;
	parameterMask[ IGMP_ENABLE						] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM; 

	// RF

	parameterMask[ LBAND_FREQUENCY					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLOAT;
	parameterMask[ LO_FREQUENCY						] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLOAT;
	parameterMask[ SYMBOL_RATE						] = F_EXISTS | F_CONFIG | F_STATUS | F_READ | F_WRITE | F_FLOAT;
	parameterMask[ LNB_POWER						] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ BAND								] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ POLARITY							] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ HI_VOLTAGE_SWITCH				] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ LONG_LINE_COMPENSATION_SWITCH	] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ POLARITY_SWITCHING_VOLTAGE		] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ BAND_SWITCHING_TONE_FREQUENCY	] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ DVB_SIGNAL_TYPE_CONTROL			] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_ENUM;


	// CONTENT

//	parameterMask[ PID								] = F_EXISTS | F_READ | F_WRITE | F_SHORT;
//	parameterMask[ RAW_FORWARD_FLAG					] = F_EXISTS | F_READ | F_WRITE | F_SHORT;
//	parameterMask[ DESTINATION_MASK					] = F_EXISTS | F_READ | F_WRITE | F_SHORT;
//	parameterMask[ PID_DESTINATION_IP				] = F_EXISTS | F_READ | F_WRITE | F_SHORT;
//	parameterMask[ PID_DESTINATION_PORT				] = F_EXISTS | F_READ | F_WRITE | F_SHORT;
	parameterMask[ STREAM_DESTINATION_IP			] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT;
	parameterMask[ STREAM_DESTINATION_PORT			] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT;
//	parameterMask[ PROGRAM_NUMBER					] = F_EXISTS | F_READ | F_WRITE | F_SHORT;
//	parameterMask[ PMT_PID							] = F_EXISTS | F_READ | F_WRITE | F_SHORT;
	parameterMask[ FORWARD_ALL_FLAG					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
	parameterMask[ FORWARD_NULLS_FLAG				] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_FLAG | F_ENUM;
//	parameterMask[ IP_REMAP_ENABLE					] = F_EXISTS | F_READ | F_WRITE | F_FLAG;
//	parameterMask[ ORIGINAL_IP						] = F_EXISTS | F_READ | F_WRITE | F_STRING;
//	parameterMask[ NEW_IP							] = F_EXISTS | F_READ | F_WRITE | F_STRING;
//	parameterMask[ IP_REMAP_MASK					] = F_EXISTS | F_READ | F_WRITE | F_STRING;
//	parameterMask[ IP_REMAP_TTL						] = F_EXISTS | F_READ | F_WRITE | F_SHORT;
//	parameterMask[ IP_REMAP_ACTION					] = F_EXISTS | F_READ | F_WRITE | F_STRING;
	parameterMask[ VPROGRAM_NUMBER					] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_INDEXED;
	parameterMask[ PROGRAM_NAME						] = F_EXISTS | F_CONFIG | F_READ | F_STRING | F_INDEXED;
	parameterMask[ VPROGRAM_DESTINATION_IP			] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_INDEXED;
	parameterMask[ VPROGRAM_DESTINATION_PORT		] = F_EXISTS | F_CONFIG | F_READ | F_WRITE | F_SHORT | F_INDEXED;


	// WRITE ONLY SECTION

	parameterMask[ PASSWORD							] = F_EXISTS | F_WRITE | F_STRING;


	Receiver::initAttributes();
}



int S200V::numConfigPorts()
{
	return I_NUM_CONFIG_PORTS;
}


unsigned short S200V::configPort( int i )
{
	if ( i < numConfigPorts() ) {
		return config_ports[i];
	} else {
		return 0;
	}
}





bool S200V::checkStatusPacket( string ip, unsigned char *buffer )
{
	bool isForThisReceiver = true;


	try {
		if ( ip != getParameter( RECEIVER_IP ).asString() ) {
			if ( ip != getParameter( RECEIVER_IP, PENDING ).asString() ) throw 1;
		}
		if ( buffer[32] != typeIDCode() ) throw 1;
//		if ( buffer[32] != ID_S200Base ) throw 1;
		if ( fixedParamFromStatus( RECEIVER_MAC, buffer ) != getParameter( RECEIVER_MAC ).asString() ) throw 1;
	}

	catch ( int ) {
		isForThisReceiver = false;
	}

	return isForThisReceiver;
}


bool S200V::setFixedParameters( unsigned char *buffer )
{
	char temp[20];

	if ( buffer[32] == typeIDCode() ) {
//	if ( buffer[32] == ID_S200Base ) {

		currentParameters[DEVICE_TYPE]->setM_value( string( itoa( receiverTypeCode(), temp, 10 ) ) );
//		currentParameters[DEVICE_TYPE]->setM_value( string( itoa( R_S200Base, temp, 10 ) ) );

		currentParameters[RECEIVER_MAC]->setM_value( fixedParamFromStatus( RECEIVER_MAC, buffer ) );

		currentParameters[DSP_VERSION]->setM_value( fixedParamFromStatus( DSP_VERSION, buffer ) );

		currentParameters[DSP_REVISION]->setM_value( fixedParamFromStatus( DSP_REVISION, buffer ) );

		currentParameters[FPGA_VERSION]->setM_value( fixedParamFromStatus( FPGA_VERSION, buffer ) );

		currentParameters[FPGA_REVISION]->setM_value( fixedParamFromStatus( FPGA_REVISION, buffer ) );

		if ( hasParameter( CAM_VERSION ) ) {

			currentParameters[CAM_VERSION]->setM_value( fixedParamFromStatus( CAM_VERSION, buffer ) );
			currentParameters[CAM_REVISION]->setM_value( fixedParamFromStatus( CAM_REVISION, buffer ) );

		}

		if ( hasParameter( SIZE_PID_LIST ) ) {
			currentParameters[SIZE_PID_LIST]->setM_value( string( itoa( NUMBER_OF_PIDS ,temp, 10 ) ) );
		}

		if ( hasParameter( SIZE_IP_REMAP_LIST ) ) {
			currentParameters[SIZE_IP_REMAP_LIST]->setM_value( string( itoa( NUMBER_OF_IP_REMAP_RULES, temp, 10 ) ) );
		}

		if ( hasParameter( SIZE_CA_PROGRAM_LIST ) ) {
			currentParameters[SIZE_CA_PROGRAM_LIST]->setM_value( string( itoa( NUMBER_OF_PROGRAMS ,temp, 10 ) ) );
		}

		if ( hasParameter( SIZE_DESTINATION_LIST ) ) {
			currentParameters[SIZE_DESTINATION_LIST]->setM_value( string( itoa( NUMBER_OF_DESTINATIONS ,temp, 10 ) ) );
		}

		if ( hasParameter( SIZE_MINI_PAT_LIST ) ) {
			currentParameters[SIZE_MINI_PAT_LIST]->setM_value( string( itoa( NUMBER_OF_PATS ,temp, 10 ) ) );
		}

		if ( hasParameter( SIZE_FIXED_KEY_LIST ) ) {
			currentParameters[SIZE_FIXED_KEY_LIST]->setM_value( string( itoa( NUMBER_OF_FIXED_KEYS ,temp, 10 ) ) );
		}

		if ( hasParameter( SIZE_VPROGRAM_LIST ) ) {
			currentParameters[SIZE_VPROGRAM_LIST]->setM_value( string( itoa( NUMBER_OF_VPROGRAMS ,temp, 10 ) ) );
		}

		if ( hasParameter( SIZE_PROGRAM_GUIDE ) ) {
			currentParameters[SIZE_PROGRAM_GUIDE]->setM_value( string( itoa( NUMBER_OF_PROGRAM_GUIDE_ENTRIES ,temp, 10 ) ) );
		}

		if ( hasParameter( SIZE_AUDIO_PID_LIST ) ) {
			currentParameters[SIZE_AUDIO_PID_LIST]->setM_value( string( itoa( NUMBER_OF_AUDIO_PIDS_PER_PROGRAM ,temp, 10 ) ) );
		}

		return true;

	} else {

		return false;

	}
}


bool S200V::createIndexedParameters()
{
	int i;

	currentPIDs = new ParameterValue[NUMBER_OF_PIDS];
	pendingPIDs = new ParameterValue[NUMBER_OF_PIDS];
//	currentRawForwardFlags = new ParameterValue[NUMBER_OF_PIDS];
//	pendingRawForwardFlags = new ParameterValue[NUMBER_OF_PIDS];
	currentRawForwardFlags = new EnumeratedReceiverParameter[NUMBER_OF_PIDS];
	for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {
		currentRawForwardFlags[i].setEnumType( RAW_FORWARD_FLAG );
	}
	pendingRawForwardFlags = new EnumeratedReceiverParameter[NUMBER_OF_PIDS];
	for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {
		pendingRawForwardFlags[i].setEnumType( RAW_FORWARD_FLAG );
	}
	currentDestinationMasks = new ParameterValue[NUMBER_OF_PIDS];
	pendingDestinationMasks = new ParameterValue[NUMBER_OF_PIDS];
	currentPIDDestinationIPs = new ParameterValue[NUMBER_OF_DESTINATIONS];
	pendingPIDDestinationIPs = new ParameterValue[NUMBER_OF_DESTINATIONS];
	currentPIDDestinationPorts = new ParameterValue[NUMBER_OF_DESTINATIONS];
	pendingPIDDestinationPorts = new ParameterValue[NUMBER_OF_DESTINATIONS];
	currentProgramNumbers = new ParameterValue[NUMBER_OF_PATS];
	pendingProgramNumbers = new ParameterValue[NUMBER_OF_PATS];
//	currentPMTPIDs = new ParameterValue[NUMBER_OF_PATS];
//	pendingPMTPIDs = new ParameterValue[NUMBER_OF_PATS];
	currentCAProgramNumbers = new ParameterValue[NUMBER_OF_PROGRAMS];
	pendingCAProgramNumbers = new ParameterValue[NUMBER_OF_PROGRAMS];
	currentFKPIDs = new ParameterValue[NUMBER_OF_FIXED_KEYS];
	pendingFKPIDs = new ParameterValue[NUMBER_OF_FIXED_KEYS];
//	currentFKSigns = new ParameterValue[NUMBER_OF_FIXED_KEYS];
//	pendingFKSigns = new ParameterValue[NUMBER_OF_FIXED_KEYS];
	currentFKSigns = new EnumeratedReceiverParameter[NUMBER_OF_FIXED_KEYS];
	for ( i = 0; i < NUMBER_OF_FIXED_KEYS; i++ ) {
		currentFKSigns[i].setEnumType( FIXED_KEY_SIGN );
	}
	pendingFKSigns = new EnumeratedReceiverParameter[NUMBER_OF_FIXED_KEYS];
	for ( i = 0; i < NUMBER_OF_FIXED_KEYS; i++ ) {
		pendingFKSigns[i].setEnumType( FIXED_KEY_SIGN );
	}
	currentFKs = new ParameterValue[NUMBER_OF_FIXED_KEYS];
	pendingFKs = new ParameterValue[NUMBER_OF_FIXED_KEYS];
	currentOriginalIPs = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
	pendingOriginalIPs = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
	currentNewIPs = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
	pendingNewIPs = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
	currentIPRemapMasks = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
	pendingIPRemapMasks = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
	currentIPRemapTTLs = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
	pendingIPRemapTTLs = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
//	currentIPRemapActions = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
//	pendingIPRemapActions = new ParameterValue[NUMBER_OF_IP_REMAP_RULES];
	currentIPRemapActions = new EnumeratedReceiverParameter[NUMBER_OF_IP_REMAP_RULES];
	for ( i = 0; i < NUMBER_OF_IP_REMAP_RULES; i++ ) {
		currentIPRemapActions[i].setEnumType( IP_REMAP_ACTION );
	}
	pendingIPRemapActions = new EnumeratedReceiverParameter[NUMBER_OF_IP_REMAP_RULES];
	for ( i = 0; i < NUMBER_OF_IP_REMAP_RULES; i++ ) {
		pendingIPRemapActions[i].setEnumType( IP_REMAP_ACTION );
	}
	currentVProgramNumbers = new ParameterValue[NUMBER_OF_VPROGRAMS];
	pendingVProgramNumbers = new ParameterValue[NUMBER_OF_VPROGRAMS];
	programNames = new ParameterValue[NUMBER_OF_VPROGRAMS];
	guideProgramNumbers = new ParameterValue[NUMBER_OF_PROGRAM_GUIDE_ENTRIES];
	guideProgramNames = new ParameterValue[NUMBER_OF_PROGRAM_GUIDE_ENTRIES];
	currentVProgramDestinationIPs = new ParameterValue[NUMBER_OF_VPROGRAMS];
	pendingVProgramDestinationIPs = new ParameterValue[NUMBER_OF_VPROGRAMS];
	currentVProgramDestinationPorts = new ParameterValue[NUMBER_OF_VPROGRAMS];
	pendingVProgramDestinationPorts = new ParameterValue[NUMBER_OF_VPROGRAMS];
//	CAProgramScrambledFlags = new ParameterValue[NUMBER_OF_VPROGRAMS];
	CAProgramScrambledFlags = new EnumeratedReceiverParameter[NUMBER_OF_VPROGRAMS];
	for ( i = 0; i < NUMBER_OF_VPROGRAMS; i++ ) {
		CAProgramScrambledFlags[i].setEnumType( CA_PROGRAM_SCRAMBLED_FLAG );
	}
//	CASystemIDs = new ParameterValue[NUMBER_OF_VPROGRAMS];
	CASystemIDs = new EnumeratedReceiverParameter[NUMBER_OF_VPROGRAMS];
	for ( i = 0; i < NUMBER_OF_VPROGRAMS; i++ ) {
		CASystemIDs[i].setEnumType( CA_SYSTEM_ID );
	}
	PMTPIDs = new ParameterValue[NUMBER_OF_VPROGRAMS];
	PMTVersions = new ParameterValue[NUMBER_OF_VPROGRAMS];
//	ProgramStreamTypes = new ParameterValue[NUMBER_OF_VPROGRAMS];
	ProgramStreamTypes = new EnumeratedReceiverParameter[NUMBER_OF_VPROGRAMS];
	for ( i = 0; i < NUMBER_OF_VPROGRAMS; i++ ) {
		ProgramStreamTypes[i].setEnumType( PROGRAM_STREAM_TYPE );
	}
	VideoPIDs = new ParameterValue[NUMBER_OF_VPROGRAMS];
	PCRPIDs = new ParameterValue[NUMBER_OF_VPROGRAMS];
//	CAProgramStatus = new ParameterValue[NUMBER_OF_VPROGRAMS];
	CAProgramStatus = new EnumeratedReceiverParameter[NUMBER_OF_VPROGRAMS];
	for ( i = 0; i < NUMBER_OF_VPROGRAMS; i++ ) {
		CAProgramStatus[i].setEnumType( CA_PROGRAM_STATUS );
	}

	AudioPIDs = new ParameterValuePtr[NUMBER_OF_VPROGRAMS];
	AudioPIDLanguages = new ParameterValuePtr[NUMBER_OF_VPROGRAMS];
	AudioPIDTypes = new ParameterValuePtr[NUMBER_OF_VPROGRAMS];

	for ( i = 0; i < NUMBER_OF_VPROGRAMS; i++ ) {

		AudioPIDs[i] = new ParameterValue[SIZE_AUDIO_PID_LIST];
		AudioPIDLanguages[i] = new ParameterValue[SIZE_AUDIO_PID_LIST];
		AudioPIDTypes[i] = new ParameterValue[SIZE_AUDIO_PID_LIST];

	}

	return true;
}


string S200V::fixedParamFromStatus( ReceiverParameter p, unsigned char *buffer )
{
	char temp[100];
	string return_str = "";
	S200VStatusMSG *status = (S200VStatusMSG *)buffer;


	switch( p ) {

	case RECEIVER_MAC			:	sprintf(	temp, "%02x-%02x-%02x-%02x-%02x-%02x", 
												status->ReceiverMAC[0], status->ReceiverMAC[1],
												status->ReceiverMAC[2],	status->ReceiverMAC[3],
												status->ReceiverMAC[4], status->ReceiverMAC[5]  );
									return_str = string( temp );
									break;

	case DSP_VERSION			:	return_str = string ( itoa( ( status->DSPFirmWare & 0xf0 ) >> 4, temp, 10 ) );
									break;

	case DSP_REVISION			:	return_str = string ( itoa( status->DSPFirmWare & 0xf, temp, 10 ) );
									break;

	case FPGA_VERSION			:	return_str = string ( itoa( status->FPGAVersion, temp, 10 ) );
									break;

	case FPGA_REVISION			:	return_str = string ( itoa( status->FPGARevision, temp, 10 ) );
									break;

	case CAM_VERSION			:	return_str = string ( itoa( status->CAMVersion, temp, 10 ) );
									break;

	case CAM_REVISION			:	return_str = string ( itoa( status->CAMRevision >> 4, temp, 10 ) );
									break;

	default						:	return_str = "";
									break;

	}

	return return_str;
}


string S200V::dynamicParamFromStatus( ReceiverParameter p, unsigned char *buffer, int index )
{
	S200VStatusMSG *status = (S200VStatusMSG *)buffer;
	char temp[100];
	string return_str = "";
	bool signal_lock;
	short ss_dbm;
	double ss_per;
	int ss_n;
#ifdef WINDOWS
	unsigned __int64 big_number = 0;
#else
	unsigned long long big_number = 0;
#endif



	switch( p ) {

	// DSP Version and revision can change if code download is used

	case DSP_VERSION					:	return_str = string ( itoa( ( status->DSPFirmWare & 0xf0 ) >> 4, temp, 10 ) );
											break;

	case DSP_REVISION					:	return_str = string ( itoa( status->DSPFirmWare & 0xf, temp, 10 ) );
											break;


	case DATA_SYNC_LOSS					:	return_str = string ( itoa( ntohs( status->Sync_Loss ), temp, 10 ) );									
											break;

	case VITERBI_RATE					:	switch ( ( ntohs( status->DVBSStatus ) & 0xff00 ) >> 8 ) {
												case 0x0d	:		return_str = string( itoa( HALF, temp, 10 ) );
																	break;
												case 0x12	:		return_str = string( itoa( TWO_THIRDS, temp, 10 ) );
																	break;
												case 0x15	:		return_str = string( itoa( THREE_QUARTERS, temp, 10 ) );
																	break;
												case 0x18	:		return_str = string( itoa( FIVE_SIXTHS, temp, 10 ) );
																	break;
												case 0x19	:		return_str = string( itoa( SIX_SEVENTHS, temp, 10 ) );
																	break;
												case 0x1a	:		return_str = string( itoa( SEVEN_EIGTHS, temp, 10 ) );
																	break;
												default		:		return_str = string( itoa( VITERBI_ERROR, temp, 10 ) );
																	break;
											};
											break;

	case MODCOD							:	switch ( ( ntohs( status->DVBSStatus ) & 0xff00 ) >> 8 ) {
												case 1	:	return_str = string( itoa( QUARTER_QPSK, temp, 10 ) );
															break;
												case 2	:	return_str = string( itoa( THIRD_QPSK, temp, 10 ) );
															break;
												case 3	:	return_str = string( itoa( TWO_FIFTHS_QPSK, temp, 10 ) );
															break;
												case 4	:	return_str = string( itoa( HALF_QPSK, temp, 10 ) );
															break;
												case 5	:	return_str = string( itoa( THREE_FIFTHS_QPSK, temp, 10 ) );
															break;
												case 6	:	return_str = string( itoa( TWO_THIRDS_QPSK, temp, 10 ) );
															break;
												case 7	:	return_str = string( itoa( THREE_QUARTERS_QPSK, temp, 10 ) );
															break;
												case 8	:	return_str = string( itoa( FOUR_FIFTHS_QPSK, temp, 10 ) );
															break;
												case 9	:	return_str = string( itoa( FIVE_SIXTHS_QPSK, temp, 10 ) );
															break;
												case 10	:	return_str = string( itoa( EIGHT_NINTHS_QPSK, temp, 10 ) );
															break;
												case 11	:	return_str = string( itoa( NINE_TENTHS_QPSK, temp, 10 ) );
															break;
												case 12	:	return_str = string( itoa( THREE_FIFTHS_8PSK, temp, 10 ) );
															break;
												case 13	:	return_str = string( itoa( TWO_THIRDS_8PSK, temp, 10 ) );
															break;
												case 14	:	return_str = string( itoa( THREE_QUARTERS_8PSK, temp, 10 ) );
															break;
												case 15	:	return_str = string( itoa( FIVE_SIXTHS_8PSK, temp, 10 ) );
															break;
												case 16	:	return_str = string( itoa( EIGHT_NINTHS_8PSK, temp, 10 ) );
															break;
												case 17	:	return_str = string( itoa( NINE_TENTHS_8PSK, temp, 10 ) );
															break;
												case 30	:	return_str = string( itoa( THIRD_BPSK, temp, 10 ) );
															break;
												case 31	:	return_str = string( itoa( QUARTER_BPSK, temp, 10 ) );
															break;
												default	:	return_str = string( itoa( MODCOD_UNKNOWN, temp, 10 ) );
															break;
											};
											break;

	case SPECTRAL_INVERSION_FLAG		:	if ( status->DvbsSignature == 0x5555 ) {
												return_str = string ( itoa( ( ntohs( status->DVBSStatus ) & 0x02 ) >> 1, temp, 10 ) );
											} else { 
												return_str = string ( itoa( ( ntohs( status->DVBSStatus ) & 0x10 ) >> 4, temp, 10 ) );
											}
											break;

	case PILOT_SYMBOL_FLAG				:	return_str = string ( itoa( ( ntohs( status->DVBSStatus ) & 0x04 ) >> 2, temp, 10 ) );
											break;
		
	case FRAME_LENGTH					:	return_str = string ( itoa( ( ntohs( status->DVBSStatus ) & 0x08 ) >> 3, temp, 10 ) );
											break;

	case SIGNAL_STRENGTH_AS_DBM			:	
											if ( status->DvbsSignature == 0x5555 ) {
												signal_lock = ntohs( status->DVBSStatus ) & 0x01;
											} else { 
												signal_lock = ( ntohs( status->DVBSStatus ) & 0x03 ) == 0x03;
											}
											if ( !signal_lock ) {
												ss_n = -100;
											} else { 
												ss_n = (signed short)(ntohs( status->SignalLevel ));
//												if ( ss_n < -70 ) ss_n = -100;
												if ( ss_n >= 5 ) ss_n = 100;
											}
											return_str = string ( itoa( ss_n, temp, 10 ) );
											break;

	case SIGNAL_STRENGTH_AS_PERCENTAGE	:	if ( status->DvbsSignature == 0x5555 ) {
												signal_lock = ntohs( status->DVBSStatus ) & 0x01;
											} else { 
												signal_lock = ( ntohs( status->DVBSStatus ) & 0x03 ) == 0x03;
											}
											if ( signal_lock ) {
												ss_dbm = ntohs( status->SignalLevel );
												if ( ss_dbm <= -75 ) {
													ss_per = 0.0;
												} else {
													if ( ss_dbm > -25 ) {
														ss_per = 101.0;
													} else {
														ss_per = 2 * ( ss_dbm + 75 );
													}
												}
												sprintf( temp, "%f", ss_per );
												return_str = string( temp );
											} else {
												return_str = string( "0.0" );
											}
											break;

	case SIGNAL_LOCK					:	if ( status->DvbsSignature == 0x5555 ) {
												return_str = string ( itoa( ntohs( status->DVBSStatus ) & 0x01, temp, 10 ) );
											} else { 
												return_str = string ( itoa( ( ntohs( status->DVBSStatus ) & 0x03 ) == 0x03, temp, 10 ) );
											}
											break;


	case DATA_LOCK						:	if ( status->DvbsSignature == 0x5555 ) {
												return_str = string ( itoa( ntohs( status->DVBSStatus ) & 0x01, temp, 10 ) );
											} else { 
												return_str = string ( itoa( ( ntohs( status->DVBSStatus ) & 0x03 ) == 0x03, temp, 10 ) );
											}
											break;

	case CAM_STATUS						:	sprintf( temp, "%d", status->CardStatus );
											return_str = string( temp );
											break;

//	case CA_PROGRAM_STATUS				:	if ( ( index >= 0 ) && ( index < NUMBER_OF_PROGRAMS ) ) {
//												sprintf( temp, "%d", ntohs( status->ProgramStatus[index] ) );
//												return_str = string( temp );
//											} else {
//												return_str = "0";
//											}
//											break;

	case LNB_FAULT						:	if ( status->DvbsSignature == 0x5555 ) {
												return_str = string ( itoa( ( ntohs( status->DVBSStatus ) & 0x04 ) >> 2, temp, 10 ) );
											} else { 
												return_str = string ( itoa( ( ntohs( status->DVBSStatus ) & 0x20 ) >> 5, temp, 10 ) );
											}
											break;

	case VBER							:	sprintf( temp, "%.2e", ntohs( status->VBER_or_PacketErrorRate ) / 10000000.0 );
											return_str = string( temp );
											break;

	case PER							:	sprintf( temp, "%.2e", ntohs( status->VBER_or_PacketErrorRate ) / 1000000.0 );
											return_str = string( temp );
											break;

	case UNCORRECTABLES					:	return_str = string ( itoa( ntohs( status->Uncorrectables ), temp, 10 ) );
											break;

	case CARRIER_TO_NOISE				:	if ( status->DvbsSignature == 0x5555 ) {
												sprintf( temp, "%f", ntohs( status->CNratio ) / 10.0 );
											} else { 
												sprintf( temp, "%f", ntohs( status->CNratio ) / 100.0 );
											}
											return_str = string( temp );
											break;

	case FREQUENCY_OFFSET				:	return_str = string ( itoa( (signed short)(ntohs( status->FrequencyOffset) ), temp, 10 ) );
											break;


	case CARRIER_FREQUENCY				:	if ( (signed short)( ntohs( status->FrequencyOffset ) ) > 0 ) {
												sprintf( temp, "%d MHz (+%d kHz)", ntohs( status->Frequency ) / 10, ntohs( status->FrequencyOffset ) );
											} else {
												sprintf( temp, "%d MHz (%d kHz)", ntohs( status->Frequency ) / 10, (signed short)(ntohs( status->FrequencyOffset )) );
											}
											return_str = string( temp );
											break;

	case PID_COUNT						:
											break;


	case DVB_ACCEPTED					:	return_str = string ( itoa( ntohs( status->DVBAccepted ), temp, 10 ) );
											break;


	case TOTAL_DVB_PACKETS_ACCEPTED		:	big_number = ntohs( status->TotalDVBPacketsAccepted[3] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalDVBPacketsAccepted[2] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalDVBPacketsAccepted[1] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalDVBPacketsAccepted[0] );
#ifdef WINDOWS
											return_str = string ( _ui64toa( big_number, temp, 10 ) );											
#else
											sprintf( temp, "%Lu", big_number );
											return_str = string( temp );
#endif
											break;


	case TOTAL_UNCORRECTABLE_TS_PACKETS	:	big_number = ntohs( status->TotalUncorrectableTSPackets[3] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalUncorrectableTSPackets[2] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalUncorrectableTSPackets[1] );
											big_number = big_number << 16;
											big_number += ntohs( status->TotalUncorrectableTSPackets[0] );
#ifdef WINDOWS
											return_str = string ( _ui64toa( big_number, temp, 10 ) );											
#else
											sprintf( temp, "%Lu", big_number );
											return_str = string( temp );
#endif
											break;


	case ETHERNET_TRANSMIT				:	return_str = string ( itoa( ntohs( status->EthernetTX ), temp, 10 ) );
											break;


	case ETHERNET_RECEIVE				:	return_str = string ( itoa( ntohs( status->EthernetRX ), temp, 10 ) );
											break;


	case ETHERNET_PACKET_DROPPED		:	return_str = string ( itoa( ntohs( status->EthernetDropped ), temp, 10 ) );
											break;


	case ETHERNET_RECEIVE_ERROR			:	return_str = string ( itoa( ntohs( status->EthernetRXErr ), temp, 10 ) );
											break;


	case DVB_SCRAMBLED					:	return_str = string ( itoa( ntohs( status->DVBScrambled ), temp, 10 ) );
											break;


	case DVB_CLEAR						:	return_str = string ( itoa( ntohs( status->DVBDescrambled ), temp, 10 ) );
											break;


	case TOTAL_ETHERNET_PACKETS_OUT		:
											break;


	case SUBNET_MASK					:	sprintf( temp, "%d.%d.%d.%d", 
													 status->SubnetMask[0], status->SubnetMask[1],
													 status->SubnetMask[2], status->SubnetMask[3]  );
											return_str = string( temp );
											break;


	case DEFAULT_GATEWAY_IP				:	sprintf( temp, "%d.%d.%d.%d", 
													 status->DefaultGatewayIP[0],
													 status->DefaultGatewayIP[1],
													 status->DefaultGatewayIP[2],
													 status->DefaultGatewayIP[3]  );
											return_str = string( temp );
											break;


	case UNICAST_STATUS_IP				:	sprintf( temp, "%d.%d.%d.%d", 
													 status->UnicastStatusIP[0], 
													 status->UnicastStatusIP[1],
													 status->UnicastStatusIP[2],
													 status->UnicastStatusIP[3]  );
											return_str = string( temp );
											break;


	case UNICAST_STATUS_PORT			:	return_str = string ( itoa( ntohs( status->UnicastStatusPort ), temp, 10 ) );
											break;


	case BROADCAST_STATUS_PORT			:	return_str = string ( itoa( ntohs( status->BroadcastStatusPort ), temp, 10 ) );
											break;

	case IGMP_ENABLE					:	return_str = string ( itoa( ( status->Filter & 0x04 ) >> 2, temp, 10 ) );
											break;


	case LBAND_FREQUENCY				:	return_str = string ( itoa( ntohs( status->Frequency ) / 10, temp, 10 ) );
											break;


	case LO_FREQUENCY					:	return_str = string ( itoa( ntohs( status->LocalOscFrequency ), temp, 10 ) );
											break;


	case SYMBOL_RATE					:	return_str = string ( itoa( ntohs( status->SymbolRate ), temp, 10 ) );
											break;


	case LNB_POWER						:	return_str = string ( itoa( status->LNBConfiguration & 0x01, temp, 10 ) );
											break;

	case POLARITY						:	return_str = string ( itoa( ( status->LNBConfiguration & 0x02 ) >> 1, temp, 10 ) );
											break;

	case BAND							:	return_str = string ( itoa( ( status->LNBConfiguration & 0x04 ) >> 2, temp, 10 ) );
											break;

	case HI_VOLTAGE_SWITCH				:	return_str = string ( itoa( ( status->LNBConfiguration & 0x08 ) >> 3, temp, 10 ) );
											break;

	case LONG_LINE_COMPENSATION_SWITCH	:	return_str = string ( itoa( ( status->LNBConfiguration & 0x10 ) >> 4, temp, 10 ) );
											break;

	case POLARITY_SWITCHING_VOLTAGE		:	return_str = string ( itoa( ( status->LNBConfiguration & 0x20 ) >> 5, temp, 10 ) );
											break;

	case BAND_SWITCHING_TONE_FREQUENCY	:	return_str = string ( itoa( ( status->LNBConfiguration & 0x40 ) >> 6, temp, 10 ) );
											break;

	case DVB_SIGNAL_TYPE				:	return_str = string ( itoa( ( status->LNBConfiguration & 0x80 ) >> 7, temp, 10 ) );
											break;

	case DVB_SIGNAL_TYPE_CONTROL		:	if ( status->LNBConfiguration & 0x80 ) {
												return_str = string ( itoa( DVBS2, temp, 10 ) );
											} else {
												return_str = string ( itoa( DVBS, temp, 10 ) );
											}
											break;

	case STATUS_TIMESTAMP			:	return timestamp();
											break;

	default								:	return_str = "";
											break;

	}

	return return_str;
}


bool S200V::connected( int timeout )
{
	unsigned char junk[1500];

	return pollStatus( junk, timeout );
}


bool S200V::pollStatus( unsigned char *packet, int timeout )
{
	bool success = false;
	unsigned char message[6] = { 0x47, 0x47, 0x48, 0x48, 0x49, 0x49 };
	struct timeval timeout_struct;
	fd_set readfs;
	int result;
	int recv_len;
	string mac_string;
	char temp[20];


	// Try up to 4 times

    for ( int i = 0; (!success) && ( i < 4 ); i++ ) {

		try {
        
			if ( send( config_sockets[I_STATUS_POLL_PORT], (const char *)(&message), 6, 0 ) != 6 ) throw 1;

#ifdef WINDOWS
			Sleep(30);
#endif
			timeout_struct.tv_sec = timeout/1000;
			timeout_struct.tv_usec = timeout % 1000;

			FD_ZERO( &readfs );
			FD_SET( config_sockets[I_STATUS_POLL_PORT], &readfs );

			// Try to receive the response from receiver.  Check for errors.

#ifdef WINDOWS
			if ( ( result = select( 1, &readfs, NULL, NULL, &timeout_struct ) ) == SOCKET_ERROR ) throw 2;
#else
			if ( ( result = select( config_sockets[I_STATUS_POLL_PORT]+1, &readfs, NULL, NULL, &timeout_struct ) ) == -1 ) throw 2;
#endif

			if ( result == 0 ) throw 3;

#ifdef WINDOWS
			if ( ( recv_len = recv( config_sockets[I_STATUS_POLL_PORT], (char *)packet, 1500, 0 ) ) == SOCKET_ERROR ) {

				result = GetLastError();

				if ( result != WSAETIMEDOUT ) throw 4;

			}
#else
			if ( ( recv_len = recv( config_sockets[I_STATUS_POLL_PORT], (char *)packet, 1500, 0 ) ) == -1 ) throw 4;
#endif

			// Make sure we have received the right number of bytes back

			if ( recv_len != sizeof( S200VStatusMSG ) ) throw 5;

			sprintf( temp, "%02x-%02x-%02x-%02x-%02x-%02x",
					 ((S200VStatusMSG *)packet)->ReceiverMAC[0],
					 ((S200VStatusMSG *)packet)->ReceiverMAC[1],
					 ((S200VStatusMSG *)packet)->ReceiverMAC[2],
					 ((S200VStatusMSG *)packet)->ReceiverMAC[3],
					 ((S200VStatusMSG *)packet)->ReceiverMAC[4],
					 ((S200VStatusMSG *)packet)->ReceiverMAC[5] );

			if ( !matches( RECEIVER_MAC, string( temp ) ) ) throw 6;

			if ( ((S200VStatusMSG *)packet)->DeviceType != typeIDCode() ) throw 7;
//			if ( ((S200VStatusMSG *)packet)->DeviceType != ID_S200Base ) throw 7;

			success = true;

		}

		catch ( int e ) {
			success = false;
		}
	
	} // for 4 retires

	return success;
}


bool S200V::login( string password )
{
	struct sockaddr_in device_address;
	SOCKET login_socket;
	int temp1 = 1000;               // Temporary storage variable
	int last_error_code ;
	int i;
	string mac;
	int mac_byte[6];
	char junk[2];
//	TCHAR temp[16];
	bool success=false;
	struct timeval timeout;
	fd_set readfs;
	int result;
	S200VVProgramListMSG programRequest;
    BYTE Buffer2[sizeof(S200VVProgramListMSG)];


	try {

		// Passwords are tested by requesting the PID List from the receiver

		memset( (void *)&m_sec, 0, sizeof( m_sec ) );
		memset( (void *)&programRequest, 0, sizeof( programRequest ) );

		programRequest.Opcode = htons(0x0001);

		srand( time(NULL) );
		programRequest.Random = rand();

		mac = getParameter( RECEIVER_MAC ).asString();
		sscanf( mac.c_str(), "%2x%c%2x%c%2x%c%2x%c%2x%c%2x",
				&(mac_byte[0]), junk, &(mac_byte[1]), junk, &(mac_byte[2]),
				junk, &(mac_byte[3]), junk, &(mac_byte[4]), junk, &(mac_byte[5]) );
		for ( i = 0; i < 6; i++ ) {
			m_sec.macaddress[i] = (uint8_t)mac_byte[i];
		}
	
		for ( i = 0; ( i < 8 ) && ( i < (int)(password.length()) ); i++ ) {
			m_sec.password[i] = ((char *)(password.data()))[i];
		}

		digitalsignature( &m_sec, (uint8_t *)(&programRequest), sizeof( S200VVProgramListMSG ) - 20 );

		memcpy( programRequest.Signature, m_sec.digest, 20 );


		// Create a socket to communicate with the receiver

#ifdef WINDOWS
		if ( ( login_socket = socket( AF_INET, SOCK_DGRAM, 17 ) ) == INVALID_SOCKET ) throw 1;
#else
		if ( ( login_socket = socket( AF_INET, SOCK_DGRAM, 17 ) ) == -1 ) throw 1;
#endif


		// Connect the newly created socket to the receiver.

   		device_address.sin_family = AF_INET;

   		device_address.sin_port = htons( VIDEO_PROGRAM_PORT );

    	device_address.sin_addr.s_addr = inet_addr( getParameter( RECEIVER_IP ).asString().c_str() );

		if ( ::connect( login_socket, (const struct sockaddr *)(&device_address), sizeof( struct sockaddr_in ) ) != 0) throw 2;

			
        // Download the PID table.  Retry (if required) up to 4 times

        for ( i = 0; (!success) && ( i < 4 ); i++) {

           	// Send the PID table request to the receiver

            if ( send( login_socket, (const char *)(&programRequest), sizeof( S200VVProgramListMSG ), 0 ) != sizeof( S200VVProgramListMSG ) ) {

                last_error_code = N_ERROR_PID_SEND;

			} else {

#ifdef WINDOWS
                Sleep(30);
#endif

                timeout.tv_sec = 1;
                timeout.tv_usec = 0;

       	        FD_ZERO( &readfs );
       	        FD_SET( login_socket, &readfs );

                // Try to receive the response from receiver.  Check for errors.

#ifdef WINDOWS
                if ( ( result = select( 1, &readfs, NULL, NULL, &timeout ) ) != SOCKET_ERROR ) {
#else
                if ( ( result = select( login_socket+1, &readfs, NULL, NULL, &timeout ) ) != -1 ) {
#endif
	       	        if ( result != 0 ) {

#ifdef WINDOWS
  		    	        if( (temp1 = recv( login_socket, (char *)Buffer2, sizeof(S200VVProgramListMSG), 0 ) ) == SOCKET_ERROR ) {

            	    		temp1 = GetLastError();

                	        if(temp1 != WSAETIMEDOUT){

        	        			last_error_code = N_ERROR_PID_RECEIVE;

							} // if


#else
   		    	        if( (temp1 = recv( login_socket, (char *)Buffer2, sizeof(S200VVProgramListMSG), 0 ) ) == -1 ) {
							last_error_code = N_ERROR_PID_RECEIVE;
#endif

						} else {

	    	    	        // Make sure we have received the right number of bytes back

    		    	        if (temp1 == sizeof(S200VVProgramListMSG)) {

				                last_error_code = N_ERROR_SUCCESS;
								success = true;

							} else {

								last_error_code = N_ERROR_PID_ACK_BAD;

							} // if

						} // if recv

					} else {

						last_error_code = N_ERROR_PID_RECEIVE;

					} // if select result 0

				} else {

					last_error_code = N_ERROR_PID_RECEIVE;

				} // if select

			} // if send
	
		} // for 4 retires

	}


	catch ( int e ) {

		switch ( e ) {

			case 1	:	last_error_code = N_ERROR_SOCKET_CREATE;
						break;

			case 2	:	last_error_code = N_ERROR_SOCKET_CONNECT;
						break;

		}
	}


#ifdef WINDOWS
    closesocket( login_socket );
#else
    close( login_socket );
#endif


	if ( last_error_code == N_ERROR_SUCCESS ) {
		
		return true; 
	
	} else {
		
		return false;

	}

}






bool S200V::apply( ReceiverParameter ParameterName )
{

	int result = 0;

	try {
		if ( ParameterName == ALL_PARAMETERS ) {
			if ( !applyVProgramConfig() ) throw 1;
			if ( !applyRFConfig() ) throw 4;
			if ( !applyNetworkConfig() ) throw 8;
		} else {
			throw 9;
		}
	}

	catch ( int e ) {

		result = e;

	}

	return !result;

}


bool S200V::applyVProgramConfig()
{ 
	int num_items = sizeof( vprogramParams ) / sizeof( ReceiverParameter );
	bool dirty = false;
	bool up_to_date = true;
	int i;
//	unsigned short program;
	int ip1, ip2, ip3, ip4;
	char junk[2];
//	bool forward_ts = false;
//	bool forward_nulls = false;
//	char temp[10];
	char temp[50];


	for ( i = 0; !dirty && ( i < num_items ); i++ ) {
		if ( dirtyFlags[vprogramParams[i]] ) {
			dirty = true;
		}
	}

	if ( dirty ) {

		try {

			S200VVProgramListMSG vprogramConfig;

			memset( (void *)&vprogramConfig, 0, sizeof( S200VVProgramListMSG ) );

			vprogramConfig.Opcode = htons( (unsigned short)0x2 );

			for ( i = 0; i < NUMBER_OF_VPROGRAMS; i++ ) {

				vprogramConfig.ProgramList[i].programNumber = htons( pendingVProgramNumbers[i].asShort() );
				sscanf( pendingVProgramDestinationIPs[i].asString().c_str(), "%d%c%d%c%d%c%d", &ip1, junk, &ip2, junk, &ip3, junk, &ip4 );	
				vprogramConfig.ProgramList[i].destinationIP[0] = (unsigned char)ip1;
				vprogramConfig.ProgramList[i].destinationIP[1] = (unsigned char)ip2;
				vprogramConfig.ProgramList[i].destinationIP[2] = (unsigned char)ip3;
				vprogramConfig.ProgramList[i].destinationIP[3] = (unsigned char)ip4;
				vprogramConfig.ProgramList[i].destinationUDP = htons( pendingVProgramDestinationPorts[i].asShort() );
				
			}

			vprogramConfig.mode = htons( 0 );

			if ( hasParameter( FORWARD_ALL_FLAG ) ) {
				if ( pendingParameters[ FORWARD_ALL_FLAG ]->asFlag() ) {
					if ( pendingParameters[ FORWARD_NULLS_FLAG ]->asFlag() ) {
						vprogramConfig.mode = htons( 1 );
					} else {
						vprogramConfig.mode = htons( 2 );
					}
					if ( pendingParameters[STREAM_DESTINATION_IP]->isSet() ) {
						sscanf( pendingParameters[ STREAM_DESTINATION_IP ]->asString().c_str(), "%d%c%d%c%d%c%d", &ip1, junk, &ip2, junk, &ip3, junk, &ip4 );	
						vprogramConfig.destinationIP[0] = (unsigned char)ip1;
						vprogramConfig.destinationIP[1] = (unsigned char)ip2;
						vprogramConfig.destinationIP[2] = (unsigned char)ip3;
						vprogramConfig.destinationIP[3] = (unsigned char)ip4;
						vprogramConfig.destinationUDP = htons( pendingParameters[ STREAM_DESTINATION_PORT ]->asShort() );
					}
				}
			}

			srand( time(NULL) );
			vprogramConfig.Random = rand();

			digitalsignature( &m_sec, (uint8_t *)(&vprogramConfig), sizeof( S200VVProgramListMSG ) - 20 );

			memcpy( vprogramConfig.Signature, m_sec.digest, 20 );

			if ( !sendConfig( I_VIDEO_PROGRAM_PORT, (unsigned char *)&vprogramConfig, sizeof( vprogramConfig ) ) ) throw 1;

			// Make pending parameters current


			for ( i = 0; i < NUMBER_OF_VPROGRAMS; i++ ) {
				currentVProgramNumbers[i].setM_value( pendingVProgramNumbers[i].getM_value() );
				currentVProgramDestinationIPs[i].setM_value( pendingVProgramDestinationIPs[i].getM_value() ); 
				currentVProgramDestinationPorts[i].setM_value( pendingVProgramDestinationPorts[i].getM_value() ); 
			}

			for ( i = 0; i < num_items; i++ ) {
				if ( parameterIsEnumerated( vprogramParams[i] ) ) {
					setCurrentParameter( vprogramParams[i], itoa( getParameter( vprogramParams[i], PENDING ).asShort(), temp, 10 ) );
				} else {
					setCurrentParameter( vprogramParams[i], getParameter( vprogramParams[i], PENDING ).asString() );
				}
				dirtyFlags[vprogramParams[i]] = false;
			}

		}

		catch ( int e ) {

			up_to_date = false;

		}

	}

	return up_to_date;
}


bool S200V::applyRFConfig()
{ 
	int num_items = sizeof( rfParams ) / sizeof( ReceiverParameter );
//	unsigned char statusMsg[1500];
	bool dirty = false;
	bool up_to_date = true;
	int i;
	char temp[50];


	for ( i = 0; !dirty && ( i < num_items ); i++ ) {
		if ( dirtyFlags[rfParams[i]] ) {
			dirty = true;
		}
	}

	if ( dirty ) {

		try {

			S200VRFConfigMSG rfConfig;

			memset( (void *)&rfConfig, 0, sizeof( rfConfig ) );

			rfConfig.SymbolRate = htons( getParameter( SYMBOL_RATE, PENDING ).asShort() );

			rfConfig.Frequency = htons( 10 * getParameter( LBAND_FREQUENCY, PENDING ).asShort() );

			if ( getParameter( DVB_SIGNAL_TYPE_CONTROL, PENDING ).asShort() == DVBS2 ) {
				rfConfig.LNBConfig = 0x1;
			}
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( BAND_SWITCHING_TONE_FREQUENCY, PENDING ).asShort();
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( POLARITY_SWITCHING_VOLTAGE, PENDING ).asShort();
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( LONG_LINE_COMPENSATION_SWITCH, PENDING ).asShort();
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( HI_VOLTAGE_SWITCH, PENDING ).asShort();
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( BAND, PENDING ).asShort();
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( POLARITY, PENDING ).asShort();
			rfConfig.LNBConfig = ( rfConfig.LNBConfig << 1 ) + getParameter( LNB_POWER, PENDING ).asShort();

//			rfConfig.ViterbiRate = 0x08;

			rfConfig.LocalOscFrequency = htons( getParameter( LO_FREQUENCY, PENDING ).asShort() );

			srand( time(NULL) );
			rfConfig.Random = rand();

			digitalsignature( &m_sec, (uint8_t *)(&rfConfig), sizeof( S200VRFConfigMSG ) - 20 );

			memcpy( rfConfig.Signature, m_sec.digest, 20 );

			if ( !sendConfig( I_RF_PORT, (unsigned char *)&rfConfig, sizeof( rfConfig ) ) ) throw 1;

/*
			if ( !pollStatus( (unsigned char *)statusMsg, 2000 ) ) throw 3;

			if ( ( ( ntohs( ((StatusMSG *)statusMsg)->SymbolRate ) ) < ( getParameter( SYMBOL_RATE, PENDING ).asShort() - 10 ) ) ||
				 ( ( ntohs( ((StatusMSG *)statusMsg)->SymbolRate ) ) > ( getParameter( SYMBOL_RATE, PENDING ).asShort() + 10 ) ) ) throw 4;			

			if ( ( ( ntohs( ((StatusMSG *)statusMsg)->Frequency ) ) < ( getParameter( LBAND_FREQUENCY, PENDING ).asShort() - 100 ) ) ||
				 ( ( ntohs( ((StatusMSG *)statusMsg)->Frequency ) ) > ( getParameter( LBAND_FREQUENCY, PENDING ).asShort() + 100 ) ) ) throw 5;

			if ( ( ( ((StatusMSG *)statusMsg)->LNBConfiguration & 0x40 ) != 0 ) != getParameter( BAND_SWITCHING_TONE_FREQUENCY, PENDING ).asFlag() ) throw 6;
			if ( ( ( ((StatusMSG *)statusMsg)->LNBConfiguration & 0x20 ) != 0 ) != getParameter( POLARITY_SWITCHING_VOLTAGE, PENDING ).asFlag() ) throw 7;
			if ( ( ( ((StatusMSG *)statusMsg)->LNBConfiguration & 0x10 ) != 0 ) != getParameter( LONG_LINE_COMPENSATION_SWITCH, PENDING ).asFlag() ) throw 8;
			if ( ( ( ((StatusMSG *)statusMsg)->LNBConfiguration & 0x08 ) != 0 ) != getParameter( HI_VOLTAGE_SWITCH, PENDING ).asFlag() ) throw 9;
			if ( ( ( ((StatusMSG *)statusMsg)->LNBConfiguration & 0x04 ) != 0 ) != getParameter( BAND, PENDING ).asFlag() ) throw 10;
			if ( ( ( ((StatusMSG *)statusMsg)->LNBConfiguration & 0x02 ) != 0 ) != getParameter( POLARITY, PENDING ).asFlag() ) throw 11;
			if ( ( ( ((StatusMSG *)statusMsg)->LNBConfiguration & 0x01 ) != 0 ) != getParameter( LNB_POWER, PENDING ).asFlag() ) throw 12;

			if ( ((StatusMSG *)statusMsg)->LocalOscFrequency != getParameter( LO_FREQUENCY, PENDING ).asShort() ) throw 13;
*/
			// Make pending parameters current

			for ( i = 0; i < num_items ; i++ ) {
				if ( parameterIsEnumerated( rfParams[i] ) ) {
					setCurrentParameter( rfParams[i], itoa( getParameter( rfParams[i], PENDING ).asShort(), temp, 10 ) );
				} else {
					setCurrentParameter( rfParams[i], getParameter( rfParams[i], PENDING ).asString() );
				}
				dirtyFlags[rfParams[i]] = false;
			}
		}

		catch ( int e ) {

			up_to_date = false;

		}

	}
	
	return up_to_date;
}


bool S200V::applyNetworkConfig()
{

	int num_items = sizeof( networkParams ) / sizeof( ReceiverParameter );
	bool dirty = false;
	bool up_to_date = true;
	unsigned char junk[2];
	unsigned int ip1, ip2, ip3, ip4;
	NOVRA_ERRORS ErrorCode;
//	unsigned char statusMsg[1500];
	int i;
//	char temp_str[20];
	char temp[50];


	for ( i = 0; !dirty && ( i < num_items ); i++ ) {
		if ( dirtyFlags[networkParams[i]] ) {
			dirty = true;
		}
	}

	if ( dirty ) {

		try {

			S200VNetConfigMSG netConfig;

			memset( (void *)&netConfig, 0, sizeof( netConfig ) );

			sscanf( getParameter( RECEIVER_IP, PENDING ).asString().c_str(),
					"%d%c%d%c%d%c%d", &ip1, junk, &ip2, junk, &ip3, junk, &ip4 );
			netConfig.ReceiverIP[0] = (unsigned char)ip1;
			netConfig.ReceiverIP[1] = (unsigned char)ip2;
			netConfig.ReceiverIP[2] = (unsigned char)ip3;
			netConfig.ReceiverIP[3] = (unsigned char)ip4;

			sscanf( getParameter( SUBNET_MASK, PENDING ).asString().c_str(),
					"%d%c%d%c%d%c%d", &ip1, junk, &ip2, junk, &ip3, junk, &ip4 );
			netConfig.SubnetMask[0] = (unsigned char)ip1;
			netConfig.SubnetMask[1] = (unsigned char)ip2;
			netConfig.SubnetMask[2] = (unsigned char)ip3;
			netConfig.SubnetMask[3] = (unsigned char)ip4;

			sscanf( getParameter( DEFAULT_GATEWAY_IP, PENDING ).asString().c_str(),
					"%d%c%d%c%d%c%d", &ip1, junk, &ip2, junk, &ip3, junk, &ip4 );
			netConfig.DefaultGatewayIP[0] = (unsigned char)ip1;
			netConfig.DefaultGatewayIP[1] = (unsigned char)ip2;
			netConfig.DefaultGatewayIP[2] = (unsigned char)ip3;
			netConfig.DefaultGatewayIP[3] = (unsigned char)ip4;

			sscanf( getParameter( UNICAST_STATUS_IP, PENDING ).asString().c_str(),
					"%d%c%d%c%d%c%d", &ip1, junk, &ip2, junk, &ip3, junk, &ip4 );
			netConfig.UnicastStatusIP[0] = (unsigned char)ip1;
			netConfig.UnicastStatusIP[1] = (unsigned char)ip2;
			netConfig.UnicastStatusIP[2] = (unsigned char)ip3;
			netConfig.UnicastStatusIP[3] = (unsigned char)ip4;

			netConfig.UnicastStatusPort = htons( getParameter( UNICAST_STATUS_PORT, PENDING ).asShort() );
			netConfig.BroadcastStatusPort = htons( getParameter( BROADCAST_STATUS_PORT, PENDING ).asShort() );

			netConfig.IGMP = htons( getParameter( IGMP_ENABLE, PENDING ).asShort() << 2 );

			srand( time(NULL) );
			netConfig.Random = rand();

			digitalsignature( &m_sec, (uint8_t *)(&netConfig), sizeof( S200VNetConfigMSG ) - 20 );

			memcpy( netConfig.Signature, m_sec.digest, 20 );

			if ( !sendConfig( I_NETWORK_PORT, (unsigned char *)&netConfig, sizeof( netConfig ) ) ) throw 1;

			if ( !connected( 2000 ) ) {

				// Try to connect using pending network config

				if ( !_connect( 2000, ErrorCode, PENDING ) ) throw 2;

				if ( !connected( 2000 ) ) throw 3;

			}

/*
			if ( !pollStatus( (unsigned char *)statusMsg, 2000 ) ) throw 3;


			sprintf( temp_str, "%d.%d.%d.%d",
					 ((StatusMSG *)statusMsg)->ReceiverIP[0],
					 ((StatusMSG *)statusMsg)->ReceiverIP[1],
					 ((StatusMSG *)statusMsg)->ReceiverIP[2],
					 ((StatusMSG *)statusMsg)->ReceiverIP[3]  );

			if ( !matches( RECEIVER_IP, string( temp_str ), PENDING ) ) throw 4;

			sprintf( temp_str, "%d.%d.%d.%d",
					 ((StatusMSG *)statusMsg)->SubnetMask[0],
					 ((StatusMSG *)statusMsg)->SubnetMask[1],
					 ((StatusMSG *)statusMsg)->SubnetMask[2],
					 ((StatusMSG *)statusMsg)->SubnetMask[3]  );

			if ( !matches( SUBNET_MASK, string( temp_str ), PENDING ) ) throw 5;

			sprintf( temp_str, "%d.%d.%d.%d",
					 ((StatusMSG *)statusMsg)->DefaultGatewayIP[0],
					 ((StatusMSG *)statusMsg)->DefaultGatewayIP[1],
					 ((StatusMSG *)statusMsg)->DefaultGatewayIP[2],
					 ((StatusMSG *)statusMsg)->DefaultGatewayIP[3]  );

			if ( !matches( DEFAULT_GATEWAY_IP, string( temp_str ), PENDING ) ) throw 6;

			sprintf( temp_str, "%d.%d.%d.%d",
					 ((StatusMSG *)statusMsg)->UnicastStatusIP[0],
					 ((StatusMSG *)statusMsg)->UnicastStatusIP[1],
					 ((StatusMSG *)statusMsg)->UnicastStatusIP[2],
					 ((StatusMSG *)statusMsg)->UnicastStatusIP[3]  );

			if ( !matches( UNICAST_STATUS_IP, string( temp_str ), PENDING ) ) throw 7;

			if ( !matches( UNICAST_STATUS_PORT, string( itoa( ntohs( ((StatusMSG *)statusMsg)->UnicastStatusPort ), temp_str, 10 ) ), PENDING ) ) throw 8;

			if ( !matches( BROADCAST_STATUS_PORT, string( itoa( ntohs( ((StatusMSG *)statusMsg)->BroadcastStatusPort ), temp_str, 10 ) ), PENDING ) ) throw 9;

			if ( !matches( IGMP_ENABLE, string ( itoa( ( ((StatusMSG *)statusMsg)->Filter & 0x04 ) >> 2, temp_str, 10 ) ), PENDING ) ) throw 10;
*/
			// Make pending parameters current

			for ( i = 0; i < num_items ; i++ ) {
				if ( parameterIsEnumerated( networkParams[i] ) ) {
					setCurrentParameter( networkParams[i], itoa( getParameter( networkParams[i], PENDING ).asShort(), temp, 10 ) );
				} else {
					setCurrentParameter( networkParams[i], getParameter( networkParams[i], PENDING ).asString() );
				}
				dirtyFlags[networkParams[i]] = false;
			}
			
		}

		catch ( int e ) {

			up_to_date = false;

		}

	}
	
	return up_to_date;
}




bool S200V::readCurrentSettings()
{
	bool success = true;

	if ( hasParameter( SIZE_VPROGRAM_LIST ) ) {
		if ( !readVProgramList() ) success = false;
	}

	return success;
}




bool S200V::setIndexedParameter( ReceiverParameter ParameterName, int index, string ParameterValue )
{
	bool success = false;

	
	switch ( ParameterName ) {

		case PID					:	if ( index < NUMBER_OF_PIDS ) {

											pendingPIDs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case RAW_FORWARD_FLAG		:	if ( index < NUMBER_OF_PIDS ) {

											pendingRawForwardFlags[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case DESTINATION_MASK		:	if ( index < NUMBER_OF_PIDS ) {

											pendingDestinationMasks[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case PID_DESTINATION_IP		:	if ( index < NUMBER_OF_DESTINATIONS ) {

											pendingPIDDestinationIPs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case PID_DESTINATION_PORT	:	if ( index < NUMBER_OF_DESTINATIONS ) {

											pendingPIDDestinationPorts[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case PROGRAM_NUMBER			:	if ( index < NUMBER_OF_PATS ) {

											pendingProgramNumbers[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

//		case PMT_PID				:	if ( index < NUMBER_OF_PATS ) {
//
//											pendingPMTPIDs[index].setM_value( ParameterValue );
//
//											dirtyFlags[ParameterName] = true;
//
//											success = true;
//
//										}
//
//										break;

		case CA_PROGRAM_NUMBER		:	if ( index < NUMBER_OF_PROGRAMS ) {

											pendingCAProgramNumbers[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

//		case CA_PROGRAM_STATUS		:	if ( index < NUMBER_OF_PROGRAMS ) {
//
//											CAProgramStatus[index].setM_value( ParameterValue );
//
//											success = true;
//
//										}
//
//										break;

		case FIXED_KEY_PID			:	if ( index < NUMBER_OF_FIXED_KEYS ) {

											pendingFKPIDs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

		case FIXED_KEY_SIGN			:	if ( index < NUMBER_OF_FIXED_KEYS ) {

											pendingFKSigns[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case FIXED_KEY				:	if ( index < NUMBER_OF_FIXED_KEYS ) {

											pendingFKs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case ORIGINAL_IP			:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											pendingOriginalIPs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case NEW_IP					:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											pendingNewIPs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case IP_REMAP_MASK			:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											pendingIPRemapMasks[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case IP_REMAP_TTL			:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											pendingIPRemapTTLs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case IP_REMAP_ACTION		:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											pendingIPRemapActions[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;


		case VPROGRAM_NUMBER		:	if ( index < NUMBER_OF_VPROGRAMS ) {

											pendingVProgramNumbers[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		case PROGRAM_NAME			:	if ( index < NUMBER_OF_VPROGRAMS ) {

											programNames[index].setM_value( ParameterValue );

											success = true;

										}

										break;


		case VPROGRAM_DESTINATION_IP :	if ( index < NUMBER_OF_VPROGRAMS ) {

											pendingVProgramDestinationIPs[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;


		case VPROGRAM_DESTINATION_PORT : if ( index < NUMBER_OF_VPROGRAMS ) {

											pendingVProgramDestinationPorts[index].setM_value( ParameterValue );

											dirtyFlags[ParameterName] = true;

											success = true;

										}

										break;

		default						:	success = false;
						
										break;

	}

	return success;
}


ParameterValue& S200V::getIndexedParameter(	ReceiverParameter ParameterName, 
											int index, 
											ParameterSet parameter_set  )
{
	switch( ParameterName ) {

		case PID					:	if ( index < NUMBER_OF_PIDS ) {

											if ( parameter_set == CURRENT ) {

												return currentPIDs[index];

											} else {

												if ( pendingPIDs[index].isSet() ) {

													return pendingPIDs[index];

												} else {

													return currentPIDs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case RAW_FORWARD_FLAG		:	if ( index < NUMBER_OF_PIDS ) {

											if ( parameter_set == CURRENT ) {

												return currentRawForwardFlags[index];

											} else {

												if ( pendingRawForwardFlags[index].isSet() ) {

													return pendingRawForwardFlags[index];

												} else {

													return currentRawForwardFlags[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case DESTINATION_MASK		:	if ( index < NUMBER_OF_PIDS ) {

											if ( parameter_set == CURRENT ) {

												return currentDestinationMasks[index];

											} else {

												if ( pendingDestinationMasks[index].isSet() ) {

													return pendingDestinationMasks[index];

												} else {

													return currentDestinationMasks[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case PID_DESTINATION_IP		:	if ( index < NUMBER_OF_DESTINATIONS ) {

											if ( parameter_set == CURRENT ) {

												return currentPIDDestinationIPs[index];

											} else {

												if ( pendingPIDDestinationIPs[index].isSet() ) {

													return pendingPIDDestinationIPs[index];

												} else {

													return currentPIDDestinationIPs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case PID_DESTINATION_PORT	:	if ( index < NUMBER_OF_DESTINATIONS ) {

											if ( parameter_set == CURRENT ) {

												return currentPIDDestinationPorts[index];

											} else {

												if ( pendingPIDDestinationPorts[index].isSet() ) {

													return pendingPIDDestinationPorts[index];

												} else {

													return currentPIDDestinationPorts[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case PROGRAM_NUMBER			:	if ( index < NUMBER_OF_PATS ) {

											if ( parameter_set == CURRENT ) {

												return currentProgramNumbers[index];

											} else {

												if ( pendingProgramNumbers[index].isSet() ) {

													return pendingProgramNumbers[index];

												} else {

													return currentProgramNumbers[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case CA_PROGRAM_NUMBER		:	if ( index < NUMBER_OF_PROGRAMS ) {

											if ( parameter_set == CURRENT ) {

												return currentCAProgramNumbers[index];

											} else {

												if ( pendingCAProgramNumbers[index].isSet() ) {

													return pendingCAProgramNumbers[index];

												} else {

													return currentCAProgramNumbers[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case FIXED_KEY_PID			:	if ( index < NUMBER_OF_FIXED_KEYS ) {

											if ( parameter_set == CURRENT ) {

												return currentFKPIDs[index];

											} else {

												if ( pendingFKPIDs[index].isSet() ) {

													return pendingFKPIDs[index];

												} else {

													return currentFKPIDs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case FIXED_KEY_SIGN			:	if ( index < NUMBER_OF_FIXED_KEYS ) {

											if ( parameter_set == CURRENT ) {

												return currentFKSigns[index];

											} else {

												if ( pendingFKSigns[index].isSet() ) {

													return pendingFKSigns[index];

												} else {

													return currentFKSigns[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case FIXED_KEY				:	if ( index < NUMBER_OF_FIXED_KEYS ) {

											if ( parameter_set == CURRENT ) {

												return currentFKs[index];

											} else {

												if ( pendingFKs[index].isSet() ) {

													return pendingFKs[index];

												} else {

													return currentFKs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case ORIGINAL_IP			:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											if ( parameter_set == CURRENT ) {

												return currentOriginalIPs[index];

											} else {

												if ( pendingOriginalIPs[index].isSet() ) {

													return pendingOriginalIPs[index];

												} else {

													return currentOriginalIPs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case NEW_IP				:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											if ( parameter_set == CURRENT ) {

												return currentNewIPs[index];

											} else {

												if ( pendingNewIPs[index].isSet() ) {

													return pendingNewIPs[index];

												} else {

													return currentNewIPs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case IP_REMAP_MASK			:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											if ( parameter_set == CURRENT ) {

												return currentIPRemapMasks[index];

											} else {

												if ( pendingIPRemapMasks[index].isSet() ) {

													return pendingIPRemapMasks[index];

												} else {

													return currentIPRemapMasks[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case IP_REMAP_TTL			:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											if ( parameter_set == CURRENT ) {

												return currentIPRemapTTLs[index];

											} else {

												if ( pendingIPRemapTTLs[index].isSet() ) {

													return pendingIPRemapTTLs[index];

												} else {

													return currentIPRemapTTLs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case IP_REMAP_ACTION		:	if ( index < NUMBER_OF_IP_REMAP_RULES ) {

											if ( parameter_set == CURRENT ) {

												return currentIPRemapActions[index];

											} else {

												if ( pendingIPRemapActions[index].isSet() ) {

													return pendingIPRemapActions[index];

												} else {

													return currentIPRemapActions[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case VPROGRAM_NUMBER		:	if ( index < NUMBER_OF_VPROGRAMS ) {

											if ( parameter_set == CURRENT ) {

												return currentVProgramNumbers[index];

											} else {

												if ( pendingVProgramNumbers[index].isSet() ) {

													return pendingVProgramNumbers[index];

												} else {

													return currentVProgramNumbers[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case PROGRAM_NAME			:	if ( index < NUMBER_OF_VPROGRAMS ) {

											return programNames[index];
						
										} else {

											return nullParameter;

										}


		case VPROGRAM_DESTINATION_IP :	if ( index < NUMBER_OF_VPROGRAMS ) {

											if ( parameter_set == CURRENT ) {

												return currentVProgramDestinationIPs[index];

											} else {

												if ( pendingVProgramDestinationIPs[index].isSet() ) {

													return pendingVProgramDestinationIPs[index];

												} else {

													return currentVProgramDestinationIPs[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case VPROGRAM_DESTINATION_PORT : if ( index < NUMBER_OF_VPROGRAMS ) {

											if ( parameter_set == CURRENT ) {

												return currentVProgramDestinationPorts[index];

											} else {

												if ( pendingVProgramDestinationPorts[index].isSet() ) {

													return pendingVProgramDestinationPorts[index];

												} else {

													return currentVProgramDestinationPorts[index];

												}

											}	
						
										} else {

											return nullParameter;

										}

		case PROGRAM_GUIDE_PROGRAM_NUMBER	:	if ( index < NUMBER_OF_PROGRAM_GUIDE_ENTRIES ) {

													if ( guideProgramNumbers[index].isSet() ) {

														return guideProgramNumbers[index];

													} else {
											
														return nullParameter;

													}

												} else {

													return nullParameter;

												}

		case PROGRAM_GUIDE_PROGRAM_NAME	:	if ( index < NUMBER_OF_PROGRAM_GUIDE_ENTRIES ) {

													if ( guideProgramNames[index].isSet() ) {

														return guideProgramNames[index];

													} else {
											
														return nullParameter;

													}

												} else {

													return nullParameter;

												}

		case CA_PROGRAM_SCRAMBLED_FLAG:	if ( index < NUMBER_OF_VPROGRAMS ) {

											return CAProgramScrambledFlags[index];
						
										} else {

											return nullParameter;

										}

		case CA_SYSTEM_ID			:	if ( index < NUMBER_OF_VPROGRAMS ) {

											return CASystemIDs[index];
						
										} else {

											return nullParameter;

										}

		case PMT_PID				:	if ( index < NUMBER_OF_VPROGRAMS ) {

											return PMTPIDs[index];
						
										} else {

											return nullParameter;

										}

		case PMT_VERSION			:	if ( index < NUMBER_OF_VPROGRAMS ) {

											return PMTVersions[index];
						
										} else {

											return nullParameter;

										}

		case PROGRAM_STREAM_TYPE	:	if ( index < NUMBER_OF_VPROGRAMS ) {

											return ProgramStreamTypes[index];
						
										} else {

											return nullParameter;

										}

		case DATA_PID				:
		case VIDEO_PID				:	if ( index < NUMBER_OF_VPROGRAMS ) {

											return VideoPIDs[index];
						
										} else {

											return nullParameter;

										}

		case PCR_PID				:	if ( index < NUMBER_OF_VPROGRAMS ) {

											return PCRPIDs[index];
						
										} else {

											return nullParameter;

										}


		default		:	return nullParameter;

	}

}



ParameterValue& S200V::getDblIndexedParameter(	ReceiverParameter ParameterName,
												int index1, 
												int index2, 
												ParameterSet parameter_set	)
{
	switch( ParameterName ) {


		case AUDIO_PID			:	if ( index1 < NUMBER_OF_VPROGRAMS ) {

										if ( index2 < NUMBER_OF_AUDIO_PIDS_PER_PROGRAM ) {

											return (AudioPIDs[index1])[index2];

										} else {

											return nullParameter;

										}
						
									} else {

										return nullParameter;

									}

		case AUDIO_PID_LANGUAGE	:	if ( index1 < NUMBER_OF_VPROGRAMS ) {

										if ( index2 < NUMBER_OF_AUDIO_PIDS_PER_PROGRAM ) {

											return (AudioPIDLanguages[index1])[index2];

										} else {

											return nullParameter;

										}
						
									} else {

										return nullParameter;

									}

		case AUDIO_TYPE			:	if ( index1 < NUMBER_OF_VPROGRAMS ) {

										if ( index2 < NUMBER_OF_AUDIO_PIDS_PER_PROGRAM ) {

											return (AudioPIDTypes[index1])[index2];

										} else {

											return nullParameter;

										}
						
									} else {

										return nullParameter;

									}


		default		:	return nullParameter;

	}

}


void S200V::cancelIndexedParameters()
{
	int i;


	for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {

		if ( currentPIDs[i].isSet() ) {

			pendingPIDs[i].setM_value( currentPIDs[i].getM_value() );

		} else {

			pendingPIDs[i].setM_isSet( false );

		}

		if ( currentRawForwardFlags[i].isSet() ) {

			pendingRawForwardFlags[i].setM_value( currentRawForwardFlags[i].getM_value() );

		} else {

			pendingRawForwardFlags[i].setM_isSet( false );

		}

		if ( currentDestinationMasks[i].isSet() ) {

			pendingDestinationMasks[i].setM_value( currentDestinationMasks[i].getM_value() );

		} else {

			pendingDestinationMasks[i].setM_isSet( false );

		}

	}

	for ( i = 0; i < NUMBER_OF_DESTINATIONS; i++ ) {

		if ( currentPIDDestinationIPs[i].isSet() ) {

			pendingPIDDestinationIPs[i].setM_value( currentPIDDestinationIPs[i].getM_value() );

		} else {

			pendingPIDDestinationIPs[i].setM_isSet( false );

		}

		if ( currentPIDDestinationPorts[i].isSet() ) {

			pendingPIDDestinationPorts[i].setM_value( currentPIDDestinationPorts[i].getM_value() );

		} else {

			pendingPIDDestinationPorts[i].setM_isSet( false );

		}

	}

	for ( i = 0; i < NUMBER_OF_PATS; i++ ) {

		if ( currentProgramNumbers[i].isSet() ) {

			pendingProgramNumbers[i].setM_value( currentProgramNumbers[i].getM_value() );

		} else {

			pendingProgramNumbers[i].setM_isSet( false );

		}


//		if ( currentPMTPIDs[i].isSet() ) {
//
//			pendingPMTPIDs[i].setM_value( currentPMTPIDs[i].getM_value() );
//
//		} else {
//
//			pendingPMTPIDs[i].setM_isSet( false );
//
//		}

	}

	for ( i = 0; i < NUMBER_OF_PROGRAMS; i++ ) {

		if ( currentCAProgramNumbers[i].isSet() ) {

			pendingCAProgramNumbers[i].setM_value( currentCAProgramNumbers[i].getM_value() );

		} else {

			pendingCAProgramNumbers[i].setM_isSet( false );

		}

	}

	for ( i = 0; i < NUMBER_OF_FIXED_KEYS; i++ ) {

		if ( currentFKPIDs[i].isSet() ) {

			pendingFKPIDs[i].setM_value( currentFKPIDs[i].getM_value() );

		} else {

			pendingFKPIDs[i].setM_isSet( false );

		}

		if ( currentFKSigns[i].isSet() ) {

			pendingFKSigns[i].setM_value( currentFKSigns[i].getM_value() );

		} else {

			pendingFKSigns[i].setM_isSet( false );

		}

		if ( currentFKs[i].isSet() ) {

			pendingFKs[i].setM_value( currentFKs[i].getM_value() );

		} else {

			pendingFKs[i].setM_isSet( false );

		}


	}

	for ( i = 0; i < NUMBER_OF_IP_REMAP_RULES; i++ ) {

		if ( currentOriginalIPs[i].isSet() ) {

			pendingOriginalIPs[i].setM_value( currentOriginalIPs[i].getM_value() );

		} else {

			pendingOriginalIPs[i].setM_isSet( false );

		}

		if ( currentNewIPs[i].isSet() ) {

			pendingNewIPs[i].setM_value( currentNewIPs[i].getM_value() );

		} else {

			pendingNewIPs[i].setM_isSet( false );

		}

		if ( currentIPRemapMasks[i].isSet() ) {

			pendingIPRemapMasks[i].setM_value( currentIPRemapMasks[i].getM_value() );

		} else {

			pendingIPRemapMasks[i].setM_isSet( false );

		}

		if ( currentIPRemapTTLs[i].isSet() ) {

			pendingIPRemapTTLs[i].setM_value( currentIPRemapTTLs[i].getM_value() );

		} else {

			pendingIPRemapTTLs[i].setM_isSet( false );

		}

		if ( currentIPRemapActions[i].isSet() ) {

			pendingIPRemapActions[i].setM_value( currentIPRemapActions[i].getM_value() );

		} else {

			pendingIPRemapActions[i].setM_isSet( false );

		}

	}

	for ( i = 0; i < NUMBER_OF_VPROGRAMS; i++ ) {

		if ( currentVProgramNumbers[i].isSet() ) {

			pendingVProgramNumbers[i].setM_value( currentVProgramNumbers[i].getM_value() );

		} else {

			pendingVProgramNumbers[i].setM_isSet( false );

		}

		if ( currentVProgramDestinationIPs[i].isSet() ) {

			pendingVProgramDestinationIPs[i].setM_value( currentVProgramDestinationIPs[i].getM_value() );

		} else {

			pendingVProgramDestinationIPs[i].setM_isSet( false );

		}

		if ( currentVProgramDestinationPorts[i].isSet() ) {

			pendingVProgramDestinationPorts[i].setM_value( currentVProgramDestinationPorts[i].getM_value() );

		} else {

			pendingVProgramDestinationPorts[i].setM_isSet( false );

		}

	}


}


void S200V::clearIndexedParameters()
{
	int i;


	for ( i = 0; i < NUMBER_OF_PIDS; i++ ) {

		pendingPIDs[i].setM_value( "8191" );
		pendingRawForwardFlags[i].setM_value( "0" );
		pendingDestinationMasks[i].setM_value( "0" );

	}

	for ( i = 0; i < NUMBER_OF_DESTINATIONS; i++ ) {

		pendingPIDDestinationIPs[i].setM_value( "0.0.0.0" );
		pendingPIDDestinationPorts[i].setM_value( "0" );

	}

	for ( i = 0; i < NUMBER_OF_PATS; i++ ) {

		pendingProgramNumbers[i].setM_value( "0" );
//		pendingPMTPIDs[i].setM_value( "0" );

	}

	for ( i = 0; i < NUMBER_OF_PROGRAMS; i++ ) {

		pendingCAProgramNumbers[i].setM_value( "0" );

	}

	for ( i = 0; i < NUMBER_OF_FIXED_KEYS; i++ ) {

		pendingFKPIDs[i].setM_value( "8191" );
		pendingFKSigns[i].setM_value( "0" );
		pendingFKs[i].setM_value( "0000000000000000" );

	}

	for ( i = 0; i < NUMBER_OF_IP_REMAP_RULES; i++ ) {

		pendingOriginalIPs[i].setM_value( "0.0.0.0" );
		pendingNewIPs[i].setM_value( "0.0.0.0" );
		pendingIPRemapMasks[i].setM_value( "0.0.0.0" );
		pendingIPRemapTTLs[i].setM_value( "0" );
		pendingIPRemapActions[i].setM_value( "No Action" );

	}

	
	for ( i = 0; i < NUMBER_OF_VPROGRAMS; i++ ) {

		pendingVProgramNumbers[i].setM_value( "0" );
		pendingVProgramDestinationIPs[i].setM_value( "0.0.0.0" );
		pendingVProgramDestinationPorts[i].setM_value( "0" );

	}


}



bool S200V::reset()
{
	S200VResetMSG resetMsg;


	resetMsg.Message[0] = 0x19;
	resetMsg.Message[1] = 0x75;
	resetMsg.Message[2] = 0x14;
	resetMsg.Message[3] = 0x03;

	srand( time(NULL) );
	resetMsg.Random = rand();

	digitalsignature( &m_sec, (uint8_t *)(&resetMsg), sizeof( S200VResetMSG ) - 20 );

	memcpy( resetMsg.Signature, m_sec.digest, 20 );

	return sendConfig( I_RESET_PORT, (unsigned char *)&resetMsg, sizeof( S200VResetMSG ) );
}


bool S200V::factoryReset()
{
	S200VDefaultConfigMSG factoryResetMsg;


	factoryResetMsg.configVerificationCode[0] = 0x5555;
	factoryResetMsg.configVerificationCode[1] = 0xaaaa;

	srand( time(NULL) );
	factoryResetMsg.Random = rand();

	digitalsignature( &m_sec, (uint8_t *)(&factoryResetMsg), sizeof( S200VDefaultConfigMSG ) - 20 );

	memcpy( factoryResetMsg.Signature, m_sec.digest, 20 );

	return sendConfig( I_DEFAULT_CONFIG_PORT, (unsigned char *)&factoryResetMsg, sizeof( S200VDefaultConfigMSG ) );
}



bool S200V::changePassword( char *password, unsigned char key[16] )
{
	S200VSetPasswordMSG password_message;
	int i;
	string mac;
	int mac_byte[6];
	char junk[2];


	mac = getParameter( RECEIVER_MAC ).asString();
	sscanf( mac.c_str(), "%2x%c%2x%c%2x%c%2x%c%2x%c%2x",
			&(mac_byte[0]), junk, &(mac_byte[1]), junk, &(mac_byte[2]),
			junk, &(mac_byte[3]), junk, &(mac_byte[4]), junk, &(mac_byte[5]) );
	for ( i = 0; i < 6; i++ ) {
		m_sec.macaddress[i] = (uint8_t)mac_byte[i];
	}
	
	memset( m_sec.password, 0, 8 );
	for ( i = 0; ( i < 8 ) && ( i < (int)strlen( password ) ); i++ ) {
		m_sec.password[i] = password[i];
	}

	// MAC address to msg buffer
	memmove( password_message.newPassword, m_sec.macaddress, sizeof( m_sec.macaddress ) );

	// pad MAC address to 8-bytes
	password_message.newPassword[6] = 0;
	password_message.newPassword[7] = 0;

	// copy password to message buffer
	memmove(&(password_message.newPassword[8]), m_sec.password, 8 );

	srand( time(NULL) );
	password_message.Random = rand();
	
	// compute digital signature on clear text
	// password and MAC address
	digitalsignature(&m_sec, (unsigned char *)(&password_message), sizeof( S200VSetPasswordMSG ) - 20 );
	memmove( password_message.Signature, m_sec.digest, sizeof(m_sec.digest));

	// encrypt MAC address
	encrypt(key, &(password_message.newPassword[0]));

	// encrypt password
	encrypt(key, &(password_message.newPassword[8]));

	return sendConfig( I_SET_PASSWORD_PORT, (unsigned char *)&password_message, sizeof( password_message ) );
}



bool S200V::readVProgramList()
{
	bool success = false;
    S200VVProgramListMSG VProgramListRequest;
	int i;
	unsigned char receive_buffer[1500];
	S200VVProgramListMSG *VProgramListReply = (S200VVProgramListMSG *)receive_buffer;
	char temp[20];


	// Download VProgramList

	memset( (void *)&VProgramListRequest, 0, sizeof( VProgramListRequest ) );
	memset( (void *)receive_buffer, 0, 1500 );

	// Set the opcode to download (0x0001)

	VProgramListRequest.Opcode = htons(0x0001);
	srand( time(NULL) );
	VProgramListRequest.Random = rand();

	digitalsignature( &m_sec, (uint8_t *)(&VProgramListRequest), sizeof( VProgramListRequest ) - 20 );

	memcpy( VProgramListRequest.Signature, m_sec.digest, 20 );
			
	if ( getConfig( I_VIDEO_PROGRAM_PORT, (unsigned char *)&VProgramListRequest, receive_buffer, sizeof( VProgramListRequest ), sizeof( S200VVProgramListMSG ) ) ) {

		switch ( ntohs( VProgramListReply->mode ) ) {

			case 1	:	currentParameters[ FORWARD_ALL_FLAG ]->setM_value( "1" ); 
						currentParameters[ FORWARD_NULLS_FLAG ]->setM_value( "1" ); 
						break;

			case 2	:	currentParameters[ FORWARD_ALL_FLAG ]->setM_value( "1" ); 
						currentParameters[ FORWARD_NULLS_FLAG ]->setM_value( "0" ); 
						break;

			default	:	currentParameters[ FORWARD_ALL_FLAG ]->setM_value( "0" ); 
						currentParameters[ FORWARD_NULLS_FLAG ]->setM_value( "0" ); 
						break;

		}

		sprintf( temp, "%d.%d.%d.%d", 
				 VProgramListReply->destinationIP[0], 
				 VProgramListReply->destinationIP[1], 
				 VProgramListReply->destinationIP[2], 
				 VProgramListReply->destinationIP[3] );

		currentParameters[ STREAM_DESTINATION_IP ]->setM_value( string( temp ) );

		sprintf( temp, "%d", ntohs( VProgramListReply->destinationUDP ) );
		currentParameters[ STREAM_DESTINATION_PORT ]->setM_value( string( temp ) );

		pendingParameters[ FORWARD_ALL_FLAG ]->setM_value( currentParameters[ FORWARD_ALL_FLAG ]->getM_value() );
		pendingParameters[ FORWARD_NULLS_FLAG ]->setM_value( currentParameters[ FORWARD_NULLS_FLAG ]->getM_value() );
		pendingParameters[ STREAM_DESTINATION_IP ]->setM_value( currentParameters[ STREAM_DESTINATION_IP ]->getM_value() );
		pendingParameters[ STREAM_DESTINATION_PORT ]->setM_value( currentParameters[ STREAM_DESTINATION_PORT ]->getM_value() );

		for ( i = 0; i < NUMBER_OF_VPROGRAMS; i++ ) {
		
			sprintf( temp, "%d", ntohs( VProgramListReply->ProgramList[i].programNumber ) );
			currentVProgramNumbers[i].setM_value( string( temp ) );
			pendingVProgramNumbers[i].setM_value( currentVProgramNumbers[i].getM_value() );	

			sprintf( temp, "%d.%d.%d.%d",
					 VProgramListReply->ProgramList[i].destinationIP[0],
					 VProgramListReply->ProgramList[i].destinationIP[1],
					 VProgramListReply->ProgramList[i].destinationIP[2],
					 VProgramListReply->ProgramList[i].destinationIP[3]  );
			currentVProgramDestinationIPs[i].setM_value( string( temp ) );
			pendingVProgramDestinationIPs[i].setM_value( currentVProgramDestinationIPs[i].getM_value() );	

			sprintf( temp, "%d", ntohs( VProgramListReply->ProgramList[i].destinationUDP ) );
			currentVProgramDestinationPorts[i].setM_value( string( temp ) );
			pendingVProgramDestinationPorts[i].setM_value( currentVProgramDestinationPorts[i].getM_value() );	

		}

		readProgramNames();

		success = true;

	}
	
	return success;
}


bool S200V::readStreamInfo()
{
	bool success = false;
	S200VGetProgramInfoMSG ProgramInfoRequest;
	unsigned char receive_buffer[1500];
	S200VProgramInfoReplyMSG *ProgramInfoReply = (S200VProgramInfoReplyMSG *)receive_buffer;
	int i;
	int index;
	int audio_index;
	char temp[20];
	int program_number;
	bool found_program = false;


	memset( (void *)&ProgramInfoRequest, 0, sizeof( ProgramInfoRequest ) );
	memset( (void *)receive_buffer, 0, 1500 );


	// Set the opcode to download (0x0001)

	ProgramInfoRequest.Opcode = htons(0x0001);
	srand( time(NULL) );
	ProgramInfoRequest.Random = rand();

	digitalsignature( &m_sec, (uint8_t *)(&ProgramInfoRequest), sizeof( ProgramInfoRequest ) - 20 );

	memcpy( ProgramInfoRequest.Signature, m_sec.digest, 20 );
			
	if ( getConfig( I_PROGRAM_INFO_PORT, (unsigned char *)&ProgramInfoRequest, receive_buffer, sizeof( ProgramInfoRequest ), sizeof( S200VProgramInfoReplyMSG ) ) ) {

		for ( i = 0; i < NUMBER_OF_VPROGRAMS; i++ ) {

			program_number = ntohs( (ProgramInfoReply->Info[i]).programNumber );
			index = 0;
			found_program = false;
			while ( !found_program && ( index < NUMBER_OF_VPROGRAMS ) ) {
				if ( getIndexedParameter( VPROGRAM_NUMBER, index ).asShort() == program_number ) {
					found_program = true;
				} else {
					index++;
				}
			}

			if ( found_program ) {
				CAProgramScrambledFlags[index].setM_value( itoa( ntohs( (ProgramInfoReply->Info[i]).programScramblingState ), temp, 10 ) );
				CASystemIDs[index].setM_value( itoa( ntohs( (ProgramInfoReply->Info[i]).CaSystemId ), temp, 10 ) );
				PMTPIDs[index].setM_value( itoa( ntohs( (ProgramInfoReply->Info[i]).PMTPID ), temp, 10 ) );
				PMTVersions[index].setM_value( itoa( ntohs( (ProgramInfoReply->Info[i]).PMTversion ), temp, 10 ) );
				ProgramStreamTypes[index].setM_value( itoa( ntohs( (ProgramInfoReply->Info[i]).streamType ), temp, 10 ) );
				VideoPIDs[index].setM_value( itoa( ntohs( (ProgramInfoReply->Info[i]).videoPID ), temp, 10 ) );
				PCRPIDs[index].setM_value( itoa( ntohs( (ProgramInfoReply->Info[i]).PCRPID ), temp, 10 ) );

				for ( audio_index = 0; audio_index < NUMBER_OF_AUDIO_PIDS_PER_PROGRAM; audio_index++ ) {	

					(AudioPIDs[index])[audio_index].setM_value( itoa( ntohs( (ProgramInfoReply->Info[i]).audioPID[audio_index] ), temp, 10 ) );
	
					strncpy( temp, (char *)((ProgramInfoReply->Info[i]).langList[audio_index].languageCode), 3 );
	
					(AudioPIDLanguages[index])[audio_index].setM_value( temp );
	
					(AudioPIDTypes[index])[audio_index].setM_value( itoa( (ProgramInfoReply->Info[i]).langList[audio_index].audioType, temp, 10 ) );

				}

			}

		}

		readProgramNames();

		success = true;

	}

	return success;
}


bool S200V::readProgramNames()
{
//	bool success = false;
    S200VGetPSIMSG PSIRequest;
//	int i;
	unsigned char receive_buffer[1500];
	S200VSITable *siTable = (S200VSITable *)receive_buffer;
	unsigned short section_number = 0;
	int descSize;
	int descUsed;
	unsigned char* ptrDesc;
	char provider_name[128];
	char service_name[128];
	unsigned short program_number;
	int descLoopLength;
	int rc;
	string tag;
	bool scrambled;
	int index;
	int guide_index = 0;
	char temp[20];


	do {
		memset( (void *)&PSIRequest, 0, sizeof( PSIRequest ) );
		memset( (void *)receive_buffer, 0, 1500 );

		PSIRequest.TableID = htons(0x42);
		PSIRequest.SectionNumber = htons(section_number);
		srand( time(NULL) );
		PSIRequest.Random = rand();

		digitalsignature( &m_sec, (uint8_t *)(&PSIRequest), sizeof( PSIRequest ) - 20 );

		memcpy( PSIRequest.Signature, m_sec.digest, 20 );
			
		if ( getConfig( I_PSI_TABLE_PORT, (unsigned char *)&PSIRequest, receive_buffer, sizeof( PSIRequest ), 0 ) ) {

			if ( ntohs( siTable->length ) > 0 ) {
				descSize = ntohs( siTable->length ) - 15;
				ptrDesc = (unsigned char *)(&(siTable->SiTable.SDTtable[3]));
				descUsed  = 0;
				do
				{
					// delete previous strings
					memset( provider_name, 0, 128 );
					memset( service_name, 0, 128 );

					// get service id (aka program number)
					program_number  = ptrDesc[0] << 8;
					program_number |= ptrDesc[1];
								
					if (ptrDesc[3] & 0x10) {
						scrambled = true;
					} else {
						scrambled = false;
					}

					// get descriptor loop length
					descLoopLength = (ptrDesc[3] << 8) &0x0f00;
					descLoopLength |= ptrDesc[4];

					// if loop length == 0, there is no descriptor
					// else parse descriptor
					if (descLoopLength)
					{							
						// parse the service descriptor
						rc = ParseServiceDescriptor( provider_name, service_name, ptrDesc + 5);
						if (rc == false)
							break;
					}

					tag = string( provider_name ) + " - " + string( service_name );
					if ( scrambled ) {
						tag = tag + " (CA)";
					}

					for ( index = 0; index < NUMBER_OF_VPROGRAMS; index++ ) {
						if ( getIndexedParameter( VPROGRAM_NUMBER, index ).asShort() == program_number ) {
							setIndexedParameter( PROGRAM_NAME, index, tag );
						}
					}

					if ( guide_index < NUMBER_OF_PROGRAM_GUIDE_ENTRIES ) {
						guideProgramNumbers[guide_index].setM_value( string( itoa( program_number, temp, 10 ) ) );
						guideProgramNames[guide_index].setM_value( tag );
						guide_index++;
					}

					// point to next descriptor
					ptrDesc  += (descLoopLength + 5);
					descUsed += (descLoopLength + 5);
					
				} while(descUsed < descSize);

			}

			section_number++;

		}

	} while ( section_number < ntohs( siTable->lastSectionNumber ) );

	for ( index = guide_index; index < NUMBER_OF_PROGRAM_GUIDE_ENTRIES; index++ ) {
		guideProgramNumbers[guide_index].setM_value( "0" );
		guideProgramNames[guide_index].setM_value( "" );
	}
	
	return true;
	
}
