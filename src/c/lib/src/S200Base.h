#ifndef S200BASE_H
#define S200BASE_H

#include "SPlusBase.h"


/**
  * class S200Base
  * 
  */


class S200Base : public SPlusBase
{
public:

	S200Base();
	S200Base( unsigned char *buffer );
	~S200Base();

	bool reset();

protected:

	virtual int typeIDCode() { return 0; }
	virtual int receiverTypeCode() { return R_UNKNOWN; }

private:

	virtual void initAttributes()=0;
	string fixedParamFromStatus( ReceiverParameter p, unsigned char* buffer );
	string dynamicParamFromStatus( ReceiverParameter p, unsigned char* buffer, int index=0 );
	bool applyRFConfig();
	bool applyNetworkConfig();
	bool pollStatus( unsigned char *packet, int timeout );

};


#endif

