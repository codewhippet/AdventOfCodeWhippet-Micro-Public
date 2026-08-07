#include "stdafx.h"
#include "uMD5.h"
#include <assert.h>

// This file is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License
// https://en.wikipedia.org/wiki/Wikipedia:Text_of_the_Creative_Commons_Attribution-ShareAlike_4.0_International_License

// Implementation based on pseudocode here:
// https://en.wikipedia.org/wiki/MD5

static const uint8_t s[64] =
{
	7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
	5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
	4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
	6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,
};

static const uint32_t K[64] =
{
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
};

inline static uint32_t leftrotate(uint32_t F, uint8_t shift)
{
#if _WIN32
	return _rotl(F, shift);
#else
	return (F << shift) | (F >> (32 - shift));
#endif
}

#if 0
static void HashChunk(const uint32_t M[16], uint32_t hash[4])
{
	uint32_t A = hash[0];
	uint32_t B = hash[1];
	uint32_t C = hash[2];
	uint32_t D = hash[3];

	uint32_t F, g;

	for (int i = 0; i < 16; i++)
	{
		F = (B & C) | (~B & D);
		g = i;

		F = F + A + K[i] + M[g];
		A = D;
		D = C;
		C = B;
		B = B + leftrotate(F, s[i]);
	}

	for (int i = 16; i < 32; i++)
	{
		F = (D & B) | (~D & C);
		g = (5 * i + 1) & 0xf;

		F = F + A + K[i] + M[g];
		A = D;
		D = C;
		C = B;
		B = B + leftrotate(F, s[i]);
	}

	for (int i = 32; i < 48; i++)
	{
		F = B ^ C ^ D;
		g = (3 * i + 5) & 0xf;

		F = F + A + K[i] + M[g];
		A = D;
		D = C;
		C = B;
		B = B + leftrotate(F, s[i]);
	}

	for (int i = 48; i < 64; i++)
	{
		F = C ^ (B | ~D);
		g = (7 * i) & 0xf;

		F = F + A + K[i] + M[g];
		A = D;
		D = C;
		C = B;
		B = B + leftrotate(F, s[i]);
	}

	hash[0] += A;
	hash[1] += B;
	hash[2] += C;
	hash[3] += D;
}
#endif

#define ROUND_F(TO, A, B, C, D, s_i, K_i, M_g) \
	F = (B & C) | (~B & D); \
	F = F + A + K_i + M_g; \
	TO = B + leftrotate(F, s_i);

#define ROUND_G(TO, A, B, C, D, s_i, K_i, M_g) \
	F = (D & B) | (~D & C); \
	F = F + A + K_i + M_g; \
	TO = B + leftrotate(F, s_i);

#define ROUND_H(TO, A, B, C, D, s_i, K_i, M_g) \
	F = B ^ C ^ D; \
	F = F + A + K_i + M_g; \
	TO = B + leftrotate(F, s_i);

#define ROUND_I(TO, A, B, C, D, s_i, K_i, M_g) \
	F = C ^ (B | ~D); \
	F = F + A + K_i + M_g; \
	TO = B + leftrotate(F, s_i);

