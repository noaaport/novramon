#ifndef S200CAPROCS_H
#define S200CAPROCS_H

#include "S200Base.h"

#define ID_S200CAPROCS 0x78


/**
  * class S200CAPROCS
  * 
  */


class S200CAPROCS : public S200Base
{
public:

	S200CAPROCS();
	S200CAPROCS( unsigned char *buffer );
	virtual ~S200CAPROCS();

private:

	void initAttributes();
	int typeIDCode() { return ID_S200CAPROCS; };
	int receiverTypeCode() { return R_S200CAPROCS; }
	
};


#endif
