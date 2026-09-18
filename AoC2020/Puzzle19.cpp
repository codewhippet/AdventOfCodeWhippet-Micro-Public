#include "stdafx.h"

using namespace std;

namespace Puzzle19_2020_Types
{
	struct Rule
	{
		char Terminal = '\0';
		uint32_t NumProductions = 0;
		array<array<int32_t, 2>, 2> Productions;
	};
}

using namespace Puzzle19_2020_Types;

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

static void GetLengths(int32_t startAt, const vector<Rule>& productionRules, vector<uint32_t>* lengths)
{
	vector<int32_t> nonTerminals;
	nonTerminals.reserve(16);

	nonTerminals.push_back(startAt);
	while (nonTerminals.empty() == false)
	{
		int32_t nonTerminal = nonTerminals.back();

		// Do we know the length?
		uint32_t existingSize = (*lengths)[nonTerminal];
		if (existingSize > 0)
		{
			nonTerminals.pop_back();
			continue;
		}

		const Rule& rule = productionRules[nonTerminal];

		// Is this a terminal?
		if (rule.Terminal)
		{
			(*lengths)[nonTerminal] = 1;
			nonTerminals.pop_back();
			continue;
		}

		// Try to work out the proper length
		// NOTE: Left and right branches are equal length in the puzzle grammar, but we need
		// to traverse both legs in order to have a full set of lengths
		assert(rule.NumProductions > 0);
		uint32_t length = numeric_limits<uint32_t>::max();
		bool everythingKnown = true;
		for (uint32_t i = 0; i < rule.NumProductions; i++)
		{
			int32_t first = rule.Productions[i][0];
			int32_t second = rule.Productions[i][1];

			// Do we know the first production non-terminal length?
			if ((*lengths)[first] == 0)
			{
				nonTerminals.push_back(first);
				everythingKnown = false;
				break;
			}

			length = (*lengths)[first];

			// Do we know the optional second production non-terminal length?
			if (second != -1)
			{
				if ((*lengths)[second] == 0)
				{
					nonTerminals.push_back(second);
					everythingKnown = false;
					break;
				}

				length += (*lengths)[second];
			}
		}

		if (everythingKnown)
		{
			(*lengths)[nonTerminal] = length;
			nonTerminals.pop_back();
		}
	}
}

static bool Match(int32_t nonterminal, size_t position, const char* message, const vector<Rule>& productionRules, const vector<uint32_t>& lengths)
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

void Puzzle19_A_2020()
{
	vector<Rule> productionRules;
	productionRules.resize(256);
	while (PuzzleInput::PeekChar() != '\n')
	{
		ParseRule(&productionRules);
	}
	Parse::DiscardExpected("\n");

	vector<uint32_t> lengths(150);
	GetLengths(0, productionRules, &lengths);

	int32_t answer = 0;

	size_t expectedLength = lengths[0];

	vector<char> line(100);
	while (PuzzleInput::NextLine())
	{
		int32_t lineLength = Parse::ReadNonEmptyLine(line.data(), line.size());
		if ((lineLength == expectedLength) && Match(0, 0, line.data(), productionRules, lengths))
		{
			answer++;
		}
	}

	PuzzleOutput::Submit(2020, 19, 1, answer);
}

void Puzzle19_B_2020()
{
	vector<Rule> productionRules;
	productionRules.resize(256);
	while (PuzzleInput::PeekChar() != '\n')
	{
		ParseRule(&productionRules);
	}
	Parse::DiscardExpected("\n");

	vector<uint32_t> lengths(150);
	GetLengths(0, productionRules, &lengths);

	int32_t answer = 0;

	size_t chunkSize = lengths[42];
	assert(lengths[31] == chunkSize);

	vector<char> line(100);
	while (PuzzleInput::NextLine())
	{
		int32_t lineLength = Parse::ReadNonEmptyLine(line.data(), line.size());

		const char* chunk = line.data();
		const char* endOfLine = chunk + lineLength;
		assert((lineLength % chunkSize) == 0);

		// Count 42s
		size_t symbol42Count = 0;
		while (chunk != endOfLine)
		{
			if (Match(42, 0, chunk, productionRules, lengths))
			{
				symbol42Count++;
				chunk += chunkSize;
			}
			else
			{
				break;
			}
		}

		// Count 31s
		bool valid = true;
		size_t symbol31Count = 0;
		while (chunk != endOfLine)
		{
			if (Match(31, 0, chunk, productionRules, lengths))
			{
				symbol31Count++;
				chunk += chunkSize;
			}
			else
			{
				valid = false;
				break;
			}
		}

		if (valid && (symbol31Count > 0) && (symbol42Count > symbol31Count))
		{
			answer++;
		}
	}

	PuzzleOutput::Submit(2020, 19, 2, answer);
}
