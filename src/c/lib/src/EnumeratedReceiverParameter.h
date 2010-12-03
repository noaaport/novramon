#ifndef ENUMERATED_RECEIVER_PARAMETER_H
#define ENUMERATED_RECEIVER_PARAMETER_H

#include "RecComLib.h"
#include "ParameterValue.h"
#include "ReceiverParameter.h"


/**
  * class EnumeratedReceiverParameter
  * 
  */


class EnumeratedReceiverParameter : public ParameterValue
{
public:

	EnumeratedReceiverParameter( );
	EnumeratedReceiverParameter( ReceiverParameter p );
	EnumeratedReceiverParameter( ReceiverParameter p, string value );
	~EnumeratedReceiverParameter();
	string asString();
	void setEnumType( ReceiverParameter p );

private:

	ReceiverParameter m_p;
	
};


#endif

