
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
	

	unsigned int iv[8] = { 0x7380166F, 0x4914B2B9,0x172442D7,0xDA8A0600,0xA96F30BC,0x163138AA,0xE38DEE4D,0xB0FB0E4E };
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


int main(int argc, char* argv[])
{
	clock_t start, end;

	unsigned char input[256] = "hello sm3";
	cout << "message:" << endl;
	for (int i = 0; i < 3; i++)
	{
		printf("%02x", input[i]);
	}
	cout << endl;
	unsigned char output[32];
	start = clock();
	for (int i = 0; i < 10000; i++)
	{

		SM3Calc(input, 3, output);
	}
	end = clock();
	cout << "hash:" << endl;
	for (int i = 0; i < 32; i++)
	{
		printf("%02x", output[i]);
		if (((i + 1) % 4) == 0) printf(" ");
	}
	cout << endl;
	cout << "ʱ��: " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;

	system("pause");
}