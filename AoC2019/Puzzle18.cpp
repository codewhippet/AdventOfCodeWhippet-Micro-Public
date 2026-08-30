#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle18_2019_Types
{
	template <int32_t K>
	struct Combinatorials
	{
		Combinatorials(int32_t N)
			: NChooseK(N + 1, { 1 })
		{
			for (int32_t n = 1; n <= N; n++)
			{
				int32_t numerator = 1;
				int32_t denominator = 1;

				for (int32_t c = 1; c <= min(n, K); c++)
				{
					numerator *= n - c + 1;
					denominator *= c;

					assert((numerator % denominator) == 0);
					NChooseK[n][c] = numerator / denominator;
				}
			}
		}

		int16_t ToCns(const array<int32_t, K>& positions) const
		{
			array<int32_t, K> sortedPos = positions;
			ranges::sort(sortedPos);

			int32_t cns = 0;

			int32_t k = 1;
			for (int32_t n : sortedPos)
			{
				cns += NChooseK[n][k++];
			}

			return static_cast<int16_t>(cns);
		}

		void FromCns(int32_t cns, array<int32_t, K>* positions) const
		{
			int32_t n = static_cast<int32_t>(NChooseK.size() - 2);
			for (int32_t k = K; k > 0; k--)
			{
				for (/***/; n >= 0; n--)
				{
					if (NChooseK[n][k] <= cns)
					{
						cns -= NChooseK[n][k];
						(*positions)[k - 1] = n--;
						break;
					}
				}
			}
		}

		uvector<array<int32_t, K + 1>> NChooseK;
	};

	template <typename TYPE>
	class FromTo
	{
	public:
		FromTo() = default;
		FromTo(const FromTo&) = default;

		FromTo(int32_t size, TYPE* data)
			: Size(size)
			, Data(data)
		{
		}

		TYPE& operator()(int32_t from, int32_t to)
		{
			int32_t index = (from * Size) + to;
			assert(index >= 0);
			assert(index < (Size * Size));
			return Data[index];
		}

		const TYPE& operator()(int32_t from, int32_t to) const
		{
			int32_t index = (from * Size) + to;
			assert(index >= 0);
			assert(index < (Size * Size));
			return Data[index];
		}

		const TYPE* GetRow(int32_t from) const
		{
			int32_t index = (from * Size);
			assert(index >= 0);
			assert(index < (Size * Size));
			return &Data[index];
		}

	private:
		int32_t Size = 0;
		TYPE* Data = nullptr;
	};

	struct Edge
	{
		int32_t Length;
		uint32_t Requires;
	};

	struct Puzzle
	{
		Combinatorials<4> Comb;
		Vec2Int Start;
		uvector<Vec2Int> SpecialLocations;
		FromTo<Edge> KeyToKeyEdges;
		FromTo<int32_t> KeyToKeyPaths;
	};

	struct KeyToKeySearchNode
	{
		Vec2Int Position;
		int32_t Steps;
		uint32_t Requires;
	};

	struct UncompressedState
	{
		array<int32_t, 4> RobotLocations;
		uint32_t KeysCollected;
	};

	struct CompressedState
	{
		int16_t RobotLocationsCns;
		uint16_t KeysCollectedLo;
		uint16_t KeysCollectedHi;
		auto operator<=>(const CompressedState&) const = default;
	};

	struct CompressedSearchNode
	{
		int16_t Priority;
		CompressedState State;
		auto operator<=>(const CompressedSearchNode&) const = default;
	};

	struct Neighbour
	{
		int32_t Distance;
		UncompressedState State;
		auto operator<=>(const Neighbour&) const = default;
	};
}

using namespace Puzzle18_2019_Types;

template <>
struct std::hash<CompressedState>
{
	size_t operator()(const CompressedState& m) const noexcept
	{
		uint32_t hash = 0x811c9dc5;

		hash ^= m.RobotLocationsCns;
		hash *= 0x01000193;
		hash ^= m.KeysCollectedLo;
		hash *= 0x01000193;
		hash ^= m.KeysCollectedHi;
		hash *= 0x01000193;

		return hash;
	}
};

// ----------------------------------------------------------------------------

static void CompressState(const Puzzle& puzzle, const UncompressedState& src, CompressedState* dst)
{
	dst->RobotLocationsCns = puzzle.Comb.ToCns(src.RobotLocations);
	dst->KeysCollectedLo = static_cast<uint16_t>(src.KeysCollected & 0xffff);
	dst->KeysCollectedHi = static_cast<uint16_t>((src.KeysCollected >> 16) & 0xffff);
}

