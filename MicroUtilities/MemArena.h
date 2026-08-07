#pragma once

#include <utility>

enum MemArenaDebugFlags : uint32_t
{
	None = 0,

	PrintOnOutOfMemory = 1 << 0,
	PrintOnOverflow = 1 << 1,

	BreakOnOutOfMemory = 1 << 8,
};

struct MemArenaSmallBlockCount
{
	size_t NumBlocks;
	size_t BlockSize;
};

struct MemArenaConfig
{
	const MemArenaSmallBlockCount* SmallBlockCounts = nullptr;
	size_t NumSmallBlockCounts = 0;
	size_t LargeBlockRegionSize = 0;

	uint32_t DebugFlags = MemArenaDebugFlags::None;
};

void MemArena_Configure(const MemArenaConfig& config);
void MemArena_Reset();

void* MemArena_Alloc(size_t size);
void MemArena_Free(void* ptr);

void* MemArena_AllocLargest(size_t* allocated);
void MemArena_ShrinkAlloc(void* ptr, size_t newSize);

void MemArena_UnitTest();
void MemArena_Fuzz();
void MemArena_DebugPrint();

template <typename TYPE>
TYPE MemArena_Alloc(size_t size)
{
	return static_cast<TYPE>(MemArena_Alloc(size));
}