static void HashChunk(const uint32_t M[16], uint32_t hash[4])
{
	uint32_t A = hash[0];
	uint32_t B = hash[1];
	uint32_t C = hash[2];
	uint32_t D = hash[3];

	uint32_t F;

	ROUND_F(A, A, B, C, D,  7, 0xd76aa478, M[ 0]);
	ROUND_F(D, D, A, B, C, 12, 0xe8c7b756, M[ 1]);
	ROUND_F(C, C, D, A, B, 17, 0x242070db, M[ 2]);
	ROUND_F(B, B, C, D, A, 22, 0xc1bdceee, M[ 3]);
	ROUND_F(A, A, B, C, D,  7, 0xf57c0faf, M[ 4]);
	ROUND_F(D, D, A, B, C, 12, 0x4787c62a, M[ 5]);
	ROUND_F(C, C, D, A, B, 17, 0xa8304613, M[ 6]);
	ROUND_F(B, B, C, D, A, 22, 0xfd469501, M[ 7]);
	ROUND_F(A, A, B, C, D,  7, 0x698098d8, M[ 8]);
	ROUND_F(D, D, A, B, C, 12, 0x8b44f7af, M[ 9]);
	ROUND_F(C, C, D, A, B, 17, 0xffff5bb1, M[10]);
	ROUND_F(B, B, C, D, A, 22, 0x895cd7be, M[11]);
	ROUND_F(A, A, B, C, D,  7, 0x6b901122, M[12]);
	ROUND_F(D, D, A, B, C, 12, 0xfd987193, M[13]);
	ROUND_F(C, C, D, A, B, 17, 0xa679438e, M[14]);
	ROUND_F(B, B, C, D, A, 22, 0x49b40821, M[15]);

	ROUND_G(A, A, B, C, D,  5, 0xf61e2562, M[ 1]);
	ROUND_G(D, D, A, B, C,  9, 0xc040b340, M[ 6]);
	ROUND_G(C, C, D, A, B, 14, 0x265e5a51, M[11]);
	ROUND_G(B, B, C, D, A, 20, 0xe9b6c7aa, M[ 0]);
	ROUND_G(A, A, B, C, D,  5, 0xd62f105d, M[ 5]);
	ROUND_G(D, D, A, B, C,  9, 0x02441453, M[10]);
	ROUND_G(C, C, D, A, B, 14, 0xd8a1e681, M[15]);
	ROUND_G(B, B, C, D, A, 20, 0xe7d3fbc8, M[ 4]);
	ROUND_G(A, A, B, C, D,  5, 0x21e1cde6, M[ 9]);
	ROUND_G(D, D, A, B, C,  9, 0xc33707d6, M[14]);
	ROUND_G(C, C, D, A, B, 14, 0xf4d50d87, M[ 3]);
	ROUND_G(B, B, C, D, A, 20, 0x455a14ed, M[ 8]);
	ROUND_G(A, A, B, C, D,  5, 0xa9e3e905, M[13]);
	ROUND_G(D, D, A, B, C,  9, 0xfcefa3f8, M[ 2]);
	ROUND_G(C, C, D, A, B, 14, 0x676f02d9, M[ 7]);
	ROUND_G(B, B, C, D, A, 20, 0x8d2a4c8a, M[12]);

	ROUND_H(A, A, B, C, D,  4, 0xfffa3942, M[ 5]);
	ROUND_H(D, D, A, B, C, 11, 0x8771f681, M[ 8]);
	ROUND_H(C, C, D, A, B, 16, 0x6d9d6122, M[11]);
	ROUND_H(B, B, C, D, A, 23, 0xfde5380c, M[14]);
	ROUND_H(A, A, B, C, D,  4, 0xa4beea44, M[ 1]);
	ROUND_H(D, D, A, B, C, 11, 0x4bdecfa9, M[ 4]);
	ROUND_H(C, C, D, A, B, 16, 0xf6bb4b60, M[ 7]);
	ROUND_H(B, B, C, D, A, 23, 0xbebfbc70, M[10]);
	ROUND_H(A, A, B, C, D,  4, 0x289b7ec6, M[13]);
	ROUND_H(D, D, A, B, C, 11, 0xeaa127fa, M[ 0]);
	ROUND_H(C, C, D, A, B, 16, 0xd4ef3085, M[ 3]);
	ROUND_H(B, B, C, D, A, 23, 0x04881d05, M[ 6]);
	ROUND_H(A, A, B, C, D,  4, 0xd9d4d039, M[ 9]);
	ROUND_H(D, D, A, B, C, 11, 0xe6db99e5, M[12]);
	ROUND_H(C, C, D, A, B, 16, 0x1fa27cf8, M[15]);
	ROUND_H(B, B, C, D, A, 23, 0xc4ac5665, M[ 2]);

	ROUND_I(A, A, B, C, D,  6, 0xf4292244, M[ 0]);
	ROUND_I(D, D, A, B, C, 10, 0x432aff97, M[ 7]);
	ROUND_I(C, C, D, A, B, 15, 0xab9423a7, M[14]);
	ROUND_I(B, B, C, D, A, 21, 0xfc93a039, M[ 5]);
	ROUND_I(A, A, B, C, D,  6, 0x655b59c3, M[12]);
	ROUND_I(D, D, A, B, C, 10, 0x8f0ccc92, M[ 3]);
	ROUND_I(C, C, D, A, B, 15, 0xffeff47d, M[10]);
	ROUND_I(B, B, C, D, A, 21, 0x85845dd1, M[ 1]);
	ROUND_I(A, A, B, C, D,  6, 0x6fa87e4f, M[ 8]);
	ROUND_I(D, D, A, B, C, 10, 0xfe2ce6e0, M[15]);
	ROUND_I(C, C, D, A, B, 15, 0xa3014314, M[ 6]);
	ROUND_I(B, B, C, D, A, 21, 0x4e0811a1, M[13]);
	ROUND_I(A, A, B, C, D,  6, 0xf7537e82, M[ 4]);
	ROUND_I(D, D, A, B, C, 10, 0xbd3af235, M[11]);
	ROUND_I(C, C, D, A, B, 15, 0x2ad7d2bb, M[ 2]);
	ROUND_I(B, B, C, D, A, 21, 0xeb86d391, M[ 9]);

	hash[0] += A;
	hash[1] += B;
	hash[2] += C;
	hash[3] += D;
}

