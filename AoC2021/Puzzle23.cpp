#include "stdafx.h"

using namespace std;

namespace Puzzle23_2021_Types
{
	enum : int8_t
	{
		NumCorridorSlots = 11,
		NumBedrooms = 4,
		NumBunks = 4,
	};

	struct Burrow
	{
		array<Vec2Int, NumCorridorSlots> Corridor;
		array<array<Vec2Int, NumBedrooms>, NumBunks> Bedrooms;
		vector<int32_t> WaitingSlots = { 0, 1, 3, 5, 7, 9, 10 };
	};

	struct UncompressedState
	{
		int8_t Corridor[NumCorridorSlots];
		int8_t Bedrooms[NumBedrooms][NumBunks];
		auto operator<=>(const UncompressedState&) const = default;
	};

	struct CompressedState
	{
		// Data[0] = Corridor[0,1,3,5,7,9,10]:Bedroom[0] = 5^11
		// Data[1] = Bedroom[1]:Bedroom[2]:Bedroom[3] = 5^12

		uint32_t Data[2];
		auto operator<=>(const CompressedState&) const = default;
	};

	struct CompressedSearchNode
	{
		int32_t Priority;
		CompressedState State;
		auto operator<=>(const CompressedSearchNode&) const = default;
	};

	struct Neighbour
	{
		int32_t Distance;
		UncompressedState State;
	};
}

using namespace Puzzle23_2021_Types;

template <>
struct std::hash<CompressedState>
{
	size_t operator()(const CompressedState& m) const noexcept
	{
		uint32_t hash = 0x811c9dc5;

		hash ^= m.Data[0];
		hash *= 0x01000193;
		hash ^= m.Data[1];
		hash *= 0x01000193;

		return hash;
	}
};

static uint32_t DivModRem(uint32_t data, int8_t* rem)
{
	int32_t rem32 = 0;
	data = Hardware::DivModRem(data, 5, &rem32);
	*rem = static_cast<char>(rem32);
	return data;
}

static void CompressState(const UncompressedState& src, CompressedState* dst)
{
	uint32_t data = 0;
	data = (data * 5) + src.Corridor[0];
	data = (data * 5) + src.Corridor[1];
	data = (data * 5) + src.Corridor[3];
	data = (data * 5) + src.Corridor[5];
	data = (data * 5) + src.Corridor[7];
	data = (data * 5) + src.Corridor[9];
	data = (data * 5) + src.Corridor[10];
	data = (data * 5) + src.Bedrooms[0][0];
	data = (data * 5) + src.Bedrooms[0][1];
	data = (data * 5) + src.Bedrooms[0][2];
	data = (data * 5) + src.Bedrooms[0][3];
	dst->Data[0] = data;

	data = 0;
	for (size_t i = 1; i < 4; i++)
	{
		data = (data * 5) + src.Bedrooms[i][0];
		data = (data * 5) + src.Bedrooms[i][1];
		data = (data * 5) + src.Bedrooms[i][2];
		data = (data * 5) + src.Bedrooms[i][3];
	}
	dst->Data[1] = data;
}

static void UncompressState(const CompressedState& src, UncompressedState* dst)
{
	uint32_t data = src.Data[0];
	data = DivModRem(data, &dst->Bedrooms[0][3]);
	data = DivModRem(data, &dst->Bedrooms[0][2]);
	data = DivModRem(data, &dst->Bedrooms[0][1]);
	data = DivModRem(data, &dst->Bedrooms[0][0]);
	data = DivModRem(data, &dst->Corridor[10]);
	data = DivModRem(data, &dst->Corridor[9]);
	dst->Corridor[8] = 0;
	data = DivModRem(data, &dst->Corridor[7]);
	dst->Corridor[6] = 0;
	data = DivModRem(data, &dst->Corridor[5]);
	dst->Corridor[4] = 0;
	data = DivModRem(data, &dst->Corridor[3]);
	dst->Corridor[2] = 0;
	data = DivModRem(data, &dst->Corridor[1]);
	data = DivModRem(data, &dst->Corridor[0]);
	
	data = src.Data[1];
	for (size_t i = 3; i > 0; i--)
	{
		data = DivModRem(data, &dst->Bedrooms[i][3]);
		data = DivModRem(data, &dst->Bedrooms[i][2]);
		data = DivModRem(data, &dst->Bedrooms[i][1]);
		data = DivModRem(data, &dst->Bedrooms[i][0]);
	}
}

