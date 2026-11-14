#include "stdafx.h"

using namespace std;

static string dummy =
R"()";

namespace Puzzle12_2026_Types
{
}

using namespace Puzzle12_2026_Types;

static void Puzzle12_A(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle12_A: %" PRId64 "\n", answer);
}

void Puzzle12_A_2026()
{
	Puzzle12_A(R"(z:\AoCInput\2026\Puzzle12.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 12, 1, answer);
}

void Puzzle12_B_2026()
{
	return PuzzleOutput::Submit(2026, 12, 2, int64_t(-1));
}
