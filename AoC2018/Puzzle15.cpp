#include "stdafx.h"

using namespace std;

namespace Puzzle15_2018_Types
{
	enum class UnitType
	{
		Dead,
		Elf,
		Goblin,
	};

	struct Unit
	{
		UnitType Type = UnitType::Dead;
		int32_t AttackPower = -1;
		int32_t HP = -1;
	};

	struct ReadingOrder
	{
		bool operator()(const Vec2Int& a, const Vec2Int& b) const
		{
			return (a.Y == b.Y) ? a.X < b.X : a.Y < b.Y;
		}
	};

	using UnitMap = map<Vec2Int, shared_ptr<Unit>, ReadingOrder>;

	class QueuedBuffer;

	struct CombatArena
	{
		uArrayMap2D Terrain;
		UnitMap Units;
		shared_ptr<QueuedBuffer> Queued;
	};

	struct Turn
	{
		pair<Vec2Int, shared_ptr<Unit>> ActiveUnit;
		UnitType ActiveEnemy;
		char EnemySymbol;
		set<Vec2Int, ReadingOrder> SquaresInRange;
		Vec2Int ChosenSquare;
		set<Vec2Int, ReadingOrder> PossibleSteps;
		Vec2Int ChosenStep;
	};

	class QueuedBuffer
	{
	public:
		QueuedBuffer(const uArrayMap2D& area)
			: Stride(area.GetWidth())
			, Queued(area.GetWidth() * area.GetHeight(), 0)
		{
		}

		bool HasQueued(const Vec2Int& p)
		{
			size_t index = p.Y * Stride + p.X;
			bool hasQueued = Queued[index];
			Queued[index] = 1;
			return hasQueued;
		}

		void Clear()
		{
			memset(Queued.data(), 0, Queued.size());
		}

	private:
		size_t Stride;
		vector<uint8_t> Queued;
	};
}

using namespace Puzzle15_2018_Types;

static void PopulateUnits(CombatArena* arena)
{
	const Unit elf{ UnitType::Elf, 3, 200 };
	const Unit goblin{ UnitType::Goblin, 3, 200 };

	for (const auto& p : arena->Terrain.Grid())
	{
		if (p.second == 'E')
		{
			arena->Units.insert({ p.first, make_shared<Unit>(elf) });
		}
		else if (p.second == 'G')
		{
			arena->Units.insert({ p.first, make_shared<Unit>(goblin) });
		}
	}
}

static CombatArena ParseArena()
{
	CombatArena arena{ ReaduArrayMap() };
	PopulateUnits(&arena);
	arena.Queued = make_shared<QueuedBuffer>(arena.Terrain);
	return arena;
}

static CombatArena CloneArena(const CombatArena& original)
{
	CombatArena cloned{ original.Terrain };
	PopulateUnits(&cloned);
	cloned.Queued = original.Queued;
	return cloned;
}

static bool Pathfind(const CombatArena& arena, const Vec2Int& startingPoint, const set<Vec2Int, ReadingOrder>& endpoints, Vec2Int *endpointChosen)
{
	vector<Vec2Int> searchQueues[2];
	searchQueues[0].reserve(128);
	searchQueues[1].reserve(128);

	searchQueues[0].push_back(startingPoint);

	set<Vec2Int, ReadingOrder> nearestEndpoints;
	arena.Queued->Clear();
	arena.Queued->HasQueued(startingPoint);

	QueuedBuffer* queued = arena.Queued.get(); // Don't continually re-resolve the shared_ptr
	for (int32_t currentSteps = 0; nearestEndpoints.empty(); currentSteps++)
	{
		vector<Vec2Int>& currentQueue = searchQueues[currentSteps & 0x1];
		if (currentQueue.empty())
			break;

		vector<Vec2Int>& nextQueue = searchQueues[(currentSteps + 1) & 0x1];
		nextQueue.clear();

		for (const Vec2Int& currentPos : currentQueue)
		{
			if (endpoints.contains(currentPos))
			{
				nearestEndpoints.insert(currentPos);
				continue;
			}

			for (const Vec2Int& dir : Vec2Int::CardinalDirections())
			{
				Vec2Int nextPos = currentPos + dir;
				if ((queued->HasQueued(nextPos) == false) && (arena.Terrain(nextPos) == '.'))
				{
					nextQueue.push_back(nextPos);
				}
			}
		}
	}

	if (!nearestEndpoints.empty())
	{
		*endpointChosen = *nearestEndpoints.begin();
		return true;
	}

	return false;
}