static void UncompressState(const Puzzle& puzzle, const CompressedState& src, UncompressedState* dst)
{
	puzzle.Comb.FromCns(src.RobotLocationsCns, &dst->RobotLocations);
	dst->KeysCollected = (static_cast<uint32_t>(src.KeysCollectedHi << 16)) | (src.KeysCollectedLo);
}

template <typename KEY_TYPE, typename MAPPED_TYPE>
class HashMap18
{
public:
	using key_type = KEY_TYPE;
	using mapped_type = MAPPED_TYPE;
	using value_type = std::pair<KEY_TYPE, MAPPED_TYPE>;
	using size_type = uint32_t;

	HashMap18(uint32_t size, const key_type& invalidKeyValue)
		: Table(size, { invalidKeyValue, {} })
		, TableSizeMask(size - 1)
		, MapSize(0)
		, InvalidKeyValue(invalidKeyValue)
	{
		// Tables must be a power of two
		assert(std::popcount(size) == 1);
	}

	HashMap18(HashMap18&&) = delete;
	HashMap18& operator=(HashMap18&&) = delete;

	HashMap18(const HashMap18&) = delete;
	HashMap18& operator=(const HashMap18&) = delete;

	bool Set(const key_type& key, const mapped_type& value)
	{
		uint32_t hashIndex = static_cast<uint32_t>(std::hash<key_type>{}(key));
		const uint32_t tableSize = static_cast<uint32_t>(Table.size());
		for (uint32_t i = 0; i < tableSize; i++, hashIndex++)
		{
			const uint32_t tableIndex = hashIndex & TableSizeMask;
			if (Table[tableIndex].first == key)
			{
				Table[tableIndex].second = value;
				return true;
			}

			if (Table[tableIndex].first == InvalidKeyValue)
			{
				Table[tableIndex] = { key, value };
				MapSize++;
				return true;
			}

#if _DEBUG && WIN32
			DEBUG_Collisions++;
#endif
		}

		assert(false);
		return false;
	}

	mapped_type FindOrDefault(const key_type& key, const mapped_type& def) const
	{
		uint32_t hashIndex = static_cast<uint32_t>(std::hash<key_type>{}(key));
		const uint32_t tableSize = static_cast<uint32_t>(Table.size());
		for (uint32_t i = 0; i < tableSize; i++, hashIndex++)
		{
			const uint32_t tableIndex = hashIndex & TableSizeMask;
			if (Table[tableIndex].first == key)
			{
				return Table[tableIndex].second;
			}

			if (Table[tableIndex].first == InvalidKeyValue)
			{
				return def;
			}
		}

		return def;
	}

	mapped_type& At(const key_type& key)
	{
		uint32_t hashIndex = static_cast<uint32_t>(std::hash<key_type>{}(key));
		const uint32_t tableSize = static_cast<uint32_t>(Table.size());
		for (uint32_t i = 0; i < tableSize; i++, hashIndex++)
		{
			const uint32_t tableIndex = hashIndex & TableSizeMask;
			if (Table[tableIndex].first == key)
			{
				return Table[tableIndex].second;
			}
		}

		//Hardware::FlashingStop(10);
		assert(false);
		return *reinterpret_cast<mapped_type*>(0);
	}

private:

	uvector<value_type> Table;
	uint32_t TableSizeMask;
	uint32_t MapSize;
	key_type InvalidKeyValue;

#if _DEBUG && WIN32
	size_t DEBUG_Collisions = 0;
#endif
};

// ----------------------------------------------------------------------------

static void GenerateEdges(const uArrayMap2D& maze, int32_t fromId, Vec2Int start, FromTo<Edge>* edges)
{
	uvector<KeyToKeySearchNode> searchQueue;
	searchQueue.reserve(1024);
	searchQueue.push_back({ start, 0, 0 });
	uvector<int32_t> queued(maze.GetWidth() * maze.GetHeight());

	auto queuedIndex = [&](const Vec2Int& p) { return (p.Y * maze.GetWidth()) + p.X; };

	for (size_t i = 0; i < searchQueue.size(); i++)
	{
		KeyToKeySearchNode current = searchQueue[i];
		char currentTile = maze(current.Position);
		if ((currentTile < 30) && (currentTile != fromId))
		{
			(*edges)(fromId, currentTile) = { current.Steps, current.Requires };
			continue;
		}
		else if (isalpha(currentTile))
		{
			assert(isupper(currentTile));
			current.Requires |= 1u << (currentTile - 'A');
		}

		for (const Vec2Int& dir : Vec2Int::CardinalDirections())
		{
			Vec2Int neighbour = current.Position + dir;
			int32_t queueIndex = queuedIndex(neighbour);
			if ((maze(neighbour) != '#') && (queued[queueIndex] == 0))
			{
				searchQueue.push_back({ neighbour, current.Steps + 1, current.Requires });
				queued[queueIndex] = 1;
			}
		}
	}
}

