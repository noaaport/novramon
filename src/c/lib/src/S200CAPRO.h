#ifndef S200CAPRO_H
#define S200CAPRO_H

#include "S200Base.h"

#define ID_S200CAPRO 0x73


/**
  * class S200CAPRO
  * 
  */


class S200CAPRO : public S200Base
{
public:

	S200CAPRO();
	S200CAPRO( unsigned char *buffer );
	~S200CAPRO();

private:

	void initAttributes();
	int typeIDCode() { return ID_S200CAPRO; };
	int receiverTypeCode() { return R_S200CAPRO; }
	
};


#endif

