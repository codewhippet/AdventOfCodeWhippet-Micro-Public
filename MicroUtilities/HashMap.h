#pragma once
#include <vector>
#include <utility>
#include <bit>
#include <stdint.h>
#include <assert.h>
#include <stdint.h>

template <typename KEY_TYPE, typename MAPPED_TYPE>
class HashMap;

template <typename KEY_TYPE, typename MAPPED_TYPE>
class HashMapConstIterator
{
public:
	using value_type = std::pair<KEY_TYPE, MAPPED_TYPE>;
	using difference_type = ptrdiff_t;

	HashMapConstIterator()
	{
		// Only here to make std::ranges::range<HashMap<...>&> happy
	}

	HashMapConstIterator(const HashMap<KEY_TYPE, MAPPED_TYPE>* sourceMap, uint32_t index)
		: SourceMap(sourceMap)
		, Index(index)
	{
		MoveToValidIndex();
	}

	HashMapConstIterator(const HashMapConstIterator<KEY_TYPE, MAPPED_TYPE>&) = default;
	HashMapConstIterator<KEY_TYPE, MAPPED_TYPE>& operator=(const HashMapConstIterator<KEY_TYPE, MAPPED_TYPE>&) = default;
	auto operator<=>(const HashMapConstIterator<KEY_TYPE, MAPPED_TYPE>&) const = default;

	HashMapConstIterator<KEY_TYPE, MAPPED_TYPE>& operator++()
	{
		++Index;
		MoveToValidIndex();
		return *this;
	}

	HashMapConstIterator<KEY_TYPE, MAPPED_TYPE> operator++(int)
	{
		HashMapConstIterator<KEY_TYPE, MAPPED_TYPE> copy(*this);
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

	bool operator==(const HashMapConstIterator<KEY_TYPE, MAPPED_TYPE>& other) const
	{
		return (SourceMap == other.SourceMap) && (Index == other.Index);
	}

private:
	void MoveToValidIndex()
	{
		while (Index < (SourceMap->TableSizeMask + 1))
		{
			if (SourceMap->Table[Index].first != SourceMap->InvalidKeyValue)
			{
				break;
			}

			Index++;
		}
	}

	const HashMap<KEY_TYPE, MAPPED_TYPE>* SourceMap = nullptr;
	uint32_t Index = 0;
};

template <typename KEY_TYPE, typename MAPPED_TYPE>
class HashMap
{
public:
	using key_type = KEY_TYPE;
	using mapped_type = MAPPED_TYPE;
	using value_type = std::pair<KEY_TYPE, MAPPED_TYPE>;
	using size_type = uint32_t;

	HashMap(uint32_t size, const key_type& invalidKeyValue)
		: Table(size, { invalidKeyValue, {} })
		, TableSizeMask(size - 1)
		, MapSize(0)
		, InvalidKeyValue(invalidKeyValue)
	{
		// Tables must be a power of two
		assert(std::popcount(size) == 1);
	}

	HashMap(HashMap&&) = default;
	HashMap& operator=(HashMap&&) = default;

	HashMap(const HashMap&) = delete;
	HashMap& operator=(const HashMap&) = delete;

	bool Insert(const key_type& key, const mapped_type& value)
	{
		uint32_t hashIndex = static_cast<uint32_t>(std::hash<key_type>{}(key));
		const uint32_t tableSize = static_cast<uint32_t>(Table.size());
		for (uint32_t i = 0; i < tableSize; i++, hashIndex++)
		{
			const uint32_t tableIndex = hashIndex & TableSizeMask;
			if (Table[tableIndex].first == key)
			{
				return false;
			}

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

	bool Set(const key_type& key, const mapped_type& value)
	{
		uint32_t hashIndex = static_cast<uint32_t>(std::hash<key_type>{}(key));
		const uint32_t tableSize = static_cast<uint32_t>(Table.size());
		for (uint32_t i = 0; i < tableSize; i++, hashIndex++)
		{
			const uint32_t tableIndex = hashIndex & TableSizeMask;
			if (Table[tableIndex].first == key)
			{
				Table[tableIndex].second = value;
				return true;
			}

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

	bool Contains(const key_type& key) const
	{
		uint32_t hashIndex = static_cast<uint32_t>(std::hash<key_type>{}(key));
		const uint32_t tableSize = static_cast<uint32_t>(Table.size());
		for (uint32_t i = 0; i < tableSize; i++, hashIndex++)
		{
			const uint32_t tableIndex = hashIndex & TableSizeMask;
			if (Table[tableIndex].first == key)
			{
				return true;
			}

			if (Table[tableIndex].first == InvalidKeyValue)
			{
				return false;
			}
		}

		return false;
	}

	mapped_type FindOrDefault(const key_type& key, const mapped_type& def)
	{
		uint32_t hashIndex = static_cast<uint32_t>(std::hash<key_type>{}(key));
		const uint32_t tableSize = static_cast<uint32_t>(Table.size());
		for (uint32_t i = 0; i < tableSize; i++, hashIndex++)
		{
			const uint32_t tableIndex = hashIndex & TableSizeMask;
			if (Table[tableIndex].first == key)
			{
				return Table[tableIndex].second;
			}

			if (Table[tableIndex].first == InvalidKeyValue)
			{
				return def;
			}
		}

		return def;
	}

	bool TryFind(const key_type& key, mapped_type* out)
	{
		uint32_t hashIndex = static_cast<uint32_t>(std::hash<key_type>{}(key));
		const uint32_t tableSize = static_cast<uint32_t>(Table.size());
		for (uint32_t i = 0; i < tableSize; i++, hashIndex++)
		{
			const uint32_t tableIndex = hashIndex & TableSizeMask;
			if (Table[tableIndex].first == key)
			{
				*out = Table[tableIndex].second;
				return true;
			}

			if (Table[tableIndex].first == InvalidKeyValue)
			{
				return false;
			}
		}

		return false;
	}

	mapped_type& At(const key_type& key)
	{
		uint32_t hashIndex = static_cast<uint32_t>(std::hash<key_type>{}(key));
		const uint32_t tableSize = static_cast<uint32_t>(Table.size());
		for (uint32_t i = 0; i < tableSize; i++, hashIndex++)
		{
			const uint32_t tableIndex = hashIndex & TableSizeMask;
			if (Table[tableIndex].first == key)
			{
				return Table[tableIndex].second;
			}
		}

		//Hardware::FlashingStop(10);
		assert(false);
		return *reinterpret_cast<mapped_type*>(0);
	}

	size_type Size() const
	{
		return MapSize;
	}

	HashMapConstIterator<KEY_TYPE, MAPPED_TYPE> begin() const
	{
		return { this, 0 };
	}

	HashMapConstIterator<KEY_TYPE, MAPPED_TYPE> end() const
	{
		return { this, TableSizeMask + 1 };
	}

private:
	friend HashMapConstIterator<KEY_TYPE, MAPPED_TYPE>;

	std::vector<value_type> Table;
	uint32_t TableSizeMask;
	uint32_t MapSize;
	key_type InvalidKeyValue;

#if _DEBUG && WIN32
	size_t DEBUG_Collisions = 0;
#endif
};