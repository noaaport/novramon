#ifndef SJ300BASE_H
#define SJ300BASE_H

#include "SJBase.h"


/**
  * class SJ300Base
  * 
  */


class SJ300Base : public SJBase
{
public:

	SJ300Base();
	SJ300Base( unsigned char *buffer );
	~SJ300Base();

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

