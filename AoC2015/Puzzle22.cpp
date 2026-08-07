#include "stdafx.h"

using namespace std;

namespace Puzzle22_2015_Types
{
	enum EffectType : size_t
	{
		Shield,
		Poison,
		Recharge,

		Count
	};

	struct Fighter
	{
		int16_t HitPoints = 0;
		int16_t Mana = 0;
		int16_t Damage = 0;
		int16_t Armour = 0;
		int8_t EffectTimers[EffectType::Count] = { 0 };
	};

	enum class Difficulty : uint8_t
	{
		Easy,
		Hard
	};

	enum class RoundStage : uint8_t
	{
		Begin,
		CheckForWin,
		CheckForLoss,
		ApplyEffects,
		CheckForWinAfterPoison,
		CastMissle,
		CastDrain,
		CastShield,
		CastPoison,
		CastRecharge,
		BossAttacks,
		End,
	};

	struct FightState
	{
		array<Fighter, 2> Fighters;
		int32_t ManaSpend;
		int8_t ActivePlayer;
		RoundStage Stage;
	};
}

using namespace Puzzle22_2015_Types;

static Fighter ParseBoss()
{
	Fighter boss;

	char buffer[32];

	int scanned = 0;

	Parse::ReadNonEmptyLine(buffer, sizeof(buffer));
	scanned += sscanf(buffer, "Hit Points: %hd", &boss.HitPoints);

	Parse::ReadNonEmptyLine(buffer, sizeof(buffer));
	scanned += sscanf(buffer, "Damage: %hd", &boss.Damage);

	assert(scanned == 2);
	(void)scanned;

	return boss;
}

static Fighter CreatePlayer()
{
	Fighter player;
	player.HitPoints = 50;
	player.Mana = 500;
	return player;
}

