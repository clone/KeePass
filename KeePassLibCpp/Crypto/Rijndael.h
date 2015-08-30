// Public domain implementation of the Rijndael cipher.
// Modified version of Szymon Stefanek's implementation of
// the Rijndael block cipher C++ class.
// Editor: Dominik Reichl (dominik.reichl@t-online.de)

#ifndef ___RIJNDAEL_H___
#define ___RIJNDAEL_H___

#include "../SysDefEx.h"

// File : rijndael.h
// Creation date : Sun Nov 5 2000 03:21:05 CEST
// Author : Szymon Stefanek (stefanek@tin.it)
//
// Another implementation of the Rijndael cipher.
// This is intended to be an easily usable library file.
// This code is public domain.
// Based on the Vincent Rijmen and K.U.Leuven implementation 2.4.

// Original Copyright notice:
//
//    rijndael-alg-fst.c   v2.4   April '2000
//    rijndael-alg-fst.h
//    rijndael-api-fst.c
//    rijndael-api-fst.h
//
//    Optimised ANSI C code
//
//    authors: v1.0: Antoon Bosselaers
//             v2.0: Vincent Rijmen, K.U.Leuven
//             v2.3: Paulo Barreto
//             v2.4: Vincent Rijmen, K.U.Leuven
//
//    This code is placed in the public domain.
//

//
// This implementation works on 128 , 192 , 256 bit keys
// and on 128 bit blocks

// Example of usage:
//
//  // Input data
//  unsigned char key[32];                       // The key
//  initializeYour256BitKey();                   // Obviously initialized with sth
//  const unsigned char * plainText = getYourPlainText(); // Your plain text
//  int plainTextLen = strlen(plainText);        // Plain text length
//
//  // Encrypting
//  Rijndael rin;
//  unsigned char output[plainTextLen + 16];
//
//  rin.init(Rijndael::CBC,Rijndael::Encrypt,key,Rijndael::Key32Bytes);
//  // It is a good idea to check the error code
//  int len = rin.padEncrypt(plainText,len,output);
//  if(len >= 0)useYourEncryptedText();
//  else encryptError(len);
//
//  // Decrypting: we can reuse the same object
//  unsigned char output2[len];
//  rin.init(Rijndael::CBC,Rijndael::Decrypt,key,Rijndael::Key32Bytes));
//  len = rin.padDecrypt(output,len,output2);
//  if(len >= 0)useYourDecryptedText();
//  else decryptError(len);

#define RD_MAX_KEY_COLUMNS (256/32)
#define RD_MAX_ROUNDS      14
#define RD_MAX_IV_SIZE      16

// Error codes
#define RIJNDAEL_SUCCESS 0
#define RIJNDAEL_UNSUPPORTED_MODE -1
#define RIJNDAEL_UNSUPPORTED_DIRECTION -2
#define RIJNDAEL_UNSUPPORTED_KEY_LENGTH -3
#define RIJNDAEL_BAD_KEY -4
#define RIJNDAEL_NOT_INITIALIZED -5
#define RIJNDAEL_BAD_DIRECTION -6
#define RIJNDAEL_CORRUPTED_DATA -7

class CRijndael
{	
public:
	// Creates a Rijndael cipher object
	// You have to call init() before you can encrypt or decrypt stuff
	CRijndael();
	~CRijndael();

	enum Direction { EncryptDir = 0, DecryptDir = 1 };
	enum Mode { ECB = 0, CBC = 1, CFB1 = 2 };
	enum KeyLength { Key16Bytes = 16, Key24Bytes = 24, Key32Bytes = 32 };

protected:
	// Internal stuff
	enum State { Valid = 0, Invalid = 1 };

	State     m_state;
	Mode      m_mode;
	Direction m_direction;
	UINT8     m_initVector[RD_MAX_IV_SIZE];
	UINT32    m_uRounds;
	UINT8     m_expandedKey[RD_MAX_ROUNDS+1][4][4];

public:
	//////////////////////////////////////////////////////////////////////////
	// API
	//////////////////////////////////////////////////////////////////////////

	// Init(): Initializes the crypt session
	// Returns RIJNDAEL_SUCCESS or an error code
	// mode      : Rijndael::ECB, Rijndael::CBC or Rijndael::CFB1
	//             You have to use the same mode for encrypting and decrypting
	// dir       : Rijndael::Encrypt or Rijndael::Decrypt
	//             A cipher instance works only in one direction
	//             (Well , it could be easily modified to work in both
	//             directions with a single init() call, but it looks
	//             useless to me...anyway , it is a matter of generating
	//             two expanded keys)
	// key       : array of unsigned octets , it can be 16 , 24 or 32 bytes long
	//             this CAN be binary data (it is not expected to be null terminated)
	// keyLen    : Rijndael::Key16Bytes , Rijndael::Key24Bytes or Rijndael::Key32Bytes
	// initVector: initialization vector, you will usually use NULL here
	int Init(Mode mode, Direction dir, const UINT8 *key, KeyLength keyLen, const UINT8 *initVector);

	// Encrypts the input array (can be binary data)
	// The input array length must be a multiple of 16 bytes, the remaining part
	// is DISCARDED.
	// so it actually encrypts inputLen / 128 blocks of input and puts it in outBuffer
	// Input len is in BITS!
	// outBuffer must be at least inputLen / 8 bytes long.
	// Returns the encrypted buffer length in BITS or an error code < 0 in case of error
	int BlockEncrypt(const UINT8 *input, int inputLen, UINT8 *outBuffer);

	// Encrypts the input array (can be binary data)
	// The input array can be any length , it is automatically padded on a 16 byte boundary.
	// Input len is in BYTES!
	// outBuffer must be at least (inputLen + 16) bytes long
	// Returns the encrypted buffer length in BYTES or an error code < 0 in case of error
	int PadEncrypt(const UINT8 *input, int inputOctets, UINT8 *outBuffer);

	// Decrypts the input vector
	// Input len is in BITS!
	// outBuffer must be at least inputLen / 8 bytes long
	// Returns the decrypted buffer length in BITS and an error code < 0 in case of error
	int BlockDecrypt(const UINT8 *input, int inputLen, UINT8 *outBuffer);

	// Decrypts the input vector
	// Input len is in BYTES!
	// outBuffer must be at least inputLen bytes long
	// Returns the decrypted buffer length in BYTES and an error code < 0 in case of error
	int PadDecrypt(const UINT8 *input, int inputOctets, UINT8 *outBuffer);

protected:
	void KeySched(UINT8 key[RD_MAX_KEY_COLUMNS][4]);
	void KeyEncToDec();
	void Encrypt(const UINT8 a[16], UINT8 b[16]);
	void Decrypt(const UINT8 a[16], UINT8 b[16]);
};

#endif // ___RIJNDAEL_H___
