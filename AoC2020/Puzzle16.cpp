#include "stdafx.h"

using namespace std;

namespace Puzzle16_2020_Types
{
	struct TicketInfo 
	{
		vector<pair<string, function<bool(uint32_t)>>> FieldRules;
		vector<uint32_t> MyTicket;
		vector<vector<uint32_t>> NearbyTickets;
	};
}

using namespace Puzzle16_2020_Types;

static function<bool(uint32_t)> MakeRule(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
	return [a, b, c, d](uint32_t v) { return ((v >= a) && (v <= b)) || ((v >= c) && (v <= d)); };
}

static TicketInfo ReadPuzzle()
{
	TicketInfo info;

	info.FieldRules.reserve(20);
	while (PuzzleInput::PeekChar() != '\n')
	{
		string label;
		label.reserve(32);
		for (char c : Parse::ReadUntilSeen(':'))
			label += c;

		uint32_t a = Parse::GetUint32();
		uint32_t b = Parse::GetUint32();
		uint32_t c = Parse::GetUint32();
		uint32_t d = Parse::GetUint32();
		info.FieldRules.push_back({ label, MakeRule(a, b, c, d) });
		Parse::DiscardExpected("\n");
	}

	Parse::DiscardExpected("\nyour ticket:\n");

	info.MyTicket.reserve(20);
	while (PuzzleInput::PeekChar() != '\n')
	{
		info.MyTicket.push_back(Parse::GetUint32());
	}

	Parse::DiscardExpected("\n\nnearby tickets:\n");

	info.NearbyTickets.reserve(256);
	while (PuzzleInput::NextLine())
	{
		vector<uint32_t> ticket;
		ticket.reserve(20);
		while (PuzzleInput::PeekChar() != '\n')
		{
			ticket.push_back(Parse::GetUint32());
		}
		info.NearbyTickets.push_back(move(ticket));
	}

	return info;
}

static bool TryGetUint32(uint32_t* out)
{
	while (!isdigit(PuzzleInput::PeekChar()))
	{
		int c = PuzzleInput::GetChar();
		if (c == EOF)
			return false;
	}

	int32_t result = 0;
	while (isdigit(PuzzleInput::PeekChar()))
	{
		result = (result * 10) + static_cast<int32_t>(PuzzleInput::GetChar() - '0');
	}

	*out = result;
	return true;
}

void Puzzle16_A_2020()
{
	vector<function<bool(uint32_t)>> fieldRules;
	fieldRules.reserve(20);

	while (PuzzleInput::PeekChar() != '\n')
	{
		uint32_t a = Parse::GetUint32();
		uint32_t b = Parse::GetUint32();
		uint32_t c = Parse::GetUint32();
		uint32_t d = Parse::GetUint32();
		fieldRules.push_back(MakeRule(a, b, c, d));
		Parse::DiscardExpected("\n");
	}

	Parse::DiscardExpected("\nyour ticket:\n");
	PuzzleInput::DropLine();
	Parse::DiscardExpected("\n\nnearby tickets:\n");

	int32_t answer = 0;

	uint32_t value;
	while (TryGetUint32(&value))
	{
		bool invalid = ranges::none_of(fieldRules, [&](const auto& rule) { return rule(value); });
		if (invalid)
		{
			answer += value;
		}
	}

	PuzzleOutput::Submit(2020, 16, 1, answer);
}

void Puzzle16_B_2020()
{
	TicketInfo info = ReadPuzzle();

	vector<vector<uint32_t>> ticketValidityMasks;
	ticketValidityMasks.resize(info.NearbyTickets.size());
	for (size_t ticketIndex = 0; ticketIndex < info.NearbyTickets.size(); ticketIndex++)
	{
		const vector<uint32_t>& ticket = info.NearbyTickets[ticketIndex];

		vector<uint32_t> ticketValueValidity;
		ticketValueValidity.reserve(20);
		for (uint32_t value : ticket)
		{
			uint32_t validityMask = 0;
			for (size_t fieldIndex = 0; fieldIndex < info.FieldRules.size(); fieldIndex++)
			{
				uint32_t valid = info.FieldRules[fieldIndex].second(value) ? 1 : 0;
				validityMask |= (valid << fieldIndex);
			}
			ticketValueValidity.push_back(validityMask);
		}

		if (find(ticketValueValidity.begin(), ticketValueValidity.end(), 0u) == ticketValueValidity.end())
		{
			ticketValidityMasks[ticketIndex].swap(ticketValueValidity);
		}
	}

	vector<uint32_t> globalValidity;
	globalValidity.resize(info.FieldRules.size());
	fill(globalValidity.begin(), globalValidity.end(), ~0);

	for (vector<vector<uint32_t>>::const_reference ticketValidity : ticketValidityMasks)
	{
		for (size_t valueIndex = 0; valueIndex < ticketValidity.size(); valueIndex++)
		{
			globalValidity[valueIndex] &= ticketValidity[valueIndex];
		}
	}

	vector<size_t> fieldIndexToValueIndex;
	fieldIndexToValueIndex.resize(info.FieldRules.size());
	while (true)
	{
		vector<uint32_t>::iterator canBeMapped = find_if(globalValidity.begin(), globalValidity.end(),
			[](uint32_t v) { return popcount(v) == 1; });
		if (canBeMapped == globalValidity.end())
		{
			break;
		}

		uint32_t validityMask = *canBeMapped;

		uint32_t fieldIndex = countr_zero(validityMask);
		fieldIndexToValueIndex[fieldIndex] = distance(globalValidity.begin(), canBeMapped);

		for_each(globalValidity.begin(), globalValidity.end(), [validityMask](uint32_t& v) { v &= ~validityMask; });
	}

	int64_t answer = 1;
	for (size_t fieldIndex = 0; fieldIndex < info.FieldRules.size(); fieldIndex++)
	{
		if (info.FieldRules[fieldIndex].first.substr(0, sizeof("departure") - 1) == "departure")
		{
			answer *= info.MyTicket[fieldIndexToValueIndex[fieldIndex]];
		}
	}

	PuzzleOutput::Submit(2020, 16, 2, answer);
}
