#include "stdafx.h"

using namespace std;

namespace Puzzle12_2021_Types
{
	enum class Joker : int32_t
	{
		NotUsed,
		Used
	};

	struct CaveSystem
	{
		int32_t Start;
		int32_t End;
		vector<uint32_t> Restrictions;
		vector<SmallVector<int32_t, 8>> Neighbours;
	};

	struct ExplorationState
	{
		int32_t CurrentCave;
		uint32_t VisitedCaves;
		Joker UsedJoker;

		int32_t ExplorationValue;
		int32_t NextNeighbour;
		int32_t* Output;
	};

	struct CacheKey
	{
		int32_t CurrentCave;
		uint32_t VisitedCaves;
		Joker UsedJoker;

		bool operator==(const CacheKey& other) const
		{
			return CurrentCave == other.CurrentCave &&
				VisitedCaves == other.VisitedCaves &&
				UsedJoker == other.UsedJoker;
		}
	};
}

using namespace Puzzle12_2021_Types;

template <>
struct std::hash<CacheKey>
{
	size_t operator()(const CacheKey& key) const noexcept
	{
		uint32_t hash = 0x811c9dc5;

		hash ^= key.CurrentCave;
		hash *= 0x01000193;
		hash ^= key.VisitedCaves;
		hash *= 0x01000193;
		hash ^= static_cast<uint32_t>(key.UsedJoker);
		hash *= 0x01000193;

		return hash;
	}
};

static CaveSystem ReadCaveSystem()
{
	const size_t maxNumberCaves = 32;

	CaveSystem caves;
	caves.Restrictions.reserve(maxNumberCaves);
	caves.Neighbours.reserve(maxNumberCaves);

	NameDictionary names(64);
	array<char, 16> line;
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line.data(), line.size());
		char* separator = to_address(ranges::find(line, '-'));

		*separator = '\0';
		int32_t from = names.IdFromName(line.data());
		int32_t to = names.IdFromName(separator + 1);

		const size_t minArraySize = max<size_t>(max(from, to) + 1, caves.Restrictions.size());
		caves.Restrictions.resize(minArraySize);
		caves.Neighbours.resize(minArraySize);

		if (islower(*line.data()))
		{
			caves.Restrictions[from] = 1u << from;
		}

		if (islower(*(separator + 1)))
		{
			caves.Restrictions[to] = 1u << to;
		}

		caves.Neighbours[from].PushBack(to);
		caves.Neighbours[to].PushBack(from);
	}

	caves.Start = names.IdFromName("start");
	caves.End = names.IdFromName("end");

	return caves;
}

static bool CheckIfCanVisit(const CaveSystem& caveSystem, int32_t caveToVisit, uint32_t visitedCaves, Joker* usedJoker)
{
	bool canVisit = true;
	if (caveSystem.Restrictions[caveToVisit] & visitedCaves)
	{
		// We can revisit a cave if we still have a joker
		if (*usedJoker == Joker::NotUsed)
		{
			*usedJoker = Joker::Used;
		}
		else
		{
			canVisit = false;
		}
	}

	return canVisit;
}

static int32_t Explore(const CaveSystem& caveSystem, const Joker usedJoker)
{
	const uint32_t solutionCacheSize = 2 * 1024;

	vector<ExplorationState> exec;
	exec.reserve(caveSystem.Restrictions.size() * 2);

	int32_t explorationValue = 0;
	{
		ExplorationState startingState;
		startingState.CurrentCave = caveSystem.Start;
		startingState.VisitedCaves = 0;
		startingState.UsedJoker = usedJoker;
		startingState.ExplorationValue = 0;
		startingState.NextNeighbour = 0;
		startingState.Output = &explorationValue;
		exec.push_back(startingState);
	}

	HashMap<CacheKey, int32_t> solutionCache(solutionCacheSize, { -1 });
	while (exec.empty() == false)
	{
		ExplorationState& current = exec.back();

		CacheKey key;
		key.CurrentCave = current.CurrentCave;
		key.VisitedCaves = current.VisitedCaves;
		key.UsedJoker = current.UsedJoker;

		int32_t existingSolution = 0;
		if (solutionCache.TryFind(key, &existingSolution))
		{
			*current.Output += existingSolution;
			exec.pop_back();
			continue;
		}

		if (current.NextNeighbour == 0)
		{
			if (current.CurrentCave == caveSystem.End)
			{
				// We have found a valid path
				*current.Output += 1;
				exec.pop_back();
				continue;
			}

			if (current.CurrentCave == caveSystem.Start)
			{
				// We can only visit the starting cave once
				if (current.VisitedCaves & (1u << current.CurrentCave))
				{
					exec.pop_back();
					continue;
				}
			}

			const int32_t neighbour = caveSystem.Neighbours[current.CurrentCave][current.NextNeighbour];
			const uint32_t newVisitedCaves = current.VisitedCaves | caveSystem.Restrictions[current.CurrentCave];
			Joker willHaveUsedJoker = current.UsedJoker;

			if (CheckIfCanVisit(caveSystem, neighbour, newVisitedCaves, &willHaveUsedJoker))
			{
				ExplorationState neighbourState;
				neighbourState.CurrentCave = neighbour;
				neighbourState.VisitedCaves = newVisitedCaves;
				neighbourState.UsedJoker = willHaveUsedJoker;
				neighbourState.ExplorationValue = 0;
				neighbourState.NextNeighbour = 0;
				neighbourState.Output = &current.ExplorationValue;
				exec.push_back(neighbourState);
			}

			current.NextNeighbour++;
		}
		else
		{
			if (current.NextNeighbour == caveSystem.Neighbours[current.CurrentCave].size())
			{
				solutionCache.Insert(key, current.ExplorationValue);

				*current.Output += current.ExplorationValue;
				exec.pop_back();
				continue;
			}
			else
			{
				const int32_t neighbour = caveSystem.Neighbours[current.CurrentCave][current.NextNeighbour];
				const uint32_t newVisitedCaves = current.VisitedCaves | caveSystem.Restrictions[current.CurrentCave];
				Joker willHaveUsedJoker = current.UsedJoker;

				if (CheckIfCanVisit(caveSystem, neighbour, newVisitedCaves, &willHaveUsedJoker))
				{
					ExplorationState neighbourState;
					neighbourState.CurrentCave = neighbour;
					neighbourState.VisitedCaves = newVisitedCaves;
					neighbourState.UsedJoker = willHaveUsedJoker;
					neighbourState.ExplorationValue = 0;
					neighbourState.NextNeighbour = 0;
					neighbourState.Output = &current.ExplorationValue;
					exec.push_back(neighbourState);
				}

				current.NextNeighbour++;
			}
		}
	}

	return explorationValue;
}

void Puzzle12_A_2021()
{
	CaveSystem caves = ReadCaveSystem();
	int32_t answer = Explore(caves, Joker::Used);

	PuzzleOutput::Submit(2021, 12, 1, answer);
}

void Puzzle12_B_2021()
{
	CaveSystem caves = ReadCaveSystem();
	int32_t answer = Explore(caves, Joker::NotUsed);

	PuzzleOutput::Submit(2021, 12, 2, answer);
}
