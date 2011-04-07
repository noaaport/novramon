//#include "stdafx.h"

#include "ReceiverList.h"


ReceiverList::ReceiverList()
{
}


ReceiverList::~ReceiverList()
{
}


Receiver* ReceiverList::findReceiver( string mac, string ip )
{
	vector<Receiver*>::iterator receiver;
	bool found = false;
	Receiver *returned_receiver = NULL;


	receiver = receiverList.begin();

	while ( !found && ( receiver != receiverList.end() ) ) {
		if ( mac == "" ) {
			found = (*receiver)->matches( RECEIVER_IP, ip );
		} else {
			found = (*receiver)->matches( RECEIVER_MAC, mac );
			if ( found && ( ip != "" ) ) {
				found = (*receiver)->matches( RECEIVER_IP, ip );
			}
		}
		if ( found ) {
			returned_receiver = *receiver;
		} else {
			receiver++;
		}
	}
	
	return returned_receiver;

}



