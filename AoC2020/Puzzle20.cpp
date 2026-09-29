#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle20_2020_Types
{
	enum EdgeName : uint32_t
	{
		Top = 0,
		Bottom = 1,
		Left = 2,
		Right = 3
	};

	using Edges = array<uint32_t, 4>;

	struct Tile
	{
		uint32_t Id;
		array<Edges, 8> EdgesByTransform;

		vector<char> Pixels;
	};

	struct SearchPattern
	{
		int32_t Width;
		int32_t Height;
		vector<Vec2Int> Points;
	};
}

using namespace Puzzle20_2020_Types;

static uint32_t BitReverse10(uint32_t a)
{
	uint32_t b = 0;
	for (int32_t i = 0; i < 10; i++)
	{
		b = (b << 1) | (a & 1);
		a >>= 1;
	}
	return b;
}

static uint32_t ReadTileIdAndEdges(SmallVector<uint32_t, 8>* edges)
{
	Parse::DiscardExpected("Tile ");
	uint32_t id = Parse::GetUint32();
	PuzzleInput::DropLine();
	PuzzleInput::NextLine();

	uint32_t left = 0;
	uint32_t right = 0;

	array<char, 11> line;
	for (int32_t y = 0; y < 10; y++)
	{
		Parse::ReadNonEmptyLine(line.data(), line.size());
		if ((y == 0) || (y == 9))
		{
			uint32_t edge = 0;
			for (int32_t x = 0; x < 10; x++)
			{
				edge = (edge << 1) | (line[x] == '#' ? 1 : 0);
			}
			edges->PushBack(edge);
			edges->PushBack(BitReverse10(edge));
		}

		left = (left << 1) | (line[0] == '#' ? 1 : 0);
		right = (right << 1) | (line[9] == '#' ? 1 : 0);
	}

	edges->PushBack(left);
	edges->PushBack(BitReverse10(left));

	edges->PushBack(right);
	edges->PushBack(BitReverse10(right));

	Parse::DiscardExpected("\n");

	return id;
}

static void ExtractEdges(const vector<char>& sourceData, Edges* edges)
{
	const ArrayWrapper2D<char> source(sourceData.data(), 10, 10);

	uint32_t left = 0;
	uint32_t right = 0;
	SmallVector<uint32_t, 2> topBottom;

	for (int32_t y = 0; y < 10; y++)
	{
		if ((y == 0) || (y == 9))
		{
			uint32_t edge = 0;
			for (int32_t x = 0; x < 10; x++)
			{
				edge = (edge << 1) | (source(x, y) == '#' ? 1 : 0);
			}
			topBottom.PushBack(edge);
		}

		left = (left << 1) | (source(0, y) == '#' ? 1 : 0);
		right = (right << 1) | (source(9, y) == '#' ? 1 : 0);
	}

	(*edges)[EdgeName::Top] = topBottom[0];
	(*edges)[EdgeName::Bottom] = topBottom[1];
	(*edges)[EdgeName::Left] = left;
	(*edges)[EdgeName::Right] = right;
}

static void ExtractPixels(const vector<char>& sourceData, vector<char>* pixels)
{
	const ArrayWrapper2D<char> source(sourceData.data(), 10, 10);

	uint32_t left = 0;
	uint32_t right = 0;
	SmallVector<uint32_t, 2> topBottom;

	for (int32_t y = 0; y < 10; y++)
	{
		if ((y > 0) && (y < 9))
		{
			for (int32_t x = 1; x < 9; x++)
			{
				pixels->push_back(source(x, y));
			}
		}

		left = (left << 1) | (source(0, y) == '#' ? 1 : 0);
		right = (right << 1) | (source(9, y) == '#' ? 1 : 0);
	}
}

