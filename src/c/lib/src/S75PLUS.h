#ifndef S75PLUS_H
#define S75PLUS_H

#include "S75PlusBase.h"

#define ID_S75PLUS 0x69

/**
  * class S75PLUS
  * 
  */


class S75PLUS : public S75PlusBase
{
public:

	S75PLUS();
	S75PLUS( unsigned char *buffer );
	~S75PLUS();

private:

	void initAttributes();
	int typeIDCode() { return ID_S75PLUS; };
	int receiverTypeCode() { return R_S75PLUS; }
};


#endif

