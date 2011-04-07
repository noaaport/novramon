#ifndef S75JPLUS_H
#define S75JPLUS_H

#include "S75PlusBase.h"

#define ID_S75JPLUS 0x69

/**
  * class S75JPLUS
  * 
  */


class S75JPLUS : public S75PlusBase
{
public:

	S75JPLUS();
	S75JPLUS( unsigned char *buffer );
	virtual ~S75JPLUS();

private:

	void initAttributes();
	int typeIDCode() { return ID_S75JPLUS; };
	int receiverTypeCode() { return R_S75JPLUS; }
};


#endif

