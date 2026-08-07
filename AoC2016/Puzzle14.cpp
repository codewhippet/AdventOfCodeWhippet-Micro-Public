#include "stdafx.h"

using namespace std;

namespace Puzzle14_2016_Types
{
}

using namespace Puzzle14_2016_Types;

static bool IsNInARow(unsigned char hash[16], const int32_t target, char *rep)
{
	int32_t c = 0xff;
	int32_t count = 0;
	for (int32_t i = 0; i < 16; i++)
	{
		int32_t h = (hash[i] >> 4) & 0xf;
		count = (h == c ? count + 1 : 1);
		if (count == target)
		{
			*rep = (char)h;
			return true;
		}
		c = h;

		h = hash[i] & 0xf;
		count = (h == c ? count + 1 : 1);
		if (count == target)
		{
			*rep = (char)h;
			return true;
		}
		c = h;
	}
	return false;
}

static int32_t FindIndexOfNthKey(const char* salt, const size_t keysNeeded, const function<void(const char*, size_t, unsigned char[16])>& hasher)
{
	char secretBuffer[64];
	strcpy(secretBuffer, salt);
	size_t secretLength = strlen(secretBuffer);
	char* digits = &secretBuffer[secretLength];

	map<int32_t, char> threeInARows;

	vector<int32_t> keysFoundAt;
	keysFoundAt.reserve(96);

	int32_t index = 0;
	int32_t stopIndex = -1;
	while ((stopIndex == -1) || (index < stopIndex))
	{
		size_t digitsLength = sprint_digits(digits, index);

		unsigned char hash[16];
		hasher(secretBuffer, secretLength + digitsLength, hash);

		{
			char rep = 0;
			if (IsNInARow(hash, 5, &rep))
			{
				auto threeInARowsBegin = threeInARows.lower_bound(index - 1000);
				auto threeInARowsEnd = threeInARows.upper_bound(index);
				for (auto it = threeInARowsBegin; it != threeInARowsEnd; ++it)
				{
					if (it->second == rep)
					{
						keysFoundAt.push_back(it->first);
						if ((keysFoundAt.size() >= keysNeeded) && (stopIndex == -1))
						{
							// Keep searching to make sure we've evaluated all potential triples prior to this point
							stopIndex = index + 1000;
						}
					}
				}
			}
		}

		{
			char rep = 0;
			if (IsNInARow(hash, 3, &rep))
			{
				threeInARows[index] = rep;
			}
		}

		{
			auto ignoreBefore = threeInARows.upper_bound(index - 1000);
			threeInARows.erase(threeInARows.begin(), ignoreBefore);
		}

		index++;
	}

	ranges::sort(keysFoundAt);
	auto dummy = unique(keysFoundAt.begin(), keysFoundAt.end());
	(void)dummy;

	return keysFoundAt[keysNeeded - 1];
}

void Puzzle14_A_2016()
{
	const size_t keysNeeded = 64;

	char salt[16];
	Parse::ReadNonEmptyLine(salt);

	int32_t answer = FindIndexOfNthKey(salt, keysNeeded,
		[&](const char* buffer, size_t secretLength, unsigned char hash[16])
		{
			uMD5::Hash((const uint8_t*)buffer, secretLength, hash);
		});

	return PuzzleOutput::Submit(2016, 14, 1, answer);
}

void Puzzle14_B_2016()
{
	const size_t keysNeeded = 64;

	char salt[16];
	Parse::ReadNonEmptyLine(salt);

	const char hex[] = "0123456789abcdef";

	int32_t answer = FindIndexOfNthKey(salt, keysNeeded,
		[&](const char* buffer, size_t secretLength, unsigned char hash[16])
		{
			uMD5::Hash((const uint8_t*)buffer, secretLength, hash);

			uMD5::Chunk scratchBuffer = {};
			for (size_t hashCount = 0; hashCount < 2016; hashCount++)
			{
				for (size_t i = 0; i < 16; i++)
				{
					uint8_t c = hash[i];
					scratchBuffer.chars[(i * 2) + 0] = hex[c >> 4];
					scratchBuffer.chars[(i * 2) + 1] = hex[c & 0xf];
				}

				uMD5::HashMiniChunkInPlace(&scratchBuffer, 16 * 2, hash);
			}
		});

	return PuzzleOutput::Submit(2016, 14, 2, answer);
}
