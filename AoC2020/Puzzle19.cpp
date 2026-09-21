#include "stdafx.h"

using namespace std;

namespace Puzzle19_2020_Types
{
	struct Rule
	{
		char Terminal = '\0';
		SmallVector<SmallVector<int32_t, 2>, 2> Productions;
	};

	enum class KnownMatchState
	{
		Unknown,
		Matches,
		DoesntMatch,
	};

	struct MatchState
	{
		int32_t NonTerminal;
		int32_t Position;
		int32_t ProductionIndex;
		int32_t ProductionNextNonterminalIndex;
		KnownMatchState SubMatchState;

		KnownMatchState *Output;
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

			rule.Productions.PushBack({});
			SmallVector<int32_t, 2>& newProductions = rule.Productions.Back();
			newProductions.PushBack(firstNonterminal);

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
				newProductions.PushBack(secondNonterminal);
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
		assert(rule.Productions.size() > 0);
		uint32_t length = numeric_limits<uint32_t>::max();
		bool everythingKnown = true;
		for (int32_t i = 0; i < rule.Productions.size(); i++)
		{
			assert(rule.Productions[i].size() > 0);

			length = 0;
			for (int32_t j = 0; j < rule.Productions[i].size(); j++)
			{
				int32_t nextNonTerminal = rule.Productions[i][j];
				uint32_t subLength = (*lengths)[nextNonTerminal];
				if (subLength == 0)
				{
					nonTerminals.push_back(nextNonTerminal);
					everythingKnown = false;
					break;
				}

				length += subLength;
			}

			if (everythingKnown == false)
				break;
		}

		if (everythingKnown)
		{
			(*lengths)[nonTerminal] = length;
			nonTerminals.pop_back();
		}
	}
}

static bool Match(int32_t startingTerminal, int32_t startingPosition, const char* message, const vector<Rule>& productionRules, const vector<uint32_t>& lengths)
{
	vector<MatchState> executionStack;
	executionStack.reserve(16);

	KnownMatchState fullMatch = KnownMatchState::Unknown;

	MatchState start;
	start.NonTerminal = startingTerminal;
	start.Position = startingPosition;
	start.ProductionIndex = 0;
	start.ProductionNextNonterminalIndex = 0;
	start.SubMatchState = KnownMatchState::Unknown;
	start.Output = &fullMatch;
	executionStack.push_back(start);

	while (executionStack.empty() == false)
	{
		MatchState& current = executionStack.back();

		const Rule& rule = productionRules[current.NonTerminal];
		if (rule.Terminal)
		{
			*current.Output = (message[current.Position] == rule.Terminal) ? KnownMatchState::Matches : KnownMatchState::DoesntMatch;
			executionStack.pop_back();
			continue;
		}

		int32_t currentMatchedTokenLength = 0;
		
		// Have we got a match?
		if (current.SubMatchState == KnownMatchState::Matches)
		{
			currentMatchedTokenLength = lengths[rule.Productions[current.ProductionIndex][current.ProductionNextNonterminalIndex]];

			// Try to match the next token in this production
			current.ProductionNextNonterminalIndex++;

			if (current.ProductionNextNonterminalIndex == rule.Productions[current.ProductionIndex].size())
			{
				// Full match for this production
				*current.Output = KnownMatchState::Matches;
				executionStack.pop_back();
				continue;
			}
		}

		// Have we got a mismatch?
		if (current.SubMatchState == KnownMatchState::DoesntMatch)
		{
			// Try the next production
			current.ProductionIndex++;
			current.ProductionNextNonterminalIndex = 0;

			if (current.ProductionIndex == rule.Productions.size())
			{
				// Full mismatch
				*current.Output = KnownMatchState::DoesntMatch;
				executionStack.pop_back();
				continue;
			}
		}

		// Kick off a sub-match
		MatchState subMatch;
		subMatch.NonTerminal = rule.Productions[current.ProductionIndex][current.ProductionNextNonterminalIndex];
		subMatch.Position = current.Position + currentMatchedTokenLength;
		subMatch.ProductionIndex = 0;
		subMatch.ProductionNextNonterminalIndex = 0;
		subMatch.SubMatchState = KnownMatchState::Unknown;
		subMatch.Output = &current.SubMatchState;
		executionStack.push_back(subMatch);

		current.SubMatchState = KnownMatchState::Unknown;
	}

	return fullMatch == KnownMatchState::Matches;
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
