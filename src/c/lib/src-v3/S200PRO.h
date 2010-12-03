#ifndef S200PRO_H
#define S200PRO_H

#include "S200Base.h"

#define ID_S200PRO 0x71


/**
  * class S200PRO
  * 
  */


class S200PRO : public S200Base
{
public:

	S200PRO();
	S200PRO( unsigned char *buffer );
	~S200PRO();

private:

	void initAttributes();
	int typeIDCode() { return ID_S200PRO; };
	int receiverTypeCode() { return R_S200PRO; }
	
};


#endif