static vector<char> Flip(vector<char> sourceBuffer, int32_t size)
{
	ArrayWrapper2D<char> source(sourceBuffer.data(), size, size);

	for (int32_t y = 0; y < size; y++)
	{
		for (int32_t x = 0; x < (size / 2); x++)
		{
			swap(source(x, y), source((size - x - 1), y));
		}
	} 

	return sourceBuffer;
}

static vector<char> Rotate(const vector<char>& sourceBuffer, int32_t size)
{
	vector<char> destBuffer(sourceBuffer.size());

	const ArrayWrapper2D<char> source(sourceBuffer.data(), size, size);
	ArrayWrapper2D<char> dest(destBuffer.data(), size, size);

	for (int32_t y = 0; y < size; y++)
	{
		for (int32_t x = 0; x < size; x++)
		{
			int32_t rotatedX = size - y - 1;
			int32_t rotatedY = x;
			dest(rotatedX, rotatedY) = source(x, y);
		}
	}

	return destBuffer;
}

static void MakeFlippingTiles(vector<char> source, Tile* tile)
{
	ExtractPixels(source, &tile->Pixels);

	vector<char> flippedSource = Flip(source, 10);

	for (size_t i = 0; i < 8; i += 2)
	{
		ExtractEdges(source, &tile->EdgesByTransform[i + 0]);
		ExtractEdges(flippedSource, &tile->EdgesByTransform[i + 1]);

		source = Rotate(source, 10);
		flippedSource = Rotate(flippedSource, 10);
	}
}

static vector<char> TransformPixels(vector<char> source, uint32_t orientation)
{
	if (orientation & 1)
	{
		source = Flip(source, 8);
	}

	for (size_t i = 0; i < orientation / 2; i++)
	{
		source = Rotate(source, 8);
	}

	return source;
}

static SearchPattern Flip(const SearchPattern& source)
{
	SearchPattern dest{ source.Width, source.Height };
	dest.Points.reserve(source.Points.size());
	ranges::copy(source.Points | views::transform([&](const Vec2Int& a) { return Vec2Int{ source.Width - a.X - 1, a.Y }; }), back_inserter(dest.Points));
	return dest;
}

static SearchPattern Rotate(const SearchPattern& source)
{
	SearchPattern dest{ source.Height, source.Width };
	dest.Points.reserve(source.Points.size());
	ranges::copy(source.Points | views::transform([&](const Vec2Int& a) { return Vec2Int{ source.Height - a.Y - 1, a.X }; }), back_inserter(dest.Points));
	return dest;
}

static vector<SearchPattern> MakeSearchPatterns()
{
	vector<Vec2Int> seaMonster =
	{
		Vec2Int{ 18, 0 },

		Vec2Int{ 0, 1 },

		Vec2Int{ 5, 1 },
		Vec2Int{ 6, 1 },

		Vec2Int{ 11, 1 },
		Vec2Int{ 12, 1 },

		Vec2Int{ 17, 1 },
		Vec2Int{ 18, 1 },
		Vec2Int{ 19, 1 },

		Vec2Int{ 1, 2 },
		Vec2Int{ 4, 2 },
		Vec2Int{ 7, 2 },
		Vec2Int{ 10, 2 },
		Vec2Int{ 13, 2 },
		Vec2Int{ 16, 2 },
	};

	vector<SearchPattern> patterns;
	patterns.reserve(8);

	patterns.push_back({ 20, 3, move(seaMonster) });
	patterns.push_back(Flip(patterns[0]));

	for (size_t i = 2; i < 8; i++)
	{
		patterns.push_back(Rotate(patterns[i - 2]));
	}

	return patterns;
}

static Tile ReadFullTile()
{
	Tile tile;

	Parse::DiscardExpected("Tile ");
	tile.Id = Parse::GetUint32();
	PuzzleInput::DropLine();
	PuzzleInput::NextLine();

	vector<char> tileSource;
	tileSource.reserve(10 * 10);
	for (int c = PuzzleInput::GetChar(); tileSource.size() != (10 * 10); c = PuzzleInput::GetChar())
	{
		if (c != '\n')
		{
			tileSource.push_back(static_cast<char>(c));
		}
	}

	MakeFlippingTiles(tileSource, &tile);

	Parse::DiscardExpected("\n");
	return tile;
}

