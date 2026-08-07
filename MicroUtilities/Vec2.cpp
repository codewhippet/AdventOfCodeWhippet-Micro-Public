#include "stdafx.h"
#include "Vec2.h"

template<typename COORDINATE_TYPE>
const std::array<Vec2<COORDINATE_TYPE>, 4>& Vec2<COORDINATE_TYPE>::CardinalDirections()
{
    static const std::array<Vec2<COORDINATE_TYPE>, 4> directions =
    {
        Vec2<COORDINATE_TYPE>::North(),
        Vec2<COORDINATE_TYPE>::East(),
        Vec2<COORDINATE_TYPE>::South(),
        Vec2<COORDINATE_TYPE>::West()
    };
    return directions;
}

template<typename COORDINATE_TYPE>
const std::array<Vec2<COORDINATE_TYPE>, 8>& Vec2<COORDINATE_TYPE>::CardinalAndDiagonalDirections()
{
    static const std::array<Vec2<COORDINATE_TYPE>, 8> directions =
    {
        Vec2<COORDINATE_TYPE>::North(),
        Vec2<COORDINATE_TYPE>::North() + Vec2<COORDINATE_TYPE>::East(),
        Vec2<COORDINATE_TYPE>::East(),
        Vec2<COORDINATE_TYPE>::South() + Vec2<COORDINATE_TYPE>::East(),
        Vec2<COORDINATE_TYPE>::South(),
        Vec2<COORDINATE_TYPE>::South() + Vec2<COORDINATE_TYPE>::West(),
        Vec2<COORDINATE_TYPE>::West(),
        Vec2<COORDINATE_TYPE>::North() + Vec2<COORDINATE_TYPE>::West()
    };
    return directions;
}

template<typename COORDINATE_TYPE>
Vec2<COORDINATE_TYPE> Vec2<COORDINATE_TYPE>::DirectionFromChar(char d)
{
    switch (d)
    {
    case '^': return Vec2<COORDINATE_TYPE>::North();
    case '>': return Vec2<COORDINATE_TYPE>::East();
    case 'v': return Vec2<COORDINATE_TYPE>::South();
    case '<': return Vec2<COORDINATE_TYPE>::West();
    }
    return {};
}

template<typename COORDINATE_TYPE>
Vec2<COORDINATE_TYPE> Vec2<COORDINATE_TYPE>::RotateClockwise(const Vec2<COORDINATE_TYPE>& direction)
{
    return { COORDINATE_TYPE(-direction.Y), direction.X };
}

template<typename COORDINATE_TYPE>
Vec2<COORDINATE_TYPE> Vec2<COORDINATE_TYPE>::RotateAnticlockwise(const Vec2<COORDINATE_TYPE>& direction)
{
    return { direction.Y, COORDINATE_TYPE(-direction.X) };
}

template<typename COORDINATE_TYPE>
Vec2<COORDINATE_TYPE> Vec2<COORDINATE_TYPE>::MinElements(const Vec2<COORDINATE_TYPE>& a, const Vec2<COORDINATE_TYPE>& b)
{
    return { std::min(a.X, b.X), std::min(a.Y, b.Y) };
}

template<typename COORDINATE_TYPE>
Vec2<COORDINATE_TYPE> Vec2<COORDINATE_TYPE>::MaxElements(const Vec2<COORDINATE_TYPE>& a, const Vec2<COORDINATE_TYPE>& b)
{
    return { std::max(a.X, b.X), std::max(a.Y, b.Y) };
}

template <typename COORDINATE_TYPE>
COORDINATE_TYPE ManhattanDistance(const Vec2<COORDINATE_TYPE>& a, const Vec2<COORDINATE_TYPE>& b)
{
    return COORDINATE_TYPE(std::abs(b.X - a.X)) + COORDINATE_TYPE(std::abs(b.Y - a.Y));
}

template int16_t ManhattanDistance<int16_t>(const Vec2<int16_t>& a, const Vec2<int16_t>& b);
template int32_t ManhattanDistance<int32_t>(const Vec2<int32_t>& a, const Vec2<int32_t>& b);
template int64_t ManhattanDistance<int64_t>(const Vec2<int64_t>& a, const Vec2<int64_t>& b);

template struct Vec2<int8_t>;
template struct Vec2<int16_t>;
template struct Vec2<int32_t>;
template struct Vec2<int64_t>;
