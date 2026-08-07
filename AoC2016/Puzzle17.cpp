#include "stdafx.h"

using namespace std;

namespace Puzzle17_2016_Types
{
}

using namespace Puzzle17_2016_Types;

static uvector<char> OpenDoors(const ustring& passcode, const ustring& route)
{
	uint8_t hash[16];
	uMD5::Hash(passcode + route, hash);

	uvector<char> doors;
	doors.reserve(4);

	if (((hash[0] >> 4) & 0xf) > 0xa)
	{
		doors.push_back('U');
	}
	if (((hash[0] >> 0) & 0xf) > 0xa)
	{
		doors.push_back('D');
	}
	if (((hash[1] >> 4) & 0xf) > 0xa)
	{
		doors.push_back('L');
	}
	if (((hash[1] >> 0) & 0xf) > 0xa)
	{
		doors.push_back('R');
	}
	return doors;
}

static ustring ShortestPathTo(const uArrayMap2D& rooms, const Vec2Int& start, const Vec2Int& end, const ustring& passcode)
{
	const umap<char, Vec2Int> directions = { { 'U', Vec2Int::Up() }, { 'D', Vec2Int::Down() }, { 'L', Vec2Int::Left() }, { 'R', Vec2Int::Right() } };

	uvector<pair<Vec2Int, ustring>> searchQueue;
	searchQueue.reserve(256);
	searchQueue.push_back({ start, "" });

	for (size_t i = 0; i < searchQueue.size(); i++)
	{
		if (searchQueue[i].first == end)
		{
			return searchQueue[i].second;
		}

		uvector<char> openDoors = OpenDoors(passcode, searchQueue[i].second);
		for (char c : openDoors)
		{
			Vec2Int nextPos = searchQueue[i].first + directions.at(c);
			if (rooms.IsInside(nextPos))
			{
				searchQueue.push_back({ nextPos, searchQueue[i].second + c });
			}
		}
	}

	return "";
}

static int32_t LongestPathTo(const uArrayMap2D& rooms, const Vec2Int& start, const Vec2Int& end, const ustring& passcode)
{
	const umap<char, Vec2Int> directions = { { 'U', Vec2Int::Up() }, { 'D', Vec2Int::Down() }, { 'L', Vec2Int::Left() }, { 'R', Vec2Int::Right() } };

	int32_t longestPath = 0;

	ulist<pair<Vec2Int, ustring>> searchQueue{ { start, "" } };
	while (searchQueue.empty() == false)
	{
		pair<Vec2Int, ustring> current = move(searchQueue.front());
		searchQueue.pop_front();

		if (current.first == end)
		{
			longestPath = max<int32_t>(longestPath, static_cast<int32_t>(current.second.size()));
			continue;
		}

		uvector<char> openDoors = OpenDoors(passcode, current.second);
		for (char c : openDoors)
		{
			Vec2Int nextPos = current.first + directions.at(c);
			if (rooms.IsInside(nextPos))
			{
				searchQueue.push_back({ nextPos, current.second + c });
			}
		}
	}

	return longestPath;
}

void Puzzle17_A_2016()
{
	const MemArenaSmallBlockCount sbas[] =
	{
		{ 16, 4 },
		{ 128, 16 },
		{ 64, 32 },
	};

	MemArenaConfig cfg;
	cfg.SmallBlockCounts = sbas;
	cfg.NumSmallBlockCounts = sizeof(sbas) / sizeof(sbas[0]);
	cfg.LargeBlockRegionSize = 16 * 1024;

	MemArena_Configure(cfg);
	{
		char passcode[16];
		Parse::ReadNonEmptyLine(passcode);

		const uArrayMap2D rooms{ {}, 4, 4, '.' };
		ustring answer = ShortestPathTo(rooms, Vec2Int{ 0, 0 }, Vec2Int{ 3, 3 }, passcode);

		PuzzleOutput::Submit(2016, 17, 1, answer.c_str());
	}
	MemArena_Reset();
}

void Puzzle17_B_2016()
{
	const MemArenaSmallBlockCount sbas[] =
	{
		{ 16, 4 },
		{ 256, 48 },
	};

	MemArenaConfig cfg;
	cfg.SmallBlockCounts = sbas;
	cfg.NumSmallBlockCounts = sizeof(sbas) / sizeof(sbas[0]);
	cfg.LargeBlockRegionSize = 96 * 1024;

	MemArena_Configure(cfg);
	{
		char passcode[16];
		Parse::ReadNonEmptyLine(passcode);

		const uArrayMap2D rooms{ {}, 4, 4, '.' };
		int32_t answer = LongestPathTo(rooms, Vec2Int{ 0, 0 }, Vec2Int{ 3, 3 }, passcode);

		PuzzleOutput::Submit(2016, 17, 2, answer);
	}
	MemArena_Reset();
}
