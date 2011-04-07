#ifndef S200_H
#define S200_H

#include "S200Base.h"

#define ID_S200 0x70


/**
  * class S200
  * 
  */


class S200 : public S200Base
{
public:

	S200();
	S200( unsigned char *buffer );
	virtual ~S200();

private:

	void initAttributes();
	int typeIDCode() { return ID_S200; };
	int receiverTypeCode() { return R_S200; }

};


#endif