static int32_t PutToBed(const UncompressedState& current, char amphipod, int32_t corridor, int32_t bedroom, const Burrow& burrow, UncompressedState* next)
{
	int32_t bunk = 3;
	while (current.Bedrooms[bedroom][bunk] != 0)
		bunk--;

	assert(bunk >= 0);
	assert(current.Bedrooms[bedroom][bunk] == 0);
	assert(current.Corridor[corridor] == amphipod);

	*next = current;
	next->Corridor[corridor] = 0;
	next->Bedrooms[bedroom][bunk] = amphipod;

	int32_t cost = ManhattanDistance(burrow.Corridor[corridor], burrow.Bedrooms[bedroom][bunk]);
	for (int32_t power = 1; power < amphipod; power++)
	{
		cost *= 10;
	}

	return cost;
}

static int32_t MoveToCorridor(const UncompressedState& current, char amphipod, int32_t bedroom, int32_t bunk, int32_t corridor, const Burrow& burrow, UncompressedState* next)
{
	assert(current.Bedrooms[bedroom][bunk] == amphipod);
	assert(current.Corridor[corridor] == 0);

	*next = current;
	next->Corridor[corridor] = amphipod;
	next->Bedrooms[bedroom][bunk] = 0;

	int32_t cost = ManhattanDistance(burrow.Bedrooms[bedroom][bunk], burrow.Corridor[corridor]);
	for (int32_t power = 1; power < amphipod; power++)
	{
		cost *= 10;
	}

	return cost;
}

static vector<Neighbour> GetNeighbours(const UncompressedState& current, const Burrow& burrow)
{
	vector<Neighbour> neighbours;

	// Check corridor to room paths
	array<array<bool, NumBedrooms>, NumCorridorSlots> corridorToBedroom;
	ranges::fill(corridorToBedroom | views::join, true);

	if (current.Corridor[1])
	{
		corridorToBedroom[0][0] = false;
		corridorToBedroom[0][1] = false;
		corridorToBedroom[0][2] = false;
		corridorToBedroom[0][3] = false;
	}

	if (current.Corridor[3])
	{
		corridorToBedroom[0][1] = false;
		corridorToBedroom[0][2] = false;
		corridorToBedroom[0][3] = false;

		corridorToBedroom[0][1] = false;
		corridorToBedroom[0][2] = false;
		corridorToBedroom[0][3] = false;

		corridorToBedroom[1][1] = false;
		corridorToBedroom[1][2] = false;
		corridorToBedroom[1][3] = false;

		corridorToBedroom[5][0] = false;

		corridorToBedroom[7][0] = false;

		corridorToBedroom[9][0] = false;

		corridorToBedroom[10][0] = false;
	}

	if (current.Corridor[5])
	{
		corridorToBedroom[0][2] = false;
		corridorToBedroom[0][3] = false;

		corridorToBedroom[1][2] = false;
		corridorToBedroom[1][3] = false;

		corridorToBedroom[3][2] = false;
		corridorToBedroom[3][3] = false;

		corridorToBedroom[7][0] = false;
		corridorToBedroom[7][1] = false;

		corridorToBedroom[9][0] = false;
		corridorToBedroom[9][1] = false;

		corridorToBedroom[10][0] = false;
		corridorToBedroom[10][1] = false;
	}

	if (current.Corridor[7])
	{
		corridorToBedroom[0][3] = false;

		corridorToBedroom[1][3] = false;

		corridorToBedroom[3][3] = false;

		corridorToBedroom[5][3] = false;;

		corridorToBedroom[9][0] = false;
		corridorToBedroom[9][1] = false;
		corridorToBedroom[9][2] = false;

		corridorToBedroom[10][0] = false;
		corridorToBedroom[10][1] = false;
		corridorToBedroom[10][2] = false;
	}

	if (current.Corridor[9])
	{
		corridorToBedroom[10][0] = false;
		corridorToBedroom[10][1] = false;
		corridorToBedroom[10][2] = false;
		corridorToBedroom[10][3] = false;
	}

	// Check bedroom states
	array<bool, NumBedrooms> bedroomOpenForBusiness{};
	for (int32_t i = 0; i < bedroomOpenForBusiness.size(); i++)
	{
		bedroomOpenForBusiness[i] = ranges::all_of(current.Bedrooms[i], [&](char a) { return (a == 0) || (a == (i + 1)); });
	}

	// Put someone home if we can
	for (int32_t slot : burrow.WaitingSlots)
	{
		char amphipod = current.Corridor[slot];
		if (amphipod == 0)
			continue;

		int32_t amphipodBedroom = amphipod - 1;
		if (bedroomOpenForBusiness[amphipodBedroom] && corridorToBedroom[slot][amphipodBedroom])
		{
			Neighbour next;
			next.Distance = PutToBed(current, amphipod, slot, amphipodBedroom, burrow, &next.State);
			neighbours.push_back(next);
		}
	}

	// Pull someone out of bedrooms that are not open for business
	for (int32_t bedroom = 0; bedroom < NumBedrooms; bedroom++)
	{
		for (int32_t bunk = 0; bunk < NumBunks; bunk++)
		{
			char amphipod = current.Bedrooms[bedroom][bunk];
			if (amphipod == 0)
				continue;

			if (bedroomOpenForBusiness[bedroom] == false)
			{
				for (int32_t slot : burrow.WaitingSlots)
				{
					if ((current.Corridor[slot] == 0) && (corridorToBedroom[slot][bedroom]))
					{
						Neighbour next;
						next.Distance = MoveToCorridor(current, amphipod, bedroom, bunk, slot, burrow, &next.State);
						neighbours.push_back(next);
					}
				}
			}

			break;
		}
	}

	return neighbours;
}

