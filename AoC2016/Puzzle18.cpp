#include "stdafx.h"

using namespace std;

namespace Puzzle18_2016_Types
{
	using Chunk = uint32_t;
	using Row = array<Chunk, 6>;
}

using namespace Puzzle18_2016_Types;

static int32_t ReadRowAndMask(Row* row, Row* mask)
{
	memset(row->data(), 0, row->size() * sizeof(Chunk));
	memset(mask->data(), 0, mask->size() * sizeof(Chunk));

	char buffer[128];
	Parse::ReadNonEmptyLine(buffer);
	const string_view line{ buffer };

	const size_t chunkBits = sizeof(Chunk) * CHAR_BIT;

	size_t destChunk = 1;
	size_t destBit = chunkBits;
	for (char c : line)
	{
		Chunk bit = (c == '^' ? 1 : 0);
		(*row)[destChunk] |= bit << (destBit - 1);
		(*mask)[destChunk] |= 1 << (destBit - 1);

		if (--destBit == 0)
		{
			destChunk++;
			destBit = chunkBits;
		}
	}

	return static_cast<int32_t>(line.size());
}

static void NextRow(const Row& prev, const Row& mask, Row* next)
{
	for (size_t i = 1; (i + 1) < prev.size(); i++)
	{
		Chunk leftTraps = (prev[i - 1] << (sizeof(Chunk) * CHAR_BIT - 1)) | (prev[i] >> 1);
		Chunk rightTraps = (prev[i + 1] >> (sizeof(prev[0]) * CHAR_BIT - 1)) | (prev[i] << 1);
		(*next)[i] = leftTraps ^ rightTraps;
	}

	for (size_t i = 0; i < mask.size(); i++)
	{
		(*next)[i] &= mask[i];
	}
}

static int32_t CountTraps(const Row& row)
{
	return accumulate(row.begin(), row.end(), 0, [](int32_t sum, Chunk element) { return sum + popcount(element); });
}

void Puzzle18_A_2016()
{
	const size_t rowsNeeded = 40;

	array<Row, 2> buffers;
	Row mask;

	int32_t lineLength = ReadRowAndMask(&buffers[0], &mask);
	assert(((mask.size() - 2) * (sizeof(Chunk) * CHAR_BIT)) >= lineLength);

	int32_t answer = 0;
	for (size_t i = 0; i < rowsNeeded; i++)
	{
		answer += lineLength - CountTraps(buffers[i & 1]);
		NextRow(buffers[i & 1], mask, &buffers[(i + 1) & 1]);
	}

	return PuzzleOutput::Submit(2016, 18, 1, answer);
}

void Puzzle18_B_2016()
{
	const size_t rowsNeeded = 400000;

	array<Row, 2> buffers;
	Row mask;

	int32_t lineLength = ReadRowAndMask(&buffers[0], &mask);

	int32_t answer = 0;
	for (size_t i = 0; i < rowsNeeded; i++)
	{
		answer += lineLength - CountTraps(buffers[i & 1]);
		NextRow(buffers[i & 1], mask, &buffers[(i + 1) & 1]);
	}

	return PuzzleOutput::Submit(2016, 18, 1, answer);
}
