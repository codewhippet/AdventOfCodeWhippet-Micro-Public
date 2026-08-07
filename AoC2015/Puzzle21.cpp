#include "stdafx.h"

using namespace std;

namespace Puzzle21_2015_Types
{
	struct Fighter
	{
		int32_t HitPoints = 0;
		int32_t Damage = 0;
		int32_t Armour = 0;
	};

	struct Item
	{
		string_view Name;
		int32_t Cost = 0;
		int32_t Damage = 0;
		int32_t Armour = 0;
	};
}

using namespace Puzzle21_2015_Types;

static Fighter ParseBoss()
{
	Fighter boss;

	char buffer[32];

	int scanned = 0;

	Parse::ReadNonEmptyLine(buffer, sizeof(buffer));
	scanned += sscanf(buffer, "Hit Points: %d", &boss.HitPoints);

	Parse::ReadNonEmptyLine(buffer, sizeof(buffer));
	scanned += sscanf(buffer, "Damage: %d", &boss.Damage);

	Parse::ReadNonEmptyLine(buffer, sizeof(buffer));
	scanned += sscanf(buffer, "Armor: %d", &boss.Armour);

	assert(scanned == 3);
	(void)scanned;

	return boss;
}

static Fighter CreatePlayer(int32_t hitPoints, const vector<Item>& items)
{
	Fighter player;
	player.HitPoints = hitPoints;

	for (const Item& item : items)
	{
		player.Damage += item.Damage;
		player.Armour += item.Armour;
	}

	return player;
}

static size_t WhoWins(array<Fighter, 2> fighters)
{
	size_t currentAttacker = 0;
	while (true)
	{
		size_t opponent = 1 - currentAttacker;
		int32_t damageDealt = max<int32_t>(fighters[currentAttacker].Damage - fighters[opponent].Armour, 1);
		fighters[opponent].HitPoints -= damageDealt;
		if (fighters[opponent].HitPoints <= 0)
		{
			return currentAttacker;
		}
		currentAttacker = 1 - currentAttacker;
	}
}

static vector<Item> WeaponInventory()
{
	return
	{
		{ "Dagger", 8, 4, 0 },
		{ "Shortsword", 10, 5, 0 },
		{ "Warhammer", 25, 6, 0 },
		{ "Longsword", 40, 7, 0 },
		{ "Greataxe", 74, 8, 0 }
	};
}

static vector<Item> ArmourInventory()
{
	return
	{
		{ "None", 0, 0, 0 },
		{ "Leather", 13, 0, 1 },
		{ "Chainmail", 31, 0, 2 },
		{ "Splintmail", 53, 0, 3 },
		{ "Bandedmail", 75, 0, 4 },
		{ "Platemail", 102, 0, 5 }
	};
}

static vector<Item> RingInventory()
{
	return
	{
		{ "None #1", 0, 0, 0 },
		{ "None #2", 0, 0, 0 },
		{ "Damage +1", 25, 1, 0 },
		{ "Damage +2", 50, 2, 0 },
		{ "Damage +3", 100, 3, 0 },
		{ "Defense +1", 20, 0, 1 },
		{ "Defense +2", 40, 0, 2 },
		{ "Defense +3", 80, 0, 3 }
	};
}

void Puzzle21_A_2015()
{
	const int32_t playerHitpoints = 100;

	vector<Item> weaponInventory = WeaponInventory();
	vector<Item> armourInventory = ArmourInventory();
	vector<Item> ringInventory = RingInventory();

	array<Fighter, 2> fighters;
	fighters[0] = { 0, 0, 0 };
	fighters[1] = ParseBoss();

	int32_t answer = numeric_limits<int32_t>::max();
	for (size_t weapon = 0; weapon < weaponInventory.size(); weapon++)
	{
		for (size_t armour = 0; armour < armourInventory.size(); armour++)
		{
			for (size_t leftRing = 0; leftRing < ringInventory.size(); leftRing++)
			{
				for (size_t rightRing = leftRing + 1; rightRing < ringInventory.size(); rightRing++)
				{
					int32_t equipmentCost =
						weaponInventory[weapon].Cost +
						armourInventory[armour].Cost +
						ringInventory[leftRing].Cost +
						ringInventory[rightRing].Cost;
					if (equipmentCost < answer)
					{
						fighters[0] = CreatePlayer(playerHitpoints,
							{
								weaponInventory[weapon],
								armourInventory[armour],
								ringInventory[leftRing],
								ringInventory[rightRing]
							});

						size_t winner = WhoWins(fighters);
						if (winner == 0)
						{
							answer = equipmentCost;
						}
					}
				}
			}
		}
	}

	return PuzzleOutput::Submit(2015, 21, 1, answer);
}

void Puzzle21_B_2015()
{
	const int32_t playerHitpoints = 100;

	vector<Item> weaponInventory = WeaponInventory();
	vector<Item> armourInventory = ArmourInventory();
	vector<Item> ringInventory = RingInventory();

	array<Fighter, 2> fighters;
	fighters[0] = { 0, 0, 0 };
	fighters[1] = ParseBoss();

	int32_t answer = 0;
	for (size_t weapon = 0; weapon < weaponInventory.size(); weapon++)
	{
		for (size_t armour = 0; armour < armourInventory.size(); armour++)
		{
			for (size_t leftRing = 0; leftRing < ringInventory.size(); leftRing++)
			{
				for (size_t rightRing = leftRing + 1; rightRing < ringInventory.size(); rightRing++)
				{
					int32_t equipmentCost =
						weaponInventory[weapon].Cost +
						armourInventory[armour].Cost +
						ringInventory[leftRing].Cost +
						ringInventory[rightRing].Cost;
					if (equipmentCost > answer)
					{
						fighters[0] = CreatePlayer(playerHitpoints,
							{
								weaponInventory[weapon],
								armourInventory[armour],
								ringInventory[leftRing],
								ringInventory[rightRing]
							});

						size_t winner = WhoWins(fighters);
						if (winner == 1)
						{
							answer = equipmentCost;
						}
					}
				}
			}
		}
	}

	return PuzzleOutput::Submit(2015, 21, 2, answer);
}
