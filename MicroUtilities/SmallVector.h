#pragma once
#include <compare>
#include <array>
#include <stdint.h>
#include <stddef.h>

template <typename TYPE, size_t SIZE>
class SmallVector;

template <typename TYPE, size_t SIZE>
class SmallVectorConstIterator
{
public:
	using my_type = SmallVectorConstIterator<TYPE, SIZE>;
    using value_type = TYPE;
    using difference_type = ptrdiff_t;

	SmallVectorConstIterator()
	{
		// Only here to make std::ranges::range<SmallVector<...>&> happy
	}

	SmallVectorConstIterator(const SmallVector<TYPE, SIZE>* sourceVector, size_t index)
		: SourceVector(sourceVector)
		, Index(index)
	{
	}

	SmallVectorConstIterator(const my_type&) = default;
	my_type& operator=(const my_type&) = default;
	auto operator<=>(const my_type&) const = default;

	my_type& operator++()
	{
		++Index;
		return *this;
	}

	my_type operator++(int)
	{
		my_type copy(*this);
		Index++;
		return copy;
	}

	const value_type& operator*() const
	{
		return SourceVector->Data[Index];
	}

	const value_type* operator->() const
	{
		return &SourceVector->Data[Index];
	}

	bool operator==(const my_type& other) const
	{
		return (SourceVector == other.SourceVector) && (Index == other.Index);
	}

private:
	const SmallVector<TYPE, SIZE>* SourceVector = nullptr;
	size_t Index = 0;
};

template <typename TYPE, size_t SIZE>
class SmallVector
{
public:

	using value_type = TYPE;
	using reference = value_type&;
	using const_reference = const value_type&;

	reference PushBack(const TYPE& value)
	{
		Data[Size] = value;
		return Data[Size++];
	}

	reference operator[](size_t index)
	{
		return Data[index];
	}

	const_reference operator[](size_t index) const
	{
		return Data[index];
	}

	int32_t size() const
	{
		return Size;
	}

	SmallVectorConstIterator<TYPE, SIZE> begin() const
	{
		return { this, 0 };
	}

	SmallVectorConstIterator<TYPE, SIZE> end() const
	{
		return { this, SIZE };
	}

private:
	friend SmallVectorConstIterator<TYPE, SIZE>;

    int32_t Size = 0;
    std::array<TYPE, SIZE> Data;
};