static void HashMiniChunk(const uint32_t M[16], uint32_t hash[4])
{
	uint32_t A = hash[0];
	uint32_t B = hash[1];
	uint32_t C = hash[2];
	uint32_t D = hash[3];

	uint32_t F;

	ROUND_F(A, A, B, C, D,  7, 0xd76aa478, M[ 0]);
	ROUND_F(D, D, A, B, C, 12, 0xe8c7b756, M[ 1]);
	ROUND_F(C, C, D, A, B, 17, 0x242070db, M[ 2]);
	ROUND_F(B, B, C, D, A, 22, 0xc1bdceee, M[ 3]);
	ROUND_F(A, A, B, C, D,  7, 0xf57c0faf, M[ 4]);
	ROUND_F(D, D, A, B, C, 12, 0x4787c62a, M[ 5]);
	ROUND_F(C, C, D, A, B, 17, 0xa8304613, M[ 6]);
	ROUND_F(B, B, C, D, A, 22, 0xfd469501, M[ 7]);
	ROUND_F(A, A, B, C, D,  7, 0x698098d8, M[ 8]);
	ROUND_F(D, D, A, B, C, 12, 0x8b44f7af, 0);
	ROUND_F(C, C, D, A, B, 17, 0xffff5bb1, 0);
	ROUND_F(B, B, C, D, A, 22, 0x895cd7be, 0);
	ROUND_F(A, A, B, C, D,  7, 0x6b901122, 0);
	ROUND_F(D, D, A, B, C, 12, 0xfd987193, 0);
	ROUND_F(C, C, D, A, B, 17, 0xa679438e, M[14]);
	ROUND_F(B, B, C, D, A, 22, 0x49b40821, 0);

	ROUND_G(A, A, B, C, D,  5, 0xf61e2562, M[ 1]);
	ROUND_G(D, D, A, B, C,  9, 0xc040b340, M[ 6]);
	ROUND_G(C, C, D, A, B, 14, 0x265e5a51, 0);
	ROUND_G(B, B, C, D, A, 20, 0xe9b6c7aa, M[ 0]);
	ROUND_G(A, A, B, C, D,  5, 0xd62f105d, M[ 5]);
	ROUND_G(D, D, A, B, C,  9, 0x02441453, 0);
	ROUND_G(C, C, D, A, B, 14, 0xd8a1e681, 0);
	ROUND_G(B, B, C, D, A, 20, 0xe7d3fbc8, M[ 4]);
	ROUND_G(A, A, B, C, D,  5, 0x21e1cde6, 0);
	ROUND_G(D, D, A, B, C,  9, 0xc33707d6, M[14]);
	ROUND_G(C, C, D, A, B, 14, 0xf4d50d87, M[ 3]);
	ROUND_G(B, B, C, D, A, 20, 0x455a14ed, M[ 8]);
	ROUND_G(A, A, B, C, D,  5, 0xa9e3e905, 0);
	ROUND_G(D, D, A, B, C,  9, 0xfcefa3f8, M[ 2]);
	ROUND_G(C, C, D, A, B, 14, 0x676f02d9, M[ 7]);
	ROUND_G(B, B, C, D, A, 20, 0x8d2a4c8a, 0);

	ROUND_H(A, A, B, C, D,  4, 0xfffa3942, M[ 5]);
	ROUND_H(D, D, A, B, C, 11, 0x8771f681, M[ 8]);
	ROUND_H(C, C, D, A, B, 16, 0x6d9d6122, 0);
	ROUND_H(B, B, C, D, A, 23, 0xfde5380c, M[14]);
	ROUND_H(A, A, B, C, D,  4, 0xa4beea44, M[ 1]);
	ROUND_H(D, D, A, B, C, 11, 0x4bdecfa9, M[ 4]);
	ROUND_H(C, C, D, A, B, 16, 0xf6bb4b60, M[ 7]);
	ROUND_H(B, B, C, D, A, 23, 0xbebfbc70, 0);
	ROUND_H(A, A, B, C, D,  4, 0x289b7ec6, 0);
	ROUND_H(D, D, A, B, C, 11, 0xeaa127fa, M[ 0]);
	ROUND_H(C, C, D, A, B, 16, 0xd4ef3085, M[ 3]);
	ROUND_H(B, B, C, D, A, 23, 0x04881d05, M[ 6]);
	ROUND_H(A, A, B, C, D,  4, 0xd9d4d039, 0);
	ROUND_H(D, D, A, B, C, 11, 0xe6db99e5, 0);
	ROUND_H(C, C, D, A, B, 16, 0x1fa27cf8, 0);
	ROUND_H(B, B, C, D, A, 23, 0xc4ac5665, M[ 2]);

	ROUND_I(A, A, B, C, D,  6, 0xf4292244, M[ 0]);
	ROUND_I(D, D, A, B, C, 10, 0x432aff97, M[ 7]);
	ROUND_I(C, C, D, A, B, 15, 0xab9423a7, M[14]);
	ROUND_I(B, B, C, D, A, 21, 0xfc93a039, M[ 5]);
	ROUND_I(A, A, B, C, D,  6, 0x655b59c3, 0);
	ROUND_I(D, D, A, B, C, 10, 0x8f0ccc92, M[ 3]);
	ROUND_I(C, C, D, A, B, 15, 0xffeff47d, 0);
	ROUND_I(B, B, C, D, A, 21, 0x85845dd1, M[ 1]);
	ROUND_I(A, A, B, C, D,  6, 0x6fa87e4f, M[ 8]);
	ROUND_I(D, D, A, B, C, 10, 0xfe2ce6e0, 0);
	ROUND_I(C, C, D, A, B, 15, 0xa3014314, M[ 6]);
	ROUND_I(B, B, C, D, A, 21, 0x4e0811a1, 0);
	ROUND_I(A, A, B, C, D,  6, 0xf7537e82, M[ 4]);
	ROUND_I(D, D, A, B, C, 10, 0xbd3af235, 0);
	ROUND_I(C, C, D, A, B, 15, 0x2ad7d2bb, M[ 2]);
	ROUND_I(B, B, C, D, A, 21, 0xeb86d391, 0);

	hash[0] += A;
	hash[1] += B;
	hash[2] += C;
	hash[3] += D;
}

