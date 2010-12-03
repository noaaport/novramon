#ifndef S200J_H
#define S200J_H

#include "S200Base.h"

#define ID_S200J 0x77


/**
  * class S200
  * 
  */


class S200J : public S200Base
{
public:

	S200J();
	S200J( unsigned char *buffer );
	~S200J();

private:

	void initAttributes();
	int typeIDCode() { return ID_S200J; };
	int receiverTypeCode() { return R_S200J; }

};


#endif

