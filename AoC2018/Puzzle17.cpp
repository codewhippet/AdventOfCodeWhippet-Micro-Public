#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle17_2018_Types
{
	enum class Mode
	{
		TrickleDown,
		FillUp
	};

	class WrappedDisplay
	{
	public:

		WrappedDisplay(size_t width, size_t height)
			: Stride(width)
			, Height(height)
		{
			assert(popcount(height) == 1);
			HeightMask = height - 1;
			Data.resize(height * Stride);
		}

		size_t GetWidth() const
		{
			return Stride;
		}

		size_t GetHeight() const
		{
			return Height;
		}

		void ClearLine(int16_t y)
		{
			size_t offsetY = (y & HeightMask) * Stride;
			memset(Data.data() + offsetY, '.', Stride);
		}

		char* GetLine(int16_t y)
		{
			size_t offsetY = (y & HeightMask) * Stride;
			return Data.data() + offsetY;
		}

		void Set(const Vec2Short& pos, char v)
		{
			size_t offsetY = (pos.Y & HeightMask) * Stride;
			Data[offsetY + pos.X] = v;
		}

	private:
		vector<char> Data;
		size_t Stride;
		size_t Height;
		size_t HeightMask;
	};
}

using namespace Puzzle17_2018_Types;

static vector<pair<Vec2Short, Vec2Short>> ParseScan()
{
	vector<pair<Vec2Short, Vec2Short>> scan;
	scan.reserve(2048 + 256);

	while (PuzzleInput::NextLine())
	{
		bool firstIsX = PuzzleInput::PeekChar() == 'x';
		int16_t a = Parse::GetInt16();
		int16_t b = Parse::GetInt16();
		int16_t c = Parse::GetInt16();
		assert(b < c);

		if (firstIsX)
		{
			scan.push_back({ { a, b }, { a, c } });
		}
		else
		{
			scan.push_back({ { b, a }, { c, a } });
		}
	}

	return scan;
}

static void Rasterise(const vector<pair<Vec2Short, Vec2Short>>& scan, WrappedDisplay* display, int16_t fromY, int16_t toY)
{
	for (int16_t y = fromY; y <= toY; y++)
	{
		display->ClearLine(y);
	}

	for (const auto& s : scan)
	{
		if ((s.first.Y <= toY) && (s.second.Y >= fromY))
		{
			Vec2Short from = s.first;
			from.Y = max(from.Y, fromY);

			Vec2Short to = s.second;
			to.Y = min(to.Y, toY);

			for (const Vec2Short& p : uLineInclusiveRange<Vec2Short>{ from, to })
			{
				display->Set(p, '#');
			}
		}
	}
}

static Mode TrickleDown(int16_t y, WrappedDisplay* display)
{
	char* currentRow = display->GetLine(y);
	char* rowAbove = display->GetLine(y - 1);

	bool hitSurfaceToSpread = false;
	for (size_t i = 0; i < display->GetWidth(); i++)
	{
		switch (rowAbove[i])
		{
		case '|':
			if (currentRow[i] == '.')
			{
				currentRow[i] = '|';
			}
			else if ((currentRow[i] == '#') || (currentRow[i] == '~'))
			{
				rowAbove[i] = '+';
				hitSurfaceToSpread = true;
			}
			break;

		case '+':
		case '~':
			if (currentRow[i] == '.')
			{
				currentRow[i] = '|';
			}
			break;
		}
	}

	return hitSurfaceToSpread ? Mode::FillUp : Mode::TrickleDown;
}

static bool SpreadFoam(char* currentRow, const char* rowBelow, size_t width)
{
	bool modified = false;

	for (size_t x = 0; x + 1 < width; x++)
	{
		if ((currentRow[x] == '+') && (currentRow[x + 1] == '.'))
		{
			if ((rowBelow[x] == '#') || (rowBelow[x] == '~'))
			{
				currentRow[x + 1] = '+';
				modified = true;
			}
		}
	}
	for (size_t x = width - 1; x > 0; x--)
	{
		if ((currentRow[x] == '+') && (currentRow[x - 1] == '.'))
		{
			if ((rowBelow[x] == '#') || (rowBelow[x] == '~'))
			{
				currentRow[x - 1] = '+';
				modified = true;
			}
		}
	}

	return modified;
}

static bool FillContainedWater(char* currentRow, size_t width)
{
	bool modified = false;

	bool halfContainedSurface = false;
	size_t surfaceStart = 0;
	for (size_t x = 0; x + 1 < width; x++)
	{
		if (halfContainedSurface)
		{
			if (currentRow[x] == '#')
			{
				assert(surfaceStart < x);
				memset(&currentRow[surfaceStart], '~', x - surfaceStart);
				halfContainedSurface = false;
				modified = true;
			}
			else if (currentRow[x] == '+')
			{
				// Continue surface
			}
			else
			{
				halfContainedSurface = false;
			}
		}
		else
		{
			if ((currentRow[x] == '#') && (currentRow[x + 1] == '+'))
			{
				surfaceStart = x + 1;
				halfContainedSurface = true;
			}
		}
	}

	return modified;
}

