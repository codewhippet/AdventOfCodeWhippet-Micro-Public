#include "stdafx.h"

using namespace std;

namespace Puzzle19_2020_Types
{
	struct Rule
	{
		char Terminal = '\0';
		size_t NumProductions = 0;
		array<array<int32_t, 2>, 2> Productions;
	};
}

using namespace Puzzle19_2020_Types;


static int MakeAtom(int id)
{
	return id + 1000;
}

static bool IsAtom(int id)
{
	return id >= 1000;
}

static vector<int> Tokenise(const string& input, const map<char, int>& tokeniserRules)
{
	return MakeEnumerator(input)
		->Select<int>([&tokeniserRules](char c) { return tokeniserRules.at(c); })
		->ToVector();
}

static void Puzzle19_A(const string &filename)
{
	(void)filename;
	ifstream input(filename);
	//istringstream input(dummy);

	map<int, set<vector<int>>> productionRules;
	map<char, int> tokeniserRules;
	vector<string> messages;
	{
		regex productionSingleFormat(R"((\d+): (\d+))");
		regex productionDoubleFormat(R"((\d+): (\d+) (\d+))");
		regex productionSingleChoiceFormat(R"((\d+): (\d+) \| (\d+))");
		regex productionDoubleChoiceFormat(R"((\d+): (\d+) (\d+) \| (\d+) (\d+))");
		regex productionAtomFormat(R"-((\d+): "(\w)")-");
		regex messageFormat(R"([ab]+)");

		for (const string& line : ReadAllLines(input))
		{
			smatch match;
			if (regex_match(line, match, productionSingleFormat))
			{
				productionRules[stoi(match[1].str())].insert({ stoi(match[2].str()) });
			}

			if (regex_match(line, match, productionDoubleFormat))
			{
				productionRules[stoi(match[1].str())].insert({ stoi(match[2].str()), stoi(match[3].str()) });
			}

			if (regex_match(line, match, productionSingleChoiceFormat))
			{
				productionRules[stoi(match[1].str())].insert({ stoi(match[2].str()) });
				productionRules[stoi(match[1].str())].insert({ stoi(match[3].str()) });
			}

			if (regex_match(line, match, productionDoubleChoiceFormat))
			{
				productionRules[stoi(match[1].str())].insert({ stoi(match[2].str()), stoi(match[3].str()) });
				productionRules[stoi(match[1].str())].insert({ stoi(match[4].str()), stoi(match[5].str()) });
			}

			if (regex_match(line, match, productionAtomFormat))
			{
				productionRules[stoi(match[1].str())].insert({ MakeAtom(match[2].str()[0]) });
				tokeniserRules[match[2].str()[0]] = MakeAtom(match[2].str()[0]);
			}

			if (regex_match(line, match, messageFormat))
			{
				messages.push_back(match.str());
			}
		}
	}

	// Generate
	map<int, set<vector<int>>> fullGrammar;
	function<const set<vector<int>>&(int id)> generate;
	generate = [&generate, &fullGrammar, &productionRules](int id) -> const set<vector<int>> &
	{
		map<int, set<vector<int>>>::const_iterator cachedIt = fullGrammar.find(id);
		if (cachedIt != fullGrammar.end())
		{
			return cachedIt->second;
		}

		if (IsAtom(id))
		{			
			vector<int> singleElement;
			singleElement.push_back(id);
			fullGrammar[id].insert(move(singleElement));
			return fullGrammar[id];
		}

		const set<vector<int>>& possibleProductions = productionRules.at(id);
		for (const vector<int> &production : possibleProductions)
		{
			if (production.size() == 1)
			{
				const set<vector<int>>& gen = generate(production[0]);
				fullGrammar[id].insert(gen.begin(), gen.end());
				continue;
			}

			assert(production.size() == 2);
			for (const vector<int>& gen1 : generate(production[0]))
			{
				for (const vector<int>& gen2 : generate(production[1]))
				{
					vector<int> combined = gen1;
					combined.insert(combined.end(), gen2.begin(), gen2.end());
					fullGrammar[id].insert(move(combined));
				}
			}
		}

		return fullGrammar[id];
	};

	generate(0);

	int64_t answer = 0;
	for (const string& message : messages)
	{
		vector<int> tokenisedMessage = Tokenise(message, tokeniserRules);
		if (fullGrammar[0].find(tokenisedMessage) != fullGrammar[0].end())
		{
			answer++;
		}
	}

	printf("[2020] Puzzle19_A: %" PRId64 "\n", answer);
}


