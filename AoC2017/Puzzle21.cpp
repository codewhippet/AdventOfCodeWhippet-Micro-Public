#include "stdafx.h"

using namespace std;

namespace Puzzle21_2017_Types
{
	struct EnhancementRules
	{
		vector<uint16_t> TwoToThree = vector<uint16_t>(1 << (2 * 2));
		vector<uint16_t> ThreeToTwo = vector<uint16_t>(1 << (3 * 3));
	};

	struct ImageBuffer
	{
		vector<uint8_t> Data;
		size_t Stride;
	};
}

using namespace Puzzle21_2017_Types;

template <>
struct std::hash<pair<uint16_t, uint16_t>>
{
	size_t operator()(const pair<uint16_t, uint16_t>& p) const noexcept
	{
		const int shiftDown = (sizeof(std::size_t) * CHAR_BIT >> 2);
		const int shiftUp = (sizeof(std::size_t) * CHAR_BIT) - shiftDown;

		size_t hash = std::hash<uint16_t>{}(p.first);
		hash = std::hash<uint16_t>{}(p.second) ^ (hash >> shiftDown | hash << shiftUp);
		return hash;
	}
};

static uint16_t Rotate2x2(uint16_t input)
{
	// 32 -> 13
	// 10    02
	//
	// 3210 -> 1302

	return
		(input & (1 << 0)) << 1 |
		(input & (1 << 1)) << 2 |
		(input & (1 << 2)) >> 2 |
		(input & (1 << 3)) >> 1;
}

static uint16_t Flip2x2(uint16_t input)
{
	// 32 -> 23
	// 10    01
	//
	// 3210 ->
	// 2301

	return
		(input & ((1 << 0) | (1 << 2))) << 1 |
		(input & ((1 << 1) | (1 << 3))) >> 1;
}

static uint16_t Rotate3x3(uint16_t input)
{
	// 876    258
	// 543 -> 147
	// 210    036
	//
	// 876543210 ->
	// 258147036

	return
		(input & (1 << 0)) << 2 |
		(input & (1 << 1)) << 4 |
		(input & (1 << 2)) << 6 |

		(input & (1 << 3)) >> 2 |
		(input & (1 << 4)) |
		(input & (1 << 5)) << 2 |

		(input & (1 << 6)) >> 6 |
		(input & (1 << 7)) >> 4 |
		(input & (1 << 8)) >> 2;
}

static uint16_t Flip3x3(uint16_t input)
{
	// 876    678
	// 543 -> 345
	// 210    012
	//
	// 876543210 ->
	// 678345012

	return
		(input & ((1 << 0) | (1 << 3) | (1 << 6))) << 2 |
		(input & ((1 << 1) | (1 << 4) | (1 << 7))) |
		(input & ((1 << 2) | (1 << 5) | (1 << 8))) >> 2;
}

static void MakePatternVariations(uint16_t pattern, size_t tileSize, array<uint16_t, 8>* variations)
{
	auto flip = (tileSize == 2) ? &Flip2x2 : &Flip3x3;
	auto rotate = (tileSize == 2) ? &Rotate2x2 : &Rotate3x3;

	for (size_t i = 0; i < 4; i++)
	{
		(*variations)[i] = pattern;
		(*variations)[i + 4] = flip(pattern);
		pattern = rotate(pattern);
	}
}

