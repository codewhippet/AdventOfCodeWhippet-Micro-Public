#include "stdafx.h"

using namespace std;

namespace Puzzle16_2015_Types
{
	enum : size_t
	{
		NUM_ITEMS = 10,

		AKITAS = 0,
		CARS = 1,
		CATS = 2,
		CHILDREN = 3,
		GOLDFISH = 4,
		PERFUMES = 5,
		POMERANIANS = 6,
		SAMOYEDS = 7,
		TREES = 8,
		VIZSLAS = 9,
	};

	struct Sue
	{
		int32_t Id;
		array<int32_t, NUM_ITEMS> Owns;
	};
}

using namespace Puzzle16_2015_Types;

static size_t TypeIndex(const char* type)
{
	int32_t prefix = *reinterpret_cast<const int32_t*>(type);
	switch (prefix)
	{
	case 'tika':
		return AKITAS;
	case 'srac':
		return CARS;
	case 'stac':
		return CATS;
	case 'lihc':
		return CHILDREN;
	case 'dlog':
		return GOLDFISH;
	case 'frep':
		return PERFUMES;
	case 'emop':
		return POMERANIANS;
	case 'omas':
		return SAMOYEDS;
	case 'eert':
		return TREES;
	case 'sziv':
		return VIZSLAS;
	}
	return ~size_t{ 0 };
}

void Puzzle16_A_2015()
{
	array<int32_t, NUM_ITEMS> detectedValues;
	detectedValues[CHILDREN] = 3;
	detectedValues[CATS] = 7;
	detectedValues[SAMOYEDS] = 2;
	detectedValues[POMERANIANS] = 3;
	detectedValues[AKITAS] = 0;
	detectedValues[VIZSLAS] = 0;
	detectedValues[GOLDFISH] = 5;
	detectedValues[TREES] = 3;
	detectedValues[CARS] = 2;
	detectedValues[PERFUMES] = 1;

	int32_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		char line[128];
		int32_t lineLength = Parse::ReadLine(line, sizeof(line));
		assert(lineLength > 0);
		(void)lineLength;

		int sueIndex = 0;
		char aType[16] = { 0 };
		int aCount = 0;
		char bType[16] = { 0 };
		int bCount = 0;
		char cType[16] = { 0 };
		int cCount = 0;
		int scanned = sscanf(line, "Sue %d: %[a-z]: %d, %[a-z]: %d, %[a-z]: %d",
			&sueIndex,
			aType,
			&aCount,
			bType,
			&bCount,
			cType,
			&cCount);
		assert(scanned == 7);
		(void)scanned;

		Sue sue{ sueIndex, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } };
		sue.Owns[TypeIndex(aType)] = aCount;
		sue.Owns[TypeIndex(bType)] = bCount;
		sue.Owns[TypeIndex(cType)] = cCount;

		bool isCorrectSue = true;
		for (size_t i = 0; i < NUM_ITEMS; i++)
		{
			if ((sue.Owns[i] != -1) &&
				(sue.Owns[i] != detectedValues[i]))
			{
				isCorrectSue = false;
				break;
			}
		}

		if (isCorrectSue)
		{
			answer = sueIndex;
			break;
		}
	}

	return PuzzleOutput::Submit(2015, 16, 1, answer);
}

void Puzzle16_B_2015()
{
	array<int32_t, NUM_ITEMS> needExact{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
	needExact[CHILDREN] = 3;
	needExact[SAMOYEDS] = 2;
	needExact[AKITAS] = 0;
	needExact[VIZSLAS] = 0;
	needExact[CARS] = 2;
	needExact[PERFUMES] = 1;

	array<int32_t, NUM_ITEMS> needGreaterThan{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
	needGreaterThan[CATS] = 7;
	needGreaterThan[TREES] = 3;

	array<int32_t, NUM_ITEMS> needLessThan{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
	needLessThan[POMERANIANS] = 3;
	needLessThan[GOLDFISH] = 5;

	int32_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		char line[128];
		int32_t lineLength = Parse::ReadLine(line, sizeof(line));
		assert(lineLength > 0);
		(void)lineLength;

		int sueIndex = 0;
		char aType[16] = { 0 };
		int aCount = 0;
		char bType[16] = { 0 };
		int bCount = 0;
		char cType[16] = { 0 };
		int cCount = 0;
		int scanned = sscanf(line, "Sue %d: %[a-z]: %d, %[a-z]: %d, %[a-z]: %d",
			&sueIndex,
			aType,
			&aCount,
			bType,
			&bCount,
			cType,
			&cCount);
		assert(scanned == 7);
		(void)scanned;

		Sue sue{ sueIndex, { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } };
		sue.Owns[TypeIndex(aType)] = aCount;
		sue.Owns[TypeIndex(bType)] = bCount;
		sue.Owns[TypeIndex(cType)] = cCount;

		bool isCorrectSue = true;
		for (size_t i = 0; i < NUM_ITEMS; i++)
		{
			if (sue.Owns[i] != -1)
			{
				bool exactMismatch = ((needExact[i] != -1) && (sue.Owns[i] != needExact[i]));
				bool greaterThanMismatch = ((needGreaterThan[i] != -1) && (sue.Owns[i] <= needGreaterThan[i]));
				bool lessThanMismatch = ((needLessThan[i] != -1) && (sue.Owns[i] >= needLessThan[i]));
				if (exactMismatch || greaterThanMismatch || lessThanMismatch)
				{
					isCorrectSue = false;
					break;
				}
			}
		}

		if (isCorrectSue)
		{
			answer = sueIndex;
			break;
		}
	}

	return PuzzleOutput::Submit(2015, 16, 2, answer);
}
