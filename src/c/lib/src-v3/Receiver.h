#ifndef RECEIVER_H
#define RECEIVER_H


#ifdef WINDOWS
	#include <iostream.h>
	#include <string>
#else
	#include <iostream>
	#include <stdlib.h>
	#include <string.h>
#endif
#include <vector>

#include "RecComLib.h"
#include "tinyxml.h"
#include "ReceiverParameter.h"
#include "ParameterValue.h"
#include "EnumeratedReceiverParameter.h"


#define F_EXISTS	0x01
#define F_READ  	0x02
#define F_WRITE 	0x04
#define F_STRING	0x08
#define F_SHORT 	0x10
#define F_LONG  	0x20
#define F_FLOAT 	0x40
#define F_FLAG  	0x80
#define F_I64   	0x100
#define F_ENUM  	0x200
#define F_INDEXED	0x400
#define F_ID		0x800
#define F_DEF    	0x1000
#define F_STATUS	0x2000
#define F_CONFIG	0x4000


#ifndef WINDOWS
	using namespace std;
#endif


extern string timestamp();


// The Receiver class is used for all interactions with a receiver.
/**
  * It is an abstract base class for all of the different types of Novra receivers.
  * You should never need to instantiate or interact with any of the subclasses as it is a primary goal of the design that you should not need to.
  * The Receiver class maintains two copies of a receiver's data items.
  * One set is labelled as CURRENT, and is read only.
  * The other set is labelled as PENDING and can be read and written to (unless the value is read-only).
  * All changes that a program makes to a receiver's data are made to the PENDING data set.
  * When the program calls the Receiver class's <b>apply()</b> function, the new data will be written to the receiver and the CURRENT data will be updated.
  * When a program reads a ParameterValue from a Receiver, it can specify whether the CURRENT copy or the PENDING copy should be read.
  * All parameters are stored as name/value pairs. The Receiver class provides a function <b>hasParameter()</b>, that can be used to ask a receiver if it has a named parameter.
  * By querying whether or not a receiver has certain key parameters, you can also tell what functionality a receiver provides.
  */

/******************************* Abstract Class ****************************
Receiver does not have any pure virtual methods, but its author
  defined it as an abstract class, so you should not use it directly.
  Inherit from it instead and create only objects from the derived classes
*****************************************************************************/

class Receiver	/// The Receiver class is used for all interactions with a receiver.

{
public:

	Receiver();
	~Receiver();

    /**
     * Connect to the receiver and retrieve its status.
	 *
	 * @param timeout time to allow for connection (in milliseconds)
	 * @param ErrorCode reference to a variable that should be set with an error code
	 *
     * @return success=true, fail=false
     */
    bool connect(	const int timeout,			///< timeout in milliseconds (input)
			NOVRA_ERRORS &ErrorCode,
			bool passive=false		);

    /**
	 *
	 * Checks to see if a receiver is connected.
	 *
	 * @param timeout time to allow for check (in milliseconds)
	 *
     * @return true if connected
     */
    virtual bool connected( int timeout )=0;


	/**
	 * Disconnect from the receiver.
	 */
    void disconnect();


	/**
	 * @param password
	 *
	 * @return true if successful login
	 */
	virtual bool login( string password ) { return true; }


    /**
	 * This function is used to ask if a particular parameter is part of the set of paramaters implemented by a specific receiver.
	 *
     * @param ParameterName a parameter from the enumerated list of Novra receiver parameters
	 *
	 * @return true if the identified parameter is one that exists for this receiver
     */
    bool hasParameter (ReceiverParameter ParameterName )
	{
		return (bool)( ( parameterMask[ParameterName] & F_EXISTS ) != 0 );
	}


    /**
	 * This function is used to ask if a particular parameter is readable.
	 *
     * @param ParameterName a parameter from the enumerated list of Novra receiver parameters
	 *
	 * @return true if the identified parameter is readable
     */
	virtual bool parameterReadable( ReceiverParameter ParameterName )
	{
		return (bool)( ( parameterMask[ParameterName] & F_READ ) != 0 );
	}


