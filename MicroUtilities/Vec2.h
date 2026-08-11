#pragma once

#pragma once

#include <functional>
#include <array>
#include <map>
#include <limits>
#include <limits.h>
#include <stdint.h>

template <typename COORDINATE_TYPE>
struct Vec2
{
	COORDINATE_TYPE X = 0;
	COORDINATE_TYPE Y = 0;

	auto operator<=>(const Vec2&) const = default;
	bool operator==(const Vec2&) const = default;

	static constexpr Vec2 Up() { return Vec2{ 0, -1 }; }
	static constexpr Vec2 Down() { return Vec2{ 0, 1 }; }
	static constexpr Vec2 Left() { return Vec2{ -1, 0 }; }
	static constexpr Vec2 Right() { return Vec2{ 1, 0 }; }

	static constexpr Vec2 North() { return Vec2{ 0, -1 }; }
	static constexpr Vec2 South() { return Vec2{ 0, 1 }; }
	static constexpr Vec2 West() { return Vec2{ -1, 0 }; }
	static constexpr Vec2 East() { return Vec2{ 1, 0 }; }

	static constexpr Vec2 NorthEast() { return { 1, -1 }; }
	static constexpr Vec2 NorthWest() { return { -1, -1 }; }
	static constexpr Vec2 SouthEast() { return { 1, 1 }; }
	static constexpr Vec2 SouthWest() { return { -1, 1 }; }

	static const std::array<Vec2, 4>& CardinalDirections();
	static const std::array<Vec2, 8>& CardinalAndDiagonalDirections();
	static Vec2 DirectionFromChar(char d);

	static Vec2 RotateClockwise(const Vec2& direction);
	static Vec2 RotateAnticlockwise(const Vec2& direction);

	static constexpr Vec2 Min() { return Vec2{ std::numeric_limits<COORDINATE_TYPE>::min(), std::numeric_limits<COORDINATE_TYPE>::min() }; }
	static constexpr Vec2 Max() { return Vec2{ std::numeric_limits<COORDINATE_TYPE>::max(), std::numeric_limits<COORDINATE_TYPE>::max() }; }

	static Vec2 MinElements(const Vec2& a, const Vec2& b);
	static Vec2 MaxElements(const Vec2& a, const Vec2& b);

	static COORDINATE_TYPE GetX(const Vec2& p)
	{
		return p.X;
	}

	static COORDINATE_TYPE GetY(const Vec2& p)
	{
		return p.Y;
	}

	Vec2& operator+=(const Vec2& other)
	{
		X += other.X;
		Y += other.Y;
		return *this;
	}

	Vec2& operator-=(const Vec2& other)
	{
		X -= other.X;
		Y -= other.Y;
		return *this;
	}

	Vec2& operator*=(COORDINATE_TYPE scalar)
	{
		X *= scalar;
		Y *= scalar;
		return *this;
	}
};

template <typename COORDINATE_TYPE>
inline Vec2<COORDINATE_TYPE> operator+(const Vec2<COORDINATE_TYPE>& a, const Vec2<COORDINATE_TYPE>& b)
{
	return Vec2<COORDINATE_TYPE>{ COORDINATE_TYPE(a.X + b.X), COORDINATE_TYPE(a.Y + b.Y) };
}

template <typename COORDINATE_TYPE>
inline Vec2<COORDINATE_TYPE> operator-(const Vec2<COORDINATE_TYPE>& a, const Vec2<COORDINATE_TYPE>& b)
{
	return Vec2<COORDINATE_TYPE>{ COORDINATE_TYPE(a.X - b.X), COORDINATE_TYPE(a.Y - b.Y) };
}

template <typename COORDINATE_TYPE>
inline Vec2<COORDINATE_TYPE> operator-(const Vec2<COORDINATE_TYPE>& a)
{
	return Vec2<COORDINATE_TYPE>{ -a.X, -a.Y };
}

template <typename COORDINATE_TYPE, typename SCALAR_TYPE>
inline Vec2<COORDINATE_TYPE> operator*(const Vec2<COORDINATE_TYPE>& a, SCALAR_TYPE scalar)
{
	return Vec2<COORDINATE_TYPE>{ a.X * scalar, a.Y * scalar };
}

template <typename SCALAR_TYPE, typename COORDINATE_TYPE>
inline Vec2<COORDINATE_TYPE> operator*(SCALAR_TYPE scalar, const Vec2<COORDINATE_TYPE>& b)
{
	return Vec2{ scalar * b.X, scalar * b.Y };
}

template <typename COORDINATE_TYPE>
struct std::hash<Vec2<COORDINATE_TYPE>>
{
	std::size_t operator()(const Vec2<COORDINATE_TYPE>& v) const noexcept
	{
		uint32_t hash = 0x811c9dc5;

		hash ^= v.X;
		hash *= 0x01000193;
		hash ^= v.Y;
		hash *= 0x01000193;

		return hash;
	}
};

template <typename COORDINATE_TYPE>
COORDINATE_TYPE ManhattanDistance(const Vec2<COORDINATE_TYPE>& a, const Vec2<COORDINATE_TYPE>& b);

extern template struct Vec2<int8_t>;
extern template struct Vec2<int16_t>;
extern template struct Vec2<int32_t>;
extern template struct Vec2<int64_t>;

using Vec2Byte = Vec2<int8_t>;
using Vec2Short = Vec2<int16_t>;
using Vec2Int = Vec2<int32_t>;
using Vec2Long = Vec2<int64_t>;
