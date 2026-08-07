#include "stdafx.h"

using namespace std;

namespace Puzzle18_2015_Types
{
	// Candidate for general utility?
	class GameOfLifeBoard
	{
	public:

		GameOfLifeBoard(int32_t width, int32_t height, char invalid);
		GameOfLifeBoard(int32_t width, int32_t height, char* data, char invalid);
		GameOfLifeBoard(const GameOfLifeBoard& other);
		GameOfLifeBoard(GameOfLifeBoard&& other) noexcept;
		~GameOfLifeBoard();

		GameOfLifeBoard& operator=(const GameOfLifeBoard& other);
		GameOfLifeBoard& operator=(GameOfLifeBoard&& other) noexcept;

		int32_t GetWidth() const
		{
			return m_width;
		}

		int32_t GetHeight() const
		{
			return m_height;
		}

		void SetCell(Vec2Int p, char c);
		int32_t Count(char c);

		void Print() const;

	private:

		friend class GameOfLifeReadKernel;
		friend class GameOfLifeWriteCell;

		int32_t GetDataSize() const;

		int32_t m_width;
		int32_t m_height;

		char* m_pStorage;
		char m_invalid;

	};

	GameOfLifeBoard::GameOfLifeBoard(int32_t width, int32_t height, char invalid)
	{
		m_width = width;
		m_height = height;

		int32_t dataSize = GetDataSize();
		m_pStorage = static_cast<char*>(MemArena_Alloc(dataSize));
		memset(m_pStorage, invalid, dataSize);

		m_invalid = invalid;
	}

	GameOfLifeBoard::GameOfLifeBoard(int32_t width, int32_t height, char* data, char invalid)
	{
		m_width = width;
		m_height = height;

		m_pStorage = data;

		m_invalid = invalid;
	}

	GameOfLifeBoard::GameOfLifeBoard(const GameOfLifeBoard& other)
	{
		m_width = other.m_width;
		m_height = other.m_height;

		int32_t dataSize = GetDataSize();
		m_pStorage = static_cast<char*>(MemArena_Alloc(dataSize));
		memcpy(m_pStorage, other.m_pStorage, dataSize);

		m_invalid = other.m_invalid;
	}

	GameOfLifeBoard::GameOfLifeBoard(GameOfLifeBoard&& other) noexcept
	{
		m_width = other.m_width;
		m_height = other.m_height;

		m_pStorage = other.m_pStorage;
		other.m_pStorage = nullptr;

		m_invalid = other.m_invalid;
	}

	GameOfLifeBoard::~GameOfLifeBoard()
	{
		MemArena_Free(m_pStorage);
	}

	GameOfLifeBoard& GameOfLifeBoard::operator=(GameOfLifeBoard&& other) noexcept
	{
		MemArena_Free(m_pStorage);

		m_width = other.m_width;
		m_height = other.m_height;

		m_pStorage = other.m_pStorage;
		other.m_pStorage = nullptr;

		m_invalid = other.m_invalid;

		return *this;
	}

	GameOfLifeBoard& GameOfLifeBoard::operator=(const GameOfLifeBoard& other)
	{
		MemArena_Free(m_pStorage);

		m_width = other.m_width;
		m_height = other.m_height;

		int32_t dataSize = GetDataSize();
		m_pStorage = static_cast<char*>(MemArena_Alloc(dataSize));
		memcpy(m_pStorage, other.m_pStorage, dataSize);

		m_invalid = other.m_invalid;

		return *this;
	}

	void GameOfLifeBoard::SetCell(Vec2Int p, char c)
	{
		if ((p.X < 0) || (p.X >= m_width))
			return;
		if ((p.Y < 0) || (p.Y >= m_height))
			return;

		int32_t stride = m_width + 2;
		m_pStorage[((p.Y + 1) * stride) + (p.X + 1)] = c;
	}

	int32_t GameOfLifeBoard::Count(char c)
	{
		int32_t count = 0;

		const char* current = m_pStorage + m_width + 2;
		for (int32_t y = 0; y < m_height; y++)
		{
			current++;
			for (int32_t x = 0; x < m_width; x++, current++)
			{
				if (*current == c)
				{
					count++;
				}
			}
			current++;
		}

		return count;
	}

