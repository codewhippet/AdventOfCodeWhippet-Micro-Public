#pragma once

#pragma once

#include <functional>
#include <array>
#include <map>
#include <limits>
#include <limits.h>
#include <stdint.h>

template <typename COORDINATE_TYPE>
struct Vec4
{
	COORDINATE_TYPE X = 0;
	COORDINATE_TYPE Y = 0;
	COORDINATE_TYPE Z = 0;
	COORDINATE_TYPE W = 0;

	auto operator<=>(const Vec4&) const = default;
	bool operator==(const Vec4&) const = default;

	static Vec4 MinElements(const Vec4& a, const Vec4& b);
	static Vec4 MaxElements(const Vec4& a, const Vec4& b);
};

template <typename COORDINATE_TYPE>
inline Vec4<COORDINATE_TYPE> operator+(const Vec4<COORDINATE_TYPE>& a, const Vec4<COORDINATE_TYPE>& b)
{
	return Vec4<COORDINATE_TYPE>{ COORDINATE_TYPE(a.X + b.X), COORDINATE_TYPE(a.Y + b.Y), COORDINATE_TYPE(a.Z + b.Z), COORDINATE_TYPE(a.W + b.W) };
}

template <typename COORDINATE_TYPE>
inline Vec4<COORDINATE_TYPE> operator-(const Vec4<COORDINATE_TYPE>& a, const Vec4<COORDINATE_TYPE>& b)
{
	return Vec4<COORDINATE_TYPE>{ COORDINATE_TYPE(a.X - b.X), COORDINATE_TYPE(a.Y - b.Y), COORDINATE_TYPE(a.Z - b.Z), COORDINATE_TYPE(a.W - b.W) };
}

template <typename COORDINATE_TYPE>
inline Vec4<COORDINATE_TYPE> operator-(const Vec4<COORDINATE_TYPE>& a)
{
	return Vec4<COORDINATE_TYPE>{ -a.X, -a.Y, -a.Z, -a.W };
}

template <typename COORDINATE_TYPE>
inline Vec4<COORDINATE_TYPE> operator*(const Vec4<COORDINATE_TYPE>& a, COORDINATE_TYPE scalar)
{
	return Vec4<COORDINATE_TYPE>{ a.X * scalar, a.Y * scalar, a.Z * scalar, a.W * scalar };
}

template <typename COORDINATE_TYPE>
inline Vec4<COORDINATE_TYPE> operator*(COORDINATE_TYPE scalar, const Vec4<COORDINATE_TYPE>& b)
{
	return Vec4{ scalar * b.X, scalar * b.Y, scalar * b.Z, scalar * b.W };
}

template <typename COORDINATE_TYPE>
struct std::hash<Vec4<COORDINATE_TYPE>>
{
	std::size_t operator()(const Vec4<COORDINATE_TYPE>& v) const noexcept
	{
		const int shiftDown = (sizeof(std::size_t) * CHAR_BIT >> 2);
		const int shiftUp = (sizeof(std::size_t) * CHAR_BIT) - shiftDown;

		size_t hash = std::hash<decltype(v.X)>{}(v.X);
		hash = std::hash<decltype(v.Y)>{}(v.Y) ^ (hash >> shiftDown | hash << shiftUp);
		hash = std::hash<decltype(v.Z)>{}(v.Z) ^ (hash >> shiftDown | hash << shiftUp);
		hash = std::hash<decltype(v.W)>{}(v.W) ^ (hash >> shiftDown | hash << shiftUp);
		return hash;
	}
};

template <typename COORDINATE_TYPE>
COORDINATE_TYPE ManhattanDistance(const Vec4<COORDINATE_TYPE>& a, const Vec4<COORDINATE_TYPE>& b);

extern template struct Vec4<int16_t>;
extern template struct Vec4<int32_t>;
extern template struct Vec4<int64_t>;

using Vec4Short = Vec4<int16_t>;
using Vec4Int = Vec4<int32_t>;
using Vec4Long = Vec4<int64_t>;
