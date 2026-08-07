#include "stdafx.h"
#include "Vec4.h"

template<typename COORDINATE_TYPE>
Vec4<COORDINATE_TYPE> Vec4<COORDINATE_TYPE>::MinElements(const Vec4<COORDINATE_TYPE>& a, const Vec4<COORDINATE_TYPE>& b)
{
    return { std::min(a.X, b.X), std::min(a.Y, b.Y), std::min(a.Z, b.Z), std::min(a.W, b.W) };
}

template<typename COORDINATE_TYPE>
Vec4<COORDINATE_TYPE> Vec4<COORDINATE_TYPE>::MaxElements(const Vec4<COORDINATE_TYPE>& a, const Vec4<COORDINATE_TYPE>& b)
{
    return { std::max(a.X, b.X), std::max(a.Y, b.Y), std::max(a.Z, b.Z), std::max(a.W, b.W) };
}

template <typename COORDINATE_TYPE>
COORDINATE_TYPE ManhattanDistance(const Vec4<COORDINATE_TYPE>& a, const Vec4<COORDINATE_TYPE>& b)
{
    return COORDINATE_TYPE(std::abs(b.X - a.X)) + COORDINATE_TYPE(std::abs(b.Y - a.Y)) + COORDINATE_TYPE(std::abs(b.Z - a.Z)) + COORDINATE_TYPE(std::abs(b.W - a.W));
}

template int16_t ManhattanDistance<int16_t>(const Vec4<int16_t>& a, const Vec4<int16_t>& b);
template int32_t ManhattanDistance<int32_t>(const Vec4<int32_t>& a, const Vec4<int32_t>& b);
template int64_t ManhattanDistance<int64_t>(const Vec4<int64_t>& a, const Vec4<int64_t>& b);

template struct Vec4<int16_t>;
template struct Vec4<int32_t>;
template struct Vec4<int64_t>;