	void GameOfLifeBoard::Print() const
	{
		std::string s;

		int32_t size = GetDataSize();
		for (int32_t i = 0; i < size; i++)
		{
			if ((i > 0) && (i % (m_width + 2)) == 0)
			{
				s += '\n';
			}
			s += m_pStorage[i];
		}

		printf("%s\n", s.c_str());
	}

	int32_t GameOfLifeBoard::GetDataSize() const
	{
		return (m_width + 2) * (m_height + 2);
	}

	GameOfLifeBoard ReadGameOfLifeBoard(char emptyChar = '.')
	{
		size_t allocated = 0;
		size_t used = 0;
		char* mapData = static_cast<char*>(MemArena_AllocLargest(&allocated));

		// How wide is the board?
		for (int c = PuzzleInput::GetChar(); c != '\n'; c = PuzzleInput::GetChar())
		{
			mapData[used++] = (char)c;
		}
		Vec2Int dimensions{ (int32_t)used, 1 };
		
		// Shift the top line down and add padding
		memcpy(&mapData[dimensions.X + 3], &mapData[0], dimensions.X);
		memset(&mapData[0], emptyChar, dimensions.X + 3);
		used += dimensions.X + 3;
		mapData[used++] = emptyChar;

		while (PuzzleInput::NextLine())
		{
			dimensions.Y++;

			mapData[used++] = emptyChar;
			for (int c = PuzzleInput::GetChar(); c != '\n'; c = PuzzleInput::GetChar())
			{
				assert(used <= allocated);
				mapData[used++] = (char)c;
			}
			mapData[used++] = emptyChar;
		}

		memset(&mapData[used], emptyChar, dimensions.X + 2);
		used += dimensions.X + 2;

		MemArena_ShrinkAlloc(mapData, used);

		return GameOfLifeBoard(dimensions.X, dimensions.Y, mapData, emptyChar);
	}

	class GameOfLifeReadKernel
	{
	public:
		GameOfLifeReadKernel(const GameOfLifeBoard* board)
			: m_board(board)
		{
			m_kernelLeftEdge[0] = m_board->m_pStorage;
			m_kernelLeftEdge[1] = m_kernelLeftEdge[0] + m_board->m_width + 2;
			m_kernelLeftEdge[2] = m_kernelLeftEdge[1] + m_board->m_width + 2;
		}

		bool IsEnd() const
		{
			return (m_focusCell.Y == m_board->m_height);
		}

		void Next()
		{
			m_kernelLeftEdge[0]++;
			m_kernelLeftEdge[1]++;
			m_kernelLeftEdge[2]++;

			if (++m_focusCell.X == m_board->m_width)
			{
				m_focusCell.X = 0;
				m_focusCell.Y++;

				m_kernelLeftEdge[0] += 2;
				m_kernelLeftEdge[1] += 2;
				m_kernelLeftEdge[2] += 2;
			}
		}

		char GetFocusCell() const
		{
			return m_kernelLeftEdge[1][1];
		}

		int32_t CountSurrounding(char c) const
		{
			int32_t count = 0;

			count += (m_kernelLeftEdge[0][0] == c);
			count += (m_kernelLeftEdge[0][1] == c);
			count += (m_kernelLeftEdge[0][2] == c);

			count += (m_kernelLeftEdge[1][0] == c);
			count += (m_kernelLeftEdge[1][2] == c);

			count += (m_kernelLeftEdge[2][0] == c);
			count += (m_kernelLeftEdge[2][1] == c);
			count += (m_kernelLeftEdge[2][2] == c);

			return count;
		}

	private:

		const GameOfLifeBoard* m_board = nullptr;
		Vec2Int m_focusCell = {};
		const char* m_kernelLeftEdge[3] = {};
	};

	class GameOfLifeWriteCell
	{
	public:
		GameOfLifeWriteCell(GameOfLifeBoard* board)
			: m_board(board)
		{
			m_cellStorage = m_board->m_pStorage + m_board->m_width + 3;
		}

