#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle21_2020_Types
{
	enum : size_t
	{
		MAX_INGREDIENTS = 200,
		MAX_ALLERGENS = 8,
	};

	using IngredientName = SmallVector<char, 8>;
	using AllergenName = SmallVector<char, 12>;

	struct Dish
	{
		vector<uint32_t> Ingredients;
		vector<uint32_t> Allergens;
	};
}

using namespace Puzzle21_2020_Types;

template <>
struct std::hash<IngredientName>
{
	size_t operator()(const IngredientName& name) const noexcept
	{
		uint32_t hash = 0x811c9dc5;
		for (size_t i = 0; i < name.size(); i++)
		{
			hash ^= name[i];
			hash *= 0x01000193;
		}
		return hash;
	}
};

template <>
struct std::hash<AllergenName>
{
	size_t operator()(const AllergenName& name) const noexcept
	{
		uint32_t hash = 0x811c9dc5;
		for (size_t i = 0; i < name.size(); i++)
		{
			hash ^= name[i];
			hash *= 0x01000193;
		}
		return hash;
	}
};

static Dish ReadFood(HashMap<IngredientName, uint32_t>* ingredientDictionary, HashMap<AllergenName, uint32_t>* allergenDictionary)
{
	Dish dish;
	dish.Ingredients.reserve(MAX_INGREDIENTS);
	dish.Allergens.reserve(MAX_ALLERGENS);

	assert(PuzzleInput::PeekChar() != EOF);	

	IngredientName ingredient;
	for (char c : Parse::ReadUntilSeen('('))
	{
		if (c == ' ')
		{
			uint32_t ingredientId = numeric_limits<uint32_t>::max();
			if (ingredientDictionary->TryFind(ingredient, &ingredientId) == false)
			{
				ingredientId = ingredientDictionary->Size();
				ingredientDictionary->Insert(ingredient, ingredientId);
			}

			dish.Ingredients.push_back(ingredientId);
			ingredient.Clear();
		}
		else
		{
			ingredient.PushBack(c);
		}
	}

	Parse::DiscardExpected("contains ");

	AllergenName allergen;
	for (char c : Parse::ReadUntilSeen('\n'))
	{
		if ((c == ',') || (c == ')'))
		{
			uint32_t allergenId = numeric_limits<uint32_t>::max();
			if (allergenDictionary->TryFind(allergen, &allergenId) == false)
			{
				allergenId = allergenDictionary->Size();
				allergenDictionary->Insert(allergen, allergenId);
			}

			dish.Allergens.push_back(allergenId);
			allergen.Clear();
		}
		else if (c == ' ')
		{
			// Ignore spaces
		}
		else
		{
			allergen.PushBack(c);
		}
	}

	ranges::sort(dish.Ingredients);

	return dish;
}

void Puzzle21_A_2020()
{
	HashMap<IngredientName, uint32_t> ingredientDictionary(512, {});
	HashMap<AllergenName, uint32_t> allergenDictionary(32, {});

	vector<int32_t> ingredientCounts(MAX_INGREDIENTS);

	vector<vector<uint32_t>> allergenCandidates(MAX_ALLERGENS);
	while (PuzzleInput::NextLine())
	{
		Dish dish = ReadFood(&ingredientDictionary, &allergenDictionary);

		for (uint32_t ingredient : dish.Ingredients)
		{
			ingredientCounts[ingredient]++;
		}

		for (uint32_t allergen : dish.Allergens)
		{
			if (allergenCandidates[allergen].empty())
			{
				allergenCandidates[allergen] = dish.Ingredients;
			}
			else
			{
				vector<uint32_t> filteredCandidates;
				ranges::set_intersection(allergenCandidates[allergen], dish.Ingredients, back_inserter(filteredCandidates));
				assert(filteredCandidates.empty() == false);
				allergenCandidates[allergen].swap(filteredCandidates);
			}
		}
	}

	for (const vector<uint32_t>& suspects : allergenCandidates)
	{
		for (uint32_t suspect : suspects)
		{
			ingredientCounts[suspect] = 0;
		}
	}

	int32_t answer = accumulate(ingredientCounts.begin(), ingredientCounts.end(), 0);

	PuzzleOutput::Submit(2020, 21, 1, answer);
}

void Puzzle21_B_2020()
{
	HashMap<IngredientName, uint32_t> ingredientDictionary(512, {});
	HashMap<AllergenName, uint32_t> allergenDictionary(32, {});

	vector<int32_t> ingredientCounts(MAX_INGREDIENTS);

	vector<vector<uint32_t>> allergenCandidates(MAX_ALLERGENS);
	while (PuzzleInput::NextLine())
	{
		Dish dish = ReadFood(&ingredientDictionary, &allergenDictionary);

		for (uint32_t ingredient : dish.Ingredients)
		{
			ingredientCounts[ingredient]++;
		}

		for (uint32_t allergen : dish.Allergens)
		{
			if (allergenCandidates[allergen].empty())
			{
				allergenCandidates[allergen] = dish.Ingredients;
			}
			else
			{
				vector<uint32_t> filteredCandidates;
				ranges::set_intersection(allergenCandidates[allergen], dish.Ingredients, back_inserter(filteredCandidates));
				assert(filteredCandidates.empty() == false);
				allergenCandidates[allergen].swap(filteredCandidates);
			}
		}
	}

	vector<string> allergenNames(MAX_ALLERGENS);

	map<string, string> allergenToIngredient;
	for (const auto& allergen : allergenDictionary)
	{
		string allergenName(&allergen.first[0], allergen.first.size());
		allergenToIngredient.insert({ allergenName, {} });
		allergenNames[allergen.second].swap(allergenName);
	}

	vector<uint32_t> ingredientContainsAllergen(MAX_INGREDIENTS, numeric_limits<uint32_t>::max());
	for (int32_t i = 0; i < MAX_ALLERGENS; i++)
	{
		vector<vector<uint32_t>>::const_iterator knownAllergen =
			find_if(allergenCandidates.begin(), allergenCandidates.end(),
				[](vector<vector<uint32_t>>::const_reference candidates)
				{
					return candidates.size() == 1;
				});
		assert(knownAllergen != allergenCandidates.end());

		uint32_t ingredient = knownAllergen->front();
		ingredientContainsAllergen[ingredient] = static_cast<uint32_t>(distance(allergenCandidates.cbegin(), knownAllergen));

		for (vector<vector<uint32_t>>::reference candidates : allergenCandidates)
		{
			vector<uint32_t>::const_iterator it = ranges::find(candidates, ingredient);
			if (it != candidates.end())
			{
				candidates.erase(it);
			}
		}
	}

	for (const auto& ingredient : ingredientDictionary)
	{
		uint32_t containsAllergen = ingredientContainsAllergen[ingredient.second];
		if (containsAllergen != numeric_limits<uint32_t>::max())
		{
			allergenToIngredient[allergenNames[containsAllergen]] = string(&ingredient.first[0], ingredient.first.size());
		}
	}

	string answer;
	for (map<string, string>::const_reference allergenPair : allergenToIngredient)
	{
		if (answer.empty() == false)
		{
			answer += ",";
		}
		answer += allergenPair.second;
	}

	PuzzleOutput::Submit(2020, 21, 2, answer.c_str());
}
