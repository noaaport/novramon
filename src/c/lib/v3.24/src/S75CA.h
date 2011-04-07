#ifndef S75CA_H
#define S75CA_H

#include "S75PlusBase.h"

#define ID_S75CA 0x67

/**
  * class S75CA
  * 
  */


class S75CA : public S75PlusBase
{
public:

	S75CA();
	S75CA( unsigned char *buffer );
	virtual ~S75CA();

private:

	void initAttributes();
	int typeIDCode() { return ID_S75CA; };
	int receiverTypeCode() { return R_S75CA; }

};


#endif

