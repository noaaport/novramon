//#include "stdafx.h"

#define VERSION_MAJOR	3
#define VERSION_MINOR	24
#define VERSION_DAY		11
#define VERSION_MONTH	03
#define VERSION_YEAR	2011


void getVersion( int *major, int *minor, int *day, int *month, int *year )
{
	// Returns the version of the library and the release date

	*major	= VERSION_MAJOR;
	*minor	= VERSION_MINOR;
	*day	= VERSION_DAY;
	*month	= VERSION_MONTH;
	*year	= VERSION_YEAR;
}
