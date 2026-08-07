#include "stdafx.h"

using namespace std;

namespace Puzzle10_2016_Types
{
	enum
	{
		NUM_BOTS = 256,
		NUM_OUTPUTS = 32,
	};

	struct Bot
	{
		int16_t WaitingFor = 2;
		int16_t Values[2];
		int16_t LowTo = -1;
		int16_t HighTo = -1;
	};

	struct Puzzle
	{
		array<Bot, NUM_BOTS> Bots;
		array<int16_t, NUM_OUTPUTS> Outputs;
	};
}

using namespace Puzzle10_2016_Types;

static void ReadPuzzle(Puzzle* puzzle, int16_t outputIdOffset)
{
	Puzzle& p = *puzzle;

	char line[128];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line);

		if (line[0] == 'v')
		{
			int16_t value, botId;
			int scanned = sscanf(line, "value %hd goes to bot %hd",
				&value,
				&botId);
			assert(scanned == 2);
			(void)scanned;

			Bot& b = p.Bots[botId];
			b.Values[2 - b.WaitingFor] = value;
			b.WaitingFor--;
		}
		else if (line[0] == 'b')
		{
			int16_t botId, lowTo, highTo;
			char lowToEntity[16] = { 0 };
			char highToEntity[16] = { 0 };
			int scanned = sscanf(line, "bot %hd gives low to %s %hd and high to %s %hd",
				&botId,
				lowToEntity,
				&lowTo,
				highToEntity,
				&highTo);
			assert(scanned == 5);
			(void)scanned;

			assert(botId < outputIdOffset);
			Bot& b = p.Bots[botId];
			b.LowTo = lowTo;
			b.HighTo = highTo;

			if (lowToEntity == "output"sv)
			{
				b.LowTo += outputIdOffset;
			}
			if (highToEntity == "output"sv)
			{
				b.HighTo += outputIdOffset;
			}
		}
		else
		{
			assert(false);
		}
	}
}

static void Simulate(Puzzle* p, int16_t outputIdOffset)
{
	vector<int16_t> executionStack;
	executionStack.reserve(NUM_BOTS);

	for (int16_t i = 0; i < (int16_t)p->Bots.size(); i++)
	{
		if (p->Bots[i].WaitingFor == 0)
		{
			executionStack.push_back(i);
		}
	}

	auto GiveValueToBot = [&](int16_t value, int16_t botId)
		{
			if (botId >= outputIdOffset)
			{
				p->Outputs[botId - outputIdOffset] = value;
			}
			else
			{
				Bot& b = p->Bots[botId];
				assert(b.WaitingFor > 0);
				b.Values[2 - b.WaitingFor] = value;
				if (--b.WaitingFor == 0)
				{
					executionStack.push_back(botId);
				}
			}
		};

	while (executionStack.empty() == false)
	{
		Bot& b = p->Bots[executionStack.back()];
		executionStack.pop_back();

		GiveValueToBot(min(b.Values[0], b.Values[1]), b.LowTo);
		GiveValueToBot(max(b.Values[0], b.Values[1]), b.HighTo);
	}
}

void Puzzle10_A_2016()
{
	const int16_t outputIdOffset = 1000;

	Puzzle p;
	ReadPuzzle(&p, outputIdOffset);
	Simulate(&p, outputIdOffset);

	int32_t answer = 0;
	for (const auto& bot : p.Bots)
	{
		if ((bot.WaitingFor == 0) &&
			(min(bot.Values[0], bot.Values[1]) == 17) &&
			(max(bot.Values[0], bot.Values[1]) == 61))
		{
			answer = (int16_t)distance(&(*p.Bots.cbegin()), &bot);
			break;
		}
	}

	return PuzzleOutput::Submit(2016, 10, 1, answer);
}

void Puzzle10_B_2016()
{
	const int16_t outputIdOffset = 1000;

	Puzzle p;
	ReadPuzzle(&p, outputIdOffset);
	Simulate(&p, outputIdOffset);

	int32_t answer = p.Outputs[0] * p.Outputs[1] * p.Outputs[2];
	return PuzzleOutput::Submit(2016, 10, 2, answer);
}
