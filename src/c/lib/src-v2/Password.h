//#ifdef JSAT  // Not JSAT EXCLUSIVE ANY MORE !!!

#ifndef _PSTRUCT

#define _PSTRUCT

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
 *	File:		password.c
 * 	Author:		Michael S. Stasenski
 * 	Date:		May 18, 2005
 * 	Comments: 	The following C source implements the password functions
 *			for the S75 JLeo DVB-S receiver.
 *
 *	Documentation:	Receiver Configuration Messages
 *			S75-JLeo Des
 *			Rev. 1, May 11, 2005
 *			M. Nashaat T. Soliman (NOVRA)
 *
 *                	RFC 2104 February 1997
 *			HMAC: Keyed-Hashing for Message Authentication
 *			H. Krawczyk (IBM)
 *			M. Bellare (UCSD)
 *			R. Canetti (IBM)
 *
 *			RFC 3174 September 2001
 *			US Secure Hash Algorithm 1 (SHA1)
 *			Donald E. Eastlake, 3rd (Motorola)
 *			Paul E. Jones (Cisco Systems)
 *
 *			RFC 2898 PCKS #5: Password_Based Cryptography Specification
 *			Version 2.0 Septmeber 2000
 *			B. Kaliski (RSA Laboratories)
 *                  
 *			TEA, a Tiny Encryption Algortithm
 *			David Wheeler
 *			Roger Needham
 *			Computer Laboratory
 *			Cambridge University
 *			England
 *
 *			Compiler:   Visual C++ 6.0
 *
 * 
 * Revision     Date        Who    Comment
 ******************************************************************************
 *  1.0       May-18-05     MS     baseline
 ******************************************************************************/

/* password parameters structure */
/* do not modify !! */

#include <inttypes.h>
/*
#define uint32_t unsigned int
#define int32_t  int
#define uint16_t unsigned short
#define int16_t  short
#define uint8_t	 unsigned char
*/

typedef struct pstruct	{
		uint8_t macaddress[6]; 		// 6-byte MAC address
		uint8_t password[8];      	// 8-char password
		uint8_t digest[20];		// 160-bit digital signature/digest
		uint8_t key[20];		// 160-bit derived key
}pstruct;


void digitalsignature(pstruct *p, uint8_t *text, uint32_t len);

void encryptpassword(pstruct *p, uint8_t *msg, uint8_t *secretkey);

void encrypt( uint8_t *key, uint8_t *text);		// @@@DBW added
void decrypt( uint8_t *key, uint8_t *text);		// @@@DBW added


#endif

//#endif