static void GenerateAllEdges(const uArrayMap2D& maze, Puzzle* p)
{
	for (int32_t i = 0; i < 30; i++)
	{
		GenerateEdges(maze, i, p->SpecialLocations[i], &p->KeyToKeyEdges);
	}
}

static void GenerateShortestPaths(int32_t from, Puzzle* p)
{
	uvector<pair<int32_t, int32_t>> priorityQueue;
	priorityQueue.reserve(16);
	priorityQueue.push_back({ 0, from });

	auto enqueue = [&](const pair<int32_t, int32_t>& s)
		{
			priorityQueue.push_back(s);
			ranges::push_heap(priorityQueue);
		};

	auto dequeue = [&]()
		{
			pair<int32_t, int32_t> s = priorityQueue[0];
			ranges::pop_heap(priorityQueue);
			priorityQueue.pop_back();
			return s;
		};

	uvector<int32_t> visited;
	visited.resize(30);

	while (priorityQueue.empty() == false)
	{
		pair<int32_t, int32_t> current = dequeue();
		if (visited[current.second])
			continue;

		p->KeyToKeyPaths(from, current.second) = -current.first;
		visited[current.second] = 1;

		const Edge* neighbours = p->KeyToKeyEdges.GetRow(current.second);
		for (int32_t i = 0; i < 30; i++)
		{
			if (neighbours[i].Length == -1)
				continue;

			if (visited[i] == 0)
			{
				enqueue({ current.first - neighbours[i].Length, i });
			}
		}
	}
}

static void GenerateAllShortestPaths(Puzzle* p)
{
	for (int32_t from = 0; from < 30; from++)
	{
		GenerateShortestPaths(from, p);
	}
}

static void ParsePuzzleNew(Puzzle* puzzle)
{
	uArrayMap2D maze = ReaduArrayMap('.');
	for (const auto& p : maze.Grid())
	{
		if (p.second == '@')
		{
			puzzle->Start = p.first;
		}
		else if (isalpha(p.second) && islower(p.second))
		{
			int32_t doorId = p.second - 'a';
			puzzle->SpecialLocations[doorId] = p.first;
			maze(p.first) = static_cast<char>(doorId);
		}
	}

	maze(puzzle->Start) = '#';
	maze(puzzle->Start + Vec2Int::Up()) = '#';
	maze(puzzle->Start + Vec2Int::Down()) = '#';
	maze(puzzle->Start + Vec2Int::Left()) = '#';
	maze(puzzle->Start + Vec2Int::Right()) = '#';

	maze(puzzle->Start + Vec2Int::Up() + Vec2Int::Left()) = '1';
	maze(puzzle->Start + Vec2Int::Up() + Vec2Int::Right()) = '2';
	maze(puzzle->Start + Vec2Int::Down() + Vec2Int::Left()) = '3';
	maze(puzzle->Start + Vec2Int::Down() + Vec2Int::Right()) = '4';

	puzzle->SpecialLocations[26] = puzzle->Start + Vec2Int::Up() + Vec2Int::Left();
	puzzle->SpecialLocations[27] = puzzle->Start + Vec2Int::Up() + Vec2Int::Right();
	puzzle->SpecialLocations[28] = puzzle->Start + Vec2Int::Down() + Vec2Int::Left();
	puzzle->SpecialLocations[29] = puzzle->Start + Vec2Int::Down() + Vec2Int::Right();

	GenerateAllEdges(maze, puzzle);
	GenerateAllShortestPaths(puzzle);
}

static void GetNeighbours(const UncompressedState& from, const Puzzle& puzzle, SmallVector<Neighbour, 16>* neighbours)
{
	for (int32_t i = 0; i < 4; i++)
	{
		const Edge* edges = puzzle.KeyToKeyEdges.GetRow(from.RobotLocations[i]);
		for (int32_t to = 0; to < 30; to++)
		{
			if (edges[to].Length == -1)
				continue;

			// Check for locked doors
			if ((edges[to].Requires & from.KeysCollected) != edges[to].Requires)
				continue;

			Neighbour neighbour;
			neighbour.Distance = edges[to].Length;
			neighbour.State = from;

			neighbour.State.RobotLocations[i] = to;
			neighbour.State.KeysCollected |= 1u << to;

			neighbours->PushBack(neighbour);
		}
	}
}

