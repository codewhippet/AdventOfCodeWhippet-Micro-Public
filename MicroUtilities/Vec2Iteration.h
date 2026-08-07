#pragma once
#include "Vec2.h"

//////////////////////////////////////////////////////////////////////////

template <typename VEC2_TYPE>
struct uLineIterator
{
	using value_type = VEC2_TYPE;
	using difference_type = ptrdiff_t;

	VEC2_TYPE Begin;
	VEC2_TYPE End;
	VEC2_TYPE Current;

	VEC2_TYPE Direction;
	
	uLineIterator& operator++();
	uLineIterator operator++(int);

	const VEC2_TYPE& operator*() const;
	const VEC2_TYPE* operator->() const;

private:

	void NextLinePoint();
};

template <typename VEC2_TYPE>
bool operator==(const uLineIterator<VEC2_TYPE>& a, const uLineIterator<VEC2_TYPE>& b);

template <typename VEC2_TYPE>
bool operator!=(const uLineIterator<VEC2_TYPE>& a, const uLineIterator<VEC2_TYPE>& b);

template <typename VEC2_TYPE>
struct uLineInclusiveRange
{
	VEC2_TYPE First;
	VEC2_TYPE Last;

	uLineInclusiveRange(const VEC2_TYPE& first, const VEC2_TYPE& last);

	uLineIterator<VEC2_TYPE> begin() const;
	uLineIterator<VEC2_TYPE> end() const;

private:
	VEC2_TYPE GetDirection() const;
};

//////////////////////////////////////////////////////////////////////////

extern template struct uLineIterator<Vec2Int>;
extern template struct uLineInclusiveRange<Vec2Int>;
extern template bool operator==(const uLineIterator<Vec2Int>& a, const uLineIterator<Vec2Int>& b);
extern template bool operator!=(const uLineIterator<Vec2Int>& a, const uLineIterator<Vec2Int>& b);

//////////////////////////////////////////////////////////////////////////

template <typename VEC2_TYPE>
struct uGridIterator
{
	using value_type = VEC2_TYPE;
	using difference_type = ptrdiff_t;

	VEC2_TYPE Begin;
	VEC2_TYPE End;
	VEC2_TYPE Current;

	uGridIterator& operator++();
	uGridIterator operator++(int);

	const VEC2_TYPE& operator*() const;
	const VEC2_TYPE* operator->() const;

private:
	void NextGridPoint();
};

template <typename VEC2_TYPE>
bool operator==(const uGridIterator<VEC2_TYPE>& a, const uGridIterator<VEC2_TYPE>& b);

template <typename VEC2_TYPE>
bool operator!=(const uGridIterator<VEC2_TYPE>& a, const uGridIterator<VEC2_TYPE>& b);

template <typename VEC2_TYPE>
struct uGridRange
{
	VEC2_TYPE Begin;
	VEC2_TYPE End;

	uGridRange(const VEC2_TYPE& begin, const VEC2_TYPE& end);

	uGridIterator<VEC2_TYPE> begin() const;
	uGridIterator<VEC2_TYPE> end() const;
};

//////////////////////////////////////////////////////////////////////////

extern template struct uGridIterator<Vec2Int>;
extern template struct uGridRange<Vec2Int>;
extern template bool operator==(const uGridIterator<Vec2Int>& a, const uGridIterator<Vec2Int>& b);
extern template bool operator!=(const uGridIterator<Vec2Int>& a, const uGridIterator<Vec2Int>& b);

//////////////////////////////////////////////////////////////////////////
