#include "stdafx.h"

using namespace std;

namespace Puzzle24_2018_Types
{
	struct Group
	{
		int32_t System;
		int32_t Group;

		int32_t Units;
		int32_t HitPoints;
		uint32_t ImuneTo;
		uint32_t WeakAgainst;
		uint32_t DamageType;
		int32_t DamageAmount;
		int32_t Initiative;

		int32_t EffectivePower() const
		{
			return Units * DamageAmount;
		}
	};

	struct System
	{
		vector<shared_ptr<Group>> Groups;
	};

	struct Simulation
	{
		vector<shared_ptr<System>> Systems;
	};

	struct AttackChoice
	{
		int32_t DamageFactor;
		int32_t EffectivePowerFactor;
		int32_t InitiativeFactor;

		auto operator<=>(const AttackChoice&) const = default;
	};

	enum class SimulationResult
	{
		Unknown,
		Completed,
		Aborted,
	};
}

using namespace Puzzle24_2018_Types;

static uint32_t ParseMultipliersFor(const string& type, const string& description)
{
	if (description.empty())
	{
		return 0;
	}

	auto damageTypes = StringSplitTrimmed(description.substr(1, description.size() - 2), ';')
		| views::filter([&](const string& s) { return s.starts_with(type); })
		| views::transform([&](const string& s) { return s.substr(type.size()); })
		| views::transform([](const string& s) { return StringSplitTrimmed(s, ','); })
		| views::join
		| views::common;

	uint32_t bitset = 0;
	for (const string& s : damageTypes)
	{
		bitset |= 1 << (s[0] - 'a');
	}

	return bitset;
}

static Simulation ParsePuzzle()
{
	Simulation sim;

	regex unitPattern(R"((\d+) units each with (\d+) hit points (\([^\)]+\))? ?with an attack that does (\d+) (\w+) damage at initiative (\d+))");

	char line[256];
	while (PuzzleInput::NextLine())
	{
		if (PuzzleInput::PeekChar() == 'I')
		{
			sim.Systems.push_back(make_shared<System>());
			PuzzleInput::DropLine();
			continue;
		}

		Parse::ReadNonEmptyLine(line);

		cmatch m;
		if (regex_match(line, m, unitPattern) == false)
		{
			continue;
		}

		shared_ptr<Group> newGroup = make_shared<Group>();

		newGroup->System = (int32_t)sim.Systems.size();
		newGroup->Group = (int32_t)sim.Systems.back()->Groups.size() + 1;

		newGroup->Units = atoi(m[1].str().c_str());
		newGroup->HitPoints = atoi(m[2].str().c_str());
		newGroup->ImuneTo = ParseMultipliersFor("immune to", m[3].str());
		newGroup->WeakAgainst = ParseMultipliersFor("weak to", m[3].str());
		newGroup->DamageType = 1 << (m[5].str()[0] - 'a');
		newGroup->DamageAmount = atoi(m[4].str().c_str());
		newGroup->Initiative = atoi(m[6].str().c_str());

		sim.Systems.back()->Groups.push_back(newGroup);
	}

	return sim;
}

static Simulation CloneSimulation(const Simulation& sim)
{
	Simulation clone;

	for (const shared_ptr<System> system : sim.Systems)
	{
		shared_ptr<System> newSystem = make_shared<System>();
		for (const shared_ptr<Group>& group : system->Groups)
		{
			shared_ptr<Group> newGroup = make_shared<Group>(*group);
			newSystem->Groups.push_back(newGroup);
		}
		clone.Systems.push_back(newSystem);
	}

	return clone;
}

static bool GroupHasUnits(const shared_ptr<Group>& g)
{
	return g->Units > 0;
}

static bool UnitsLeftToFight(const Simulation& sim)
{
	return ranges::any_of(sim.Systems[0]->Groups, GroupHasUnits) && ranges::any_of(sim.Systems[1]->Groups, GroupHasUnits);
}

static int32_t CountUnitsInSystem(const Simulation& sim, size_t system)
{
	auto unitCount = sim.Systems[system]->Groups | views::transform([](const shared_ptr<Group>& g) { return g->Units; });
	return (int32_t)accumulate(unitCount.begin(), unitCount.end(), 0ll);
}

