// CHash.cpp : implementation file
//

// @@@DBW #include "stdafx.h"
#include <stdio.h>
#include "CHash.h"
#include "gosthash.h"
#include "md2.h"
#include "md4.h"
#include "md5.h"
#include "sha1.h"
#include "sha2.h"

#include <string>	// @@@DBW

using namespace std;	// @@@DBW

// Common variables
FILE *fileToHash = NULL;
unsigned char fileBuf[SIZE_OF_BUFFER];
unsigned long lenRead = 0;

// The outputted hash
// @@@DBW CString outHash;
string outHash;  // @@@DBW

// Temporary working buffers
unsigned char* tempOut = new unsigned char[256];
// @@@DBW CString tempHash = "";
string tempHash = "";

/////////////////////////////////////////////////////////////////////////////
// CHash class



CHash::CHash()
{
	// Set some variables for defaultness
	hashAlgo = SHA1;
	hashFile = "";
	hashOperation = 1;
	inHash = "";
	sha2Strength = 256;
}



// Retrieve the hash
//
//
// @@@DBW CString CHash::DoHash()
string CHash::DoHash()	// @@@DBW
{
// @@@DBW	CString retHash = "";
	string retHash = "";	// @@@DBW
// @@@DBW	tempHash.Empty();
	tempHash = "";	// @@@DBW

	// See what hash was selected and initiate it
	if (hashAlgo == GOSTHASH)
		retHash = GOSTHash();

	if (hashAlgo == MD2)
		retHash = MD2Hash();

	if (hashAlgo == MD4)
		retHash = MD4Hash();

	if (hashAlgo == MD5)
		retHash = MD5Hash();

	if (hashAlgo == SHA1)
		retHash = SHA1Hash();

	if (hashAlgo == SHA2)
		retHash = SHA2Hash();

// @@@DBW	tempHash.Empty();
	tempHash = "";	// @@@DBW

	// Do the styling
	if (hashFormatting == 2 || hashFormatting == 4)
	{
		// Add spacing
		bool isChar = false;
// @@@DBW		int hashLen = retHash.GetLength();
		int hashLen = retHash.length();	// @@@DBW

		for (int i = 0; i < hashLen; i++)
		{
			if (!isChar)
			{
// @@@DBW				tempHash += retHash.GetAt(i);
				tempHash += retHash.at(i);	// @@@DBW
				isChar = true;
			}
			else
			{
// @@@DBW				tempHash += retHash.GetAt(i);
				tempHash += retHash.at(i);	// @@@DBW
				tempHash += " ";
				isChar = false;
			}
		}

		retHash = tempHash;
	}

	if (hashFormatting == 3 || hashFormatting == 4)
	{
// @@@DBW		retHash.MakeUpper();
		char big[2] = { 0, 0 };
		for ( int i = 0; i < retHash.length(); i++ ) {
			big[0] = toupper(retHash.at(i));
			retHash.replace(i,1,big);
		}
	}

	return retHash;
}
// End DoHash ///////////////////////////////////////////////////



// Get the current algorithm
//
//
int CHash::GetHashAlgorithm()
{
	return hashAlgo;
}
// End GetHashAlgorithm /////////////////////////////////////////



// Get the current operation
//
//
int CHash::GetHashOperation()
{
	return hashOperation;
}
// End GetHashOperation /////////////////////////////////////////



// Get the input string
//
//
// @@@DBW CString CHash::GetHashString()
string CHash::GetHashString()	// @@@DBW
{
	return inHash;
}
// End GetHashString ////////////////////////////////////////////



// Get the hash file
//
//
// @@@DBW CString CHash::GetHashFile()
string CHash::GetHashFile()	// @@@DBW
{
	return hashFile;
}
// End GetHashFile //////////////////////////////////////////////



// Get the strength of the SHA-2 hash (256, 384 or 512)
//
//
int CHash::GetSHA2Strength()
{
	return sha2Strength;
}
// End GetSHA2Strength //////////////////////////////////////////



