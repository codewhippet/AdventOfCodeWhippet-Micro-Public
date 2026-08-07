#include "stdafx.h"

using namespace std;

namespace Puzzle11_2018_Types
{
}

using namespace Puzzle11_2018_Types;

static int32_t PowerLevelAt(const Vec2Int& p, int32_t serialNumber)
{
	int32_t rackId = p.X + 10;
	int32_t powerLevel = rackId * p.Y;
	powerLevel += serialNumber;
	powerLevel *= rackId;

	powerLevel = (powerLevel / 100) % 10;
	powerLevel -= 5;

	return powerLevel;
}

static vector<vector<int32_t>> GenerateGridSum(const int32_t gridSize, const Vec2Int& offset, int32_t serialNumber)
{
	vector<vector<int32_t>> gridSum(gridSize + 1);
	ranges::fill(gridSum, (vector<int32_t>(gridSize + 1)));

	for (const Vec2Int& p : uGridRange<Vec2Int>{ Vec2Int{ 1, 1 }, Vec2Int{ gridSize, gridSize } + Vec2Int{ 1, 1 } })
	{
		Vec2Int up = p + Vec2Int::Up();
		Vec2Int left = p + Vec2Int::Left();

		int32_t sum = gridSum[up.Y][up.X] + gridSum[left.Y][left.X] + PowerLevelAt(p + offset, serialNumber) - gridSum[up.Y][left.X];

		gridSum[p.Y][p.X] = sum;
	}

	return gridSum;
}

void Puzzle11_A_2018()
{
	// Constants picked carefully to ensure we get an exact 300x300 coverage
	const int32_t chunkSize = 102;
	const int32_t chunkOverlap = 3;
	const int32_t chunkDiff = chunkSize - chunkOverlap;
	const int32_t chunkCount = 3;
	assert(((chunkSize * chunkCount) - ((chunkCount - 1) * chunkOverlap)) == 300);

	const int32_t serialNumber = Parse::GetInt32();

	int32_t totalPower = numeric_limits<int32_t>::min();
	Vec2Int coordinate;

	for (int32_t y = 0; y < chunkCount; y++)
	{
		for (int32_t x = 0; x < chunkCount; x++)
		{
			Vec2Int offset = Vec2Int{ x, y } * chunkDiff;

			vector<vector<int32_t>> gridSum = GenerateGridSum(chunkSize, offset, serialNumber);
			for (const Vec2Int& p : uGridRange<Vec2Int>{ Vec2Int{ 3, 3 }, Vec2Int{ chunkSize, chunkSize } + Vec2Int{ 1, 1 } })
			{
				Vec2Int up = p + Vec2Int::Up() * 3;
				Vec2Int left = p + Vec2Int::Left() * 3;

				int32_t power = gridSum[p.Y][p.X] - gridSum[up.Y][up.X] - gridSum[left.Y][left.X] + gridSum[up.Y][left.X];
				if (power > totalPower)
				{
					totalPower = power;
					coordinate = p - Vec2Int{ 2, 2 } + offset;
				}
			}
		}
	}

	char answer[64];
	sprintf(answer, "%d,%d", coordinate.X, coordinate.Y);

	PuzzleOutput::Submit(2018, 11, 1, answer);
}

void Puzzle11_B_2018()
{
	// Constants picked carefully to ensure we get an exact 300x300 coverage
	// We make the assumption that the answer is chunkOverlap x chunkOverlap or smaller
	const int32_t chunkSize = 114;
	const int32_t chunkOverlap = 21;
	const int32_t chunkDiff = chunkSize - chunkOverlap;
	const int32_t chunkCount = 3;
	assert(((chunkSize * chunkCount) - ((chunkCount - 1) * chunkOverlap)) == 300);

	const int32_t serialNumber = Parse::GetInt32();

	int32_t totalPower = numeric_limits<int32_t>::min();
	pair<Vec2Int, int32_t> coordinateAndSize;

	for (int32_t y = 0; y < chunkCount; y++)
	{
		for (int32_t x = 0; x < chunkCount; x++)
		{
			Vec2Int offset = Vec2Int{ x, y } * chunkDiff;

			vector<vector<int32_t>> gridSum = GenerateGridSum(chunkSize, offset, serialNumber);

			for (const Vec2Int& p : uGridRange<Vec2Int>{ Vec2Int{ 1, 1 }, Vec2Int{ chunkSize, chunkSize } + Vec2Int{ 1, 1 } })
			{
				int32_t largestSquarePossible = min(min(p.X, p.Y), chunkOverlap);
				for (int32_t size = 1; size <= largestSquarePossible; size++)
				{
					Vec2Int up = p + Vec2Int::Up() * size;
					Vec2Int left = p + Vec2Int::Left() * size;

					int32_t power = gridSum[p.Y][p.X] - gridSum[up.Y][up.X] - gridSum[left.Y][left.X] + gridSum[up.Y][left.X];
					if (power > totalPower)
					{
						totalPower = power;
						coordinateAndSize = { Vec2Int{ left.X, up.Y } + Vec2Int{ 1, 1 } + offset, size };
					}
				}
			}
		}
	}

	char answer[64];
	sprintf(answer, "%d,%d,%d", coordinateAndSize.first.X, coordinateAndSize.first.Y, coordinateAndSize.second);

	PuzzleOutput::Submit(2018, 11, 2, answer);
}