int32_t Heuristic(const UncompressedState& from, const Burrow& burrow)
{
	int32_t minimumCost = 0;

	// Every amphipod in the corridor needs to go to their bedroom
	for (int32_t slot : burrow.WaitingSlots)
	{
		int8_t amphipod = from.Corridor[slot];
		if (amphipod == 0)
			continue;

		int32_t amphipodBedroom = amphipod - 1;
		int32_t cost = ManhattanDistance(burrow.Corridor[slot], burrow.Bedrooms[amphipodBedroom][0]);
		for (int32_t power = 1; power < amphipod; power++)
		{
			cost *= 10;
		}
		minimumCost += cost;
	}

	// Every amphipod in the wrong bedroom needs to go to the right bedroom
	for (int32_t bedroom = 0; bedroom < NumBedrooms; bedroom++)
	{
		for (int32_t bunk = 0; bunk < NumBunks; bunk++)
		{
			int8_t amphipod = from.Bedrooms[bedroom][bunk];
			if (amphipod == 0)
				continue;

			int32_t amphipodBedroom = amphipod - 1;
			if (amphipodBedroom == bedroom)
				continue;

			int32_t cost = ManhattanDistance(burrow.Bedrooms[bedroom][bunk], burrow.Bedrooms[amphipodBedroom][0]) + 2; // +2: into the corridor and out again
			for (int32_t power = 1; power < amphipod; power++)
			{
				cost *= 10;
			}
			minimumCost += cost;
		}
	}

	return minimumCost;
}

static int32_t ShortestPath(const UncompressedState& uncompressedStart,
	const UncompressedState& uncompressedEnd,
	const Burrow& burrow,
	const size_t expectedMaxQueueSize,
	const uint32_t gScoreTableSize)
{
	CompressedState compressedStart;
	CompressState(uncompressedStart, &compressedStart);

	vector<CompressedSearchNode> priorityQueue;
	priorityQueue.reserve(expectedMaxQueueSize);
	priorityQueue.push_back({ 0, compressedStart });

	MaxValue<size_t> biggestQueue;

	auto enqueue = [&](const CompressedSearchNode& s)
		{
			priorityQueue.push_back(s);
			assert(priorityQueue.size() <= expectedMaxQueueSize);
			ranges::push_heap(priorityQueue);

			biggestQueue.Update(priorityQueue.size());
		};

	auto dequeue = [&]()
		{
			CompressedSearchNode s = priorityQueue[0];
			ranges::pop_heap(priorityQueue);
			priorityQueue.pop_back();
			return s;
		};

	HashMap<CompressedState, int32_t> gScore(gScoreTableSize, { 0 });
	gScore.Set(compressedStart, 0);

	UncompressedState uncompressedState;
	while (priorityQueue.empty() == false)
	{
		CompressedSearchNode current = dequeue();
		UncompressState(current.State, &uncompressedState);
		if (uncompressedState == uncompressedEnd)
		{
			return -current.Priority;
		}

		vector<Neighbour> neighbours = GetNeighbours(uncompressedState, burrow);
		for (const Neighbour& neighbour : neighbours)
		{
			CompressedState compressedNeighbour;
			CompressState(neighbour.State, &compressedNeighbour);

			int32_t tentativeGScore = gScore.At(current.State) + neighbour.Distance;
			if (gScore.FindOrDefault(compressedNeighbour, numeric_limits<int32_t>::max()) > tentativeGScore)
			{
				gScore.Set(compressedNeighbour, static_cast<int32_t>(tentativeGScore));
				int32_t priority = tentativeGScore + Heuristic(neighbour.State, burrow);
				enqueue({ static_cast<int32_t>(-priority), compressedNeighbour });
			}
		}
	}

	return -1;
}