// Hash with GOST-Hash
//
//
// @@@DBW CString CHash::GOSTHash()
string CHash::GOSTHash()	// @@@DBW
{
	// GOST-Hash object to work with
	GostHashCtx m_gost;

	// Check what operation we're doing
	if (hashOperation == 1)
	{
		// Hashing a string, reset the object, give it the text, and finalise it
		gosthash_reset(&m_gost);
// @@@DBW		gosthash_update(&m_gost, reinterpret_cast<unsigned char *>(inHash.GetBuffer(inHash.GetLength())), inHash.GetLength());
		gosthash_update(&m_gost, (unsigned char *)(inHash.c_str()), inHash.length());	// @@@DBW
		gosthash_final(&m_gost, tempOut);
	}

	// Hash a file
	if (hashOperation == 2)
	{
// @@@DBW		if (hashFile.IsEmpty()) return "";
		if (hashFile.empty()) return "";	// @@@DBW
		gosthash_reset(&m_gost);

// @@@DBW		fileToHash = fopen(hashFile, "rb");
		fileToHash = fopen(hashFile.c_str(), "rb");
	
		while(1)
		{
			lenRead = fread(fileBuf, 1, SIZE_OF_BUFFER, fileToHash);
			if(lenRead != 0)
			{	
				gosthash_update(&m_gost, fileBuf, lenRead);
			}
			if(lenRead != SIZE_OF_BUFFER) break;
		}

		fclose(fileToHash); fileToHash = NULL;

		gosthash_final(&m_gost, tempOut);
	}

	for (int i = 0 ; i < 32 ; i++)
	{
		char tmp[3];
// @@@DBW		_itoa(tempOut[i], tmp , 16);
		sprintf( tmp, "%2x", tempOut[i] );	// @@@DBW

		if (strlen(tmp) == 1)
		{
			tmp[1] = tmp[0];
			tmp[0] = '0';
			tmp[2] = '\0';
		}
		tempHash += tmp;
	}

	outHash = tempHash;

	return outHash;
}
// End GOSTHash /////////////////////////////////////////////////



// Hash with MD2
//
//
// @@@DBW CString CHash::MD2Hash()
string CHash::MD2Hash()	// @@@DBW
{
	// MD2 object to work with
	CMD2 m_md2;

	// Check what operation we're doing
	if (hashOperation == 1)
	{
		// Hashing a string, open the object, give it the text, and finalise it
		m_md2.Init();
// @@@DBW		m_md2.Update(reinterpret_cast<unsigned char *>(inHash.GetBuffer(inHash.GetLength())), inHash.GetLength());
		m_md2.Update((unsigned char *)(inHash.c_str()), inHash.length());	// @@@DBW
		m_md2.TruncatedFinal(tempOut, 16);
	}

	// Hash a file
	if (hashOperation == 2)
	{
// @@@DBW		if (hashFile.IsEmpty()) return "";
		if (hashFile.empty()) return "";	// @@@DBW
		m_md2.Init();

// @@@DBW		fileToHash = fopen(hashFile, "rb");
		fileToHash = fopen(hashFile.c_str(), "rb");	// @@@DBW
	
		while(1)
		{
			lenRead = fread(fileBuf, 1, SIZE_OF_BUFFER, fileToHash);
			if(lenRead != 0)
			{	
				m_md2.Update(fileBuf, lenRead);
			}
			if(lenRead != SIZE_OF_BUFFER) break;
		}

		fclose(fileToHash); fileToHash = NULL;

		m_md2.TruncatedFinal(tempOut, 16);
	}

	for (int i = 0 ; i < 16 ; i++)
	{
		char tmp[3];
// @@@DBW		_itoa(tempOut[i], tmp, 16);
		sprintf( tmp, "%2x", tempOut[i] );	// @@@DBW
		if (strlen(tmp) == 1)
		{
			tmp[1] = tmp[0];
			tmp[0] = '0';
			tmp[2] = '\0';
		}
		tempHash += tmp;	
	}

	outHash = tempHash;

	return outHash;
}
// End MD2Hash //////////////////////////////////////////////////



// Hash with MD4
//
//
// @@@DBW CString CHash::MD4Hash()
string CHash::MD4Hash()	// @@@DBW
{
	// MD4 object to work with
	MD4_CTX m_md4;

	// Check what operation we're doing
	if (hashOperation == 1)
	{
		// Hashing a string, open the object, give it the text, and finalise it
		MD4Init(&m_md4);
// @@@DBW		MD4Update(&m_md4, reinterpret_cast<unsigned char *>(inHash.GetBuffer(inHash.GetLength())), inHash.GetLength());
		MD4Update(&m_md4, (unsigned char *)(inHash.c_str()), inHash.length());	// @@@DBW
		MD4Final(tempOut, &m_md4);
	}

	// Hash a file
	if (hashOperation == 2)
	{
// @@@DBW		if (hashFile.IsEmpty()) return "";
		if (hashFile.empty()) return "";	// @@@DBW
		MD4Init(&m_md4);

// @@@DBW		fileToHash = fopen(hashFile, "rb");
		fileToHash = fopen(hashFile.c_str(), "rb");
	
		while(1)
		{
			lenRead = fread(fileBuf, 1, SIZE_OF_BUFFER, fileToHash);
			if(lenRead != 0)
			{	
				MD4Update(&m_md4, fileBuf, lenRead);
			}
			if(lenRead != SIZE_OF_BUFFER) break;
		}

		fclose(fileToHash); fileToHash = NULL;

		MD4Final(tempOut, &m_md4);
	}

	for (int i = 0 ; i < 16 ; i++)
	{
		char tmp[3];
// @@@DBW		_itoa(tempOut[i], tmp, 16);
		sprintf( tmp, "%2x", tempOut[i] );	// @@@DBW
		if (strlen(tmp) == 1)
		{
			tmp[1] = tmp[0];
			tmp[0] = '0';
			tmp[2] = '\0';
		}

		tempHash += tmp;	
	}

	outHash = tempHash;

	return outHash;
}
// End MD4Hash //////////////////////////////////////////////////



