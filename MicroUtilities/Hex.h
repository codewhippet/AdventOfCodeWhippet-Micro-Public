#pragma once
#include <functional>
#include <vector>
#include <stdint.h>

// With reference to https://www.redblobgames.com/grids/hexagons/
// Cube co-ordinate system for both Flat and Pointy varieties

namespace HexFlat
{
	template <typename COORDINATE_TYPE>
	struct Hex
	{
		COORDINATE_TYPE Q = 0;
		COORDINATE_TYPE R = 0;
		COORDINATE_TYPE S = 0;

		static constexpr Hex North() { return Hex{ 0, -1, 1 }; }
		static constexpr Hex NorthEast() { return Hex{ 1, -1, 0 }; }
		static constexpr Hex SouthEast() { return Hex{ 1, 0, -1 }; }
		static constexpr Hex South() { return Hex{ 0, 1, -1 }; }
		static constexpr Hex SouthWest() { return Hex{ -1, 1, 0 }; }
		static constexpr Hex NorthWest() { return Hex{ -1, 0, 1 }; }

		static const std::array<Hex, 6>& NeighbourDirections();

		auto operator<=>(const Hex&) const = default;
	};

	template <typename COORDINATE_TYPE>
	inline Hex<COORDINATE_TYPE> operator+(const Hex<COORDINATE_TYPE>& a, const Hex<COORDINATE_TYPE>& b)
	{
		return Hex<COORDINATE_TYPE>{ a.Q + b.Q, a.R + b.R, a.S + b.S };
	}

	template <typename COORDINATE_TYPE>
	inline Hex<COORDINATE_TYPE> operator-(const Hex<COORDINATE_TYPE>& a, const Hex<COORDINATE_TYPE>& b)
	{
		return Hex<COORDINATE_TYPE>{ a.Q - b.Q, a.R - b.R, a.S - b.S };
	}

	template <typename COORDINATE_TYPE>
	inline Hex<COORDINATE_TYPE> operator-(const Hex<COORDINATE_TYPE>& a)
	{
		return Hex<COORDINATE_TYPE>{ -a.Q, -a.R, -a.S };
	}

	template <typename COORDINATE_TYPE>
	inline COORDINATE_TYPE MahattanDistance(const Hex<COORDINATE_TYPE>& a, const Hex<COORDINATE_TYPE>& b)
	{
		Hex<COORDINATE_TYPE> h = a - b;
		return (std::abs(h.Q) + std::abs(h.R) + std::abs(h.S)) / 2;
	}

	extern template struct Hex<int32_t>;

	using HexInt = Hex<int32_t>;
}

namespace HexPointy
{
	template <typename COORDINATE_TYPE>
	struct Hex
	{
		int64_t Q = 0;
		int64_t R = 0;
		int64_t S = 0;

		static constexpr Hex NorthEast() { return Hex{ 1, -1, 0 }; }
		static constexpr Hex East() { return Hex{ 1, 0, -1 }; }
		static constexpr Hex SouthEast() { return Hex{ 0, 1, -1 }; }
		static constexpr Hex SouthWest() { return Hex{ -1, 1, 0 }; }
		static constexpr Hex West() { return Hex{ -1, 0, 1 }; }
		static constexpr Hex NorthWest() { return Hex{ 0, -1, 1 }; }

		static const std::array<Hex, 6>& NeighbourDirections();

		auto operator<=>(const Hex&) const = default;
	};

	template <typename COORDINATE_TYPE>
	inline Hex<COORDINATE_TYPE> operator+(const Hex<COORDINATE_TYPE>& a, const Hex<COORDINATE_TYPE>& b)
	{
		return Hex{ a.Q + b.Q, a.R + b.R, a.S + b.S };
	}

	template <typename COORDINATE_TYPE>
	inline Hex<COORDINATE_TYPE> operator-(const Hex<COORDINATE_TYPE>& a, const Hex<COORDINATE_TYPE>& b)
	{
		return Hex{ a.Q - b.Q, a.R - b.R, a.S - b.S };
	}

	template <typename COORDINATE_TYPE>
	inline Hex<COORDINATE_TYPE> operator-(const Hex<COORDINATE_TYPE>& a)
	{
		return Hex{ -a.Q, -a.R, -a.S };
	}

	template <typename COORDINATE_TYPE>
	inline COORDINATE_TYPE MahattanDistance(const Hex<COORDINATE_TYPE>& a, const Hex<COORDINATE_TYPE>& b)
	{
		Hex h = a - b;
		return (std::abs(h.Q) + std::abs(h.R) + std::abs(h.S)) / 2;
	}

	extern template struct Hex<int32_t>;

	using HexInt = Hex<int32_t>;
}

template <>
struct std::hash<HexFlat::Hex<int32_t>>
{
	std::size_t operator()(const HexFlat::Hex<int32_t>& h) const noexcept
	{
		const int shift = (sizeof(std::size_t) * CHAR_BIT >> 2);

		size_t hash = std::hash<decltype(h.Q)>{}(h.Q);
		hash = std::hash<decltype(h.R)>{}(h.R) ^ (hash >> shift | hash << shift);
		hash = std::hash<decltype(h.S)>{}(h.S) ^ (hash >> shift | hash << shift);
		return hash;
	}
};

