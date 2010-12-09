//*****************************************************************************
//*
//* Copyright Novra Technologies Incorporated, 2003
//* 1100-330 St. Mary Avenue
//* Winnipeg, Manitoba
//* Canada R3C 3Z5
//* Telephone (204) 989-4724
//*
//* All rights reserved. The copyright for the computer program(s) contained
//* herein is the property of Novra Technologies Incorporated, Canada.
//* No part of the computer program(s) contained herein may be reproduced or
//* transmitted in any form by any means without direct written permission
//* from Novra Technologies Inc. or in accordance with the terms and
//* conditions stipulated in the agreement/contract under which the
//* program(s) have been supplied.
//*
//*****************************************************************************
//
//
// S75.h : main header file for the S75 Communications Library
//
// -------------------------------------------------------------
//
// Defines the following ...
//--------------------------
//
// Constants:
//-----------
//
//	S75_OK			++ most functions in this library return this constant on success.
//	S75_FAILED		++ most functions in this library return this constant on Failure.
//	SIZE_PID_LIST	++ This is the size of the PID List structure. Devices may support less than this value.
//	MAX_DEVICES		++ This is the maximum number of devices that can be contained in a device List
//
// Types:
//-------
//
//	S75_Handle			++ This is a handle used to manipulate the device.
//	VITERBI_RATES		++ Enumerator for the different viterbi rates
//	NOVRA_DEVICES		++ Enumerator for the different devices that can be identified by this Liberary
//	Novra_DeviceEntry	++ A Structure that holds all the relevant information about the device.
//	Novra_DeviceList	++ A structure that has a list of the devices that were discovered
//
//	S75D_NetSetting		++ The network setting structure for version 2.x devices.
//	S75D_RFSetting		++ The RF settings Structure for the version 2.x devices (can also be used for version 3.x)
//	S75D_PIDList		++ This structure is used to configure PIDs and as a part of the status
//	S75D_Status	   	    ++ This is the status structure for version 2.x devices only.
//
//	S75D_Status_V3		++ This is the status structure for version 3.x devices only.
//	S75D_NetSetting_V3	++ The network setting structure for version 3.x devices.
//	S75D_RFSetting_V3	++ A Rename of the RFsetting structure for consistincy  - prefered for version 3.x
//	S75D_PIDList_V3		++ A Rename of the PIDList structure for consistincy    - prefered for version 3.x
//
//	PesMaker_Status		++ This is the status structure for version 5.x devices only. they forward PES packets an Ts packets 
//	PesMaker_NetSetting	++ The network setting structure for version 5.x devices only. they forward PES packets an Ts packets
//	PesMaker_RFSetting	++ A Rename of the RFsetting structure for consistincy  - prefered for 5.x devices only. thy forward PES packets an Ts packets 
//	PesMaker_PIDList	++ This structure is used to configure PIDs for version 5.x devices only. thye forward PES packets an Ts packets 
//
//	A75_Status			++ Status structure for A75
//	A75_NetSetting		++ Rename for A75 Network Settings.
//	A75_RFSetting		++ Structure that defines the RF Settings for A75
//	A75_PIDList			++ A rename for convienience..
//	A75_Handle			++ A Handle to A75 Devices.
//
//  S75_Status_JLEO       ++ This is the status structure for JLEO devices only.
//  S75_NetSetting_JLEO   ++ The network setting structure for JLEO devices.
//  S75_RFSetting_JLEO    ++ A Rename of the RFsetting structure for consistincy  - prefered for JLEO version
//  S75_PIDList_JLEO      ++ A Rename of the PIDList structure for consistincy    - prefered for JLEO version 
//  S75_TrapSetting_JLEO  ++ The trap setting structure for JLEO devices
//
//
// Error Codes:
//-------------
//
//	The Error Codes used by the library, Some select functions will return the error codes to the caller
//	to assist in trouble shooting and extended user diagonistic messages.
//
// Functions
//----------
//
//	discoverDevices		++ Auto Discovery function
//	OpenDevice		++ Returns a Handle to a device
//	configureNetwork	++ Configure the network settings
//	configureRF		++ Configure the RF Settings
//	setPIDList		++ Configure the PID List
//	getPIDList		++ Retrieve the PID List
//	getStatus		++ Retrieves Status of the Device
//	resetS75		++ Reset teh Device
//	resetCAM		++ Reset the CAM on the device
//	setCAMWatchdogTimeout		++ Set the CAM watchdog timeout on the device
//	getCAMWatchdogTimeout		++ Get the CAM watchdog timeout from the device
//	GetDeviceType		++ Returns teh Device Type (a NovraDevice Type)
//	ForceDevice		++ Reset Network Settings to default -- Should be avoided, use only if the device isn't reachable --
//	GetAvaiablePort		++ Returns an available Status Port on your system. All status ports should be obtained this way
//	SetDVBMACAddress	++ Configure the DVB MAC Address for version 3.x devices.
//	closeS75
//  calcSignalStrengthPercentage
//
//  ----------------------------------------------------------------------------------------------
//
//  These functions are provided for older code, It is strongly recommended that they NOT be used.
//
//	b_openS75		(uses byte arrays for IP and MAC)
//	s_openS75		(uses strings for IP and MAC)
//	b_forceS75		(uses byte arrays for IP and MAC)
//	s_forceS75		(uses strings for IP and MAC)
//
//  ----------------------------------------------------------------------------------------------
//
//JSAT Functions
//--------------

//	discoveRemoteDevice	++ Function to get a handle for a remote device by IP address
//	openRemoteDevice	++ Returns a Handle to a remote device
//	changePassword		++ Changes the password on a device
//	configureTraps		++ Configure the Trap Settings
//	pollStatus		++ Retrieves Status of a Device by request
//	broadcastPollStatus	++ Retrieves Status of Device by request when MAC is known but not IP
//	forceRemoteDevice	++ Reset Network Settings to default -- Should be avoided, use only if the device isn't reachable --
//	specifyPassword         ++ Specifies the password to send to the device when configuring
//
//
//---------------------------------------------------------------------------

/*****************************************************
******************************************************
******************************************************
*** You will need a header file to define the TYPES **
*** Windows.h , or Linux-windowstypes.h, Usually    **
*** The application will include Either.            **
******************************************************
*****************************************************/



// PREPROCESSOR DIRECTIVES

#ifndef __S75
#define __S75

#ifndef LINUX
#ifndef  _WindowsIncluded_
#define	 _WindowsIncluded_
#include <windows.h>
#endif
#else
#include "linux_windows_types.h"
#endif

/** @name General Purpose Types, Structures, Enumeration and Constants
*
* @{
*
*/

// CONSTANTS
//----------

const int S75_OK				= 0;		///< S75_OK is returned by most library functions on success

const int S75_FAILED			= -1;		///< S75_FAILED indicates library function failed 

const int SIZE_PID_LIST			= 32;		///< Standard PID list size (some receivers support only 16)

const int SIZE_PROGRAM_LIST		= 16;		///< Number of DVB-S programs supported by most receivers

const int NUMBER_OF_FIXED_KEYS	= 64;		///< Maximum number of fixed keys that an FK device can hold at once

const int MAX_DEVICES			= 64;		///< The maximum number of devices that can be stored in a Novra_DeviceList

const int NULL_PID				= 0x1FFF;	///< The value of the NULL PID



// TYPE DEFINITIONS

typedef void * S75_Handle;		///< A pointer used in all satellite device communications

typedef S75_Handle  A75_Handle;	///< A pointer used in all ATSC device communications

//----------------------------------------------------------------------------------------

typedef enum NOVRA_DEVICES		/// Used to enumerate the Novra device types
{ 
				S75_ANY,		///< Used by the auto discovery class
				S75_UNKNOWN,	///< A device that reports firmware that is not recognised or bad status packet.
				S75_V_1,		///< S75 Version 1 device, if we ever decide to support them.
				S75D_V21,		///< S75 Version 2.X device.
				S75D_V3,		///< S75 Version 3.X device.
				A75,			///< A75 device.
				PesMaker,		///< S75 Version 5.x PesMaker device.
				S75CA,			///< S75 Conditional Access device.
				A75PesMaker,	///< A75 PesMaker device.
//#ifdef JSAT
				S75_JLEO,		///< S75 JSAT Leo-Palace device.
//#endif
				S75FK,			///< S75 Fixed Key device.
				S75PLUS,		///< S75 Plus device.
				S75CAPRO,		///< S75 Conditional Access device with support for the PRO feature set
				S75FKPRO,		///< S75 Fixed Key device with support for the PRO feature set
				S75PLUSPRO,		///< S75 Plus device with support for the PRO feature set
				S75IPREMAP
};

//----------------------------------------------------------------------------------------

typedef enum VITERBI_RATES		/// Used to enumerate the possible Viterbi rates
{ 
				VITERBI_ERROR,	///< The Viterbi rate is not known
				HALF,			///< 1/2
				TWO_THIRDS,		///< 2/3
				THREE_QUARTERS,	///< 3/4
				FOUR_FIFTHS,	///< 4/5
				FIVE_SIXTHS,	///< 5/6
				SIX_SEVENTHS,	///< 6/7
				SEVEN_EIGTHS,	///< 7/8
				EIGHT_NINTHS	///< 8/9
};

//----------------------------------------------------------------------------------------

#ifndef _AutoDetectPorts_
#define _AutoDetectPorts_

const int num_auto_detect_ports = 10;	///< Novra's device discovery strategy assumes receivers will be sending status on a predefined list of ports

const WORD auto_detect_status_ports[num_auto_detect_ports]	/// Novra's device discovery strategy assumes receivers will be sending status on a predefined list of ports.  This is the list of auto-detection ports... already compiled into the library
= {	

	0x1974,
	0x2075,
	0x2175,
	0x2275,
	0x2375,
	0x2475,
	0x3475,
	0x4475,
	0x4575,
	0x4675

};

#endif

//----------------------------------------------------------------------------------------

typedef struct Novra_DeviceEntry	/// Essential information for talking to a Novra device
{

	BYTE DeviceIP[4];	///< The IP address of the device
	BYTE DeviceMAC[6];	///< The MAC address of the device
	WORD StatusPort;	///< The UDP port that the device is sending status to
	WORD DeviceType;	///< The device type ... one of typedef enum NOVRA_DEVICES

} Novra_DeviceEntry;	

//----------------------------------------------------------------------------------------

// Novra_DeviceList

/**
*
* Pass a pointer to one of these to discoverDevices() and it will be filled in with all the Novra Devices found on the host's subnet.
*
*/

typedef struct Novra_DeviceList /// A list of Novra_DeviceEntry
{

	WORD num_devices;						///< The number of devices in the list
	Novra_DeviceEntry Device[MAX_DEVICES];	///< An array of Novra_DeviceEntry

} Novra_DeviceList;	

//----------------------------------------------------------------------------------------

typedef struct FWD_Info	/// UDP/IP forwarding information for a raw data PID
{

	BYTE	Destination_IP[4];	///< A destination IP address
	WORD	Destination_Port ;	///< A destination UDP port

} FWD_Info;	

//----------------------------------------------------------------------------------------

typedef struct S75_ProgramList	/// A list of DVB programs
{	

	WORD Entry[SIZE_PROGRAM_LIST];	///< Program numbers
	int NumberofPrograms;			///< Number of valid entries in the list
            
} S75_ProgramList;	

/** @} */



/****************************************************************************************

S75 Version 2.X Specific Structures

****************************************************************************************/

/** @name S75 V2.x Structures
 *
 * @{
 *
 */

// S75D_NetSetting
//----------------

/**
*
* Pass a pointer to a populated instance of one of these to configureNetwork() if you are programming an S75 V2.X Device.
*
*/

