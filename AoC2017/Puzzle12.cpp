#include "stdafx.h"

using namespace std;

namespace Puzzle12_2017_Types
{
	struct Pipe
	{
		array<int16_t, 7> ConnectedTo;
		int8_t NumConnections = 0;
		int8_t InGroup = false;
	};
}

using namespace Puzzle12_2017_Types;

static int32_t GetConnectedGroup(vector<Pipe>* p, int16_t startFrom)
{
	vector<Pipe>& pipes = (*p);

	vector<int16_t> searchQueue;
	searchQueue.reserve(512);

	searchQueue.push_back(startFrom);
	pipes[startFrom].InGroup = true;
	int32_t addedIntoGroup = 1;

	for (size_t i = 0; i < searchQueue.size(); i++)
	{
		const Pipe& current = pipes[searchQueue[i]];
		for (int8_t connection = 0; connection < current.NumConnections; connection++)
		{
			int16_t neighbourId = current.ConnectedTo[connection];
			if (!pipes[neighbourId].InGroup)
			{
				searchQueue.push_back(neighbourId);
				pipes[neighbourId].InGroup = true;
				addedIntoGroup++;
			}
		}
	}

	return addedIntoGroup;
}

void Puzzle12_A_2017()
{
	vector<Pipe> pipes;
	pipes.reserve(2048);

	while (PuzzleInput::NextLine())
	{
		pipes.push_back({});
		Pipe& pipe = pipes.back();

		int16_t connectedTo;
		while (Parse::TryGetInt16FromLine(&connectedTo))
		{
			pipe.ConnectedTo[pipe.NumConnections++] = connectedTo;
		}
	}

	int32_t answer = GetConnectedGroup(&pipes, 0);
	PuzzleOutput::Submit(2017, 12, 1, answer);
}

void Puzzle12_B_2017()
{
	vector<Pipe> pipes;
	pipes.reserve(2048);

	while (PuzzleInput::NextLine())
	{
		pipes.push_back({});
		Pipe& pipe = pipes.back();

		int16_t connectedTo;
		while (Parse::TryGetInt16FromLine(&connectedTo))
		{
			pipe.ConnectedTo[pipe.NumConnections++] = connectedTo;
		}
	}

	int32_t answer = 0;
	for (size_t i = 0; i < pipes.size(); i++)
	{
		if (!pipes[i].InGroup)
		{
			GetConnectedGroup(&pipes, static_cast<int16_t>(i));
			answer++;
		}
	}

	PuzzleOutput::Submit(2017, 12, 2, answer);
}
