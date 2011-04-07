//#ifdef JSAT

/****************************************************************************
 * Copyright (c) 2005  by Novra Technologies Inc.
 *
 *Novra Technologies Inc. 
 *1100-330 St. Mary Avenue
 *Winnipeg, Manitoba R3C 3Z5
 *Telephone (204) 989-4724

 *All rights reserved. The copyright for the computer program(s) contained 
 *herein is the property of Novra Technologies Inc. No part of the computer 
 *program(s) contained herein may be reproduced or transmitted in any form by 
 *any means without direct written permission from Novra Technologies Inc. or 
 *in accordance with the terms and conditions stipulated in the 
 *agreement/contract under which the program(s) have been supplied.
 *****************************************************************************/

/*****************************************************************************
 *						File:		password.c
 * 		  		Author:		Michael S. Stasenski
 * 						Date:   May 18, 2005
 * 				Comments:   The following C source implements the password functions
 *										for the S75 JLeo DVB-S receiver.
 *
 *	 Documentation:		Receiver Configuration Messages
 *										S75-JLeo Des
 *										Rev. 1, May 11, 2005
 *										M. Nashaat T. Soliman (NOVRA)
 *
 *                		RFC 2104 February 1997
 *										HMAC: Keyed-Hashing for Message Authentication
 *										H. Krawczyk (IBM)
 *										M. Bellare (UCSD)
 *										R. Canetti (IBM)
 *
 *										RFC 3174 September 2001
 *										US Secure Hash Algorithm 1 (SHA1)
 *										Donald E. Eastlake, 3rd (Motorola)
 *										Paul E. Jones (Cisco Systems)
 *
 *										RFC 2898 PCKS #5: Password_Based Cryptography Specification
 *										Version 2.0 Septmeber 2000
 *										B. Kaliski (RSA Laboratories)
 *                  	
 *										TEA, a Tiny Encryption Algortithm
 *										David Wheeler
 *										Roger Needham
 *										Computer Laboratory
 *										Cambridge University
 *										England
 *
 *				Compiler:   Visual C++ 6.0
 *
 * 
 * Revision     Date        Who    Comment
 ******************************************************************************
 *  1.0       May-18-05     MS     baseline
 ******************************************************************************/
//#include "stdafx.h"
#include <string.h>

#include "Password.h"


/* SAH-1 */
int16_t SHA1( uint8_t *, uint8_t *, uint32_t);

/* TEA */
void decrypt( uint8_t *key, uint8_t *text);
void encrypt( uint8_t *key, uint8_t *text);

/* Derive key */
void derivekey(pstruct *);

