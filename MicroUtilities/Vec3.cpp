#include "stdafx.h"
#include "Vec3.h"

template<typename COORDINATE_TYPE>
const std::array<Vec3<COORDINATE_TYPE>, 6>& Vec3<COORDINATE_TYPE>::CardinalDirections()
{
    static const std::array<Vec3<COORDINATE_TYPE>, 6> directions =
    {
        Vec3<COORDINATE_TYPE>{ COORDINATE_TYPE( 1), COORDINATE_TYPE( 0), COORDINATE_TYPE( 0) },
        Vec3<COORDINATE_TYPE>{ COORDINATE_TYPE(-1), COORDINATE_TYPE( 0), COORDINATE_TYPE( 0) },
        Vec3<COORDINATE_TYPE>{ COORDINATE_TYPE( 0), COORDINATE_TYPE( 1), COORDINATE_TYPE( 0) },
        Vec3<COORDINATE_TYPE>{ COORDINATE_TYPE( 0), COORDINATE_TYPE(-1), COORDINATE_TYPE( 0) },
        Vec3<COORDINATE_TYPE>{ COORDINATE_TYPE( 0), COORDINATE_TYPE( 0), COORDINATE_TYPE( 1) },
        Vec3<COORDINATE_TYPE>{ COORDINATE_TYPE( 0), COORDINATE_TYPE( 0), COORDINATE_TYPE(-1) },
    };
    return directions;
}

template<typename COORDINATE_TYPE>
Vec3<COORDINATE_TYPE> Vec3<COORDINATE_TYPE>::MinElements(const Vec3<COORDINATE_TYPE>& a, const Vec3<COORDINATE_TYPE>& b)
{
    return { std::min(a.X, b.X), std::min(a.Y, b.Y), std::min(a.Z, b.Z) };
}

template<typename COORDINATE_TYPE>
Vec3<COORDINATE_TYPE> Vec3<COORDINATE_TYPE>::MaxElements(const Vec3<COORDINATE_TYPE>& a, const Vec3<COORDINATE_TYPE>& b)
{
    return { std::max(a.X, b.X), std::max(a.Y, b.Y), std::max(a.Z, b.Z) };
}

template <typename COORDINATE_TYPE>
COORDINATE_TYPE Dot(const Vec3<COORDINATE_TYPE>& a, const Vec3<COORDINATE_TYPE>& b)
{
    return (a.X * b.X) + (a.Y * b.Y) + (a.Z * b.Z);
}

template <typename COORDINATE_TYPE>
Vec3<COORDINATE_TYPE> Cross(const Vec3<COORDINATE_TYPE>& a, const Vec3<COORDINATE_TYPE>& b)
{
    return Vec3<COORDINATE_TYPE>{
        (a.Y* b.Z) - (a.Z * b.Y),
        (a.Z* b.X) - (a.X * b.Z),
        (a.X* b.Y) - (a.Y * b.X)
    };
}

template <typename COORDINATE_TYPE>
COORDINATE_TYPE ManhattanDistance(const Vec3<COORDINATE_TYPE>& a, const Vec3<COORDINATE_TYPE>& b)
{
    return COORDINATE_TYPE(std::abs(b.X - a.X)) + COORDINATE_TYPE(std::abs(b.Y - a.Y)) + COORDINATE_TYPE(std::abs(b.Z - a.Z));
}

template int16_t Dot<int16_t>(const Vec3<int16_t>& a, const Vec3<int16_t>& b);
template int32_t Dot<int32_t>(const Vec3<int32_t>& a, const Vec3<int32_t>& b);
template int64_t Dot<int64_t>(const Vec3<int64_t>& a, const Vec3<int64_t>& b);

template int16_t ManhattanDistance<int16_t>(const Vec3<int16_t>& a, const Vec3<int16_t>& b);
template int32_t ManhattanDistance<int32_t>(const Vec3<int32_t>& a, const Vec3<int32_t>& b);
template int64_t ManhattanDistance<int64_t>(const Vec3<int64_t>& a, const Vec3<int64_t>& b);

template struct Vec3<int16_t>;
template struct Vec3<int32_t>;
template struct Vec3<int64_t>;
