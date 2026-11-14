#include "stdafx.h"

using namespace std;

static string dummy =
R"()";

namespace Puzzle06_2026_Types
{
}

using namespace Puzzle06_2026_Types;

static void Puzzle06_A(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle06_A: %" PRId64 "\n", answer);
}

static void Puzzle06_B(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle06_B: %" PRId64 "\n", answer);
}

void Puzzle06_A_2026()
{
	Puzzle06_A(R"(z:\AoCInput\2026\Puzzle06.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 6, 1, answer);
}

void Puzzle06_B_2026()
{
	Puzzle06_B(R"(z:\AoCInput\2026\Puzzle06.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 6, 2, answer);
}
