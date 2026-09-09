#include "stdafx.h"

using namespace std;

namespace Puzzle07_2020_Types
{
	template <typename KEY_TYPE, typename MAPPED_TYPE>
	class HashMultiMap;

	template <typename KEY_TYPE, typename MAPPED_TYPE>
	class HashMultiMapEqualRangeIterator
	{
	public:
		using value_type = std::pair<KEY_TYPE, MAPPED_TYPE>;
		using difference_type = ptrdiff_t;

		HashMultiMapEqualRangeIterator()
		{
			// Only here to make std::ranges::range<HashMap<...>&> happy
		}

		HashMultiMapEqualRangeIterator(const HashMultiMap<KEY_TYPE, MAPPED_TYPE>* sourceMap, const KEY_TYPE& key, uint32_t index)
			: SourceMap(sourceMap)
			, Key(key)
			, Index(index)
		{
			MoveToValidIndex();
		}

		HashMultiMapEqualRangeIterator(const HashMultiMapEqualRangeIterator<KEY_TYPE, MAPPED_TYPE>&) = default;
		HashMultiMapEqualRangeIterator<KEY_TYPE, MAPPED_TYPE>& operator=(const HashMultiMapEqualRangeIterator<KEY_TYPE, MAPPED_TYPE>&) = default;
		auto operator<=>(const HashMultiMapEqualRangeIterator<KEY_TYPE, MAPPED_TYPE>&) const = default;

		HashMultiMapEqualRangeIterator<KEY_TYPE, MAPPED_TYPE>& operator++()
		{
			++Index;
			MoveToValidIndex();
			return *this;
		}

		HashMultiMapEqualRangeIterator<KEY_TYPE, MAPPED_TYPE> operator++(int)
		{
			HashMultiMapEqualRangeIterator<KEY_TYPE, MAPPED_TYPE> copy(*this);
			Index++;
			MoveToValidIndex();
			return copy;
		}

		const value_type& operator*() const
		{
			return SourceMap->Table[Index];
		}

		const value_type* operator->() const
		{
			return &SourceMap->Table[Index];
		}

		bool operator==(const HashMultiMapEqualRangeIterator<KEY_TYPE, MAPPED_TYPE>& other) const
		{
			return (SourceMap == other.SourceMap) && (Index == other.Index);
		}

	private:
		void MoveToValidIndex()
		{
			while (Index < (SourceMap->TableSizeMask + 1))
			{
				if (SourceMap->Table[Index].first == SourceMap->InvalidKeyValue)
				{
					Index = SourceMap->TableSizeMask + 1;
					break;
				}

				if (SourceMap->Table[Index].first == Key)
				{
					break;
				}

				Index++;
			}
		}

		const HashMultiMap<KEY_TYPE, MAPPED_TYPE>* SourceMap = nullptr;
		KEY_TYPE Key = {};
		uint32_t Index = 0;
	};

	template <typename KEY_TYPE, typename MAPPED_TYPE>
	struct HashMultiMapEqualRange
	{
		const HashMultiMap<KEY_TYPE, MAPPED_TYPE>* SourceMap = nullptr;
		KEY_TYPE Key = {};
		uint32_t Index = 0;

		HashMultiMapEqualRangeIterator<KEY_TYPE, MAPPED_TYPE> begin() const
		{
			return HashMultiMapEqualRangeIterator<KEY_TYPE, MAPPED_TYPE>(SourceMap, Key, Index);
		}
		HashMultiMapEqualRangeIterator<KEY_TYPE, MAPPED_TYPE> end() const
		{
			return HashMultiMapEqualRangeIterator<KEY_TYPE, MAPPED_TYPE>(SourceMap, Key, SourceMap->TableSizeMask + 1);
		}
	};

	template <typename KEY_TYPE, typename MAPPED_TYPE>
	class HashMultiMap
	{
	public:
		using key_type = KEY_TYPE;
		using mapped_type = MAPPED_TYPE;
		using value_type = std::pair<KEY_TYPE, MAPPED_TYPE>;
		using size_type = uint32_t;

		HashMultiMap(uint32_t size, const key_type& invalidKeyValue)
			: Table(size, { invalidKeyValue, {} })
			, TableSizeMask(size - 1)
			, MapSize(0)
			, InvalidKeyValue(invalidKeyValue)
		{
			// Tables must be a power of two
			assert(std::popcount(size) == 1);
		}

		HashMultiMap(HashMultiMap&&) = default;
		HashMultiMap& operator=(HashMultiMap&&) = default;

		HashMultiMap(const HashMultiMap&) = delete;
		HashMultiMap& operator=(const HashMultiMap&) = delete;

		bool Insert(const key_type& key, const mapped_type& value)
		{
			uint32_t hashIndex = static_cast<uint32_t>(std::hash<key_type>{}(key));
			const uint32_t tableSize = static_cast<uint32_t>(Table.size());
			for (uint32_t i = 0; i < tableSize; i++, hashIndex++)
			{
				const uint32_t tableIndex = hashIndex & TableSizeMask;
				if (Table[tableIndex].first == InvalidKeyValue)
				{
					Table[tableIndex] = { key, value };
					MapSize++;
					return true;
				}

#if _DEBUG && WIN32
				DEBUG_Collisions++;
#endif
			}

			assert(false);
			return false;
		}

		HashMultiMapEqualRange<KEY_TYPE, MAPPED_TYPE> EqualRange(const key_type& key) const
		{
			uint32_t hashIndex = static_cast<uint32_t>(std::hash<key_type>{}(key));
			uint32_t tableIndex = hashIndex & TableSizeMask;
			return { this, key, tableIndex };
		}

