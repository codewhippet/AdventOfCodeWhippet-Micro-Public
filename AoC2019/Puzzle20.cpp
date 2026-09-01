#include "stdafx.h"

using namespace std;

namespace Puzzle20_2019_Types
{
	template <typename TYPE>
	class FromTo
	{
	public:
		FromTo() = default;
		FromTo(const FromTo&) = default;

		FromTo(int32_t size, const TYPE& value)
			: Size(size)
		{
			Data.resize(Size * Size, value);
		}

		TYPE& operator()(int32_t from, int32_t to)
		{
			int32_t index = (from * Size) + to;
			return Data[index];
		}

		const TYPE& operator()(int32_t from, int32_t to) const
		{
			int32_t index = (from * Size) + to;
			return Data[index];
		}

		const TYPE* GetRow(int32_t from) const
		{
			int32_t index = (from * Size);
			return &Data[index];
		}

		int32_t GetSize() const
		{
			return Size;
		}

	private:
		int32_t Size = 0;
		vector<TYPE> Data;
	};

	using Label = pair<char, char>;

	struct Teleporter
	{
		Vec2Int Location;
		Label Label;
		bool IsOuter;
	};

	struct Puzzle
	{
		vector<Teleporter> Teleporters;
		int32_t StartIndex;
		int32_t EndIndex;
		FromTo<int32_t> TeleporterToTeleporterDistances;
	};

	struct TeleportToTeleportFinder
	{
		using State = Vec2Int;

		FromTo<int32_t>* TeleporterToTeleporterDistances = nullptr;

		const uArrayMap2D* Maze = nullptr;
		const HashMap<Vec2Int, int32_t>* TeleporterLocations = nullptr;

		Vec2Int Start;
		int32_t FromIndex;
		vector<int32_t> AlreadyQueued;
		SmallVector<Vec2Int, 4> Neighbours;

		size_t MaxQueueSize() const;

		void InitialiseQueuedChecker();
		void HasQueued(const Vec2Int& s);
		bool HasAlreadyQueued(const Vec2Int& s);

		BFSControl ProcessCurrent(const Vec2Int& s, int32_t steps);

		void CacheNeighbours(const Vec2Int& s);
		const SmallVector<Vec2Int, 4>& GetNeighbours() const;
		bool CanMoveTo(const Vec2Int& s);
	};

	struct FlatFinder
	{
		using State = int32_t;

		const Puzzle& P;
		int32_t Start;
		int32_t End;
		vector<pair<int32_t, int32_t>> Neighbours;
		HashSet<int32_t>* Visited = nullptr;

		size_t MaxQueueSize() const;
		
		void InitialiseVisitedChecker();
		DijkstraControl Visit(const int32_t& s);
		
		void CacheNeighbours(const int32_t& s);
		const vector<pair<int32_t, int32_t>>& GetNeighbours() const;
		bool CanMoveTo(const int32_t& s);
	};

	struct RecursiveFinder
	{
		using State = pair<int32_t, int32_t>;

		const Puzzle& P;
		pair<int32_t, int32_t> Start;
		pair<int32_t, int32_t> End;
		vector<pair<int32_t, pair<int32_t, int32_t>>> Neighbours;
		HashSet<pair<int32_t, int32_t>>* Visited = nullptr;

		size_t MaxQueueSize() const;

		void InitialiseVisitedChecker();
		DijkstraControl Visit(const pair<int32_t, int32_t>& s);

		void CacheNeighbours(const pair<int32_t, int32_t>& s);
		const vector<pair<int32_t, pair<int32_t, int32_t>>>& GetNeighbours() const;
		bool CanMoveTo(const pair<int32_t, int32_t>& s);
	};
}

using namespace Puzzle20_2019_Types;

template <>
struct std::hash<pair<int32_t, int32_t>>
{
	size_t operator()(const pair<int32_t, int32_t>& p) const noexcept
	{
		uint32_t hash = 0x811c9dc5;

		hash ^= p.first;
		hash *= 0x01000193;
		hash ^= p.second;
		hash *= 0x01000193;

		return hash;
	}
};

static bool IsOuterTeleporter(const uArrayMap2D& maze, const Vec2Int& teleporter)
{
	return (teleporter.X == 2)
		|| (teleporter.Y == 2)
		|| (teleporter.X == maze.GetWidth() - 3)
		|| (teleporter.Y == maze.GetHeight() - 3);
}

static Label ParseLabel(const uArrayMap2D &maze, const Vec2Int& adjacentChar)
{
	Label label;
	for (const Vec2Int& dir : array<Vec2Int, 2>{ Vec2Int::Left(), Vec2Int::Up() })
	{
		if (isalpha(maze(adjacentChar + dir)))
		{
			label = { maze(adjacentChar + dir), maze(adjacentChar) };
		}
	}
	for (const Vec2Int& dir : array<Vec2Int, 2>{ Vec2Int::Right(), Vec2Int::Down() })
	{
		if (isalpha(maze(adjacentChar + dir)))
		{
			label = { maze(adjacentChar), maze(adjacentChar + dir) };
		}
	}
	return label;
}

