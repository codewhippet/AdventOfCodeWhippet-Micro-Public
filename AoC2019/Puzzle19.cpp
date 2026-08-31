#include "stdafx.h"
#include "Intputer.h"

using namespace std;

namespace Puzzle19_2019_Types
{
}

using namespace Puzzle19_2019_Types;

static bool IsInside(const Vec2Int& pos, const vector<int32_t>& program, uIntputer<int32_t>* puter)
{
	puter->Reset(program);

	puter->GetReadQueue()->push_back(pos.X);
	puter->GetReadQueue()->push_back(pos.Y);

	auto exec = puter->Execute();
	assert(exec == uIntputerExecutionResult::Finished);
	(void)exec;

	return puter->GetWriteQueue()->front() == 1;
}

void Puzzle19_A_2019()
{
	vector<int32_t> program;
	program.reserve(512);
	while (PuzzleInput::NextLine())
	{
		if (PuzzleInput::PeekChar() == '\n')
			break;

		program.push_back(Parse::GetInt32());
	}
	assert(program.size() < 512);
	program.resize(512);

	uIntputer<int32_t> puter;

	uIntputerIO<int32_t> io;
	puter.SetReadWriteQueues(&io);

	int32_t answer = (int32_t)ranges::count_if(uGridRange{ Vec2Int{}, Vec2Int{ 50, 50 } },
		[&](const auto& p)
		{
			return IsInside(p, program, &puter);
		});

	PuzzleOutput::Submit(2019, 19, 1, answer);
}

void Puzzle19_B_2019()
{
	vector<int32_t> program;
	program.reserve(512);
	while (PuzzleInput::NextLine())
	{
		if (PuzzleInput::PeekChar() == '\n')
			break;

		program.push_back(Parse::GetInt32());
	}
	assert(program.size() < 512);
	program.resize(512);

	uIntputer<int32_t> puter;

	uIntputerIO<int32_t> io;
	puter.SetReadWriteQueues(&io);

	Vec2Int topRight{ 10, 0 };
	while (IsInside(topRight, program, &puter) == false)
		topRight += Vec2Int::Down();

	Vec2Int closest;
	while (true)
	{
		assert(IsInside(topRight, program, &puter));

		Vec2Int bottomLeft = topRight + 99 * (Vec2Int::Left() + Vec2Int::Down());
		if (IsInside(bottomLeft, program, &puter))
		{
			closest = Vec2Int{ bottomLeft.X, topRight.Y };
			break;
		}

		topRight += IsInside(topRight + Vec2Int::Right(), program, &puter) ? Vec2Int::Right() : Vec2Int::Down();
	}

	int32_t answer = closest.X * 10000 + closest.Y;

	PuzzleOutput::Submit(2019, 19, 2, answer);
}