    /**
	 * This function is used to ask if a particular parameter is writeable.
	 *
     * @param ParameterName a parameter from the enumerated list of Novra receiver parameters
	 *
	 * @return true if the identified parameter is writeable
     */
	virtual bool parameterWriteable( ReceiverParameter ParameterName )
	{
		return (bool)( ( parameterMask[ParameterName] & F_WRITE ) != 0 );
	}


	virtual bool parameterIsIndexed( ReceiverParameter ParameterName )
	{
		return (bool)( ( parameterMask[ParameterName] & F_INDEXED ) != 0 );
	}


	virtual bool parameterIsIdentifier( ReceiverParameter ParameterName )
	{
		return (bool)( ( parameterMask[ParameterName] & F_ID ) != 0 );
	}


	virtual bool parameterIsDefinition( ReceiverParameter ParameterName )
	{
		return (bool)( ( parameterMask[ParameterName] & F_DEF ) != 0 );
	}


	virtual bool parameterIsStatus( ReceiverParameter ParameterName )
	{
		return (bool)( ( parameterMask[ParameterName] & F_STATUS ) != 0 );
	}


	virtual bool parameterIsConfig( ReceiverParameter ParameterName )
	{
		return (bool)( ( parameterMask[ParameterName] & F_CONFIG ) != 0 );
	}


	virtual bool parameterIsEnumerated( ReceiverParameter ParameterName )
	{
		return (bool)( ( parameterMask[ParameterName] & F_ENUM ) != 0 );
	}




    /**
     * Set a parameter of a receiver.  The change is made to the PENDING paramater set (ie. it is not sent to the receiver hardware until the apply() function is used.
	 *
     * @param  ParameterName the name of the parameter being set.
     * @param  ParamValue the value to set the parameter to
	 *
	 * @return true if parameter was set
     */
    virtual bool setParameter( ReceiverParameter ParameterName, string ParamValue )
    {
		if ( ( parameterMask[ParameterName] & ( F_EXISTS | F_WRITE ) ) != 0 ) {
			(pendingParameters[ParameterName])->setM_value( ParamValue );
			dirtyFlags[ParameterName] = true;
			return true;
		} else {
			return false;
		}
    }


    /**
     * Set an indexed parameter of a receiver.  An indexed parameter is one where the receiver has a list of items grouped under one name.  The list of PIDs is an example of an indexed parameter.  The change is made to the PENDING paramater set (ie. it is not sent to the receiver hardware until the apply() function is used.
	 *
     * @param  ParameterName the name of the parameter being set.
	 * @param  index the position of the item in the list (0..n)
     * @param  ParamValue the value to set the parameter to
	 *
	 * @return true if parameter was set
     */
	virtual bool setIndexedParameter( ReceiverParameter ParameterName, int index, string ParamValue )=0;



    /**
     * Retrieve a parameter setting.
	 *
	 * @param ParameterName the name of the parameter being read.
	 * @param parameter_set specify as CURRENT or PENDING (default is CURRENT)
	 *
	 * @return a reference to the ParameterValue object containg the data item
     */
    virtual ParameterValue& getParameter (ReceiverParameter ParameterName, ParameterSet parameter_set=CURRENT )
    {
		if ( ( parameterMask[ParameterName] & ( F_EXISTS | F_READ ) ) != 0 ) {
			switch ( ParameterName ) {
				case PID_COUNT	:	countParameter( ParameterName );
							break;
				default		:	break;
			}
			if ( ( ( parameterMask[ParameterName] & F_WRITE ) != 0 ) && ( parameter_set==PENDING ) ) {
				if ( (*(pendingParameters[ParameterName])).isSet() ) {
					return *(pendingParameters[ParameterName]);
				} else {
					return *(currentParameters[ParameterName]);
				}
			} else {
				return *(currentParameters[ParameterName]);
			}
		} else {
//			return ParameterValue();
			return nullParameter;
		}
    }


