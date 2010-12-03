//#include "stdafx.h"

#include <stdio.h>
#ifndef WINDOWS
	#include <sys/time.h>
#endif
#ifdef WINDOWS
	#include "Windows.h"
#endif

#include <string>

using namespace std;

#ifndef WINDOWS
string timestamp()
{

	struct timeval tv;
	struct tm *time_date;;
	char time_str[30];


//	memset( time_str, 0, sizeof( time_str ) );
	gettimeofday( &tv, NULL );
	time_date = gmtime( &(tv.tv_sec) );
	sprintf( time_str, "%d/%02d/%02d %d:%02d:%02d.%03d",
		 1900+time_date->tm_year, time_date->tm_mon+1, time_date->tm_mday, time_date->tm_hour, time_date->tm_min, time_date->tm_sec, (int)( tv.tv_usec / 1000 ) );

	return string( time_str );

}
#endif


#ifdef WINDOWS
string timestamp()
{
	SYSTEMTIME time_date;
	char time_str[30];

	GetSystemTime( &time_date );
	sprintf( time_str, "%d/%02d/%02d %d:%02d:%02d.%03d",
			 time_date.wYear, time_date.wMonth, time_date.wDay, time_date.wHour, time_date.wMinute, time_date.wSecond, time_date.wMilliseconds );

	return string( time_str );

}
#endif
