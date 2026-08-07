#include "stdafx.h"

using namespace std;

namespace Puzzle09_2015_Types
{
	struct Roadmap
	{
		map<char, string> Destinations;
		map<string, char> DestinationCodes;
		map<pair<char, char>, int32_t> Distances;
	};
}

using namespace Puzzle09_2015_Types;

static char AddOrGetDestination(const char* parsedBuffer, Roadmap *roadmap)
{
	string destination{ parsedBuffer };
	auto existingDestination = roadmap->DestinationCodes.find(destination);
	if (existingDestination == roadmap->DestinationCodes.end())
	{
		char newCode = 'a' + (char)roadmap->DestinationCodes.size();
		roadmap->Destinations[newCode] = destination;
		existingDestination = roadmap->DestinationCodes.insert({ destination, newCode }).first;
	}
	return existingDestination->second;
}

static Roadmap ParseRoads()
{
	Roadmap roadmap;
	while (PuzzleInput::NextLine())
	{
		char line[64];
		int32_t lineLength = Parse::ReadLine(line, sizeof(line));
		assert(lineLength > 0);
		(void)lineLength;

		char from[32];
		char to[32];
		int32_t distance;
		int scanned = sscanf(line, "%s to %s = %d",
			from,
			to,
			&distance);
		assert(scanned == 3);
		(void)scanned;

		char fromCode = AddOrGetDestination(from, &roadmap);
		char toCode = AddOrGetDestination(to, &roadmap);

		roadmap.Distances.insert({ { fromCode, toCode }, distance });
		roadmap.Distances.insert({ { toCode, fromCode }, distance });
	}
	return roadmap;
}

static int32_t RouteDistance(const string& route, const Roadmap& roadmap)
{
	int32_t distance = 0;
	for (size_t i = 0; i + 1 < route.size(); i++)
	{
		distance += roadmap.Distances.at({ route[i + 0], route[i + 1] });
	}
	return distance;
}

void Puzzle09_A_2015()
{
	Roadmap roadmap = ParseRoads();

	string route;
	ranges::copy(roadmap.Destinations
		| views::transform([](const auto& kvp)
			{
				return kvp.first;
			}),
		back_inserter(route));

	int32_t answer = numeric_limits<int32_t>::max();
	do
	{
		answer = min(answer, RouteDistance(route, roadmap));

	} while (next_permutation(route.begin(), route.end()));

	return PuzzleOutput::Submit(2015, 9, 1, answer);
}


void Puzzle09_B_2015()
{
	Roadmap roadmap = ParseRoads();

	string route;
	ranges::copy(roadmap.Destinations
		| views::transform([](const auto& kvp)
			{
				return kvp.first;
			}),
		back_inserter(route));

	int32_t answer = 0;
	do
	{
		answer = max(answer, RouteDistance(route, roadmap));

	} while (next_permutation(route.begin(), route.end()));

	return PuzzleOutput::Submit(2015, 9, 2, answer);
}