// Hash with MD5
//
//
// @@@DBW CString CHash::MD5Hash()
string CHash::MD5Hash()	// @@@DBW
{
	// MD5 object to work with
	MD5_CTX m_md5;

	// Check what operation we're doing
	if (hashOperation == 1)
	{
		// Hashing a string, open the object, give it the text, and finalise it
		MD5Init(&m_md5);
// @@@DBW		MD5Update(&m_md5, reinterpret_cast<unsigned char *>(inHash.GetBuffer(inHash.GetLength())), inHash.GetLength());
		MD5Update(&m_md5, (unsigned char *)(inHash.c_str()), inHash.length());	// @@@DBW
		MD5Final(&m_md5);

		// Get each chunk of the hash and append it to the output
		for ( int i = 0 ; i < 16 ; i++ )
		{
			char tmp[3];
// @@@DBW			_itoa(m_md5.digest[i], tmp , 16);
			sprintf( tmp, "%2x", m_md5.digest[i] );	// @@@DBW

			if (strlen(tmp) == 1)
			{
				tmp[1] = tmp[0];
				tmp[0] = '0';
				tmp[2] = '\0';
			}
			tempHash += tmp;
		}
		// Copy the temporary hash to the final buffer
		outHash = tempHash;
	}

	// Hash a file
	if (hashOperation == 2)
	{
// @@@DBW		if (hashFile.IsEmpty()) return "";
		if (hashFile.empty()) return "";	// @@@DBW

// @@@DBW		if (md5file(hashFile.GetBuffer(MAX_PATH), 0, &m_md5))
		if (md5file((char *)(hashFile.c_str()), 0, &m_md5))
		{
			for (int i = 0; i < 16; i++)
			{
				char tmp[3];
// @@@DBW				_itoa(m_md5.digest[i], tmp, 16);
				sprintf( tmp, "%2x", m_md5.digest[i] );	// @@@DBW
				if (strlen(tmp) == 1)
				{
					tmp[1] = tmp[0];
					tmp[0] = '0';
					tmp[2] = '\0';
				}
				tempHash += tmp;		
			}
		}
		outHash = tempHash;
	}

	return outHash;
}
// End MD5Hash //////////////////////////////////////////////////



// Hash with SHA-1
//
//
// @@@DBW CString CHash::SHA1Hash()
string CHash::SHA1Hash()	// @@@DBW
{
	// SHA-1 object
	sha1_ctx m_sha1;

	// Check what operation we're doing
	if (hashOperation == 1)
	{
		// Hashing a string, open the object, give it the text, and finalise it
		sha1_begin(&m_sha1);
// @@@DBW		sha1_hash(reinterpret_cast<unsigned char *>(inHash.GetBuffer(inHash.GetLength())), inHash.GetLength(), &m_sha1);
		sha1_hash((unsigned char *)(inHash.c_str()), inHash.length(), &m_sha1);	// @@@DBW
		sha1_end(tempOut, &m_sha1);
	}

	// Hash a file
	if (hashOperation == 2)
	{
// @@@DBW		if (hashFile.IsEmpty()) return "";
		if (hashFile.empty()) return "";	// @@@DBW
		sha1_begin(&m_sha1);

// @@@DBW		fileToHash = fopen(hashFile, "rb");
		fileToHash = fopen(hashFile.c_str(), "rb");	// @@@DBW
	
		while(1)
		{
			lenRead = fread(fileBuf, 1, SIZE_OF_BUFFER, fileToHash);
			if(lenRead != 0)
			{	
				sha1_hash(fileBuf, lenRead, &m_sha1);
			}
			if(lenRead != SIZE_OF_BUFFER) break;
		}

		fclose(fileToHash); fileToHash = NULL;

		sha1_end(tempOut, &m_sha1);
	}

	for (int i = 0 ; i < 20 ; i++)
	{
		char tmp[3];
// @@@DBW		_itoa(tempOut[i], tmp, 16);
		sprintf( tmp, "%2x", tempOut[i] );	// @@@DBW
		if (strlen(tmp) == 1)
		{
			tmp[1] = tmp[0];
			tmp[0] = '0';
			tmp[2] = '\0';
		}
		tempHash += tmp;	

	}

	outHash = tempHash;

	return outHash;
}
// End SHA1Hash /////////////////////////////////////////////////



