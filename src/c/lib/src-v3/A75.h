#ifndef A75_H
#define A75_H

#include "Receiver.h"

#define ID_A75 0x43

#define NUM_PIDS_A75 16

/**
  * class A75
  * 
  */


class A75 : public Receiver
{
public:

	A75();
	A75( unsigned char *buffer );
	~A75();


	bool apply( ReceiverParameter ParameterName=ALL_PARAMETERS ) { return false; }
	bool reset() { return false; }
	bool changePassword( char *password, unsigned char key[16] ) { return false; }

protected:

	int numConfigPorts() { return 0; }
	unsigned short configPort( int i ) { return 0; }
	bool readCurrentSettings() { return false; }

private:

	void initAttributes();
	bool setFixedParameters( unsigned char *buffer );
	bool createIndexedParameters() { return false; }
	bool setIndexedParameter( ReceiverParameter ParameterName, int index, string ParameterValue ) { return false; }
    ParameterValue& getIndexedParameter( ReceiverParameter ParameterName, int index, ParameterSet parameter_set=CURRENT ) { return nullParameter; }
	void cancelIndexedParameters() { return; }
	void clearIndexedParameters() { return; }

	bool checkStatusPacket( string ip, unsigned char *buffer );
//	string macFromStatus( unsigned char *buffer );
	string fixedParamFromStatus( ReceiverParameter p, unsigned char* buffer );
	string dynamicParamFromStatus( ReceiverParameter p, unsigned char* buffer, int index=0 );
	bool connected( int timeout );
	bool getPIDList( unsigned char *pids ) { return false; }

};


#endif