static uint32_t HashMiniChunkMSBOnly(const uint32_t M[16])
{
	uint32_t A = 0x67452301;
	uint32_t B = 0xefcdab89;
	uint32_t C = 0x98badcfe;
	uint32_t D = 0x10325476;

	uint32_t F;

	ROUND_F(A, A, B, C, D,  7, 0xd76aa478, M[ 0]);
	ROUND_F(D, D, A, B, C, 12, 0xe8c7b756, M[ 1]);
	ROUND_F(C, C, D, A, B, 17, 0x242070db, M[ 2]);
	ROUND_F(B, B, C, D, A, 22, 0xc1bdceee, M[ 3]);
	ROUND_F(A, A, B, C, D,  7, 0xf57c0faf, M[ 4]);
	ROUND_F(D, D, A, B, C, 12, 0x4787c62a, M[ 5]);
	ROUND_F(C, C, D, A, B, 17, 0xa8304613, M[ 6]);
	ROUND_F(B, B, C, D, A, 22, 0xfd469501, M[ 7]);
	ROUND_F(A, A, B, C, D,  7, 0x698098d8, M[ 8]);
	ROUND_F(D, D, A, B, C, 12, 0x8b44f7af, 0);
	ROUND_F(C, C, D, A, B, 17, 0xffff5bb1, 0);
	ROUND_F(B, B, C, D, A, 22, 0x895cd7be, 0);
	ROUND_F(A, A, B, C, D,  7, 0x6b901122, 0);
	ROUND_F(D, D, A, B, C, 12, 0xfd987193, 0);
	ROUND_F(C, C, D, A, B, 17, 0xa679438e, M[14]);
	ROUND_F(B, B, C, D, A, 22, 0x49b40821, 0);

	ROUND_G(A, A, B, C, D,  5, 0xf61e2562, M[ 1]);
	ROUND_G(D, D, A, B, C,  9, 0xc040b340, M[ 6]);
	ROUND_G(C, C, D, A, B, 14, 0x265e5a51, 0);
	ROUND_G(B, B, C, D, A, 20, 0xe9b6c7aa, M[ 0]);
	ROUND_G(A, A, B, C, D,  5, 0xd62f105d, M[ 5]);
	ROUND_G(D, D, A, B, C,  9, 0x02441453, 0);
	ROUND_G(C, C, D, A, B, 14, 0xd8a1e681, 0);
	ROUND_G(B, B, C, D, A, 20, 0xe7d3fbc8, M[ 4]);
	ROUND_G(A, A, B, C, D,  5, 0x21e1cde6, 0);
	ROUND_G(D, D, A, B, C,  9, 0xc33707d6, M[14]);
	ROUND_G(C, C, D, A, B, 14, 0xf4d50d87, M[ 3]);
	ROUND_G(B, B, C, D, A, 20, 0x455a14ed, M[ 8]);
	ROUND_G(A, A, B, C, D,  5, 0xa9e3e905, 0);
	ROUND_G(D, D, A, B, C,  9, 0xfcefa3f8, M[ 2]);
	ROUND_G(C, C, D, A, B, 14, 0x676f02d9, M[ 7]);
	ROUND_G(B, B, C, D, A, 20, 0x8d2a4c8a, 0);

	ROUND_H(A, A, B, C, D,  4, 0xfffa3942, M[ 5]);
	ROUND_H(D, D, A, B, C, 11, 0x8771f681, M[ 8]);
	ROUND_H(C, C, D, A, B, 16, 0x6d9d6122, 0);
	ROUND_H(B, B, C, D, A, 23, 0xfde5380c, M[14]);
	ROUND_H(A, A, B, C, D,  4, 0xa4beea44, M[ 1]);
	ROUND_H(D, D, A, B, C, 11, 0x4bdecfa9, M[ 4]);
	ROUND_H(C, C, D, A, B, 16, 0xf6bb4b60, M[ 7]);
	ROUND_H(B, B, C, D, A, 23, 0xbebfbc70, 0);
	ROUND_H(A, A, B, C, D,  4, 0x289b7ec6, 0);
	ROUND_H(D, D, A, B, C, 11, 0xeaa127fa, M[ 0]);
	ROUND_H(C, C, D, A, B, 16, 0xd4ef3085, M[ 3]);
	ROUND_H(B, B, C, D, A, 23, 0x04881d05, M[ 6]);
	ROUND_H(A, A, B, C, D,  4, 0xd9d4d039, 0);
	ROUND_H(D, D, A, B, C, 11, 0xe6db99e5, 0);
	ROUND_H(C, C, D, A, B, 16, 0x1fa27cf8, 0);
	ROUND_H(B, B, C, D, A, 23, 0xc4ac5665, M[ 2]);

	ROUND_I(A, A, B, C, D,  6, 0xf4292244, M[ 0]);
	ROUND_I(D, D, A, B, C, 10, 0x432aff97, M[ 7]);
	ROUND_I(C, C, D, A, B, 15, 0xab9423a7, M[14]);
	ROUND_I(B, B, C, D, A, 21, 0xfc93a039, M[ 5]);
	ROUND_I(A, A, B, C, D,  6, 0x655b59c3, 0);
	ROUND_I(D, D, A, B, C, 10, 0x8f0ccc92, M[ 3]);
	ROUND_I(C, C, D, A, B, 15, 0xffeff47d, 0);
	ROUND_I(B, B, C, D, A, 21, 0x85845dd1, M[ 1]);
	ROUND_I(A, A, B, C, D,  6, 0x6fa87e4f, M[ 8]);
	ROUND_I(D, D, A, B, C, 10, 0xfe2ce6e0, 0);
	ROUND_I(C, C, D, A, B, 15, 0xa3014314, M[ 6]);
	ROUND_I(B, B, C, D, A, 21, 0x4e0811a1, 0);
	ROUND_I(A, A, B, C, D,  6, 0xf7537e82, M[ 4]);
	//ROUND_I(D, D, A, B, C, 10, 0xbd3af235, 0);
	//ROUND_I(C, C, D, A, B, 15, 0x2ad7d2bb, M[ 2]);
	//ROUND_I(B, B, C, D, A, 21, 0xeb86d391, 0);

	return A + 0x67452301;
}

