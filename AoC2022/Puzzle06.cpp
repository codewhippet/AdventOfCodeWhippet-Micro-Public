#include "stdafx.h"

using namespace std;

namespace Puzzle06_2022_Types
{
}

using namespace Puzzle06_2022_Types;

static int32_t FindMarker(int32_t requiredCount)
{
	const size_t windowWidth = 16;
	const size_t windowMask = windowWidth - 1;
	assert(requiredCount < windowWidth);

	vector<char> seen(windowWidth);
	memset(seen.data(), PuzzleInput::PeekChar(), seen.size());

	vector<vector<bool>> dp(requiredCount, vector<bool>(windowWidth, false));
	for (size_t messageIndex = 0; PuzzleInput::PeekChar() != EOF; messageIndex++)
	{
		const size_t currentWindowIndex = messageIndex & windowMask;
		const size_t prevWindowIndex = (currentWindowIndex + windowWidth - 1) & windowMask;;

		seen[currentWindowIndex] = static_cast<char>(PuzzleInput::GetChar());

		dp[0][currentWindowIndex] = true;
		for (int32_t i = 1; i < requiredCount; i++)
		{
			bool aDisjoint = dp[i - 1][currentWindowIndex];
			bool bDisjoint = dp[i - 1][prevWindowIndex];

			const size_t checkHistoryWindowIndex = (currentWindowIndex + windowWidth - i) & windowMask;
			bool newCharUnequal = (seen[currentWindowIndex] != seen[checkHistoryWindowIndex]);

			dp[i][currentWindowIndex] = (aDisjoint && bDisjoint && newCharUnequal);
		}
		if (dp[requiredCount - 1][currentWindowIndex])
		{
			return static_cast<int32_t>(messageIndex + 1);
		}
	}

	return -1;
}

void Puzzle06_A_2022()
{
	int32_t answer = FindMarker(4);
	PuzzleOutput::Submit(2022, 6, 1, answer);
}

void Puzzle06_B_2022()
{
	int32_t answer = FindMarker(14);
	PuzzleOutput::Submit(2022, 6, 2, answer);
}