typedef struct S75D_NetSetting	/// Network Configuration Structure for an S75 Version 2.X device
{

	BYTE DestinationIP[4];		///< IP Address to send raw traffic (TS packets) to
	BYTE SubnetMask[4];			///< Subnet Mask for the device
	BYTE DefaultGatewayIP[4];	///< Default gateway to be used by the device for IP traffic that is not on the same subnet as the device
	WORD DestinationUDP;		///< UDP port to send raw traffic (TS packets) to
	BYTE ReceiverIP[4];			///< The IP address to assign to the receiver.
	WORD StatusPort;			///< The UDP port used for sending status.  It has to be one of the auto detect status ports. The function GetAvailablePort() can be used to get a value for this parameter

} S75D_NetSetting;	

//----------------------------------------------------------------------------------------

// S75D_RFSetting
//---------------

/**
*
* Pass a pointer to a populated instance of one of these to configureRF() if you are programming an S75 V2.X Device.
*
*/

typedef struct S75D_RFSetting	/// RF Configuration Structure for an S75 Version 2.X device
{  

	float frequency;   ///< LBand Frequency in MHz
	float symbolRate;  ///< Symbol Rate in Msps
	int powerOn;       ///< 0 = power to LNB is off, 1 = power to LNB is on
	int band;          ///< 0 = low, 1 = high
	int polarity;      ///< 0 = vertical or right, 1 = horizontal or left

} S75D_RFSetting;	

//----------------------------------------------------------------------------------------

// S75D_PIDList
//-------------

/**
*
* Supports up to 32 PIDs.
*
* Pass a pointer to a populated instance of one of these to setPIDList() if you are programming an S75 V2.X Device.
*
* Pass a pointer to an empty instance of one of these to getPIDList() to have the device fill it in for you.
*
*/

typedef struct S75D_PIDList	/// The PID list structure used for an S75 Version 2.X device
{

	WORD Entry[SIZE_PID_LIST];		///< PID values
	BYTE Control[SIZE_PID_LIST];	///< 0 = pass raw, 1 = MPE Processing
	int  NumberofPIDs;				///< Number of valid entries in the list

} S75D_PIDList;	

//----------------------------------------------------------------------------------------

// S75D_Status
//------------

/**
*
* Pass a pointer to one of these to getStatus() to have it filled in for you.
*
*/

typedef struct S75D_Status	/// The status structure for an S75 Version 2.X device
{

	BYTE 			FirmWareVer;			///< DSP Firmware version 
	BYTE 			FirmWareRev;			///< DSP Firmware revision
	WORD 			RF_FIRMWARE;			///< RF Firmware version and revision
	BYTE 			ReceiverIP[4];			///< The IP address of the receiver
	BYTE 			DestinationIP[4];		///< The destination IP Address for RAW-DVB (forwarded traffic)
	BYTE 			SubnetMask[4];			///< Subnet mask of the device 
	BYTE 			DefaultGatewayIP[4];	///< The default gateway IP address to route traffic to
	WORD 			DestinationUDP;			///< Destination UDP Port for forwarded raw traffic
	BYTE 			ReceiverMAC[6];			///< The MAC Address of the receiver (Etherent MAC)
	BYTE 			RFStatusValid;			///< 0 means next 12 structure members DO NOT contain valid data (SymbolRate, Frequency, Polarity, Band, Power, ViterbiRate, SignalStrength, SignalLock, DataLock, LNBFault, VBER, Uncorrectables)
	float 			SymbolRate;				///< Symbol rate in Msps
	float 			Frequency;				///< LBand frequency in MHz
	BYTE 			Polarity;				///< 0 = vertical or right, 1 = horizontal or left
	BYTE 			Band;					///< 0 = low, 1 = high
	BYTE 			Power;					///< 0 = power to LNB is off, 1 = power to LNB is on
	BYTE 			ViterbiRate;			///< FEC
	BYTE 			SignalStrength;			///< 0-110 percent
	BYTE 			SignalLock;				///< 0 = cannot lock on carrier, 1 = carrier locked
	BYTE 			DataLock;				///< 0 = cannot lock to data rate, 1 = data lock
	BYTE 			LNBFault;				///< 0 = OK, 1 = LNB power short circuited
	float 			VBER;					///< Viterbi Bit Error Rate
	BYTE 			Uncorrectables;			///< Number of uncorrectable DVB packets received since last status packet
	S75D_PIDList 	PIDList;				///< The PID list 
	BYTE 			Reserved[20];			///< Reserved field for future extentions
	BYTE 			UID[32];				///< Unit Identification Code.
	WORD 			EthernetTransmit;		///< Number of ethernet packets transmitted
	WORD 			EthernetReceive;		///< Number of ethernet packets Received
	WORD 			EthernetTransmitError;	///< Number of ethernet errors during transmit.
	WORD 			EthernetPacketDropped;	///< Number of ethernet packets dropped by the receiver
	WORD 			WRESERVED[5];			///< Reserved fields. Undocumented/unsupported

} S75D_Status;	

/** @} */


/****************************************************************************************

S75 Version 3 Specific Structures

****************************************************************************************/

/** @name S75 V3 Structures
 *
 * @{
 *
 */

typedef  S75D_PIDList	S75D_PIDList_V3;	///< The S75 V3 PID list is the same as the S75 V2 except that 3.x devices support only 16 PIDs.

//----------------------------------------------------------------------------------------

// S75D_NetSetting_V3
//-------------------

/**
*
* Pass a pointer to a populated instance of one of these to configureNetwork() if you are programming an S75 V3 Device.
*
*/

typedef struct S75D_NetSetting_V3	/// Network Configuration Structure for an S75 Version 3 device
{

	BYTE UniCastStatusDestIP[4];	///< The destination IP for unicast status messages.  Having a non unicast address here will disable the unicast status messages.
	BYTE SubnetMask[4];				///< Subnet Mask for the device
	BYTE DefaultGatewayIP[4];		///< Default gateway to be used by the device for IP traffic that is not on the same subnet as the device
	WORD UniCastStatusDestUDP;		///< The UDP port where unicast status packets are to be sent
	BYTE ReceiverIP[4];				///< The IP address to assign to the receiver.
	WORD BCastStatusPort;			///< The UDP port where broadcast status packets are to be sent
	BYTE IGMP_Enable;				///< 0 = The device will forward all multicast traffic. 1 = The device will use IGMP to determine which multicasts to forward.

} S75D_NetSetting_V3;	

//----------------------------------------------------------------------------------------

// S75D_RFSetting_V3
//------------------

/**
*
* Pass a pointer to a populated instance of one of these to configureRF() if you are programming an S75 V3 Device.
*
*/

typedef	 struct S75D_RFSetting_V3	/// RF Configuration Structure for an S75 Version 3 device
{

	float	frequency;				///< LBand frequency in MHz
	float	symbolRate;				///< Symbol rate in Msps
	int		powerOn;				///< 0 = power to LNB is off, 1 = power to LNB is on
	int		band;					///< 0 = low, 1 = high
	int		polarity;				///< 0 = vertical or right, 1 = horizontal or left
	BYTE	HighVoltage;			///< 0 = LNB high voltage of 18V, 1 = LNB high voltage of 20V
	BYTE	LongLineCompensation;   ///< Long Line Compensation 0 = off, 1 = on

} S75D_RFSetting_V3;	

//----------------------------------------------------------------------------------------

// S75D_Status_V3
//---------------

/**
*
* Pass a pointer to one of these to getStatus() to have it filled in for you.
*
*/

typedef struct S75D_Status_V3	/// The status structure for an S75 Version 3 device
{

	BYTE		FirmWareVer;			///< DSP Firmware version
	BYTE		FirmWareRev;			///< DSP Firmware revision
	WORD		RF_FIRMWARE;			///< RF Firmware version and revision
	BYTE		IGMP_Enable;			///< 0 = forwarding all multicast, 1 = using IGMP
	BYTE		ReceiverIP[4];			///< The IP address of the receiver
	BYTE		UniCastStatusDestIP[4];	///< The destination IP address for unicast status messages
	BYTE		SubnetMask[4];			///< Subnet mask of the device
	BYTE		DefaultGatewayIP[4];	///< The default gateway IP address to route traffic to
	WORD		UniCastStatusDestUDP;	///< Destination UDP port for unicast status messages
	BYTE		ReceiverMAC[6];			///< The MAC address of the receiver (Ethernet MAC)
	BYTE 		RFStatusValid;			///< 0 means next 14 structure members DO NOT contain valid data (SymbolRate, Frequency, Polarity, Band, Power, ViterbiRate, Long_Line_Comp, Hi_VoltageMode, SignalStrength, SignalLock, DataLock, LNBFault, VBER, Uncorrectables)
	float		SymbolRate;				///< Symbol rate in Msps
	float		Frequency;				///< LBand frequency in MHz
	BYTE		Polarity;				///< 0 = vertical or rights, 1 = horizontal or left
	BYTE		Band;					///< 0 = low, 1 = high
	BYTE		Power;					///< 0 = power to LNB is off, 1 = power to LNB is on
	BYTE		ViterbiRate;			///< FEC
	BYTE		Long_Line_Comp;			///< Long Line Compensation for losses in coax. 0 = off, 1 = on
	BYTE		Hi_VoltageMode;			///< High Voltage Option, 0 = off, 1 = on
	BYTE		SignalStrength;			///< 0-110 percent
	BYTE		SignalLock;				///< 0 = cannot lock on carrier, 1 = carrier locked
	BYTE		DataLock;				///< 0 = cannot lock to data rate, 1 = data lock
	BYTE		LNBFault;				///< 0 = OK, 1 = LNB power short circuited
	float		VBER;					///< Viterbi Bit Error Rate
	BYTE		Uncorrectables;			///< Number of uncorrectable DVB packets received since last status packet
	S75D_PIDList	PIDList;			///< The PID List
	BYTE		DVB_MACADDRESS[6];		///< The DVB MAC address
	BYTE		Reserved[20];			///< Reserved field for future extentions
	BYTE		UID[32];				///< Unit Identification Code
	WORD		EthernetTransmit;		///< Number of ethernet packets transmitted
	WORD		EthernetReceive;		///< Number of ethernet packets received
	WORD		EthernetTransmitError;	///< Number of ethernet errors during transmit
	WORD		EthernetPacketDropped;	///< Number of ethernet packet dropped by the receiver
	WORD		WRESERVED[5];			///< Reserved fields. Undocumented/unsupported

} S75D_Status_V3;	

/** @} */


/****************************************************************************************

A75 Specific Structures

****************************************************************************************/

/** @name A75 Structures
 *
 * @{
 *
 */

typedef	 S75D_NetSetting_V3		A75_NetSetting;	///< The network setting for an A75 is the same as for an S75 V3.

typedef  S75D_PIDList_V3		A75_PIDList;	///< The PID list for an A75 is the same as for an S75 V3.

//----------------------------------------------------------------------------------------

// A75_RFSetting
//--------------

/**
*
* Pass a pointer to a populated instance of one of these to configureRF() if you are programming an A75.
*
*/

typedef struct A75_RFSetting	/// RF Configuration Structure for an A75 device
{

	WORD ChannelNumber;	///< The channel number to tune to

} A75_RFSetting;	

//----------------------------------------------------------------------------------------

// A75_Status
//-----------

/**
*
* Pass a pointer to one of these to getStatus() to have it filled in for you.
*
*/

