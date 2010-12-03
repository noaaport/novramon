#ifndef S75IPREMAP_H
#define S75IPREMAP_H

#include "S75PlusBase.h"

#define ID_S75IPREMAP 0x6C


/**
  * class S75IPREMAP
  * 
  */


class S75IPREMAP : public S75PlusBase
{
public:

	S75IPREMAP();
	S75IPREMAP( unsigned char *buffer );
	~S75IPREMAP();



private:

	void initAttributes();
	int typeIDCode() { return ID_S75IPREMAP; };
	int receiverTypeCode() { return R_S75IPREMAP; }

};


#endif


