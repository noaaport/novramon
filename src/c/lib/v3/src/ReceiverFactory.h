#ifndef RECEIVER_FACTORY_H
#define RECEIVER_FACTORY_H

#include <string>

#include "Receiver.h"
#include "S75V21.h"
#include "S75V3.h"
#include "S75V.h"
#include "A75.h"
#include "A75V.h"
#include "S75J.h"
#include "S75PLUS.h"
#include "S75PLUSPRO.h"
#include "S75CA.h"
#include "S75CAPRO.h"
#include "S75FK.h"
#include "S75FKPRO.h"
#include "S75IPREMAP.h"
#include "S200.h"
#include "S200PRO.h"
#include "S200CA.h"
#include "S200CAPRO.h"
#include "S200FK.h"
#include "S200FKPRO.h"
#include "S200V.h"
#include "S200VCA.h"
#include "S200J.h"
#include "SJ300.h"


using namespace std;


class ReceiverFactory
{
public:

	ReceiverFactory() {};
	~ReceiverFactory() {};


//	NOVRA_DEVICE DeviceTypeFromStatus( unsigned char *buffer );

	string MACFromStatus( unsigned char *buffer );

	Receiver* Create( unsigned char *buffer, string ip = "", int status_port = 0 );

};


#endif

