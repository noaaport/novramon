#ifndef S200CA_H
#define S200CA_H

#include "S200Base.h"

#define ID_S200CA 0x72


/**
  * class S200CA
  * 
  */


class S200CA : public S200Base
{
public:

	S200CA();
	S200CA( unsigned char *buffer );
	virtual ~S200CA();

private:

	void initAttributes();
	int typeIDCode() { return ID_S200CA; };
	int receiverTypeCode() { return R_S200CA; }

};


#endif