    /**
     * Retrieve an indexed parameter setting.
	 *
	 * @param ParameterName the name of the parameter being read.
	 * @param index the position of the item in the list (0..n)
	 * @param parameter_set specify as CURRENT or PENDING (default is CURRENT)
	 *
	 * @return a reference to the ParameterValue object containg the data item
     */
    virtual ParameterValue& getIndexedParameter (ReceiverParameter ParameterName, int index, ParameterSet parameter_set=CURRENT )=0;


    /**
     * Retrieve a double indexed parameter setting. ( 0..m, 0..n )
	 *
	 * @param ParameterName the name of the parameter being read.
	 * @param index1 the first index (0..m)
	 * @param index2 the second index (0..n)
	 * @param parameter_set specify as CURRENT or PENDING (default is CURRENT)
	 *
	 * @return a reference to the ParameterValue object containg the data item
     */
    virtual ParameterValue& getDblIndexedParameter (ReceiverParameter ParameterName, int index1, int index2, ParameterSet parameter_set=CURRENT ) { return nullParameter; };


    /**
     * Attempts to configure the receiver with the PENDING parameter set.
	 *
	 * @param ParameterName name of the parameter to apply.  Currently only two values are implemented, ALL_PARAMETERS (the default) and CAM_WATCHDOG_INTERVAL
	 *
     * @return returns true if successful
     */
    virtual bool apply( ReceiverParameter ParameterName=ALL_PARAMETERS )=0;


    /**
     * Reset all values of the PENDING parameter set to be the same as the CURRENT parameter set.
     */
    virtual void cancel( );


    /**
     * Resets a specific PENDING parameter value to be the same as the CURRENT parameter value.
	 *
	 * @param ParameterName name of the parameter whose change is being cancelled.
	 *
	 * @return true if successful, false otherwise (parameter didn't exist or is not writeable)
     */
	virtual bool cancelParameter( ReceiverParameter ParameterName )
    {
		if ( ( parameterMask[ParameterName] & ( F_EXISTS | F_WRITE ) ) != 0 ) {
			(pendingParameters[ParameterName])->setM_value( (currentParameters[ParameterName])->getM_value() );
			dirtyFlags[ParameterName] = false;
			return true;
		} else {
			return false;
		}
    }


    /**
     * Creates an empty PENDING configuration
     */
	virtual void clear();


	/**
	 * Cause a receiver to go through a reset cycle
	 *
	 * @return true if successful
	 */
	virtual bool reset()=0;


	/**
	 * Cause a receiver to restore its factory config
	 *
	 * @return true if successful
	 */
	virtual bool factoryReset() { return false; };


	/**
	 * Some receiver parameters are enumerated.  This function can be used to get a human readable string for an enumerated value.
	 * 
	 * @param ParameterName name of the parameter for which an enumeration is to be converted
	 * @param enumeratedValue a number as returned by the ParamaterValue::asInt() member function.
	 *
	 * @return a human readable string
	 */
	virtual string enumToString( ReceiverParameter ParameterName, int enumeratedValue );


	/**
	 * Obtains a new status packet from the receiver.
	 * 
	 * @return true if a new status packet was obtained
	 */
	virtual bool getStatus();

	/**
	 * Accepts a status packet from an outside source (if it belongs to the receiver).
	 * 
	 * @return true if the new status packet was accepted or false if it was rejected
	 */
	virtual bool acceptStatus( string ip, unsigned char *packet );

	
	/**
	 * Updates the CURRENT data set using the status obtained from the most recent call to getStatus()
	 *
	 * @return true on success 
	 */
	bool updateStatus();


	/**
	 * Reads the receiver's configuration settings into the CURRENT parameter set
	 *
	 * @return true on success
	 */
	virtual bool readCurrentSettings()=0;


	/**
	 * Reads meta information from the received signal
	 *
	 * @return true on success
	 */
	virtual bool readStreamInfo() { return false; };