typedef struct A75_Status	/// The status structure for an A75 device
{

	BYTE		FirmWareVer;			///< DSP Firmware version
	BYTE		FirmWareRev;			///< DSP Firmware revision
	WORD		RF_FIRMWARE;			///< RF Firmware version and revision
	BYTE		IGMP_Enable;			///< 0 = forwarding all multicast, 1 = using IGMP
	BYTE		ReceiverIP[4];			///< The IP address of the receiver
	BYTE		UniCastStatusDestIP[4];	///< The destination IP address for unicast status messages
	BYTE		SubnetMask[4];			///< Subnet mask of the device
	BYTE		DefaultGatewayIP[4];	///< The default gateway IP address to route traffic to
	WORD		UniCastStatusDestUDP;	///< Destination UDP port for unicast status messages
	BYTE		ReceiverMAC[6];			///< The MAC address of the receiver (Ethernet MAC)
	BYTE		RFStatusValid;			///< 0 means next 18 structure members DO NOT contain valid data (ChannelNumber, DeModStats, VSBMode, SER_1, SER_10, SNR_BE, SNR_AE, Carrier_Off, IF_AGC, RFAGC, FEC_UnErr, FEC_Corr, FEC_UnCorr, FieldLocked, EqualizerState, ChannelState, NTSC_Filter, InverseSpectrum)
	WORD		ChannelNumber;			///< Channel number the device is tuned to
	BYTE		DeModStats;				///< Demodulator status, bit 0 (Lock), bit 1 (channel equalizer on/off), bits 2-3 (channel state 0 = dynamic, 1 = static, 2 = fast dynamic), bit 4 (NTSC filter on/off), bit 7 (spectrum inverted/not inverted).  Repeated in structure members FieldLocked, EqualizerState, ChannelState, NTSC_Filter and InverseSpectrum)
	BYTE		VSBMode;				///< modulation order, always 8 VSB with an A75
	BYTE		SER_1;					///< 1 second symbol error rate
	BYTE		SER_10;					///< 10 second symbol error rate
	float		SNR_BE;					///< SNR before equalization
	float		SNR_AE;					///< SNR after equalisation.
	float		Carrier_Off;			///< Carrier offset in Hz
	float		IF_AGC;					///< AGC of IF amp
	float		RFAGC;					///< AGC of RF amp
	WORD		FEC_UnErr;				///< FEC Unerrorred (number of frames that had errors removed but not guaranteed correct)
	WORD		FEC_Corr;				///< FEC Corrected (number of frames corrected)
	WORD		FEC_UnCorr;				///< FEC Uncorrected (number of frames that couldn't be corrected)
	bool		FieldLocked;			///< Field locked (true or false)
	bool		EqualizerState;			///< Equalizer state (True = on, False = off)
	BYTE		ChannelState;			///< 0:Dynamic, 1:Static, 2:Fast Dynamic
	bool		NTSC_Filter;			///< True: ON, False: OFF 
	bool		InverseSpectrum;		///< True: Spectrum inverted, False: Not inverted
	S75D_PIDList	PIDList;			///< The PID List
	BYTE		Reserved[20];			///< Reserved field for future extentions
	BYTE		DVB_MACADDRESS[6];		///< The DVB MAC address
	BYTE		UID[32];				///< Unit Identification Code
	WORD		EthernetTransmit;		///< Number of ethernet packets transmitted
	WORD		EthernetReceive;		///< Number of ethernet packets received
	WORD		EthernetTransmitError;	///< Number of ethernet errors during transmit
	WORD		EthernetPacketDropped;	///< Number of ethernet packet dropped by the receiver
	WORD		WRESERVED[5];			///< Reserved fields. Undocumented/unsupported

} A75_Status;	

/** @} */


/****************************************************************************************

PesMaker Specific Structures

****************************************************************************************/

/** @name PesMaker Structures
 *
 * @{
 *
 */

typedef	 S75D_NetSetting_V3	PesMaker_NetSetting;	///< The network setting for the PesMaker is the same as for the S75 V3.

typedef	 S75D_RFSetting_V3	PesMaker_RFSetting;		///< The RF setting for the PesMaker is the same as for the S75 V3.

//----------------------------------------------------------------------------------------

// PesMaker_PIDList
//-----------------

/**
*
* Supports up to 32 PIDs.
*
* Pass a pointer to a populated instance of one of these to setPIDList() if you are programming a PesMaker device.
*
* Pass a pointer to an empty instance of one of these to getPIDList() to have the device fill it in for you.
*
*/

typedef struct PesMaker_PIDList	/// The PID list structure used for an S75 Version 5.X device (PesMaker)
{	
											
	WORD		Entry[SIZE_PID_LIST];				///< PID values
	FWD_Info	ForwardInformation[SIZE_PID_LIST];	///< Forwarding information for individual PIDs
	BYTE		Control[SIZE_PID_LIST];				///< 0 = pass raw, 1 = MPE Processing, 2 = Pass PES content
	int			NumberofPIDs;						///< Number of valid entries in the list

} PesMaker_PIDList;	

//----------------------------------------------------------------------------------------

// PesMaker_Status
//----------------

/**
*
* Pass a pointer to one of these to getStatus() to have it filled in for you.
*
*/

typedef struct PesMaker_Status	/// The status structure for an S75 Version 5.x device (PesMaker)
{ 

	BYTE			FirmWareVer;			///< DSP Firmware version
	BYTE			FirmWareRev;			///< DSP Firmware revision
	WORD			RF_FIRMWARE;			///< RF Firmware version and revision
	BYTE			IGMP_Enable;			///< 0 = forwarding all multicast, 1 = using IGMP
	BYTE			ReceiverIP[4];			///< The IP address of the receiver
	BYTE			UniCastStatusDestIP[4];	///< The destination IP address for unicast status messages
	BYTE			SubnetMask[4];			///< Subnet mask of the device
	BYTE			DefaultGatewayIP[4];	///< The default gateway IP address to route traffic to
	WORD			UniCastStatusDestUDP;	///< Destination UDP port for unicast status messages
	BYTE			ReceiverMAC[6];			///< The MAC address of the receiver (Ethernet MAC)
	BYTE			RFStatusValid;			///< 0 means next 14 structure members DO NOT contain valid data (SymbolRate, Frequency, Polarity, Band, Power, Long_Line_Comp, Hi_VoltageMode, Viterbi_Rate, SignalStrength, SignalLock, DataLock, LNBFault, VBER, Uncorrectables)
	float			SymbolRate;				///< Symbol rate in Msps
	float			Frequency;				///< LBand frequencty in MHz
	BYTE			Polarity;				///< 0 = vertical or right, 1 = horizontal or left
	BYTE			Band;					///< 0 = low, 1 = high
	BYTE			Power;					///< 0 = power to LNB is off, 1 = power to LNB is on
	BYTE			Long_Line_Comp;			///< Long Line Compensation for losses in coax. 0 = off, 1 = on
	BYTE			Hi_VoltageMode;			///< High Voltage Option, 0 = off, 1 = on
	BYTE			ViterbiRate;			///< FEC
	BYTE			SignalStrength;			///< 0-110 percent
	BYTE			SignalLock;				///< 0 = cannot lock on carrier, 1 = carrier locked
	BYTE			DataLock;				///< 0 = cannot lock to data rate, 1 = data lock
	BYTE			LNBFault;				///< 0 = OK, 1 = LNB power short circuited
	float			VBER;					///< Viterbi Bit Error Rate
	BYTE			Uncorrectables;			///< Number of uncorrectable DVB packets received since last status packet
	S75D_PIDList	PIDList;				///< The PID List.  Note that this list does not contain PID forwarding information.  Use the function getPIDList() to get forwarding information.
	BYTE			Reserved[20];			///< Reserved field for future extentions
	BYTE			DVB_MACADDRESS[6];		///< The DVB MAC address
	BYTE			UID[32];				///< Unit Identification Code
	WORD			EthernetTransmit;		///< Number of ethernet packets transmitted
	WORD			EthernetReceive;		///< Number of ethernet packets received
	WORD			EthernetTransmitError;	///< Number of ethernet errors during transmit.
	WORD			EthernetPacketDropped;	///< Number of ethernet packets dropped by the receiver
	WORD			WRESERVED[5];			///< Reserved fields.  Undocumented/unsupported

} PesMaker_Status;	

/** @} */




/****************************************************************************************

A75PesMaker Specific Structures

****************************************************************************************/

/** @name A75PesMaker Structures
 *
 * @{
 *
 */

typedef S75D_NetSetting_V3	A75PesMaker_NetSetting;	///< The A75PesMaker network setting is the same as the S75 V3.

typedef A75_RFSetting		A75PesMaker_RFSetting;	///< The A75PesMaker RF setting is the same as the A75.

typedef PesMaker_PIDList	A75PesMaker_PIDList;	///< The A75PesMaker PID list is the same as the S75 V5.X PesMaker.

typedef A75_Status			A75PesMaker_Status;		///< The A75PesMaker status is the same as the A75 status.

/** @} */



/****************************************************************************************

S75JLEO Specific Structures

****************************************************************************************/

/** @name S75 JLeo Structures
 *
 * @{
 *
 */

typedef S75D_PIDList	S75_PIDList_JLEO;	///< The S75JLEO PID list is the same as the S75 V2.

//----------------------------------------------------------------------------------------

// S75_NetSetting_JLEO
//--------------------

/**
*
* Pass a pointer to a populated instance of one of these to configureNetwork() if you are programming an S75 JLeo Device.
*
*/

typedef S75D_NetSetting_V3	S75_NetSetting_JLEO;	///< The S75Jleo network setting is the same as the S75 V3.


//----------------------------------------------------------------------------------------

// S75_RFSetting_JLEO
//-------------------

/**
*
* Pass a pointer to a populated instance of one of these to configureRF() if you are programming an S75 JLeo Device.
*
*/

typedef	 struct S75_RFSetting_JLEO /// RF Configuration Structure for an S75 JLeo device
{

	float	frequency;				///< LBand frequency in MHz
	float	symbolRate;				///< Symbol rate in Msps
	int		powerOn;				///< 0 = power to LNB is off, 1 = power to LNB is on
	int		band;					///< 0 = low, 1 = high
	int		polarity;				///< 0 = vertical or right, 1 = horizontal or left
	BYTE	HighVoltage;			///< 0 = LNB high voltage of 18V, 1 = LNB high voltage of 20V
	BYTE	LongLineCompensation;	///< Long Line Compensation 0 = off, 1 = on
	int		LocalOscFrequency;		///< LNB Local Oscillator Frequency


} S75_RFSetting_JLEO  ;

//----------------------------------------------------------------------------------------

// S75_TrapSetting_JLEO
//---------------------

/**
*
* Pass a pointer to a populated instance of one of these to configureTraps() if you are programming an S75 JLeo Device.
*
*/

typedef struct S75_TrapSetting_JLEO	/// The trap setting structure for JLEO devices
{

	WORD			MonitoringInterval;				///< Monitoring Interval in seconds
	BYTE			NotificationIP[4];				///< IP address to send traps to
	WORD			NotificationPort;				///< Port to send traps to
	bool			DataLockMonitorOn;				///< Data lock monitor on/off
	bool			EthernetTxErrorOn;				///< Ethernet TX Error monitor on/off
	bool			TSErrorOn;						///< Transport stream error monitor on/of
	bool			VBEROn;							///< Viterbi Bit Error Rate Monitor on/off
	WORD			VBERThreshold;					///< Viterbi Bit Error Rate threshold
	unsigned long	UncorrectableTSErrorThreshold;	///< Uncorrectable transport stream error threshold
	WORD			EthernetTxErrorThreshold;		///< Ethernet TX error threshold
	
} S75_TrapSetting_JLEO;

//----------------------------------------------------------------------------------------

// S75_Status_JLEO
//-----------------

/**
*
* Pass a pointer to one of these to getStatus() to have it filled in for you.
*
*/

