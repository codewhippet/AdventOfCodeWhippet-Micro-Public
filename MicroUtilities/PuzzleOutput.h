#pragma once
#include <stdint.h>

namespace PuzzleOutput
{
	void Submit(int year, int puzzle, int part, int32_t answer);
	void Submit(int year, int puzzle, int part, int64_t answer);
	void Submit(int year, int puzzle, int part, size_t answer);
	void Submit(int year, int puzzle, int part, const char* answer);
}
