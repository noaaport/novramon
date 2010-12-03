#ifndef S75PLUSPRO_H
#define S75PLUSPRO_H

#include "S75PlusBase.h"

#define ID_S75PLUSPRO 0x68

/**
  * class S75PLUSPRO
  * 
  */


class S75PLUSPRO : public S75PlusBase
{
public:

	S75PLUSPRO();
	S75PLUSPRO( unsigned char *buffer );
	~S75PLUSPRO();

private:

	void initAttributes();
	int typeIDCode() { return ID_S75PLUSPRO; };
	int receiverTypeCode() { return R_S75PLUSPRO; }

};


#endif

