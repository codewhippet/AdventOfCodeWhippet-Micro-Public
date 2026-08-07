#include "stdafx.h"

using namespace std;

namespace Puzzle13_2015_Types
{
	struct Party
	{
		set<size_t> People;
		vector<int32_t> Happiness = vector<int32_t>(26 * 26);

		void SetHappiness(size_t from, size_t to, int32_t happiness)
		{
			Happiness[(from * 26) + to] = happiness;
		}

		int32_t GetHappiness(size_t from, size_t to) const
		{
			return Happiness[(from * 26) + to];
		}
	};
}

using namespace Puzzle13_2015_Types;

static Party ParseParty()
{
	Party party;

	char line[128];
	while (PuzzleInput::NextLine())
	{
		int32_t lineLength = Parse::ReadLine(line, sizeof(line));
		assert(lineLength > 0);
		(void)lineLength;

		char from;
		char to;
		char action[16] = { 0 };
		int32_t happiness;
		int scanned = sscanf(line, "%c%*[A-Za-z] would %s %d happiness units by sitting next to %c%*[A-Za-z].",
			&from,
			action,
			&happiness,
			&to);
		assert(scanned == 4);
		(void)scanned;

		happiness = (strcmp(action, "lose") == 0) ? -happiness : happiness;

		size_t fromIndex = from - 'A';
		size_t toIndex = to - 'A';
		party.SetHappiness(fromIndex, toIndex, happiness);

		party.People.insert(fromIndex);
	}
	return party;
}

//**NOTE: Need to be super-careful about the stack frame size here. Should be okay with the small party sizes here
// Current stack frame 20 bytes, maximum recursion depth 10, should be well within the 4K limit
static int32_t MaxHappiness(const Party& party, vector<size_t>* arrangement, size_t activeIndex, int32_t partialHappiness)
{
	if (activeIndex == arrangement->size())
	{
		return partialHappiness
			+ party.GetHappiness(arrangement->front(), arrangement->back())
			+ party.GetHappiness(arrangement->back(), arrangement->front());
	}

	// Pick the next person for this slot
	int32_t maxHappiness = 0;
	for (size_t i = activeIndex; i < arrangement->size(); i++)
	{
		swap((*arrangement)[activeIndex], (*arrangement)[i]);

		// Build up the happiness as we go
		int32_t happinessDelta = 0;
		if (activeIndex > 0)
		{
			happinessDelta += party.GetHappiness((*arrangement)[activeIndex], (*arrangement)[activeIndex - 1]);
			happinessDelta += party.GetHappiness((*arrangement)[activeIndex - 1], (*arrangement)[activeIndex]);
		}

		int32_t candidateHappiness = MaxHappiness(party, arrangement, activeIndex + 1, partialHappiness + happinessDelta);
		if (candidateHappiness > maxHappiness)
		{
			maxHappiness = candidateHappiness;
		}

		swap((*arrangement)[activeIndex], (*arrangement)[i]);
	}

	return maxHappiness;
}

void Puzzle13_A_2015()
{
	Party party = ParseParty();
	vector<size_t> partyPermutation;
	partyPermutation.reserve(16);

	ranges::copy(party.People, back_inserter(partyPermutation));
	int32_t answer = MaxHappiness(party, &partyPermutation, 1, 0);

	return PuzzleOutput::Submit(2015, 13, 1, answer);
}


void Puzzle13_B_2015()
{
	Party party = ParseParty();
	vector<size_t> partyPermutation;
	partyPermutation.reserve(16);

	ranges::copy(party.People, back_inserter(partyPermutation));
	partyPermutation.push_back(party.People.size());

	int32_t answer = MaxHappiness(party, &partyPermutation, 1, 0);

	return PuzzleOutput::Submit(2015, 13, 2, answer);
}
