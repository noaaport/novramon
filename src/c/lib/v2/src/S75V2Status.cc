// S75V2Status.cpp: implementation of the S75V2Status class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S75V2Status.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

S75V2Status::S75V2Status( void *Container )
{
	status = (S75D_Status *)Container;
}

S75V2Status::~S75V2Status()
{

}
