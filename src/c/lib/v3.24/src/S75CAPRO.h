#ifndef S75CAPRO_H
#define S75CAPRO_H

#include "S75PlusBase.h"

#define ID_S75CAPRO      0x66
#define ID_S75CAPRO_BETA 0x76

/**
  * class S75CAPRO
  * 
  */


class S75CAPRO : public S75PlusBase
{
public:

	S75CAPRO();
	S75CAPRO( unsigned char *buffer );
	virtual ~S75CAPRO();

private:

	void initAttributes();
	int typeIDCode() { return ID_S75CAPRO; };
	int receiverTypeCode() { return R_S75CAPRO; }

};


#endif

