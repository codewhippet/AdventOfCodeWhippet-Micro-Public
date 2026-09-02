#include "stdafx.h"
#include <intrin.h>

using namespace std;

namespace Puzzle22_2019_Types
{
	struct ModularEquation
	{
		// (A.x + B) % M
		int64_t A;
		int64_t B;
		int64_t M;

		int64_t Evaluate(int64_t x) const;
	};
}

using namespace Puzzle22_2019_Types;

static int64_t MultiplicativeInverse(int64_t deckSize, int64_t increment)
{
	// https://extendedeuclideanalgorithm.com/multiplicative_inverse.php
	int64_t a, b, q, r, t1, t2, t3;

	b = deckSize;
	r = increment;
	t2 = 0;
	t3 = 1;
	while (true)
	{
		a = b;
		b = r;
		if (b == 0)
		{
			assert(false); // deckSize and increment aren't relatively prime
			return -1;
		}

		q = a / b;
		r = a % b;
		t1 = t2;
		t2 = t3;
		t3 = t1 - q * t2;

		if (b == 1)
		{
			int64_t inverse = abs(t2) % deckSize;
			assert(inverse != 0);
			return (t2 > 0 ? inverse : deckSize - inverse);
		}
	}
}

static int64_t MulMod(int64_t a, int64_t b, int64_t mod)
{
	int64_t result = 0;
	while (a)
	{
		if (a & 1)
		{
			result = (result + b) % mod;
		}
		a >>= 1;
		b = (b << 1) % mod;
	}
	return result;
}

int64_t Puzzle22_2019_Types::ModularEquation::Evaluate(int64_t x) const
{
	return (MulMod(A, x, M) + B) % M;
}

static ModularEquation Compose(const ModularEquation& f, const ModularEquation& g)
{
	// (A.(c.x + d) + B) % M
	// (A.c.x + A.d + B) % M
	// A' = A.c
	// B' = A.d + B
	// (A'.x + B') % M

	assert(f.M == g.M);

	ModularEquation ret = f;

	int64_t ac = MulMod(f.A, g.A, f.M);
	int64_t ad = MulMod(f.A, g.B, f.M);
	ret.A = f.M + ac;
	ret.B = f.M + ad + f.B;

	assert(ret.A >= 0);
	assert(ret.B >= 0);

	ret.A = ret.A % ret.M;
	ret.B = ret.B % ret.M;

	return ret;
}

static ModularEquation ComposeShuffleSteps(int64_t deckSize)
{
	ModularEquation ret{ 1, 0, deckSize };

	char lineBuffer[32];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(lineBuffer);
		string_view line{ lineBuffer };

		if (line.starts_with("deal into"sv))
		{
			ret = Compose(ret, { -1, -1, deckSize });
		}
		else if (line.starts_with("cut"sv))
		{
			int32_t param = Parse::GetInt32(line.data());
			int64_t offset = param >= 0 ? param : deckSize + param;
			ret = Compose(ret, { 1, offset, deckSize });
		}
		else
		{
			assert(line.starts_with("deal with"sv));
			int32_t param = Parse::GetInt32(line.data());
			ret = Compose(ret, { MultiplicativeInverse(deckSize, param), 0, deckSize });
		}
	}

	return ret;
}

static int64_t RepeatEquation(int64_t x, ModularEquation f, int64_t repeatCount)
{
	while (repeatCount)
	{
		if (repeatCount & 1)
		{
			x = f.Evaluate(x);
		}
		repeatCount >>= 1;
		f = Compose(f, f);
	}
	return x;
}

void Puzzle22_A_2019()
{
	const int64_t deckSize = 10007;
	ModularEquation meq = ComposeShuffleSteps(deckSize);

	// (A.x + B = 2019) % M
	// (A.x = 2019 - B) % M
	// x = (A^-1 * (2019 - B)) % M
	int64_t inv = MultiplicativeInverse(meq.M, meq.A);
	int64_t answer = (inv * (meq.M + 2019 - meq.B)) % meq.M;

	PuzzleOutput::Submit(2019, 22, 1, answer);
}

void Puzzle22_B_2019()
{
	const int64_t deckSize = 119315717514047ll;
	ModularEquation meq = ComposeShuffleSteps(deckSize);

	const int64_t numShuffles = 101741582076661ll;
	int64_t answer = RepeatEquation(2020, meq, numShuffles);

	PuzzleOutput::Submit(2019, 22, 2, answer);
}