static void Puzzle19_B(const string& filename)
{
	(void)filename;
	ifstream input(filename);
	//istringstream input(dummy);

	map<int, set<vector<int>>> productionRules;
	map<char, int> tokeniserRules;
	vector<string> messages;
	{
		regex productionSingleFormat(R"((\d+): (\d+))");
		regex productionDoubleFormat(R"((\d+): (\d+) (\d+))");
		regex productionSingleChoiceFormat(R"((\d+): (\d+) \| (\d+))");
		regex productionDoubleChoiceFormat(R"((\d+): (\d+) (\d+) \| (\d+) (\d+))");
		regex productionAtomFormat(R"-((\d+): "(\w)")-");
		regex messageFormat(R"([ab]+)");

		for (const string& line : ReadAllLines(input))
		{
			smatch match;
			if (regex_match(line, match, productionSingleFormat))
			{
				productionRules[stoi(match[1].str())].insert({ stoi(match[2].str()) });
			}

			if (regex_match(line, match, productionDoubleFormat))
			{
				productionRules[stoi(match[1].str())].insert({ stoi(match[2].str()), stoi(match[3].str()) });
			}

			if (regex_match(line, match, productionSingleChoiceFormat))
			{
				productionRules[stoi(match[1].str())].insert({ stoi(match[2].str()) });
				productionRules[stoi(match[1].str())].insert({ stoi(match[3].str()) });
			}

			if (regex_match(line, match, productionDoubleChoiceFormat))
			{
				productionRules[stoi(match[1].str())].insert({ stoi(match[2].str()), stoi(match[3].str()) });
				productionRules[stoi(match[1].str())].insert({ stoi(match[4].str()), stoi(match[5].str()) });
			}

			if (regex_match(line, match, productionAtomFormat))
			{
				productionRules[stoi(match[1].str())].insert({ MakeAtom(match[2].str()[0]) });
				tokeniserRules[match[2].str()[0]] = MakeAtom(match[2].str()[0]);
			}

			if (regex_match(line, match, messageFormat))
			{
				messages.push_back(match.str());
			}
		}
	}

	// Clear out unwanted production rules
	productionRules.erase(8);
	productionRules.erase(11);

	// Non-recursive rules
	//productionRules[8].insert(vector<int>{ 42 });
	//productionRules[11].insert(vector<int>{ 42, 31 });

	// Generate
	set<vector<int>> emptyGrammar;

	map<int, set<vector<int>>> fullGrammar;
	function<const set<vector<int>>& (int id)> generate;
	generate = [&](int id) -> const set<vector<int>> &
	{
		map<int, set<vector<int>>>::const_iterator cachedIt = fullGrammar.find(id);
		if (cachedIt != fullGrammar.end())
		{
			return cachedIt->second;
		}

		if (IsAtom(id))
		{
			vector<int> singleElement;
			singleElement.push_back(id);
			fullGrammar[id].insert(move(singleElement));
			return fullGrammar[id];
		}

		if (productionRules.find(id) == productionRules.end())
		{
			return emptyGrammar;
		}

		const set<vector<int>>& possibleProductions = productionRules.at(id);
		for (const vector<int>& production : possibleProductions)
		{
			if (production.size() == 1)
			{
				const set<vector<int>>& gen = generate(production[0]);
				fullGrammar[id].insert(gen.begin(), gen.end());
				continue;
			}

			assert(production.size() == 2);
			for (const vector<int>& gen1 : generate(production[0]))
			{
				for (const vector<int>& gen2 : generate(production[1]))
				{
					vector<int> combined = gen1;
					combined.insert(combined.end(), gen2.begin(), gen2.end());
					fullGrammar[id].insert(move(combined));
				}
			}
		}

		return fullGrammar[id];
	};

	//generate(0);
	generate(42);
	generate(31);

	// Do 42 & 31 overlap?
	vector<vector<int>> common;
	set_intersection(fullGrammar[31].begin(), fullGrammar[31].end(),
		fullGrammar[42].begin(), fullGrammar[42].end(),
		back_inserter(common));
	assert(common.empty());

	// Make sure all fragments are the same size for easy parsing
	for (const vector<int> &fragment : fullGrammar[31])
	{
		assert(fragment.size() == fullGrammar[31].begin()->size());
		(void)fragment;
	}
	for (const vector<int>& fragment : fullGrammar[42])
	{
		assert(fragment.size() == fullGrammar[31].begin()->size());
		(void)fragment;
	}
	assert(fullGrammar[31].begin()->size() == fullGrammar[42].begin()->size());


	size_t fragmentSize = fullGrammar[31].begin()->size();

	int64_t answer = 0;
	for (const string& message : messages)
	{
		if (message.size() % fragmentSize != 0)
		{
			continue;
		}

		vector<int> tokenisedMessage = Tokenise(message, tokeniserRules);

		// Grammar is 42+ 42^n 31^n
		// A lots of 42 followed by B lots of 31, where A > B
		int num42s = 0;
		while (true)
		{
			if (tokenisedMessage.empty())
			{
				break;
			}

			vector<int> f;
			f.insert(f.end(), tokenisedMessage.begin(), tokenisedMessage.begin() + fragmentSize);
			if (fullGrammar[42].find(f) == fullGrammar[42].end())
			{
				break;
			}

			tokenisedMessage.erase(tokenisedMessage.begin(), tokenisedMessage.begin() + fragmentSize);
			num42s++;
		}

		bool valid = true;

		int num31s = 0;
		while (true)
		{
			if (tokenisedMessage.empty())
			{
				break;
			}

			vector<int> f;
			f.insert(f.end(), tokenisedMessage.begin(), tokenisedMessage.begin() + fragmentSize);
			if (fullGrammar[31].find(f) == fullGrammar[31].end())
			{
				valid = false;
				break;
			}

			tokenisedMessage.erase(tokenisedMessage.begin(), tokenisedMessage.begin() + fragmentSize);
			num31s++;
		}

		if (valid && (num42s > 0) && (num31s > 0) && (num42s > num31s))
		{
			answer++;
		}
	}

	printf("[2020] Puzzle19_B: %" PRId64 "\n", answer);
}