static EnhancementRules ParseRules()
{
	EnhancementRules rules;

	// Handily:
	// '#' is 0x23 : 0010 0011
	// '.' is 0x2e : 0010 1110
	// So char & 1 gets us whether the bit is on or not

	char line[64];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line);
		if (line[6] == '=')
		{
			uint16_t startingPattern = 0;
			startingPattern |= (line[0] & 1) << 3;
			startingPattern |= (line[1] & 1) << 2;
			startingPattern |= (line[3] & 1) << 1;
			startingPattern |= (line[4] & 1) << 0;

			uint16_t finishingPattern = 0;
			finishingPattern |= (line[ 9] & 1) << 8;
			finishingPattern |= (line[10] & 1) << 7;
			finishingPattern |= (line[11] & 1) << 6;
			finishingPattern |= (line[13] & 1) << 5;
			finishingPattern |= (line[14] & 1) << 4;
			finishingPattern |= (line[15] & 1) << 3;
			finishingPattern |= (line[17] & 1) << 2;
			finishingPattern |= (line[18] & 1) << 1;
			finishingPattern |= (line[19] & 1) << 0;

			array<uint16_t, 8> variations = {};
			MakePatternVariations(startingPattern, 2, &variations);
			for (uint16_t pattern : variations)
			{
				rules.TwoToThree[pattern] = finishingPattern;
			}
		}
		else
		{
			uint16_t startingPattern = 0;
			startingPattern |= (line[ 0] & 1) << 8;
			startingPattern |= (line[ 1] & 1) << 7;
			startingPattern |= (line[ 2] & 1) << 6;
			startingPattern |= (line[ 4] & 1) << 5;
			startingPattern |= (line[ 5] & 1) << 4;
			startingPattern |= (line[ 6] & 1) << 3;
			startingPattern |= (line[ 8] & 1) << 2;
			startingPattern |= (line[ 9] & 1) << 1;
			startingPattern |= (line[10] & 1) << 0;

			uint16_t finishingPattern = 0;
			finishingPattern |= (line[15] & 1) << 15;
			finishingPattern |= (line[16] & 1) << 14;
			finishingPattern |= (line[17] & 1) << 13;
			finishingPattern |= (line[18] & 1) << 12;

			finishingPattern |= (line[20] & 1) << 11;
			finishingPattern |= (line[21] & 1) << 10;
			finishingPattern |= (line[22] & 1) << 9;
			finishingPattern |= (line[23] & 1) << 8;

			finishingPattern |= (line[25] & 1) << 7;
			finishingPattern |= (line[26] & 1) << 6;
			finishingPattern |= (line[27] & 1) << 5;
			finishingPattern |= (line[28] & 1) << 4;

			finishingPattern |= (line[30] & 1) << 3;
			finishingPattern |= (line[31] & 1) << 2;
			finishingPattern |= (line[32] & 1) << 1;
			finishingPattern |= (line[33] & 1) << 0;

			array<uint16_t, 8> variations = {};
			MakePatternVariations(startingPattern, 3, &variations);
			for (uint16_t pattern : variations)
			{
				rules.ThreeToTwo[pattern] = finishingPattern;
			}
		}
	}

	return rules;
}

static uint16_t ExtractSquare(const ImageBuffer& src, size_t squareSize, size_t squareX, size_t squareY)
{
	uint16_t pattern = 0;

	size_t squareOffset = squareSize * (squareY * src.Stride + squareX);
	for (size_t y = 0; y < squareSize; y++)
	{
		const uint8_t* patternData = &src.Data[squareOffset];
		for (size_t x = 0; x < squareSize; x++)
		{
			pattern = (pattern << 1) | (*patternData++ & 1);
		}
		squareOffset += src.Stride;
	}

	return pattern;
}

static void InjectSquare(uint16_t pattern, size_t squareSize, size_t squareX, size_t squareY, ImageBuffer* dst)
{
	uint16_t patternMask = 1 << ((squareSize * squareSize) - 1);

	size_t squareOffset = squareSize * (squareY * dst->Stride + squareX);
	for (size_t y = 0; y < squareSize; y++)
	{
		uint8_t* patternData = &dst->Data[squareOffset];
		for (size_t x = 0; x < squareSize; x++)
		{
			*patternData++ = (pattern & patternMask) ? 1 : 0;
			patternMask >>= 1;
		}
		squareOffset += dst->Stride;
	}
}

static void Enhance(const EnhancementRules& rules, const ImageBuffer& src, ImageBuffer* dst)
{
	size_t oldSquareSize = ((src.Stride & 1) == 0) ? 2 : 3;
	size_t expandedSquareSize = oldSquareSize + 1;
	const vector<uint16_t>& patternLookup = ((src.Stride & 1) == 0) ? rules.TwoToThree : rules.ThreeToTwo;

	size_t sizeInSquares = src.Stride / oldSquareSize;
	size_t newImageSize = sizeInSquares * expandedSquareSize;
	assert(dst->Data.size() >= (newImageSize * newImageSize));
	dst->Stride = newImageSize;

	for (size_t squareY = 0; squareY < sizeInSquares; squareY++)
	{
		for (size_t squareX = 0; squareX < sizeInSquares; squareX++)
		{
			uint16_t oldSquare = ExtractSquare(src, oldSquareSize, squareX, squareY);
			uint16_t newSquare = patternLookup[oldSquare];
			InjectSquare(newSquare, expandedSquareSize, squareX, squareY, dst);
		}
	}
}

