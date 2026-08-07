#include "stdafx.h"

using namespace std;

namespace Puzzle11_2016_Types
{
	enum : size_t
	{
		Generator = 0,
		Microchip = 1,
	};

	template <size_t NUM_ITEMS>
	struct UncompressedState
	{
		array<uint8_t, NUM_ITEMS> Items = {};
		uint8_t Elevator = 0;
	};

	using CompressedState = uint32_t;
}

using namespace Puzzle11_2016_Types;

template <size_t NUM_TYPES>
static void ReadStartingState(UncompressedState<NUM_TYPES>* state)
{
	map<string, uint8_t> itemIds;

	const regex microchips(R"(([a-z]+)-compatible microchip)");
	const regex generators(R"(([a-z]+) generator)");

	array<char, 192> line;

	uint8_t floor = 0;
	while (PuzzleInput::NextLine())
	{
		int32_t lineSize = Parse::ReadLine(line.data(), line.size());

		for (cregex_iterator generatorIt(line.data(), line.data() + lineSize, generators); generatorIt != cregex_iterator{}; ++generatorIt)
		{
			auto existingId = itemIds.find((*generatorIt)[1].str());
			if (existingId == itemIds.end())
			{
				existingId = itemIds.insert({ (*generatorIt)[1].str(), (uint8_t)itemIds.size() }).first;
			}
			state->Items[Generator + (existingId->second * 2)] = floor;
		}
		for (cregex_iterator microchipIt(line.data(), line.data() + lineSize, microchips); microchipIt != cregex_iterator{}; ++microchipIt)
		{
			auto existingId = itemIds.find((*microchipIt)[1].str());
			if (existingId == itemIds.end())
			{
				existingId = itemIds.insert({ (*microchipIt)[1].str(), (uint8_t)itemIds.size() }).first;
			}
			state->Items[Microchip + (existingId->second * 2)] = floor;
		}

		floor++;
	}

	state->Elevator = 0;
}

template <size_t NUM_ITEMS>
static CompressedState Compress(const UncompressedState<NUM_ITEMS>& uncompressed)
{
	array<pair<uint8_t, uint8_t>, NUM_ITEMS / 2> items;
	for (int32_t i = 0; i < (NUM_ITEMS / 2); i++)
	{
		items[i] = { uncompressed.Items[Generator + (i * 2)], uncompressed.Items[Microchip + (i * 2)] };
	}
	ranges::sort(items);

	CompressedState compressed = 0;
	for (int32_t i = 0; i < (NUM_ITEMS / 2); i++)
	{
		compressed = (compressed << 2) | (items[i].first & 0x3);
		compressed = (compressed << 2) | (items[i].second & 0x3);
	}
	compressed = (compressed << 2) | (uncompressed.Elevator & 0x3);

	return compressed;
}

template <size_t NUM_ITEMS>
static UncompressedState<NUM_ITEMS> Uncompress(const CompressedState& compressed)
{
	uint32_t c = compressed;

	UncompressedState<NUM_ITEMS> uncompressed;
	uncompressed.Elevator = c & 0x3;
	c >>= 2;

	for (int32_t i = 0; i < NUM_ITEMS; i += 2)
	{
		uncompressed.Items[Microchip + i] = (c >> 0) & 0x3;
		uncompressed.Items[Generator + i] = (c >> 2) & 0x3;
		c >>= 4;
	}

	return uncompressed;
}

template <size_t NUM_ITEMS>
static bool IsStateValid(const UncompressedState<NUM_ITEMS>& state)
{
	uint32_t unconnectedChips = 0;
	uint32_t generators = 0;
	for (int32_t i = 0; i < NUM_ITEMS; i += 2)
	{
		if (state.Items[Microchip + i] != state.Items[Generator + i])
		{
			unconnectedChips |= 1 << state.Items[Microchip + i];
		}
		generators |= 1 << state.Items[Generator + i];
	}

	// Is there a generator on the same floor as an unshielded chip?
	return (unconnectedChips & generators) == 0;
}

template <size_t NUM_ITEMS>
static size_t GetSameFloorItems(const UncompressedState<NUM_ITEMS>& state, int8_t floor, size_t* items)
{
	size_t numItems = 0;
	for (size_t i = 0; i < NUM_ITEMS; i++)
	{
		if (state.Items[i] == floor)
		{
			*items++ = i;
			numItems++;
		}
	}
	return numItems;
}