static bool CreateNewFoam(char* rowAbove, const char* currentRow, size_t width)
{
	bool modified = false;

	for (size_t x = 0; x < width; x++)
	{
		if ((currentRow[x] == '~') && (rowAbove[x] == '|'))
		{
			rowAbove[x] = '+';
			modified = true;
		}
	}

	return modified;
}

static Mode FillUp(int16_t y, WrappedDisplay* display)
{
	const size_t width = display->GetWidth();

	char* rowAbove = display->GetLine(y - 1);
	char* currentRow = display->GetLine(y);
	char* rowBelow = display->GetLine(y + 1);

	bool displayUpdated = false;
	displayUpdated |= SpreadFoam(currentRow, rowBelow, width);
	displayUpdated |= FillContainedWater(currentRow, width);
	displayUpdated |= CreateNewFoam(rowAbove, currentRow, width);

	// Go back to trickling down if we make no changes to the display
	return displayUpdated ? Mode::FillUp : Mode::TrickleDown;
}

static pair<int32_t, int32_t> RunWater(const vector<pair<Vec2Short, Vec2Short>>& scan, int16_t sourceX, int16_t minY, int16_t maxY, WrappedDisplay* display)
{
	Mode mode = Mode::TrickleDown;

	const int16_t chunkSize = 16;
	int16_t maxReachedY = chunkSize - 1;
	Rasterise(scan, display, 0, maxReachedY);
	display->Set({ sourceX, 0 }, '|');
	
	vector<pair<uint8_t, uint8_t>> waterCounts(2048);

	int16_t currentY = 1;
	while (true)
	{
		if (currentY > maxY)
			break;

		if (currentY > maxReachedY)
		{
			Rasterise(scan, display, maxReachedY + 1, maxReachedY + chunkSize);
			maxReachedY = maxReachedY + chunkSize;
		}

		switch (mode)
		{
		case Mode::TrickleDown:
			mode = TrickleDown(currentY, display);
			break;

		case Mode::FillUp:
			mode = FillUp(currentY, display);
			break;
		}

		switch (mode)
		{
		case Mode::TrickleDown:
			if ((currentY >= minY) && (currentY <= maxY))
			{
				uint8_t runningWaterBlocks = 0;
				uint8_t staticWaterBlocks = 0;

				const char* line = display->GetLine(currentY);
				const char* end = line + display->GetWidth();
				for (/***/; line != end; line++)
				{
					switch (line[0])
					{
					case '|':
					case '+':
						runningWaterBlocks++;
						break;

					case '~':
						staticWaterBlocks++;
						break;
					}
				}

				waterCounts[currentY] = { runningWaterBlocks, staticWaterBlocks };
			}
			currentY++;
			break;

		case Mode::FillUp:
			currentY--;
			break;
		}
	}

	int32_t totalRunningWaterBlocks = 0;
	int32_t totalStaticWaterBlocks = 0;

	for (const auto& counts : waterCounts)
	{
		totalRunningWaterBlocks += counts.first;
		totalStaticWaterBlocks += counts.second;
	}

	return { totalRunningWaterBlocks, totalStaticWaterBlocks };
}

void Puzzle17_A_2018()
{
	vector<pair<Vec2Short, Vec2Short>> scan = ParseScan();

	MinMaxValues<int16_t> minMaxX;
	MinMaxValues<int16_t> minMaxY;
	ranges::for_each(scan, [&](const auto& p)
		{
			minMaxX.Update(p.first.X);
			minMaxX.Update(p.second.X);

			minMaxY.Update(p.first.Y);
			minMaxY.Update(p.second.Y);
		});

	int16_t offsetX = minMaxX.GetMin() - 2;
	int16_t width = minMaxX.GetMax() - minMaxX.GetMin() + 5;
	ranges::for_each(scan, [&](auto& p)
		{
			p.first.X -= offsetX;
			p.second.X -= offsetX;
		});

	WrappedDisplay display(width, 64);
	auto waterCounts = RunWater(scan, 500 - offsetX, minMaxY.GetMin(), minMaxY.GetMax(), &display);

	int32_t answer = waterCounts.first + waterCounts.second;
	PuzzleOutput::Submit(2018, 17, 1, answer);
}

void Puzzle17_B_2018()
{
	vector<pair<Vec2Short, Vec2Short>> scan = ParseScan();

	MinMaxValues<int16_t> minMaxX;
	MinMaxValues<int16_t> minMaxY;
	ranges::for_each(scan, [&](const auto& p)
		{
			minMaxX.Update(p.first.X);
			minMaxX.Update(p.second.X);

			minMaxY.Update(p.first.Y);
			minMaxY.Update(p.second.Y);
		});

	int16_t offsetX = minMaxX.GetMin() - 2;
	int16_t width = minMaxX.GetMax() - minMaxX.GetMin() + 5;
	ranges::for_each(scan, [&](auto& p)
		{
			p.first.X -= offsetX;
			p.second.X -= offsetX;
		});

	WrappedDisplay display(width, 64);
	auto waterCounts = RunWater(scan, 500 - offsetX, minMaxY.GetMin(), minMaxY.GetMax(), &display);

	int32_t answer = waterCounts.second;
	PuzzleOutput::Submit(2018, 17, 2, answer);
}
