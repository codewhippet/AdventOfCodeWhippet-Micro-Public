#include "stdafx.h"

using namespace std;

namespace Puzzle17_2017_Types
{
}

using namespace Puzzle17_2017_Types;

void Puzzle17_A_2017()
{
	const uint32_t stepAmount = Parse::GetUint32();
	vector<size_t> insertedAt;
	insertedAt.reserve(2017 + 1);

	size_t currentPosition = 0;
	for (int64_t i = 0; i <= 2017; i++)
	{
		insertedAt.push_back(currentPosition);
		currentPosition = (currentPosition + stepAmount + 1) % (i + 1);
	}

	int32_t answer = 0;

	size_t targetInsertedAt = insertedAt.back();
	for (size_t i = insertedAt.size() - 2; i > 0; i--)
	{
		if (insertedAt[i] == targetInsertedAt)
		{
			answer = static_cast<int32_t>(i);
			break;
		}

		if (insertedAt[i] < targetInsertedAt)
		{
			targetInsertedAt--;
		}
	}

	PuzzleOutput::Submit(2017, 17, 1, answer);
}

void Puzzle17_B_2017()
{
	const uint32_t stepAmount = Parse::GetUint32();

	int32_t numberAfterZero = -1;

	int32_t bufferSize = 1;
	int32_t currentPosition = 0;
	while (bufferSize <= 50000000)
	{
		if (currentPosition == 0)
		{
			numberAfterZero = bufferSize;
		}

		int32_t diffToEnd = bufferSize - currentPosition;

		int32_t increments = (diffToEnd + stepAmount - 1) / stepAmount;
		bufferSize += increments;

		int32_t nextPosition = currentPosition + increments * (stepAmount + 1);
		currentPosition = nextPosition % bufferSize;
	}

	int32_t answer = numberAfterZero;

	PuzzleOutput::Submit(2017, 17, 2, answer);
}
