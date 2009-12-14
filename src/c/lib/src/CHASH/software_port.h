#ifndef SOFTWARE_PORT

#define SOFTWARE_PORT

//#ifdef WINDOWS

	#define EMPTY(x) 		x.Empty()
	#define LENGTH(x) 		x.GetLength()
	#define GET_AT(x,n) 		x.GetAt(n)
	#define UPPER(x) 		x.MakeUpper()
	#define GET_BUFFER(x,n) 	x.GetBuffer(n)
	#define IS_EMPTY(x) 		x.IsEmpty()
	#define ITOA(i,s,r)		_itoa(i,s,r)

//#endif

#ifdef LINUX

	#include "stdio.h"

	#include <string>

	#define CString string

//	void MakeUpper( string *x )
//	{
//		for ( int i = 0; i < x->length(); i++ ) {
//			x->replace(i,1,toupper(x->at(i)));
//		}
//	}

	#define EMPTY(x) 		x = ""
	#define LENGTH(x) 		x.length()
	#define GET_AT(x,n) 		x.at(n)
	#define UPPER(x) 		for ( int i = 0; i < x.length(); i++ ) { x.replace(i,1,toupper(x.at(i))); }
	#define GET_BUFFER(x,n) 	x.c_str()
	#define IS_EMPTY(x) 		x.empty()
	#define ITOA(i,s,r)		sprintf(s,"%d",i%r)


#endif

#endif
