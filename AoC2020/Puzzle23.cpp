#include "stdafx.h"

using namespace std;

namespace Puzzle23_2020_Types
{
	struct Cup
	{
		int32_t Next;
	};
}

using namespace Puzzle23_2020_Types;

static vector<Cup> MakeCupCircle(const char* input)
{
	string order = input;

	vector<Cup> cups;
	cups.resize(order.size());

	for (size_t i = 0; i < order.size(); i++)
	{
		int32_t currentCup = order[i] - '0';
		int32_t nextCup = order[(i + 1) % order.size()] - '0';

		cups[currentCup - 1].Next = nextCup - 1;
	}

	return cups;
}

static Cup* MakeLargeCupCircle(const char* input, int32_t cupCircleSize)
{
	string order = input;

	Cup* cups = static_cast<Cup*>(Hardware::PsramBase());

	for (int32_t i = 0; i < cupCircleSize; i++)
	{
		//cups[i].DbgId = (int32_t)i + 1;
		cups[i].Next = i + 1;
	}

	cups[cupCircleSize - 1].Next = order.front() - '0' - 1;

	for (size_t i = 0; (i + 1) < order.size(); i++)
	{
		int32_t currentCup = order[i] - '0';
		int32_t nextCup = order[i + 1] - '0';

		cups[currentCup - 1].Next = nextCup - 1;
	}

	cups[order.back() - '0' - 1].Next = static_cast<int32_t>(order.size());

	return cups;
}

#if 0
void PrintCupCircle(const Cup* cups, const Cup* start)
{
	size_t index = start - &cups[0];
	printf("%zu", index + 1);
	for (size_t c = cups[index].Next; c != index; c = cups[c].Next)
	{
		printf(" %zu", c + 1);
	}
	printf("\n");
}
#endif

static Cup* PickUpNext(Cup* cups, Cup* c)
{
	Cup* pickUp = &cups[c->Next];
	c->Next = pickUp->Next;
	pickUp->Next = -1;
	return pickUp;
}

static bool IsPickedUp(const Cup& c)
{
	return c.Next == -1;
}

static void InsertAfter(const Cup* cups, Cup* cupToInsert, Cup* cupToInsertAfter)
{
	cupToInsert->Next = cupToInsertAfter->Next;
	cupToInsertAfter->Next = static_cast<int32_t>(cupToInsert - &cups[0]);
}

void Puzzle23_A_2020()
{
	const size_t rounds = 100;

	char startingArrangement[16];
	Parse::ReadNonEmptyLine(startingArrangement, sizeof(startingArrangement));

	vector<Cup> cupCircle = MakeCupCircle(&startingArrangement[0]);
	const size_t numberOfCups = cupCircle.size();
	Cup* currentCup = &cupCircle[startingArrangement[0] - '0' - 1];

	for (size_t i = 0; i < rounds; i++)
	{
		//PrintCupCircle(cupCircle, currentCup);

		// The crab picks up the three cups that are immediately clockwise of the current cup.
		// They are removed from the circle; cup spacing is adjusted as necessary to maintain the circle.
		Cup* pickup[3];
		for (int p = 0; p < 3; p++)
		{
			pickup[p] = PickUpNext(&cupCircle[0], currentCup);
		}

		// The crab selects a destination cup: the cup with a label equal to the current cup's label minus one.
		// If this would select one of the cups that was just picked up, the crab will keep subtracting one
		// until it finds a cup that wasn't just picked up.
		// If at any point in this process the value goes below the lowest value on any cup's label,
		// it wraps around to the highest value on any cup's label instead.
		size_t currentCupIndex = currentCup - &cupCircle[0];
		size_t destinationCupIndex = (currentCupIndex + numberOfCups - 1) % numberOfCups;
		while (IsPickedUp(cupCircle[destinationCupIndex]))
		{
			destinationCupIndex = (destinationCupIndex + numberOfCups - 1) % numberOfCups;
		}

		Cup* destinationCup = &cupCircle[destinationCupIndex];

		// The crab places the cups it just picked up so that they are immediately clockwise
		// of the destination cup. They keep the same order as when they were picked up.
		for (int p = 3; p > 0; p--)
		{
			InsertAfter(&cupCircle[0], pickup[p - 1], destinationCup);
		}

		// The crab selects a new current cup: the cup which is immediately clockwise of the current cup.
		currentCup = &cupCircle[currentCup->Next];
	}

	//PrintCupCircle(cupCircle, currentCup);

	int64_t answer = 0;
	for (int32_t c = cupCircle[0].Next; c != 0; c = cupCircle[c].Next)
	{
		answer = (answer * 10) + (c + 1);
	}

	PuzzleOutput::Submit(2020, 23, 1, answer);
}

void Puzzle23_B_2020()
{
	const size_t psramNeeded = 4 * 1024 * 1024;
	if (Hardware::PsramSize() < psramNeeded)
	{
		PuzzleOutput::Unsupported(2020, 23, 2);
		return;
	}

	const size_t cupCircleSize = 1000000;
	const size_t rounds = 10000000;

	char startingArrangement[16];
	Parse::ReadNonEmptyLine(startingArrangement, sizeof(startingArrangement));

	Cup* cupCircle = MakeLargeCupCircle(&startingArrangement[0], cupCircleSize);
	const size_t numberOfCups = cupCircleSize;
	Cup* currentCup = &cupCircle[startingArrangement[0] - '0' - 1];

	for (size_t i = 0; i < rounds; i++)
	{
		//PrintCupCircle(cupCircle, currentCup);

		// The crab picks up the three cups that are immediately clockwise of the current cup.
		// They are removed from the circle; cup spacing is adjusted as necessary to maintain the circle.
		Cup* pickup[3];
		for (int p = 0; p < 3; p++)
		{
			pickup[p] = PickUpNext(cupCircle, currentCup);
		}

		// The crab selects a destination cup: the cup with a label equal to the current cup's label minus one.
		// If this would select one of the cups that was just picked up, the crab will keep subtracting one
		// until it finds a cup that wasn't just picked up.
		// If at any point in this process the value goes below the lowest value on any cup's label,
		// it wraps around to the highest value on any cup's label instead.
		size_t currentCupIndex = currentCup - &cupCircle[0];
		size_t destinationCupIndex = (currentCupIndex + numberOfCups - 1) % numberOfCups;
		while (IsPickedUp(cupCircle[destinationCupIndex]))
		{
			destinationCupIndex = (destinationCupIndex + numberOfCups - 1) % numberOfCups;
		}

		Cup* destinationCup = &cupCircle[destinationCupIndex];

		// The crab places the cups it just picked up so that they are immediately clockwise
		// of the destination cup. They keep the same order as when they were picked up.
		for (int p = 3; p > 0; p--)
		{
			InsertAfter(cupCircle, pickup[p - 1], destinationCup);
		}

		// The crab selects a new current cup: the cup which is immediately clockwise of the current cup.
		currentCup = &cupCircle[currentCup->Next];
	}

	int64_t answer = static_cast<int64_t>(cupCircle[0].Next + 1) * (cupCircle[cupCircle[0].Next].Next + 1);

	PuzzleOutput::Submit(2020, 23, 2, answer);
}
