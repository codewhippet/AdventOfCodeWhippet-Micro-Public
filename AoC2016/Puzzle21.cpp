#include "stdafx.h"

using namespace std;

namespace Puzzle21_2016_Types
{
	using Operation = function<string(const string&)>;
}

using namespace Puzzle21_2016_Types;

static vector<Operation> ParseProgram()
{
	vector<Operation> program;

	const regex swapPosition{ R"(swap position (\d+) with position (\d+))" };
	const regex swapLetter{ R"(swap letter (\w) with letter (\w))" };
	const regex rotate{ R"(rotate (\w+) (\d+) steps?)" };
	const regex rotateByLetter{ R"(rotate based on position of letter (\w))" };
	const regex reverse{ R"(reverse positions (\d+) through (\d+))" };
	const regex move{ R"(move position (\d+) to position (\d+))" };

	char line[64];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line);

		cmatch m;
		if (regex_match(line, m, swapPosition))
		{
			program.emplace_back(
				[a = atoi(m[1].first), b = atoi(m[2].first)](const string& s)
				{
					string t = s;
					swap(t[a], t[b]);
					return t;
				});
		}
		else if (regex_match(line, m, swapLetter))
		{
			program.emplace_back(
				[a = m[1].first[0], b = m[2].first[0]](const string& s)
				{
					string t = s;
					swap(t[t.find(a)], t[t.find(b)]);
					return t;
				});
		}
		else if (regex_match(line, m, rotate))
		{
			bool right = (string_view{ m[1].first, m[1].second } == "right"sv);
			program.emplace_back(
				[right, a = atoi(m[2].first)](const string& s)
				{
					assert(a <= (int64_t)s.size());

					string t = s;
					for (size_t sourceIndex = 0; sourceIndex < s.size(); sourceIndex++)
					{
						size_t destinationIndex = (right ? (sourceIndex + a) % t.size() : (sourceIndex + t.size() - a) % t.size());
						t[destinationIndex] = s[sourceIndex];
					}

					return t;
				});
		}
		else if (regex_match(line, m, rotateByLetter))
		{
			program.emplace_back(
				[a = m[1].str()[0]](const string& s)
				{
					size_t position = s.find(a);
					size_t rotation = 1 + position + (position >= 4 ? 1 : 0);

					string t = s;
					for (size_t sourceIndex = 0; sourceIndex < s.size(); sourceIndex++)
					{
						size_t destinationIndex = (sourceIndex + rotation) % t.size();
						t[destinationIndex] = s[sourceIndex];
					}

					return t;
				});
		}
		else if (regex_match(line, m, reverse))
		{
			program.emplace_back(
				[a = atoi(m[1].str().c_str()), b = atoi(m[2].str().c_str())](const string& s)
				{
					assert(a < b);

					auto i = a;
					auto j = b;

					string t = s;
					while (i < j)
					{
						swap(t[i++], t[j--]);
					}

					return t;
				});
		}
		else if (regex_match(line, m, move))
		{
			program.emplace_back(
				[a = atoi(m[1].str().c_str()), b = atoi(m[2].str().c_str())](const string& s)
				{
					string t = s;
					char letter = t[a];
					t.erase(a, 1);
					t.insert(t.begin() + b, letter);
					return t;
				});
		}
		else
		{
			assert(false);
		}
	}

	return program;
}

void Puzzle21_A_2016()
{
	vector<Operation> program = ParseProgram();

	string answer = "abcdefgh";
	for (const Operation& op : program)
	{
		answer = op(answer);
	}

	return PuzzleOutput::Submit(2016, 21, 1, answer.c_str());
}

void Puzzle21_B_2016()
{
	vector<Operation> program = ParseProgram();

	string answer = "abcdefgh";
	do
	{
		string scrambled = answer;
		for (const Operation& op : program)
		{
			scrambled = op(scrambled);
		}

		if (scrambled == "fbgdceah"sv)
		{
			break;
		}

	} while (next_permutation(answer.begin(), answer.end()));

	return PuzzleOutput::Submit(2016, 21, 2, answer.c_str());
}
