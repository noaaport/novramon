//#include "stdafx.h"

#define VERSION_MAJOR	3
#define VERSION_MINOR	13
#define VERSION_DAY		30
#define VERSION_MONTH	11
#define VERSION_YEAR	2010


void getVersion( int *major, int *minor, int *day, int *month, int *year )
{
	// Returns the version of the library and the release date

	*major	= VERSION_MAJOR;
	*minor	= VERSION_MINOR;
	*day	= VERSION_DAY;
	*month	= VERSION_MONTH;
	*year	= VERSION_YEAR;
}