void Puzzle23_A_2021()
{
	UncompressedState end;
	memset(end.Corridor, 0, sizeof(end.Corridor));

	for (int8_t i = 0; i < NumBedrooms; i++)
	{
		for (int8_t j = 0; j < NumBunks; j++)
		{
			end.Bedrooms[i][j] = i + 1;
		}
	}

	UncompressedState start = end;

	Parse::DiscardExpected("#############\n");
	Parse::DiscardExpected("#...........#\n");

	array<char, 16> line;

	Parse::ReadNonEmptyLine(line.data(), line.size());
	start.Bedrooms[0][0] = line[3] - 'A' + 1;
	start.Bedrooms[1][0] = line[5] - 'A' + 1;
	start.Bedrooms[2][0] = line[7] - 'A' + 1;
	start.Bedrooms[3][0] = line[9] - 'A' + 1;

	Parse::ReadNonEmptyLine(line.data(), line.size());
	start.Bedrooms[0][1] = line[3] - 'A' + 1;
	start.Bedrooms[1][1] = line[5] - 'A' + 1;
	start.Bedrooms[2][1] = line[7] - 'A' + 1;
	start.Bedrooms[3][1] = line[9] - 'A' + 1;

	{
		CompressedState testA;
		UncompressedState testB;

		CompressState(start, &testA);
		UncompressState(testA, &testB);
		assert(testB == start);

		CompressState(end, &testA);
		UncompressState(testA, &testB);
		assert(testB == end);
	}

	Burrow burrow;
	for (int32_t i = 0; i < burrow.Corridor.size(); i++)
	{
		burrow.Corridor[i] = Vec2Int{ i, 0 };
	}
	for (int32_t i = 0; i < burrow.Bedrooms[0].size(); i++)
	{
		burrow.Bedrooms[0][i] = Vec2Int{ 2, i + 1 };
		burrow.Bedrooms[1][i] = Vec2Int{ 4, i + 1 };
		burrow.Bedrooms[2][i] = Vec2Int{ 6, i + 1 };
		burrow.Bedrooms[3][i] = Vec2Int{ 8, i + 1 };
	}

	const size_t expectedMaxQueueSize = 6000;
	const uint32_t gScoreTableSize = 16 * 1024;
	int32_t answer = ShortestPath(start, end, burrow, expectedMaxQueueSize, gScoreTableSize);

	PuzzleOutput::Submit(2021, 23, 1, answer);
}

void Puzzle23_B_2021()
{
	UncompressedState end;
	memset(end.Corridor, 0, sizeof(end.Corridor));

	for (int8_t i = 0; i < NumBedrooms; i++)
	{
		for (int8_t j = 0; j < NumBunks; j++)
		{
			end.Bedrooms[i][j] = i + 1;
		}
	}

	UncompressedState start = end;

	Parse::DiscardExpected("#############\n");
	Parse::DiscardExpected("#...........#\n");

	array<char, 16> line;

	Parse::ReadNonEmptyLine(line.data(), line.size());
	start.Bedrooms[0][0] = line[3] - 'A' + 1;
	start.Bedrooms[1][0] = line[5] - 'A' + 1;
	start.Bedrooms[2][0] = line[7] - 'A' + 1;
	start.Bedrooms[3][0] = line[9] - 'A' + 1;

	start.Bedrooms[0][1] = 'D' - 'A' + 1;
	start.Bedrooms[1][1] = 'C' - 'A' + 1;
	start.Bedrooms[2][1] = 'B' - 'A' + 1;
	start.Bedrooms[3][1] = 'A' - 'A' + 1;

	start.Bedrooms[0][2] = 'D' - 'A' + 1;
	start.Bedrooms[1][2] = 'B' - 'A' + 1;
	start.Bedrooms[2][2] = 'A' - 'A' + 1;
	start.Bedrooms[3][2] = 'C' - 'A' + 1;

	Parse::ReadNonEmptyLine(line.data(), line.size());
	start.Bedrooms[0][3] = line[3] - 'A' + 1;
	start.Bedrooms[1][3] = line[5] - 'A' + 1;
	start.Bedrooms[2][3] = line[7] - 'A' + 1;
	start.Bedrooms[3][3] = line[9] - 'A' + 1;

	Burrow burrow;
	for (int32_t i = 0; i < burrow.Corridor.size(); i++)
	{
		burrow.Corridor[i] = Vec2Int{ i, 0 };
	}
	for (int32_t i = 0; i < burrow.Bedrooms[0].size(); i++)
	{
		burrow.Bedrooms[0][i] = Vec2Int{ 2, i + 1 };
		burrow.Bedrooms[1][i] = Vec2Int{ 4, i + 1 };
		burrow.Bedrooms[2][i] = Vec2Int{ 6, i + 1 };
		burrow.Bedrooms[3][i] = Vec2Int{ 8, i + 1 };
	}

	const size_t expectedMaxQueueSize = 10000;
	const uint32_t gScoreTableSize = 128 * 1024;
	int32_t answer = ShortestPath(start, end, burrow, expectedMaxQueueSize, gScoreTableSize);

	PuzzleOutput::Submit(2021, 23, 2, answer);
}
