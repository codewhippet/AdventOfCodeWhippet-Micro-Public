#include "stdafx.h"

using namespace std;

namespace Puzzle02_2022_Types
{
}

using namespace Puzzle02_2022_Types;

void Puzzle02_A_2022()
{
    // A = rock
    // B = paper
    // C = scissors

    // X = rock
    // Y = paper
    // Z = scissors

    HashMap<string, int32_t> scoring(64, {});

    // I win
    scoring["A Y"] = 6 + 2;
    scoring["B Z"] = 6 + 3;
    scoring["C X"] = 6 + 1;

    // I lose
    scoring["A Z"] = 0 + 3;
    scoring["B X"] = 0 + 1;
    scoring["C Y"] = 0 + 2;

    // I draw
    scoring["A X"] = 3 + 1;
    scoring["B Y"] = 3 + 2;
    scoring["C Z"] = 3 + 3;

    int32_t score = 0;
    while (PuzzleInput::NextLine())
    {
        array<char, 8> line;
        Parse::ReadNonEmptyLine(line.data(), line.size());

        score += scoring.At(line.data());
    }

	int32_t answer = score;
	PuzzleOutput::Submit(2022, 2, 1, answer);
}

void Puzzle02_B_2022()
{
    HashMap<string, int32_t> scoring(64, {});

    // I need to lose
    scoring["A X"] = 3 + 0;
    scoring["B X"] = 1 + 0;
    scoring["C X"] = 2 + 0;

    // I need to draw
    scoring["A Y"] = 1 + 3;
    scoring["B Y"] = 2 + 3;
    scoring["C Y"] = 3 + 3;

    // I need to win
    scoring["A Z"] = 2 + 6;
    scoring["B Z"] = 3 + 6;
    scoring["C Z"] = 1 + 6;

    int32_t score = 0;
    while (PuzzleInput::NextLine())
    {
        array<char, 8> line;
        Parse::ReadNonEmptyLine(line.data(), line.size());

        score += scoring.At(line.data());
    }

	int32_t answer = score;
	PuzzleOutput::Submit(2022, 2, 2, answer);
}
