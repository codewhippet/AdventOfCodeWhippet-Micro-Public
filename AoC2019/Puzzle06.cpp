#include "stdafx.h"

using namespace std;

namespace Puzzle06_2019_Types
{
}

using namespace Puzzle06_2019_Types;

static uint32_t IdFromInput()
{
	uint32_t id = 0;
	id = (id << 8) | PuzzleInput::GetChar();
	id = (id << 8) | PuzzleInput::GetChar();
	id = (id << 8) | PuzzleInput::GetChar();
	return id;
}

static uint32_t IdFromString(const char* s)
{
	uint32_t id = 0;
	id = (id << 8) | *s++;
	id = (id << 8) | *s++;
	id = (id << 8) | *s++;
	return id;
}

void Puzzle06_A_2019()
{
	const size_t MAX_ORBITALS = 3;

	HashMap<uint32_t, SmallVector<uint32_t, MAX_ORBITALS>> orbits(4 * 1024, 0xffffffff);
	while (PuzzleInput::NextLine())
	{
		uint32_t a = IdFromInput();
		PuzzleInput::DropChar();
		uint32_t b = IdFromInput();

		orbits[a].PushBack(b);
		orbits[b]; // Make sure b exists as a key in the map
	}

	const uint32_t start = IdFromString("COM");

	array<vector<uint32_t>, 2> searchQueues;
	searchQueues[0].reserve(32);
	searchQueues[1].reserve(32);
	searchQueues[0].push_back(start);

	HashSet<uint32_t> queued(4 * 1024, 0xffffffff);
	queued.Insert(start);

	int32_t answer = 0;
	for (int32_t steps = 0; /***/; steps++)
	{
		const vector<uint32_t>& currentQueue = searchQueues[steps & 1];
		if (currentQueue.empty())
			break;

		vector<uint32_t>& nextQueue = searchQueues[(steps + 1) & 1];
		nextQueue.clear();

		for (uint32_t currentPlanet : currentQueue)
		{
			const SmallVector<uint32_t, MAX_ORBITALS>& outerPlanets = orbits.At(currentPlanet);
			for (uint32_t outerPlanet : outerPlanets)
			{
				if (queued.Insert(outerPlanet))
				{
					assert(outerPlanet);
					nextQueue.push_back(outerPlanet);
					answer += steps + 1;
				}
			}
		}
	}

	PuzzleOutput::Submit(2019, 6, 1, answer);
}

void Puzzle06_B_2019()
{
	const size_t MAX_ORBITALS = 4;

	HashMap<uint32_t, SmallVector<uint32_t, MAX_ORBITALS>> orbits(4 * 1024, 0xffffffff);
	while (PuzzleInput::NextLine())
	{
		uint32_t a = IdFromInput();
		PuzzleInput::DropChar();
		uint32_t b = IdFromInput();

		orbits[a].PushBack(b);
		orbits[b].PushBack(a);
	}

	const uint32_t start = IdFromString("YOU");
	const uint32_t end = IdFromString("SAN");

	array<vector<uint32_t>, 2> searchQueues;
	searchQueues[0].reserve(32);
	searchQueues[1].reserve(32);
	searchQueues[0].push_back(start);

	HashSet<uint32_t> queued(4 * 1024, 0xffffffff);
	queued.Insert(start);

	int32_t answer = 0;
	for (int32_t steps = 0; answer == 0; steps++)
	{
		const vector<uint32_t>& currentQueue = searchQueues[steps & 1];
		if (currentQueue.empty())
			break;

		vector<uint32_t>& nextQueue = searchQueues[(steps + 1) & 1];
		nextQueue.clear();

		for (uint32_t currentPlanet : currentQueue)
		{
			if (currentPlanet == end)
			{
				answer = steps - 2;
				break;
			}

			const SmallVector<uint32_t, MAX_ORBITALS>& outerPlanets = orbits.At(currentPlanet);
			for (uint32_t outerPlanet : outerPlanets)
			{
				if (queued.Insert(outerPlanet))
				{
					assert(outerPlanet);
					nextQueue.push_back(outerPlanet);
				}
			}
		}
	}

	PuzzleOutput::Submit(2019, 6, 2, answer);
}
