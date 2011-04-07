#ifndef S75FK_H
#define S75FK_H

#include "S75PlusBase.h"

#define ID_S75FK 0x6B

/**
  * class S75FK
  * 
  */


class S75FK : public S75PlusBase
{
public:

	S75FK();
	S75FK( unsigned char *buffer );
	virtual ~S75FK();

private:

	void initAttributes();
	int typeIDCode() { return ID_S75FK; };
	int receiverTypeCode() { return R_S75FK; }

};


#endif