		size_type Size() const
		{
			return MapSize;
		}

	private:
		friend HashMultiMapEqualRangeIterator<KEY_TYPE, MAPPED_TYPE>;
		friend HashMultiMapEqualRange<KEY_TYPE, MAPPED_TYPE>;

		std::vector<value_type> Table;
		uint32_t TableSizeMask;
		uint32_t MapSize;
		key_type InvalidKeyValue;

#if _DEBUG && WIN32
		size_t DEBUG_Collisions = 0;
#endif
	};
}

using namespace Puzzle07_2020_Types;

static uint32_t HashBag(const string& bag)
{
	size_t space = bag.find(' ');
	string tone = bag.substr(0, space);
	string colour = bag.substr(space + 1);
	colour += ' '; // Some colours are only 3 chars

	uint32_t hash = 0;
	for (size_t i = 0; i < 2; i++)
	{
		hash = (hash * 26) + (tone[i] - 'a');
	}
	for (size_t i = 0; i < 4; i++)
	{
		hash = (hash * 26) + (colour[i] - 'a');
	}
	return hash;
}

static uint32_t HashBagFromInput()
{
	SmallVector<char, 32> colour;
	array<size_t, 2> spaces = {};

	int32_t spacesRead = 0;
	while (spacesRead < 2)
	{
		char c = static_cast<char>(PuzzleInput::GetChar());
		if (c == ' ')
		{
			spaces[spacesRead] = colour.size();
			spacesRead++;
		}
		colour.PushBack(c);
	}
	colour.PushBack('\0'); // For debugging

	uint32_t hash = 0;
	for (size_t i = 0; i < 2; i++)
	{
		hash = (hash * 26) + (colour[i] - 'a');
	}
	for (size_t i = 0; i < 4; i++)
	{
		hash = (hash * 26) + (colour[i + spaces[0] + 1] - 'a');
	}

	return hash;
}

void Puzzle07_A_2020()
{
	HashMultiMap<uint32_t, uint32_t> containedBy(4096, numeric_limits<uint32_t>::max());
	while (PuzzleInput::NextLine())
	{
		uint32_t parent = HashBagFromInput();
		Parse::DiscardExpected("bags contain ");

		if (!isdigit(PuzzleInput::PeekChar()))
		{
			PuzzleInput::DropLine();
			continue;
		}

		while (true)
		{
			int32_t numChildren = Parse::GetInt32();
			(void)numChildren;
			PuzzleInput::DropChar();

			uint32_t child = HashBagFromInput();
			containedBy.Insert(child, parent);

			while (isalpha(PuzzleInput::PeekChar()) || (isspace(PuzzleInput::PeekChar())))
				PuzzleInput::DropChar();

			if (PuzzleInput::GetChar() == '.')
				break;
		}
	}

	HashSet<uint32_t> visited(1024, numeric_limits<uint32_t>::max());

	vector<uint32_t> searchQueue;
	searchQueue.reserve(400);
	searchQueue.push_back(HashBag("shiny gold"));
	for (size_t i = 0; i < searchQueue.size(); i++)
	{
		if (visited.Contains(searchQueue[i]))
			continue;

		for (const auto& childPair : containedBy.EqualRange(searchQueue[i]))
		{
			uint32_t parent = childPair.second;
			if (visited.Contains(parent) == false)
			{
				searchQueue.push_back(parent);
			}
		}

		visited.Insert(searchQueue[i]);
	}

	int32_t answer = visited.Size() - 1;

	PuzzleOutput::Submit(2020, 7, 1, answer);
}

void Puzzle07_B_2020()
{
	HashMap<uint32_t, SmallVector<pair<int32_t, uint32_t>, 4>> contains(1024, numeric_limits<uint32_t>::max());
	while (PuzzleInput::NextLine())
	{
		uint32_t parent = HashBagFromInput();
		Parse::DiscardExpected("bags contain ");

		if (!isdigit(PuzzleInput::PeekChar()))
		{
			contains[parent]; // Create empty
			PuzzleInput::DropLine();
			continue;
		}

		while (true)
		{
			int32_t numChildren = Parse::GetInt32();
			(void)numChildren;
			PuzzleInput::DropChar();

			uint32_t child = HashBagFromInput();
			contains[parent].PushBack({ numChildren, child });

			while (isalpha(PuzzleInput::PeekChar()) || (isspace(PuzzleInput::PeekChar())))
				PuzzleInput::DropChar();

			if (PuzzleInput::GetChar() == '.')
				break;
		}
	}

	HashMap<uint32_t, int32_t> answerCache(64, numeric_limits<uint32_t>::max());

	const uint32_t shinyGold = HashBag("shiny gold"); 

	vector<uint32_t> evaluationStack;
	evaluationStack.reserve(16);
	evaluationStack.push_back(shinyGold);

	while (evaluationStack.empty() == false)
	{
		const uint32_t parent = evaluationStack.back();

		int32_t containsBags = 0;
		for (const auto& childBags : contains.At(parent))
		{
			int32_t childMultiplier;
			if (answerCache.TryFind(childBags.second, &childMultiplier))
			{
				containsBags += childBags.first + childBags.first * childMultiplier;
			}
			else
			{
				evaluationStack.push_back(childBags.second);
				containsBags = -1;
				break;
			}
		}

		if (containsBags == -1)
		{
			continue;
		}

		answerCache.Insert(parent, containsBags);
		evaluationStack.pop_back();
	}

	int32_t answer = answerCache.At(shinyGold);

	PuzzleOutput::Submit(2020, 7, 2, answer);
}
