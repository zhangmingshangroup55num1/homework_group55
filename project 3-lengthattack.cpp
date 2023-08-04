
#include <stdio.h>
#include <memory.h>
#include "sm3.h"
#include <string.h>
#include <iostream>
#include <time.h>
#include <immintrin.h>
#include <mmintrin.h>  
#include <xmmintrin.h> 
#include <emmintrin.h> 
#include <pmmintrin.h> 
#include <tmmintrin.h> 
#include <smmintrin.h> 
#include <nmmintrin.h> 
#include <wmmintrin.h> 
#include <immintrin.h>
#include <intrin.h>   
#include <emmintrin.h>
#include <immintrin.h>
using namespace std;
const unsigned char allChar[63] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
void generateString(unsigned char* dest, unsigned int len);
void generateString(unsigned char* dest, const unsigned int len)
{
	unsigned int cnt, randNo;
	srand((unsigned int)time(NULL));

	for (cnt = 0; cnt < len; cnt++)
	{
		randNo = rand() % 62;
		*dest = allChar[randNo];
		dest++;
	}

	*dest = '\0';
}

static const int endianTest = 1;
#define IsLittleEndian() (*(char *)&endianTest == 1)

#define LeftRotate(word, bits) ( (word) << (bits) | (word) >> (32 - (bits)) )

unsigned int* ReverseWord(unsigned int* word)
{
	unsigned char* byte, temp;

	byte = (unsigned char*)word;
	temp = byte[0];
	byte[0] = byte[3];
	byte[3] = temp;

	temp = byte[1];
	byte[1] = byte[2];
	byte[2] = temp;
	return word;
}

unsigned int T(int i)
{
	if (i >= 0 && i <= 15)
		return 0x79CC4519;
	else if (i >= 16 && i <= 63)
		return 0x7A879D8A;
	else
		return 0;
}

unsigned int FF(unsigned int X, unsigned int Y, unsigned int Z, int i)
{
	if (i >= 0 && i <= 15)
		return X ^ Y ^ Z;
	else if (i >= 16 && i <= 63)
		return (X & Y) | (X & Z) | (Y & Z);
	else
		return 0;
}

unsigned int GG(unsigned int X, unsigned int Y, unsigned int Z, int i)
{
	if (i >= 0 && i <= 15)
		return X ^ Y ^ Z;
	else if (i >= 16 && i <= 63)
		return (X & Y) | (~X & Z);
	else
		return 0;
}

unsigned int P0(unsigned int X)
{
	return X ^ LeftRotate(X, 9) ^ LeftRotate(X, 17);
}

unsigned int P1(unsigned int X)
{
	return X ^ LeftRotate(X, 15) ^ LeftRotate(X, 23);
}


void SM3Init(SM3Context* context)
{
	unsigned int iv[8] = { 0x66c7f0f4,0x62eeedd9,0xd1f2d46b,0xdc10e4e2,0x4167c487,0x5cf2f7a2,0x297da02b,0x8f4ba8e0 };
	_mm256_storeu_epi32(&context->intermediateHash[0], _mm256_loadu_epi32(&iv[0]));
}

void SM3Init_1(SM3Context* context)
{
	context->intermediateHash[0] = 0x7380166F;
	context->intermediateHash[1] = 0x4914B2B9;
	context->intermediateHash[2] = 0x172442D7;
	context->intermediateHash[3] = 0xDA8A0600;
	context->intermediateHash[4] = 0xA96F30BC;
	context->intermediateHash[5] = 0x163138AA;
	context->intermediateHash[6] = 0xE38DEE4D;
	context->intermediateHash[7] = 0xB0FB0E4E;

}
void SM3Init_2(SM3Context* context)
{
	unsigned int iv[8] = { 0x49019597,0x27066d2e,0x50d4c2f1,0x1ef3b47b,0xc74a174e,0x7362ff4c,0xd79a36b9,0xb068c072 };
	_mm256_storeu_epi32(&context->intermediateHash[0], _mm256_loadu_epi32(&iv[0]));
}

