#include "stdafx.h"
#include "Intputer.h"
#include <iostream>

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle25_2019_Types
{
	struct Room
	{
		string Id;
		vector<string> Doors;
		vector<string> Items;
		string Description;
	};

	struct Ship
	{
		map<string, Room> Rooms;
		string StartingRoom;
		pair<string, string> Goal;

		map<string, set<string>> Neighbours;
		map<pair<string, string>, string> DoorToUse;

		map<string, string> ItemLocations;
	};

	enum class ParsingState
	{
		None,
		Doors,
		Items,
	};

	struct ExploreState
	{
		string CurrentRoom;
		string CameFromRoom;
		string DoorUsedToComeHere;
		set<string> DoorsToExplore;
	};
}

using namespace Puzzle25_2019_Types;

#if 0
static void InteractiveMode(uIntputer<int64_t>* puter)
{
	while (true)
	{
		puter->GetWriteQueue()->clear();

		auto exec = puter->Execute();
		assert(exec != uIntputerExecutionResult::Exception);

		ranges::for_each(*puter->GetWriteQueue() | views::filter([](int64_t c) { return c < 256; }),
			[](int64_t c) { putc((int)c, stdout); });

		printf("$: ");
		string line;
		getline(cin, line);

		if (exec == uIntputerExecutionResult::Finished)
			break;

		ranges::copy(line, back_inserter(*puter->GetReadQueue()));
		puter->GetReadQueue()->push_back('\n');
	}
}
#endif

static Room ParseRoom(const string& description)
{
	Room r;
	r.Description = description;

	istringstream input(description);
	ParsingState state = ParsingState::None;
	for (const string& line : ReadEachLine(input))
	{
		if (line.starts_with("=="sv))
		{
			r.Id = line;
			r.Id.erase(remove(r.Id.begin(), r.Id.end(), '='), r.Id.end());
			r.Id.erase(remove(r.Id.begin(), r.Id.end(), ' '), r.Id.end());
		}
		else if (line == "Doors here lead:"sv)
		{
			state = ParsingState::Doors;
		}
		else if (line == "Items here:"sv)
		{
			state = ParsingState::Items;
		}
		else if (line.starts_with("- "sv))
		{
			switch (state)
			{
			case ParsingState::Doors:
				r.Doors.push_back(line.substr(2));
				break;
			case ParsingState::Items:
				r.Items.push_back(line.substr(2));
				break;
			}
		}
	}

	return r;
}

static void Explore(uIntputerWithIO<int64_t> puter, Ship* ship)
{
	const map<string, string> goBack =
	{
		{ "north", "south" },
		{ "east", "west" },
		{ "south", "north" },
		{ "west", "east" },
	};

	vector<ExploreState> explorationQueue;
	explorationQueue.push_back({});

	while (explorationQueue.empty() == false)
	{
		ExploreState& current = explorationQueue.back();

		// Move to the room and get the room details if needed
		if (current.CurrentRoom.empty())
		{
			if (!current.DoorUsedToComeHere.empty())
			{
				ranges::copy(current.DoorUsedToComeHere, back_inserter(*puter.GetReadQueue()));
				puter.GetReadQueue()->push_back('\n');
			}

			auto exec = puter.Execute();
			assert(exec != uIntputerExecutionResult::Exception);
			(void)exec;

			string output;
			ranges::copy(*puter.GetWriteQueue() | views::transform([](int64_t c) { return (char)c; }), back_inserter(output));
			puter.GetWriteQueue()->clear();

			Room newRoom = ParseRoom(output);
			assert(newRoom.Items.size() < 2);

			current.CurrentRoom = newRoom.Id;

			if (ship->Rooms.empty())
			{
				ship->StartingRoom = newRoom.Id;
			}

			if (newRoom.Id == current.CameFromRoom)
			{
				// The finishing room won't allow you to progress without the right item
				ship->Goal = { newRoom.Id, current.DoorUsedToComeHere };
			}
			else
			{
				ship->Neighbours[current.CameFromRoom].insert(newRoom.Id);
				ship->DoorToUse[{ current.CameFromRoom, newRoom.Id }] = current.DoorUsedToComeHere;
			}

			if (ship->Rooms.insert({ newRoom.Id, newRoom }).second)
			{
				// New room, record the details
				ranges::copy(newRoom.Items
					| views::transform([&](const string& s) -> pair<string, string>
						{
							return { s, newRoom.Id };
						}),
					inserter(ship->ItemLocations, ship->ItemLocations.end()));

				ranges::copy(newRoom.Doors, inserter(current.DoorsToExplore, current.DoorsToExplore.begin()));
			}
			else
			{
				// Room that we've seen before, don't bother exploring.
				// Should happen by default because we haven't populated current.DoorsToExplore
			}
		}

		// Have we finished exploring all of the doors from this room?
		if (current.DoorsToExplore.empty())
		{
			// Go back the way we came (if we did indeed transition rooms)
			if ((current.CurrentRoom != current.CameFromRoom) && (goBack.contains(current.DoorUsedToComeHere)))
			{
				ranges::copy(goBack.at(current.DoorUsedToComeHere), back_inserter(*puter.GetReadQueue()));
				puter.GetReadQueue()->push_back('\n');

				auto exec = puter.Execute();
				assert(exec != uIntputerExecutionResult::Exception);
				(void)exec;

				string output;
				ranges::copy(*puter.GetWriteQueue() | views::transform([](int64_t c) { return (char)c; }), back_inserter(output));
				puter.GetWriteQueue()->clear();
			}

			explorationQueue.pop_back();
			continue;
		}

		// Next room to explore
		ExploreState nextState;
		nextState.CameFromRoom = current.CurrentRoom;
		nextState.DoorUsedToComeHere = *current.DoorsToExplore.begin();
		current.DoorsToExplore.erase(current.DoorsToExplore.begin());

		explorationQueue.push_back(nextState);
	}
}