static uint32_t HashMicroChunkMSBOnly(const uint32_t M[16])
{
	uint32_t A = 0x67452301;
	uint32_t B = 0xefcdab89;
	uint32_t C = 0x98badcfe;
	uint32_t D = 0x10325476;

	uint32_t F;

	ROUND_F(A, A, B, C, D,  7, 0xd76aa478, M[ 0]);
	ROUND_F(D, D, A, B, C, 12, 0xe8c7b756, M[ 1]);
	ROUND_F(C, C, D, A, B, 17, 0x242070db, M[ 2]);
	ROUND_F(B, B, C, D, A, 22, 0xc1bdceee, M[ 3]);
	ROUND_F(A, A, B, C, D,  7, 0xf57c0faf, 0);
	ROUND_F(D, D, A, B, C, 12, 0x4787c62a, 0);
	ROUND_F(C, C, D, A, B, 17, 0xa8304613, 0);
	ROUND_F(B, B, C, D, A, 22, 0xfd469501, 0);
	ROUND_F(A, A, B, C, D,  7, 0x698098d8, 0);
	ROUND_F(D, D, A, B, C, 12, 0x8b44f7af, 0);
	ROUND_F(C, C, D, A, B, 17, 0xffff5bb1, 0);
	ROUND_F(B, B, C, D, A, 22, 0x895cd7be, 0);
	ROUND_F(A, A, B, C, D,  7, 0x6b901122, 0);
	ROUND_F(D, D, A, B, C, 12, 0xfd987193, 0);
	ROUND_F(C, C, D, A, B, 17, 0xa679438e, M[14]);
	ROUND_F(B, B, C, D, A, 22, 0x49b40821, 0);

	ROUND_G(A, A, B, C, D,  5, 0xf61e2562, M[ 1]);
	ROUND_G(D, D, A, B, C,  9, 0xc040b340, 0);
	ROUND_G(C, C, D, A, B, 14, 0x265e5a51, 0);
	ROUND_G(B, B, C, D, A, 20, 0xe9b6c7aa, M[ 0]);
	ROUND_G(A, A, B, C, D,  5, 0xd62f105d, 0);
	ROUND_G(D, D, A, B, C,  9, 0x02441453, 0);
	ROUND_G(C, C, D, A, B, 14, 0xd8a1e681, 0);
	ROUND_G(B, B, C, D, A, 20, 0xe7d3fbc8, 0);
	ROUND_G(A, A, B, C, D,  5, 0x21e1cde6, 0);
	ROUND_G(D, D, A, B, C,  9, 0xc33707d6, M[14]);
	ROUND_G(C, C, D, A, B, 14, 0xf4d50d87, M[ 3]);
	ROUND_G(B, B, C, D, A, 20, 0x455a14ed, 0);
	ROUND_G(A, A, B, C, D,  5, 0xa9e3e905, 0);
	ROUND_G(D, D, A, B, C,  9, 0xfcefa3f8, M[ 2]);
	ROUND_G(C, C, D, A, B, 14, 0x676f02d9, 0);
	ROUND_G(B, B, C, D, A, 20, 0x8d2a4c8a, 0);

	ROUND_H(A, A, B, C, D,  4, 0xfffa3942, 0);
	ROUND_H(D, D, A, B, C, 11, 0x8771f681, 0);
	ROUND_H(C, C, D, A, B, 16, 0x6d9d6122, 0);
	ROUND_H(B, B, C, D, A, 23, 0xfde5380c, M[14]);
	ROUND_H(A, A, B, C, D,  4, 0xa4beea44, M[ 1]);
	ROUND_H(D, D, A, B, C, 11, 0x4bdecfa9, 0);
	ROUND_H(C, C, D, A, B, 16, 0xf6bb4b60, 0);
	ROUND_H(B, B, C, D, A, 23, 0xbebfbc70, 0);
	ROUND_H(A, A, B, C, D,  4, 0x289b7ec6, 0);
	ROUND_H(D, D, A, B, C, 11, 0xeaa127fa, M[ 0]);
	ROUND_H(C, C, D, A, B, 16, 0xd4ef3085, M[ 3]);
	ROUND_H(B, B, C, D, A, 23, 0x04881d05, 0);
	ROUND_H(A, A, B, C, D,  4, 0xd9d4d039, 0);
	ROUND_H(D, D, A, B, C, 11, 0xe6db99e5, 0);
	ROUND_H(C, C, D, A, B, 16, 0x1fa27cf8, 0);
	ROUND_H(B, B, C, D, A, 23, 0xc4ac5665, M[ 2]);

	ROUND_I(A, A, B, C, D,  6, 0xf4292244, M[ 0]);
	ROUND_I(D, D, A, B, C, 10, 0x432aff97, 0);
	ROUND_I(C, C, D, A, B, 15, 0xab9423a7, M[14]);
	ROUND_I(B, B, C, D, A, 21, 0xfc93a039, 0);
	ROUND_I(A, A, B, C, D,  6, 0x655b59c3, 0);
	ROUND_I(D, D, A, B, C, 10, 0x8f0ccc92, M[ 3]);
	ROUND_I(C, C, D, A, B, 15, 0xffeff47d, 0);
	ROUND_I(B, B, C, D, A, 21, 0x85845dd1, M[ 1]);
	ROUND_I(A, A, B, C, D,  6, 0x6fa87e4f, 0);
	ROUND_I(D, D, A, B, C, 10, 0xfe2ce6e0, 0);
	ROUND_I(C, C, D, A, B, 15, 0xa3014314, 0);
	ROUND_I(B, B, C, D, A, 21, 0x4e0811a1, 0);
	ROUND_I(A, A, B, C, D,  6, 0xf7537e82, 0);
	//ROUND_I(D, D, A, B, C, 10, 0xbd3af235, 0);
	//ROUND_I(C, C, D, A, B, 15, 0x2ad7d2bb, M[ 2]);
	//ROUND_I(B, B, C, D, A, 21, 0xeb86d391, 0);

	return A + 0x67452301;
}