typedef struct S75_Status_JLEO	/// Status Structure used by JLEO devices
{

	BYTE				FirmWareVer;					///< DSP firmware version
	BYTE				FirmWareRev;					///< DSP firmware revision
	WORD				RF_FIRMWARE;					///< RF firmware ver/rev
	BYTE				IGMP_Enable;					///< State of the IGMP filter 0=off, 1=on
	BYTE				ReceiverIP[4];					///< The receiver's IP address
	BYTE				UniCastStatusDestIP[4];			///< The destination for unicast status messages
	BYTE				SubnetMask[4];					///< The subnet mask of the device
	BYTE				DefaultGatewayIP[4];			///< The default gateway ip address
	WORD				UniCastStatusDestUDP;			///< Unicast status message destination UDP port
	BYTE				ReceiverMAC[6];					///< the MAC address of the receiver.
	BYTE				RFStatusValid;					///< 0 means next 12 structure members DO NOT contain valid data
	float				SymbolRate;						///< Symbol rate in Msps
	float				Frequency;						///< LBand frequency in MHz
	BYTE				Polarity;						///< 0 = vertical or right,	1 = horizontal or left
	BYTE				Band;							///< 0 = low, 1 = high
	BYTE				Power;							///< 0 = power to LNB is off, 1 = power to LNB is on
	BYTE				ViterbiRate;					///< FEC
	BYTE				Long_Line_Comp;					///< Long line compensation for losses in coax. 0 = off, 1 = on
	BYTE				Hi_VoltageMode;					///< High voltage option; 0 = off, 1 = on 
	BYTE				AutomaticGainControl;			///< 0-110
	BYTE				SignalLock;						///< 0 = cannot lock on carrier, 1 = carrier locked
	BYTE				DataLock;						///< 0 = cannot lock to data rate, 1 = data locked
	BYTE				LNBFault;						///< 0 = OK, 1 = LNB power short circuited
	float				VBER;							///< Viterbi Bit Error Rate
	float				CarrierToNoise;					///< C/N
	BYTE				Uncorrectables;					///< Number of uncorrectable DVB packets received since last status packet
	BYTE				DemodulatorGain;				///< Demodulator Gain
	S75D_PIDList		PIDList	;						///< the PID List
	BYTE				DVB_MACADDRESS[6];				///< The DVBMACADDRESS (unused; only3.X support setting DVB MAC)
	BYTE				Reserved[20];					///< Reserved field for future extentions
	BYTE				UID[32];						///< Unit identification code.
	WORD				EthernetTransmit;				///< Number of ethernet packets transmitted (this status interval)
	WORD				EthernetReceive;				///< Number of ethernet packets received (this status interval)
	WORD				EthernetTransmitError;			///< Number of ethernet errors during transmit (this status interval)
	WORD				EthernetPacketDropped;			///< Number of ethernet packet dropped by DSP (this status interval)
	WORD				EthernetReceiveError;			///< Ethernet receive error (this status interval)
	WORD				FrontEndLockLoss;				///< FEL loss count (this status interval)
	WORD				DataSyncLoss;					///< Data sync loss count (this status interval)
	WORD				DVBAccepted;					///< Number of DVB packets accepted (this status interval)
	unsigned long		TotalDVBPacketsAccepted[2];		///< Total DVB packets accepted
	unsigned long		TotalDVBPacketsRXInError[2];	///< Total DVB packets received in error
	unsigned long		TotalEthernetPacketsOut[2];		///< Total ethernet packets sent
	unsigned long		TotalUncorrectableTSPackets[2];	///< Total uncorrectable transport stream packets
	WORD				BroadcastStatusPort;			///< UDP port where status is being broadcast
	WORD				LocalOscFrequency;				///< LO frequency of the LNB
		
} S75_Status_JLEO;

/** @} */

/****************************************************************************************

S75Plus Specific Structures

****************************************************************************************/

/** @name S75Plus Structures
 *
 * @{
 *
 */

#define NUM_S75PRO_DESTINATIONS 16	///< S75 Plus based receivers that support the PRO feature set can forward each PID to several destinations

#ifndef PID_ROUTE
	#define PID_ROUTE

	typedef struct PIDRoute /// Identifies where a PID should be sent. Each bit in the DestinationMask corresponds to an entry in the destination list. A bit value of 0 means the PID does not go to the corresponding destination; while a 1 means it does.
	{
		WORD PID;				///< The PID value
		WORD DestinationMask;	///< The destination mask; msb refers to destination index 0, lsb refers to destination index 15 
	} PIDRoute;


	typedef struct PIDDestination	/// A destination where a PID should be sent
	{
		BYTE DestinationIP[4];	///< Destination IP address
		WORD DestinationUDP;	///< Destination port
	} PIDDestination;

#endif


typedef	S75D_NetSetting_V3	S75Plus_NetSetting;	///< The network setting for the S75Plus is the same as for the S75 V3.

// S75Plus_RFSetting
//-------------------

/**
*
* Pass a pointer to a populated instance of one of these to configureRF() if you are programming an S75Plus Device.
*
*/

typedef	 struct S75Plus_RFSetting /// RF Configuration Structure for an S75 Plus device
{

	float	frequency;					///< LBand frequency in MHz
	float	symbolRate;					///< Symbol rate in Msps
	int		powerOn;					///< 0 = power to LNB is off, 1 = power to LNB is on
	int		band;						///< 0 = low, 1 = high
	int		polarity;					///< 0 = vertical or right, 1 = horizontal or left
	BYTE	HighVoltage;				///< 0 = LNB high voltage off, 1 = LNB high voltage on
	BYTE	LongLineCompensation;		///< Long Line Compensation 0 = off, 1 = on
	BYTE	PolaritySwitchingVoltage;	///< 0 = 11-15V, 1= 13-18V (12-20V if HighVolatge on)
	BYTE	HiLoBandSwitchingTone;		///< 0 = 22kHz, 1 = 44kHz
	int		LocalOscFrequency;			///< LNB Local Oscillator Frequency


} S75Plus_RFSetting;

//----------------------------------------------------------------------------------------

// S75Plus_PIDList
//-----------------

/**
*
* Supports up to 32 PIDs.
*
* Pass a pointer to a populated instance of one of these to setPIDList() if you are programming an S75Plus device.
*
* Pass a pointer to an empty instance of one of these to getPIDList() to have the device fill it in for you.
*
*/

typedef struct S75Plus_PIDList	/// The PID list structure used for an S75Plus device.  This structure is also used by devices that support the Pro feature set.  Devices that don't support the Pro feature set will ignore the destination information and will assume all control flags are set to 1 (MPE)
{
											
	WORD			PID[SIZE_PID_LIST];						///< PID values
	BYTE			Control[SIZE_PID_LIST];					///< 0 = pass raw (Pro feature), 1 = MPE Processing
	WORD			DestinationMask[SIZE_PID_LIST];			///< PID to Destination map (Pro feature); leftmost bit refers to Destination[0]
	PIDDestination	Destination[NUM_S75PRO_DESTINATIONS];	///< Destination IP and port (Pro feature)

} S75Plus_PIDList;	


//----------------------------------------------------------------------------------------

// S75Plus_Status
//----------------

/**
*
* Pass a pointer to one of these to getStatus() to have it filled in for you.
*
*/


typedef struct S75Plus_Status 	/// The status structure for an S75Plus device
{

  BYTE FirmWareVer;  ///< DSP Firmware version
  BYTE FirmWareRev;  ///< DSP Firmware revision
  WORD RF_FIRMWARE;  ///< RF Firmware version and revision
  BYTE IGMP_Enable;  ///< 0 = forwarding all multicast, 1 = using IGMP
  BYTE ReceiverIP[4]; ///< The IP address of the receiver
  BYTE UniCastStatusDestIP[4]; ///< The destination IP address for unicast status messages
  BYTE SubnetMask[4];	      ///< Subnet mask of the device
  BYTE DefaultGatewayIP[4];  ///< The default gateway IP address to route traffic to
  WORD UniCastStatusDestUDP; ///< Destination UDP port for unicast status messages
  BYTE ReceiverMAC[6]; ///< The MAC address of the receiver (Ethernet MAC)
  BYTE RFStatusValid; ///< 0 means next 19 structure members DO NOT contain valid data
  /*
   * (SymbolRate, Frequency, Polarity, Band, Power, Long_Line_Comp, Hi_VoltageMode,
   * PolaritySwitchingVoltage, HiLoBandSwitchingTone, Viterbi_Rate, SignalStrength,
   * SignalLock, DataLock, LNBFault, VBER, AGCA, AGCN, GNYQA, GRARA, Uncorrectables)
   */
  float			SymbolRate;									///< Symbol rate in Msps
  float			Frequency;									///< LBand frequency in MHz
  BYTE			Polarity;									///< 0 = vertical or right, 1 = horizontal or left
  BYTE			Band;										///< 0 = low, 1 = high
  BYTE			Power;										///< 0 = power to LNB is off, 1 = power to LNB is on
  BYTE			ViterbiRate;								///< FEC
  BYTE			Long_Line_Comp;								///< Long Line Compensation for losses in coax. 0 = off, 1 = on
  BYTE			Hi_VoltageMode;								///< High Voltage Option, 0 = off, 1 = on (subtracts 1 Volt from low and adds 2 Volts to high polarity switching voltages)
  BYTE			PolaritySwitchingVoltage;					///< 0 = 11-15 Volts, 1 = 13-18 Volts
  BYTE			HiLoBandSwitchingTone;						///< 0 = 22kHz tone, 1 = 44 kHz tone for high band signal
  BYTE			SignalStrength;								///< 0-110 percent
  BYTE			SignalLock;									///< 0 = cannot lock on carrier, 1 = carrier locked
  BYTE			DataLock;									///< 0 = cannot lock to data rate, 1 = data lock
  BYTE			LNBFault;									///< 0 = OK, 1 = LNB power short circuited
  float			VBER;										///< Viterbi Bit Error Rate
  BYTE			AGCA;										///< Analog AGC
  BYTE			AGCN;										///< Digital AGC
  BYTE			GNYQA;										///< The Nyquist filter gain
  BYTE			GFARA;										///< Antia-alias filter gain
  BYTE			NEST;										///< Noise Estimate
  BYTE			Clock_Off;									///< Clock Offset
  WORD			Freq_Err;									///< Frequency Error
  BYTE			ADC_MID;									///< MID point crossing for ADC
  BYTE			ADC_CLIP;									///< ADC clipping count
  BYTE			Digital_Gain;								///< Digital Gain
  BYTE			AA_CLAMP;									///< Clamping count
  float			CarrierToNoise;								///< C/N
  BYTE			Uncorrectables;								///< Number of uncorrectable DVB packets received since last status packet
  BYTE			DemodulatorGain;							///< Demodulator Gain
  WORD			CardStatus;									///< CAM Status 0 = no card, 1 = card inserted, 3 = card initialized, 7 = ready to decrypt
  BYTE			UID[32];									///< Unit Identification Code.
  WORD			EthernetTransmit;							///< Number of ethernet packets transmitted (this status interval)
  WORD EthernetReceive;							///< Number of ethernet packets received (this status interval)
  WORD DVBScrambled;								///< Number of DVB packets still scrambled (this status interval)
  WORD EthernetPacketDropped;						///< Number of ethernet packet dropped by the receiver (this status interval)
  WORD EthernetReceiveError;						///< Number of ethernet packets received in error (this status interval)
  WORD DVBDescrambled;								///< Number of descrambled DVB packets (this status interval)
  WORD DataSyncLoss;								///< Number of times data sync lost (this status interval)
  WORD DVBAccepted;								///< Number of DVB packets accepted (this status interval)
  unsigned long	TotalDVBPacketsAccepted[2];					///< Total number of DVB packets accepted
  unsigned long	TotalDVBPacketsRXInError[2];				///< Total number of DVB packets received in error
  unsigned long	TotalEthernetPacketsOut[2];					///< Total number of ethernet packets sent
  unsigned long	TotalUncorrectableTSPackets[2];				///< Total number of uncorrectable transport stream packets
  WORD BroadcastStatusPort;						///< The UDP port being used for broadcast status messages
  WORD LocalOscFrequency;							///< The LO frequency of the LNB
  S75D_PIDList	PIDList;									///< List of PIDs being forwarded by the receiver
  PIDRoute PIDRoutes[32];								///< PID routing flags (Pro feature)
  PIDDestination PIDDestinations[NUM_S75PRO_DESTINATIONS];	///< PID destinations (Pro feature)
  WORD ProgramList[16];							///< List of DVB programs numbers (Conditional Access receivers)
  WORD ProgramStatus[16];							///< DVB program status (Conditional Access receivers); 0 = N/A, 1 = Program doesn't exist, 2 = Trying PMT, 3 = Descrambling
  
} S75Plus_Status;	

/** @} */

/****************************************************************************************

S75PlusPro Specific Structures

****************************************************************************************/

/** @name S75PlusPro Structures
 *
 * @{
 *
 */


typedef	S75Plus_NetSetting	S75PlusPro_NetSetting;	///< The network setting for the S75PlusPro is the same as for the S75Plus.

