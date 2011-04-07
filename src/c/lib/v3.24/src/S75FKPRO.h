#ifndef S75FKPRO_H
#define S75FKPRO_H

#include "S75PlusBase.h"

#define ID_S75FKPRO 0x6A

/**
  * class S75FKPRO
  * 
  */


class S75FKPRO : public S75PlusBase
{
public:

	S75FKPRO();
	S75FKPRO( unsigned char *buffer );
	virtual ~S75FKPRO();

private:

	void initAttributes();
	int typeIDCode() { return ID_S75FKPRO; };
	int receiverTypeCode() { return R_S75FKPRO; }

};


#endif