/* Message signing and authentication */
int  messageauthentication(pstruct *p, uint8_t *text, uint32_t len);
void digitalsignature(pstruct *p, uint8_t *text, uint32_t len);
void encryptpassword(pstruct *p, uint8_t *msg, uint8_t *secretkey);

 //----------------------------------------------------------------------------
 //			Function:	  digitalsignature()
 //	 Description:		Computes digital signature on a text array using the HMAC
 //                 algorithm.
 //    Arguments:		* password structure
 //									* text
 //									message length in bytes
 //		   Returns:		p->digest = digital signature		
 //----------------------------------------------------------------------------
 void
 digitalsignature(pstruct *p, uint8_t *text, uint32_t len)
 {
 // HMAC structure
typedef struct hmac
	{
		uint8_t k_pad[20];		// i-pad, o-pad buffer
// @@@DBW		uint8_t text[256];		// 256 byte message buffer
		uint8_t text[6*256];		// @@@DBW
		}hmac;
 	int32_t i;
 	// reserve storage for the HMAC algorithm parameters
	hmac hmacbuffer;
	// derive the HMAC key
	derivekey(p);
	// compute: H(K XOR opad, H(K XOR ipad, text)
	// generate k_ipad
	memmove(hmacbuffer.k_pad, p->key, sizeof(hmacbuffer.k_pad));
	for (i = 0; i < sizeof(hmacbuffer.k_pad); i++)
		hmacbuffer.k_pad[i] ^= 0x36;
	// copy text to buffer
	memmove(hmacbuffer.text, text, len);
	// compute message digest H(K XOR ipad, text)
	SHA1(p->digest, &hmacbuffer.k_pad[0], len + sizeof(hmacbuffer.k_pad));
	// generate k_opad
	memmove(hmacbuffer.k_pad, p->key, sizeof(hmacbuffer.k_pad));
	for (i = 0; i < sizeof(hmacbuffer.k_pad); i++)
		hmacbuffer.k_pad[i] ^= 0x5c;
	// copy digest from last message to buffer
	memmove(hmacbuffer.text, p->digest, sizeof(p->digest));
	// compute next digest H(K XOR opad, digest)
	SHA1(p->digest, &hmacbuffer.k_pad[0],  sizeof(p->digest)+sizeof(hmacbuffer.k_pad));
	}

 //----------------------------------------------------------------------------
 //			Function:	  messageauthentication()
 //	 Description:		Authenticates the message using password provided.
 //    Arguments:		* password structure
 //									* text
 //									message length in bytes including digital signature
 //		   Returns:		!0 for authentic message		
 //----------------------------------------------------------------------------
 int
 messageauthentication(pstruct *p, uint8_t *text, uint32_t len)
 {
#define sizeofSIGNATURE 20
 	// compute the digital signature
 	digitalsignature(p, text, len - sizeofSIGNATURE);
 	// compare the signatures
 	return(!(memcmp(p->digest, text + (len - sizeofSIGNATURE), sizeofSIGNATURE)));
 }

 //----------------------------------------------------------------------------
 //			Function:	  encryptpassword()
 //	 Description:		Generates the set password message.
 //    Arguments:		* password structure
 //		   Returns:		message in buffer	
 //----------------------------------------------------------------------------
 void
 encryptpassword(pstruct *p, uint8_t *msg, uint8_t *secretkey)
 {
	// MAC address to msg buffer
	memmove(&msg[0], p->macaddress, sizeof(p->macaddress));
	// pad MAC address to 8-bytes
	msg[6] = 0;
	msg[7] = 0;
	// copy password to message buffer
	memmove(&msg[8], p->password, sizeof(p->password));
	// compute digital signature on clear text
	// password and MAC address
	digitalsignature(p, msg, 16);
	memmove(&msg[16], p->digest, sizeof(p->digest));
	// encrypt MAC address
	encrypt(secretkey, &msg[0]);
	// encrypt password
	encrypt(secretkey, &msg[8]);
	}
	
 //----------------------------------------------------------------------------
 //			Function:		SHA1()
 //	 Description:	  'C' implementation of the SHA-1 hash algorithm.
 //    Arguments:		* to message digest result
 //									* to message
 //									message length in 16-bit words (54xx format)
 //		   Returns:		error code
 //----------------------------------------------------------------------------

 // SHA-1 definitions
 #define SHA1HASHSIZE 20
 #define CircShift(x,n) (((x) << (n)) | ((x) >> (32-(n))))
 
 // This structure holds the context information for the
 // SHA-1 hashing operation
 typedef struct SHA1Context
 {
 		uint32_t	Hash[SHA1HASHSIZE/4];			// Message Digest
 		uint8_t	  M[64];										// 512-bit message block
 }SHA1Context;

 // Function prototypes
 void SHA1processmessageblocks(SHA1Context *, uint8_t *, uint32_t);
 void SHA1processmessageblock(SHA1Context *);
 
 // SHA-1
 int16_t
 SHA1( uint8_t *digest,  uint8_t *text,  uint32_t len)
 {
 uint16_t i;
 // hash algorithm buffer
 SHA1Context sha;
 	// 0 length not allowed
 	if (!len)
 		return(1);
 	// initialize SHA-1 algorithm parameters
 	sha.Hash[0] = 0x67452301;
	sha.Hash[1] = 0xefcdab89;
	sha.Hash[2] = 0x98badcfe;
	sha.Hash[3] = 0x10325476;
	sha.Hash[4] = 0xc3d2e1f0;
 	// process message block(s)
 	SHA1processmessageblocks(&sha, text, len);
 	// format SHA-1 digest
 	for( i = 0; i < SHA1HASHSIZE/4; i ++)	{
		*digest++ = sha.Hash[i] >> 24;
		*digest++ = sha.Hash[i] >> 16;
 		*digest++ = sha.Hash[i] >> 8;
 		*digest++ = sha.Hash[i];
 		} 	
 	return(0);
 }
 
 //----------------------------------------------------------------------------
 //			Function:		SHA1processmessageblocks()
 //	 Description:		Process the message blocks.
 //----------------------------------------------------------------------------
 void
 SHA1processmessageblocks(SHA1Context *context, uint8_t *msg, uint32_t len)
 {
 	uint16_t numberofbits;
 	// calculate the number of bits in the message
 	numberofbits = len * 8;
 	// process message in 512-bit blocks
 	while (len >= 64)	{	
 		// Move message into block buffer
 		memmove(context->M, msg, 64);
 		// update msg pointer and length field
 		msg += 64;
 		len -= 64;
		// process message block
		SHA1processmessageblock(context);
		}
	// process block size < 512-bits
	// Erase message block buffer
 	memset(context->M, 0, sizeof(context->M));
 	// Move message into block buffer
 	memmove(context->M, msg, len);
 	// Set ms bit of message pad
 	context->M[len] = 0x80;  		
	if (len < 56)	{
		// last block with room for message pad flag and length field
 		// Set message length field to the number of bits in the message
 		// not including the message pad or length fields
 	 	context->M[62] = numberofbits>>8;
		context->M[63] = (uint8_t)numberofbits;
		// process block
		SHA1processmessageblock(context);
		return;
		}
	else	{
	 	// add a block to fit the length field
		// process previous block < 512 bits
		SHA1processmessageblock(context);	
		// Erase message block buffer
 		memset(context->M, 0, sizeof(context->M));
		// Set message length field to the number of bits in the message
 		// not including the message pad or length fields
 	 	context->M[62] = numberofbits>>8;
		context->M[63] = (uint8_t)numberofbits;
		// process block
		SHA1processmessageblock(context);
		}		
 }
 
 //----------------------------------------------------------------------------
 //			Function:		SHA1processmessageblock()
 //	 Description:		Process 512-bit message block.
 //----------------------------------------------------------------------------
 void
 SHA1processmessageblock(SHA1Context *context)
 {
 	const uint32_t K[] = { 		// constants defined in SHA-1
 												0x5a827999,
 												0x6ed9eba1,
 												0x8f1bbcdc,
 												0xca62c1d6
 												};
	int t;									  // Loop counters
	uint32_t	Temp;						// Temporary word count
	uint32_t	W[80];					// Word sequence
	uint32_t	A, B, C, D, E;	// Word buffers

	// Divide M(i) into 16 32-bit lwords W[0], W[1] ..., W[15]
	// where W[0] is the left-most word.
	for (t = 0; t < 16; t++)	{
			W[t] =  context->M[t*4 + 3];
			W[t] |= context->M[t*4 + 2] << 8;
			W[t] |= context->M[t*4 + 1] << 16;
			W[t] |= context->M[t*4] << 24;			
			}	 		
	// For t = 16 to 79 let
	// W(t) = S^1(W(t-3) XOR W(t-8) XOR W(t-14) XOR W(t-16))	
	for ( t = 16; t < 80; t++)
		W[t] = CircShift((W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]),1);
	// Let A = H0, B = H1, C = H2, D = H3 , E = H4
	A = context->Hash[0];
	B = context->Hash[1];
	C = context->Hash[2];
	D = context->Hash[3];
	E = context->Hash[4];
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
	context->Hash[0] += A;
	context->Hash[1] += B;
	context->Hash[2] += C;
	context->Hash[3] += D;
	context->Hash[4] += E;		 												 												
 }

 //----------------------------------------------------------------------------
 //			Function:		derivekey()
 //	 Description:		Derives the authentication key from the password
 //									for use in S75 message authentication.
 //    Arguments:		* password structure
 //		   Returns:		p->key[] = 160-bit key		
 //----------------------------------------------------------------------------
 void
 derivekey(pstruct *p)
 {
 	int i, ii;
 	// compute:
 	// H(P,S) ^ H(P, U_1) ^... H(P, U_n-1)
 	// U_n = result from previous hash
 	// compute initial intermediate key value
 	// and save it H(P,S) 
 	SHA1(&p->digest[0], &p->macaddress[0], (sizeof(p->macaddress)+sizeof(p->password)));
 	memmove(p->key, p->digest, sizeof(p->digest)); 	
 	// compute the next 7 intermediate key values
 	// and XOR all 8 results together
 	for ( i = 1; i < 8; i++)	{
  	SHA1(&p->digest[0], &p->password[0], (sizeof(p->password)+sizeof(p->digest)));
 		for ( ii = 0; ii < sizeof(p->digest); ii++)
 			p->key[ii] ^= p->digest[ii]; 		
 		}
 }

 
 //----------------------------------------------------------------------------
 //			Function:		encrypt()
 //	 Description:		C++ implementation of the TEA encryption
 //									algorithm.
 //    Arguments:		* key 128-bits
 //									* cleartext 64-bits
 //		   Returns:		cleartext = ciphertext		
 //----------------------------------------------------------------------------
 void
 encrypt( uint8_t *key,  uint8_t *text)
 {
 	uint32_t i;
 	uint32_t y = 0, z = 0, sum = 0;
	uint32_t k[4];
 	uint32_t delta = 0x9e3779b9;
	// convert key array  to 4 words
	for (i = 0; i < 4; i++)		{
				k[i]  = *key++ << 24;
				k[i] |= *key++ << 16;
				k[i] |= *key++ << 8;
				k[i] |= *key++;
				}
	// convert text to two uint32_t variables
	for (i = 0; i < 4; i++)		{
			y |= text[i]   << (3-i) * 8;
			z |= text[i+4] << (3-i) * 8;
	}
 	// compute the cyphertext
 	for ( i = 0; i < 32; i++)	{
 		sum += delta;
 		  y += (z<<4)+k[0] ^ z+sum ^ (z>>5) + k[1];
 		  z += (y<<4)+k[2] ^ y+sum ^ (y>>5) + k[3];
 		}
	// convert two uint32_t variables to text
	for (i = 0; i < 4; i++)		{
			text[i]   = y >> (3-i) * 8;
			text[i+4] = z >> (3-i) * 8;
			}
	}

 //----------------------------------------------------------------------------
 //			Function:		decrypt()
 //	 Description:		C++ implementation of the TEA decryption
 //									algorithm.
 //    Arguments:		* key 128-bits
 //									* ciphertext 64-bits
 //		   Returns:		ciphertext = cleartext		
 //----------------------------------------------------------------------------
 void
 decrypt( uint8_t *key,  uint8_t *text)
 {
 	uint32_t i;
 	uint32_t y = 0, z = 0, sum = 0;
	uint32_t k[4];
 	uint32_t delta = 0x9e3779b9;
	// convert key array  to 4 words
	for (i = 0; i < 4; i++)		{
				k[i]  = *key++ << 24;
				k[i] |= *key++ << 16;
				k[i] |= *key++ << 8;
				k[i] |= *key++;
				}
	// convert text string to two uint32_t variables
	for (i = 0; i < 4; i++)		{
			y |= text[i]   << (3-i) * 8;
			z |= text[i+4] << (3-i) * 8;
	}
 	sum = delta << 5;
 	// compute the clear text
 	for ( i = 0; i < 32; i++)	{
 		  z -= (y<<4)+k[2] ^ y+sum ^ (y>>5) + k[3];
 		  y -= (z<<4)+k[0] ^ z+sum ^ (z>>5) + k[1];
		sum -= delta;
 		}
	// convert two uint32_t variables to text
	for (i = 0; i < 4; i++)		{
			text[i]   = y >> (3-i) * 8;
			text[i+4] = z >> (3-i) * 8;
			}
	}

//#endif


