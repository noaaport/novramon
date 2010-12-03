#ifndef _RECEIVER_SEARCH

#define _RECEIVER_SEARCH

#include <string>

#include "Receiver.h"
#include "ReceiverList.h"


class ReceiverList;

class Receiver;

using namespace std;

// class ReceiverSearch
/** 
  * Local discovery of Novra receivers requires that the receivers use a port from a specific list of ports.
  * The class provides some helper functions the programmer can use in order to work within this list.  This class is
  * also used to make connections to receivers that can not be discovered locally.
  */

class ReceiverSearch	/// This class encapsulates methods for obtaining initial contact with Novra receivers.
{
public:

    ReceiverSearch ( );

    virtual ~ReceiverSearch ( );

	/**
	*
	* Listens to all the ports on the auto-detect list for status packets coming from Novra receivers, creates a Receiver object for each unique receiver and adds them to a list.
	*
	* @param receiverList a pointer to the ReceiverList to add the Receiver objects to
	*
	* @return success flag
	*/
    bool discoverLocal( ReceiverList *receiverList );


	/**
	*
	* Contacts a receiver using an IP address and creates a Receiver object for it (if it was found)
	*
	* @param ip the IP address of a receiver
	* @param timeout time to wait for a reply from the receiver
	* @param retries the number of times to try making contact with the receiver
	*
	* @return a pointer to a Receiver object or a NULL pointer if no contact was made with a recognizable receiver
	*
	*/
	Receiver* getRemote( string ip, int timeout=2000, int retries=4 );


	/**
	* @return number of auto-detect ports
	*/
	static int numAutoDetectPorts();


	/**
	* @param index port at position index (0..number of auto-detect ports minus 1)
	* 
	* @return port number
	*/
	static unsigned short autoDetectPort( int index );


	/**
	* @param port port number
	*
	* @return index of port in the list of auto-detect ports, or -1 if the port is not in the list
	*/
	static int portIndex( unsigned short port );


	/**
	* Finds a port from the auto-detect list, that is not being used on the host PC
	*
	* @param port a reference to a variable to hold a useable port value
	*
	* @return error code (N_ERROR_SUCCESS on success)
	*/
	static NOVRA_ERRORS GetAvailablePort( unsigned short &port);

};


#endif

