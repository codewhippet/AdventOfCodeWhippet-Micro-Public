#include "stdafx.h"

using namespace std;

namespace Puzzle14_2017_Types
{
	struct Hash
	{
		array<uint8_t, 16> Bytes = {};
	};
}

using namespace Puzzle14_2017_Types;

static Hash KnotHash(const string_view& s, uvector<int32_t>* workingBuffer)
{
	uvector<int32_t> lengths;
	lengths.reserve(s.size() + 5);
	for (char c : s)
	{
		lengths.push_back(c);
	}
	for (int32_t i : { 17, 31, 73, 47, 23 })
	{
		lengths.push_back(i);
	}

	const size_t numElements = 256;
	const size_t nodeArayMask = numElements - 1;

	uvector<int32_t>& nodes = *workingBuffer;
	assert(nodes.size() == numElements);
	iota(nodes.begin(), nodes.end(), 0);

	int32_t currentPosition = 0;
	int32_t skipSize = 0;
	for (int32_t round = 0; round < 64; round++)
	{
		for (int32_t length : lengths)
		{
			assert(length <= (int32_t)nodes.size());

			int32_t reverseBegin = currentPosition;
			int32_t reverseEnd = currentPosition + length - 1;

			while (reverseEnd > reverseBegin)
			{
				swap(nodes[reverseBegin & nodeArayMask], nodes[reverseEnd & nodeArayMask]);

				reverseBegin++;
				reverseEnd--;
			}

			currentPosition = (currentPosition + length + skipSize) & nodeArayMask;
			skipSize++;
		}
	}

	Hash denseHash;
	for (size_t i = 0; i < nodes.size(); i++)
	{
		denseHash.Bytes[i / 16] ^= static_cast<uint8_t>(nodes[i]);
	}

	return denseHash;
}

static const ustring& ByteToBinary(uint8_t byte, HashMap<uint16_t, ustring>* cache)
{
	if (!cache->Contains(byte))
	{
		ustring binary = "00000000";
		for (int i = 0; i < 8; i++)
		{
			if ((byte >> (7 - i)) & 0x1)
			{
				binary[i] = '1';
			}
		}
		cache->Insert(byte, binary);
	}

	return cache->At(byte);
}

static void ClearConnectedGroup(const Vec2Int& startFrom, uArrayMap2D* terrain)
{
	uvector<Vec2Int> searchQueue;
	searchQueue.reserve(2 * 1024);
	searchQueue.push_back(startFrom);
	(*terrain)(startFrom) = '0';

	for (size_t i = 0; i < searchQueue.size(); i++)
	{
		Vec2Int current = searchQueue[i];
		for (const Vec2Int& dir : Vec2Int::CardinalDirections())
		{
			Vec2Int neighbour = current + dir;
			if ((*terrain)(neighbour) == '1')
			{
				searchQueue.push_back(neighbour);
				(*terrain)(neighbour) = '0';
			}
		}
	}

	assert(searchQueue.capacity() <= 2 * 1024);
}

void Puzzle14_A_2017()
{
	array<MemArenaSmallBlockCount, 2> sbas =
	{
		MemArenaSmallBlockCount{ 512, 16 }
	};

	MemArenaConfig cfg{};
	cfg.SmallBlockCounts = sbas.data();
	cfg.NumSmallBlockCounts = sbas.size();
	cfg.LargeBlockRegionSize = 32 * 1024;

	MemArena_Configure(cfg);
	{
		char secret[128];
		int32_t secretSize = Parse::ReadLine(secret, sizeof(secret));
		secret[secretSize] = '-';
		char* digits = &secret[secretSize + 1];

		uvector<int32_t> knotHashWorkingBuffer(256);

		int32_t answer = 0;
		for (int32_t i = 0; i < 128; i++)
		{
			sprint_digits(digits, i);
			Hash h = KnotHash(secret, &knotHashWorkingBuffer);
			for (uint8_t byte : h.Bytes)
			{
				answer += popcount(byte);
			}
		}

		PuzzleOutput::Submit(2017, 14, 1, answer);
	}
	MemArena_Reset();
}

void Puzzle14_B_2017()
{
	array<MemArenaSmallBlockCount, 2> sbas =
	{
		MemArenaSmallBlockCount{ 512 + 64, 16 }
	};

	MemArenaConfig cfg{};
	cfg.SmallBlockCounts = sbas.data();
	cfg.NumSmallBlockCounts = sbas.size();
	cfg.LargeBlockRegionSize = 48 * 1024;

	cfg.DebugFlags = MemArenaDebugFlags::PrintOnOutOfMemory | MemArenaDebugFlags::PrintOnOverflow | MemArenaDebugFlags::BreakOnOutOfMemory;

	MemArena_Configure(cfg);
	{
		char secret[128];
		int32_t secretSize = Parse::ReadLine(secret, sizeof(secret));
		secret[secretSize] = '-';
		char* digits = &secret[secretSize + 1];

		uvector<int32_t> knotHashWorkingBuffer(256);
		HashMap<uint16_t, ustring> byteToBinaryCache(512, 256);

		uArrayMap2D terrain(Vec2Int{ 0, 0 }, 128, 128, '0');
		for (int32_t y = 0; y < 128; y++)
		{
			sprint_digits(digits, y);
			Hash h = KnotHash(secret, &knotHashWorkingBuffer);

			int32_t x = 0;
			for (int32_t byteIndex = 0; byteIndex < h.Bytes.size(); byteIndex++)
			{
				const ustring& binary = ByteToBinary(h.Bytes[byteIndex], &byteToBinaryCache);
				for (char c : binary)
				{
					terrain(x++, y) = c;
				}
			}
		}

		int32_t answer = 0;
		for (const auto& gridPos : terrain.Grid())
		{
			if (gridPos.second == '1')
			{
				ClearConnectedGroup(gridPos.first, &terrain);
				answer++;
			}
		}

		PuzzleOutput::Submit(2017, 14, 2, answer);
	}
	MemArena_Reset();
}
