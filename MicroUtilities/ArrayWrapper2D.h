#pragma once

template<typename WRAPPED_TYPE>
struct ArrayWrapper2D
{
    ArrayWrapper2D(WRAPPED_TYPE* data, int32_t width, int32_t height)
        : Data(data)
        , Width(width)
        , Height(height)
        , WasConst(false)
    {
    }

    ArrayWrapper2D(const WRAPPED_TYPE* data, int32_t width, int32_t height)
        : Data(const_cast<WRAPPED_TYPE*>(data))
        , Width(width)
        , Height(height)
        , WasConst(true)
    {
    }

    WRAPPED_TYPE& operator()(Vec2Int p)
    {
        assert(p.X >= 0 && p.X < Width);
        assert(p.Y >= 0 && p.Y < Height);
        assert(WasConst == false);
        return Data[p.Y * Width + p.X];
    }

    WRAPPED_TYPE& operator()(int32_t x, int32_t y)
    {
        assert(x >= 0 && x < Width);
        assert(y >= 0 && y < Height);
        assert(WasConst == false);
        return Data[y * Width + x];
    }

    const WRAPPED_TYPE& operator()(Vec2Int p) const
    {
        assert(p.X >= 0 && p.X < Width);
        assert(p.Y >= 0 && p.Y < Height);
        return Data[p.Y * Width + p.X];
    }

    const WRAPPED_TYPE& operator()(int32_t x, int32_t y) const
    {
        assert(x >= 0 && x < Width);
        assert(y >= 0 && y < Height);
        return Data[y * Width + x];
    }

    WRAPPED_TYPE* Data;
    int32_t Width;
    int32_t Height;
    bool WasConst = false;
};
