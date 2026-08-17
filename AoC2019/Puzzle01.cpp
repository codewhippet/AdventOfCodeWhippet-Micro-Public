#include "stdafx.h"

using namespace std;

namespace Puzzle01_2019_Types
{
}

using namespace Puzzle01_2019_Types;

static int32_t FuelRequired(int32_t mass)
{
	return (mass / 3) - 2;
}

static int32_t FuelRequiredNonZero(int32_t mass)
{
	return max(FuelRequired(mass), 0);
}

static int32_t FuelForMassAndFuel(int32_t mass)
{
	int32_t totalFuelNeeded = 0;
	while (mass > 0)
	{
		int32_t additionalFuel = FuelRequiredNonZero(mass);
		totalFuelNeeded += additionalFuel;
		mass = additionalFuel;
	}
	return totalFuelNeeded;
}

void Puzzle01_A_2019()
{
	int32_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		answer += FuelRequired(Parse::GetInt32());
	}

	PuzzleOutput::Submit(2019, 1, 1, answer);
}

void Puzzle01_B_2019()
{
	int32_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		answer += FuelForMassAndFuel(Parse::GetInt32());
	}

	PuzzleOutput::Submit(2019, 1, 2, answer);
}