		void Next()
		{
			m_cellStorage++;

			if (++m_focusCell.X == m_board->m_width)
			{
				m_focusCell.X = 0;
				m_focusCell.Y++;

				m_cellStorage += 2;
			}
		}

		void SetFocusCell(char c)
		{
			*m_cellStorage = c;
		}

	private:
		GameOfLifeBoard* m_board = nullptr;
		Vec2Int m_focusCell = {};
		char* m_cellStorage = nullptr;
	};
}

using namespace Puzzle18_2015_Types;

static void SetCorners(GameOfLifeBoard* board)
{
	board->SetCell({ 0, 0 }, '#');
	board->SetCell({ board->GetWidth() - 1, 0 }, '#');
	board->SetCell({ 0, board->GetHeight() - 1 }, '#');
	board->SetCell({ board->GetWidth() - 1, board->GetHeight() - 1 }, '#');
}

void Puzzle18_A_2015()
{
	MemArenaConfig cfg = {};
	cfg.LargeBlockRegionSize = 24 * 1024;

	MemArena_Configure(cfg);
	{
		const int iterations = 100;

		vector<GameOfLifeBoard> boards;
		boards.reserve(2);

		boards.emplace_back(ReadGameOfLifeBoard());
		boards.emplace_back(GameOfLifeBoard{ boards[0] });

		for (int i = 0; i < iterations; i++)
		{
			GameOfLifeBoard& activeBoard = boards[i % 2];
			GameOfLifeBoard& nextBoard = boards[1 - (i % 2)];

			GameOfLifeReadKernel readKernel(&activeBoard);
			GameOfLifeWriteCell writeCell(&nextBoard);

			for (/***/; !readKernel.IsEnd(); readKernel.Next(), writeCell.Next())
			{
				int32_t neighboursAlive = readKernel.CountSurrounding('#');

				if (readKernel.GetFocusCell() == '#')
				{
					// A light which is on stays on when 2 or 3 neighbors are on, and turns off otherwise.
					writeCell.SetFocusCell((neighboursAlive == 2 || neighboursAlive == 3) ? '#' : '.');
				}
				else
				{
					// A light which is off turns on if exactly 3 neighbors are on, and stays off otherwise.
					writeCell.SetFocusCell((neighboursAlive == 3) ? '#' : '.');
				}
			}
		}

		int32_t answer = boards[iterations % 2].Count('#');

		PuzzleOutput::Submit(2015, 18, 1, answer);
	}
	MemArena_Reset();
}

void Puzzle18_B_2015()
{
	MemArenaConfig cfg = {};
	cfg.LargeBlockRegionSize = 24 * 1024;

	MemArena_Configure(cfg);
	{
		const int iterations = 100;

		vector<GameOfLifeBoard> boards;
		boards.reserve(2);

		boards.emplace_back(ReadGameOfLifeBoard());
		boards.emplace_back(GameOfLifeBoard{ boards[0] });

		SetCorners(&boards[0]);

		for (int i = 0; i < iterations; i++)
		{
			GameOfLifeBoard& activeBoard = boards[i % 2];
			GameOfLifeBoard& nextBoard = boards[1 - (i % 2)];

			GameOfLifeReadKernel readKernel(&activeBoard);
			GameOfLifeWriteCell writeCell(&nextBoard);

			for (/***/; !readKernel.IsEnd(); readKernel.Next(), writeCell.Next())
			{
				int32_t neighboursAlive = readKernel.CountSurrounding('#');

				if (readKernel.GetFocusCell() == '#')
				{
					// A light which is on stays on when 2 or 3 neighbors are on, and turns off otherwise.
					writeCell.SetFocusCell((neighboursAlive == 2 || neighboursAlive == 3) ? '#' : '.');
				}
				else
				{
					// A light which is off turns on if exactly 3 neighbors are on, and stays off otherwise.
					writeCell.SetFocusCell((neighboursAlive == 3) ? '#' : '.');
				}
			}

			SetCorners(&nextBoard);
		}

		int32_t answer = boards[iterations % 2].Count('#');

		PuzzleOutput::Submit(2015, 18, 2, answer);
	}
	MemArena_Reset();
}