static void FindSurroundingSquares(const CombatArena& arena, const Vec2Int& origin, char lookingFor, set<Vec2Int, ReadingOrder> *squares)
{
	for (const Vec2Int& dir : Vec2Int::CardinalDirections())
	{
		Vec2Int adjacent = origin + dir;
		if (arena.Terrain(adjacent) == lookingFor)
		{
			squares->insert(adjacent);
		}
	}
}

static bool FindSquaresInRange(const CombatArena& arena, Turn *turn)
{
	ranges::for_each(arena.Units | views::filter([&](const auto& unit) { return unit.second->Type == turn->ActiveEnemy; }),
		[&](const auto& unit)
		{
			FindSurroundingSquares(arena, unit.first, '.', &turn->SquaresInRange);
		});
	return turn->SquaresInRange.empty() == false;
}

static bool NeedToMove(const CombatArena& arena, Turn* turn)
{
	return ranges::none_of(Vec2Int::CardinalDirections() | views::transform([&](const Vec2Int& p) { return turn->ActiveUnit.first + p; }),
		[&](const Vec2Int& p)
		{
			return arena.Terrain(p) == turn->EnemySymbol;
		});
}

static bool FindMove(const CombatArena& arena, Turn* turn)
{
	return FindSquaresInRange(arena, turn) && Pathfind(arena, turn->ActiveUnit.first, turn->SquaresInRange, &turn->ChosenSquare);
}

static void Move(CombatArena *arena, Turn* turn)
{
	FindSurroundingSquares(*arena, turn->ActiveUnit.first, '.', &turn->PossibleSteps);
	assert(turn->PossibleSteps.empty() == false);

	bool foundPath = Pathfind(*arena, turn->ChosenSquare, turn->PossibleSteps, &turn->ChosenStep);
	assert(foundPath);
	(void)foundPath;

	arena->Terrain(turn->ActiveUnit.first) = '.';
	arena->Terrain(turn->ChosenStep) = turn->ActiveUnit.second->Type == UnitType::Elf ? 'E' : 'G';

	arena->Units.erase(turn->ActiveUnit.first);
	arena->Units[turn->ChosenStep] = turn->ActiveUnit.second; // Force an overwrite, potentially clearing out dead bodies

	turn->ActiveUnit.first = turn->ChosenStep;
}

static bool CanAttack(const CombatArena& arena, Turn* turn)
{
	return ranges::any_of(Vec2Int::CardinalDirections() | views::transform([&](const Vec2Int& p) { return turn->ActiveUnit.first + p; }),
		[&](const Vec2Int& p)
		{
			return arena.Terrain(p) == turn->EnemySymbol;
		});
}

static void Attack(CombatArena *arena, Turn* turn)
{
	set<Vec2Int, ReadingOrder> surroundingEnemySquares;
	FindSurroundingSquares(*arena, turn->ActiveUnit.first, turn->EnemySymbol, &surroundingEnemySquares);
	assert(surroundingEnemySquares.empty() == false);

	pair<Vec2Int, shared_ptr<Unit>> enemyToAttack;
	int64_t lowestHp = numeric_limits<int64_t>::max();
	for (const auto& enemySquare : surroundingEnemySquares)
	{
		pair<Vec2Int, shared_ptr<Unit>> candidateEnemy = *arena->Units.find(enemySquare);
		if (candidateEnemy.second->HP < lowestHp)
		{
			enemyToAttack = candidateEnemy;
			lowestHp = enemyToAttack.second->HP;
		}
	}
	assert((bool)enemyToAttack.second);

	enemyToAttack.second->HP -= turn->ActiveUnit.second->AttackPower;
	if (enemyToAttack.second->HP <= 0)
	{
		enemyToAttack.second->Type = UnitType::Dead;
		arena->Terrain(enemyToAttack.first) = '.';
	}
}