static uint32_t NotThisTile(const SmallVector<uint32_t, 2>& tiles, uint32_t thisTile)
{
	assert(tiles.size() == 2);
	return tiles[0] == thisTile ? tiles[1] : tiles[0];
}

static uint32_t GetOrientation(const Tile& tile, const pair<EdgeName, uint32_t>& constraint)
{
	for (uint32_t orientation = 0; orientation < 8; orientation++)
	{
		if (tile.EdgesByTransform[orientation][static_cast<uint32_t>(constraint.first)] == constraint.second)
		{
			return orientation;
		}
	}
	assert(false);
	return numeric_limits<uint32_t>::max();
}

void Puzzle20_A_2020()
{
	HashMap<uint32_t, SmallVector<uint32_t, 2>> edgeToTileIndices(2048, numeric_limits<uint32_t>::max());
	vector<pair<uint32_t, int32_t>> tileReferenceCounts;
	tileReferenceCounts.reserve(144);

	while (PuzzleInput::NextLine())
	{
		SmallVector<uint32_t, 8> edges;
		uint32_t tileId = ReadTileIdAndEdges(&edges);

		uint32_t tileIndex = static_cast<uint32_t>(tileReferenceCounts.size());
		tileReferenceCounts.push_back({ tileId, 0 });

		for (int32_t i = 0; i < 8; i++)
		{
			edgeToTileIndices[edges[i]].PushBack(tileIndex);
		}
	}

	for (const auto& edge : edgeToTileIndices)
	{
		const SmallVector<uint32_t, 2>& tilesReferenced = edge.second;
		for (int32_t i = 0; i < tilesReferenced.size(); i++)
		{
			tileReferenceCounts[tilesReferenced[i]].second += tilesReferenced.size();
		}
	}

	auto cornerTiles = tileReferenceCounts
		| views::filter([](const auto& p) { return p.second == 12; })
		| views::keys;
	int64_t answer = accumulate(cornerTiles.begin(), cornerTiles.end(), 1ll, multiplies{});

	PuzzleOutput::Submit(2020, 20, 1, answer);
}