static int32_t MinimumManaForWin(const array<Fighter, 2>& startingFighters, Difficulty difficulty)
{
	int32_t globalMinimumMana = numeric_limits<int32_t>::max();

	const size_t maxExpectedStack = 64;
	vector<FightState> evalStack;
	evalStack.resize(maxExpectedStack);
	int32_t topOfStack = -1;

	evalStack[++topOfStack] = { startingFighters, 0, 0, RoundStage::Begin };
	while (topOfStack >= 0)
	{
		assert(evalStack.size() <= maxExpectedStack);

		FightState& currentState = evalStack[topOfStack];
		switch (currentState.Stage)
		{
		case RoundStage::Begin:
			{
				// The cheapest damage we can buy is poison at 173 mana for 18HP
				const int32_t minimumManaPerHP = 9;

				// Early out if we can't improve on the global minimum
				int32_t minimumManaForRemainingHP = max<int32_t>(currentState.Fighters[1].HitPoints * minimumManaPerHP, 0);
				currentState.Stage = ((currentState.ManaSpend + minimumManaForRemainingHP) > globalMinimumMana ? RoundStage::End : RoundStage::CheckForWin);
			}
			break;

		case RoundStage::CheckForWin:
			{
				if (currentState.Fighters[1].HitPoints <= 0)
				{
					if (currentState.ManaSpend < globalMinimumMana)
					{
						globalMinimumMana = currentState.ManaSpend;
					}
					currentState.Stage = RoundStage::End;
				}
				else
				{
					currentState.Stage = RoundStage::CheckForLoss;
				}
			}
			break;

		case RoundStage::CheckForLoss:
			{
				// At the start of each player turn (before any other effects apply), you lose 1 hit point.
				if ((currentState.ActivePlayer == 0) && (difficulty == Difficulty::Hard))
				{
					currentState.Fighters[0].HitPoints -= 1;
				}

				// Have we died?
				if (currentState.Fighters[0].HitPoints <= 0)
				{
					currentState.Stage = RoundStage::End;
				}
				else
				{
					currentState.Stage = RoundStage::ApplyEffects;
				}
			}
			break;

		case RoundStage::ApplyEffects:
			{
				// Apply effects
				if (currentState.Fighters[0].EffectTimers[EffectType::Shield] > 0)
				{
					currentState.Fighters[0].Armour = 7;
					currentState.Fighters[0].EffectTimers[EffectType::Shield]--;
				}
				else
				{
					currentState.Fighters[0].Armour = 0;
				}

				if (currentState.Fighters[0].EffectTimers[EffectType::Poison] > 0)
				{
					currentState.Fighters[1].HitPoints -= 3;
					currentState.Fighters[0].EffectTimers[EffectType::Poison]--;
				}

				if (currentState.Fighters[0].EffectTimers[EffectType::Recharge] > 0)
				{
					currentState.Fighters[0].Mana += 101;
					currentState.Fighters[0].EffectTimers[EffectType::Recharge]--;
				}

				currentState.Stage = RoundStage::CheckForWinAfterPoison;
			}
			break;

		case RoundStage::CheckForWinAfterPoison:
			{
				// Have we killed the boss (second check because poison might have killed him)?
				if (currentState.Fighters[1].HitPoints <= 0)
				{
					if (currentState.ManaSpend < globalMinimumMana)
					{
						globalMinimumMana = currentState.ManaSpend;
					}
					currentState.Stage = RoundStage::End;
				}
				else if (currentState.ActivePlayer == 0)
				{
					currentState.Stage = RoundStage::CastMissle;
				}
				else
				{
					currentState.Stage = RoundStage::BossAttacks;
				}
			}
			break;

		case RoundStage::CastMissle:
			{
				currentState.Stage = RoundStage::CastDrain;

				// Magic Missile costs 53 mana. It instantly does 4 damage.
				if (currentState.Fighters[0].Mana >= 53)
				{
					evalStack[++topOfStack] = currentState;
					FightState& newState = evalStack[topOfStack];
					newState.Stage = RoundStage::Begin;
					newState.Fighters[0].Mana -= 53;
					newState.Fighters[1].HitPoints -= 4;
					newState.ManaSpend += 53;
					newState.ActivePlayer = 1;
				}
			}
			break;

		case RoundStage::CastDrain:
			{
				currentState.Stage = RoundStage::CastShield;

				// Drain costs 73 mana. It instantly does 2 damage and heals you for 2 hit points.
				if (currentState.Fighters[0].Mana >= 73)
				{
					evalStack[++topOfStack] = currentState;
					FightState& newState = evalStack[topOfStack];
					newState.Stage = RoundStage::Begin;
					newState.Fighters[0].Mana -= 73;
					newState.Fighters[0].HitPoints += 2;
					newState.Fighters[1].HitPoints -= 2;
					newState.ManaSpend += 73;
					newState.ActivePlayer = 1;
				}
			}
			break;

		case RoundStage::CastShield:
			{
				currentState.Stage = RoundStage::CastPoison;

				// Shield costs 113 mana. It starts an effect that lasts for 6 turns. While it is active, your armor is increased by 7.
				if ((currentState.Fighters[0].Mana >= 113) && (currentState.Fighters[0].EffectTimers[EffectType::Shield] == 0))
				{
					evalStack[++topOfStack] = currentState;
					FightState& newState = evalStack[topOfStack];
					newState.Stage = RoundStage::Begin;
					newState.Fighters[0].Mana -= 113;
					newState.Fighters[0].EffectTimers[EffectType::Shield] = 6;
					newState.ManaSpend += 113;
					newState.ActivePlayer = 1;
				}
			}
			break;

		case RoundStage::CastPoison:
			{
				currentState.Stage = RoundStage::CastRecharge;

				// Poison costs 173 mana. It starts an effect that lasts for 6 turns. At the start of each turn while it is active, it deals the boss 3 damage.
				if ((currentState.Fighters[0].Mana >= 173) && (currentState.Fighters[0].EffectTimers[EffectType::Poison] == 0))
				{
					evalStack[++topOfStack] = currentState;
					FightState& newState = evalStack[topOfStack];
					newState.Stage = RoundStage::Begin;
					newState.Fighters[0].Mana -= 173;
					newState.Fighters[0].EffectTimers[EffectType::Poison] = 6;
					newState.ManaSpend += 173;
					newState.ActivePlayer = 1;
				}
			}
			break;

		case RoundStage::CastRecharge:
			{
				currentState.Stage = RoundStage::End;

				// Recharge costs 229 mana. It starts an effect that lasts for 5 turns. At the start of each turn while it is active, it gives you 101 new mana.
				if ((currentState.Fighters[0].Mana >= 229) && (currentState.Fighters[0].EffectTimers[EffectType::Recharge] == 0))
				{
					evalStack[++topOfStack] = currentState;
					FightState& newState = evalStack[topOfStack];
					newState.Stage = RoundStage::Begin;
					newState.Fighters[0].Mana -= 229;
					newState.Fighters[0].EffectTimers[EffectType::Recharge] = 5;
					newState.ManaSpend += 229;
					newState.ActivePlayer = 1;
				}
			}
			break;

		case RoundStage::BossAttacks:
			{
				currentState.Stage = RoundStage::End;

				int16_t damageDealt = max<int16_t>(currentState.Fighters[1].Damage - currentState.Fighters[0].Armour, 1);

				evalStack[++topOfStack] = currentState;
				FightState& newState = evalStack[topOfStack];
				newState.Stage = RoundStage::Begin;
				newState.Fighters[0].HitPoints -= damageDealt;
				newState.ActivePlayer = 0;
			}
			break;

		case RoundStage::End:
			{
				topOfStack--;
			}
			break;
		}
	}

	return globalMinimumMana;
}

void Puzzle22_A_2015()
{
	array<Fighter, 2> fighters = { CreatePlayer(), ParseBoss() };
	int32_t answer = MinimumManaForWin(fighters, Difficulty::Easy);

	return PuzzleOutput::Submit(2015, 22, 1, answer);
}

void Puzzle22_B_2015()
{
	array<Fighter, 2> fighters = { CreatePlayer(), ParseBoss() };
	int32_t answer = MinimumManaForWin(fighters, Difficulty::Hard);

	return PuzzleOutput::Submit(2015, 22, 2, answer);
}
