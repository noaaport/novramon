#ifndef S200VCA_H
#define S200VCA_H

#include "Receiver.h"

#define ID_S200VCA 0x82


/**
  * class S200VCA
  * 
  */


class S200VCA : public Receiver
{
public:

	S200VCA();
	S200VCA( unsigned char *buffer );
	~S200VCA();

	bool login( string password );

	bool apply( ReceiverParameter ParameterName=ALL_PARAMETERS );
	bool applyVProgramConfig();
	bool readStreamInfo();
	bool reset();
	bool factoryReset();
	bool changePassword( char *password, unsigned char key[16] );

protected:

	int numConfigPorts();
	unsigned short configPort( int i );
	bool readCurrentSettings();
	bool setFixedParameters( unsigned char *buffer );
	bool createIndexedParameters();


private:

	int typeIDCode() { return ID_S200VCA; };
	int receiverTypeCode() { return R_S200VCA; }

	void initAttributes();
	bool setIndexedParameter( ReceiverParameter ParameterName, int index, string ParameterValue );
	ParameterValue& getIndexedParameter( ReceiverParameter ParameterName, int index, ParameterSet parameter_set=CURRENT );
	ParameterValue& getDblIndexedParameter( ReceiverParameter ParameterName, int index1, int index2, ParameterSet parameter_set	);
	void cancelIndexedParameters();
	void clearIndexedParameters();

	bool checkStatusPacket( string ip, unsigned char *buffer );
//	string macFromStatus( unsigned char *buffer );
	string fixedParamFromStatus( ReceiverParameter p, unsigned char* buffer );
	string dynamicParamFromStatus( ReceiverParameter p, unsigned char* buffer, int index=0 );
	bool applyNetworkConfig();
	bool applyRFConfig();
	bool applyPIDConfig();
	bool applyPATConfig();
	bool applyCAConfig();
	bool applyCAMWatchdogInterval();
	bool applyFixedKeys();
	bool applyIPRemapRules();
	bool connected( int timeout );
	bool pollStatus( unsigned char *packet, int timeout );
//	bool getPIDList( unsigned char *pids );
	bool readPIDTable();
	bool readPATTable();
	bool readCATable();
	bool readCAMWatchdogInterval();
	bool readFixedKeys();
	bool readIPRemapRules();
	bool readVProgramList();
	bool readProgramNames();

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
//	ParameterValue *currentPMTPIDs;
//	ParameterValue *pendingPMTPIDs;
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
	ParameterValue *currentVProgramNumbers;
	ParameterValue *pendingVProgramNumbers;
	ParameterValue *programNames;
	ParameterValue *guideProgramNumbers;
	ParameterValue *guideProgramNames;
	ParameterValue *currentVProgramDestinationIPs;
	ParameterValue *pendingVProgramDestinationIPs;
	ParameterValue *currentVProgramDestinationPorts;
	ParameterValue *pendingVProgramDestinationPorts;
	EnumeratedReceiverParameter *CASystemIDs;
	EnumeratedReceiverParameter *CAProgramScrambledFlags;
	ParameterValue *PMTPIDs;
	ParameterValue *PMTVersions;
	EnumeratedReceiverParameter *ProgramStreamTypes;
	ParameterValue *VideoPIDs;
	ParameterValue *PCRPIDs;
	ParameterValue **AudioPIDs;
	ParameterValue **AudioPIDLanguages;
	ParameterValue **AudioPIDTypes;

	unsigned short *config_ports;
	
};


#endif


