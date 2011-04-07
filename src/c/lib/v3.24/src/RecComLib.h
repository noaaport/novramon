#ifndef _REC_COM_LIB

#define _REC_COM_LIB



typedef enum NOVRA_DEVICES		/// Used to enumerate the Novra device types
{ 

	R_ANY,			///< Used by the auto discovery class
	R_UNKNOWN,		///< A device that reports firmware that is not recognised or bad status packet.
	R_S75_V1,		///< S75 Version 1 device
	R_S75_V2_1,		///< S75 Version 2.X device
	R_S75_V3,		///< S75 Version 3.X device
	R_A75,			///< A75 device.
	R_S75V,			///< S75 Version 5.x PesMaker device.
	R_S75CA,		///< S75 Conditional Access device.
	R_A75V,			///< A75 PesMaker device.
	R_S75J,			///< S75 J
	R_S75JPLUS,		///< S75 J Plus device.
	R_S75FK,		///< S75 Fixed Key device.
	R_S75PLUS,		///< S75 Plus device.
	R_S75CAPRO,		///< S75 Conditional Access device with support for the PRO feature set
	R_S75FKPRO,		///< S75 Fixed Key device with support for the PRO feature set
	R_S75PLUSPRO,	///< S75 Plus device with support for the PRO feature set
	R_S75IPREMAP,	///< S75+ Pro device with IP Remapping function
	R_S200,			///< S200 
	R_S200PRO,		///< S200 device with support for the PRO feature set
	R_S200CA,		///< S200 Conditional Access device
	R_S200CAPRO,	///< S200 Conditional Access device with support for the PRO feature set
	R_S200CAPROCS,	///< S200 Conditional Access device with support for the PRO feature set and Cipher Saber output encryption
	R_S200FK,		///< S200 Fixed Key device
	R_S200FKPRO,	///< S200 Fixed Key device with support for the PRO feature set
	R_S200V,		///< S200 Video receiver
	R_S200VCA,		///< S200 Video receiver with Conditional Access
	R_S200J,		///< S200 J
	R_SJ300,		///< SJ300
	MAX_DEVICE

} NOVRA_DEVICES;


typedef enum SIGNAL_TYPE_CONTROLS
{

	AUTO,
	DVBS,
	DVBS2

} SIGNAL_TYPE_CONTROLS;


typedef enum VITERBI_RATES		/// Used to enumerate the possible Viterbi rates
{ 

	VITERBI_ERROR,		///< The Viterbi rate is not known
	HALF,				///< 1/2
	TWO_THIRDS,			///< 2/3
	THREE_QUARTERS,		///< 3/4
	FOUR_FIFTHS,		///< 4/5
	FIVE_SIXTHS,		///< 5/6
	SIX_SEVENTHS,		///< 6/7
	SEVEN_EIGTHS,		///< 7/8
	EIGHT_NINTHS		///< 8/9

} VITERBI_RATES;


typedef enum MODCODS		/// Used to enumerate the possible MODCODS
{ 

	MODCOD_UNKNOWN,			///< The MODCOD is not known
	QUARTER_QPSK,			///< 1/4 QPSK
	THIRD_QPSK,				///< 1/3 QPSK
	TWO_FIFTHS_QPSK,		///< 2/5 QPSK
	HALF_QPSK,				///< 1/2 QPSK
	THREE_FIFTHS_QPSK,		///< 3/5 QPSK
	TWO_THIRDS_QPSK,		///< 2/3 QPSK
	THREE_QUARTERS_QPSK,	///< 3/4 QPSK
	FOUR_FIFTHS_QPSK,		///< 4/5 QPSK
	FIVE_SIXTHS_QPSK,		///< 5/6 QPSK
	EIGHT_NINTHS_QPSK,		///< 8/9 QPSK
	NINE_TENTHS_QPSK,		///< 9/10 QPSK
	THREE_FIFTHS_8PSK,		///< 3/5 8PSK
	TWO_THIRDS_8PSK,		///< 2/3 8PSK
	THREE_QUARTERS_8PSK,	///< 3/4 8PSK
	FIVE_SIXTHS_8PSK,		///< 5/6 8PSK
	EIGHT_NINTHS_8PSK,		///< 8/9 8PSK
	NINE_TENTHS_8PSK,		///< 9/10 8PSK
	TWO_THIRDS_16PSK,		///< 2/3 16PSK
	THREE_QUARTERS_16PSK,	///< 3/4 16PSK
	FOUR_FIFTHS_16PSK,		///< 4/5 16PSK
	FIVE_SIXTHS_16PSK,		///< 5/6 16PSK
	EIGHT_NINTHS_16PSK,		///< 8/9 16PSK
	NINE_TENTHS_16PSK,		///< 9/10 16PSK
	THREE_QUARTERS_32PSK,	///< 3/4 32PSK
	FOUR_FIFTHS_32PSK,		///< 4/5 32PSK
	FIVE_SIXTHS_32PSK,		///< 5/6 32PSK
	EIGHT_NINTHS_32PSK,		///< 8/9 32PSK
	NINE_TENTHS_32PSK,		///< 9/10 32PSK
	THIRD_BPSK,				///< 1/3 BPSK
	QUARTER_BPSK,			///< 1/4 BPSK
	LAST_MODCOD

} MODCODS;


typedef enum STREAM_TYPES
{

	VIDEO_1		= 0x01,
	VIDEO_2		= 0x02,
	AUDIO_3		= 0x03,
	AUDIO_4		= 0x04,
	AUDIO_DOLBY	= 0x06,
	TYPE_A_MPE	= 0x0a,
	TYPE_D_MPE	= 0x0d,
	VIDEO_H264	= 0x1b

} STREAM_TYPES;


typedef enum IP_REMAP_RULES
{

	NO_RULE,
	NORMAL,
	FORWARD,
	DISCARD

} IP_REMAP_RULES;


typedef enum NOVRA_ERRORS	/// Used to enumerate error codes
{

	N_ERROR_FAILED	= -1,	///< The operation failed
	N_ERROR_SUCCESS = 0,	///< The operation was successful
	N_ERROR_SOCKET_CREATE,	///< Socket creation failed
	N_ERROR_SOCKET_CONNECT,	///< Socket connect failed
	N_ERROR_SOCKET_BIND,	///< Socket bind failed
	N_ERROR_NO_STATUS,		///< Could not obtain a status packet from the receiver
	N_ERROR_PID_SEND,		///< Could not send the PID list to the receiver
	N_ERROR_PID_RECEIVE,	///< Could not receive the PID list from the receiver
	N_ERROR_PID_ACK_BAD,	///< The acknowledgement to a PID request was bad
	N_ERROR_READ_CURRENT	///< Cound not get the receiver's current configuration

} NOVRA_ERRORS;


#ifndef WINDOWS
	extern char* itoa( int num, char *buf, int radix );
#endif

#include "Receiver.h"
#include "ReceiverSearch.h"
#include "ReceiverList.h"


/**
 * This function returns version information for the library.
 *
 * @param major pointer to an integer to hold the major version number
 * @param minor pointer to an integer to hold the minor version number
 * @param day pointer to an integer to hold the day
 * @param month pointer to an integer to hold the month
 * @param year pointer to an integer to hold the year
 */
extern void getVersion( int *major, int *minor, int *day, int *month, int *year );

#endif

