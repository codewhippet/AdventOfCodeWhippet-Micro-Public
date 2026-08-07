#include "stdafx.h"

using namespace std;

namespace Puzzle13_2016_Types
{
	class Vec2Set
	{
	public:
		Vec2Set(size_t tableSize)
			: Table(tableSize, { -1, -1 })
		{
		}
		Vec2Set(const Vec2Set&) = delete;
		Vec2Set& operator=(const Vec2Set&) = delete;

		bool Insert(const Vec2Short& v)
		{
			size_t hashBegin = std::hash<Vec2Short>{}(v);
			size_t hashEnd = hashBegin + Table.size();
			for (size_t i = hashBegin; i < hashEnd; i++)
			{
				const size_t tableIndex = i % Table.size();
				if (Table[tableIndex] == v)
				{
					return false;
				}

				if (Table[tableIndex] == Vec2Short{ -1, -1 })
				{
					Table[tableIndex] = v;
					return true;
				}
			}

			assert(false);
			return false;
		}

	private:
		vector<Vec2Short> Table;
	};
}

using namespace Puzzle13_2016_Types;

static bool IsSpace(const int32_t magic, const Vec2Short& pos)
{
	if ((pos.X < 0) || (pos.Y < 0))
	{
		return false;
	}

	uint32_t polynomial = (pos.X * pos.X) + (3 * pos.X) + (2 * pos.X * pos.Y) + (pos.Y) + (pos.Y * pos.Y);
	polynomial += magic;
	int bitsSet = popcount(polynomial);
	return (bitsSet % 2) == 0;
}

static int32_t FindShortestRoute(const int32_t magic, const Vec2Short& start, const Vec2Short& target)
{
	vector<pair<int16_t, Vec2Short>> searchQueue;
	searchQueue.reserve(256);
	searchQueue.push_back({ int16_t(0), start });

	Vec2Set queued(512);
	queued.Insert(start);

	for (size_t i = 0; i < searchQueue.size(); i++)
	{
		const pair<int16_t, Vec2Short> current = searchQueue[i];
		if (current.second == target)
		{
			return current.first;
		}

		for (const Vec2Short& dir : Vec2Short::CardinalDirections())
		{
			Vec2Short neighbour = current.second + dir;
			if (IsSpace(magic, neighbour) && queued.Insert(neighbour))
			{
				searchQueue.push_back({ static_cast<int16_t>(current.first + 1), neighbour });
			}
		}
	}

	return -1;
}

static int32_t BFSToDepth(const int32_t magic, const Vec2Short& start, const int32_t steps)
{
	vector<pair<int16_t, Vec2Short>> searchQueue;
	searchQueue.reserve(256);
	searchQueue.push_back({ int16_t(0), start });

	Vec2Set queued(512);
	queued.Insert(start);

	for (size_t i = 0; i < searchQueue.size(); i++)
	{
		const pair<int16_t, Vec2Short> current = searchQueue[i];
		if (current.first == steps)
		{
			continue;
		}

		for (const Vec2Short& dir : Vec2Short::CardinalDirections())
		{
			Vec2Short neighbour = current.second + dir;
			if (IsSpace(magic, neighbour) && queued.Insert(neighbour))
			{
				searchQueue.push_back({ static_cast<int16_t>(current.first + 1), neighbour });
			}
		}
	}

	return static_cast<int32_t>(searchQueue.size());
}

void Puzzle13_A_2016()
{
	const Vec2Short start{ 1, 1 };
	const Vec2Short goal{ 31, 39 };

	int32_t magicNumber = Parse::GetInt32();
	assert(IsSpace(magicNumber, goal));
	int32_t answer = FindShortestRoute(magicNumber, start, goal);

	return PuzzleOutput::Submit(2016, 13, 1, answer);
}

void Puzzle13_B_2016()
{
	const Vec2Short start{ 1, 1 };

	int32_t magicNumber = Parse::GetInt32();
	int32_t answer = BFSToDepth(magicNumber, start, 50);

	return PuzzleOutput::Submit(2016, 13, 2, answer);
}
