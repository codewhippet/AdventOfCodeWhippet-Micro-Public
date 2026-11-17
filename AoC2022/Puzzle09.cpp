#include "stdafx.h"

using namespace std;

namespace Puzzle09_2022_Types
{
}

using namespace Puzzle09_2022_Types;

static int32_t SimulateRope(size_t elementCount)
{
	vector<Vec2Short> rope{ elementCount };

	const size_t alphabetSize = 128;
	vector<Vec2Short> directions(alphabetSize);
	directions['U'] = Vec2Short::Up();
	directions['D'] = Vec2Short::Down();
	directions['L'] = Vec2Short::Left();
	directions['R'] = Vec2Short::Right();

	HashSet<Vec2Short> uniquePositions{ 16 * 1024, Vec2Short{ numeric_limits<int16_t>::max(), numeric_limits<int16_t>::max() } };
	while (PuzzleInput::NextLine())
	{
		Vec2Short direction = directions[PuzzleInput::GetChar()];
		int32_t steps = Parse::GetInt16();

		for (int64_t count = 0; count < steps; count++)
		{
			rope[0] += direction;
			for (size_t i = 1; i < rope.size(); i++)
			{
				Vec2Short delta = rope[i - 1] - rope[i];

				// Pure axis movement
				bool moveU = (delta.Y == -2);
				bool moveD = (delta.Y == 2);
				bool moveL = (delta.X == -2);
				bool moveR = (delta.X == 2);

				// Extra movement for diagonals
				bool moveU_LR = (moveL || moveR) && (delta.Y == -1);
				bool moveD_LR = (moveL || moveR) && (delta.Y == 1);
				bool moveL_UD = (moveU || moveD) && (delta.X == -1);
				bool moveR_UD = (moveU || moveD) && (delta.X == 1);

				if (moveR || moveR_UD)
				{
					rope[i] += Vec2Short::Right();
				}
				if (moveL || moveL_UD)
				{
					rope[i] += Vec2Short::Left();
				}
				if (moveU || moveU_LR)
				{
					rope[i] += Vec2Short::Up();
				}
				if (moveD || moveD_LR)
				{
					rope[i] += Vec2Short::Down();
				}
			}

			uniquePositions.Insert(rope.back());
		}
	}

	return uniquePositions.Size();
}

void Puzzle09_A_2022()
{
	int32_t answer = SimulateRope(2);

	PuzzleOutput::Submit(2022, 9, 1, answer);
}

void Puzzle09_B_2022()
{
	int32_t answer = SimulateRope(10);

	PuzzleOutput::Submit(2022, 9, 2, answer);
}
