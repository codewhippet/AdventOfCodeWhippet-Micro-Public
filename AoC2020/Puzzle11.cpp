#include "stdafx.h"

using namespace std;

namespace Puzzle11_2020_Types
{
	struct Neighbours
	{
		vector<array<int8_t, 8>> Data;
		int32_t Stride;

		Neighbours(int32_t width, int32_t height)
			: Data(width * height)
			, Stride(width)
		{
			memset(Data.data(), 0, Data.size() * sizeof(Data[0]));
		}

		void Set(const Vec2Int& pos, size_t index, int8_t neighbour)
		{
			Data[pos.Y * Stride + pos.X][index] = neighbour;
		}

		const array<int8_t, 8>& Get(const Vec2Int& pos) const
		{
			return Data[pos.Y * Stride + pos.X];
		};
	};
}

using namespace Puzzle11_2020_Types;

static Neighbours GenerateVisibleNeighboursNew(const uArrayMap2D& seating)
{
	Neighbours neighbours(seating.GetWidth(), seating.GetHeight());

	const size_t northIndex = distance(Vec2Int::CardinalAndDiagonalDirections().begin(), ranges::find(Vec2Int::CardinalAndDiagonalDirections(), Vec2Int::North()));
	const size_t eastIndex = distance(Vec2Int::CardinalAndDiagonalDirections().begin(), ranges::find(Vec2Int::CardinalAndDiagonalDirections(), Vec2Int::East()));
	const size_t southIndex = distance(Vec2Int::CardinalAndDiagonalDirections().begin(), ranges::find(Vec2Int::CardinalAndDiagonalDirections(), Vec2Int::South()));
	const size_t westIndex = distance(Vec2Int::CardinalAndDiagonalDirections().begin(), ranges::find(Vec2Int::CardinalAndDiagonalDirections(), Vec2Int::West()));

	const size_t northEastIndex = distance(Vec2Int::CardinalAndDiagonalDirections().begin(), ranges::find(Vec2Int::CardinalAndDiagonalDirections(), Vec2Int::NorthEast()));
	const size_t southEastIndex = distance(Vec2Int::CardinalAndDiagonalDirections().begin(), ranges::find(Vec2Int::CardinalAndDiagonalDirections(), Vec2Int::SouthEast()));
	const size_t northWestIndex = distance(Vec2Int::CardinalAndDiagonalDirections().begin(), ranges::find(Vec2Int::CardinalAndDiagonalDirections(), Vec2Int::NorthWest()));
	const size_t southWestIndex = distance(Vec2Int::CardinalAndDiagonalDirections().begin(), ranges::find(Vec2Int::CardinalAndDiagonalDirections(), Vec2Int::SouthWest()));

	// Left/right neighbours
	{
		for (int32_t y : seating.AxisRangeY())
		{
			optional<Vec2Int> lastSeat;
			for (int32_t x : seating.AxisRangeX())
			{
				if (seating(x, y) == 'L')
				{
					if (lastSeat)
					{
						int8_t diff = static_cast<int8_t>(abs(x - lastSeat->X));
						neighbours.Set(Vec2Int{ x, y }, westIndex, diff);
						neighbours.Set(*lastSeat, eastIndex, diff);
					}

					lastSeat = Vec2Int{ x, y };
				}
			}
		}
	}

	// Up/down neighbours
	{
		for (int32_t x : seating.AxisRangeX())
		{
			optional<Vec2Int> lastSeat;
			for (int32_t y : seating.AxisRangeY())
			{
				if (seating(x, y) == 'L')
				{
					if (lastSeat)
					{
						int8_t diff = static_cast<int8_t>(abs(y - lastSeat->Y));
						neighbours.Set(Vec2Int{ x, y }, northIndex, diff);
						neighbours.Set(*lastSeat, southIndex, diff);
					}

					lastSeat = Vec2Int{ x, y };
				}
			}
		}
	}

	// NW-SE Diagonal neighbours
	{
		int32_t width = seating.GetWidth();
		int32_t height = seating.GetHeight();

		for (int32_t w = -height; w < width; w++)
		{
			int32_t x = w;

			optional<Vec2Int> lastSeat;
			for (int32_t y = 0; y < height; y++, x++)
			{
				if (seating(x, y) == 'L')
				{
					if (lastSeat)
					{
						int8_t diff = static_cast<int8_t>(abs(x - lastSeat->X));
						assert(diff == abs(y - lastSeat->Y));
						neighbours.Set(Vec2Int{ x, y }, northWestIndex, diff);
						neighbours.Set(*lastSeat, southEastIndex, diff);
					}

					lastSeat = Vec2Int{ x, y };
				}
			}
		}
	}

	// NE-SW Diagonal neighbours
	{
		int32_t width = seating.GetWidth();
		int32_t height = seating.GetHeight();

		for (int32_t w = width + height; w > 0; w--)
		{
			int32_t x = w;

			optional<Vec2Int> lastSeat;
			for (int32_t y = 0; y < height; y++, x--)
			{
				if (seating(x, y) == 'L')
				{
					if (lastSeat)
					{
						int8_t diff = static_cast<int8_t>(abs(x - lastSeat->X));
						assert(diff == abs(y - lastSeat->Y));
						neighbours.Set(Vec2Int{ x, y }, northEastIndex, diff);
						neighbours.Set(*lastSeat, southWestIndex, diff);
					}

					lastSeat = Vec2Int{ x, y };
				}
			}
		}
	}

	return neighbours;
}