static vector<string> PlanRouteToRoom(const Ship& ship, const string &from, const string &to)
{
	vector<pair<string, int32_t>> searchQueue{ { from, -1 } };
	set<string> visited;
	for (int32_t searchNode = 0; searchNode < static_cast<int32_t>(searchQueue.size()); searchNode++)
	{
		auto [currentRoom, cameFrom] = searchQueue[searchNode];
		if (currentRoom == to)
		{
			vector<int32_t> route;
			route.push_back(searchNode);
			while (cameFrom != -1)
			{
				route.push_back(cameFrom);
				cameFrom = searchQueue[cameFrom].second;
			}
			ranges::reverse(route);

			vector<string> directions;
			for (size_t step = 0; (step + 1) < route.size(); step++)
			{
				directions.push_back(ship.DoorToUse.at({ searchQueue[route[step]].first, searchQueue[route[step + 1]].first }));
			}

			return directions;
		}

		if (visited.insert(currentRoom).second == false)
			continue;

		for (const auto& neighbour : ship.Neighbours.at(currentRoom))
		{
			searchQueue.push_back({ neighbour, searchNode });
		}
	}

	return {};
}

static pair<uIntputerExecutionResult, string> ExecuteStep(uIntputerWithIO<int64_t>* puter, const string& instruction)
{
	assert(puter->GetReadQueue()->empty());
	assert(puter->GetWriteQueue()->empty());

	ranges::copy(instruction, back_inserter(*puter->GetReadQueue()));
	puter->GetReadQueue()->push_back('\n');

	auto exec = puter->Execute(100000, 1 * 1024);

	string output;
	if (exec == uIntputerExecutionResult::Exception)
	{
		output = "*** EXCEPTION ***";
	}
	else
	{
		ranges::copy(*puter->GetWriteQueue() | views::transform([](int64_t c) { return (char)c; }), back_inserter(output));
	}

	puter->GetWriteQueue()->clear();

	return { exec, output };
}

static bool IsBoobyTrap(uIntputerWithIO<int64_t> puter, const Ship& ship, const string item)
{
	ranges::for_each(PlanRouteToRoom(ship, ship.StartingRoom, ship.ItemLocations.at(item)),
		[&](const string& inst)
		{
			ExecuteStep(&puter, inst);
		});
	auto [exec, output] = ExecuteStep(&puter, "take " + item);

	return (exec == uIntputerExecutionResult::Exception) || (output.find("Command?"sv) == string::npos);
}

static string TryGoal(uIntputerWithIO<int64_t>* puter, const string &goalDoor, const vector<string>& instructions)
{
	for (const string& inst : instructions)
	{
		auto [exec, output] = ExecuteStep(puter, inst);
		assert(puter->GetReadQueue()->empty());
		assert(exec == uIntputerExecutionResult::PendingIo);
		(void)exec;
		(void)output;
	}

	{
		auto [exec, output] = ExecuteStep(puter, goalDoor);
		assert(exec != uIntputerExecutionResult::Exception);
		(void)exec;

		if (output.find("Command?"sv) == string::npos)
		{
			smatch passcode;
			if (regex_search(output, passcode, regex{ R"((\d{4,}))" }))
			{
				return passcode.str();
			}
		}
	}

	return {};
}

string TryCombination(uIntputerWithIO<int64_t> puter, const Ship& ship, const set<string>& items)
{
	vector<string> instructions;

	string currentRoom = ship.StartingRoom;
	for (const string& item : items)
	{
		string itemLocation = ship.ItemLocations.at(item);
		ranges::copy(PlanRouteToRoom(ship, currentRoom, itemLocation), back_inserter(instructions));
		instructions.push_back("take " + item);
		currentRoom = itemLocation;
	}

	ranges::copy(PlanRouteToRoom(ship, currentRoom, ship.Goal.first), back_inserter(instructions));
	return TryGoal(&puter, ship.Goal.second, instructions);
}

void Puzzle25_A_2019()
{
	uIntputerWithIO<int64_t> puter(5 * 1024);

	//InteractiveMode(&puter);

	Ship ship;
	Explore(puter, &ship);

	vector<string> allItems;
	ranges::copy(views::join(ship.Rooms | views::values | views::transform([](const auto& r) { return r.Items; })), back_inserter(allItems));

	vector<string> boobyTraps;
	ranges::copy(allItems | views::filter([&](const string& item) { return IsBoobyTrap(puter, ship, item); }), back_inserter(boobyTraps));
	ranges::for_each(boobyTraps, [&](const string& item) { allItems.erase(ranges::find(allItems, item)); });

	string answer;

	int maxCombination = 1 << allItems.size();
	for (int combination = 1; combination < maxCombination; combination++)
	{
		set<string> testItems;
		for (int i = 0; i < allItems.size(); i++)
		{
			if (combination & (1 << i))
			{
				testItems.insert(allItems[i]);
			}
		}

		answer = TryCombination(puter, ship, testItems);
		if (answer.empty() == false)
			break;
	}

	PuzzleOutput::Submit(2019, 25, 1, answer.c_str());
}

void Puzzle25_B_2019()
{
	return PuzzleOutput::Submit(2019, 25, 2, int64_t(-1));
}
