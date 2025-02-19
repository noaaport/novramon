// Free implementation of the MD4 hash algorithm

// Original header in MD4C.C and MD4.h:

// MD4C.C - RSA Data Security, Inc., MD4 message-digest algorithm

/*
	Copyright (C) 1990-2, RSA Data Security, Inc. All rights reserved.

	License to copy and use this software is granted provided that it
	is identified as the "RSA Data Security, Inc. MD4 Message-Digest
	Algorithm" in all material mentioning or referencing this software
	or this function.

	License is also granted to make and use derivative works provided
	that such works are identified as "derived from the RSA Data
	Security, Inc. MD4 Message-Digest Algorithm" in all material
	mentioning or referencing the derived work.  

	RSA Data Security, Inc. makes no representations concerning either
	the merchantability of this software or the suitability of this
	software for any particular purpose. It is provided "as is"
	without express or implied warranty of any kind.  

	These notices must be retained in any copies of any part of this
	documentation and/or software.  
*/

#include <memory.h>
// @@@DBW #include "stdafx.h"
#include "md4.h"

// Constants for MD4_Transform routine.
#define MD4_S11 3
#define MD4_S12 7
#define MD4_S13 11
#define MD4_S14 19
#define MD4_S21 3
#define MD4_S22 5
#define MD4_S23 9
#define MD4_S24 13
#define MD4_S31 3
#define MD4_S32 9
#define MD4_S33 11
#define MD4_S34 15

static void MD4_Transform(UINT4 *state, unsigned char *block);
static void MD4_Encode(unsigned char *output, UINT4 *input, unsigned int len);
static void MD4_Decode(UINT4 *output, unsigned char *input, unsigned int len);