// Hash with SHA-2
//
//
// @@@DBW CString CHash::SHA2Hash()
string CHash::SHA2Hash()	// @@@DBW
{
	sha2_ctx sha;

	// Work out hash length
	int hashLength = sha2Strength / 8;

	if (hashOperation == 1)
	{
		// Hashing a string, open the object, give it the text, and finalise it
		sha2_begin(sha2Strength, &sha);
// @@@DBW		sha2_hash(reinterpret_cast<unsigned char *>(inHash.GetBuffer(inHash.GetLength())), inHash.GetLength(), &sha);
		sha2_hash((unsigned char *)(inHash.c_str()), inHash.length(), &sha);	// @@@DBW
		sha2_end(tempOut, &sha);
	}

	// Hash a file
	if (hashOperation == 2)
	{
// @@@DBW		if (hashFile.IsEmpty()) return "";
		if (hashFile.empty()) return "";	// @@@DBW
		sha2_begin(sha2Strength, &sha);

// @@@DBW		fileToHash = fopen(hashFile, "rb");
		fileToHash = fopen(hashFile.c_str(), "rb");
	
		while(1)
		{
			lenRead = fread(fileBuf, 1, SIZE_OF_BUFFER, fileToHash);
			if(lenRead != 0)
			{	
				sha2_hash(fileBuf, lenRead, &sha);
			}
			if(lenRead != SIZE_OF_BUFFER) break;
		}

		fclose(fileToHash); fileToHash = NULL;

		sha2_end(tempOut, &sha);
	}

	for (int i = 0; i < hashLength; i++)
	{
		char tmp[3];
// @@@DBW		_itoa(tempOut[i], tmp, 16);
		sprintf( tmp, "%2x", tempOut[i] );	// @@@DBW
		if (strlen(tmp) == 1)
		{
			tmp[1] = tmp[0];
			tmp[0] = '0';
			tmp[2] = '\0';
		}
		tempHash += tmp;		
	}

	outHash = tempHash;


	return outHash;
}
// End SHA2Hash /////////////////////////////////////////////////



// Set the hash algorithm
//
//
void CHash::SetHashAlgorithm(int Algo)
{
	hashAlgo = Algo;
}
// End SetHashAlgorithm /////////////////////////////////////////



// Set the formatting style of the output hash
//
//
void CHash::SetHashFormat(int Style)
{
	if (Style == 1 || Style == 2 || Style == 3 || Style == 4)
	{
		hashFormatting = Style;
	}
	else
	{
		hashFormatting = 1;
	}
}
// End SetHashFormat ////////////////////////////////////////////



// Set the hash operation
// Available operations:
//
// STRING_HASH
// FILE_HASH
//
//
void CHash::SetHashOperation(int Style)
{
	// Check the style is legit
	if (Style == 1 || Style == 2)
	{
		hashOperation = Style;
	}
	else
	{
		hashOperation = 1;
	}
}
// End SetHashOperation /////////////////////////////////////////



// Set the hash file
//
//
// @@@DBW void CHash::SetHashFile(CString File)
void CHash::SetHashFile(string File)	// @@@DBW
{
	hashFile = File;
}
// End SetHashFile //////////////////////////////////////////////



// Set the SHA-2 hash strength
//
// 256, 384 or 512
//
//
void CHash::SetSHA2Strength(int Strength)
{
	// Check the strength is legit
	if (Strength == 256 || Strength == 384 || Strength == 512)
	{
		sha2Strength = Strength;
	}
	else
	{
		sha2Strength = 256;
	}
}
// End SetSHA2Strength///////////////////////////////////////////



// Set the input string
//
//
// @@@DBW void CHash::SetHashString(CString Hash)
void CHash::SetHashString(string Hash)	// @@@DBW
{
// @@@DBW	inHash.Empty();
	inHash = "";	// @@@DBW
	inHash = Hash;
}
// End SetHashString ////////////////////////////////////////////
