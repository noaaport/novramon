// S75CAStatus.cpp: implementation of the S75CAStatus class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S75CAStatus.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

S75CAStatus::S75CAStatus( void *Container )
{
	status = (S75CA_Status *)Container;
}

S75CAStatus::~S75CAStatus()
{

}
