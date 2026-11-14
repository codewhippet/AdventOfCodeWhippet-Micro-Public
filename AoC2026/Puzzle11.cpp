#include "stdafx.h"

using namespace std;

static string dummy =
R"()";

namespace Puzzle11_2026_Types
{
}

using namespace Puzzle11_2026_Types;

static void Puzzle11_A(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle11_A: %" PRId64 "\n", answer);
}

static void Puzzle11_B(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle11_B: %" PRId64 "\n", answer);
}

void Puzzle11_A_2026()
{
	Puzzle11_A(R"(z:\AoCInput\2026\Puzzle11.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 11, 1, answer);
}

void Puzzle11_B_2026()
{
	Puzzle11_B(R"(z:\AoCInput\2026\Puzzle11.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 11, 2, answer);
}