void SM3ProcessMessageBlock(SM3Context* context)
{
	int i;
	unsigned int W[68];
	unsigned int W_[64];
	unsigned int A, B, C, D, E, F, G, H, SS1, SS2, TT1, TT2;

	for (i = 0; i < 16; i += 2)
	{
		W[i] = *(unsigned int*)(context->messageBlock + i * 4);
		W[i + 1] = *(unsigned int*)(context->messageBlock + (i + 1) * 4);
		if (IsLittleEndian())
		{
			ReverseWord(W + i);
			ReverseWord(W + i + 1);
		}
	}


	for (i = 16; i < 68; i += 2)
	{
		W[i] = P1(W[i - 16] ^ W[i - 9] ^ LeftRotate(W[i - 3], 15))
			^ LeftRotate(W[i - 13], 7)
			^ W[i - 6];
		W[i + 1] = P1(W[i - 15] ^ W[i - 8] ^ LeftRotate(W[i - 2], 15))
			^ LeftRotate(W[i - 12], 7)
			^ W[i - 5];
	}
	for (i = 0; i < 64; i += 8)
	{

		__m256i temp3 = _mm256_xor_epi32(_mm256_loadu_epi32(&W[i]), _mm256_loadu_epi32(&W[i + 4]));
		_mm256_storeu_epi32(&W_[i], temp3);

	
	}

	A = context->intermediateHash[0];
	B = context->intermediateHash[1];
	C = context->intermediateHash[2];
	D = context->intermediateHash[3];
	E = context->intermediateHash[4];
	F = context->intermediateHash[5];
	G = context->intermediateHash[6];
	H = context->intermediateHash[7];

	for (i = 0; i < 64; i++)
	{
		SS1 = LeftRotate((LeftRotate(A, 12) + E + LeftRotate(T(i), i)), 7);
		SS2 = SS1 ^ LeftRotate(A, 12);
		TT1 = FF(A, B, C, i) + D + SS2 + W_[i];
		TT2 = GG(E, F, G, i) + H + SS1 + W[i];
		D = C;
		C = LeftRotate(B, 9);
		B = A;
		A = TT1;
		H = G;
		G = LeftRotate(F, 19);
		F = E;
		E = P0(TT2);
	}
	context->intermediateHash[0] ^= A;
	context->intermediateHash[1] ^= B;
	context->intermediateHash[2] ^= C;
	context->intermediateHash[3] ^= D;
	context->intermediateHash[4] ^= E;
	context->intermediateHash[5] ^= F;
	context->intermediateHash[6] ^= G;
	context->intermediateHash[7] ^= H;
}


unsigned char* SM3Calc(const unsigned char* message,
	unsigned int messageLen, unsigned char digest[SM3_HASH_SIZE])
{
	SM3Context context;
	unsigned int i, remainder, bitLen;

	SM3Init(&context);

	for (i = 0; i < messageLen / 64; i += 2)
	{
		memcpy(context.messageBlock, message + i * 64, 64);
		SM3ProcessMessageBlock(&context);
		memcpy(context.messageBlock, message + (i + 1) * 64, 64);
		SM3ProcessMessageBlock(&context);
	}

	bitLen = messageLen * 8;
	if (IsLittleEndian())
		ReverseWord(&bitLen);
	remainder = messageLen % 64;
	memcpy(context.messageBlock, message + i * 64, remainder);
	context.messageBlock[remainder] = 0x80;
	if (remainder <= 55)
	{

		memset(context.messageBlock + remainder + 1, 0, 64 - remainder - 1 - 8 + 4);
		memcpy(context.messageBlock + 64 - 4, &bitLen, 4);
		SM3ProcessMessageBlock(&context);
	}
	else
	{
		memset(context.messageBlock + remainder + 1, 0, 64 - remainder - 1);
		SM3ProcessMessageBlock(&context);
		memset(context.messageBlock, 0, 64 - 4);
		memcpy(context.messageBlock + 64 - 4, &bitLen, 4);
		SM3ProcessMessageBlock(&context);
	}

	if (IsLittleEndian())
		for (i = 0; i < 8; i++)
			ReverseWord(context.intermediateHash + i);
	memcpy(digest, context.intermediateHash, SM3_HASH_SIZE);

	return digest;
}
unsigned char* SM3Calc_1(const unsigned char* message,
	unsigned int messageLen, unsigned char digest[SM3_HASH_SIZE])
{
	SM3Context context;
	unsigned int i, remainder, bitLen;
	SM3Init_1(&context);
	for (i = 0; i < messageLen / 64; i += 2)
	{
		memcpy(context.messageBlock, message + i * 64, 64);
		SM3ProcessMessageBlock(&context);
		memcpy(context.messageBlock, message + (i + 1) * 64, 64);
		SM3ProcessMessageBlock(&context);
	}

	/* 填充消息分组，并处理 */
	bitLen = messageLen * 8;
	if (IsLittleEndian())
		ReverseWord(&bitLen);
	remainder = messageLen % 64;
	memcpy(context.messageBlock, message + i * 64, remainder);
	context.messageBlock[remainder] = 0x80;
	if (remainder <= 55)
	{
		/* 长度按照大端法占8个字节，该程序只考虑长度在 2**32 - 1（单位：比特）以内的情况，
		* 故将高 4 个字节赋为 0 。*/
		memset(context.messageBlock + remainder + 1, 0, 64 - remainder - 1 - 8 + 4);
		memcpy(context.messageBlock + 64 - 4, &bitLen, 4);
		SM3ProcessMessageBlock(&context);
	}
	else
	{
		memset(context.messageBlock + remainder + 1, 0, 64 - remainder - 1);
		SM3ProcessMessageBlock(&context);
		/* 长度按照大端法占8个字节，该程序只考虑长度在 2**32 - 1（单位：比特）以内的情况，
		* 故将高 4 个字节赋为 0 。*/
		memset(context.messageBlock, 0, 64 - 4);
		memcpy(context.messageBlock + 64 - 4, &bitLen, 4);
		SM3ProcessMessageBlock(&context);
	}

	/* 返回结果 */
	if (IsLittleEndian())
		for (i = 0; i < 8; i++)
			ReverseWord(context.intermediateHash + i);
	memcpy(digest, context.intermediateHash, SM3_HASH_SIZE);

	return digest;
}
unsigned char* SM3Calc_2(const unsigned char* message,
	unsigned int messageLen, unsigned char digest[SM3_HASH_SIZE])
{
	SM3Context context;
	unsigned int i, remainder, bitLen;

	/* 初始化上下文 */
	SM3Init_2(&context);

	for (i = 0; i < messageLen / 64; i += 2)
	{
		memcpy(context.messageBlock, message + i * 64, 64);
		SM3ProcessMessageBlock(&context);
		memcpy(context.messageBlock, message + (i + 1) * 64, 64);
		SM3ProcessMessageBlock(&context);
	}

	bitLen = messageLen * 8;
	if (IsLittleEndian())
		ReverseWord(&bitLen);
	remainder = messageLen % 64;
	memcpy(context.messageBlock, message + i * 64, remainder);
	context.messageBlock[remainder] = 0x80;
	if (remainder <= 55)
	{
		
		memset(context.messageBlock + remainder + 1, 0, 64 - remainder - 1 - 8 + 4);
		memcpy(context.messageBlock + 64 - 4, &bitLen, 4);
		SM3ProcessMessageBlock(&context);
	}
	else
	{
		memset(context.messageBlock + remainder + 1, 0, 64 - remainder - 1);
		SM3ProcessMessageBlock(&context);
		/* 长度按照大端法占8个字节，该程序只考虑长度在 2**32 - 1（单位：比特）以内的情况，
		* 故将高 4 个字节赋为 0 。*/
		memset(context.messageBlock, 0, 64 - 4);
		memcpy(context.messageBlock + 64 - 4, &bitLen, 4);
		SM3ProcessMessageBlock(&context);
	}

	/* 返回结果 */
	if (IsLittleEndian())
		for (i = 0; i < 8; i++)
			ReverseWord(context.intermediateHash + i);
	memcpy(digest, context.intermediateHash, SM3_HASH_SIZE);

	return digest;
}
unsigned char secret[256] = "hello";
const unsigned int strlen1 = 3;
unsigned char output[32];


