#include "stdafx.h"

using namespace std;

namespace Puzzle15_2015_Types
{
	enum : size_t
	{
		NUM_PROPERTIES = 5,

		CAP = 0,
		DUR = 1,
		FLA = 2,
		TEX = 3,

		CAL = 4,
	};

	using Properties = array<int32_t, NUM_PROPERTIES>;
}

using namespace Puzzle15_2015_Types;

static vector<Properties> ParseRecipe()
{
	vector<Properties> r;
	while (PuzzleInput::NextLine())
	{
		r.push_back({});
		for (size_t i = 0; i < NUM_PROPERTIES; i++)
		{
			r.back()[i] = Parse::GetInt32();
		}
	}
	return r;
}

static void FindHighestScoreWithoutCalories(const vector<Properties>& recipe,
	size_t ingredientIndex,
	int32_t teaspoonsLeft,
	vector<Properties>* partialScores,
	int32_t* highestScore)
{
	Properties& oldScore = (*partialScores)[ingredientIndex];
	if (ingredientIndex == recipe.size())
	{
		int32_t score = max<int32_t>(oldScore[CAP], 0) * max<int32_t>(oldScore[DUR], 0) * max<int32_t>(oldScore[FLA], 0) * max<int32_t>(oldScore[TEX], 0);
		if (score > *highestScore)
		{
			*highestScore = score;
		}
		return;
	}

	int32_t minSpoons = (ingredientIndex == (recipe.size() - 1)) ? teaspoonsLeft : 0;

	const Properties& nextIngredient = recipe[ingredientIndex];
	for (int32_t spoons = minSpoons; spoons <= teaspoonsLeft; spoons++)
	{
		Properties& newScore = (*partialScores)[ingredientIndex + 1];

		newScore[CAP] = oldScore[CAP] + (spoons * nextIngredient[CAP]);
		newScore[DUR] = oldScore[DUR] + (spoons * nextIngredient[DUR]);
		newScore[FLA] = oldScore[FLA] + (spoons * nextIngredient[FLA]);
		newScore[TEX] = oldScore[TEX] + (spoons * nextIngredient[TEX]);

		FindHighestScoreWithoutCalories(recipe, ingredientIndex + 1, teaspoonsLeft - spoons, partialScores, highestScore);
	}
}

static void FindHighestScoreWithSpecifiedCalories(const vector<Properties>& recipe,
	int32_t calories,
	size_t ingredientIndex,
	int32_t teaspoonsLeft,
	vector<Properties>* partialScores,
	int32_t* highestScore)
{
	Properties& oldScore = (*partialScores)[ingredientIndex];
	if (ingredientIndex == recipe.size())
	{
		if (oldScore[CAL] == calories)
		{
			int32_t score = max<int32_t>(oldScore[CAP], 0) * max<int32_t>(oldScore[DUR], 0) * max<int32_t>(oldScore[FLA], 0) * max<int32_t>(oldScore[TEX], 0);
			if (score > *highestScore)
			{
				*highestScore = score;
			}
		}
		return;
	}

	int32_t minSpoons = (ingredientIndex == (recipe.size() - 1)) ? teaspoonsLeft : 0;

	const Properties& nextIngredient = recipe[ingredientIndex];
	for (int32_t spoons = minSpoons; spoons <= teaspoonsLeft; spoons++)
	{
		Properties& newScore = (*partialScores)[ingredientIndex + 1];

		newScore[CAP] = oldScore[CAP] + (spoons * nextIngredient[CAP]);
		newScore[DUR] = oldScore[DUR] + (spoons * nextIngredient[DUR]);
		newScore[FLA] = oldScore[FLA] + (spoons * nextIngredient[FLA]);
		newScore[TEX] = oldScore[TEX] + (spoons * nextIngredient[TEX]);
		newScore[CAL] = oldScore[CAL] + (spoons * nextIngredient[CAL]);

		FindHighestScoreWithSpecifiedCalories(recipe, calories, ingredientIndex + 1, teaspoonsLeft - spoons, partialScores, highestScore);
	}
}

void Puzzle15_A_2015()
{
	const int64_t teaspoons = 100;

	vector<Properties> recipe = ParseRecipe();
	vector<Properties> scores(recipe.size() + 1);

	int32_t answer = 0;
	FindHighestScoreWithoutCalories(recipe, 0, teaspoons, &scores, &answer);

	return PuzzleOutput::Submit(2015, 15, 1, answer);
}

void Puzzle15_B_2015()
{
	const int64_t teaspoons = 100;
	const int64_t calories = 500;

	vector<Properties> recipe = ParseRecipe();
	vector<Properties> scores(recipe.size() + 1);

	int32_t answer = 0;
	FindHighestScoreWithSpecifiedCalories(recipe, calories, 0, teaspoons, &scores, &answer);

	return PuzzleOutput::Submit(2015, 15, 2, answer);
}
