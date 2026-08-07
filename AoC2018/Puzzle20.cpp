#include "stdafx.h"

using namespace std;

namespace Puzzle20_2018_Types
{
}

using namespace Puzzle20_2018_Types;

void Puzzle20_A_2018()
{
	vector<pair<Vec2Byte, uint16_t>> stack;
	stack.reserve(512);

	HashMap<Vec2Byte, uint16_t> roomDistances(32 * 1024, { numeric_limits<int8_t>::min(), numeric_limits<int8_t>::min() });
	auto visitRoom = [&](const pair<Vec2Byte, uint16_t>& room)
		{
			uint16_t existingDistance;
			bool existingDistanceIsShortest = roomDistances.TryFind(room.first, &existingDistance) && (existingDistance <= room.second);
			if (!existingDistanceIsShortest)
			{
				roomDistances.Set(room.first, room.second);
			}
		};

	pair<Vec2Byte, uint16_t> current = {};
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
			current.first += Vec2Byte::East();
			current.second++;
			visitRoom(current);
			break;

		case 'N':
			current.first += Vec2Byte::North();
			current.second++;
			visitRoom(current);
			break;

		case 'S':
			current.first += Vec2Byte::South();
			current.second++;
			visitRoom(current);
			break;

		case 'W':
			current.first += Vec2Byte::West();
			current.second++;
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
		maxShortesDistance.Update(roomDist.second);
	}

	int32_t answer = maxShortesDistance.Get();
	PuzzleOutput::Submit(2018, 20, 1, answer);
}

void Puzzle20_B_2018()
{
	vector<pair<Vec2Byte, uint16_t>> stack;
	stack.reserve(512);

	HashMap<Vec2Byte, uint16_t> roomDistances(32 * 1024, { numeric_limits<int8_t>::min(), numeric_limits<int8_t>::min() });
	auto visitRoom = [&](const pair<Vec2Byte, uint16_t>& room)
		{
			uint16_t existingDistance;
			bool existingDistanceIsShortest = roomDistances.TryFind(room.first, &existingDistance) && (existingDistance <= room.second);
			if (!existingDistanceIsShortest)
			{
				roomDistances.Set(room.first, room.second);
			}
		};

	pair<Vec2Byte, uint16_t> current = {};
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
			current.first += Vec2Byte::East();
			current.second++;
			visitRoom(current);
			break;

		case 'N':
			current.first += Vec2Byte::North();
			current.second++;
			visitRoom(current);
			break;

		case 'S':
			current.first += Vec2Byte::South();
			current.second++;
			visitRoom(current);
			break;

		case 'W':
			current.first += Vec2Byte::West();
			current.second++;
			visitRoom(current);
			break;

		case '|':
			current = stack.back();
			break;
		}
	}

	int32_t answer = static_cast<int32_t>(ranges::count_if(roomDistances, [](const auto& roomDist) { return roomDist.second >= 1000; }));
	PuzzleOutput::Submit(2018, 20, 2, answer);
}