void Puzzle20_B_2020()
{
	HashMap<uint32_t, SmallVector<uint32_t, 2>> edgeToTileIndices(2048, numeric_limits<uint32_t>::max());
	vector<pair<uint32_t, int32_t>> tileReferenceCounts;
	tileReferenceCounts.reserve(144);

	vector<Tile> tiles;
	tiles.reserve(144);
	while (PuzzleInput::NextLine())
	{
		Tile tile = ReadFullTile();
		uint32_t tileIndex = static_cast<uint32_t>(tiles.size());
		tiles.push_back(tile);

		tileReferenceCounts.push_back({ tileIndex, 0 });

		for (int32_t i = 0; i < 8; i++)
		{
			edgeToTileIndices[tile.EdgesByTransform[i][0]].PushBack(tileIndex);
		}
	}

	for (const auto& edge : edgeToTileIndices)
	{
		const SmallVector<uint32_t, 2>& tilesReferenced = edge.second;
		for (int32_t i = 0; i < tilesReferenced.size(); i++)
		{
			tileReferenceCounts[tilesReferenced[i]].second += tilesReferenced.size();
		}
	}

	// Start in the top left corner with an arbitrary corner tile
	uint32_t startingTile = numeric_limits<uint32_t>::max();
	for (const auto& p : tileReferenceCounts)
	{
		if (p.second == 12)
		{
			startingTile = p.first;
			break;
		}
	}

	// Assemble the tiles into a picture
	vector<pair<uint32_t, uint32_t>> tileIdAndOrientationBuffer(144);
	ArrayWrapper2D<pair<uint32_t, uint32_t>> tileIdAndOrientation(tileIdAndOrientationBuffer.data(), 12, 12);

	// Work out the starting orientation
	{
		const Tile& cornerTile = tiles[startingTile];
		for (uint32_t orientation = 0; orientation < 8; orientation++)
		{
			if ((edgeToTileIndices[cornerTile.EdgesByTransform[orientation][EdgeName::Top]].size() == 1) &&
				(edgeToTileIndices[cornerTile.EdgesByTransform[orientation][EdgeName::Left]].size() == 1))
			{
				tileIdAndOrientation(0, 0) = { startingTile, orientation };
				break;
			}
		}
	}

	// Fill in the rest
	for (int32_t y = 0; y < 12; y++)
	{
		// Start the row
		if (y > 0)
		{
			const pair<uint32_t, uint32_t>& tileAbove = tileIdAndOrientation(0, y - 1);
			uint32_t joiningEdge = tiles[tileAbove.first].EdgesByTransform[tileAbove.second][EdgeName::Bottom];
			uint32_t nextTileIndex = NotThisTile(edgeToTileIndices[joiningEdge], tileAbove.first);
			uint32_t orientation = GetOrientation(tiles[nextTileIndex], { EdgeName::Top, joiningEdge });
			tileIdAndOrientation(0, y) = { nextTileIndex, orientation };
		}

		for (int32_t x = 1; x < 12; x++)
		{
			// Left/right edge
			const pair<uint32_t, uint32_t>& tileToLeft = tileIdAndOrientation(x - 1, y);
			uint32_t joiningEdge = tiles[tileToLeft.first].EdgesByTransform[tileToLeft.second][EdgeName::Right];
			uint32_t nextTileIndex = NotThisTile(edgeToTileIndices[joiningEdge], tileToLeft.first);
			uint32_t orientation = GetOrientation(tiles[nextTileIndex], { EdgeName::Left, joiningEdge });
			tileIdAndOrientation(x, y) = { nextTileIndex, orientation };
		}
	}

	// Create a complete image
	vector<uint8_t> imageBuffer(12 * 8 * 12 * 8);
	ArrayWrapper2D<uint8_t> image(imageBuffer.data(), 12 * 8, 12 * 8);
	for (int32_t tileY = 0; tileY < 12; tileY++)
	{
		for (int32_t tileX = 0; tileX < 12; tileX++)
		{
			const pair<uint32_t, uint32_t>& sourceTileIdAndOrientation = tileIdAndOrientation(tileX, tileY);
			const Tile& sourceTile = tiles[sourceTileIdAndOrientation.first];
			vector<char> sourcePixels = TransformPixels(sourceTile.Pixels, sourceTileIdAndOrientation.second);
			for (int32_t pixel = 0; pixel < 64; pixel++)
			{
				image((tileX * 8) + (pixel & 0x7), (tileY * 8) + (pixel / 8)) = sourcePixels[pixel];
			}
		}
	}

	int32_t monstersFound = 0;

	const vector<SearchPattern> monsterPatterns = MakeSearchPatterns();
	for (const SearchPattern& pattern : monsterPatterns)
	{
		for (int32_t y = 0; y < image.Height - pattern.Height; y++)
		{
			for (int32_t x = 0; x < image.Width - pattern.Width; x++)
			{
				bool monsterFound = ranges::all_of(pattern.Points, [&](const Vec2Int& p) { return image(p + Vec2Int{ x, y }) == '#'; });
				if (monsterFound)
				{
					monstersFound++;
				}
			}
		}

		if (monstersFound > 0)
			break;
	}

	// Assume no overlapping monsters
	int32_t answer = static_cast<int32_t>(ranges::count(imageBuffer, '#') - (monstersFound * monsterPatterns.front().Points.size()));

	PuzzleOutput::Submit(2020, 20, 2, answer);
}