static vector<shared_ptr<Group>> GetAllAliveGroups(const Simulation& sim)
{
	vector<shared_ptr<Group>> aliveGroups;
	aliveGroups.reserve(32);

	for (const auto& system : sim.Systems)
	{
		for (const auto& group : system->Groups)
		{
			if (group->Units > 0)
			{
				aliveGroups.push_back(group);
			}
		}
	}

	return aliveGroups;
}

static int32_t MaximumDamageDealt(const shared_ptr<Group>& attacker, const shared_ptr<Group>& target)
{
	int32_t damageMultiplier = 1;
	if (target->ImuneTo & attacker->DamageType)
	{
		damageMultiplier = 0;
	}
	if (target->WeakAgainst & attacker->DamageType)
	{
		assert(damageMultiplier == 1);
		damageMultiplier = 2;
	}
	return attacker->EffectivePower() * damageMultiplier;
}

static SimulationResult SimulateFight(Simulation* sim)
{
	while (UnitsLeftToFight(*sim))
	{
		vector<shared_ptr<Group>> allAliveGroups = GetAllAliveGroups(*sim);

		// Target selection
		vector<pair<shared_ptr<Group>, bool>> unselectedGroups;
		unselectedGroups.reserve(allAliveGroups.size());

		ranges::copy(allAliveGroups
			| views::transform([](const shared_ptr<Group>& g)
				{
					return pair<shared_ptr<Group>, bool>{ g, false };
				}),
			inserter(unselectedGroups, unselectedGroups.end()));

		ranges::sort(allAliveGroups,
			[](const shared_ptr<Group>& a, const shared_ptr<Group>& b)
			{
				int32_t aEffectivePower = a->EffectivePower();
				int32_t bEffectivePower = b->EffectivePower();
				if (aEffectivePower != bEffectivePower)
				{
					return aEffectivePower > bEffectivePower;
				}

				return a->Initiative > b->Initiative;
			});

		map<shared_ptr<Group>, shared_ptr<Group>> attackTargets;
		for (const auto& attackingGroup : allAliveGroups)
		{
			tuple<AttackChoice, shared_ptr<Group>, size_t> bestTarget{};
			for (size_t i : IndicesOf(unselectedGroups))
			{
				const pair<shared_ptr<Group>, bool>& potentialTarget = unselectedGroups[i];
				if (potentialTarget.second)
					continue;
				if (potentialTarget.first->System == attackingGroup->System)
					continue;

				AttackChoice choice;
				choice.DamageFactor = MaximumDamageDealt(attackingGroup, potentialTarget.first);
				choice.EffectivePowerFactor = potentialTarget.first->EffectivePower();
				choice.InitiativeFactor = potentialTarget.first->Initiative;
				if ((!get<1>(bestTarget)) || (choice > get<0>(bestTarget)))
				{
					bestTarget = { choice, potentialTarget.first, i };
				}
			}

			if (get<1>(bestTarget))
			{
				shared_ptr<Group> target = get<1>(bestTarget);

				assert(attackTargets.contains(attackingGroup) == false);
				attackTargets[attackingGroup] = target;
				unselectedGroups[get<2>(bestTarget)].second = true;
			}
		}

		// Attack
		ranges::sort(allAliveGroups,
			[](const shared_ptr<Group>& a, const shared_ptr<Group>& b)
			{
				return a->Initiative > b->Initiative;
			});

		int32_t totalUnitsKilled = 0;
		for (const shared_ptr<Group>& attackingGroup : allAliveGroups)
		{
			if (attackingGroup->Units == 0)
				continue;

			if (attackTargets.contains(attackingGroup) == false)
				continue;

			shared_ptr<Group> targetGroup = attackTargets.at(attackingGroup);
			int32_t damageDealt = MaximumDamageDealt(attackingGroup, targetGroup);
			int32_t unitsKilled = min(damageDealt / targetGroup->HitPoints, targetGroup->Units);
			targetGroup->Units = targetGroup->Units - unitsKilled;

			totalUnitsKilled += unitsKilled;
		}
		if (totalUnitsKilled == 0)
		{
			return SimulationResult::Aborted;
		}
	}

	return SimulationResult::Completed;
}

