#ifndef S75V21_H
#define S75V21_H

#include "Receiver.h"

#define ID_S75V21 0x32

#define NUM_PIDS_S75V21 32

/**
  * class S75V21
  * 
  */


class S75V21 : public Receiver
{
public:

	S75V21();
	S75V21( unsigned char *buffer );
	~S75V21();

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

