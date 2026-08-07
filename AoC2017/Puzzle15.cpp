#include "stdafx.h"

using namespace std;

namespace Puzzle15_2017_Types
{
	// https://en.wikipedia.org/wiki/Lehmer_random_number_generator

	template <uint32_t FACTOR>
	struct GeneratorOne
	{
		uint32_t Current;
		uint32_t Next()
		{
			uint32_t lo = (Current & 0x7fff) * FACTOR;
			uint32_t hi = (Current >> 15) * FACTOR;
			Current = lo + ((hi & 0xffff) << 15) + (hi >> 16);
			Current = (Current & 0x7fffffff) + (Current >> 31);
			return Current;
		}
	};

	template <uint32_t FACTOR, uint32_t MULTIPLES_OF_MASK>
	struct GeneratorTwo
	{
		uint32_t Current;
		uint32_t Next()
		{
			do
			{
				uint32_t lo = (Current & 0x7fff) * FACTOR;
				uint32_t hi = (Current >> 15) * FACTOR;
				Current = lo + ((hi & 0xffff) << 15) + (hi >> 16);
				Current = (Current & 0x7fffffff) + (Current >> 31);

			} while ((Current & MULTIPLES_OF_MASK) != 0);

			return Current;
		}
	};
}

using namespace Puzzle15_2017_Types;

void Puzzle15_A_2017()
{
	const size_t rounds = 40000000;

	GeneratorOne<16807> a{ Parse::GetUint32() };
	GeneratorOne<48271> b{ Parse::GetUint32() };

	int32_t answer = 0;
	for (size_t i = 0; i < rounds; i++)
	{
		uint32_t nextA = a.Next();
		uint32_t nextB = b.Next();
		if ((nextA & 0xffff) == (nextB & 0xffff))
		{
			answer++;
		}
	}

	PuzzleOutput::Submit(2017, 15, 1, answer);
}

void Puzzle15_B_2017()
{
	const size_t rounds = 5000000;

	GeneratorTwo<16807, 4 - 1> a{ Parse::GetUint32() };
	GeneratorTwo<48271, 8 - 1> b{ Parse::GetUint32() };

	int32_t answer = 0;
	for (size_t i = 0; i < rounds; i++)
	{
		uint32_t nextA = a.Next();
		uint32_t nextB = b.Next();
		if ((nextA & 0xffff) == (nextB & 0xffff))
		{
			answer++;
		}
	}

	PuzzleOutput::Submit(2017, 15, 2, answer);
}