static SimulationResult GetOutcomeOfFight(const Simulation& sim, int32_t boost, pair<int32_t, int32_t> *unitsLeft)
{
	Simulation boostedSim = CloneSimulation(sim);
	ranges::for_each(boostedSim.Systems[0]->Groups, [&](const shared_ptr<Group>& g) { g->DamageAmount += boost; });
	SimulationResult simResult = SimulateFight(&boostedSim);
	if (simResult == SimulationResult::Completed)
	{
		*unitsLeft = { CountUnitsInSystem(boostedSim, 0), CountUnitsInSystem(boostedSim, 1) };
	}
	return simResult;
}

void Puzzle24_A_2018()
{
	Simulation sim = ParsePuzzle();

	pair<int32_t, int32_t> unitsLeft = { 0, 0 };
	SimulationResult outcome = GetOutcomeOfFight(sim, 0, &unitsLeft);
	assert(outcome == SimulationResult::Completed);
	(void)outcome;
	int32_t answer = max(unitsLeft.first, unitsLeft.second);

	PuzzleOutput::Submit(2018, 24, 1, answer);
}

void Puzzle24_B_2018()
{
	const Simulation startingSimulation = ParsePuzzle();

	const int32_t maxBoost = 100;

	vector<tuple<int32_t, SimulationResult, int32_t>> outcomeCache;
	for (int32_t i = 0; i < maxBoost; i++)
	{
		outcomeCache.push_back({ i, SimulationResult::Unknown, 0 });
	}
	outcomeCache.push_back({ maxBoost, SimulationResult::Completed, numeric_limits<int32_t>::max() }); // Sentinel

	int32_t answer = 0;

	size_t rangeBegin = 0;
	size_t rangeEnd = outcomeCache.size() - 1;

	auto isCacheValidFor = [&](size_t rangeIndex) -> bool
		{
			if (get<1>(outcomeCache[rangeIndex]) == SimulationResult::Unknown)
			{
				pair<int32_t, int32_t> unitsLeft = { -1, -1 };
				SimulationResult outcome = GetOutcomeOfFight(startingSimulation, get<0>(outcomeCache[rangeIndex]), &unitsLeft);
				if (outcome == SimulationResult::Aborted)
				{
					// If this isn't a valid battle, drop it from the range we're binary searching and restart
					outcomeCache.erase(outcomeCache.begin() + rangeIndex);
					rangeEnd--;
					return false;
				}

				get<1>(outcomeCache[rangeIndex]) = outcome;
				get<2>(outcomeCache[rangeIndex]) = unitsLeft.first;
			}

			return true;
		};

	while (true)
	{
		assert(rangeEnd > rangeBegin);

		size_t rangeMiddle = (rangeBegin + rangeEnd) / 2;
		if (isCacheValidFor(rangeMiddle) == false)
			continue;

		size_t rangeMiddlePlusOne = rangeMiddle + 1;
		if (isCacheValidFor(rangeMiddlePlusOne) == false)
			continue;

		assert(get<1>(outcomeCache[rangeMiddle]) == SimulationResult::Completed);
		assert(get<1>(outcomeCache[rangeMiddlePlusOne]) == SimulationResult::Completed);

		int32_t unitsLeftMiddle = get<2>(outcomeCache[rangeMiddle]);
		int32_t unitsLeftMiddlePlusOne = get<2>(outcomeCache[rangeMiddlePlusOne]);
		if ((unitsLeftMiddle == 0) && (unitsLeftMiddlePlusOne > 0))
		{
			answer = unitsLeftMiddlePlusOne;
			break;
		}
		else if (unitsLeftMiddle == 0)
		{
			rangeBegin = rangeMiddle;
		}
		else if (unitsLeftMiddle > 0)
		{
			rangeEnd = rangeMiddlePlusOne;
		}
	}

	PuzzleOutput::Submit(2018, 24, 2, answer);
}
