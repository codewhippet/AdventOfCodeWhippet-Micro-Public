#include "stdafx.h"

using namespace std;

namespace Puzzle08_2019_Types
{
	struct FewestTracker
	{
		array<int32_t, 3> Values = { numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max() };

		void Update(const array<int32_t, 3>& values)
		{
			if (values[0] < Values[0])
			{
				Values = values;
			}
		}
	};
}

using namespace Puzzle08_2019_Types;

void Puzzle08_A_2019()
{
	const int32_t width = 25;
	const int32_t height = 6;
	const int32_t layerSize = width * height;

	FewestTracker tracker;
	while (PuzzleInput::PeekChar() != '\n')
	{
		array<int32_t, 3> values = {};
		for (int32_t i = 0; i < layerSize; i++)
		{
			values[PuzzleInput::GetChar() - '0']++;
		}
		tracker.Update(values);
	}

	int64_t answer = tracker.Values[1] * tracker.Values[2];;
	PuzzleOutput::Submit(2019, 8, 1, answer);
}

void Puzzle08_B_2019()
{
	const int32_t width = 25;
	const int32_t height = 6;

	// Make sure the image data is printf printable
	vector<char> line(width + 1, '-');
	line.back() = '\n';

	vector<char> image;
	image.reserve(line.size() + height + 2);
	image.push_back('\n');
	for (int32_t i = 0; i < height; i++)
	{
		ranges::copy(line, back_inserter(image));
	}
	image.push_back('\0');

	char* const startOfRaster = image.data() + 1;
	char* const endOfRaster = startOfRaster + ((width + 1) * height);

	char* writeHead = startOfRaster;
	for (int c = PuzzleInput::GetChar(); c != '\n'; c = PuzzleInput::GetChar())
	{
		if (*writeHead == '-')
		{
			switch (c)
			{
			case '0':
				*writeHead = ' ';
				break;

			case '1':
				*writeHead = '#';
				break;
			}
		}

		writeHead++;

		if (*writeHead == '\n')
		{
			writeHead++;
		}

		if (writeHead == endOfRaster)
		{
			writeHead = startOfRaster;
		}
	}

	PuzzleOutput::Submit(2019, 8, 2, image.data());
}
