#include "stdafx.h"

using namespace std;

namespace Puzzle13_2020_Types
{
}

using namespace Puzzle13_2020_Types;

void Puzzle13_A_2020()
{
	int32_t startingTime = Parse::GetInt32();
	PuzzleInput::NextLine();

	vector<char> timetable(256);
	Parse::ReadNonEmptyLine(timetable.data(), timetable.size());

	vector<int32_t> busses;
	busses.reserve(16);

	const char *delims = ",x";
	for (char* bus = strtok(timetable.data(), delims); bus; bus = strtok(nullptr, delims))
	{
		busses.push_back(Parse::GetInt32(bus));
	}

	MinValue<pair<int32_t, int32_t>> firstDeparture({ numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max() });
	for (int32_t bus : busses)
	{
		int32_t timeSinceLastDeparture = startingTime % bus;
		int32_t timeUntilNextDeparture = (bus - timeSinceLastDeparture) % bus;
		firstDeparture.Update({ startingTime + timeUntilNextDeparture, bus });
	}

	int32_t answer = (firstDeparture.Get().first - startingTime) * firstDeparture.Get().second;

	PuzzleOutput::Submit(2020, 13, 1, answer);
}

void Puzzle13_B_2020()
{
	PuzzleInput::DropLine();
	PuzzleInput::NextLine();

	vector<char> timetable(256);
	Parse::ReadNonEmptyLine(timetable.data(), timetable.size());

	vector<pair<int64_t, int64_t>> busses;
	busses.reserve(16);

	const char* delims = ",";
	int64_t busConstraint = 0;
	for (char* bus = strtok(timetable.data(), delims); bus; bus = strtok(nullptr, delims))
	{
		if (bus[0] != 'x')
		{
			busses.push_back(make_pair(stoll(bus), busConstraint));
		}
		busConstraint++;
	}

	int64_t t = 0;
	int64_t timeIncrement = busses[0].first;

	for (size_t i =  1; i < busses.size(); i++)
	{
		int64_t bus = busses[i].first;
		int64_t constraint = busses[i].second;
		while (((t + constraint) % bus) != 0)
		{
			t += timeIncrement;
		}
		timeIncrement *= bus;
	}

	int64_t answer = t;

	PuzzleOutput::Submit(2020, 13, 2, answer);
}
