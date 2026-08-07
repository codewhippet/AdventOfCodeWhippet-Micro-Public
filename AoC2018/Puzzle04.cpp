#include "stdafx.h"
#include <chrono>

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle04_2018_Types
{
	enum EventType : uint32_t
	{
		StartsShift,
		FallsAsleep,
		WakesUp,
	};

	struct Event
	{
		uint32_t SortIndex;
		EventType Type;
		int32_t Data = 0;

		auto operator<=>(const Event&) const = default;
	};

	struct GuardSleepiestMinute
	{
		int32_t SleepsForMinute;
		int32_t SleepiestMinute;
		int32_t GuardId;

		auto operator<=>(const GuardSleepiestMinute&) const = default;
	};
}

using namespace Puzzle04_2018_Types;

static vector<Event> ParseShiftLog()
{
	vector<Event> ret;

	const size_t startOfLogType = 19;
	const size_t startOfGuardId = 26;
	const size_t startOfMonth = 6;
	const size_t startOfMinutes = 15;
	const size_t endOfMinutes = 17;

	char line[64];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line);

		Event e;
		switch (line[startOfLogType])
		{
		case 'G':
			e.Type = EventType::StartsShift;
			e.Data = Parse::GetInt32(&line[startOfGuardId]);
			break;
		case 'f':
			e.Type = EventType::FallsAsleep;
			e.Data = (line[startOfMinutes] - '0') * 10 + (line[startOfMinutes + 1] - '0');
			break;
		case 'w':
			e.Type = EventType::WakesUp;
			e.Data = (line[startOfMinutes] - '0') * 10 + (line[startOfMinutes + 1] - '0');
			break;
		}

		e.SortIndex = 0;
		for (size_t i = startOfMonth; i < endOfMinutes; i++)
		{
			char c = line[i];
			if (isdigit(c))
			{
				e.SortIndex = e.SortIndex * 10 + (c - '0');
			}
		}

		ret.push_back(e);
	}

	return ret;
}

static HashMap<uint32_t, uint32_t> CalculateSleepTotals(const vector<Event>& shiftLog)
{
	HashMap<uint32_t, uint32_t> guardSleepTotals(64, 0);

	int32_t currentGuard = 0;
	for (size_t i = 0; i < shiftLog.size(); /***/)
	{
		switch (shiftLog[i].Type)
		{
		case EventType::StartsShift:
			currentGuard = shiftLog[i].Data;
			guardSleepTotals.Insert(currentGuard, 0); // Default to 0 when first seen
			i++;
			break;

		case EventType::FallsAsleep:
			assert(shiftLog[i + 1].Type == EventType::WakesUp);
			uint32_t sleepMinutes = shiftLog[i + 1].Data - shiftLog[i].Data;
			guardSleepTotals.At(currentGuard) += sleepMinutes;
			i += 2;
			break;
		}
	}

	return guardSleepTotals;
}

static GuardSleepiestMinute GetSleepiestMinute(const vector<Event>& shiftLog, int32_t guardId)
{
	array<int32_t, 60> sleepByMinute = {};

	int32_t currentGuard = 0;
	for (size_t i = 0; i < shiftLog.size(); /***/)
	{
		switch (shiftLog[i].Type)
		{
		case EventType::StartsShift:
			currentGuard = shiftLog[i].Data;
			i++;
			break;

		case EventType::FallsAsleep:
			if (currentGuard == guardId)
			{
				for (int32_t min = shiftLog[i].Data; min < shiftLog[i + 1].Data; min++)
				{
					sleepByMinute[min]++;
				}
			}
			i += 2;
			break;
		}
	}

	int32_t sleepiestMinute = static_cast<int32_t>(distance(sleepByMinute.begin(), ranges::max_element(sleepByMinute)));

	return { sleepByMinute[sleepiestMinute], sleepiestMinute, guardId };
}

void Puzzle04_A_2018()
{
	vector<Event> shiftLog = ParseShiftLog();
	ranges::sort(shiftLog);

	HashMap<uint32_t, uint32_t> guardSleepTotals = CalculateSleepTotals(shiftLog);

	int32_t sleepiestGuard = ranges::max(guardSleepTotals |
		views::transform([](const pair<uint32_t, uint32_t>& p)
			{
				return pair<uint32_t, uint32_t>{ p.second, p.first };
			}))
		.second;

	GuardSleepiestMinute sleepiestMinuteData = GetSleepiestMinute(shiftLog, sleepiestGuard);

	int32_t answer = sleepiestGuard * sleepiestMinuteData.SleepiestMinute;

	PuzzleOutput::Submit(2018, 4, 1, answer);
}

void Puzzle04_B_2018()
{
	vector<Event> shiftLog = ParseShiftLog();
	ranges::sort(shiftLog);

	HashMap<uint32_t, uint32_t> guardSleepTotals = CalculateSleepTotals(shiftLog);

	GuardSleepiestMinute mostSleepProneGuard = {};
	for (const auto& guardSleep : guardSleepTotals)
	{
		GuardSleepiestMinute sleepiestMinuteData = GetSleepiestMinute(shiftLog, guardSleep.first);
		if (sleepiestMinuteData > mostSleepProneGuard)
		{
			mostSleepProneGuard = sleepiestMinuteData;
		}
	}

	int32_t answer = mostSleepProneGuard.GuardId * mostSleepProneGuard.SleepiestMinute;

	PuzzleOutput::Submit(2018, 4, 2, answer);
}
