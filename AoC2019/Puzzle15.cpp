#include "stdafx.h"
#include "Intputer.h"

using namespace std;

namespace Puzzle15_2019_Types
{
	struct SearchNode
	{
		Vec2Int Position;
		int32_t Movement;
		int32_t Steps;
		int32_t From;
	};
}

using namespace Puzzle15_2019_Types;

static vector<int32_t> FindNearest(const uArrayMap2D& maze, const Vec2Int& start, char target)
{
	const array<pair<Vec2Int, int32_t>, 4> directions =
	{
		pair<Vec2Int, int32_t>{ Vec2Int::North(), 1 },
		pair<Vec2Int, int32_t>{ Vec2Int::South(), 2 },
		pair<Vec2Int, int32_t>{ Vec2Int::West(), 3 },
		pair<Vec2Int, int32_t>{ Vec2Int::East(), 4 },
	};

	vector<SearchNode> searchQueue;
	searchQueue.reserve(512);
	searchQueue.push_back({ start, -1, 0, -1 });

	uArrayMap2D queued(maze);
	queued(start) = '#';

	for (int32_t searchIndex = 0; searchIndex < (int32_t)searchQueue.size(); searchIndex++)
	{
		SearchNode current = searchQueue[searchIndex];
		if (maze(current.Position) == target)
		{
			vector<int32_t> reconstructedPath;
			reconstructedPath.reserve(current.Steps);
			for (int64_t node = searchIndex; node != 0; node = searchQueue[node].From)
			{
				reconstructedPath.push_back(searchQueue[node].Movement);
			}
			ranges::reverse(reconstructedPath);
			assert(reconstructedPath.capacity() <= current.Steps);
			return reconstructedPath;
		}

		for (const auto& dir : directions)
		{
			Vec2Int nextPos = current.Position + dir.first;
			if (maze(nextPos) != '#' && queued(nextPos) != '#')
			{
				searchQueue.push_back({ nextPos, dir.second, current.Steps + 1, searchIndex });
				queued(nextPos) = '#';
			}
		}
	}

	return {};
}

static Vec2Int ExploreMaze(uArrayMap2D* maze)
{
	uIntputer<int32_t> puter(1024 + 512);

	deque<int32_t> in;
	deque<int32_t> out;
	puter.SetReadWriteQueues(&in, &out);

	const array<Vec2Int, 5> directionCodes =
	{
		Vec2Int{},
		Vec2Int::North(),
		Vec2Int::South(),
		Vec2Int::West(),
		Vec2Int::East(),
	};

	(*maze)({}) = '.';

	Vec2Int oxygenBottle;

	Vec2Int droidPos;
	while (true)
	{
		vector<int32_t> movements = FindNearest(*maze, droidPos, '?');
		if (movements.empty())
			break;

		ranges::for_each(movements | views::take(movements.size() - 1),
			[&](int32_t move)
			{
				in.push_back(move);
				auto exec = puter.Execute();
				(void)exec;
				assert(exec == uIntputerExecutionResult::PendingIo);
				assert(out.size() == 1);
				assert(out[0] == 1);
				out.clear();

				droidPos = droidPos + directionCodes[move];
			});

		in.push_back(movements.back());
		auto exec = puter.Execute();
		assert(exec == uIntputerExecutionResult::PendingIo);
		(void)exec;
		assert(out.size() == 1);

		Vec2Int expectedNextDroidPos = droidPos + directionCodes[movements.back()];
		switch (out[0])
		{
		case 0:
			(*maze)(expectedNextDroidPos) = '#';
			break;
		case 1:
			(*maze)(expectedNextDroidPos) = '.';
			droidPos = expectedNextDroidPos;
			break;
		case 2:
			(*maze)(expectedNextDroidPos) = 'O';
			droidPos = expectedNextDroidPos;
			oxygenBottle = droidPos;
			break;
		}
		out.clear();
	}

	return oxygenBottle;
}

static int32_t TimeToFill(const uArrayMap2D& maze, const Vec2Int& start)
{
	vector<SearchNode> searchQueue;
	searchQueue.reserve(512);
	searchQueue.push_back({ start, -1, 0, -1 });

	uArrayMap2D queued(maze);
	queued(start) = '#';

	for (size_t searchIndex = 0; searchIndex < searchQueue.size(); searchIndex++)
	{
		SearchNode current = searchQueue[searchIndex];
		for (const auto& dir : Vec2Int::CardinalDirections())
		{
			Vec2Int nextPos = current.Position + dir;
			if (maze(nextPos) != '#' && queued(nextPos) != '#')
			{
				searchQueue.push_back(SearchNode{ nextPos, -1, current.Steps + 1 });
				queued(nextPos) = '#';
			}
		}
	}

	return searchQueue.back().Steps;
}

void Puzzle15_A_2019()
{
	const int32_t mazeWidth = 64;
	const int32_t mazeHeight = 64;

	uArrayMap2D maze(CreateArrayMap2DAllocator_Heap(), Vec2Int{ -32, -32 }, mazeWidth, mazeHeight, '?');

	ExploreMaze(&maze);

	auto shortestRoute = FindNearest(maze, {}, 'O');
	int32_t answer = static_cast<int32_t>(shortestRoute.size());

	PuzzleOutput::Submit(2019, 15, 1, answer);
}

void Puzzle15_B_2019()
{
	const int32_t mazeWidth = 64;
	const int32_t mazeHeight = 64;

	uArrayMap2D maze(CreateArrayMap2DAllocator_Heap(), Vec2Int{ -32, -32 }, mazeWidth, mazeHeight, '?');

	Vec2Int oxygenBottle = ExploreMaze(&maze);
	int32_t answer = TimeToFill(maze, oxygenBottle);

	PuzzleOutput::Submit(2019, 15, 2, answer);
}
