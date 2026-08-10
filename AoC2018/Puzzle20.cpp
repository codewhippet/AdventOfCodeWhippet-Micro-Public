#include "stdafx.h"

using namespace std;

namespace Puzzle20_2018_Types
{
	struct CurrentState
	{
		// Nothing goes further than [-128, 127] so we just let the co-ordinates wrap for  64Kib perfect hash
		uint16_t Distance;
		uint8_t X;
		uint8_t Y;
	};
}

using namespace Puzzle20_2018_Types;

void Puzzle20_A_2018()
{
	vector<CurrentState> stack;
	stack.reserve(512);

	vector<uint16_t> roomDistances(64 * 1024, numeric_limits<uint16_t>::max());
	auto visitRoom = [&](const CurrentState& room)
		{
			size_t distanceIndex = (room.X << 8) | room.Y;
			uint16_t existingDistance = roomDistances[distanceIndex];
			if (room.Distance < existingDistance)
			{
				roomDistances[distanceIndex] = room.Distance;
			}
		};

	CurrentState current = {};
	visitRoom(current);

	for (int c = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		switch (static_cast<char>(c))
		{
		case '(':
			stack.push_back(current);
			break;

		case ')':
			stack.pop_back();
			break;

		case 'E':
			current.X++;
			current.Distance++;
			visitRoom(current);
			break;

		case 'N':
			current.Y--;
			current.Distance++;
			visitRoom(current);
			break;

		case 'S':
			current.Y++;
			current.Distance++;
			visitRoom(current);
			break;

		case 'W':
			current.X--;
			current.Distance++;
			visitRoom(current);
			break;

		case '|':
			current = stack.back();
			break;
		}
	}

	MaxValue<uint16_t> maxShortesDistance;
	for (const auto& roomDist : roomDistances)
	{
		if (roomDist != numeric_limits<uint16_t>::max())
		{
			maxShortesDistance.Update(roomDist);
		}
	}

	int32_t answer = maxShortesDistance.Get();
	PuzzleOutput::Submit(2018, 20, 1, answer);
}

void Puzzle20_B_2018()
{
	vector<CurrentState> stack;
	stack.reserve(512);

	vector<uint16_t> roomDistances(64 * 1024, numeric_limits<uint16_t>::max());
	auto visitRoom = [&](const CurrentState& room)
		{
			size_t distanceIndex = (room.X << 8) | room.Y;
			uint16_t existingDistance = roomDistances[distanceIndex];
			if (room.Distance < existingDistance)
			{
				roomDistances[distanceIndex] = room.Distance;
			}
		};

	CurrentState current = {};
	visitRoom(current);

	for (int c = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		switch (static_cast<char>(c))
		{
		case '(':
			stack.push_back(current);
			break;

		case ')':
			stack.pop_back();
			break;

		case 'E':
			current.X++;
			current.Distance++;
			visitRoom(current);
			break;

		case 'N':
			current.Y--;
			current.Distance++;
			visitRoom(current);
			break;

		case 'S':
			current.Y++;
			current.Distance++;
			visitRoom(current);
			break;

		case 'W':
			current.X--;
			current.Distance++;
			visitRoom(current);
			break;

		case '|':
			current = stack.back();
			break;
		}
	}

	int32_t answer = static_cast<int32_t>(ranges::count_if(roomDistances,
		[](const auto& roomDist)
		{
			return (roomDist != numeric_limits<uint16_t>::max()) && (roomDist >= 1000);
		}));
	PuzzleOutput::Submit(2018, 20, 2, answer);
}
