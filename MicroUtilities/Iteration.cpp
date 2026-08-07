#include "stdafx.h"
#include <ranges>

static_assert(std::input_or_output_iterator<AllUnorderedPairsIterator>);
static_assert(std::ranges::input_range<AllUnorderedPairsRange>);

AllUnorderedPairsIterator::AllUnorderedPairsIterator() = default;

AllUnorderedPairsIterator::AllUnorderedPairsIterator(size_t size)
	: Size(size)
{
	assert(Size > 0);
	NextPair();
}

AllUnorderedPairsIterator& AllUnorderedPairsIterator::operator++()
{
	NextPair();
	return *this;
}

AllUnorderedPairsIterator AllUnorderedPairsIterator::operator++(int)
{
	AllUnorderedPairsIterator copy(*this);
	NextPair();
	return copy;
}

void AllUnorderedPairsIterator::NextPair()
{
	if (Size > 0)
	{
		Current.second++;
		if (Current.second == Size)
		{
			Current.first++;
			Current.second = Current.first + 1;
			if (Current.second == Size)
			{
				Size = 0;
			}
		}
	}
}

AllUnorderedPairsRange::AllUnorderedPairsRange() = default;

AllUnorderedPairsRange::AllUnorderedPairsRange(size_t size)
	: Size(size)
{
}

AllUnorderedPairsIterator AllUnorderedPairsRange::begin() const
{
	return Size > 1 ? AllUnorderedPairsIterator{ Size } : AllUnorderedPairsIterator{};
}

AllUnorderedPairsIterator AllUnorderedPairsRange::end() const
{
	return {};
}

AllUnorderedPairsRange AllUnorderedPairs(size_t size)
{
	return AllUnorderedPairsRange{ size };
}

// ----------------------------------------------------------------------------

static_assert(std::input_or_output_iterator<IndicesOfIterator>);
static_assert(std::ranges::input_range<IndicesOfRange>);

IndicesOfIterator::IndicesOfIterator()
	: Current(0)
	, End(0)
{
}

IndicesOfIterator::IndicesOfIterator(size_t size)
	: Current(0)
	, End(size)
{
}

IndicesOfIterator& IndicesOfIterator::operator++()
{
	Next();
	return *this;
}

IndicesOfIterator IndicesOfIterator::operator++(int)
{
	IndicesOfIterator copy(*this);
	Next();
	return copy;
}

void IndicesOfIterator::Next()
{
	if (End > 0)
	{
		if (++Current == End)
		{
			Current = 0;
			End = 0;
		}
	}
}

IndicesOfRange::IndicesOfRange() = default;

IndicesOfRange::IndicesOfRange(size_t size)
	: Size(size)
{
}

IndicesOfIterator IndicesOfRange::begin() const
{
	return IndicesOfIterator{ Size };
}

IndicesOfIterator IndicesOfRange::end() const
{
	return {};
}