typedef	S75Plus_RFSetting S75PlusPro_RFSetting;		///< The RF setting for the S75PlusPro is the same as for the S75Plus.

typedef S75Plus_PIDList S75PlusPro_PIDList;			///< The PID setting for the S75PlusPro is the same as for the S75Plus.

typedef S75Plus_Status S75PlusPro_Status;			///< The status for the S75PlusPro is the same as for the S75Plus.


//----------------------------------------------------------------------------------------

// S75PlusPro_PAT
//----------------

/**
*
* Supports up to 16 programs.
*
* Pass a pointer to one of these setPAT() if you are programming an S75PlusPro device.
* This structure will also be returned by getPAT()
*
*/

typedef struct S75PlusPro_PAT	/// The PAT structure for an S75PlusPro device
{

	WORD	ProgramNumber[NUM_S75PRO_DESTINATIONS];	///< DVB program numbers
	WORD	PMTPID[NUM_S75PRO_DESTINATIONS];		///< PMT PIDs

} S75PlusPro_PAT;	


//----------------------------------------------------------------------------------------
// S75PlusPro_TrapSetting
//------------------------

/**
*
* Pass a pointer to a populated instance of one of these to setTraps() or getTraps() if you are programming an S75PlusPro.
*
* NOTE: THIS FEATURE IS NOT CURRENTLY SUPPORTED IN DEVICE FIRMWARE
*
*/

typedef struct S75PlusPro_TrapSetting	/// The trap setting structure for S75PlusPro device.  NOTE: THIS FEATURE IS NOT CURRENTLY SUPPORTED IN DEVICE FIRMWARE.
{ 

	WORD			MonitoringInterval;				///< Monitoring Interval in seconds
	BYTE			NotificationIP[4];				///< IP address to send traps to
	WORD			NotificationPort;				///< Port to send traps to
	bool			DataLockMonitorOn;				///< Data lock monitor on/off
	bool			EthernetTxErrorOn;				///< Ethernet TX Error monitor on/off
	bool			TSErrorOn;						///< Transport stream error monitor on/of
	bool			VBEROn;							///< Viterbi Bit Error Rate Monitor on/off
	WORD			VBERThreshold;					///< Viterbi Bit Error Rate threshold
	unsigned long	UncorrectableTSErrorThreshold;	///< Uncorrectable transport stream error threshold
	WORD			EthernetTxErrorThreshold;		///< Ethernet TX error threshold
	
} S75PlusPro_TrapSetting;

/** @} */

/****************************************************************************************

S75CA Specific Structures

****************************************************************************************/

/** @name S75CA Structures
 *
 * @{
 *
 */


typedef	S75Plus_NetSetting S75CA_NetSetting;	///< The network setting for the S75CA is the same as for the S75Plus.

typedef	S75Plus_RFSetting S75CA_RFSetting;		///< The RF setting for the S75CA is the same as for the S75Plus.

typedef S75Plus_PIDList S75CA_PIDList;			///< The PID setting for the S75CA is the same as for the S75Plus.

typedef S75Plus_Status S75CA_Status;			///< The status for the S75CA is the same as for the S75Plus.


//----------------------------------------------------------------------------------------


// S75CA_ProgramList
//-------------------

/**
*
* Supports up to 16 programs.
*
* Pass a pointer to one of these with the ProgramNumber array populated to setProgramList() if you are programming an S75CA device.
* This structure will also be returned by getProgramList() with the ProgramStatus array filled in.
*
*/

typedef struct S75CA_ProgramList	/// The program list structure for an S75CA device
{

	WORD	ProgramNumber[SIZE_PROGRAM_LIST];	///< Program numbers
	WORD	ProgramStatus[SIZE_PROGRAM_LIST];	///< Program Status; 0 = N/A, 1 = Program doesn't exist, 2 = Trying PMT, 3 = Descrambling

} S75CA_ProgramList;

/** @} */

/****************************************************************************************

S75CAPro Specific Structures

****************************************************************************************/

/** @name S75CAPro Structures
 *
 * @{
 *
 */


typedef	S75Plus_NetSetting S75CAPro_NetSetting;			///< The network setting for the S75CAPro is the same as for the S75Plus.

typedef	S75Plus_RFSetting S75CAPro_RFSetting;			///< The RF setting for the S75CAPro is the same as for the S75Plus.

typedef S75Plus_PIDList S75CAPro_PIDList;				///< The PID setting for the S75CAPro is the same as for the S75Plus.

typedef S75Plus_Status S75CAPro_Status;					///< The status for the S75CAPro is the same as for the S75Plus.

typedef S75CA_ProgramList S75CAPro_ProgramList;			///< The program list for the S75CAPro is the same as for the S75CA.

typedef S75PlusPro_PAT S75CAPro_PAT;					///< The PAT for the S75CAPro is the same as for the S75PlusPro.

typedef S75PlusPro_TrapSetting S75CAPro_TrapSetting;	///< The Trap setting for the S75CAPro is the same as for the S75PlusPro.

/** @} */

/****************************************************************************************

S75FK Specific Structures

****************************************************************************************/

/** @name S75FK Structures
 *
 * @{
 *
 */

typedef	S75Plus_NetSetting S75FK_NetSetting;		///< The network setting for the S75FK is the same as for the S75Plus.

typedef	S75Plus_RFSetting S75FK_RFSetting;			///< The RF setting for the S75FK is the same as for the S75Plus.

typedef S75Plus_PIDList S75FK_PIDList;				///< The PID setting for the S75FK is the same as for the S75Plus.

typedef S75Plus_Status S75FK_Status;				///< The status for the S75FK is the same as for the S75Plus.


//----------------------------------------------------------------------------------------

// S75FK_KeyList
//---------------

/**
*
* Supports up to 64 fixed keys
*
* Pass a pointer to one of these to setFixedKeys() if you are programming an S75FK device.
* This structure will also be returned by getFixedKeys()
*
*/


typedef struct S75FK_KeyList	/// The key list structure for an S75FK device
{

	BYTE	key[NUMBER_OF_FIXED_KEYS][8];	///< 8 byte keys
	BOOL	odd[NUMBER_OF_FIXED_KEYS];		///< odd/even flag 0 = even, 1 = odd
	WORD	PID[NUMBER_OF_FIXED_KEYS];		///< associated PID

} S75FK_KeyList;	

/** @} */

/****************************************************************************************

S75FKPro Specific Structures

****************************************************************************************/

/** @name S75FKPro Structures
 *
 * @{
 *
 */

typedef	S75Plus_NetSetting S75FKPro_NetSetting;			///< The network setting for the S75FKPro is the same as for the S75Plus.

typedef	S75Plus_RFSetting S75FKPro_RFSetting;			///< The RF setting for the S75FKPro is the same as for the S75Plus.

typedef S75Plus_PIDList S75FKPro_PIDList;				///< The PID setting for the S75FKPro is the same as for the S75Plus.

typedef S75Plus_Status S75FKPro_Status;					///< The status for the S75FKPro is the same as for the S75Plus.

typedef S75PlusPro_PAT S75FKPro_PAT;					///< The PAT for the S75FKPro is the same as for the S75PlusPro.

typedef S75PlusPro_TrapSetting S75FKPro_TrapSetting;	///< The Trap setting for the S75FKPro is the same as for the S75PlusPro.

typedef S75FK_KeyList S75FKPro_KeyList;					///< The Fixed Key setting for the S75FKPro is the same as for the S75FK.

/** @} */



/****************************************************************************************

S75IPRemap Specific Structures

****************************************************************************************/

/** @name SIPRemap Structures
 *
 * @{
 *
 */

#define NUM_REMAPPING_RULES 16

typedef	S75Plus_NetSetting S75IPRemap_NetSetting;		///< The network setting for the S75IPRemap is the same as for the S75Plus.

typedef	S75Plus_RFSetting S75IPRemap_RFSetting;			///< The RF setting for the S75FKPro is the same as for the S75Plus.

typedef S75Plus_PIDList S75IPRemap_PIDList;				///< The PID setting for the S75FKPro is the same as for the S75Plus.

typedef S75Plus_Status S75IPRemap_Status;					///< The status for the S75FKPro is the same as for the S75Plus.

typedef struct IPRemapRule {

	BYTE action;
	BYTE ttl;
	BYTE originalIP[4];
	BYTE newIP[4];
	BYTE mask[4];

} IPRemapRule;

typedef struct S75IPRemap_RemapTable {

	bool enable;
	
	IPRemapRule remapping_rules[NUM_REMAPPING_RULES];

} S75IPRemap_RemapTable;


/** @} */


/****************************************************************************************
*****************************************************************************************

ERROR DEFINITIONS

*****************************************************************************************
****************************************************************************************/

// Some of the Error Definitions Overlap. This is for debugging Purpose.
// At one point when the Distribution is stable these errors codes will be reduced to minimum set.

/** @name Error Codes
 *
 * @{
 * 
 */

#ifndef __ERRORS__

#define __ERRORS__

//	Error Name              	Error Code 	Error Description
//	==========              	========== 	==================================

#define	ERR_SUCCESS				0x00000000	///< Last operation Successful
#define	ERR_FAILED					    -1	///< Last operation Failed
#define	ERR_METHODNOTSUPPORTED		 0x002	///< Last operation/method is not supported by the device class.


// PID Programmer Errors

#define	ERR_PID_SOCKET_CREATE	0x00001001	///< CPIDProgrammer had error  creating socket
#define	ERR_PID_SOCKET_CONNECT	0x00001002	///< CPIDProgrammer had error connecting to socket
#define	ERR_PID_SEND			0x00001003	///< CPIDProgrammer had error sending message to S75D
#define	ERR_PID_SOCKET_BLOCK	0x00001004	///< CPIDProgrammer had error converting socket to blocking
#define	ERR_PID_RECEIVE			0x00001005	///< CPIDProgrammer had error receiving message from S75D
#define	ERR_PID_ACK_NONE		0x00001006	///< CPIDProgrammer didn't receiver acknowledgement from S75D
#define	ERR_PID_ACK_BAD			0x00001007	///< CPIDProgrammer received bad acknowledgment from S75D


// Program Programmer Errors

#define	ERR_PROGRAM_SOCKET_CREATE	0x00001011	///< CProgramProgrammer had error creating socket
#define	ERR_PROGRAM_SOCKET_CONNECT	0x00001012	///< CProgramProgrammer had error connecting to socket
#define	ERR_PROGRAM_SEND			0x00001013	///< CProgramProgrammer had error sending message to S75D
#define	ERR_PROGRAM_SOCKET_BLOCK	0x00001014	///< CProgramProgrammer had error converting socket to blocking
#define	ERR_PROGRAM_RECEIVE			0x00001015	///< CProgramProgrammer had error receiving message from S75D
#define	ERR_PROGRAM_ACK_NONE		0x00001016	///< CProgramProgrammer didn't receive acknowledgement from S75D
#define	ERR_PROGRAM_ACK_BAD			0x00001017	///< CProgramProgrammer received bad acknowledgment from S75D

#define	ERR_PAT_SOCKET_CREATE	0x00001021	///< CPATProgrammer had error creating socket
#define	ERR_PAT_SOCKET_CONNECT	0x00001022	///< CPATProgrammer had error connecting to socket
#define	ERR_PAT_SEND			0x00001023	///< CPATProgrammer had error sending message to S75D
#define	ERR_PAT_SOCKET_BLOCK	0x00001024	///< CPATProgrammer had error converting socket to blocking
#define	ERR_PAT_RECEIVE			0x00001025	///< CPATProgrammer had error receiving message from S75D
#define	ERR_PAT_ACK_NONE		0x00001026	///< CPATProgrammer didn't receive acknowledgement from S75D
#define	ERR_PAT_ACK_BAD			0x00001027	///< CPATProgrammer received bad acknowledgment from S75D

// Trap Programmer Errors

