///////////////////////////////////////////////////////////////////////////////
//	Copyright (c) 2010  by Novra Technologies Inc.
//
//	Novra Technologies Inc. 
//	900-330 St. Mary Avenue
//	Winnipeg, Manitoba R3C 3Z5
//	Telephone (204) 989-4724
//
//  All rights reserved. The copyright for the computer program(s) contained 
//	herein is the property of Novra Technologies Inc. No part of the computer 
//	program(s) contained herein may be reproduced or transmitted in any form by 
//	any means without direct written permission from Novra Technologies Inc. or 
//	in accordance with the terms and conditions stipulated in the 
//	agreement/contract under which the program(s) have been supplied.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//						File:		CPassword.cpp
// 		  		Author:		Michael S. Stasenski
//						Date:   Sept. 13, 2010
// 				Comments:   Implemnation of the SHA1
//
//	 Documentation:		RFC 3174 September 2001
//										US Secure Hash Algorithm 1 (SHA1)
//										Donald E. Eastlake, 3rd (Motorola)
//										Paul E. Jones (Cisco Systems)
//
//				Compiler:   Visual C++ 6.0
///////////////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "sha1.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

sha1::sha1()
{

}

sha1::~sha1()
{

}

////////////////////////////////////////////////////////////////////////////////
//			Function:		SHA1()
//	 Description:	  'C' implementation of the SHA-1 hash algorithm.
//    Arguments:		* to message digest result
//									* to message
//									message length in 16-bit words (54xx format)
//		   Returns:		error code
int
sha1::SHA1(unsigned char *text, int len)
{
 int i;

 	// 0 length not allowed
 	if (!len)
 		return(1);
 	
	// initialize SHA-1 algorithm parameters
 	Hash[0] = 0x67452301;
	Hash[1] = 0xefcdab89;
	Hash[2] = 0x98badcfe;
	Hash[3] = 0x10325476;
	Hash[4] = 0xc3d2e1f0;
 	
	// process message block(s)
 	SHA1ProcessMessageBlocks(text, len);
 	
	// load SHA-1 digest
 	for( i = 0; i < SHA1HASHSIZE/4; i++)
	{
		digest[i*4] = Hash[i] >> 24;
		digest[i*4 + 1] = Hash[i] >> 16;
 		digest[i*4 + 2] = Hash[i] >> 8;
 		digest[i*4 + 3] = Hash[i];
 	} 	
 	return(0);
}

////////////////////////////////////////////////////////////////////////////////
//			Function:		SHA1ProcessMessageBlocks
//	 Description:		Process message blocks
void
sha1::SHA1ProcessMessageBlocks(unsigned char *msg, int len)
{
 	unsigned int numberofbits;
 	
		// calculate the number of bits in the message
 		numberofbits = len * 8;
 		
		// process message in 512-bit blocks
 		while (len >= 64)
		{	
 			// Move message into block buffer
 			memmove(M, msg, 64);
 			
			// update msg pointer and length field
 			msg += 64;
 			len -= 64;
			
			// process message block
			SHA1ProcessMessageBlock();
		}

		// process block size < 512-bits
		// Erase message block buffer
 		memset(M, 0, sizeof(M));
 		
		// Move message into block buffer
 		memmove(M, msg, len);
 		
		// Set ms bit of message pad
 		M[len] = 0x80;  		
		if (len < 56)
		{
			// last block with room for message pad flag and length field
 			// Set message length field to the number of bits in the message
 			// not including the message pad or length fields
 	 		M[62] = numberofbits>>8;
			M[63] = (unsigned char)numberofbits;
			
			// process block
			SHA1ProcessMessageBlock();
			return;
		}
		else
		{
	 		// add a block to fit the length field
			// process previous block < 512 bits
			SHA1ProcessMessageBlock();	
			
			// Erase message block buffer
 			memset(M, 0, sizeof(M));
			
			// Set message length field to the number of bits in the message
 			// not including the message pad or length fields
 	 		M[62] = numberofbits>>8;
			M[63] = (unsigned char)numberofbits;
			
			// process block
			SHA1ProcessMessageBlock();
		}
}

////////////////////////////////////////////////////////////////////////////////
//			Function:		SHA1ProcessMessageBlock
//	 Description:		Process message block
void
sha1::SHA1ProcessMessageBlock()
{
	// constants defined in SHA-1
 	const unsigned int
	K[] = {
 					0x5a827999,
 					0x6ed9eba1,
 					0x8f1bbcdc,
 					0xca62c1d6
 				};
	unsigned int t;							// Loop counters
	unsigned int Temp;					// Temporary word count
	unsigned int W[80];					// Word sequence
	unsigned int A, B, C, D, E;	// Word buffers

		// Divide M(i) into 16 32-bit lwords W[0], W[1] ..., W[15]
		// where W[0] is the left-most word.
		for (t = 0; t < 16; t++)	{
				W[t] =  M[t*4 + 3];
				W[t] |= M[t*4 + 2] << 8;
				W[t] |= M[t*4 + 1] << 16;
				W[t] |= M[t*4] << 24;			
				}	 		
		
		// For t = 16 to 79 let
		// W(t) = S^1(W(t-3) XOR W(t-8) XOR W(t-14) XOR W(t-16))	
		for ( t = 16; t < 80; t++)
			W[t] = CircShift((W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]),1);
		
		// Let A = H0, B = H1, C = H2, D = H3 , E = H4
		A = Hash[0];
		B = Hash[1];
		C = Hash[2];
		D = Hash[3];
		E = Hash[4];
		
		// For t = 0 to 79 Do
		for ( t = 0 ; t < 20 ; t++)	{
			Temp = CircShift(A,5) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
			E = D; D = C; C = CircShift(B,30); B = A; A = Temp;
			}
		for (t = 20; t < 40 ; t++)	{
			Temp = CircShift(A,5) + (B ^ C ^ D) + E + W[t] + K[1];
			E = D; D = C; C = CircShift(B,30); B = A; A = Temp;
			}		
		for ( t = 40 ; t < 60 ; t++)	{
			Temp = CircShift(A,5) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
			E = D; D = C; C = CircShift(B,30); B = A; A = Temp;
			}		
		for (t = 60; t < 80 ; t++)	{
			Temp = CircShift(A,5) + (B ^ C ^ D) + E + W[t] + K[3];
			E = D; D = C; C = CircShift(B,30); B = A; A = Temp;
			}
		
		// Let H0 = H0 + A, H1 = H1 + B, H2 = H2 + C, H3 = H3 + D, H4 = H4 + E
		Hash[0] += A;
		Hash[1] += B;
		Hash[2] += C;
		Hash[3] += D;
		Hash[4] += E;
}
