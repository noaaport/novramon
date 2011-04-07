#ifndef SPLUSBASE_H
#define SPLUSBASE_H

#include "Receiver.h"


/**
  * class SPlusBase
  * 
  */


class SPlusBase : public Receiver
{
public:

	SPlusBase();
	SPlusBase( unsigned char *buffer );
	~SPlusBase();

	bool login( string password );

	bool apply( ReceiverParameter ParameterName=ALL_PARAMETERS );
	virtual bool reset()=0;
	bool changePassword( char *password, unsigned char key[16] );

protected:

	int numConfigPorts();
	unsigned short configPort( int i );
	bool readCurrentSettings();
	virtual int typeIDCode() { return 0; }
	virtual int receiverTypeCode() { return R_UNKNOWN; }
	bool setFixedParameters( unsigned char *buffer );
	bool createIndexedParameters();
	bool connected( int timeout );

	unsigned short *config_ports;


private:

	virtual void initAttributes()=0;
	bool setIndexedParameter( ReceiverParameter ParameterName, int index, string ParameterValue );
	ParameterValue& getIndexedParameter( ReceiverParameter ParameterName, int index, ParameterSet parameter_set=CURRENT );
	void cancelIndexedParameters();
	void clearIndexedParameters();

	bool checkStatusPacket( string ip, unsigned char *buffer );
//	string macFromStatus( unsigned char *buffer );
	virtual string fixedParamFromStatus( ReceiverParameter p, unsigned char* buffer )=0;
	virtual string dynamicParamFromStatus( ReceiverParameter p, unsigned char* buffer, int index=0 )=0;
	virtual bool applyNetworkConfig()=0;
	virtual bool applyRFConfig()=0;
	bool applyPIDConfig();
	bool applyPATConfig();
	bool applyCAConfig();
	bool applyCAMWatchdogInterval();
	bool applyFixedKeys();
	bool applyIPRemapRules();
	bool applyCipherSaber();
	virtual bool pollStatus( unsigned char *packet, int timeout )=0;
//	bool getPIDList( unsigned char *pids );
	bool readPIDTable();
	bool readPATTable();
	bool readCATable();
	bool readCAMWatchdogInterval();
	bool readFixedKeys();
	bool readIPRemapRules();
	bool readCipherSaber();


	ParameterValue *currentPIDs;
	ParameterValue *pendingPIDs;
	EnumeratedReceiverParameter *currentRawForwardFlags;
	EnumeratedReceiverParameter *pendingRawForwardFlags;
	ParameterValue *currentDestinationMasks;
	ParameterValue *pendingDestinationMasks;
	ParameterValue *currentPIDDestinationIPs;
	ParameterValue *pendingPIDDestinationIPs;
	ParameterValue *currentPIDDestinationPorts;
	ParameterValue *pendingPIDDestinationPorts;
	ParameterValue *currentProgramNumbers;
	ParameterValue *pendingProgramNumbers;
	ParameterValue *currentPMTPIDs;
	ParameterValue *pendingPMTPIDs;
	ParameterValue *currentCAProgramNumbers;
	ParameterValue *pendingCAProgramNumbers;
	EnumeratedReceiverParameter *CAProgramStatus;
	ParameterValue *currentFKPIDs;
	ParameterValue *pendingFKPIDs;
	EnumeratedReceiverParameter *currentFKSigns;
	EnumeratedReceiverParameter *pendingFKSigns;
	ParameterValue *currentFKs;
	ParameterValue *pendingFKs;
	ParameterValue *currentOriginalIPs;
	ParameterValue *pendingOriginalIPs;
	ParameterValue *currentNewIPs;
	ParameterValue *pendingNewIPs;
	ParameterValue *currentIPRemapMasks;
	ParameterValue *pendingIPRemapMasks;
	ParameterValue *currentIPRemapTTLs;
	ParameterValue *pendingIPRemapTTLs;
	EnumeratedReceiverParameter *currentIPRemapActions;
	EnumeratedReceiverParameter *pendingIPRemapActions;
	
};


#endif

