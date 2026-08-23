#include "stdafx.h"
#include "Intputer.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle13_2019_Types
{
}

using namespace Puzzle13_2019_Types;

void Puzzle13_A_2019()
{
	uIntputer<int32_t> puter(2048 + 512);

	deque<int32_t> in;
	deque<int32_t> out;
	puter.SetReadWriteQueues(&in, &out);

	auto exec = puter.Execute();
	assert(exec == uIntputerExecutionResult::Finished);
	(void)exec;

	int32_t answer = 0;
	for (size_t i = 0; (i + 2) < out.size(); i += 3)
	{
		if (out[i + 2] == 2)
		{
			answer++;
		}
	}

	PuzzleOutput::Submit(2019, 13, 1, answer);
}

void Puzzle13_B_2019()
{
	uIntputer<int32_t> puter(2048 + 512);
	puter.Poke(0, 2);

	deque<int32_t> in;
	deque<int32_t> out;
	puter.SetReadWriteQueues(&in, &out);

	int32_t answer = 0;

	HashSet<Vec2Int> blocks(1024, Vec2Int::Min());
	while (true)
	{
		puter.Execute();
		assert(in.empty());
		assert(!out.empty());

		Vec2Int ballLocation;
		Vec2Int paddleLocation;
		for (size_t i = 0; (i + 2) < out.size(); i += 3)
		{
			Vec2Int screenLocation{ out[i + 0], out[i + 1] };
			if (screenLocation == Vec2Int{ -1, 0 })
			{
				answer = out[i + 2];
			}
			else
			{
				switch (out[i + 2])
				{
				case 0:
					blocks.Erase(screenLocation);
					break;

				case 2:
					blocks.Insert(screenLocation);
					break;

				case 3:
					assert(paddleLocation == Vec2Int{});
					paddleLocation = screenLocation;
					break;

				case 4:
					ballLocation = screenLocation;
					break;
				}
			}
		}
		out.clear();

		if (blocks.Size() == 0)
		{
			break;
		}

		if (ballLocation.X < paddleLocation.X)
		{
			in.push_back(-1);
		}
		else if (ballLocation.X > paddleLocation.X)
		{
			in.push_back(1);
		}
		else
		{
			in.push_back(0);
		}
	}

	PuzzleOutput::Submit(2019, 13, 2, answer);
}
