#include "stdafx.h"

using namespace std;

namespace Puzzle20_2015_Types
{
}

using namespace Puzzle20_2015_Types;

static int32_t WhippetApproximation(int32_t target, int32_t presentsPerElf)
{
	int32_t unitTarget = target / presentsPerElf;

	int32_t upperBound = unitTarget;
	int32_t lowerBound = 1;
	int32_t midPoint = 0;
	while (upperBound > lowerBound)
	{
		midPoint = (lowerBound + upperBound) / 2;
		int32_t sigma = static_cast<int32_t>(1.781062 * midPoint * log(log(midPoint)));
		if (sigma == unitTarget)
		{
			break;
		}
		else if (sigma > unitTarget)
		{
			upperBound = midPoint - 1;
		}
		else
		{
			lowerBound = midPoint + 1;
		}
	}

	return midPoint;
}

static int32_t HousePartA(int32_t startAt, int32_t presentsPerElf, int32_t target, vector<int32_t>* housesBuffer)
{
	vector<int32_t>& houses = *housesBuffer;
	ranges::fill(houses, 0);

	int32_t chunkSize = static_cast<int32_t>(houses.size());
	int32_t lastElfForChunk = startAt + chunkSize;
	for (int32_t elf = 1; elf < lastElfForChunk; elf++)
	{
		int32_t elfNextDeliveryNumber = ((startAt + elf - 1) / elf);
		int32_t startingElfOffset = (elfNextDeliveryNumber * elf) - startAt;
		for (int32_t chunkIndex = startingElfOffset; chunkIndex < chunkSize; chunkIndex += elf)
		{
			houses[chunkIndex] += elf * presentsPerElf;
		}
	}

	auto firstHouse = find_if(houses.begin(), houses.end(),
		[&](int32_t h)
		{
			return h >= target;
		});
	return (firstHouse == houses.end() ? -1 : static_cast<int32_t>(distance(houses.begin(), firstHouse)) + startAt);
}

static int32_t HousePartB(int32_t startAt, int32_t presentsPerElf, int32_t deliveriesPerElf, int32_t target, vector<int32_t>* housesBuffer)
{
	vector<int32_t>& houses = *housesBuffer;
	ranges::fill(houses, 0);

	int32_t startingElf = startAt / deliveriesPerElf; //**TODO: tighten this boun

	int32_t chunkSize = static_cast<int32_t>(houses.size());
	int32_t lastElfForChunk = startAt + chunkSize;
	for (int32_t elf = startingElf; elf < lastElfForChunk; elf++)
	{
		int32_t elfNextDeliveryNumber = ((startAt + elf - 1) / elf);
		if (elfNextDeliveryNumber > deliveriesPerElf)
			continue;

		int32_t startingElfOffset = (elfNextDeliveryNumber * elf) - startAt;
		for (int32_t chunkIndex = startingElfOffset; chunkIndex < chunkSize; chunkIndex += elf)
		{
			houses[chunkIndex] += elf * presentsPerElf;
		}
	}

	auto firstHouse = find_if(houses.begin(), houses.end(),
		[&](int32_t h)
		{
			return h >= target;
		});
	return (firstHouse == houses.end() ? -1 : static_cast<int32_t>(distance(houses.begin(), firstHouse)) + startAt);
}

void Puzzle20_A_2015()
{
	const int32_t presentsPerElf = 10;
	const int32_t target = Parse::GetInt32();
	const int32_t chunkSize = 20000;

	vector<int32_t> houseBuffer(chunkSize);
	int32_t answer = 0;

	for (int32_t house = WhippetApproximation(target, presentsPerElf); house <= (target / presentsPerElf); house += chunkSize)
	{
		int32_t houseInChunk = HousePartA(house, presentsPerElf, target, &houseBuffer);
		if (houseInChunk != -1)
		{
			answer = houseInChunk;
			break;
		}
	}

	return PuzzleOutput::Submit(2015, 20, 1, answer);
}

void Puzzle20_B_2015()
{
	const int32_t presentsPerElf = 11;
	const int32_t maximumDeliveriesPerElf = 50;
	const int32_t target = Parse::GetInt32();
	const int32_t chunkSize = 20000;

	vector<int32_t> houseBuffer(chunkSize);
	int32_t answer = 0;

	for (int32_t house = WhippetApproximation(target, presentsPerElf); house <= (target / presentsPerElf); house += chunkSize)
	{
		int32_t houseInChunk = HousePartB(house, presentsPerElf, maximumDeliveriesPerElf, target, &houseBuffer);
		if (houseInChunk != -1)
		{
			answer = houseInChunk;
			break;
		}
	}

	return PuzzleOutput::Submit(2015, 20, 2, answer);
}
