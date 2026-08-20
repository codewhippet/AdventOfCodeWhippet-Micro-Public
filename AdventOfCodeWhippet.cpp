#include "AoC2015/AoC2015.h"
#include "AoC2016/AoC2016.h"
#include "AoC2017/AoC2017.h"
#include "AoC2018/AoC2018.h"
#include "AoC2019/AoC2019.h"
#include <MicroUtils.h>
#include <string>
#include <vector>
#include <string.h>

#if _WIN32
#include <Windows.h>
#include <crtdbg.h>
#endif

#if PICO_ON_DEVICE
#include <malloc.h>
#endif

#define ENABLE_TIMING 1
#define ENABLE_MEMORY_WATERMARK 1
#define ENABLE_ALLOCATION_CHECK 1

enum class OperatingMode
{
	Auto, // Run through every puzzle in sequence
	Single, // For debugging
	Year, // For debugging
	Client, // Wait to be told what puzzles to run
};

struct Puzzles
{
	const int CalendarYear;
	const size_t* PuzzleCount;
	const std::pair<void(*)(), void(*)()>* PuzzleTable;
};

static const Puzzles PuzzleTables[] =
{
	{ 2015, &PuzzleTableCount_2015, PuzzleTable_2015 },
	{ 2016, &PuzzleTableCount_2016, PuzzleTable_2016 },
	{ 2017, &PuzzleTableCount_2017, PuzzleTable_2017 },
	{ 2018, &PuzzleTableCount_2018, PuzzleTable_2018 },
	{ 2019, &PuzzleTableCount_2019, PuzzleTable_2019 },
};

#if ENABLE_MEMORY_WATERMARK && _WIN32
namespace MemoryWatermark
{
	static int64_t CurrentAlloc;
	static int64_t WatermarkBaseline;
	static int64_t HighWatermark;

	int __cdecl AllocHook(int allocType,
		void* userData,
		size_t newSize,
		int /*blockType*/,
		long /*requestNumber*/,
		const unsigned char* /*filename*/,
		int /*lineNumber*/)
	{
		switch (allocType)
		{
		case _HOOK_ALLOC:
			{
				CurrentAlloc += newSize;
			}
			break;

		case _HOOK_REALLOC:
			{
				size_t oldSize = _msize(userData);
				CurrentAlloc = CurrentAlloc + newSize - oldSize;
			}
			break;

		case _HOOK_FREE:
			{
				size_t oldSize = _msize(userData);
				CurrentAlloc -= oldSize;
			}
			break;
		}

		HighWatermark = std::max(HighWatermark, CurrentAlloc - WatermarkBaseline);

		return TRUE;
	}

	void Init()
	{
		_CrtSetAllocHook(&AllocHook);
	}

	void Start()
	{
		WatermarkBaseline = CurrentAlloc;
		HighWatermark = 0;
	}
}
#endif

static void ExecutePuzzle(int year, int puzzle, int part, void (*puzzleFn)())
{
#if ENABLE_TIMING && _WIN32
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	LARGE_INTEGER start, stop;
	QueryPerformanceCounter(&start);
#endif

#if ENABLE_MEMORY_WATERMARK && _WIN32
	MemoryWatermark::Start();
#endif

#if ENABLE_ALLOCATION_CHECK && PICO_ON_DEVICE
	{
		void* largeAlloc = malloc(200 * 1024);
		if (largeAlloc == nullptr)
		{
			PuzzleOutput::Submit(year, puzzle, part, "[ALLOCATION ERROR]");
			return;
		}
		free(largeAlloc);
	}
#endif

	if (puzzleFn)
	{
		puzzleFn();
	}
	else
	{
		PuzzleOutput::Submit(year, puzzle, part, "[ERROR]");
	}

#if PICO_ON_DEVICE
	// Newlib nano doesn't always recover the final free block in the managed arena and it
	// uses sbrk to try to grab the *full* allocation size, resulting in a sort of
	// 'false fragmentation'. e.g. If malloc has a ~30Kb free block at the end of the
	// arena and the system still has ~170kb of free memory after the arena, an allocation
	// of 200Kb will fail. malloc_trim releases the trailing free block.
	malloc_trim(0);
#endif

#if ENABLE_TIMING && _WIN32
	QueryPerformanceCounter(&stop);
	printf(" Time: %0.3fms\n", (stop.QuadPart - start.QuadPart) / (freq.QuadPart / 1000.0));
#endif

#if ENABLE_MEMORY_WATERMARK && _WIN32
	printf("%s Memory: %lld bytes\n", MemoryWatermark::HighWatermark > 220 * 1024 ? "***********" : "", MemoryWatermark::HighWatermark);
#endif
}

// For IO timing
static void Reflect(int year, int puzzle, int part)
{
	PuzzleInput::DiscardRemaining();
	PuzzleOutput::Submit(year, puzzle + 1, part, static_cast<int32_t>(0));
}

