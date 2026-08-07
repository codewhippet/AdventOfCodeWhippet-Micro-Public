#include "stdafx.h"

using namespace std;

namespace Puzzle14_2015_Types
{
	struct Reindeer
	{
		string Name;
		int32_t Speed;
		int32_t DistancePerQuantum;
		int32_t QuantumDuration;
	};
}

using namespace Puzzle14_2015_Types;

static vector<Reindeer> ParseReindeers()
{
	vector<Reindeer> reindeers;
	char line[128];
	while (PuzzleInput::NextLine())
	{
		int32_t lineLength = Parse::ReadLine(line, sizeof(line));
		assert(lineLength > 0);
		(void)lineLength;

		Reindeer r;

		char name[64] = { 0 };
		int32_t flyDuration;
		int32_t restDuration;
		int scanned = sscanf(line, "%s can fly %d km/s for %d seconds, but then must rest for %d seconds.",
			name,
			&r.Speed,
			&flyDuration,
			&restDuration);
		assert(scanned == 4);
		(void)scanned;

		r.Name = name;
		r.DistancePerQuantum = flyDuration * r.Speed;
		r.QuantumDuration = flyDuration + restDuration;

		reindeers.push_back(move(r));

	}
	return reindeers;
}

static int32_t DistanceTravelled(const Reindeer& reindeer, int32_t time)
{
	int32_t fullQuanta = time / reindeer.QuantumDuration;
	int32_t partialQuantum = time % reindeer.QuantumDuration;

	int32_t fullDurationDistances = fullQuanta * reindeer.DistancePerQuantum;
	int32_t partialDurationDistance = min(reindeer.Speed * partialQuantum, reindeer.DistancePerQuantum);

	return fullDurationDistances + partialDurationDistance;
}

void Puzzle14_A_2015()
{
	const int32_t raceDuration = 2503;

	vector<Reindeer> reindeers = ParseReindeers();

	vector<pair<int32_t, string>> reindeerDistances;
	ranges::copy(reindeers
		| views::transform([&](const Reindeer& r)
			{
				return make_pair(DistanceTravelled(r, raceDuration), r.Name);
			}),
		back_inserter(reindeerDistances));
	ranges::sort(reindeerDistances);

	int32_t answer = reindeerDistances.back().first;

	return PuzzleOutput::Submit(2015, 14, 1, answer);
}


void Puzzle14_B_2015()
{
	const int32_t raceDuration = 2503;

	vector<Reindeer> reindeers = ParseReindeers();

	map<string, int32_t> reindeerScores;

	vector<int32_t> reindeerDistances(reindeers.size());
	for (int32_t i = 0; i < raceDuration; i++)
	{
		int32_t leadingDistance = 0;
		for (size_t r = 0; r < reindeers.size(); r++)
		{
			 int32_t distance = DistanceTravelled(reindeers[r], i + 1);
			 reindeerDistances[r] = distance;
			 leadingDistance = max(distance, leadingDistance);
		}

		for (size_t r = 0; r < reindeers.size(); r++)
		{
			if (reindeerDistances[r] == leadingDistance)
			{
				reindeerScores[reindeers[r].Name]++;
			}
		}
	}

	int32_t answer = ranges::max(reindeerScores | views::transform([](const auto& kvp) { return kvp.second; }));

	return PuzzleOutput::Submit(2015, 14, 2, answer);
}
