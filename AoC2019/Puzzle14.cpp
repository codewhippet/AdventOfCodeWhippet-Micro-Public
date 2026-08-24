#include "stdafx.h"

using namespace std;

namespace Puzzle14_2019_Types
{
	struct Reaction
	{
		int32_t Chemical = -1;
		int32_t Created;
		vector<pair<int32_t, int32_t>> Requires;
	};
}

using namespace Puzzle14_2019_Types;

static vector<Reaction> ParseReactions(NameDictionary* names)
{
	vector<Reaction> reactions;
	reactions.resize(64); // We're going to have some slop at the top with junk reactions

	const regex reaction{ R"(([^=]+) => (\d+) (\w+))" };
	const regex chemical{ R"((\d+) (\w+))" };

	char line[128];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line);

		cmatch lineMatch;
		regex_match(line, lineMatch, reaction);

		Reaction r;
		r.Chemical = names->IdFromName(lineMatch[3].str().c_str());
		r.Created = atoi(lineMatch[2].str().c_str());

		string precursors = lineMatch[1].str();
		auto precursors_begin = sregex_iterator(precursors.begin(), precursors.end(), chemical);
		for (sregex_iterator chem = precursors_begin; chem != sregex_iterator{}; ++chem)
		{
			r.Requires.push_back({ names->IdFromName((*chem)[2].str().c_str()), atoi((*chem)[1].str().c_str()) });
		}
		reactions[r.Chemical] = r;
	}

	return reactions;
}

static int64_t OreRequiredForFuel(int64_t fuel, const vector<Reaction>& reactions, const NameDictionary& names)
{
	vector<int64_t> required(reactions.size());
	vector<int64_t> used(reactions.size());
	vector<int64_t> overspill(reactions.size());

	required[names.IdFromName("FUEL")] = fuel;
	const int32_t ORE = names.IdFromName("ORE");

	int64_t oreNeeded = 0;
	while (true)
	{
		auto nonEmpty = ranges::find_if(required, [](int64_t v) { return v != 0; });
		if (nonEmpty == required.end())
			break;

		int32_t chemical = static_cast<int32_t>(distance(required.begin(), nonEmpty));
		int64_t amountNeeded = *nonEmpty;
		*nonEmpty = 0;

		if (chemical == ORE)
		{
			oreNeeded += amountNeeded;
			continue;
		}

		const Reaction& reaction = reactions[chemical];

		int64_t numReactions = ((amountNeeded % reaction.Created == 0) ? amountNeeded : (amountNeeded + reaction.Created)) / reaction.Created;
		int64_t amountCreated = numReactions * reaction.Created;

		// Sort out the precursors
		for (const auto& p : reaction.Requires)
		{
			int64_t precursorAmount = p.second * numReactions;

			// Use up overspill first
			int64_t overspillUsed = min(precursorAmount, overspill[p.first]);
			overspill[p.first] -= overspillUsed;
			precursorAmount -= overspillUsed;

			// Put in an order for the rest
			if (precursorAmount > 0)
			{
				required[p.first] += precursorAmount;
			}
		}

		// Top up the overspill with the excess
		overspill[chemical] += amountCreated - amountNeeded;
	}

	return oreNeeded;
}

void Puzzle14_A_2019()
{
	NameDictionary names(256);
	vector<Reaction> reactions = ParseReactions(&names);
	int64_t answer = OreRequiredForFuel(1, reactions, names);

	PuzzleOutput::Submit(2019, 14, 1, answer);
}

void Puzzle14_B_2019()
{
	const int64_t maximumOre = 1000000000000;

	NameDictionary names(256);
	vector<Reaction> reactions = ParseReactions(&names);

	int64_t answer = 0;

	int64_t lowerFuel = 1;
	int64_t upperFuel = maximumOre;
	while (true)
	{
		int64_t midpointFuel = (lowerFuel + upperFuel) / 2;
		int64_t oreAtMidpoint = OreRequiredForFuel(midpointFuel, reactions, names);
		int64_t oreAboveMidpoint = OreRequiredForFuel(midpointFuel + 1, reactions, names);

		if ((oreAtMidpoint <= maximumOre) && (oreAboveMidpoint > maximumOre))
		{
			answer = midpointFuel;
			break;
		}

		if (oreAtMidpoint > maximumOre)
		{
			upperFuel = midpointFuel;
		}
		else
		{
			lowerFuel = midpointFuel;
		}
	}

	PuzzleOutput::Submit(2019, 14, 2, answer);
}