void Puzzle11_A_2020()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 20 * 1024;

	MemArena_Configure(cfg);
	{
		vector<uArrayMap2D> seating;
		seating.reserve(2);
		seating.push_back(ReaduArrayMap());
		seating.push_back(uArrayMap2D(uArrayMap2DOptions::CloneAsUninitialised, seating[0]));

		int32_t answer = 0;
		for (size_t i = 0; i < numeric_limits<size_t>::max(); i++)
		{
			const uArrayMap2D& current = seating[i & 1];
			uArrayMap2D& next = seating[1 - (i & 1)];

			size_t swaps = 0;
			for (const auto& seat : current.Grid())
			{
				auto [currentPos, currentTile] = seat;
				if (currentTile == '.')
				{
					continue;
				}

				int occupiedCount = 0;
				for (const Vec2Int& offset : Vec2Int::CardinalAndDiagonalDirections())
				{
					if (current(currentPos + offset) == '#')
					{
						occupiedCount++;
					}
				}

				if (currentTile == 'L' && occupiedCount == 0)
				{
					next(currentPos) = '#';
					swaps++;
				}
				else if (currentTile == '#' && occupiedCount >= 4)
				{
					next(currentPos) = 'L';
					swaps++;
				}
				else
				{
					next(currentPos) = currentTile;
				}
			}

			if (swaps == 0)
			{
				answer = current.Count('#');
				break;
			}
		}

		PuzzleOutput::Submit(2020, 11, 1, answer);
	}
	MemArena_Reset();
}

void Puzzle11_B_2020()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 20 * 1024;

	MemArena_Configure(cfg);
	{
		vector<uArrayMap2D> seating;
		seating.reserve(2);
		seating.push_back(ReaduArrayMap());
		seating.push_back(uArrayMap2D(uArrayMap2DOptions::CloneAsUninitialised, seating[0]));

		Neighbours visibleNeighbours = GenerateVisibleNeighboursNew(seating[0]);

		int32_t answer = 0;
		for (size_t i = 0; i < numeric_limits<size_t>::max(); i++)
		{
			const uArrayMap2D& current = seating[i & 1];
			uArrayMap2D& next = seating[1 - (i & 1)];

			size_t swaps = 0;
			for (const auto& seat : current.Grid())
			{
				auto [currentPos, currentTile] = seat;
				if (currentTile == '.')
				{
					continue;
				}

				size_t occupiedCount = 0;

				const array<int8_t, 8>& neighbours = visibleNeighbours.Get(currentPos);
				for (size_t neighbourIndex = 0; neighbourIndex < 8; neighbourIndex++)
				{
					if (neighbours[neighbourIndex] == 0)
						continue;

					Vec2Int neighbour = currentPos + neighbours[neighbourIndex] * Vec2Int::CardinalAndDiagonalDirections()[neighbourIndex];
					if (current(neighbour) == '#')
					{
						occupiedCount++;
					}
				}

				if (currentTile == 'L' && occupiedCount == 0)
				{
					next(currentPos) = '#';
					swaps++;
				}
				else if (currentTile == '#' && occupiedCount >= 5)
				{
					next(currentPos) = 'L';
					swaps++;
				}
				else
				{
					next(currentPos) = currentTile;
				}
			}

			if (swaps == 0)
			{
				answer = current.Count('#');
				break;
			}
		}

		PuzzleOutput::Submit(2020, 11, 2, answer);
	}
	MemArena_Reset();
}