void uMD5::Hash(const uint8_t* buffer, size_t bufferSize, uint8_t hash[16])
{
	size_t numChunks = bufferSize >> 6;

	uint32_t* out = reinterpret_cast<uint32_t*>(hash);
	out[0] = 0x67452301;
	out[1] = 0xefcdab89;
	out[2] = 0x98badcfe;
	out[3] = 0x10325476;

	const uint32_t* message = reinterpret_cast<const uint32_t*>(buffer);
	for (size_t i = 0; i < numChunks; i++)
	{
		HashChunk(message, out);
		message += 16;
	}

	Chunk padding;
	size_t lastChunkSize = bufferSize & 0x3f;
	memcpy(padding.bytes, message, lastChunkSize);
	memset(&padding.bytes[lastChunkSize], 0, 64 - lastChunkSize);
	padding.bytes[lastChunkSize] = 0x80;

	if (lastChunkSize > 55)
	{
		HashChunk((&padding.dwords[0]), out);
		memset(&padding.bytes[0], 0, 64);
	}

	assert(bufferSize < (std::numeric_limits<uint32_t>::max() >> 3));
	padding.dwords[14] = static_cast<uint32_t>(bufferSize << 3);
	HashChunk(&padding.dwords[0], out);
}

void uMD5::Hash(const std::string_view& sv, uint8_t hash[16])
{
	return Hash((uint8_t*)sv.data(), sv.size(), hash);
}

