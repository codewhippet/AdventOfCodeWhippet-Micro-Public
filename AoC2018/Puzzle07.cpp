#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle07_2018_Types
{
	struct Step
	{
		char Id = '\0';
		int32_t BlockedByCount = 0;
		SmallVector<size_t, 8> Unblocks;
		int32_t WorkRequired = 0;
		int32_t WorkPerformed = 0;
	};

	struct Worker
	{
		size_t CurrentTask = numeric_limits<size_t>::max();
	};
}

using namespace Puzzle07_2018_Types;

static vector<Step> ParseSteps()
{
	vector<Step> steps(26);
	for (size_t i = 0; i < steps.size(); i++)
	{
		steps[i].Id = static_cast<char>('A' + i);
	}

	char line[64];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line);
		size_t stepMustBeFinished = line[5] - 'A';
		size_t stepCanBegin = line[36] - 'A';

		steps[stepMustBeFinished].Unblocks.PushBack(stepCanBegin);
		steps[stepCanBegin].BlockedByCount++;
	}

	return steps;
}

void Puzzle07_A_2018()
{
	vector<Step> steps = ParseSteps();

	set<size_t> executionQueue;
	ranges::copy(IndicesOf(steps) | views::filter([&](size_t i) { return steps[i].BlockedByCount == 0; }), inserter(executionQueue, executionQueue.end()));

	string answer;
	answer.reserve(steps.size());
	while (executionQueue.empty() == false)
	{
		size_t stepToExecute = *executionQueue.begin();
		executionQueue.erase(executionQueue.begin());

		answer += steps[stepToExecute].Id;
		for (size_t unblocks : steps[stepToExecute].Unblocks)
		{
			if (--steps[unblocks].BlockedByCount == 0)
			{
				executionQueue.insert(unblocks);
			}
		}
	}

	PuzzleOutput::Submit(2018, 7, 1, answer.c_str());
}

void Puzzle07_B_2018()
{
	const int32_t stepBaseCost = 60;
	const size_t numWorkers = 5;
	const size_t noCurrentTask = numeric_limits<size_t>::max();

	vector<Step> steps = ParseSteps();
	for (size_t i = 0; i < steps.size(); i++)
	{
		steps[i].WorkRequired = static_cast<int32_t>(stepBaseCost + i + 1);
	}

	set<size_t> executionQueue;
	ranges::copy(IndicesOf(steps) | views::filter([&](size_t i) { return steps[i].BlockedByCount == 0; }), inserter(executionQueue, executionQueue.end()));

	vector<Worker> workers(numWorkers, { noCurrentTask });

	int32_t elapsedTime = 0;
	while (true)
	{
		// Idle workers pick up new tasks
		for (Worker& w : workers)
		{
			if (w.CurrentTask == noCurrentTask)
			{
				auto headOfQueue = executionQueue.begin();
				if (headOfQueue != executionQueue.end())
				{
					size_t stepToExecute = *headOfQueue;
					w.CurrentTask = stepToExecute;
					executionQueue.erase(headOfQueue);
				}
			}
		}

		// If there's nothing to do, we're done
		if (ranges::all_of(workers, [&](const Worker& w) { return w.CurrentTask == noCurrentTask; }))
		{
			break;
		}

		// Workers progress tasks
		int32_t completedThisFrame = 0;
		for (Worker& w : workers)
		{
			if (w.CurrentTask != noCurrentTask)
			{
				Step& step = steps[w.CurrentTask];
				if (++step.WorkPerformed == step.WorkRequired)
				{
					for (size_t followingTask : step.Unblocks)
					{
						if (--steps[followingTask].BlockedByCount == 0)
						{
							executionQueue.insert(followingTask);
						}
					}
					w.CurrentTask = noCurrentTask;
					completedThisFrame++;
				}
			}
		}
		assert(completedThisFrame < 2);

		elapsedTime++;
	}

	int32_t answer = elapsedTime;
	PuzzleOutput::Submit(2018, 7, 2, answer);
}
