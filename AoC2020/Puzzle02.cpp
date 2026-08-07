#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle02_2020_Types
{
}

using namespace Puzzle02_2020_Types;


static void Puzzle02_A(const string &filename)
{
	(void)filename;
	ifstream input(filename);
	//istringstream input(dummy);

	const regex format(R"((\d+)-(\d+) (\w): (\w+))");

	int64_t answer = MakeEnumerator(ReadAllLines(input))
		->Where([&format](const string& s)
			{
				smatch m;
				regex_match(s, m, format);
				int minRepeats = stoi(m[1].str());
				int maxRepeats = stoi(m[2].str());
				string password = m[4].str();
				int64_t numRepeats = count(password.begin(), password.end(), m[3].str()[0]);
				return (numRepeats >= minRepeats) && (numRepeats <= maxRepeats);
			})
		->Count();

	printf("[2020] Puzzle02_A: %" PRId64 "\n", answer);
}

static void Puzzle02_B(const string& filename)
{
	(void)filename;
	ifstream input(filename);
	//istringstream input(dummy);

	const regex format(R"((\d+)-(\d+) (\w): (\w+))");

	int64_t answer = MakeEnumerator(ReadAllLines(input))
		->Where([&format](const string& s)
			{
				smatch m;
				regex_match(s, m, format);
				int posOne = stoi(m[1].str()) - 1;
				int posTwo = stoi(m[2].str()) - 1;
				string password = m[4].str();
				char c = m[3].str()[0];
				return (password[posOne] == c) ^ (password[posTwo] == c);
			})
		->Count();

	printf("[2020] Puzzle02_B: %" PRId64 "\n", answer);
}

void Puzzle02_A_2020()
{
	Puzzle02_A(R"(z:\AoCInput\2020\Puzzle02.txt)");
}

void Puzzle02_B_2020()
{
	Puzzle02_B(R"(z:\AoCInput\2020\Puzzle02.txt)");
}
