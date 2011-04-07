#ifndef RECEIVER_LIST_H
#define RECEIVER_LIST_H

#include <string>
#include <vector>

#ifdef VS6
	#include <iostream.h>
#else
	#include <iostream>
#endif

#include "Receiver.h"
#include "ReceiverParameter.h"
#include "ParameterValue.h"


using namespace std;

class Receiver;


// class ReceiverList
/** 
  * Typically it will be returned by the ReceiverSearch class but a programmer could also build one using the addReceiver() member function.
  *
  * Because Novra didn't need all the usual list manipulation functions like delete, sort, modify and so on, these have not yet been provided.  However the class is just a thin veneer for the C++ standard template library object 'vector'.  By using the list() member function the programmer can reference the underlying vector class and use all its member functions available from the standard template library.
  */
class ReceiverList	/// Represents a list of Receivers
{
public:
	
	ReceiverList();
	~ReceiverList();

    /**
     * @return number of receivers in the list
     */
	int count() { return receiverList.size(); }

    /**
     * Empties the list
     */
	void clear() { receiverList.clear(); }

    /**
	 * Adds a receiver to the list.
	 *
     * @param receiver pointer to a receiver object
     */
	void addReceiver( Receiver *receiver )	{ receiverList.push_back( receiver );}

    /**
	 * Gets a receiver from the list using an index.
	 *
     * @param i list index (index starts at 0)
	 * @return pointer to the Receiver at position i in the list
     */
	Receiver* getReceiver( int i ) { return receiverList[i]; }

    /**
	 * Finds a receiver (if present) in the list using a MAC address. 
	 * If the optional IP address is specified then the receiver must match both.
	 *
     * @param mac receiver MAC address expressed in the form xx-xx-xx-xx-xx-xx
	 * @param ip receiver IP address
	 * @return pointer to the matching receiver or NULL if not found.
     */
	Receiver* findReceiver(	string mac, string ip = "" );

	/**
	 * Provides a reference to the underlying vector object.
	 *
	 * @return a vector of Receiver pointers (vector belongs to the C++ standard template library)
	 */
	vector<Receiver*> list() { return receiverList; }

private:

	vector<Receiver*> receiverList;

};

#endif