int main(int argc, char* argv[])
{

	printf("secret:\n");
	printf("%s\n", secret);

	unsigned char m_1[strlen1 + 1] = {};
	generateString(m_1, strlen1);
	cout << "append message:" << endl;
	for (int i = 0; i < strlen1; i++)
	{
		printf("%02x", m_1[i]);
	}
	cout << endl;
	unsigned char result[32];
	SM3Calc(m_1, strlen1, result);
	cout << "hash(append):" << endl;
	for (int i = 0; i < 32; i++)
	{
		printf("%02x", result[i]);
		if (((i + 1) % 4) == 0) printf(" ");
	}
	cout << endl;
	const unsigned int strlen = strlen1;
	unsigned int messageLen = strlen;
	SM3Context context;
	unsigned int i, remainder, bitLen;

	SM3Init_1(&context);

	for (i = 0; i < messageLen / 64; i += 2)
	{
		memcpy(context.messageBlock, secret + i * 64, 64);
		SM3ProcessMessageBlock(&context);
		memcpy(context.messageBlock, secret + (i + 1) * 64, 64);
		SM3ProcessMessageBlock(&context);
	}

	bitLen = messageLen * 8;
	if (IsLittleEndian())
		ReverseWord(&bitLen);
	remainder = messageLen % 64;
	memcpy(context.messageBlock, secret + i * 64, remainder);
	context.messageBlock[remainder] = 0x80;
	memset(context.messageBlock + remainder + 1, 0, 64 - remainder - 1 - 8 + 4);
	memcpy(context.messageBlock + 64 - 4, &bitLen, 4);

	unsigned char array[64 + strlen];
	for (int i = 0; i < 64; i++)
	{
		array[i] = context.messageBlock[i];
	}
	for (int i = 64; i < 64 + strlen; i++)
	{
		array[i] = m_1[i - 64];
	}

	cout << "s+p+a:" << endl;
	for (int i = 0; i < 64 + strlen; i++)
	{
		printf("%02x", array[i]);
		if (((i + 1) % 4) == 0) printf(" ");
	}
	cout << endl;
	unsigned char result_1[32];
	SM3Calc_1(context.messageBlock, 64, result_1);
	unsigned char result_2[32];
	SM3Calc_2(m_1, strlen, result_2);
	cout << "hash(s+p):" << endl;
	for (int i = 0; i < 32; i++)
	{
		printf("%02x", result_1[i]);
		if (((i + 1) % 4) == 0) printf(" ");
	}
	cout << endl;
	cout << "hash(s+p+a):" << endl;
	for (int i = 0; i < 32; i++)
	{
		printf("%02x", result[i]);
		if (((i + 1) % 4) == 0) printf(" ");
	}
	cout << endl;
	system("pause");
}