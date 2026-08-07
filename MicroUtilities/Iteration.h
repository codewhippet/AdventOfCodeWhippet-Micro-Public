#pragma once

struct AllUnorderedPairsIterator
{
	using value_type = std::pair<size_t, size_t>;
	using difference_type = ptrdiff_t;

	value_type Current = { 0, 0 };
	size_t Size = 0;

	AllUnorderedPairsIterator();
	explicit AllUnorderedPairsIterator(size_t size);

	AllUnorderedPairsIterator& operator++();
	AllUnorderedPairsIterator operator++(int);

	const value_type& operator*() const
	{
		return Current;
	}

	const value_type* operator->() const
	{
		return &Current;
	}

private:
	void NextPair();
};

inline bool operator==(const AllUnorderedPairsIterator& a, const AllUnorderedPairsIterator& b)
{
	// Minimum support for comparing against end sentinel
	return a.Size == b.Size;
}

inline bool operator!=(const AllUnorderedPairsIterator& a, const AllUnorderedPairsIterator& b)
{
	return !(a == b);
}

struct AllUnorderedPairsRange
{
	size_t Size = 0;

	AllUnorderedPairsRange();
	explicit AllUnorderedPairsRange(size_t size);

	AllUnorderedPairsIterator begin() const;
	AllUnorderedPairsIterator end() const;
};

AllUnorderedPairsRange AllUnorderedPairs(size_t size);

// ----------------------------------------------------------------------------

struct IndicesOfIterator
{
public:
	using value_type = size_t;
	using difference_type = ptrdiff_t;

	value_type Current;
	value_type End;

	IndicesOfIterator();
	explicit IndicesOfIterator(size_t size);

	IndicesOfIterator& operator++();
	IndicesOfIterator operator++(int);

	const value_type& operator*() const
	{
		return Current;
	}

	const value_type* operator->() const
	{
		return &Current;
	}

private:
	void Next();
};

inline bool operator==(const IndicesOfIterator& a, const IndicesOfIterator& b)
{
	// Minimum support for comparing against end sentinel
	return a.End == b.End;
}

inline bool operator!=(const IndicesOfIterator& a, const IndicesOfIterator& b)
{
	return !(a == b);
}

struct IndicesOfRange
{
	size_t Size = 0;

	IndicesOfRange();
	explicit IndicesOfRange(size_t size);

	IndicesOfIterator begin() const;
	IndicesOfIterator end() const;
};

template <typename CONTAINER>
concept HasSizeMemberFunction = requires(CONTAINER c)
{
	{ c.size() } -> std::convertible_to<std::size_t>;
};

inline IndicesOfRange IndicesOf(const HasSizeMemberFunction auto& c)
{
	return IndicesOfRange(c.size());
}