template <size_t NUM_ITEMS>
static size_t GetNextStates(const CompressedState& compState, CompressedState* states, vector<UncompressedState<NUM_ITEMS>>* workingBuffer)
{
	UncompressedState<NUM_ITEMS> state = Uncompress<NUM_ITEMS>(compState);

	vector<UncompressedState<NUM_ITEMS>>& nextBuffer = *workingBuffer;
	size_t nextBufferCount = 0;

	auto moveFromTo = [&](uint8_t from, uint8_t to)
		{
			size_t items[NUM_ITEMS];
			size_t itemCount = GetSameFloorItems(state, from, &items[0]);
			assert(itemCount > 0);

			// Take up to 2 items with you		
			for (size_t i = 0; i < itemCount; i++)
			{
				nextBuffer[nextBufferCount] = state;
				assert(nextBuffer[nextBufferCount].Items[items[i]] == from);
				nextBuffer[nextBufferCount].Items[items[i]] = to;
				nextBuffer[nextBufferCount].Elevator = to;
				nextBufferCount++;

				for (size_t j = i + 1; j < itemCount; j++)
				{
					nextBuffer[nextBufferCount] = state;
					assert(nextBuffer[nextBufferCount].Items[items[i]] == from);
					nextBuffer[nextBufferCount].Items[items[i]] = to;
					assert(nextBuffer[nextBufferCount].Items[items[j]] == from);
					nextBuffer[nextBufferCount].Items[items[j]] = to;
					nextBuffer[nextBufferCount].Elevator = to;
					nextBufferCount++;
				}
			}
		};

	// Go up
	if (state.Elevator < 3)
	{
		moveFromTo(state.Elevator, state.Elevator + 1);
	}

	// Go down
	if (state.Elevator > 0)
	{
		moveFromTo(state.Elevator, state.Elevator - 1);
	}

	// Compress states and compact to unique equivalent states
	assert(nextBufferCount > 0);
	assert(nextBufferCount <= nextBuffer.size());
	CompressedState* dest = states;
	for (size_t i = 0; i < nextBufferCount; i++)
	{
		if (IsStateValid(nextBuffer[i]))
		{
			*dest++ = Compress(nextBuffer[i]);
		}
	}

	sort(states, dest);
	CompressedState* newEnd = unique(states, dest);
	size_t numUniqueStates = newEnd - states;
	return numUniqueStates;
}

static bool IsAlreadyQueued(const CompressedState& state, const vector<CompressedState>& searchQueue, size_t sortedUpTo)
{
	if (sortedUpTo < 128) // Don't bother binary searching below a certain threashold
	{
		return ranges::any_of(searchQueue, [&](const auto& s) { return s == state; });
	}

	bool foundInSortedSection = binary_search(searchQueue.begin(), searchQueue.begin() + sortedUpTo, state);
	if (foundInSortedSection)
	{
		return true;
	}

	return ranges::any_of(searchQueue.begin() + sortedUpTo, searchQueue.end(), [&](const auto& s) { return s == state; });
}

template <size_t NUM_ITEMS>
static int32_t ShortestPathToTargetFloor(const UncompressedState<NUM_ITEMS>& start, const UncompressedState<NUM_ITEMS>& finish)
{
	CompressedState goal = Compress(finish);

	vector<CompressedState> searchQueue;
	searchQueue.reserve(16 * 1024);
	searchQueue.push_back({ Compress(start) });
	size_t queueSortedEnd = 1;

	int32_t currentSteps = 0;
	vector<size_t> stepCountsStartAt(64, numeric_limits<size_t>::max());
	stepCountsStartAt[0] = 0;
	stepCountsStartAt[1] = 1;

	const size_t maximumNextStates = 210; // (Up + Down) * (14 choose 1 + 14 choose 2)
	vector<UncompressedState<NUM_ITEMS>> nextStateWorkingBuffer(maximumNextStates);
	vector<CompressedState> nextStateBuffer(maximumNextStates);
	for (size_t searchIndex = 0; searchIndex < searchQueue.size(); searchIndex++)
	{
		if (searchIndex == stepCountsStartAt[currentSteps + 1])
		{
			currentSteps++;
			sort(searchQueue.begin(), searchQueue.begin() + searchIndex - 1);
			queueSortedEnd = searchIndex - 1;
		}

		CompressedState currentState = searchQueue[searchIndex];
		if (currentState == goal)
		{
			return currentSteps;
		}

		size_t numNextStates = GetNextStates(currentState, &nextStateBuffer[0], &nextStateWorkingBuffer);
		for (size_t i = 0; i < numNextStates; i++)
		{
			bool alreadyQueued = IsAlreadyQueued(nextStateBuffer[i], searchQueue, queueSortedEnd);
			if (!alreadyQueued)
			{
				if (searchQueue.size() < stepCountsStartAt[currentSteps + 1])
				{
					stepCountsStartAt[currentSteps + 1] = searchQueue.size();
				}
				searchQueue.push_back({ nextStateBuffer[i] });
			}
		}
	}

	return -1;
}

void Puzzle11_A_2016()
{
	const size_t NUM_TYPES = 5;

	UncompressedState<NUM_TYPES * 2> start;
	ranges::fill(start.Items, uint8_t(0));
	ReadStartingState(&start);

	UncompressedState<NUM_TYPES * 2> finish;
	finish.Elevator = 3;
	ranges::fill(finish.Items, uint8_t(3));

	int32_t answer = ShortestPathToTargetFloor(start, finish);
	return PuzzleOutput::Submit(2016, 11, 1, answer);
}

void Puzzle11_B_2016()
{
	const size_t NUM_TYPES = 7;

	UncompressedState<NUM_TYPES * 2> start;
	ranges::fill(start.Items, uint8_t(0));
	ReadStartingState(&start);

	UncompressedState<NUM_TYPES * 2> finish;
	finish.Elevator = 3;
	ranges::fill(finish.Items, uint8_t(3));

	int32_t answer = ShortestPathToTargetFloor(start, finish);
	return PuzzleOutput::Submit(2016, 11, 2, answer);
}