int32_t Heuristic(const UncompressedState& from, const Puzzle& puzzle)
{
	array<int32_t, 4> robotMaxDistances = {};

	uint32_t uncollectedKeys = ~from.KeysCollected;
	for (int32_t key = 0; key < 26; key++)
	{
		if (uncollectedKeys & 1)
		{
			for (int32_t robot = 0; robot < 4; robot++)
			{
				robotMaxDistances[robot] = max(robotMaxDistances[robot], puzzle.KeyToKeyPaths(from.RobotLocations[robot], key));
			}
		}

		uncollectedKeys >>= 1;
	}

	int32_t h = robotMaxDistances[0] + robotMaxDistances[1] + robotMaxDistances[2] + robotMaxDistances[3];
	return h;
}

static int32_t ShortestPathMultibot(const Puzzle& puzzle)
{
	const uint32_t allKeys = 0x03ffffff;

	const UncompressedState uncompressedStart{ { 26, 27, 28, 29 }, {} };
	CompressedState compressedStart;
	CompressState(puzzle, uncompressedStart, &compressedStart);

	uvector<CompressedSearchNode> priorityQueue;
	priorityQueue.reserve(6 * 1024);
	priorityQueue.push_back({ 0, compressedStart });

	auto enqueue = [&](const CompressedSearchNode& s)
		{
			priorityQueue.push_back(s);
			assert(priorityQueue.size() <= 6 * 1024);
			ranges::push_heap(priorityQueue);
		};

	auto dequeue = [&]()
		{
			CompressedSearchNode s = priorityQueue[0];
			ranges::pop_heap(priorityQueue);
			priorityQueue.pop_back();
			return s;
		};

	HashMap18<CompressedState, int16_t> gScore(16 * 1024, { 0 });
	gScore.Set(compressedStart, 0);

	UncompressedState uncompressedState;
	while (priorityQueue.empty() == false)
	{
		CompressedSearchNode current = dequeue();
		uint32_t currentKeysCollected = (static_cast<uint32_t>(current.State.KeysCollectedHi << 16)) | (current.State.KeysCollectedLo);
		if (currentKeysCollected == allKeys)
		{
			return -current.Priority;
		}

		UncompressState(puzzle, current.State, &uncompressedState);

		SmallVector<Neighbour, 16> neighbours;
		GetNeighbours(uncompressedState, puzzle, &neighbours);
		for (const Neighbour& neighbour : neighbours)
		{
			CompressedState compressedNeighbour;
			CompressState(puzzle, neighbour.State, &compressedNeighbour);

			int32_t tentativeGScore = gScore.At(current.State) + neighbour.Distance;
			if (gScore.FindOrDefault(compressedNeighbour, numeric_limits<int16_t>::max()) > tentativeGScore)
			{
				gScore.Set(compressedNeighbour, static_cast<int16_t>(tentativeGScore));
				int16_t priority = static_cast<int16_t>(tentativeGScore + Heuristic(neighbour.State, puzzle));
				enqueue({ -priority, compressedNeighbour });
			}
		}
	}

	return -1;
}

void Puzzle18_A_2019()
{
	int32_t answer = 0;
	PuzzleOutput::Submit(2019, 18, 1, answer);
}

void Puzzle18_B_2019()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 200 * 1024;
	cfg.DebugFlags = MemArenaDebugFlags::PrintOnOutOfMemory | MemArenaDebugFlags::BreakOnOutOfMemory;

	MemArena_Configure(cfg);
	{
		uvector<Edge> edgeBuffer(30 * 30, { -1, 0xffffffffu });
		uvector<int32_t> pathLengthBuffer(30 * 30, 0);

		Puzzle puzzle{ Combinatorials<4>(30) };
		puzzle.SpecialLocations.resize(30);
		puzzle.KeyToKeyEdges = FromTo<Edge>(30, edgeBuffer.data());
		puzzle.KeyToKeyPaths = FromTo<int32_t>(30, pathLengthBuffer.data());

		ParsePuzzleNew(&puzzle);

		int32_t answer = ShortestPathMultibot(puzzle);

		PuzzleOutput::Submit(2019, 18, 2, answer);
	}
	MemArena_Reset();
}
