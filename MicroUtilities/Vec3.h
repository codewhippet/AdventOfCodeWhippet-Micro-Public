#pragma once

#pragma once

#include <functional>
#include <array>
#include <map>
#include <limits>
#include <limits.h>
#include <stdint.h>

template <typename COORDINATE_TYPE>
struct Vec3
{
	COORDINATE_TYPE X = 0;
	COORDINATE_TYPE Y = 0;
	COORDINATE_TYPE Z = 0;

	auto operator<=>(const Vec3&) const = default;
	bool operator==(const Vec3&) const = default;

	static const std::array<Vec3, 6>& CardinalDirections();

	static Vec3 MinElements(const Vec3& a, const Vec3& b);
	static Vec3 MaxElements(const Vec3& a, const Vec3& b);
};

template <typename COORDINATE_TYPE>
inline Vec3<COORDINATE_TYPE> operator+(const Vec3<COORDINATE_TYPE>& a, const Vec3<COORDINATE_TYPE>& b)
{
	return Vec3<COORDINATE_TYPE>{ COORDINATE_TYPE(a.X + b.X), COORDINATE_TYPE(a.Y + b.Y), COORDINATE_TYPE(a.Z + b.Z) };
}

template <typename COORDINATE_TYPE>
inline Vec3<COORDINATE_TYPE> operator-(const Vec3<COORDINATE_TYPE>& a, const Vec3<COORDINATE_TYPE>& b)
{
	return Vec3<COORDINATE_TYPE>{ COORDINATE_TYPE(a.X - b.X), COORDINATE_TYPE(a.Y - b.Y), COORDINATE_TYPE(a.Z - b.Z) };
}

template <typename COORDINATE_TYPE>
inline Vec3<COORDINATE_TYPE> operator-(const Vec3<COORDINATE_TYPE>& a)
{
	return Vec3<COORDINATE_TYPE>{ -a.X, -a.Y, -a.Z };
}

template <typename COORDINATE_TYPE>
inline Vec3<COORDINATE_TYPE> operator*(const Vec3<COORDINATE_TYPE>& a, COORDINATE_TYPE scalar)
{
	return Vec3<COORDINATE_TYPE>{ a.X * scalar, a.Y * scalar, a.Z * scalar };
}

template <typename COORDINATE_TYPE>
inline Vec3<COORDINATE_TYPE> operator*(COORDINATE_TYPE scalar, const Vec3<COORDINATE_TYPE>& b)
{
	return Vec3{ scalar * b.X, scalar * b.Y, scalar * b.Z };
}

template <typename COORDINATE_TYPE>
COORDINATE_TYPE Dot(const Vec3<COORDINATE_TYPE>& a, const Vec3<COORDINATE_TYPE>& b);

template <typename COORDINATE_TYPE>
Vec3<COORDINATE_TYPE> Cross(const Vec3<COORDINATE_TYPE>& a, const Vec3<COORDINATE_TYPE>& b);

template <typename COORDINATE_TYPE>
inline Vec3<COORDINATE_TYPE> Mul(const Vec3<COORDINATE_TYPE>& a, const Vec3<COORDINATE_TYPE>& b)
{
	return Vec3<COORDINATE_TYPE>{ a.X * b.X, a.Y * b.Y, a.Z * b.Z };
}

template <typename COORDINATE_TYPE>
struct std::hash<Vec3<COORDINATE_TYPE>>
{
	std::size_t operator()(const Vec3<COORDINATE_TYPE>& v) const noexcept
	{
		const int shiftDown = (sizeof(std::size_t) * CHAR_BIT >> 2);
		const int shiftUp = (sizeof(std::size_t) * CHAR_BIT) - shiftDown;

		size_t hash = std::hash<decltype(v.X)>{}(v.X);
		hash = std::hash<decltype(v.Y)>{}(v.Y) ^ (hash >> shiftDown | hash << shiftUp);
		hash = std::hash<decltype(v.Z)>{}(v.Z) ^ (hash >> shiftDown | hash << shiftUp);
		return hash;
	}
};

template <typename COORDINATE_TYPE>
COORDINATE_TYPE ManhattanDistance(const Vec3<COORDINATE_TYPE>& a, const Vec3<COORDINATE_TYPE>& b);

extern template struct Vec3<int16_t>;
extern template struct Vec3<int32_t>;
extern template struct Vec3<int64_t>;

using Vec3Short = Vec3<int16_t>;
using Vec3Int = Vec3<int32_t>;
using Vec3Long = Vec3<int64_t>;