// ------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------

static void ParseRule(vector<Rule>* rules)
{
	int32_t ruleIndex = Parse::GetInt32();
	Parse::DiscardExpected(": ");

	Rule& rule = (*rules)[ruleIndex];

	if (PuzzleInput::PeekChar() == '"')
	{
		PuzzleInput::DropChar();
		rule.Terminal = static_cast<char>(PuzzleInput::GetChar());
		Parse::DiscardExpected("\"");
	}
	else
	{
		while (true)
		{
			int32_t firstNonterminal = Parse::GetInt32();

			array<int32_t, 2>& productions = rule.Productions[rule.NumProductions++];
			productions[0] = firstNonterminal;
			productions[1] = -1;

			if (PuzzleInput::PeekChar() == '\n')
				break;

			Parse::DiscardExpected(" ");

			if (PuzzleInput::PeekChar() == '|')
			{
				Parse::DiscardExpected("| ");
				continue;
			}
			else if (isdigit(PuzzleInput::PeekChar()))
			{
				int32_t secondNonterminal = Parse::GetInt32();
				productions[1] = secondNonterminal;
			}

			if (PuzzleInput::PeekChar() == '\n')
				break;

			Parse::DiscardExpected(" | ");
		}
	}

	Parse::DiscardExpected("\n");
}

