#include "stdafx.h"
#include "Vec2Iteration.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////////

template <typename VEC2_TYPE>
uLineIterator<VEC2_TYPE>& uLineIterator<VEC2_TYPE>::operator++()
{
	NextLinePoint();
	return *this;
}

template <typename VEC2_TYPE>
uLineIterator<VEC2_TYPE> uLineIterator<VEC2_TYPE>::operator++(int)
{
	uLineIterator<VEC2_TYPE> copy(*this);
	NextLinePoint();
	return copy;
}

template <typename VEC2_TYPE>
const VEC2_TYPE& uLineIterator<VEC2_TYPE>::operator*() const
{
	return Current;
}

template <typename VEC2_TYPE>
const VEC2_TYPE* uLineIterator<VEC2_TYPE>::operator->() const
{
	return &Current;
}

template <typename VEC2_TYPE>
void uLineIterator<VEC2_TYPE>::NextLinePoint()
{
	assert(Current != End);
	Current = Current + Direction;
}

template <typename VEC2_TYPE>
bool operator==(const uLineIterator<VEC2_TYPE>& a, const uLineIterator<VEC2_TYPE>& b)
{
	return a.Current == b.Current;
}

template <typename VEC2_TYPE>
bool operator!=(const uLineIterator<VEC2_TYPE>& a, const uLineIterator<VEC2_TYPE>& b)
{
	return !(a == b);
}

static_assert(std::input_or_output_iterator<uLineIterator<Vec2Int>>);

template<typename VEC2_TYPE>
uLineInclusiveRange<VEC2_TYPE>::uLineInclusiveRange(const VEC2_TYPE& first, const VEC2_TYPE& last)
	: First(first)
	, Last(last)
{
	//assert(First != Last);
}

template<typename VEC2_TYPE>
uLineIterator<VEC2_TYPE> uLineInclusiveRange<VEC2_TYPE>::begin() const
{
	VEC2_TYPE direction = GetDirection();
	return { First, Last + direction, First, direction };
}

template<typename VEC2_TYPE>
uLineIterator<VEC2_TYPE> uLineInclusiveRange<VEC2_TYPE>::end() const
{
	VEC2_TYPE direction = GetDirection();
	return { First, Last + direction, Last + direction, direction };
}

template<typename VEC2_TYPE>
VEC2_TYPE uLineInclusiveRange<VEC2_TYPE>::GetDirection() const
{
	if (First.X == Last.X)
	{
		return (First.Y < Last.Y) ? VEC2_TYPE::Down() : VEC2_TYPE::Up();
	}
	else
	{
		return (First.X < Last.X) ? VEC2_TYPE::Right() : VEC2_TYPE::Left();
	}
}

static_assert(std::ranges::input_range<uLineInclusiveRange<Vec2Int>>);

//////////////////////////////////////////////////////////////////////////

template struct uLineIterator<Vec2Short>;
template struct uLineInclusiveRange<Vec2Short>;
template bool operator==(const uLineIterator<Vec2Short>& a, const uLineIterator<Vec2Short>& b);
template bool operator!=(const uLineIterator<Vec2Short>& a, const uLineIterator<Vec2Short>& b);

template struct uLineIterator<Vec2Int>;
template struct uLineInclusiveRange<Vec2Int>;
template bool operator==(const uLineIterator<Vec2Int>& a, const uLineIterator<Vec2Int>& b);
template bool operator!=(const uLineIterator<Vec2Int>& a, const uLineIterator<Vec2Int>& b);

//////////////////////////////////////////////////////////////////////////

template <typename VEC2_TYPE>
uGridIterator<VEC2_TYPE>& uGridIterator<VEC2_TYPE>::operator++()
{
	NextGridPoint();
	return *this;
}

template <typename VEC2_TYPE>
uGridIterator<VEC2_TYPE> uGridIterator<VEC2_TYPE>::operator++(int)
{
	uGridIterator copy(*this);
	NextGridPoint();
	return copy;
}

template <typename VEC2_TYPE>
const VEC2_TYPE& uGridIterator<VEC2_TYPE>::operator*() const
{
	return Current;
}

template <typename VEC2_TYPE>
const VEC2_TYPE* uGridIterator<VEC2_TYPE>::operator->() const
{
	return &Current;
}

template <typename VEC2_TYPE>
void uGridIterator<VEC2_TYPE>::NextGridPoint()
{
	assert(Current != End);
	if (++Current.X == End.X)
	{
		Current.X = Begin.X;
		Current.Y++;
	}
}

template <typename VEC2_TYPE>
bool operator==(const uGridIterator<VEC2_TYPE>& a, const uGridIterator<VEC2_TYPE>& b)
{
	return a.Current == b.Current;
}

template <typename VEC2_TYPE>
bool operator!=(const uGridIterator<VEC2_TYPE>& a, const uGridIterator<VEC2_TYPE>& b)
{
	return !(a == b);
}

static_assert(std::input_or_output_iterator<uGridIterator<Vec2Int>>);

template <typename VEC2_TYPE>
uGridRange<VEC2_TYPE>::uGridRange(const VEC2_TYPE& begin, const VEC2_TYPE& end)
	: Begin(begin)
	, End(end)
{
}

template <typename VEC2_TYPE>
uGridIterator<VEC2_TYPE> uGridRange<VEC2_TYPE>::begin() const
{
	return { Begin, End, Begin };
}

template <typename VEC2_TYPE>
uGridIterator<VEC2_TYPE> uGridRange<VEC2_TYPE>::end() const
{
	return { Begin, End, { Begin.X, End.Y } };
}

static_assert(std::ranges::input_range<uGridRange<Vec2Int>>);

//////////////////////////////////////////////////////////////////////////

template struct uGridIterator<Vec2Short>;
template struct uGridRange<Vec2Short>;
template bool operator==(const uGridIterator<Vec2Short>& a, const uGridIterator<Vec2Short>& b);
template bool operator!=(const uGridIterator<Vec2Short>& a, const uGridIterator<Vec2Short>& b);

template struct uGridIterator<Vec2Int>;
template struct uGridRange<Vec2Int>;
template bool operator==(const uGridIterator<Vec2Int>& a, const uGridIterator<Vec2Int>& b);
template bool operator!=(const uGridIterator<Vec2Int>& a, const uGridIterator<Vec2Int>& b);

//////////////////////////////////////////////////////////////////////////