    /**
     * Matches a parameter of a receiver.  Returns true if value matches, false otherwise.  This function can be used when searching for a receiver within a ReceiverList
	 *
     * @param  ParameterName The name of the parameter being matched.
     * @param  ParamValue The value being matched
	 * @param  parameter_set specify whether to match agains the CURRENT (default) or PENDING data set
	 *
	 * @return true if the receiver matches the input
     */
	virtual bool matches( ReceiverParameter ParameterName, string ParamValue, ParameterSet parameter_set=CURRENT ) {

		if ( hasParameter( ParameterName ) ) {
			if ( parameter_set == PENDING ) {
				if ( pendingParameters[ParameterName]->asString() == ParamValue ) {
					return true;
				} else {
					return false;
				}
			} else {
				if ( currentParameters[ParameterName]->asString() == ParamValue ) {
					return true;
				} else {
					return false;
				}
			}
		} else {
			return false;
		}

	}

//    virtual bool isValidValue (ReceiverParameter receiverParm, string parmValue )
//    {
//		return false;
//    }


//    virtual bool isValidValue (ReceiverParameter receiverParm, short parmValue )
//    {
//		return false;
//    }


//    virtual bool isValidValue (ReceiverParameter receiverParm, long parmValue )
//    {
//		return false;
//    }


//    virtual bool isValidValue (ReceiverParameter receiverParm, double parmValue )
//    {
//		return false;
//    }


	virtual bool changePassword( char *password, unsigned char key[16] )=0;

	int loadConfigFromXML( string file_path, bool load_network=false );

	int saveConfigToXML( string file_path );

	int statusToXML( string file_path );

	int definitionToXML( string file_path );

	int configToXML( string file_path );

	int toXML( string file_path );

	int statusToXML( FILE *f );

	int definitionToXML( FILE *f );

	int configToXML( FILE *f );

	int toXML( FILE *f );


protected:

	friend class ReceiverFactory;

	//	virtual bool pollStatus( unsigned char *packet ) { return false; }
	
	virtual bool pollStatus( unsigned char *packet, int timeout ) { return false; }


    virtual bool setCurrentParameter (ReceiverParameter ParameterName, string ParameterValue, int index=0 )
    {
		if ( ( parameterMask[ParameterName] & ( F_EXISTS ) ) != 0 ) {
			(currentParameters[ParameterName])->setM_value( ParameterValue );
			return true;
		} else {
			return false;
		}
    }


    bool _connect(	const int timeout,			///< timeout in milliseconds (input)
			NOVRA_ERRORS &ErrorCode,
			ParameterSet parameter_set=CURRENT,
			bool passive=false	);

	virtual void initAttributes()=0;
	virtual bool setFixedParameters( unsigned char *buffer )=0;
	virtual bool createIndexedParameters()=0;
	virtual void cancelIndexedParameters()=0;
	virtual void clearIndexedParameters()=0;
//    virtual ParameterValue indexedParameter( ReceiverParameter ParameterName, int index, ParameterSet parameter_set=CURRENT )=0;
	virtual bool checkStatusPacket( string ip, unsigned char *buffer )=0;
//	virtual string macFromStatus( unsigned char *buffer )=0;
	virtual string fixedParamFromStatus( ReceiverParameter p, unsigned char* buffer )=0;
	virtual string dynamicParamFromStatus( ReceiverParameter p, unsigned char* buffer, int index=0 )=0;
	virtual int numConfigPorts()=0;
	virtual unsigned short configPort( int i )=0;

	bool sendConfig( int socket_index, unsigned char *packet, int packet_size );

	bool getConfig( int socket_index,
					unsigned char *request_packet,
					unsigned char *reply_packet,
					int request_packet_size, 
					int reply_packet_size			);

	void setRemote() { m_remote = true; }

	void countParameter( ReceiverParameter ParameterName );

	unsigned short *parameterMask;
	ParameterValue **pendingParameters;
	ParameterValue **currentParameters;
	bool *dirtyFlags;
	unsigned char m_status_packet[1500];
	int status_socket;
	int *config_sockets;
	bool m_remote;
	ParameterValue nullParameter;

private :

	void configToXML( TiXmlDocument *doc );
	void statusToXML( TiXmlDocument *doc );
	void definitionToXML( TiXmlDocument *doc );
	void toXML( TiXmlDocument *doc );
};


//typedef vector<Receiver*> ReceiverList;


#endif // RECEIVER_H

