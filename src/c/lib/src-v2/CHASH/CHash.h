// CHash.h : header file
//

#ifndef _CHASH_H
#define _CHASH_H

#include <string>	// @@@DBW

using namespace std;	// @@@DBW

/////////////////////////////////////////////////////////////////////////////
// CHash definitions

class CHash
{
// Construction
public:
	CHash();   // Standard constructor
// @@@DBW	CString DoHash();
	string DoHash();	// @@@DBW
	int GetHashAlgorithm();
	int GetHashOperation();
// @@@DBW	CString GetHashFile();
	string GetHashFile();	// @@@DBW
// @@@DBW	CString GetHashString();
	string GetHashString();	// @@@DBW
	int GetSHA2Strength();
// @@@DBW	CString GOSTHash();
	string GOSTHash();	// @@@DBW
// @@@DBW	CString MD2Hash();
	string MD2Hash();	// @@@DBW
// @@@DBW	CString MD4Hash();
	string MD4Hash();	// @@@DBW
// @@@DBW	CString MD5Hash();
	string MD5Hash();	// @@@DBW
// @@@DBW	CString SHA1Hash();
	string SHA1Hash();	// @@@DBW
// @@@DBW	CString SHA2Hash();
	string SHA2Hash();	// @@@DBW
	void SetHashAlgorithm(int Algo);
	void SetHashOperation(int Style);
// @@@DBW	void SetHashFile(CString File);
	void SetHashFile(string File);	// @@@DBW
	void SetHashFormat(int Style);
// @@@DBW	void SetHashString(CString Hash);
	void SetHashString(string Hash);	// @@@DBW
	void SetSHA2Strength(int Strength);

// Implementation
protected:
// @@@DBW	CString    inHash;
	string	   inHash;	// @@@DBW
	int        hashAlgo;
// @@@DBW	CString    hashFile;
	string     hashFile;	// @@@DBW
	int        hashFormatting;
	int        hashStyle;
	int        hashOperation;
	int        sha2Strength;
};

// Definitions of some kind
#define        STRING_HASH            1
#define        FILE_HASH              2

#define        SIZE_OF_BUFFER         16000

// Algorithms
#define        GOSTHASH               1
#define        MD2                    2
#define        MD4                    3
#define        MD5                    4
#define        SHA1                   5
#define        SHA2                   6

// Formatting options
#define        LOWERCASE_NOSPACES     1
#define        LOWERCASE_SPACES       2
#define        UPPERCASE_NOSPACES     3
#define        UPPERCASE_SPACES       4

#endif