void uMD5::Hash(const std::string& str, uint8_t hash[16])
{
	return Hash((uint8_t*)str.data(), str.size(), hash);
}

void uMD5::HashChunkInPlace(Chunk* chunk, size_t chunkSize, uint8_t hash[16])
{
	assert(chunkSize < 56);

	uint32_t* out = reinterpret_cast<uint32_t*>(hash);
	out[0] = 0x67452301;
	out[1] = 0xefcdab89;
	out[2] = 0x98badcfe;
	out[3] = 0x10325476;

	memset(&chunk->bytes[chunkSize], 0, 64 - chunkSize);
	chunk->bytes[chunkSize] = 0x80;

	chunk->dwords[14] = static_cast<uint32_t>(chunkSize << 3);
	HashChunk(&chunk->dwords[0], out);
}

void uMD5::HashMiniChunkInPlace(Chunk* chunk, size_t chunkSize, uint8_t hash[16])
{
	assert(chunkSize <= 32);

	uint32_t* out = reinterpret_cast<uint32_t*>(hash);
	out[0] = 0x67452301;
	out[1] = 0xefcdab89;
	out[2] = 0x98badcfe;
	out[3] = 0x10325476;

	//memset(&chunk->bytes[chunkSize], 0, 64 - chunkSize);
	chunk->bytes[chunkSize] = 0x80;

	chunk->dwords[14] = static_cast<uint32_t>(chunkSize << 3);
	HashMiniChunk(&chunk->dwords[0], out);
}

