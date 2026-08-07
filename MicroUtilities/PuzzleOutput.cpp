#include "stdafx.h"
#include "PuzzleOutput.h"
#include <inttypes.h>

void PuzzleOutput::Submit(int year, int puzzle, int part, int32_t answer)
{
	printf("[%d] Puzzle%02d_%c: %d\n\x17", year, puzzle, part == 1 ? 'A' : 'B', answer);
	fflush(stdout);
}

void PuzzleOutput::Submit(int year, int puzzle, int part, int64_t answer)
{
	printf("[%d] Puzzle%02d_%c: %" PRId64 "\n\x17", year, puzzle, part == 1 ? 'A' : 'B', answer);
	fflush(stdout);
}

void PuzzleOutput::Submit(int year, int puzzle, int part, size_t answer)
{
	printf("[%d] Puzzle%02d_%c: %zu\n\x17", year, puzzle, part == 1 ? 'A' : 'B', answer);
	fflush(stdout);
}

void PuzzleOutput::Submit(int year, int puzzle, int part, const char* answer)
{
	printf("[%d] Puzzle%02d_%c: %s\n\x17", year, puzzle, part == 1 ? 'A' : 'B', answer);
	fflush(stdout);
}