#define	ERR_TRAP_SOCKET_CREATE	0x00001031	///< CTrapProgrammer had error creating socket
#define	ERR_TRAP_SOCKET_CONNECT	0x00001032	///< CTrapProgrammer had error connecting to socket
#define	ERR_TRAP_SEND			0x00001033	///< CTrapProgrammer had error sending message to S75D
#define	ERR_TRAP_SOCKET_BLOCK	0x00001034	///< CTrapProgrammer had error converting socket to blocking
#define	ERR_TRAP_RECEIVE		0x00001035	///< CTrapProgrammer had error receiving message from S75D
#define	ERR_TRAP_ACK_NONE		0x00001036	///< CTrapProgrammer didn't receive acknowledgement from S75D
#define	ERR_TRAP_ACK_BAD		0x00001037	///< CTrapProgrammer received bad acknowledgment from S75D

// Fixed Key Programmer Errors

#define	ERR_FK_SOCKET_CREATE	0x00001041	///< CFKProgrammer had error creating socket
#define	ERR_FK_SOCKET_CONNECT	0x00001042	///< CFKProgrammer had error connecting to socket
#define	ERR_FK_SEND				0x00001043	///< CFKProgrammer had error sending message to S75
#define	ERR_FK_SOCKET_BLOCK		0x00001044	///< CFKProgrammer had error converting socket to blocking
#define	ERR_FK_RECEIVE			0x00001045	///< CFKProgrammer had error receiving message from S75
#define	ERR_FK_ACK_NONE			0x00001046	///< CFKProgrammer didn't receive acknowledgement from S75
#define	ERR_FK_ACK_BAD			0x00001047	///< CFKProgrammer received bad acknowledgment from S75
#define	ERR_FK_NOTAPPLIED		0x00001048	///< Configuration not applied when attempting to set Fixed Keys

// CAM Watchdog Timer Errors

#define	ERR_CAM_WATCHDOG_TIMEOUT_SOCKET_CREATE	0x00001051	///< CCAMWatchdogTimeoutProgrammer had error creating socket
#define	ERR_CAM_WATCHDOG_TIMEOUT_SOCKET_CONNECT	0x00001052	///< CCAMWatchdogTimeoutProgrammer had error connecting to socket
#define	ERR_CAM_WATCHDOG_TIMEOUT_SEND			0x00001053	///< CCAMWatchdogTimeoutProgrammer had error sending message to S75
#define	ERR_CAM_WATCHDOG_TIMEOUT_SOCKET_BLOCK	0x00001054	///< CCAMWatchdogTimeoutProgrammer had error converting socket to blocking
#define	ERR_CAM_WATCHDOG_TIMEOUT_RECEIVE		0x00001055	///< CCAMWatchdogTimeoutProgrammer had error receiving message from S75
#define	ERR_CAM_WATCHDOG_TIMEOUT_ACK_NONE		0x00001056	///< CCAMWatchdogTimeoutProgrammer didn't receive acknowledgement from S75
#define	ERR_CAM_WATCHDOG_TIMEOUT_ACK_BAD		0x00001057	///< CCAMWatchdogTimeoutProgrammer received bad acknowledgment from S75

// IP REMAP Programming Errors

#define	ERR_IPREMAP_SOCKET_CREATE	0x00001061	///< CPATProgrammer had error creating socket
#define	ERR_IPREMAP_SOCKET_CONNECT	0x00001062	///< CPATProgrammer had error connecting to socket
#define	ERR_IPREMAP_SEND			0x00001063	///< CPATProgrammer had error sending message to S75D
#define	ERR_IPREMAP_SOCKET_BLOCK	0x00001064	///< CPATProgrammer had error converting socket to blocking
#define	ERR_IPREMAP_RECEIVE			0x00001065	///< CPATProgrammer had error receiving message from S75D
#define	ERR_IPREMAP_ACK_NONE		0x00001066	///< CPATProgrammer didn't receive acknowledgement from S75D
#define	ERR_IPREMAP_ACK_BAD			0x00001067	///< CPATProgrammer received bad acknowledgment from S75D


// Message Sender Errors

#define	ERR_SSC_SOCKET_CREATE	0x00002001	///< CSockSendConfirm had error creating socket
#define	ERR_SSC_SOCKET_CONNECT	0x00002002	///< CSockSendConfirm had error connecting to socket
#define	ERR_SSC_SEND			0x00002003	///< CSockSendConfirm had error sending message to S75D 
#define	ERR_SSC_SOCKET_BLOCK	0x00002004	///< CSockSendConfirm had error converting socket to blocking
#define	ERR_SSC_RECEIVE			0x00002005	///< CSockSendConfirm had error receiving message from S75D
#define	ERR_SSC_ACK_NONE		0x00002006	///< CSockSendConfirm didn't receive acknowledgement from S75D
#define	ERR_SSC_ACK_BAD			0x00002007	///< CSockSendConfirm received bad acknowledgment from S75D


// Status Socket Errors

#define	ERR_SS_SOCKET_CREATE		0x00003001	///< CStatusSocket had error creating socket
#define	ERR_SS_SOCKET_CONNECT		0x00003002	///< CStatusSocket had error connecting to socket
#define	ERR_SS_SOCKET_BLOCK			0x00003003	///< CStatusSocket had error converting socket to blocking mode
#define	ERR_SS_SOCKET_NOBLOCK		0x00003004	///< CStatusSocket had error converting socket to non-blocking mode
#define	ERR_SS_RECEIVE				0x00003005	///< CStatusSocket had error receiving message from S75D
#define	ERR_SS_TIMEOUT				0x00003006	///< CStatusSocket timed out waiting for status packet
#define	ERR_SS_SIZE					0x00003007	///< CStatusSocket received status packet of wrong size
#define	ERR_SS_ONRECEIVE_ERROR		0x00003008	///< CStatusSocket entered OnReceive() with non-zero value for nErrorCode
#define	ERR_SS_FLUSH				0x00003009	///< CStatusSocket had unknown  error when flushing buffer


// Status Errors

#define	ERR_STATUS_INVALID_PORT			0x00004001	///< Attempt made to start status listener with invalid port number
#define	ERR_STATUS_NOT_UP				0x00004002	///< Attempt made to operate on status listner when status listener is not up
#define	ERR_STATUS_FILTER_NOIP			0x00004003	///< Attempt made to set IP filtering without first setting the IP address
#define	ERR_STATUS_FILTER_NOMAC			0x00004004	///< Attempt made to set MAC filtering without first setting the MAC address
#define	ERR_STATUS_BufferNotInitialized	    0x4005	///< An attempt to start status with no buffer a allocated for it


// Network Configuration Errors

#define	ERR_NETCFG_NOIP			0x00004005	///< Attempt made to configure network without first setting the IP address
#define	ERR_NETCFG_NONETPATH	0x00004006	///< Attempt made to configure network without first setting the network path
#define	ERR_NETCFG_NOTAPPLIED	0x00004007	///< Configuration not applied when attempting to configure S75D network settings.


// PID Configuration Errors

#define	ERR_PIDGET_NOIP			0x00004008	///< Attempt made to get PID list without first setting the IP address
#define	ERR_PIDGET_NONETPATH	0x00004009	///< Attempt made to get PID list without first setting the network path
#define	ERR_PIDSET_NOIP			0x0000400A	///< Attempt made to set PID list without first setting the IP address
#define	ERR_PIDSET_NONETPATH	0x0000400B	///< Attempt made to set PID list without first setting the network path
#define	ERR_PIDSET_NOTAPPLIED	0x0000400C	///< Configuration not applied when attempting to set S75D PID list


// RF Configuration Errors

#define	ERR_RFCFG_NOIP			0x0000400D	///< Attempt made to configure RF without first setting the IP address
#define	ERR_RFCFG_NONETPATH		0x0000400E	///< Attempt made to configure RF without first setting the network path
#define	ERR_RFCFG_NOTAPPLIED	0x0000400F	///< Configuration not applied when attempting to configure S75D RF settings.


// Miscellaneous Errors

#define	ERR_NETPATH_NODESTINFO	0x00004010	///< Attempt made to set network path without first setting the IP or MAC address.
#define	ERR_RESET_NOIP			0x00004011	///< Attempt made to reset S75D without first setting the IP address
#define	ERR_RESET_NONETPATH		0x00004012	///< Attempt made to reset S75D without first setting the network path


// MAC Programming Errors

#define	ERR_MACPGM_INVALIDMAC	0x00004013	///< Attempt made to program S75D with an invalid MAC address
#define	ERR_MACPGM_HASMAC		0x00004014	///< Attempt made to program S75D with a MAC address when it already has a MAC address.
#define	ERR_MACPGM_NOSTATUS		0x00004015	///< Attempt made to program S75D with a MAC address when status listener is not up.
#define	ERR_MACPGM_NOIP			0x00004016	///< Attempt made to program S75D with a MAC address without first setting the IP address
#define	ERR_MACPGM_NONETPATH	0x00004017	///< Attempt made to program S75D with a MAC address without first setting the network path
#define	ERR_MACPGM_NOTAPPLIED	0x00004018	///< New MAC address not applied when attempting to program S75D MAC address.


// Unit ID Programming Errors

#define	ERR_UIDPGM_NOIP			0x00004019	///< Attempt made to program S75D with a unit ID without first setting the IP address
#define	ERR_UIDPGM_NONETPATH	0x0000401A	///< Attempt made to program S75D with a unit ID without first setting the network path
#define	ERR_UIDPGM_NOTAPPLIED	0x0000401B	///< New unit ID not applied when attempting to program S75D unit ID.


// Program Configuration Errors

#define	ERR_PROGRAMGET_NOIP			0x0000401C	///< Attempt made to get Program list without first setting the IP address
#define	ERR_PROGRAMGET_NONETPATH	0x0000401D	///< Attempt made to get Program list without first setting the network path
#define	ERR_PROGRAMSET_NOIP			0x0000401E	///< Attempt made to set Program list without first setting the IP address
#define	ERR_PROGRAMSET_NONETPATH	0x0000401F	///< Attempt made to set Program list without first setting the network path
#define	ERR_PROGRAMSET_NOTAPPLIED	0x00004020	///< Configuration not applied when attempting to set S75 Program list
#define	ERR_PAT_NOTAPPLIED			0x00004021	///< Configuration not applied when attempting to set S75 PAT

#define	ERR_BAD_STATUS_PORT		    0x402F	///< Trying to configure to Bad Status Port. Status port not supported by auto detection

#define	ERR_BAD_IP_SUBNET_COMBO	    0x2030	///< Invalid combination of device IP and subnet mask

//#ifdef JSAT

// Trap Configuration Errors

#define  ERR_TRAPCFG_NOIP        0x00004030  ///< Attempt made to configure Traps
                                             ///< without first setting the IP
                                             ///< address

#define  ERR_POLL_STATUS_SOCKET_CREATE	0x00004031  ///< CPollStatus had error creating socket
#define  ERR_POLL_STATUS_CONNECT		0x00004032  ///< CPollStatus had error connecting to socket
#define  ERR_STATUS_POLL_SEND			0x00004033  ///< CPollStatus had error sending request
#define  ERR_STATUS_POLL				0x00004034  ///< CPollStatus had error
#define  ERR_STATUS_POLL_ACK_BAD		0x00004035  ///< CPollStatus had error
#define  ERR_WRONG_RECEIVER				0x00004036  ///< CPollStatus had error

//#endif

// Attachment Errors

#define	Error_Already_Attached			0x5000	///< Trying to attach the class to a device when it is already attached 
#define	Error_Attach_MAC				0x5001	///< Invalid MAC Address for the device.
#define	Error_Attach_IP					0x5002	///< Invalid IP address for the device.
#define	Error_Attach_Port				0x5003	///< Invalid Status Port
#define	Attach_Success					0x5004	///< Device Attached Successfully
#define	NetPath_Failed					0x5005	///< Failed to set NetPath
#define	NetPath_Success					0x5006	///< NetPath Set successfully
#define	ERR_NETPATH_ALREADYSET			0x6005	///< Net Path already exist
#define	ERR_CHECK						0x6006	///< Check complete
#define	ERR_NO_NETINFO					0x6007	///< The device doesn't have network Info.
#define	Err_Attach_Device_TypeMismatch	0x5007	///< Attaching the wrong device to the wrong class.
#define	ERR_DEVICE_TYPE_MISMATCH	    0x5008	///< Status does not match class type..
#define	ERR_DEVICE_TYPE_UNKNOWN		    0x5009	///< Trying to OPen a Device with unknown/unsupported device type
#define	ERR_SS_MSG_TOO_BIG				0x500A	///< The received Message on the status port was too big to fit in the buffer
#define	ERR_PID_TOOLONG					0x500B	///< Too many pids in the PID List.
#define	ERR_NO_DefaultConfig		    0x500C	///< Failed to obtain a default configuration for the device.
#define	ERR_PROGRAM_TOOLONG				0x500D	///< Too many programs in the Program List.


