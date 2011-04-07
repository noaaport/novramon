//#include "stdafx.h"

#include "Password.h"

void encryptCSAkeys(unsigned short * ptrKeyTable, int numKeys )
 {
 unsigned char secretKey[] = {	0xfe, 0xc5, 0x5d, 0x1f,
								0xe0, 0x67, 0x71, 0xa4,
								0x28, 0xcd, 0x0c, 0x8e,
								0x6d, 0x6e, 0xd0, 0x8c };
 unsigned int keySchedule = 2264783973;
 unsigned int i, j;
 unsigned int key;
		
	 // encrypt the CSA keys
	 for ( i = 0; i < numKeys * 5; i += 4)	{
 		encrypt((unsigned char *)(&secretKey[0]), (unsigned char *)(&ptrKeyTable[i]));
		for ( j = 0; j < 4; j++ ) {
			key = secretKey[4*j+0];
			key = key << 8;
			key += secretKey[4*j+1];
			key = key << 8;
			key += secretKey[4*j+2];
			key = key << 8;
			key += secretKey[4*j+3];
			key += keySchedule;
			secretKey[4*j+3] = key & 0x000000ff;
			key = key >> 8;
			secretKey[4*j+2] = key & 0x000000ff;
			key = key >> 8;
			secretKey[4*j+1] = key & 0x000000ff;
			key = key >> 8;
			secretKey[4*j+0] = key & 0x000000ff;
		}
	}	 				 				 				 			
 } 
 
