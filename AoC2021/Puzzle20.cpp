#include "stdafx.h"

using namespace std;

namespace Puzzle20_2021_Types
{
	struct Puzzle
	{
		vector<uint8_t> IEAString;

		vector<uint8_t> LightPixelBuffer;
		vector<uint8_t> DarkPixelBuffer;

		int32_t Width = 0;
		int32_t Height = 0;
	};
}

using namespace Puzzle20_2021_Types;

static Puzzle ReadPuzzle()
{
	const size_t imageEhancementStringSize = 512;
	const int32_t bufferWidth = 256;
	const int32_t bufferHeight = 256;
	const int32_t bufferSize = bufferWidth * bufferHeight;
	const Vec2Int pixelOffset{ 64, 64 };

	Puzzle p;
	p.Width = bufferWidth;
	p.Height = bufferHeight;

	p.IEAString.resize(imageEhancementStringSize);
	for (size_t i = 0; i < p.IEAString.size(); i++)
	{
		p.IEAString[i] = (PuzzleInput::GetChar() == '#') ? 1 : 0;
	}

	Parse::DiscardExpected("\n\n");

	p.LightPixelBuffer.resize(bufferSize);
	
	ArrayWrapper2D<uint8_t> lightPixels(p.LightPixelBuffer.data(), bufferWidth, bufferHeight);
	Vec2Int pos = pixelOffset;
	for (int c  = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		if (c == '\n')
		{
			pos.X = pixelOffset.X;
			pos.Y++;

		}
		else
		{
			lightPixels(pos) = (c == '#') ? 1 : 0;
			pos.X++;
		}
	}

	p.DarkPixelBuffer.resize(bufferSize);

	return p;
}

static void EnhanceLightToDark(Puzzle* puzzle)
{
	const Vec2Int offsets[] = {
		Vec2Int{ -1, -1 }, Vec2Int{  0, -1 }, Vec2Int{  1, -1 },
		Vec2Int{ -1,  0 }, Vec2Int{  0,  0 }, Vec2Int{  1,  0 },
		Vec2Int{ -1,  1 }, Vec2Int{  0,  1 }, Vec2Int{  1,  1 },
	};

	const ArrayWrapper2D<uint8_t> lightPixels(puzzle->LightPixelBuffer.data(), puzzle->Width, puzzle->Height);

	ArrayWrapper2D<uint8_t> darkPixels(puzzle->DarkPixelBuffer.data(), puzzle->Width, puzzle->Height);
	for (const Vec2Int& pixel : uGridRange<Vec2Int>({ 1, 1 }, { puzzle->Width - 1, puzzle->Height - 1 }))
	{
		uint32_t pattern = 0;
		for (const Vec2Int& offset : offsets)
		{
			pattern = (pattern << 1) | lightPixels(pixel + offset);
		}

		uint8_t newPixel = puzzle->IEAString[pattern];
		darkPixels(pixel) = 1 - newPixel;
	}

	ranges::fill(puzzle->LightPixelBuffer, uint8_t{ 0 });
}

static void EnhanceDarkToLight(Puzzle* puzzle)
{
	const Vec2Int offsets[] = {
		Vec2Int{ -1, -1 }, Vec2Int{  0, -1 }, Vec2Int{  1, -1 },
		Vec2Int{ -1,  0 }, Vec2Int{  0,  0 }, Vec2Int{  1,  0 },
		Vec2Int{ -1,  1 }, Vec2Int{  0,  1 }, Vec2Int{  1,  1 },
	};

	assert(popcount(puzzle->IEAString.size()) == 1);
	const uint32_t patternMask = static_cast<uint32_t>(puzzle->IEAString.size()) - 1;

	const ArrayWrapper2D<uint8_t> darkPixels(puzzle->DarkPixelBuffer.data(), puzzle->Width, puzzle->Height);

	ArrayWrapper2D<uint8_t> lightPixels(puzzle->LightPixelBuffer.data(), puzzle->Width, puzzle->Height);
	for (const Vec2Int& pixel : uGridRange<Vec2Int>({ 1, 1 }, { puzzle->Width - 1, puzzle->Height - 1 }))
	{
		uint32_t pattern = 0;
		for (const Vec2Int& offset : offsets)
		{
			pattern = (pattern << 1) | darkPixels(pixel + offset);
		}
		pattern ^= patternMask;

		uint8_t newPixel = puzzle->IEAString[pattern];
		lightPixels(pixel) = newPixel;
	}

	ranges::fill(puzzle->DarkPixelBuffer, uint8_t{ 0 });
}

void Puzzle20_A_2021()
{
	const size_t enhancementIterations = 2;

	Puzzle p = ReadPuzzle();

	// The *actual* puzzle alternates infinite fields of light/dark
	assert(p.IEAString[0] == 1);
	assert(p.IEAString[511] == 0);

	for (size_t i = 0; i < enhancementIterations; i += 2)
	{
		EnhanceLightToDark(&p);
		EnhanceDarkToLight(&p);
	}

	int32_t answer = static_cast<int32_t>(ranges::count(p.LightPixelBuffer, 1));
	PuzzleOutput::Submit(2021, 20, 1, answer);
}

void Puzzle20_B_2021()
{
	const size_t enhancementIterations = 50;

	Puzzle p = ReadPuzzle();

	// The *actual* puzzle alternates infinite fields of light/dark
	assert(p.IEAString[0] == 1);
	assert(p.IEAString[511] == 0);

	for (size_t i = 0; i < enhancementIterations; i += 2)
	{
		EnhanceLightToDark(&p);
		EnhanceDarkToLight(&p);
	}

	int32_t answer = static_cast<int32_t>(ranges::count(p.LightPixelBuffer, 1));
	PuzzleOutput::Submit(2021, 20, 2, answer);
}