int main(int argc, const char* argv[])
{
	Hardware::Initialise();
	PuzzleInput::InitialiseStdIn();

	OperatingMode mode;
#ifdef PICO_ON_DEVICE
	mode = OperatingMode::Client;
#elif _WIN32
	mode = OperatingMode::Auto;
#else
#	error "Unsupported platform"
#endif

	int selectedYear = -1;
	int selectedPuzzle = -1;
#if !defined(PICO_ON_DEVICE)
	for (int arg = 1; arg < argc; arg++)
	{
		if ((argv[arg] == std::string_view{ "--auto" }) || (argv[arg] == std::string_view{ "-a" }))
		{
			mode = OperatingMode::Auto;
		}

		if ((argv[arg] == std::string_view{ "--client" }) || (argv[arg] == std::string_view{ "-c" }))
		{
			mode = OperatingMode::Client;
		}

		if ((argv[arg] == std::string_view{ "--single" }) || (argv[arg] == std::string_view{ "-s" }))
		{
			selectedYear = atoi(argv[++arg]);
			selectedPuzzle = atoi(argv[++arg]);

			mode = OperatingMode::Single;
		}

		if ((argv[arg] == std::string_view{ "--year" }) || (argv[arg] == std::string_view{ "-y" }))
		{
			selectedYear = atoi(argv[++arg]);

			mode = OperatingMode::Year;
		}
	}
#endif

#if ENABLE_MEMORY_WATERMARK && _WIN32
	MemoryWatermark::Init();
#endif

	switch (mode)
	{
	case OperatingMode::Auto:
		{
			for (size_t year = 0; year < sizeof(PuzzleTables) / sizeof(PuzzleTables[0]); year++)
			{
				for (int i = 0; i < *PuzzleTables[year].PuzzleCount; i++)
				{
					PuzzleInput::FromFile(PuzzleTables[year].CalendarYear, i + 1);
					ExecutePuzzle(PuzzleTables[year].CalendarYear, i + 1, 1, PuzzleTables[year].PuzzleTable[i].first);

					PuzzleInput::FromFile(PuzzleTables[year].CalendarYear, i + 1);
					ExecutePuzzle(PuzzleTables[year].CalendarYear, i + 1, 2, PuzzleTables[year].PuzzleTable[i].second);
				}
			}
		}
		break;

	case OperatingMode::Single:
		{
			for (int i = 0; i < 250; i++)
			{
				for (size_t yearIndex = 0; yearIndex < sizeof(PuzzleTables) / sizeof(PuzzleTables[0]); yearIndex++)
				{
					if (PuzzleTables[yearIndex].CalendarYear == selectedYear)
					{
						PuzzleInput::FromFile(PuzzleTables[yearIndex].CalendarYear, selectedPuzzle);
						ExecutePuzzle(PuzzleTables[yearIndex].CalendarYear, selectedPuzzle, 1, PuzzleTables[yearIndex].PuzzleTable[selectedPuzzle - 1].first);

						PuzzleInput::FromFile(PuzzleTables[yearIndex].CalendarYear, selectedPuzzle);
						ExecutePuzzle(PuzzleTables[yearIndex].CalendarYear, selectedPuzzle, 2, PuzzleTables[yearIndex].PuzzleTable[selectedPuzzle - 1].second);
					}
				}
			}
		}
		break;

	case OperatingMode::Year:
		{
			for (size_t yearIndex = 0; yearIndex < sizeof(PuzzleTables) / sizeof(PuzzleTables[0]); yearIndex++)
			{
				if (PuzzleTables[yearIndex].CalendarYear == selectedYear)
				{
					for (int i = 0; i < *PuzzleTables[yearIndex].PuzzleCount; i++)
					{
						PuzzleInput::FromFile(PuzzleTables[yearIndex].CalendarYear, i + 1);
						ExecutePuzzle(PuzzleTables[yearIndex].CalendarYear, i + 1, 1, PuzzleTables[yearIndex].PuzzleTable[i].first);

						PuzzleInput::FromFile(PuzzleTables[yearIndex].CalendarYear, i + 1);
						ExecutePuzzle(PuzzleTables[yearIndex].CalendarYear, i + 1, 2, PuzzleTables[yearIndex].PuzzleTable[i].second);
					}
				}
			}
		}
		break;

	case OperatingMode::Client:
		{
			while (true)
			{
				int year, puzzle, part, size;
				PuzzleInput::FromStdIn(&year, &puzzle, &part, &size);			
				if (year == -1)
				{
					continue;
				}

				if (part == 2)
				{
					Reflect(year, puzzle, part);
					continue;
				}

				void (*puzzleFn)() = nullptr;
				for (size_t i = 0; i < sizeof(PuzzleTables) / sizeof(PuzzleTables[0]); i++)
				{
					if (PuzzleTables[i].CalendarYear == year)
					{
						if (puzzle < *PuzzleTables[i].PuzzleCount)
						{
							switch (part)
							{
							case 0:
								puzzleFn = PuzzleTables[i].PuzzleTable[puzzle].first;
								break;
							case 1:
								puzzleFn = PuzzleTables[i].PuzzleTable[puzzle].second;
								break;
							}
						}
					}
				}

				ExecutePuzzle(year, puzzle + 1, part + 1, puzzleFn);

				PuzzleInput::DiscardRemaining();
			}
		}
		break;
	}

	Hardware::FlashingStop(3);
}