static unsigned char MD4_PADDING[64] = {
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// MD4F, MD4G and MD4H are basic MD4 functions.
#define MD4F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define MD4G(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define MD4H(x, y, z) ((x) ^ (y) ^ (z))

// MD4_ROTL rotates x left n bits.
#define MD4_ROTL(x, n) (((x) << (n)) | ((x) >> (32-(n))))

// MD4FF, MD4GG and MD4HH are transformations for rounds 1, 2 and 3
// Rotation is separate from addition to prevent recomputation
#define MD4FF(a, b, c, d, x, s) { (a) += MD4F ((b), (c), (d)) + (x); (a) = MD4_ROTL ((a), (s)); }
#define MD4GG(a, b, c, d, x, s) { (a) += MD4G ((b), (c), (d)) + (x) + (UINT4)0x5a827999; (a) = MD4_ROTL ((a), (s)); }
#define MD4HH(a, b, c, d, x, s) { (a) += MD4H ((b), (c), (d)) + (x) + (UINT4)0x6ed9eba1; (a) = MD4_ROTL ((a), (s)); }

// MD4 initialization. Begins an MD4 operation, writing a new context
void MD4Init(MD4_CTX *context)
{
	context->count[0] = context->count[1] = 0;

	// Load magic initialization constants
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
}

// MD4 block update operation. Continues an MD4 message-digest
//     operation, processing another message block, and updating the
//     context
void MD4Update(MD4_CTX *context, unsigned char *input, unsigned int inputLen)
{
	unsigned int i = 0, index = 0, partLen = 0;

	// Compute number of bytes mod 64
	index = (unsigned int)((context->count[0] >> 3) & 0x3F);

	// Update number of bits
	if ((context->count[0] += ((UINT4)inputLen << 3))
		< ((UINT4)inputLen << 3))
		context->count[1]++;
	context->count[1] += ((UINT4)inputLen >> 29);

	partLen = 64 - index;

	// Transform as many times as possible
	if (inputLen >= partLen)
	{
		memcpy((MD4_POINTER)&context->buffer[index], (MD4_POINTER)input, partLen);
		MD4_Transform(context->state, context->buffer);
  
		for (i = partLen; i + 63 < inputLen; i += 64)
			MD4_Transform (context->state, &input[i]);

		index = 0;
	}
	else i = 0;
  
	// Buffer remaining input
	memcpy((MD4_POINTER)&context->buffer[index], (MD4_POINTER)&input[i], inputLen - i);
}

// MD4 finalization. Ends an MD4 message-digest operation, writing the
//     the message digest and zeroizing the context.
void MD4Final(unsigned char *digest, MD4_CTX *context)
{
	unsigned char bits[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	unsigned int index = 0, padLen = 0;

	// Save number of bits
	MD4_Encode (bits, context->count, 8);

	// Pad out to 56 mod 64.
	index = (unsigned int)((context->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	MD4Update(context, MD4_PADDING, padLen);

	// Append length (before padding)
	MD4Update(context, bits, 8);

	// Store state in digest
	MD4_Encode(digest, context->state, 16);

	// Zeroize sensitive information
	memset((MD4_POINTER)context, 0, sizeof(MD4_CTX));
}

// MD4 basic transformation. Transforms state based on block.
static void MD4_Transform(UINT4 *state, unsigned char *block)
{
	UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

	MD4_Decode(x, block, 64);

	// Round 1
	MD4FF (a, b, c, d, x[ 0], MD4_S11); /* 1 */
	MD4FF (d, a, b, c, x[ 1], MD4_S12); /* 2 */
	MD4FF (c, d, a, b, x[ 2], MD4_S13); /* 3 */
	MD4FF (b, c, d, a, x[ 3], MD4_S14); /* 4 */
	MD4FF (a, b, c, d, x[ 4], MD4_S11); /* 5 */
	MD4FF (d, a, b, c, x[ 5], MD4_S12); /* 6 */
	MD4FF (c, d, a, b, x[ 6], MD4_S13); /* 7 */
	MD4FF (b, c, d, a, x[ 7], MD4_S14); /* 8 */
	MD4FF (a, b, c, d, x[ 8], MD4_S11); /* 9 */
	MD4FF (d, a, b, c, x[ 9], MD4_S12); /* 10 */
	MD4FF (c, d, a, b, x[10], MD4_S13); /* 11 */
	MD4FF (b, c, d, a, x[11], MD4_S14); /* 12 */
	MD4FF (a, b, c, d, x[12], MD4_S11); /* 13 */
	MD4FF (d, a, b, c, x[13], MD4_S12); /* 14 */
	MD4FF (c, d, a, b, x[14], MD4_S13); /* 15 */
	MD4FF (b, c, d, a, x[15], MD4_S14); /* 16 */

	// Round 2
	MD4GG (a, b, c, d, x[ 0], MD4_S21); /* 17 */
	MD4GG (d, a, b, c, x[ 4], MD4_S22); /* 18 */
	MD4GG (c, d, a, b, x[ 8], MD4_S23); /* 19 */
	MD4GG (b, c, d, a, x[12], MD4_S24); /* 20 */
	MD4GG (a, b, c, d, x[ 1], MD4_S21); /* 21 */
	MD4GG (d, a, b, c, x[ 5], MD4_S22); /* 22 */
	MD4GG (c, d, a, b, x[ 9], MD4_S23); /* 23 */
	MD4GG (b, c, d, a, x[13], MD4_S24); /* 24 */
	MD4GG (a, b, c, d, x[ 2], MD4_S21); /* 25 */
	MD4GG (d, a, b, c, x[ 6], MD4_S22); /* 26 */
	MD4GG (c, d, a, b, x[10], MD4_S23); /* 27 */
	MD4GG (b, c, d, a, x[14], MD4_S24); /* 28 */
	MD4GG (a, b, c, d, x[ 3], MD4_S21); /* 29 */
	MD4GG (d, a, b, c, x[ 7], MD4_S22); /* 30 */
	MD4GG (c, d, a, b, x[11], MD4_S23); /* 31 */
	MD4GG (b, c, d, a, x[15], MD4_S24); /* 32 */

	// Round 3
	MD4HH (a, b, c, d, x[ 0], MD4_S31); /* 33 */
	MD4HH (d, a, b, c, x[ 8], MD4_S32); /* 34 */
	MD4HH (c, d, a, b, x[ 4], MD4_S33); /* 35 */
	MD4HH (b, c, d, a, x[12], MD4_S34); /* 36 */
	MD4HH (a, b, c, d, x[ 2], MD4_S31); /* 37 */
	MD4HH (d, a, b, c, x[10], MD4_S32); /* 38 */
	MD4HH (c, d, a, b, x[ 6], MD4_S33); /* 39 */
	MD4HH (b, c, d, a, x[14], MD4_S34); /* 40 */
	MD4HH (a, b, c, d, x[ 1], MD4_S31); /* 41 */
	MD4HH (d, a, b, c, x[ 9], MD4_S32); /* 42 */
	MD4HH (c, d, a, b, x[ 5], MD4_S33); /* 43 */
	MD4HH (b, c, d, a, x[13], MD4_S34); /* 44 */
	MD4HH (a, b, c, d, x[ 3], MD4_S31); /* 45 */
	MD4HH (d, a, b, c, x[11], MD4_S32); /* 46 */
	MD4HH (c, d, a, b, x[ 7], MD4_S33); /* 47 */
	MD4HH (b, c, d, a, x[15], MD4_S34); /* 48 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	// Zeroize sensitive information.
	memset ((MD4_POINTER)x, 0, sizeof(x));
}

// MD4_Encodes input (UINT4) into output (unsigned char). Assumes len is
//     a multiple of 4.
static void MD4_Encode(unsigned char *output, UINT4 *input, unsigned int len)
{
	unsigned int i = 0, j = 0;

	for (i = 0, j = 0; j < len; i++, j += 4)
	{
	output[j] = (unsigned char)(input[i] & 0xff);
	output[j+1] = (unsigned char)((input[i] >> 8) & 0xff);
	output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);
	output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);
	}
}

// MD4_Decodes input (unsigned char) into output (UINT4). Assumes len is
//     a multiple of 4.
static void MD4_Decode(UINT4 *output, unsigned char *input, unsigned int len)
{
	unsigned int i = 0, j = 0;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((UINT4)input[j]) | (((UINT4)input[j+1]) << 8) |
			(((UINT4)input[j+2]) << 16) | (((UINT4)input[j+3]) << 24);
}