// Status Listner Errors 

#define	Err_SL_Not_Initialized	0x6000	///< Status Listner not initialized before a start listening call
#define	SL_SUCCESS			    0x6001	///< Status Listner operation completed Successfully.
#define	Err_SL_NotListening		0x6002	///< An attempt to receive from the status listener when it is not listening.
#define	ERR_INVALID_TIME		   -10	///< Invalid Time difference.


#endif

/** @} */


//------------------------------------------------------------------------------------------------------------------



// FUNCTION DECLARATIONS

//-----------------------------------------------------------------------------
#ifndef LINUX

#ifndef S75_DLL_EXPORTS

// Static Library Function Declarations for Windows
//**************************************************

/** @name Device Selection Functions
 *
 * @{
 *
 */

extern "C"
int __stdcall discoverDevices( Novra_DeviceList *devices );

///< Discovers all Novra receivers that are sending status reports on one of the 10 auto detect status ports

// input   : none
// output  : devices
// returns : 0 = failure
//           1 = success
//
// The 10 auto detect status ports are listed below ...
//
// 0x1974, 0x2075, 0x2175, 0x2275, 0x2375,
// 0x2475, 0x3475, 0x4475, 0x4575, 0x4675
//

//-----------------------------------------------------------------------------------------

extern "C"
S75_Handle __stdcall openDevice( Novra_DeviceEntry * pDev, const int  timeout, int& ErrorCode);

///< Creates a handle for communicating with a device

// This function retrieves a handle to the device pointed to by pDev.

// input   : Device Entry as returned by the Discover Devices, timeout (in ms)
// output  : ErrorCode.
// returns : valid handle or NULL pointer

//--------------------------------------------------------------------------------------

extern "C"
S75_Handle __stdcall forceDevice( const Novra_DeviceEntry * pDev,WORD	Port, const int timeout, int& ErrorCode);

///< Creates a handle for communicating with a device

// input   : Device pointer, statusPort, timeout (in ms)
// output  : ErrorCode
// returns : valid handle or NULL pointer
// The function resets the network configuration of the prescribed device (pDev), and 
// forces it to send status on the given Port (Port).

//================== Start of deprecated section ===============================
//
// The next 4 functions allow the user to communicate with a device using MAC address and IP Address..
// They are no longer supported and not recommended..they will work for version 2.1
// and won't work for any other version of DSP code.

extern "C"
S75_Handle __stdcall b_openS75( const BYTE  S75IPAddress[4],
				const BYTE  S75MACAddress[6],
				const int   *statusPort,
				const int   *timeout          );

///< Creates a handle for communicating with a device

// input   : S75IPAddress, S75MACAddress, statusPort, timeout (in ms)
// output  : none
// returns : valid handle or NULL pointer

//-----------------------------------------------------------------------------

extern "C"
S75_Handle __stdcall s_openS75( const char  *S75IPAddress,
				const char  *S75MACAddress,
				const int   *statusPort,
				const int   *timeout        );

///< Creates a handle for communicating with a device

// input   : S75IPAddress, S75MACAddress, statusPort, timeout (in ms)
// output  : none
// returns : valid handle or NULL pointer

//-----------------------------------------------------------------------------

extern "C"
S75_Handle __stdcall b_forceS75( const BYTE  S75MACAddress[6],
				 const BYTE  S75IPAddress[4],
				 const int   *statusPort,
				 const int   *timeout          );

///< Creates a handle for communicating with a device

// input   : S75IPAddress, S75MACAddress, statusPort, timeout (in ms)
// output  : none
// returns : valid handle or NULL pointer

//-----------------------------------------------------------------------------

extern "C"
S75_Handle __stdcall s_forceS75( const char  *S75MACAddress,
				 const char  *S75IPAddress,
				 const int   *statusPort,
				 const int   *timeout        );

///< Creates a handle for communicating with a device

// input   : S75IPAddress, S75MACAddress, statusPort, timeout (in ms)
// output  : none
// returns : valid handle or NULL pointer

//================== End of deprecated section ================================

/** @} */

/** @name Device Configuration Functions
*
* @{
*
*/

extern "C"
int __stdcall configureNetwork( S75_Handle S75Unit, BYTE* networkSettings );

///< Changes the network parameters of a device

// The function will attempt to configure and verify the Device S75Unit, using the
// Settings in the structure Network Settings.
// The function will try all the possible interfaces on the machine untill a configuration is
// successful.
//  Care must be taken that the network setting structure is the appropriate type for the device.
// input   : S75Unit, networkSettings
// output  : none
// returns : S75_OK or ERROR (see above)

//-----------------------------------------------------------------------------

extern "C"
int __stdcall configureRF( S75_Handle  S75Unit, BYTE* RFSettings );

///< Changes the RF parameters of a device

// The function will attempt to configure and verify the Device S75Unit, using the
// Settings in the structure RFSettings.
//  Care must be taken that the RF setting structure is the appropriate type for the device.
// the device IP should be reachable by the machine

// input   : S75Unit, RFSettings
// output  : none
// returns : S75_OK or ERROR (see above)


//-----------------------------------------------------------------------------

extern "C"
int __stdcall setPIDList( S75_Handle S75Unit, BYTE*  PIDs );

///< Sets the PID filter on a device.

// This function will configure the PID List on the device using the list of PIDs in
// the structure S75D_PIDList

// input   : S75Unit, PIDs
// output  : none
// returns : S75_OK or ERROR (see above)

//-----------------------------------------------------------------------------

extern "C"
int __stdcall setProgramList( S75_Handle S75Unit, BYTE *Programs );

///< Sets the program filter on a device (only the S75CA & S75CA Pro support this functionality)

//-----------------------------------------------------------------------------

extern "C"
int __stdcall setPAT( S75_Handle S75Unit, BYTE *PAT );

///< Sets the PAT on a device (only the Pro devices support this functionality)

//-----------------------------------------------------------------------------

extern "C"
int __stdcall setFixedKeys( S75_Handle S75Unit, BYTE *KeyTable );

///< Sets the fixed keys on a device (S75FK & S75FK Pro)

//-----------------------------------------------------------------------------

extern "C"
int __stdcall setTraps( S75_Handle S75Unit, BYTE *Traps );

///< Sets the Traps on a device (CURRENTLY ONLY THE S75 JLeo SUPPORTS THIS FUNCTIONALITY)

//-----------------------------------------------------------------------------

extern "C"
int __stdcall setCAMWatchdogTimeout( S75_Handle  S75Unit, WORD timeout );

///< This function is used to set the CAM watchdog timeout on a CA device

// input   : S75Unit
// output  : none
// returns : S75_OK if the attempt was a success

//-----------------------------------------------------------------------------

extern "C"
int __stdcall SetDVBMACAddress(S75_Handle S75Unit, BYTE* DVBMAC);

///< This function sets the DVB MAC of a device. (S75V3 only)

// The functions sets the DVB MAC address of the version 3 unit.
//
// input   : Device Handle, DVB MAC,
// output  : NONE
// returns : Error Code

//-----------------------------------------------------------------------------

extern "C"
int __stdcall setIPRemapTable( S75_Handle S75Unit, BYTE*  IPRemapTable );

///< Sets the IP Remap Rules on a device.

// This function will configure the IP Remap Rules on the device using the list of rules in
// the structure S75IPRemapRules

// input   : S75Unit, IPRemapRules
// output  : none
// returns : S75_OK or ERROR (see above)

/** @} */

//-----------------------------------------------------------------------------------------

/** @name Device Query Functions
*
* @{
*
*/

extern "C"
int __stdcall GetDeviceType(S75_Handle S75Unit, WORD &DevType);

///< This function returns the device type.

// input   : Device Handle,
// output  : DevType
// returns : Error Code

//-----------------------------------------------------------------------------

extern "C"
int __stdcall getStatus( S75_Handle S75Unit, BYTE  *status  );

///< Retrieves status from a device

// This function will get the status of the S75Unit and fill the appropriate status
// Structure. the status structure should be of the proper type for the device.


// input   : S75Unit
// output  : status
// returns : S75_OK or ERROR (see above)

//-----------------------------------------------------------------------------

extern "C"
int __stdcall getPIDList( S75_Handle  S75Unit, BYTE*  PIDs );

///< Retrieves the PID filter from a device.

// This Function will read the PID List from the device and fill the structure PIDs .
// input   : S75Unit
// output  : PIDs
// returns : S75_OK or ERROR (see above)

//-----------------------------------------------------------------------------

extern "C"
int __stdcall getProgramList( S75_Handle S75Unit, BYTE *Programs );

///< Gets the program filter on a device (only the S75CA & S75CA Pro support this functionality)

//-----------------------------------------------------------------------------

extern "C"
int __stdcall getPAT( S75_Handle S75Unit, BYTE *PAT );

///< Gets the PAT on a device (only Pro devices support this functionality)

//-----------------------------------------------------------------------------

extern "C"
int __stdcall getFixedKeys( S75_Handle S75Unit, BYTE *KeyTable );

///< Gets the fixed keys from a device  (S75FK & S75FK Pro)

//-----------------------------------------------------------------------------

extern "C"
int __stdcall getTraps( S75_Handle S75Unit, BYTE *Traps );

///< Gets the Traps settings on a device (CURRENTLY ONLY THE S75 JLeo SUPPORTS THIS FUNCTIONALITY)

//-----------------------------------------------------------------------------

extern "C"
int __stdcall getCAMWatchdogTimeout( S75_Handle  S75Unit, WORD *timeout );

///< This function is used to read the CAM watchdog timeout on a CA device

// input   : S75Unit
// output  : none
// returns : S75_OK if the attempt was a success

extern "C"
int __stdcall getIPRemapTable( S75_Handle  S75Unit, BYTE*  IPRemapTable );

///< Retrieves the IP Remap Table from a device.

// This Function will read the IP Remap Table from the device and fill the structure RemapTable .
// input   : S75Unit
// output  : RemapTable
// returns : S75_OK or ERROR (see above)


/** @} */


//-----------------------------------------------------------------------------

/** @name Misc Functions
*
* @{
*
*/

extern "C"
int __stdcall closeS75( S75_Handle S75Unit );

///< Disposes of an S75_Handle (closes communication connections)

// input   : S75Unit
// output  : none
// returns : S75_OK
// the function deletes the dynamically allocated object it should be called to avoid memory leaks

//-----------------------------------------------------------------------------

extern "C"
int __stdcall calcSignalStrengthPercentage( S75_Handle S75Unit, BYTE AutomaticGainControl, BYTE DemodulatorGain, BYTE SignalLock, BYTE DataLock );

///< Calculates signal strength as a percentage.

// input   : Automatic Gain Control value, Demodulator Gain value, Data Lock status
// output  : signal strength as a percentage
// returns : 0-100

//-----------------------------------------------------------------------------

extern "C"
int __stdcall resetS75( S75_Handle  S75Unit );

///< This function causes a reset of the device firmware.

// input   : S75Unit
// output  : none
// returns : S75_OK if the attempt was a success

//-----------------------------------------------------------------------------

extern "C"
int __stdcall resetCAM( S75_Handle  S75Unit );

///< This function causes a reset of the CAM on an S75CA or S75CA Pro device

// input   : S75Unit
// output  : none
// returns : S75_OK if the attempt was a success

