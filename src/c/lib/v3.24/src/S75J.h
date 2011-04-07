#ifndef S75J_H
#define S75J_H

#include "Receiver.h"

#define ID_S75J 0x55

#define NUM_PIDS_S75J 16

/**
  * class S75J
  * 
  */


class S75J : public Receiver
{
public:

	S75J();
	S75J( unsigned char *buffer );
	~S75J();

	bool login( string password );

	bool apply( ReceiverParameter ParameterName=ALL_PARAMETERS );
	bool reset();
	bool changePassword( char *password, unsigned char key[16] );

protected:

	int numConfigPorts();
	unsigned short configPort( int i );
	bool readCurrentSettings();
	int typeIDCode() { return ID_S75J; };
	int receiverTypeCode() { return R_S75J; }

	unsigned short *config_ports;

private:

	void initAttributes();
	bool setFixedParameters( unsigned char *buffer );
	bool createIndexedParameters();
	bool setIndexedParameter( ReceiverParameter ParameterName, int index, string ParameterValue );
	ParameterValue& getIndexedParameter( ReceiverParameter ParameterName, int index, ParameterSet parameter_set=CURRENT );
	void cancelIndexedParameters();
	void clearIndexedParameters();

	bool checkStatusPacket( string ip, unsigned char *buffer );
//	string macFromStatus( unsigned char *buffer );
	string fixedParamFromStatus( ReceiverParameter p, unsigned char* buffer );
	string dynamicParamFromStatus( ReceiverParameter p, unsigned char* buffer, int index=0 );
	bool connected( int timeout );
	bool applyNetworkConfig();
	bool applyRFConfig();
	bool applyPIDConfig();
	bool pollStatus( unsigned char *packet, int timeout );
//	bool getPIDList( unsigned char *pids );
	bool readPIDTable();


	ParameterValue *currentPIDs;
	ParameterValue *pendingPIDs;

};


#endif