static bool NoMoreTargets(const CombatArena& arena, Turn* turn)
{
	return ranges::count_if(arena.Units, [&](const auto& u) { return u.second->Type == turn->ActiveEnemy; }) == 0;
}

static void SetAttackPowerForElves(CombatArena* arena, int32_t attackPower)
{
	for (auto& unit : arena->Units)
	{
		if (unit.second->Type == UnitType::Elf)
		{
			unit.second->AttackPower = attackPower;
		}
	}
}

static int32_t RunSimulation(CombatArena* arena)
{
	int32_t round = 0;
	while (true)
	{
		vector<pair<Vec2Int, shared_ptr<Unit>>> roundOrder;
		ranges::copy(arena->Units, back_inserter(roundOrder));

		for (const auto& unit : roundOrder)
		{
			if (unit.second->Type == UnitType::Dead)
				continue;

			Turn turn;
			turn.ActiveUnit = unit;
			turn.ActiveEnemy = turn.ActiveUnit.second->Type == UnitType::Elf ? UnitType::Goblin : UnitType::Elf;
			turn.EnemySymbol = turn.ActiveEnemy == UnitType::Elf ? 'E' : 'G';

			if (NoMoreTargets(*arena, &turn))
			{
				return round;
			}

			if (NeedToMove(*arena, &turn) && FindMove(*arena, &turn))
			{
				Move(arena, &turn);
			}
			if (CanAttack(*arena, &turn))
			{
				Attack(arena, &turn);
			}
		}

		round++;
	}

	return -1;
}

static pair<int32_t, int32_t> GetCombatOutcome(const CombatArena startingArena, int32_t attackPower)
{
	CombatArena arena = CloneArena(startingArena);
	SetAttackPowerForElves(&arena, attackPower);

	int32_t fullRounds = RunSimulation(&arena);

	auto aliveUnitHp = arena.Units
		| views::filter([](const auto& unit) { return unit.second->Type != UnitType::Dead; })
		| views::transform([](const auto& unit) { return unit.second->HP; });
	int32_t hitpointsRemaining = accumulate(aliveUnitHp.begin(), aliveUnitHp.end(), 0);

	return { fullRounds * hitpointsRemaining, static_cast<int32_t>(ranges::count_if(arena.Units, [](const auto& unit) { return unit.second->Type == UnitType::Elf; })) };
}

void Puzzle15_A_2018()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 4 * 1024;

	MemArena_Configure(cfg);
	{
		CombatArena world = ParseArena();
		auto [outcome, elvesAlive] = GetCombatOutcome(world, 3);

		int32_t answer = outcome;

		PuzzleOutput::Submit(2018, 15, 1, answer);
	}
	MemArena_Reset();
}

void Puzzle15_B_2018()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 4 * 1024;

	MemArena_Configure(cfg);
	{
		const CombatArena startingWorld = ParseArena();
		const int32_t startingElves = static_cast<int32_t>(ranges::count_if(startingWorld.Units, [](const auto& unit) { return unit.second->Type == UnitType::Elf; }));

		int32_t answer = 0;

		int32_t lowerAttackPower = 4;
		int32_t upperAttackPower = 100 + 1; // No point going beyond insta-kill
		while (true)
		{
			int32_t midAttackPower = (lowerAttackPower + upperAttackPower) / 2;
			auto outcomeAtMid = GetCombatOutcome(startingWorld, midAttackPower);
			auto outcomeAfterMid = GetCombatOutcome(startingWorld, midAttackPower + 1);

			if ((outcomeAtMid.second < startingElves) && (outcomeAfterMid.second == startingElves))
			{
				answer = outcomeAfterMid.first;
				break;
			}

			if (outcomeAtMid.second < startingElves)
			{
				lowerAttackPower = midAttackPower;
			}
			else
			{
				upperAttackPower = midAttackPower;
			}
		}

		PuzzleOutput::Submit(2018, 15, 2, answer);
	}
	MemArena_Reset();
}