static array<uint16_t, 9> EnhanceThreeToNine(const EnhancementRules& rules, uint16_t pattern)
{
	const size_t maximumExpansionSize = 9 * 9;
	array<ImageBuffer, 2> imageBuffers;
	imageBuffers[0].Data.resize(maximumExpansionSize);
	imageBuffers[1].Data.resize(maximumExpansionSize);

	imageBuffers[0].Stride = 3;
	for (size_t i = 0; i < 9; i++)
	{
		imageBuffers[0].Data[9 - i - 1] = pattern & 1;
		pattern >>= 1;
	}

	for (size_t i = 0; i < 3; i++)
	{
		Enhance(rules, imageBuffers[i & 1], &imageBuffers[1 - (i & 1)]);
	}

	array<uint16_t, 9> newPatterns =
	{
		ExtractSquare(imageBuffers[1], 3, 0, 0),
		ExtractSquare(imageBuffers[1], 3, 0, 1),
		ExtractSquare(imageBuffers[1], 3, 0, 2),

		ExtractSquare(imageBuffers[1], 3, 1, 0),
		ExtractSquare(imageBuffers[1], 3, 1, 1),
		ExtractSquare(imageBuffers[1], 3, 1, 2),

		ExtractSquare(imageBuffers[1], 3, 2, 0),
		ExtractSquare(imageBuffers[1], 3, 2, 1),
		ExtractSquare(imageBuffers[1], 3, 2, 2),
	};

	return newPatterns;
}

void Puzzle21_A_2017()
{
	EnhancementRules rules = ParseRules();

	const size_t maximumExpansionSize = 18 * 18;
	array<ImageBuffer, 2> imageBuffers;
	imageBuffers[0].Data.resize(maximumExpansionSize);
	imageBuffers[1].Data.resize(maximumExpansionSize);

	imageBuffers[0].Stride = 3;
	imageBuffers[0].Data[1] = 1;
	imageBuffers[0].Data[5] = 1;
	imageBuffers[0].Data[6] = 1;
	imageBuffers[0].Data[7] = 1;
	imageBuffers[0].Data[8] = 1;

	for (size_t i = 0; i < 5; i++)
	{
		Enhance(rules, imageBuffers[i & 1], &imageBuffers[1 - (i & 1)]);
	}

	int32_t answer = static_cast<int32_t>(ranges::count(imageBuffers[1].Data, 1));
	PuzzleOutput::Submit(2017, 21, 1, answer);
}

void Puzzle21_B_2017()
{
	EnhancementRules rules = ParseRules();

	HashMap<uint16_t, array<uint16_t, 9>> patternExpansionCache(16, numeric_limits<uint16_t>::max());
	HashMap<pair<uint16_t, uint16_t>, uint32_t> solutionCache(64, { numeric_limits<uint16_t>::max(), uint16_t{ 0 } });

	const uint16_t startingPattern = 143;
	const uint16_t enhancements = 18;

	vector<pair<uint16_t, uint16_t>> evaluationStack;
	evaluationStack.reserve(256);
	evaluationStack.push_back({ startingPattern, enhancements });

	while (!evaluationStack.empty())
	{
		const pair<uint16_t, uint16_t> current = evaluationStack.back();

		if (solutionCache.Contains(current))
		{
			evaluationStack.pop_back();
			continue;
		}

		if (current.second == 0)
		{
			solutionCache.Insert(current, popcount(current.first));
			evaluationStack.pop_back();
			continue;
		}

		array<uint16_t, 9> expandsTo;
		if (patternExpansionCache.TryFind(current.first, &expandsTo) == false)
		{
			expandsTo = EnhanceThreeToNine(rules, current.first);
			patternExpansionCache.Insert(current.first, expandsTo);
		}

		uint32_t candidateAnswer = 0;
		for (uint16_t subPattern : expandsTo)
		{
			const pair<uint16_t, uint16_t> subAnswerKey{ subPattern, static_cast<uint16_t>(current.second - 3) };
			uint32_t existingSubAnswer = 0;
			if (solutionCache.TryFind(subAnswerKey, &existingSubAnswer))
			{
				candidateAnswer += existingSubAnswer;
			}
			else
			{
				evaluationStack.push_back(subAnswerKey);
			}
		}

		// Did we find existing solutions for everything?
		if (evaluationStack.back() == current)
		{
			solutionCache.Insert(current, candidateAnswer);
			evaluationStack.pop_back();
		}
	}

	int32_t answer = solutionCache.At({ startingPattern, enhancements });
	PuzzleOutput::Submit(2017, 21, 2, answer);
}
