#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle06_2020_Types
{
}

using namespace Puzzle06_2020_Types;


static void Puzzle06_A(const string &filename)
{
	(void)filename;
	ifstream input(filename);
	//istringstream input(dummy);

	vector<string> lines = ReadAllLines(input);
	lines.push_back("");

	set<char> seen;
	vector<size_t> groupCounts;
	for (const string& line : lines)
	{
		if (line.empty())
		{
			groupCounts.push_back(seen.size());
			seen.clear();
		}

		for (char c : line)
		{
			seen.insert(c);
		}
	}

	int64_t answer = accumulate(groupCounts.begin(), groupCounts.end(), 0ll);

	printf("[2020] Puzzle06_A: %" PRId64 "\n", answer);
}

static void Puzzle06_B(const string& filename)
{
	(void)filename;
	ifstream input(filename);
	//istringstream input(dummy);

	vector<string> lines = ReadAllLines(input);
	lines.push_back("");

	int numPeople = 0;
	map<char, int> seen;
	vector<size_t> groupCounts;
	for (const string& line : lines)
	{
		if (line.empty())
		{
			size_t allAnswered = count_if(seen.begin(), seen.end(),
				[&seen, numPeople](const pair<char, int>& kvp)
				{
					return kvp.second == numPeople;
				});

			groupCounts.push_back(allAnswered);
			seen.clear();
			numPeople = 0;

			continue;
		}

		for (char c : line)
		{
			seen[c]++;
		}

		numPeople++;
	}

	int64_t answer = accumulate(groupCounts.begin(), groupCounts.end(), 0ll);

	printf("[2020] Puzzle06_B: %" PRId64 "\n", answer);
}

void Puzzle06_A_2020()
{
	Puzzle06_A(R"(z:\AoCInput\2020\Puzzle06.txt)");
}

void Puzzle06_B_2020()
{
	Puzzle06_B(R"(z:\AoCInput\2020\Puzzle06.txt)");
}