static void FindTeleporterToTeleporterDistances(const uArrayMap2D& maze, Puzzle* puzzle);

static Puzzle ParsePuzzle()
{
	Puzzle puzzle;
	puzzle.TeleporterToTeleporterDistances = FromTo<int32_t>(64, -1);

	size_t mazeBufferSize = 128 * 128;
	uArrayMap2D maze = ReaduArrayMap_PresizedHeap(static_cast<char*>(malloc(mazeBufferSize)), mazeBufferSize);
	for (const auto& p : maze.Grid())
	{
		if (p.second == '.')
		{
			for (const auto& dir : Vec2Int::CardinalDirections())
			{
				Vec2Int neighbour = p.first + dir;
				if (isalpha(maze(neighbour)))
				{
					Label label = ParseLabel(maze, neighbour);
					if (label == Label{ 'A', 'A' })
					{
						puzzle.StartIndex = static_cast<int32_t>(puzzle.Teleporters.size());
					}
					if (label == Label{ 'Z', 'Z' })
					{
						puzzle.EndIndex = static_cast<int32_t>(puzzle.Teleporters.size());
					}

					puzzle.Teleporters.push_back({ p.first, label, IsOuterTeleporter(maze, p.first) });
				}
			}
		}
	}

	FindTeleporterToTeleporterDistances(maze, &puzzle);

	return puzzle;
}

// ----------------------------------------------------------------------------

size_t TeleportToTeleportFinder::MaxQueueSize() const
{
	return 32;
}

void TeleportToTeleportFinder::InitialiseQueuedChecker()
{
	AlreadyQueued.clear();
	AlreadyQueued.resize(Maze->GetWidth() * Maze->GetHeight(), 0);
}

void TeleportToTeleportFinder::HasQueued(const Vec2Int& s)
{
	size_t index = (s.Y * Maze->GetWidth()) + s.X;
	AlreadyQueued[index] = 1;
}

bool TeleportToTeleportFinder::HasAlreadyQueued(const Vec2Int& s)
{
	size_t index = (s.Y * Maze->GetWidth()) + s.X;
	return AlreadyQueued[index];
}

BFSControl TeleportToTeleportFinder::ProcessCurrent(const Vec2Int& s, int32_t steps)
{
	if (s != Start)
	{
		int32_t teleporterIndex = -1;
		if (TeleporterLocations->TryFind(s, &teleporterIndex))
		{
			(*TeleporterToTeleporterDistances)(FromIndex, teleporterIndex) = steps;
			(*TeleporterToTeleporterDistances)(teleporterIndex, FromIndex) = steps;
			return BFSControl::Skip;
		}
	}

	return BFSControl::Process;
}

void TeleportToTeleportFinder::CacheNeighbours(const Vec2Int& s)
{
	Neighbours.Clear();
	for (const Vec2Int& dir : Vec2Int::CardinalDirections())
	{
		Neighbours.PushBack(s + dir);
	}
}

const SmallVector<Vec2Int, 4>& TeleportToTeleportFinder::GetNeighbours() const
{
	return Neighbours;
}

bool TeleportToTeleportFinder::CanMoveTo(const Vec2Int& s)
{
	return (*Maze)(s) == '.';
}

static void FindTeleporterToTeleporterDistances(const uArrayMap2D& maze, Puzzle* puzzle)
{
	BFS<TeleportToTeleportFinder> bfs;
	bfs.TeleporterToTeleporterDistances = &puzzle->TeleporterToTeleporterDistances;
	bfs.Maze = &maze;

	HashMap<Vec2Int, int32_t> teleporterLocations(128, Vec2Int::Min());
	for (int32_t i = 0; i < static_cast<int32_t>(puzzle->Teleporters.size()); i++)
	{
		teleporterLocations.Insert(puzzle->Teleporters[i].Location, i);
	}
	bfs.TeleporterLocations = &teleporterLocations;

	for (int32_t i = 0; i < static_cast<int32_t>(puzzle->Teleporters.size()); i++)
	{
		bfs.FromIndex = i;
		bfs.Start = puzzle->Teleporters[i].Location;
		bfs.FindAll(puzzle->Teleporters[i].Location);
	}
}

// ----------------------------------------------------------------------------

size_t FlatFinder::MaxQueueSize() const
{
	return 32;
}

void FlatFinder::InitialiseVisitedChecker()
{
}

DijkstraControl FlatFinder::Visit(const int32_t& s)
{
	if (s == P.EndIndex)
		return DijkstraControl::Found;

	if (Visited->Insert(s) == false)
		return DijkstraControl::Skip;

	return DijkstraControl::Process;
}