//-----------------------------------------------------------------------------

extern"C"
int __stdcall GetAvailablePort( WORD &Port);

///< This function can be used to help choose a status port for a device.

// Input  : none 
// Output : Port, 
// return value : Error Code
// The functions scans the auto detection ports and returns the first available
// port that can be used on this machine. i.e. no applications is listening on that port

//-----------------------------------------------------------------------------

extern "C"
void  __stdcall getVersion( int *major, int *minor, int *day, int *month, int *year );

///< Returns the version of the library and the release date.

/** @} */

//-----------------------------------------------------------------------------

//#ifdef JSAT

/** @name JSAT Functions
*
* @{
*
*/

extern "C"
S75_Handle __stdcall discoverRemoteDevice( BYTE* ip, int timeout );

///< This function creates a handle for a remote Novra receiver by IP address
//
// input   : none
// output  : device
// returns : 0 = failure
//           1 = success
//

#ifdef JSAT
extern "C"
int __stdcall configureTraps( S75_Handle  S75Unit, BYTE* TrapSettings );

// The function will attempt to configure and verify the Device S75Unit, using the
// Settings in the structure TrapSettings.
//  Care must be taken that the Trap setting structure is the appropriate type for the device.
// the device IP should be reachable by the machine

// input   : S75Unit, TrapSettings
// output  : none
// returns : S75_OK or ERROR (see above)
#endif

extern "C"
int __stdcall pollStatus( S75_Handle S75Unit, BYTE  *status, int timeout=2000  );

///< This function will get the status of the S75Unit and fill the appropriate status structure. The status structure should be of the proper type for the device.


// input   : S75Unit
// output  : status
// returns : S75_OK or ERROR (see above)

#ifdef BROADCAST_POLL_FOR_STATUS
extern "C"
int __stdcall broadcastPollStatus( S75_Handle S75Unit, BYTE  *status  );

// This function will get the status of the S75Unit and fill the appropriate status
// Structure. the status structure should be of the proper type for the device.


// input   : S75Unit
// output  : status
// returns : S75_OK or ERROR (see above)
//-----------------------------------------------------------------------------
#endif

extern "C"
S75_Handle __stdcall openRemoteDevice( Novra_DeviceEntry * pDev, const int  timeout, int& ErrorCode);

///< This function retrieves a handle to the device pointed to by pDev.

// input   : Device Entry as returned by the Discover Devices, timeout (in ms)
// output  : ErrorCode.
// returns : valid handle or NULL pointer

extern "C"
int __stdcall changePassword(S75_Handle S75Unit, char *password, unsigned char key[16]);

///< This function changes the password on a unit (that supports passwords).

// input   : Device Handle, password,
// output  : NONE
// returns : Error Code

extern "C"
int __stdcall specifyPassword(S75_Handle S75Unit, char *password);

///< This functions specifies the password to use when communicating with a unit.

// input   : Device Handle, password,
// output  : NONE
// returns : Error Code 

extern "C"
S75_Handle __stdcall forceRemoteDevice( const Novra_DeviceEntry * pDev,WORD	Port, const int timeout, int& ErrorCode);

///< Creates a handle for communicating with a device

// input   : Device pointer, statusPort, timeout (in ms)
// output  : ErrorCode
// returns : valid handle or NULL pointer
// The function resets the network configuration of the prescribed device (pDev), and 
// forces it to send status on the given Port (Port).

/** @} */

//#endif

//-----------------------------------------------------------------------------



#ifdef _NOVRA_INTERNAL_BUILD
extern "C"
int __stdcall ProgramUnitIdentifier(S75_Handle S75Unit, BYTE* UID);

///< This function is for Novra use only.

// This function programs the Unit Identification Code in the Unit EEPROM.

// input   : Device Handle, UID pointer
// output  : None
// returns : Error Code

//-----------------------------------------------------------------------------------------

extern "C"
int __stdcall ProgramMACAddress(S75_Handle S75Unit, BYTE* MAC);

///< This function is for Novra use only.

// This function programs the MAC address in the new Unit EEPROM.

// input   : Device Handle, MAC pointer
// output  : None
// returns : Error Code

//-----------------------------------------------------------------------------------------

#endif

#endif


// DLL Exports
// ************

#ifdef S75_DLL_EXPORTS

#define S75_DLL_API __declspec(dllexport)

S75_DLL_API void  getVersion( int *major, int *minor, int *day, int *month, int *year );

S75_DLL_API int discoverDevices( Novra_DeviceList *devices );

S75_DLL_API int closeS75( S75_Handle S75Unit );

S75_DLL_API int calcSignalStrengthPercentage(  S75_Handle S75Unit, BYTE AutomaticGainControl, BYTE DemodulatorGain, BYTE SignalLock, BYTE DataLock );

S75_DLL_API int configureNetwork( S75_Handle S75Unit, BYTE* networkSettings );

S75_DLL_API int configureRF( S75_Handle  S75Unit, BYTE* RFSettings );

S75_DLL_API int setPIDList( S75_Handle S75Unit, BYTE*  PIDs    );

S75_DLL_API int getPIDList( S75_Handle  S75Unit, BYTE*  PIDs    );

S75_DLL_API int setProgramList( S75_Handle S75Unit, BYTE *Programs );

S75_DLL_API int getProgramList( S75_Handle S75Unit, BYTE *Programs );

S75_DLL_API int setPAT( S75_Handle S75Unit, BYTE *PAT );

S75_DLL_API int getPAT( S75_Handle S75Unit, BYTE *PAT );

S75_DLL_API int setFixedKeys( S75_Handle S75Unit, BYTE *KeyTable );

S75_DLL_API int getFixedKeys( S75_Handle S75Unit, BYTE *KeyTable );

S75_DLL_API int setTraps( S75_Handle S75Unit, BYTE *Traps );

S75_DLL_API int getTraps( S75_Handle S75Unit, BYTE *Traps );

S75_DLL_API int getStatus( S75_Handle S75Unit, BYTE  *status  );

S75_DLL_API int resetS75( S75_Handle  S75Unit );

S75_DLL_API int resetCAM( S75_Handle  S75Unit );

S75_DLL_API int setCAMWatchdogTimeout( S75_Handle  S75Unit, WORD timeout );

S75_DLL_API int getCAMWatchdogTimeout( S75_Handle  S75Unit, WORD *timeout );

S75_DLL_API int setIPRemapTable( S75_Handle  S75Unit, BYTE *IPRemapTable );

S75_DLL_API int getIPRemapTable( S75_Handle S75Unit, BYTE *IPRemapTable );

S75_DLL_API int GetAvailablePort( WORD &Port);

S75_DLL_API S75_Handle openDevice( Novra_DeviceEntry * pDev, const int  timeout, int& ErrorCode);

S75_DLL_API S75_Handle forceDevice( const Novra_DeviceEntry * pDev,WORD	Port, const int timeout, int& ErrorCode);

S75_DLL_API int GetDeviceType(S75_Handle S75Unit, WORD &DevType);

S75_DLL_API int SetDVBMACAddress(S75_Handle S75Unit, BYTE* DVBMAC);

S75_DLL_API int ProgramUnitIdentifier(S75_Handle S75Unit, BYTE* UID);

S75_DLL_API int ProgramMACAddress(S75_Handle S75Unit, BYTE* MAC);

S75_DLL_API S75_Handle b_openS75(const BYTE  S75IPAddress[4], const BYTE  S75MACAddress[6], const int   *statusPort, const int   *timeout);

S75_DLL_API S75_Handle s_openS75(const char  *S75IPAddress, const char  *S75MACAddress, const int   *statusPort, const int   *timeout);

S75_DLL_API S75_Handle b_forceS75(const BYTE  S75MACAddress[6], const BYTE  S75IPAddress[4], const int   *statusPort, const int   *timeout);

S75_DLL_API S75_Handle s_forceS75(const char  *S75MACAddress, const char  *S75IPAddress, const int   *statusPort, const int   *timeout);

#endif


#else

// LINUX style declarations... Check the documentation above.
//***********************************************************
extern "C" { 

void getVersion( int *major, int *minor, int *day, int *month, int *year );

int discoverDevices( Novra_DeviceList *devices );

S75_Handle openDevice( Novra_DeviceEntry * pDev, const int  timeout, int& ErrorCode);

S75_Handle forceDevice( const Novra_DeviceEntry * pDev,WORD	Port, const int timeout, int& ErrorCode);

int configureNetwork( S75_Handle S75Unit, BYTE* networkSettings );

int configureRF( S75_Handle  S75Unit, BYTE* RFSettings );

int setPIDList( S75_Handle S75Unit, BYTE  *PIDs );

int getPIDList( S75_Handle  S75Unit, BYTE  *PIDs    );

int setProgramList( S75_Handle S75Unit,	BYTE *Programs );

int getProgramList( S75_Handle S75Unit,	BYTE *Programs );

int setPAT( S75_Handle S75Unit,	BYTE *PAT );

int getPAT( S75_Handle S75Unit,	BYTE *PAT );

int setFixedKeys( S75_Handle S75Unit,	BYTE *KeyTable );

int getFixedKeys( S75_Handle S75Unit,	BYTE *KeyTable );

int setTraps( S75_Handle S75Unit,	BYTE *Traps );

int getTraps( S75_Handle S75Unit,	BYTE *Traps );

int getStatus( S75_Handle S75Unit, BYTE  *status  );

int resetS75( S75_Handle  S75Unit );

int resetCAM( S75_Handle  S75Unit );

int setCAMWatchdogTimeout( S75_Handle  S75Unit, WORD timeout );

int getCAMWatchdogTimeout( S75_Handle  S75Unit, WORD *timeout );

int setIPRemapTable( S75_Handle  S75Unit, BYTE*  IPRemapTable );

int getIPRemapTable( S75_Handle  S75Unit, BYTE*  IPRemapTable );

int GetAvailablePort( WORD &Port);

int GetDeviceType(S75_Handle S75Unit, WORD &DevType);

int SetDVBMACAddress(S75_Handle S75Unit, BYTE* DVBMAC);

int closeS75( S75_Handle S75Unit );

int calcSignalStrengthPercentage(  S75_Handle S75Unit, BYTE AutomaticGainControl, BYTE DemodulatorGain, BYTE SignalLock, BYTE DataLock ) ;


//#ifdef JSAT

S75_Handle discoverRemoteDevice( BYTE* ip, int timeout );
int specifyPassword(S75_Handle S75Unit, char *password);
int pollStatus( S75_Handle S75Unit, BYTE  *status, int timeout=2000  );

#ifdef JSAT
int changePassword(S75_Handle S75Unit, char *password, unsigned char key[16]);
int findAdapters( struct ifconf *adapters );
S75_Handle openRemoteDevice( Novra_DeviceEntry * pDev, const int  timeout, int& ErrorCode);
S75_Handle forceRemoteDevice( const Novra_DeviceEntry * pDev,WORD	Port, const int timeout, int& ErrorCode);
int configureTraps( S75_Handle  S75Unit, BYTE* TrapSettings );
#endif

#ifdef BROADCAST_POLL_FOR_STATUS
	int broadcastPollStatus( S75_Handle S75Unit, BYTE  *status  );
#endif

	//#endif

#ifdef _NOVRA_INTERNAL_BUILD

	int ProgramUnitIdentifier(S75_Handle S75Unit, BYTE* UID);

	int ProgramMACAddress(S75_Handle S75Unit, BYTE* MAC);

#endif
}

//==========================================================================================

S75_Handle b_openS75( const BYTE S75IPAddress[4], const BYTE  S75MACAddress[6], const int *statusPort, const int *timeout);

S75_Handle s_openS75( const char *S75IPAddress, const char *S75MACAddress, const int *statusPort, const int *timeout);

S75_Handle b_forceS75( const BYTE S75MACAddress[6], const BYTE S75IPAddress[4], const int *statusPort, const int *timeout);

S75_Handle s_forceS75( const char *S75MACAddress, const char *S75IPAddress, const int *statusPort, const int *timeout);

#endif   // For the function declaration under linux

#endif // For the header