uint32_t uMD5::HashMiniChunkInPlaceMSBOnly(Chunk* chunk, size_t chunkSize)
{
	assert(chunkSize <= 32);

	//memset(&chunk->bytes[chunkSize], 0, 64 - chunkSize);
	chunk->bytes[chunkSize] = 0x80;

	chunk->dwords[14] = static_cast<uint32_t>(chunkSize << 3);
	return HashMiniChunkMSBOnly(&chunk->dwords[0]);
}

uint32_t uMD5::HashMicroChunkInPlaceMSBOnly(Chunk* chunk, size_t chunkSize)
{
	assert(chunkSize <= 16);

	//memset(&chunk->bytes[chunkSize], 0, 64 - chunkSize);
	chunk->bytes[chunkSize] = 0x80;

	chunk->dwords[14] = static_cast<uint32_t>(chunkSize << 3);
	return HashMicroChunkMSBOnly(&chunk->dwords[0]);
}

void uMD5::Fuzz()
{
#if 0
	for (int i = 0; i < 10000; i++)
	{
		uint8_t oldHash[16];
		uint8_t newHash[16];

		uint32_t* out = reinterpret_cast<uint32_t*>(oldHash);
		out[0] = 0x67452301;
		out[1] = 0xefcdab89;
		out[2] = 0x98badcfe;
		out[3] = 0x10325476;
		memcpy(newHash, oldHash, sizeof(newHash));

		Chunk chunk;
		for (int b = 0; b < sizeof(chunk.bytes); b++)
		{
			chunk.bytes[b] = rand() & 0xff;
		}

		HashChunk(&chunk.dwords[0], reinterpret_cast<uint32_t*>(oldHash));
		HashChunk_x64_reorg(&chunk.dwords[0], reinterpret_cast<uint32_t*>(newHash));

		assert(memcmp(oldHash, newHash, sizeof(oldHash)) == 0);
	}

	for (int i = 0; i < 10000; i++)
	{
		uint8_t oldHash[16];
		uint8_t newHash[16];

		uint32_t* out = reinterpret_cast<uint32_t*>(oldHash);
		out[0] = 0x67452301;
		out[1] = 0xefcdab89;
		out[2] = 0x98badcfe;
		out[3] = 0x10325476;
		memcpy(newHash, oldHash, sizeof(newHash));

		Chunk chunk = {};
		for (int b = 0; b < 32; b++)
		{
			chunk.bytes[b] = rand() & 0xff;
		}

		HashChunk_x64(&chunk.dwords[0], reinterpret_cast<uint32_t*>(oldHash));
		HashMiniChunk_reorg(&chunk.dwords[0], reinterpret_cast<uint32_t*>(newHash));

		assert(memcmp(oldHash, newHash, sizeof(oldHash)) == 0);
		HashMiniChunk(&chunk.dwords[0], reinterpret_cast<uint32_t*>(newHash));
	}
#endif
}
