#pragma once
#include <vector>
#include <utility>
#include <bit>
#include <assert.h>

template <typename KEY_TYPE>
class HashSet;

template <typename KEY_TYPE>
class HashSetConstIterator
{
public:
	using value_type = KEY_TYPE;
	using difference_type = ptrdiff_t;

	HashSetConstIterator()
	{
		// Only here to make std::ranges::range<HashSet<...>&> happy
	}

	HashSetConstIterator(const HashSet<KEY_TYPE>* sourceSet, size_t index)
		: SourceSet(sourceSet)
		, Index(index)
	{
		MoveToValidIndex();
	}

	HashSetConstIterator(const HashSetConstIterator<KEY_TYPE>&) = default;
	HashSetConstIterator<KEY_TYPE>& operator=(const HashSetConstIterator<KEY_TYPE>&) = default;
	auto operator<=>(const HashSetConstIterator<KEY_TYPE>&) const = default;

	HashSetConstIterator<KEY_TYPE>& operator++()
	{
		++Index;
		MoveToValidIndex();
		return *this;
	}

	HashSetConstIterator<KEY_TYPE> operator++(int)
	{
		HashSetConstIterator<KEY_TYPE> copy(*this);
		Index++;
		MoveToValidIndex();
		return copy;
	}

	const value_type& operator*() const
	{
		return SourceSet->Table[Index];
	}

	const value_type* operator->() const
	{
		return &SourceSet->Table[Index];
	}

	bool operator==(const HashSetConstIterator<KEY_TYPE>& other) const
	{
		return (SourceSet == other.SourceSet) && (Index == other.Index);
	}

private:
	void MoveToValidIndex()
	{
		while (Index < (SourceSet->TableSizeMask + 1))
		{
			if (SourceSet->Table[Index] != SourceSet->InvalidValue)
			{
				break;
			}

			Index++;
		}
	}

	const HashSet<KEY_TYPE>* SourceSet = nullptr;
	size_t Index = 0;
};

template <typename KEY_TYPE>
class HashSet
{
public:
	using key_type = KEY_TYPE;
	using value_type = KEY_TYPE;
	using size_type = size_t;

	HashSet(size_t size, const key_type& invalidValue)
		: Table(size, invalidValue)
		, TableSizeMask(size - 1)
		, SetSize(0)
		, InvalidValue(invalidValue)
	{
		// Tables must be a power of two
		assert(std::popcount(size) == 1);
	}
	HashSet(const HashSet&) = delete;
	HashSet& operator=(const HashSet&) = delete;

	bool Insert(const key_type& value)
	{
		size_t hashBegin = std::hash<key_type>{}(value);
		size_t hashEnd = hashBegin + Table.size();
		for (size_t i = hashBegin; i < hashEnd; i++)
		{
			const size_t tableIndex = i & TableSizeMask;
			if (Table[tableIndex] == value)
			{
				return false;
			}

			if (Table[tableIndex] == InvalidValue)
			{
				Table[tableIndex] = value;
				SetSize++;
				return true;
			}

#if _DEBUG && WIN32
			DEBUG_Collisions++;
#endif
		}

		assert(false);
		return false;
	}

	bool Contains(const key_type& value) const
	{
		size_t hashBegin = std::hash<key_type>{}(value);
		size_t hashEnd = hashBegin + Table.size();
		for (size_t i = hashBegin; i < hashEnd; i++)
		{
			const size_t tableIndex = i & TableSizeMask;
			if (Table[tableIndex] == value)
			{
				return true;
			}

			if (Table[tableIndex] == InvalidValue)
			{
				return false;
			}
		}

		return false;
	}

	bool Erase(const key_type& value)
	{
		size_t hashBegin = std::hash<key_type>{}(value);
		size_t hashEnd = hashBegin + Table.size();
		for (size_t i = hashBegin; i < hashEnd; i++)
		{
			const size_t tableIndex = i & TableSizeMask;
			if (Table[tableIndex] == InvalidValue)
			{
				return false;
			}

			if (Table[tableIndex] == value)
			{
				Table[tableIndex] = InvalidValue;
				SetSize--;

				// Rehash all of the following entries
				for (size_t followingEntry = i + 1; Table[followingEntry & TableSizeMask] != InvalidValue; followingEntry++)
				{
					key_type temp = Table[followingEntry & TableSizeMask];
					Table[followingEntry & TableSizeMask] = InvalidValue;
					SetSize--;

					Insert(temp);
				}

				return true;
			}
		}

		return false;
	}

	size_t Size() const
	{
		return SetSize;
	}

	HashSetConstIterator<KEY_TYPE> begin() const
	{
		return { this, 0 };
	}

	HashSetConstIterator<KEY_TYPE> end() const
	{
		return { this, TableSizeMask + 1 };
	}

private:
	friend HashSetConstIterator<KEY_TYPE>;
	std::vector<KEY_TYPE> Table;
	size_t TableSizeMask;
	size_t SetSize;
	KEY_TYPE InvalidValue;

#if _DEBUG && WIN32
	size_t DEBUG_Collisions = 0;
#endif
};
