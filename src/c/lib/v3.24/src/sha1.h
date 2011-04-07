// sha1.h: interface for the sha1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHA1_H__D646E2E7_DACC_4295_AA30_90726A48BFA5__INCLUDED_)
#define AFX_SHA1_H__D646E2E7_DACC_4295_AA30_90726A48BFA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _SHA1_
#define _SHA1_

#ifdef WINDOWS
	#include <string>
#else
	#include <stdlib.h>
	#include <string.h>
#endif

#define SHA1HASHSIZE 20
#define CircShift(x,n) (((x) << (n)) | ((x) >> (32-(n))))
#define SIZEOFSIGNATURE 20

#endif

class sha1  
{
public:
	unsigned int  Hash[SHA1HASHSIZE/4];		// Message Digest
	unsigned char M[64];									// 512-bit message block
	unsigned char digest[20];							// 160-bit digital signature/digest

public:
	void SHA1ProcessMessageBlocks(unsigned char *msg, int len);
	void SHA1ProcessMessageBlock();
	int  SHA1(unsigned char *text, int len);


public:
	sha1();
	virtual ~sha1();

};

#endif // !defined(AFX_SHA1_H__D646E2E7_DACC_4295_AA30_90726A48BFA5__INCLUDED_)
