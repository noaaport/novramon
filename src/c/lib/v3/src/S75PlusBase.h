#ifndef S75PLUSBASE_H
#define S75PLUSBASE_H

#include "SPlusBase.h"


/**
  * class S75PlusBase
  * 
  */


class S75PlusBase : public SPlusBase
{
public:

	S75PlusBase();
	S75PlusBase( unsigned char *buffer );
	~S75PlusBase();

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