//**Needs converting to stack based
static size_t GetLength(int32_t nonterminal, const vector<Rule>& productionRules, vector<size_t>* lengths)
{
	size_t existingSize = (*lengths)[nonterminal];
	if (existingSize > 0)
		return existingSize;

	size_t length = numeric_limits<size_t>::max();
	const Rule& rule = productionRules[nonterminal];

	if (rule.Terminal)
	{
		length = 1;
	}
	else
	{
		assert(rule.NumProductions > 0);
		for (size_t i = 0; i < rule.NumProductions; i++)
		{
			length = GetLength(rule.Productions[i][0], productionRules, lengths);
			if (rule.Productions[i][1] != -1)
			{
				length += GetLength(rule.Productions[i][1], productionRules, lengths);
			}
		}
	}

	(*lengths)[nonterminal] = length;
	return length;
}

static bool Match(int32_t nonterminal, size_t position, const char* message, const vector<Rule>& productionRules, const vector<size_t>& lengths)
{
	const Rule& rule = productionRules[nonterminal];

	bool matches = false;
	if (rule.Terminal)
	{
		matches = message[position] == rule.Terminal;
	}
	else
	{
		size_t lengthOfNonterminal = lengths[nonterminal];
		for (size_t production = 0; production < rule.NumProductions; production++)
		{
			size_t subPosition = position;
			for (int32_t nextNonterminal : rule.Productions[production])
			{
				if (nextNonterminal == -1)
					break;

				if (Match(nextNonterminal, subPosition, message, productionRules, lengths))
				{
					subPosition += lengths[nextNonterminal];
				}
				else
				{
					break;
				}
			}
			if (subPosition == (position + lengthOfNonterminal))
			{
				matches = true;
				break;
			}
		}
	}

	return matches;
}

static void Combined()
{
	vector<Rule> productionRules;
	productionRules.resize(256);

	while (PuzzleInput::PeekChar() != '\n')
	{
		ParseRule(&productionRules);
	}

	Parse::DiscardExpected("\n");

	vector<size_t> lengths(150);
	GetLength(0, productionRules, &lengths);

	int32_t part1Answer = 0;
	int32_t part2Answer = 0;

	size_t part1Length = lengths[0];
	size_t part2ChunkSize = lengths[42];
	assert(lengths[31] == part2ChunkSize);

	vector<char> line(100);
	while (PuzzleInput::NextLine())
	{
		int32_t lineLength = Parse::ReadNonEmptyLine(line.data(), line.size());
		if ((lineLength == part1Length) && Match(0, 0, line.data(), productionRules, lengths))
		{
			part1Answer++;
		}

		const char* part2Chunks = line.data();
		const char* endOfLine = part2Chunks + lineLength;
		assert((lineLength % part2ChunkSize) == 0);

		// Count 42s
		size_t symbol42Count = 0;
		while (part2Chunks != endOfLine)
		{
			if (Match(42, 0, part2Chunks, productionRules, lengths))
			{
				symbol42Count++;
				part2Chunks += part2ChunkSize;
			}
			else
			{
				break;
			}
		}

		// Count31s
		bool valid = true;
		size_t symbol31Count = 0;
		while (part2Chunks != endOfLine)
		{
			if (Match(31, 0, part2Chunks, productionRules, lengths))
			{
				symbol31Count++;
				part2Chunks += part2ChunkSize;
			}
			else
			{
				valid = false;
				break;
			}
		}

		if (valid && (symbol31Count > 0) && (symbol42Count > symbol31Count))
		{
			part2Answer++;
		}
	}

	printf("Part 1: %d\n", part1Answer);
	printf("Part 2: %d\n", part2Answer);
}


void Puzzle19_A_2020()
{
	Puzzle19_A(R"(z:\AoCInput\2020\Puzzle19.txt)");

	Combined();

	int32_t answer = 0;
	PuzzleOutput::Submit(2020, 19, 1, answer);
}

void Puzzle19_B_2020()
{
	Puzzle19_B(R"(z:\AoCInput\2020\Puzzle19.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2020, 19, 2, answer);
}
