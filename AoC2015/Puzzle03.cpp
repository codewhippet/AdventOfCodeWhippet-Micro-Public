#include "stdafx.h"
#include <malloc.h>

using namespace std;

namespace Puzzle03_2015_Types
{
	struct SantaBuffer
	{
		uint32_t* BufferBegin = nullptr;
		uint32_t* BufferEnd = nullptr;

		Vec2Short* WriteHead = nullptr;

		SantaBuffer(size_t maxPoints)
		{
			BufferBegin = new uint32_t[maxPoints];
			BufferEnd = BufferBegin + maxPoints;

			WriteHead = reinterpret_cast<Vec2Short*>(BufferBegin);
		}

		~SantaBuffer()
		{
			delete[] BufferBegin;
		}

		SantaBuffer(const SantaBuffer&) = delete;
		SantaBuffer& operator=(const SantaBuffer&) = delete;

		void AddLocation(Vec2Short pos)
		{
			assert(static_cast<void*>(WriteHead) < static_cast<void*>(BufferEnd));
			*WriteHead++ = pos;
		}

		int32_t CountUnique()
		{
			uint32_t* sortEnd = reinterpret_cast<uint32_t*>(WriteHead);
			sort(BufferBegin, sortEnd);
			uint32_t* uniqueEnd = unique(BufferBegin, sortEnd);
			return (int32_t)distance(BufferBegin, uniqueEnd);
		}
	};
}

using namespace Puzzle03_2015_Types;

static_assert(sizeof(Vec2Short) == sizeof(uint32_t));

void Puzzle03_A_2015()
{
	const size_t expectedInputSize = 8192;
	const size_t workingBufferMaxPoints = expectedInputSize + 1; // Starting point plus one per instruction

	SantaBuffer santa(workingBufferMaxPoints);
	Vec2Short currentPos{};
	santa.AddLocation(currentPos);
	for (int c = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		currentPos = currentPos + Vec2Short::DirectionFromChar((char)c);
		santa.AddLocation(currentPos);
	}
	
	int32_t answer = santa.CountUnique();

	return PuzzleOutput::Submit(2015, 3, 1, answer);
}

void Puzzle03_B_2015()
{
	const size_t expectedInputSize = 8192;
	const size_t workingBufferMaxPoints = expectedInputSize + 1; // Starting point plus one per instruction

	SantaBuffer santas(workingBufferMaxPoints);
	santas.AddLocation({});

	Vec2Short currentPos[2] = { {}, {} };

	size_t santaIndex = 0;
	for (int c = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		currentPos[santaIndex] = currentPos[santaIndex] + Vec2Short::DirectionFromChar((char)c);
		santas.AddLocation(currentPos[santaIndex]);
		santaIndex = 1 - santaIndex;
	}

	int32_t answer = santas.CountUnique();

	return PuzzleOutput::Submit(2015, 3, 2, answer);
}
