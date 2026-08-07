#include "stdafx.h"

using namespace std;

namespace Puzzle25_2015_Types
{
}

using namespace Puzzle25_2015_Types;

static int32_t GridIndexFromRowColumn(int32_t row, int32_t column)
{
	// Get the 'starting' position [r0,1] of the diagonal containing [row,column]
	int32_t r0 = row + (column - 1);

	// The starting grid index for each row [r0,1] is TriangleNumber(r0-1)
	int32_t triangleNumber = ((r0 - 1) * (r0)) / 2;
	int32_t startingGridIndex = triangleNumber;

	// Then add back on the row count we took away
	int32_t gridIndex = startingGridIndex + (r0 - row);
	return gridIndex;
}

static int64_t ModPow(int64_t base, int64_t exponent, int64_t modulus)
{
	int64_t result = 1;
	while (exponent)
	{
		if (exponent & 1)
		{
			result = (result * base) % modulus;
		}
		exponent >>= 1;
		base = (base * base) % modulus;
	}
	return result;
}

void Puzzle25_A_2015()
{
	char instructions[128];
	Parse::ReadNonEmptyLine(instructions, sizeof(instructions));

	int32_t row = 0;
	int32_t column = 0;
	int scanned = sscanf(instructions, "To continue, please consult the code grid in the manual.  Enter the code at row %d, column %d.",
		&row,
		&column);
	assert(scanned == 2);
	(void)scanned;

	int32_t rngIterations = GridIndexFromRowColumn(row, column);
	int64_t answer = (20151125 * ModPow(252533, rngIterations, 33554393)) % 33554393;

	return PuzzleOutput::Submit(2015, 25, 1, answer);
}

void Puzzle25_B_2015()
{
	return PuzzleOutput::Submit(2015, 25, 2, int32_t(-1));
}
