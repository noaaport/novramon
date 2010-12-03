#ifndef S200FKPRO_H
#define S200FKPRO_H

#include "S200Base.h"

#define ID_S200FKPRO 0x75


/**
  * class S200FKPRO
  * 
  */


class S200FKPRO : public S200Base
{
public:

	S200FKPRO();
	S200FKPRO( unsigned char *buffer );
	~S200FKPRO();

private:

	void initAttributes();
	int typeIDCode() { return ID_S200FKPRO; };
	int receiverTypeCode() { return R_S200FKPRO; }

};


#endif

