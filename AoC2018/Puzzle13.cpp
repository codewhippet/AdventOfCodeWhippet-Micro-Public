#include "stdafx.h"

using namespace std;

namespace Puzzle13_2018_Types
{
	struct Cart
	{
		size_t Id;
		Vec2Int Dir;
		size_t TurnIndex;
	};

	struct CartSorter
	{
		bool operator()(const Vec2Int& a, const Vec2Int& b) const
		{
			return (a.Y == b.Y) ? a.X < b.X : a.Y < b.Y;
		}
	};

	using CartSet = umap<Vec2Int, Cart, CartSorter>;
}

using namespace Puzzle13_2018_Types;

static CartSet ExtractCarts(uArrayMap2D* tracks)
{
	const umap<char, Vec2Int> cartDirections =
	{
		{ '^', Vec2Int::Up() },
		{ 'v', Vec2Int::Down() },
		{ '<', Vec2Int::Left() },
		{ '>', Vec2Int::Right() },
	};

	const umap<char, char> cartRepairs =
	{
		{ '^', '|' },
		{ 'v', '|' },
		{ '<', '-' },
		{ '>', '-' },
	};

	CartSet carts;
	for (const pair<Vec2Int, char> p : tracks->Grid())
	{
		if (cartDirections.contains(p.second))
		{
			carts.insert({ p.first, { carts.size(), cartDirections.at(p.second), 0 } });
			(*tracks)(p.first) = cartRepairs.at(p.second);
		}
	}
	return carts;
}

static Vec2Int FindFirstCrashLocation(const uArrayMap2D& tracks, CartSet *carts)
{
	const umap<pair<char, Vec2Int>, Vec2Int> cornerTurns =
	{
		{ { '/', Vec2Int::Up() }, Vec2Int::Right() },
		{ { '/', Vec2Int::Down() }, Vec2Int::Left() },
		{ { '/', Vec2Int::Left() }, Vec2Int::Down() },
		{ { '/', Vec2Int::Right() }, Vec2Int::Up() },

		{ { '\\', Vec2Int::Up() }, Vec2Int::Left() },
		{ { '\\', Vec2Int::Down() }, Vec2Int::Right() },
		{ { '\\', Vec2Int::Left() }, Vec2Int::Up() },
		{ { '\\', Vec2Int::Right() }, Vec2Int::Down() },
	};

	const uvector<function<Vec2Int(const Vec2Int& d)>> intersectionTurns =
	{
		[](const Vec2Int& d) { return Vec2Int::RotateAnticlockwise(d); },
		[](const Vec2Int& d) { return d; },
		[](const Vec2Int& d) { return Vec2Int::RotateClockwise(d); },
	};

	while (true)
	{
		CartSet newCarts;
		while (carts->empty() == false)
		{
			CartSet::value_type cart = *carts->begin();
			carts->erase(carts->begin());

			Vec2Int newCartPos = cart.first + cart.second.Dir;
			if (newCarts.contains(newCartPos) || carts->contains(newCartPos))
			{
				return newCartPos;
			}

			char trackPiece = tracks(newCartPos);
			switch (trackPiece)
			{
			case '/':
			case '\\':
				cart.second.Dir = cornerTurns.at({ trackPiece, cart.second.Dir });
				break;

			case '+':
				cart.second.Dir = intersectionTurns.at(cart.second.TurnIndex)(cart.second.Dir);
				cart.second.TurnIndex = (cart.second.TurnIndex + 1) % intersectionTurns.size();
				break;
			};

			newCarts.insert({ newCartPos, cart.second });
		}

		carts->swap(newCarts);
	}

	return Vec2Int{ -1, -1 };
}

static Vec2Int FindLastCartLocation(const uArrayMap2D& tracks, CartSet* carts)
{
	const umap<pair<char, Vec2Int>, Vec2Int> cornerTurns =
	{
		{ { '/', Vec2Int::Up() }, Vec2Int::Right() },
		{ { '/', Vec2Int::Down() }, Vec2Int::Left() },
		{ { '/', Vec2Int::Left() }, Vec2Int::Down() },
		{ { '/', Vec2Int::Right() }, Vec2Int::Up() },

		{ { '\\', Vec2Int::Up() }, Vec2Int::Left() },
		{ { '\\', Vec2Int::Down() }, Vec2Int::Right() },
		{ { '\\', Vec2Int::Left() }, Vec2Int::Up() },
		{ { '\\', Vec2Int::Right() }, Vec2Int::Down() },
	};

	const uvector<function<Vec2Int(const Vec2Int& d)>> intersectionTurns =
	{
		[](const Vec2Int& d) { return Vec2Int::RotateAnticlockwise(d); },
		[](const Vec2Int& d) { return d; },
		[](const Vec2Int& d) { return Vec2Int::RotateClockwise(d); },
	};

	while (carts->size() != 1)
	{
		CartSet newCarts;
		while (carts->empty() == false)
		{
			CartSet::value_type cart = *carts->begin();
			carts->erase(carts->begin());

			Vec2Int newCartPos = cart.first + cart.second.Dir;
			if (newCarts.contains(newCartPos))
			{
				newCarts.erase(newCartPos);
				continue;
			}
			if (carts->contains(newCartPos))
			{
				carts->erase(newCartPos);
				continue;
			}

			char trackPiece = tracks(newCartPos);
			switch (trackPiece)
			{
			case '/':
			case '\\':
				cart.second.Dir = cornerTurns.at({ trackPiece, cart.second.Dir });
				break;

			case '+':
				cart.second.Dir = intersectionTurns.at(cart.second.TurnIndex)(cart.second.Dir);
				cart.second.TurnIndex = (cart.second.TurnIndex + 1) % intersectionTurns.size();
				break;
			};

			newCarts.insert({ newCartPos, cart.second });
		}

		if (newCarts.size() == 1)
		{
			return newCarts.begin()->first;
		}

		carts->swap(newCarts);
	}

	return Vec2Int{ -1, -1 };
}

void Puzzle13_A_2018()
{
	const MemArenaSmallBlockCount sbas[] =
	{
		{ 32, 64 },
	};

	MemArenaConfig cfg;
	cfg.SmallBlockCounts = sbas;
	cfg.NumSmallBlockCounts = sizeof(sbas) / sizeof(sbas[0]);
	cfg.LargeBlockRegionSize = 32 * 1024;

	MemArena_Configure(cfg);
	{
		uArrayMap2D tracks = ReaduArrayMap(' ');
		CartSet carts = ExtractCarts(&tracks);

		Vec2Int answer = FindFirstCrashLocation(tracks, &carts);

		char answerText[32];
		sprintf(answerText, "%d,%d", answer.X, answer.Y);
		PuzzleOutput::Submit(2018, 13, 1, answerText);
	}
	MemArena_Reset();
}

void Puzzle13_B_2018()
{
	const MemArenaSmallBlockCount sbas[] =
	{
		{ 32, 64 },
	};

	MemArenaConfig cfg;
	cfg.SmallBlockCounts = sbas;
	cfg.NumSmallBlockCounts = sizeof(sbas) / sizeof(sbas[0]);
	cfg.LargeBlockRegionSize = 32 * 1024;

	MemArena_Configure(cfg);
	{
		uArrayMap2D tracks = ReaduArrayMap(' ');
		CartSet carts = ExtractCarts(&tracks);

		Vec2Int answer = FindLastCartLocation(tracks, &carts);

		char answerText[32];
		sprintf(answerText, "%d,%d", answer.X, answer.Y);
		PuzzleOutput::Submit(2018, 13, 2, answerText);
	}
	MemArena_Reset();
}
