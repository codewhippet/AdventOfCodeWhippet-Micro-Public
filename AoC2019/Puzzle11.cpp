#include "stdafx.h"
#include "Intputer.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle11_2019_Types
{
}

using namespace Puzzle11_2019_Types;

void Puzzle11_A_2019()
{
	uIntputer<int32_t> puter(1024 + 512);

	deque<int32_t> in;
	deque<int32_t> out;
	puter.SetReadWriteQueues(&in, &out);

	assert(puter.Execute() == uIntputerExecutionResult::PendingIo);

	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 17 * 1024;
	MemArena_Configure(cfg);
	{
		uArrayMap2D area(Vec2Int{ -48, -48 }, 128, 128, ' ');

		Vec2Int robotPos;
		Vec2Int robotDir = Vec2Int::Up();
		HashSet<Vec2Int> paintedPanels(4 * 1024, Vec2Int::Min());
		while (true)
		{
			in.push_back(area(robotPos) == '#');
			auto exec = puter.Execute();
			if (exec == uIntputerExecutionResult::Finished)
				break;

			assert(exec == uIntputerExecutionResult::PendingIo);
			assert(in.size() == 0);
			assert(out.size() == 2);

			assert(area.IsInside(robotPos));

			area(robotPos) = (out[0] == 0 ? ' ' : '#');
			paintedPanels.Insert(robotPos);

			if (out[1] == 0)
			{
				robotDir = Vec2Int::RotateAnticlockwise(robotDir);
			}
			else
			{
				robotDir = Vec2Int::RotateClockwise(robotDir);
			}

			out.clear();

			robotPos = robotPos + robotDir;
		}

		int32_t answer = paintedPanels.Size();
		PuzzleOutput::Submit(2019, 11, 1, answer);
	}
	MemArena_Reset();
}

void Puzzle11_B_2019()
{
	uIntputer<int64_t> puter(1024);

	deque<int64_t> in;
	deque<int64_t> out;
	puter.SetReadWriteQueues(&in, &out);

	assert(puter.Execute() == uIntputerExecutionResult::PendingIo);

	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 17 * 1024;
	MemArena_Configure(cfg);
	{
		uArrayMap2D area(Vec2Int{ -48, -48 }, 128, 128, ' ');
		area({}) = '#';

		Vec2Int robotPos;
		Vec2Int robotDir = Vec2Int::Up();
		while (true)
		{
			in.push_back(area(robotPos) == '#');
			auto exec = puter.Execute();
			if (exec == uIntputerExecutionResult::Finished)
				break;

			assert(exec == uIntputerExecutionResult::PendingIo);
			assert(in.size() == 0);
			assert(out.size() == 2);

			assert(area.IsInside(robotPos));

			area(robotPos) = (out[0] == 0 ? ' ' : '#');

			if (out[1] == 0)
			{
				robotDir = Vec2Int::RotateAnticlockwise(robotDir);
			}
			else
			{
				robotDir = Vec2Int::RotateClockwise(robotDir);
			}

			out.clear();

			robotPos = robotPos + robotDir;
		}

		const int32_t width = 50;
		const int32_t height = 6;

		string answer;
		answer.reserve(width * height);

		for (int32_t y = 0; y < height; y++)
		{
			answer += '\n';
			for (int32_t x = 0; x < width; x++)
			{
				answer += area({ x, y });
			}
		}

		PuzzleOutput::Submit(2019, 11, 2, answer.c_str());
	}
	MemArena_Reset();
}
