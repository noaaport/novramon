#ifndef S200FK_H
#define S200FK_H

#include "S200Base.h"

#define ID_S200FK 0x74


/**
  * class S200FK
  * 
  */


class S200FK : public S200Base
{
public:

	S200FK();
	S200FK( unsigned char *buffer );
	virtual ~S200FK();

private:

	void initAttributes();
	int typeIDCode() { return ID_S200FK; };
	int receiverTypeCode() { return R_S200FK; }

};


#endif