void FlatFinder::CacheNeighbours(const int32_t& from)
{
	Neighbours.clear();
	const int32_t *to = P.TeleporterToTeleporterDistances.GetRow(from);
	for (int32_t i = 0; i < P.TeleporterToTeleporterDistances.GetSize(); i++)
	{
		if (to[i] != -1)
		{
			Neighbours.push_back({ to[i], i });
		}
	}
}

const vector<pair<int32_t, int32_t>>& FlatFinder::GetNeighbours() const
{
	return Neighbours;
}

bool FlatFinder::CanMoveTo(const int32_t&)
{
	return true;
}

static int32_t ShortestPath(const Puzzle& puzzle)
{
	Dijkstra<FlatFinder> dijkstra{ puzzle };
	HashSet<int32_t> visited(128, -1);
	dijkstra.Visited = &visited;
	dijkstra.Neighbours.reserve(16);

	dijkstra.Start = puzzle.StartIndex;
	dijkstra.End = puzzle.EndIndex;

	int32_t shortest = dijkstra.FindShortest(puzzle.StartIndex);

	return shortest;
}

// ----------------------------------------------------------------------------

size_t RecursiveFinder::MaxQueueSize() const
{
	return 128;
}

void RecursiveFinder::InitialiseVisitedChecker()
{
}

DijkstraControl RecursiveFinder::Visit(const pair<int32_t, int32_t>& s)
{
	if (s == End)
		return DijkstraControl::Found;

	if (Visited->Insert(s) == false)
		return DijkstraControl::Skip;

	return DijkstraControl::Process;
}

void RecursiveFinder::CacheNeighbours(const pair<int32_t, int32_t>& from)
{
	Neighbours.clear();
	const int32_t* to = P.TeleporterToTeleporterDistances.GetRow(from.first);
	for (int32_t i = 0; i < P.TeleporterToTeleporterDistances.GetSize(); i++)
	{
		if (to[i] != -1)
		{
			if (to[i] == 0)
			{
				// Up and down connections
				if (P.Teleporters[from.first].IsOuter)
				{
					if (from.second > 0)
					{
						Neighbours.push_back({ 1, { i, from.second - 1 } });
					}
				}
				else
				{
					Neighbours.push_back({ 1, { i, from.second + 1 } });
				}
			}
			else
			{
				// Same level connections
				Neighbours.push_back({ to[i], { i, from.second } });
			}
		}
	}
}

const vector<pair<int32_t, pair<int32_t, int32_t>>>& RecursiveFinder::GetNeighbours() const
{
	return Neighbours;
}

bool RecursiveFinder::CanMoveTo(const pair<int32_t, int32_t>&)
{
	return true;
}

static int32_t ShortestPathRecursive(const Puzzle& puzzle)
{
	Dijkstra<RecursiveFinder> dijkstra{ puzzle };
	HashSet<pair<int32_t, int32_t>> visited(16 * 1024, { -1, -1 });
	dijkstra.Visited = &visited;
	dijkstra.Neighbours.reserve(16);

	dijkstra.Start = { puzzle.StartIndex, 0 };
	dijkstra.End = { puzzle.EndIndex, 0 };

	int32_t shortest = dijkstra.FindShortest(dijkstra.Start);

	return shortest;
}

// ----------------------------------------------------------------------------

void Puzzle20_A_2019()
{
	Puzzle puzzle = ParsePuzzle();

	// Connect the inner and outer teleporters
	for (const auto& p : AllUnorderedPairs(puzzle.Teleporters.size()))
	{
		if (puzzle.Teleporters[p.first].Label == puzzle.Teleporters[p.second].Label)
		{
			puzzle.TeleporterToTeleporterDistances((int32_t)p.first, (int32_t)p.second) = 1;
			puzzle.TeleporterToTeleporterDistances((int32_t)p.second, (int32_t)p.first) = 1;
		}
	}

	int32_t answer = ShortestPath(puzzle);

	PuzzleOutput::Submit(2019, 20, 1, answer);
}

void Puzzle20_B_2019()
{
	Puzzle puzzle = ParsePuzzle();

	// Connect the inner and outer teleporters
	for (const auto& p : AllUnorderedPairs(puzzle.Teleporters.size()))
	{
		if (puzzle.Teleporters[p.first].Label == puzzle.Teleporters[p.second].Label)
		{
			puzzle.TeleporterToTeleporterDistances((int32_t)p.first, (int32_t)p.second) = 0;
			puzzle.TeleporterToTeleporterDistances((int32_t)p.second, (int32_t)p.first) = 0;
		}
	}

	int32_t answer = ShortestPathRecursive(puzzle);

	PuzzleOutput::Submit(2019, 20, 2, answer);
}
