#ifndef SJ300_H
#define SJ300_H

#include "SJ300Base.h"

#define ID_SJ300 0x90


/**
  * class SJ300
  * 
  */


class SJ300 : public SJ300Base
{
public:

	SJ300();
	SJ300( unsigned char *buffer );
	virtual ~SJ300();

private:

	void initAttributes();
	int typeIDCode() { return ID_SJ300; };
	int receiverTypeCode() { return R_SJ300; }

};


#endif

