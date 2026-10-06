#include "stdafx.h"

using namespace std;

namespace Puzzle24_2020_Types
{
	using StateTransform = pair<int32_t, Vec3Int (*)(const Vec3Int&)>;

	struct HexField
	{
		HexField(const Vec2Int& origin, int32_t width, int32_t height)
		{
			Layers.push_back({ CreateArrayMap2DAllocator_Heap(), origin, width, height, 0 });
			Layers.push_back({ CreateArrayMap2DAllocator_Heap(), origin, width, height, 0 });
		}

		void Set(const Vec3Int& pos, char v)
		{
			assert((pos.X == 0) || (pos.X == 1));
			uArrayMap2D& layer = Layers[pos.X];
			assert(layer.IsInside({ pos.Y, pos.Z }));
			layer(pos.Y, pos.Z) = v;
		}

		char Get(const Vec3Int& pos) const
		{
			assert((pos.X == 0) || (pos.X == 1));
			const uArrayMap2D& layer = Layers[pos.X];
			assert(layer.IsInside({ pos.Y, pos.Z }));
			return layer(pos.Y, pos.Z);
		}

		void Inc(const Vec3Int& pos)
		{
			assert((pos.X == 0) || (pos.X == 1));
			uArrayMap2D& layer = Layers[pos.X];
			assert(layer.IsInside({ pos.Y, pos.Z }));
			layer(pos.Y, pos.Z)++;
		}

		int32_t Count(char v) const
		{
			return Layers[0].Count(v) + Layers[1].Count(v);
		}

		void Reset()
		{
			ranges::fill(Layers[0].GetData(), '\0');
			ranges::fill(Layers[1].GetData(), '\0');
		}

		void ForEach(const function<void(const Vec3Int&, char)>& func) const
		{
			for (int32_t i = 0; i < static_cast<int32_t>(Layers.size()); i++)
			{
				for (const auto& p : Layers[i].Grid())
				{
					func({ i, p.first.X, p.first.Y }, p.second);
				}
			}
		}

		vector<uArrayMap2D> Layers;
	};
}

using namespace Puzzle24_2020_Types;

static Vec3Int Noop(const Vec3Int& v)
{
	return v;
}

static Vec3Int East(const Vec3Int& v)
{
	return { v.X, v.Y, v.Z + 1 };
}

static Vec3Int SouthEast(const Vec3Int& v)
{
	return { 1 - v.X, v.Y + v.X, v.Z + v.X };
}

static Vec3Int SouthWest(const Vec3Int& v)
{
	return { 1 - v.X, v.Y + v.X, v.Z - (1 - v.X) };
}

static Vec3Int West(const Vec3Int& v)
{
	return { v.X, v.Y, v.Z - 1 };
}

static Vec3Int NorthWest(const Vec3Int& v)
{
	return { 1 - v.X, v.Y - (1 - v.X), v.Z - (1 - v.X) };
}

static Vec3Int NorthEast(const Vec3Int& v)
{
	return { 1 - v.X, v.Y - (1 - v.X), v.Z + v.X };
}

void Puzzle24_A_2020()
{
	const int32_t size = 100;
	const Vec2Int origin{ -(size / 2), -(size / 2) };

	array<array<StateTransform, 26>, 3> stateMachine{};
	
	stateMachine[0]['e' - 'a'] = { 0, &East };
	stateMachine[0]['w' - 'a'] = { 0, &West };

	stateMachine[0]['s' - 'a'] = { 1, &Noop };
	stateMachine[1]['e' - 'a'] = { 0, &SouthEast };
	stateMachine[1]['w' - 'a'] = { 0, &SouthWest };

	stateMachine[0]['n' - 'a'] = { 2, &Noop };
	stateMachine[2]['e' - 'a'] = { 0, &NorthEast };
	stateMachine[2]['w' - 'a'] = { 0, &NorthWest };

	HexField field(origin, size, size);
	while (PuzzleInput::NextLine())
	{
		int32_t currentState = 0;
		Vec3Int currentPos{ 0, 0, 0 };

		for (char c : Parse::ReadUntilSeen('\n'))
		{
			const StateTransform& transform = stateMachine[currentState][c - 'a'];
			currentPos = transform.second(currentPos);
			currentState = transform.first;
		}

		field.Set(currentPos, 1 - field.Get(currentPos));
	}

	int32_t answer = field.Count(1);

	PuzzleOutput::Submit(2020, 24, 1, answer);
}

void Puzzle24_B_2020()
{
	const int32_t size = 120;
	const Vec2Int origin{ -(size / 2), -(size / 2) };
	const size_t rounds = 100;

	array<array<StateTransform, 26>, 3> stateMachine{};
	
	stateMachine[0]['e' - 'a'] = { 0, &East };
	stateMachine[0]['w' - 'a'] = { 0, &West };

	stateMachine[0]['s' - 'a'] = { 1, &Noop };
	stateMachine[1]['e' - 'a'] = { 0, &SouthEast };
	stateMachine[1]['w' - 'a'] = { 0, &SouthWest };

	stateMachine[0]['n' - 'a'] = { 2, &Noop };
	stateMachine[2]['e' - 'a'] = { 0, &NorthEast };
	stateMachine[2]['w' - 'a'] = { 0, &NorthWest };

	vector<HexField> fields;
	fields.reserve(2);
	fields.push_back({ origin, size, size });
	fields.push_back({ origin, size, size });

	while (PuzzleInput::NextLine())
	{
		int32_t currentState = 0;
		Vec3Int currentPos{ 0, 0, 0 };

		for (char c : Parse::ReadUntilSeen('\n'))
		{
			const StateTransform& transform = stateMachine[currentState][c - 'a'];
			currentPos = transform.second(currentPos);
			currentState = transform.first;
		}

		fields[0].Set(currentPos, 1 - fields[0].Get(currentPos));
	}

	const array<Vec3Int(*)(const Vec3Int&), 6> neighbourTransforms = { &East, &SouthEast, &SouthWest, &West, &NorthWest, &NorthEast };

	HexField whiteTileAdjacencyCounts(origin, size, size);
	for (size_t i = 0; i < rounds; i++)
	{
		const HexField& current = fields[i & 1];
		HexField& next = fields[1 - (i & 1)];
		next.Reset();

		whiteTileAdjacencyCounts.Reset();
		current.ForEach([&](const Vec3Int& pos, char c)
			{
				if (c == 0)
					return;

				int32_t blackNeighbours = 0;
				for (Vec3Int(*neighbourTransform)(const Vec3Int&) : neighbourTransforms)
				{
					Vec3Int neighbour = neighbourTransform(pos);
					if (current.Get(neighbour))
					{
						blackNeighbours++;
					}
					else
					{
						whiteTileAdjacencyCounts.Inc(neighbour);
					}
				}

				if ((blackNeighbours == 1) || (blackNeighbours == 2))
				{
					next.Set(pos, 1);
				}
			});

		whiteTileAdjacencyCounts.ForEach([&](const Vec3Int& pos, char c)
			{
				if (c == 2)
				{
					next.Set(pos, 1);
				}
			});
	}

	int32_t answer = fields[rounds & 1].Count(1);

	PuzzleOutput::Submit(2020, 24, 2, answer);
}
