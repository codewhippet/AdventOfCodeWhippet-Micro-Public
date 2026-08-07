#include "stdafx.h"
#include "MemArena.h"
#include <memory>
#include <assert.h>

#define USE_MEMORY_FILL_PATTERNS _DEBUG
#define USE_LARGE_POINTERS 1
#define USE_EXTRA_DEBUG_INFO 1
#define USE_STRUCTURE_MARKERS _DEBUG
#define USE_OPTIONAL_DEBUG_LOGGING _DEBUG

// ----------------------------------------------------------------------------

class SmallBlockAllocator;
class LargeBlockAllocator;

// ----------------------------------------------------------------------------

class MemArena
{
public:
	void Initialise(const MemArenaConfig& config);
	void Reset();

	void* Alloc(size_t size);
	bool Free(void* ptr);

	void* AllocLargest(size_t* allocated);
	void ShrinkAlloc(void* ptr, size_t newSize);

	void Debug_Print();

private:

	static size_t CalculateSizeRequiredForSBA(const MemArenaConfig& config);
	static SmallBlockAllocator* InitialiseSBA(const MemArenaConfig& config, uint8_t* const regionStart, uint8_t** regionEnd);
	static size_t CalculateSizeRequiredForLBA(const MemArenaConfig& config);
	static LargeBlockAllocator* InitialiseLBA(const MemArenaConfig& config, uint8_t* regionStart, size_t regionSize);

	uint8_t* Region = nullptr;
	size_t Size = 0;
	size_t NumSBAs = 0;

	SmallBlockAllocator* SBA = nullptr;
	LargeBlockAllocator* LBA = nullptr;

	static char ZeroAlloc;

public:
#if USE_OPTIONAL_DEBUG_LOGGING
	uint32_t DebugFlags = MemArenaDebugFlags::None;
#endif
};

// ----------------------------------------------------------------------------

static MemArena Arena;
char MemArena::ZeroAlloc;

// ----------------------------------------------------------------------------

template <typename TYPE>
struct SmallPtr
{
#ifdef _WIN32
#if USE_LARGE_POINTERS
	TYPE* Get() const
	{
		return Ptr;
	}

	void Set(TYPE* ptr)
	{
		Ptr = ptr;
	}

	TYPE* Ptr;
#else
	TYPE* Get() const
	{
		if (Ptr == std::numeric_limits<int32_t>::min())
		{
			return nullptr;
		}
		else
		{
			return reinterpret_cast<TYPE*>(const_cast<char*>(reinterpret_cast<const char*>(this)) + Ptr);
		}
	}

	void Set(TYPE* ptr)
	{
		if (ptr == nullptr)
		{
			Ptr = std::numeric_limits<int32_t>::min();
		}
		else
		{
			int64_t offsetFromThis = reinterpret_cast<char*>(ptr) - reinterpret_cast<char*>(this);
			assert((offsetFromThis >= std::numeric_limits<int32_t>::min()) && (offsetFromThis <= std::numeric_limits<int32_t>::max()));
			Ptr = static_cast<int32_t>(offsetFromThis);
		}
	}

	int32_t Ptr;
#endif
#elif defined(PICO_ON_DEVICE)
	TYPE* Get() const
	{
		return Ptr;
	}

	void Set(TYPE* ptr)
	{
		Ptr = ptr;
	}

	TYPE* Ptr;
#else
#error "Unsupported platform"
#endif
};

// ----------------------------------------------------------------------------

#if defined(_DEBUG) && _DEBUG
static bool IsAlign4(size_t size)
{
	return (size & size_t(0x3)) == 0;
}

static bool IsAlign4(uint8_t* size)
{
	return (reinterpret_cast<uintptr_t>(size) & 0x3) == 0;
}
#endif

// ----------------------------------------------------------------------------

struct SmallBlockHeader
{
	SmallPtr<SmallBlockHeader> Next;
};

class SmallBlockAllocator
{
public:
	SmallBlockAllocator(size_t blockSize, size_t blockCount, uint8_t* regionBegin, size_t regionSize);

	size_t GetBlockSize() const;

	void* TryAlloc(size_t size);
	bool TryFree(void* ptr);

	bool TryShrinkAlloc(void* ptr, size_t size);

	static size_t CalculateRegionSize(size_t blockSize, size_t blockCount);

	void Debug_PrintStats();
	size_t Debug_CountFreeBlocks();

private:
	SmallBlockAllocator(const SmallBlockAllocator&) = delete;
	SmallBlockAllocator& operator=(const SmallBlockAllocator&) = delete;

#if USE_STRUCTURE_MARKERS
	uint64_t StartMarker = 0x5a11b10c00000011ull;
#endif

	int32_t BlockSize = 0;
	size_t NumBlocks = 0;

	SmallPtr<uint8_t> RegionBegin{ 0 };
	SmallPtr<uint8_t> RegionEnd{ 0 };

	SmallPtr<SmallBlockHeader> FirstFree{ 0 };

#if USE_EXTRA_DEBUG_INFO
	size_t NumBlocksFree = 0;
#endif

#if USE_STRUCTURE_MARKERS
	uint64_t StopMarker = 0x5a11b10c00000022ull;
#endif
};

SmallBlockAllocator::SmallBlockAllocator(size_t blockSize, size_t blockCount, uint8_t* regionBegin, size_t regionSize)
	: BlockSize(static_cast<int32_t>(blockSize))
	, NumBlocks(blockCount)
#if USE_EXTRA_DEBUG_INFO
	, NumBlocksFree(blockCount)
#endif
{
	assert(IsAlign4(BlockSize));
	assert(regionSize >= CalculateRegionSize(blockSize, NumBlocks));
	assert(IsAlign4(regionBegin));

#if USE_MEMORY_FILL_PATTERNS
	memset(regionBegin, 0xca, regionSize);
#endif
	RegionBegin.Set(regionBegin);
	RegionEnd.Set(RegionBegin.Get() + regionSize);

	SmallBlockHeader* prev = nullptr;
	for (int32_t i = static_cast<int32_t>(NumBlocks) - 1; i >= 0; i--)
	{
		SmallBlockHeader* current = reinterpret_cast<SmallBlockHeader*>(regionBegin + (i * std::max(blockSize, sizeof(SmallBlockHeader))));
		current->Next.Set(prev);
		prev = current;
	}
	FirstFree.Set(prev);
}

size_t SmallBlockAllocator::GetBlockSize() const
{
	return BlockSize;
}

void* SmallBlockAllocator::TryAlloc(size_t size)
{
	if (size > BlockSize)
		return nullptr;

	SmallBlockHeader* firstFree = FirstFree.Get();
	if (firstFree)
	{
		FirstFree.Set(firstFree->Next.Get());
#if USE_MEMORY_FILL_PATTERNS
		memset(firstFree, 0x55, BlockSize);
#endif
#if USE_EXTRA_DEBUG_INFO
		NumBlocksFree--;
#endif
	}
#if USE_OPTIONAL_DEBUG_LOGGING
	else if (Arena.DebugFlags & MemArenaDebugFlags::PrintOnOverflow)
	{
		printf("[MemArena] SmallBlockAllocator[%d] overflowed\n", BlockSize);
	}
#endif

	return firstFree;
}

bool SmallBlockAllocator::TryFree(void* ptr)
{
	if ((ptr < RegionBegin.Get()) || (ptr >= RegionEnd.Get()))
		return false;

#if USE_MEMORY_FILL_PATTERNS
	memset(ptr, 0xdd, BlockSize);
#endif

	SmallBlockHeader* newFree = static_cast<SmallBlockHeader*>(ptr);
	newFree->Next.Set(FirstFree.Get());
	FirstFree.Set(newFree);
#if USE_EXTRA_DEBUG_INFO
	NumBlocksFree++;
#endif

	return true;
}

bool SmallBlockAllocator::TryShrinkAlloc(void* ptr, size_t size)
{
	if ((ptr < RegionBegin.Get()) || (ptr >= RegionEnd.Get()))
		return false;

#if USE_MEMORY_FILL_PATTERNS
	uint8_t* endOfAllocation = static_cast<uint8_t*>(ptr) + size;
	memset(endOfAllocation, 0xdd, BlockSize - size);
#endif
	(void)size;

	return true;
}

size_t SmallBlockAllocator::CalculateRegionSize(size_t blockSize, size_t blockCount)
{
	return std::max(blockSize, sizeof(SmallBlockHeader)) * blockCount;
}

void SmallBlockAllocator::Debug_PrintStats()
{
#if _DEBUG && USE_EXTRA_DEBUG_INFO
	printf("  SBA [%llu][% 4d] Used: %llu  Free: %llu  Free (counted): %llu\n",
		NumBlocks,
		BlockSize,
		NumBlocks - NumBlocksFree,
		NumBlocksFree,
		Debug_CountFreeBlocks());
#elif _DEBUG && !USE_EXTRA_DEBUG_INFO
	printf("  SBA [%llu][% 4d] Free (counted): %llu\n",
		NumBlocks,
		BlockSize,
		Debug_CountFreeBlocks());
#endif
}

size_t SmallBlockAllocator::Debug_CountFreeBlocks()
{
	size_t freeBlocks = 0;
#if _DEBUG
	for (const SmallBlockHeader* current = FirstFree.Get(); current; current = current->Next.Get())
	{
		assert((static_cast<const void*>(current) >= RegionBegin.Get()) && (static_cast<const void*>(current) < RegionEnd.Get()));
		freeBlocks++;
	}
#endif
	return freeBlocks;
}

// ----------------------------------------------------------------------------

enum class LargeBlockFlags : uint32_t
{
	None = 0,
	Free = 1 << 0,
	Sentinel = 1 << 1,
};

inline constexpr LargeBlockFlags operator|(LargeBlockFlags a, LargeBlockFlags b)
{
	return static_cast<LargeBlockFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline constexpr LargeBlockFlags operator&(LargeBlockFlags a, LargeBlockFlags b)
{
	return static_cast<LargeBlockFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline constexpr LargeBlockFlags operator~(LargeBlockFlags a)
{
	return static_cast<LargeBlockFlags>(~static_cast<uint32_t>(a));
}

struct LargeBlockHeader
{
#if USE_STRUCTURE_MARKERS
	uint64_t StartMarker;
#endif

	SmallPtr<LargeBlockHeader> Prev;
	SmallPtr<LargeBlockHeader> Next;

	SmallPtr<LargeBlockHeader> NextFree;

	int32_t SizeInBlocks;
	LargeBlockFlags Flags;

#if USE_STRUCTURE_MARKERS
	uint64_t StopMarker;
#endif
};

struct LargeBlockAllocatorStats
{
	size_t SentinelBlocks;
	size_t FreeBlocks;
	size_t UsedBlocks;
	size_t TotalBlocks;

	size_t FreeBytes;
	size_t UsedBytes;
	size_t TotalBytes;
};

class LargeBlockAllocator
{
public:
	LargeBlockAllocator(uint8_t* regionBegin, size_t regionSize);

	void* TryAlloc(size_t size);
	bool TryFree(void* ptr);

	void* AllocLargest(size_t* allocated);
	void ShrinkAlloc(void* ptr, size_t size);

	void Debug_PrintStats();
	void Debug_PrintBlocks();
	LargeBlockAllocatorStats Debug_GetStats();
	size_t Debug_CountFreeBlocks();

	static size_t SizeInBlocksRoundDown(size_t sizeInBytes);
	static size_t SizeInBlocksRoundUp(size_t sizeInBytes);

private:
	LargeBlockAllocator(const LargeBlockAllocator&) = delete;
	LargeBlockAllocator& operator=(const LargeBlockAllocator&) = delete;

	std::pair<LargeBlockHeader*, LargeBlockHeader*> FindPlaceForFreeBlock(const LargeBlockHeader* block);
	static void UpdateSizeInBlocksBetweenHeaders(LargeBlockHeader* block);

#if USE_STRUCTURE_MARKERS
	uint64_t StartMarker = 0xb1ccb10c00000000ull;
#endif

	SmallPtr<LargeBlockHeader> Head;
	SmallPtr<LargeBlockHeader> Tail;

#if USE_STRUCTURE_MARKERS
	uint64_t StopMarker = 0xb1ccb10c00000001ull;
#endif
};

LargeBlockAllocator::LargeBlockAllocator(uint8_t* regionBegin, size_t regionSize)
{
	assert(IsAlign4(regionBegin));
#if USE_MEMORY_FILL_PATTERNS
	memset(regionBegin, 0xcb, regionSize);
#endif

	size_t regionSizeInBlocks = SizeInBlocksRoundDown(regionSize);
	assert(regionSizeInBlocks >= 3);

	LargeBlockHeader* startSentinel = reinterpret_cast<LargeBlockHeader*>(regionBegin);
	LargeBlockHeader* firstFree = startSentinel + 1;
	LargeBlockHeader* stopSentinel = startSentinel + regionSizeInBlocks - 1;

	startSentinel->Prev.Set(nullptr);
	startSentinel->Next.Set(firstFree);
	startSentinel->NextFree.Set(firstFree);
	startSentinel->SizeInBlocks = 0;
	startSentinel->Flags = LargeBlockFlags::Sentinel;
#if USE_STRUCTURE_MARKERS
	startSentinel->StartMarker = 0xb1ccb10c00000011ull;
	startSentinel->StopMarker = 0xb1ccb10c00000022ull;
#endif

	firstFree->Prev.Set(startSentinel);
	firstFree->Next.Set(stopSentinel);
	firstFree->NextFree.Set(stopSentinel);
	firstFree->SizeInBlocks = static_cast<int32_t>(stopSentinel - firstFree - 1);
	firstFree->Flags = LargeBlockFlags::Free;
#if USE_STRUCTURE_MARKERS
	firstFree->StartMarker = 0xb1ccb10c00000011ull;
	firstFree->StopMarker = 0xb1ccb10c00000022ull;
#endif

	stopSentinel->Prev.Set(firstFree);
	stopSentinel->Next.Set(nullptr);
	stopSentinel->NextFree.Set(nullptr);
	stopSentinel->SizeInBlocks = 0;
	stopSentinel->Flags = LargeBlockFlags::Sentinel;
#if USE_STRUCTURE_MARKERS
	stopSentinel->StartMarker = 0xb1ccb10c00000011ull;
	stopSentinel->StopMarker = 0xb1ccb10c00000022ull;
#endif

	Head.Set(startSentinel);
	Tail.Set(stopSentinel);
}

void* LargeBlockAllocator::TryAlloc(size_t size)
{
	const size_t sizeInBlocks = SizeInBlocksRoundUp(size);

	LargeBlockHeader* prevFree = nullptr;
	for (LargeBlockHeader* currentFree = Head.Get(); currentFree; currentFree = currentFree->NextFree.Get())
	{
		if (currentFree->SizeInBlocks >= sizeInBlocks)
		{
			// Block is no longer free
			currentFree->Flags = currentFree->Flags & ~LargeBlockFlags::Free;

			// Should we add in a new free block following this?
			size_t leftoverBlocks = currentFree->SizeInBlocks - sizeInBlocks;
			if (leftoverBlocks > 3) //**TODO - tune
			{
				// Set up the new block
				LargeBlockHeader* newBlock = currentFree + 1 + sizeInBlocks;
				newBlock->Next.Set(currentFree->Next.Get());
				newBlock->Prev.Set(currentFree);
				newBlock->NextFree.Set(currentFree->NextFree.Get());
				newBlock->SizeInBlocks = static_cast<int32_t>(newBlock->Next.Get() - newBlock - 1);
				newBlock->Flags = LargeBlockFlags::Free;
#if USE_STRUCTURE_MARKERS
				newBlock->StartMarker = 0xb1ccb10c00000011ull;
				newBlock->StopMarker = 0xb1ccb10c00000022ull;
#endif

				// Insert the new block into the full chain
				currentFree->Next.Get()->Prev.Set(newBlock);
				currentFree->Next.Set(newBlock);

				// Replace the current block with the new block in the free list
				prevFree->NextFree.Set(newBlock);
			}
			else
			{
				// Remove this block from the free list
				prevFree->NextFree.Set(currentFree->NextFree.Get());
			}

			UpdateSizeInBlocksBetweenHeaders(currentFree);
			currentFree->NextFree.Set(nullptr);

#if USE_MEMORY_FILL_PATTERNS
			memset(currentFree + 1, 0xcc, currentFree->SizeInBlocks * sizeof(LargeBlockHeader));
#endif

			return currentFree + 1;
		}

		prevFree = currentFree;
	}

	return nullptr;
}

bool LargeBlockAllocator::TryFree(void* ptr)
{
	LargeBlockHeader* block = static_cast<LargeBlockHeader*>(ptr) - 1;
#if USE_MEMORY_FILL_PATTERNS
	memset(ptr, 0xdd, block->SizeInBlocks * sizeof(LargeBlockHeader));
#endif

	// Find where this should go in the free block list
	auto [prev, next] = FindPlaceForFreeBlock(block);
	if (prev && next)
	{
		// The start and stop sentinels are in the free list
		bool canMergeWithLeft = (block->Prev.Get() == prev) && ((prev->Flags & LargeBlockFlags::Free) == LargeBlockFlags::Free);
		bool canMergeWithRight = (block->Next.Get() == next) && ((next->Flags & LargeBlockFlags::Free) == LargeBlockFlags::Free);

		if (canMergeWithLeft && canMergeWithRight)
		{
			//          v
			// ..<->O<->0<->O<->0<->.. [Full chain]
			// ..-->O------>O------>.. [Free list]
			// to:
			// ..<->O<--------->0<->.. [Full chain]
			// ..-->O-------------->.. [Free list]

#if USE_MEMORY_FILL_PATTERNS
			memset(block, 0xdd, sizeof(LargeBlockHeader));
#endif
			LargeBlockHeader* oneAfterNext = next->Next.Get();
			assert((oneAfterNext->Flags & LargeBlockFlags::Free) == LargeBlockFlags::None);

			prev->NextFree.Set(next->NextFree.Get());
#if USE_MEMORY_FILL_PATTERNS
			memset(next, 0xdd, sizeof(LargeBlockHeader));
#endif

			prev->Next.Set(oneAfterNext);
			oneAfterNext->Prev.Set(prev);
			UpdateSizeInBlocksBetweenHeaders(prev);
		}
		else if (canMergeWithLeft)
		{
			//          v
			// ..<->O<->0<->0<->.. [Full chain]
			// ..-->O---------->.. [Free list]
			// to:
			// ..<->O<----->0<->.. [Full chain]
			// ..-->O---------->.. [Free list]

			block->Next.Get()->Prev.Set(prev);
			prev->Next.Set(block->Next.Get());
#if USE_MEMORY_FILL_PATTERNS
			memset(block, 0xdd, sizeof(LargeBlockHeader));
#endif
			UpdateSizeInBlocksBetweenHeaders(prev);
		}
		else if (canMergeWithRight)
		{
			//          v
			// ..<->0<->0<->O<->0<->.. [Full chain]
			// ..---------->O------>.. [Free list]
			// to:
			// ..<->0<->O<----->0<->.. [Full chain]
			// ..------>O---------->.. [Free list]

			block->Flags = block->Flags | LargeBlockFlags::Free;

			LargeBlockHeader* oneAfterNext = next->Next.Get();
			assert((oneAfterNext->Flags & LargeBlockFlags::Free) == LargeBlockFlags::None);

			block->NextFree.Set(next->NextFree.Get());
			prev->NextFree.Set(block);

#if USE_MEMORY_FILL_PATTERNS
			memset(next, 0xdd, sizeof(LargeBlockHeader));
#endif

			block->Next.Set(oneAfterNext);
			oneAfterNext->Prev.Set(block);
			UpdateSizeInBlocksBetweenHeaders(block);
		}
		else
		{
			//          v
			// ..<->0<->0<->0<->.. [Full chain]
			// ..-------------->.. [Free list]
			// to:
			// ..<->0<->O<->0<->.. [Full chain]
			// ..------>O------>.. [Free list]

			block->Flags = block->Flags | LargeBlockFlags::Free;
			block->NextFree.Set(next);
			prev->NextFree.Set(block);
		}

		return true;
	}

	return false;
}

void* LargeBlockAllocator::AllocLargest(size_t* allocated)
{
	LargeBlockHeader* largestFree = Head.Get();
	for (LargeBlockHeader* currentFree = Head.Get(); currentFree; currentFree = currentFree->NextFree.Get())
	{
		if (currentFree->SizeInBlocks > largestFree->SizeInBlocks)
		{
			largestFree = currentFree;
		}
	}

	if (largestFree->SizeInBlocks == 0)
	{
		*allocated = 0;
		return nullptr;
	}

	LargeBlockHeader* prevFree = nullptr;
	for (LargeBlockHeader* currentFree = Head.Get(); currentFree; currentFree = currentFree->NextFree.Get())
	{
		if (currentFree == largestFree)
		{
			// Block is no longer free
			currentFree->Flags = currentFree->Flags & ~LargeBlockFlags::Free;

			// Remove this block from the free list
			prevFree->NextFree.Set(currentFree->NextFree.Get());

			UpdateSizeInBlocksBetweenHeaders(currentFree);
			currentFree->NextFree.Set(nullptr);

#if USE_MEMORY_FILL_PATTERNS
			memset(currentFree + 1, 0xcc, currentFree->SizeInBlocks * sizeof(LargeBlockHeader));
#endif

			*allocated = currentFree->SizeInBlocks * sizeof(LargeBlockHeader);
			return currentFree + 1;
		}

		prevFree = currentFree;
	}

	return nullptr;
}

void LargeBlockAllocator::ShrinkAlloc(void* ptr, size_t size)
{
	LargeBlockHeader* blockToShrink = static_cast<LargeBlockHeader*>(ptr) - 1;
	const size_t sizeInBlocks = SizeInBlocksRoundUp(size);
	assert(sizeInBlocks <= blockToShrink->SizeInBlocks);

#if USE_MEMORY_FILL_PATTERNS
	uint8_t* endOfAllocation = static_cast<uint8_t*>(ptr) + size;
	memset(endOfAllocation, 0xdd, (blockToShrink->SizeInBlocks * sizeof(LargeBlockHeader)) - size);
#endif

	// Is it worth creating a new free block after this?
	size_t sizeGained = blockToShrink->SizeInBlocks - sizeInBlocks;
	if (sizeGained > 3) //**TODO - Tune
	{
		LargeBlockHeader* newBlock = blockToShrink + 1 + sizeInBlocks;
		newBlock->Next.Set(blockToShrink->Next.Get());
		newBlock->Prev.Set(blockToShrink);
		newBlock->NextFree.Set(nullptr);
		newBlock->Flags = LargeBlockFlags::Free;
		UpdateSizeInBlocksBetweenHeaders(newBlock);
#if USE_STRUCTURE_MARKERS
		newBlock->StartMarker = 0xb1ccb10c00000011ull;
		newBlock->StopMarker = 0xb1ccb10c00000022ull;
#endif

		blockToShrink->Next.Set(newBlock);
		newBlock->Next.Get()->Prev.Set(newBlock);

		auto [prev, next] = FindPlaceForFreeBlock(newBlock);
		if (prev && next)
		{
			// The start and stop sentinels are in the free list
			bool canMergeWithRight = (newBlock->Next.Get() == next) && ((next->Flags & LargeBlockFlags::Free) == LargeBlockFlags::Free);

			if (canMergeWithRight)
			{
				//      v   n
				// ..<->0<->O<->O<->0<->.. [Full chain]
				// ..---------->O------>.. [Free list]
				// to:
				// ..<->0<->O<----->0<->.. [Full chain]
				// ..------>O---------->.. [Free list]

				LargeBlockHeader* oneAfterNext = next->Next.Get();
				assert((oneAfterNext->Flags & LargeBlockFlags::Free) == LargeBlockFlags::None);

				newBlock->NextFree.Set(next->NextFree.Get());
				prev->NextFree.Set(newBlock);

#if USE_MEMORY_FILL_PATTERNS
				memset(next, 0xdd, sizeof(LargeBlockHeader));
#endif

				newBlock->Next.Set(oneAfterNext);
				oneAfterNext->Prev.Set(newBlock);
				UpdateSizeInBlocksBetweenHeaders(newBlock);
			}
			else
			{
				//      v   n
				// ..<->0<->O<->0<->.. [Full chain]
				// ..---------------.. [Free list]
				// to:
				// ..<->0<->O<->0<->.. [Full chain]
				// ..------>O-------.. [Free list]

				newBlock->NextFree.Set(next);
				prev->NextFree.Set(newBlock);
			}
		}
	}

	UpdateSizeInBlocksBetweenHeaders(blockToShrink);
}

void LargeBlockAllocator::Debug_PrintStats()
{
	LargeBlockAllocatorStats stats = Debug_GetStats();
	printf("  [Blocks] Total: %llu  Free: %llu  Used: %llu  Sentinel: %llu\n",
		stats.TotalBlocks,
		stats.FreeBlocks,
		stats.UsedBlocks,
		stats.SentinelBlocks);
	printf("  [Bytes] Total: %llu  Free: %llu  Used: %llu\n",
		stats.TotalBytes,
		stats.FreeBytes,
		stats.UsedBytes);
	printf("  [FreeChain] Free: %llu\n",
		Debug_CountFreeBlocks());
}

void LargeBlockAllocator::Debug_PrintBlocks()
{
	LargeBlockHeader* prev = nullptr;
	for (LargeBlockHeader* block = Head.Get(); block; block = block->Next.Get())
	{
		assert(block->Prev.Get() == prev);
		assert(block > prev);
		assert((block->Next.Get() > block) || (block->Next.Get() == nullptr));
		assert((block->Next.Get() == (block + block->SizeInBlocks + 1)) || (block->Next.Get() == nullptr));

		const char* type = "UNKNOWN";
		if ((block->Flags & LargeBlockFlags::Sentinel) == LargeBlockFlags::Sentinel)
		{
			assert(block->SizeInBlocks == 0);
			type = "Sentinel";
		}
		else if ((block->Flags & LargeBlockFlags::Free) == LargeBlockFlags::Free)
		{
			assert(block->NextFree.Get());
			assert(block->NextFree.Get() > block);
			type = "Free";
		}
		else
		{
			assert(block->NextFree.Get() == nullptr);
			type = "Allocated";
		}
		printf("[0x%p][% 9s] % 9d blocks  % 9llu bytes\n", block, type, block->SizeInBlocks, block->SizeInBlocks * sizeof(LargeBlockHeader));

		prev = block;
	}
}

LargeBlockAllocatorStats LargeBlockAllocator::Debug_GetStats()
{
	LargeBlockAllocatorStats ret{};
	for (const LargeBlockHeader* block = Head.Get(); block; block = block->Next.Get())
	{
		ret.TotalBlocks++;
		ret.TotalBytes += block->SizeInBlocks * sizeof(LargeBlockHeader);

		if ((block->Flags & LargeBlockFlags::Sentinel) == LargeBlockFlags::Sentinel)
		{
			ret.SentinelBlocks++;
		}
		else if ((block->Flags & LargeBlockFlags::Free) == LargeBlockFlags::Free)
		{
			ret.FreeBlocks++;
			ret.FreeBytes += block->SizeInBlocks * sizeof(LargeBlockHeader);
		}
		else
		{
			ret.UsedBlocks++;
			ret.UsedBytes += block->SizeInBlocks * sizeof(LargeBlockHeader);
		}
	}
	return ret;
}

size_t LargeBlockAllocator::Debug_CountFreeBlocks()
{
	size_t freeBlocks = 0;
	for (const LargeBlockHeader* block = Head.Get(); block; block = block->NextFree.Get())
	{
		assert(((block->Flags & LargeBlockFlags::Sentinel) == LargeBlockFlags::Sentinel) ||
			((block->Flags & LargeBlockFlags::Free) == LargeBlockFlags::Free));

		if ((block->Flags & LargeBlockFlags::Free) == LargeBlockFlags::Free)
		{
			freeBlocks++;
		}
	}
	return freeBlocks;
}

size_t LargeBlockAllocator::SizeInBlocksRoundDown(size_t sizeInBytes)
{
	return sizeInBytes / sizeof(LargeBlockHeader);
}

size_t LargeBlockAllocator::SizeInBlocksRoundUp(size_t sizeInBytes)
{
	return (sizeInBytes + sizeof(LargeBlockHeader) - 1) / sizeof(LargeBlockHeader);
}

std::pair<LargeBlockHeader*, LargeBlockHeader*> LargeBlockAllocator::FindPlaceForFreeBlock(const LargeBlockHeader* block)
{
	// Find where this should go in the free block list
	LargeBlockHeader* prev = nullptr;
	for (LargeBlockHeader* next = Head.Get(); next; next = next->NextFree.Get())
	{
		if ((block > prev) && (block < next))
		{
			return { prev, next };
		}
		prev = next;
	}

	return { nullptr, nullptr };
}

void LargeBlockAllocator::UpdateSizeInBlocksBetweenHeaders(LargeBlockHeader* block)
{
	const LargeBlockHeader* next = block->Next.Get();
	assert(next > block);
	assert(((reinterpret_cast<const uint8_t*>(next) - reinterpret_cast<const uint8_t*>(block)) % sizeof(LargeBlockHeader)) == 0);
	block->SizeInBlocks = static_cast<int32_t>(next - block - 1);
}

// ----------------------------------------------------------------------------

void MemArena::Initialise(const MemArenaConfig& config)
{
#if USE_OPTIONAL_DEBUG_LOGGING
	DebugFlags = config.DebugFlags;
#endif

	size_t sbaSize = CalculateSizeRequiredForSBA(config);
	assert(IsAlign4(sbaSize));
	size_t lbaSize = CalculateSizeRequiredForLBA(config);
	assert(IsAlign4(lbaSize));

	Size = sbaSize + lbaSize;
#if _WIN32
	Region = static_cast<uint8_t*>(_aligned_malloc(Size, 4));
#elif defined(PICO_ON_DEVICE)
	Region = static_cast<uint8_t*>(malloc(Size)); //** Fingers crossed it's 4 byte aligned!
#else
#error "Unsupported platform"
#endif

	uint8_t* lbaRegionStart = nullptr;
	SBA = InitialiseSBA(config, Region, &lbaRegionStart);
	NumSBAs = config.NumSmallBlockCounts;

	LBA = InitialiseLBA(config, lbaRegionStart, lbaSize);
}

void MemArena::Reset()
{
#if _WIN32
	_aligned_free(Region);
#elif defined(PICO_ON_DEVICE)
	free(Region);
#else
#error "Unsupported platform"
#endif
}

void* MemArena::Alloc(size_t size)
{
	if (size == 0)
		return &ZeroAlloc;

	for (size_t i = 0; i < NumSBAs; i++)
	{
		void* block = SBA[i].TryAlloc(size);
		if (block)
		{
			return block;
		}
	}

	void* ptr = LBA ? LBA->TryAlloc(size) : nullptr;
	return ptr;
}

bool MemArena::Free(void* ptr)
{
	if (ptr == 0)
		return true;
	if (ptr == &ZeroAlloc)
		return true;

	for (size_t i = 0; i < NumSBAs; i++)
	{
		if (SBA[i].TryFree(ptr))
		{
			return true;
		}
	}

	bool returned = LBA ? LBA->TryFree(ptr) : false;
	assert(returned);
	(void)returned;

	return returned;
}

void* MemArena::AllocLargest(size_t* allocated)
{
	*allocated = 0;
	return LBA ? LBA->AllocLargest(allocated) : nullptr;
}

void MemArena::ShrinkAlloc(void* ptr, size_t newSize)
{
	if (ptr == 0)
		return;
	if (ptr == &ZeroAlloc)
		return;

	for (size_t i = 0; i < NumSBAs; i++)
	{
		if (SBA[i].TryShrinkAlloc(ptr, newSize))
		{
			return;
		}
	}

	assert(LBA);
	if (LBA)
	{
		LBA->ShrinkAlloc(ptr, newSize);
	}
}

void MemArena::Debug_Print()
{
	printf("-------------------------------------\n");
	printf("SBAs\n");
	for (size_t i = 0; i < NumSBAs; i++)
	{
		SBA[i].Debug_PrintStats();
	}
	printf("-------------------------------------\n");
	printf("LBA Stats\n");
	if (LBA)
	{
		LBA->Debug_PrintStats();
	}
	printf("-------------------------------------\n");
	printf("LBA Blocks\n");
	if (LBA)
	{
		LBA->Debug_PrintBlocks();
	}
	printf("-------------------------------------\n");
}

size_t MemArena::CalculateSizeRequiredForSBA(const MemArenaConfig& config)
{
	size_t regionSize = sizeof(SmallBlockAllocator) * config.NumSmallBlockCounts;
	for (size_t i = 0; i < config.NumSmallBlockCounts; i++)
	{
		const MemArenaSmallBlockCount& sbaConfig = config.SmallBlockCounts[i];
		regionSize += SmallBlockAllocator::CalculateRegionSize(sbaConfig.BlockSize, sbaConfig.NumBlocks);
	}
	return regionSize;
}

SmallBlockAllocator* MemArena::InitialiseSBA(const MemArenaConfig& config, uint8_t* const regionStart, uint8_t** regionEnd)
{
	assert(IsAlign4(regionStart));
	if (config.NumSmallBlockCounts == 0)
	{
		*regionEnd = regionStart;
		return nullptr;
	}

	SmallBlockAllocator* allocators = reinterpret_cast<SmallBlockAllocator*>(regionStart);
	uint8_t* blockStorage = regionStart + (sizeof(SmallBlockAllocator) * config.NumSmallBlockCounts);
	for (size_t i = 0; i < config.NumSmallBlockCounts; i++)
	{
		const MemArenaSmallBlockCount& sbaConfig = config.SmallBlockCounts[i];
		size_t blockRegionSize = SmallBlockAllocator::CalculateRegionSize(sbaConfig.BlockSize, sbaConfig.NumBlocks);
		new (&allocators[i]) SmallBlockAllocator(sbaConfig.BlockSize, sbaConfig.NumBlocks, blockStorage, blockRegionSize);
		blockStorage += blockRegionSize;
	}

	*regionEnd = blockStorage;
	return allocators;
}

size_t MemArena::CalculateSizeRequiredForLBA(const MemArenaConfig& config)
{
	if (config.LargeBlockRegionSize == 0)
		return 0;

	// To make the memory calculations easier, the LBA works in blocks that are always a multiple of sizeof(LargeBlockHeader)
	size_t regionSizeInBlocks = LargeBlockAllocator::SizeInBlocksRoundUp(config.LargeBlockRegionSize);

	size_t lbaSize = 0;
	lbaSize += sizeof(LargeBlockAllocator);
	lbaSize += sizeof(LargeBlockHeader); // Start sentinel
	lbaSize += sizeof(LargeBlockHeader); // First free block
	lbaSize += regionSizeInBlocks * sizeof(LargeBlockHeader);
	lbaSize += sizeof(LargeBlockHeader); // Stop sentinel
	return lbaSize;
}

LargeBlockAllocator* MemArena::InitialiseLBA(const MemArenaConfig& config, uint8_t* regionStart, size_t regionSize)
{
	if (config.LargeBlockRegionSize == 0)
		return nullptr;

	assert(IsAlign4(regionStart));
	assert(CalculateSizeRequiredForLBA(config) <= regionSize);

#if USE_MEMORY_FILL_PATTERNS
	memset(regionStart, 0xcb, regionSize);
#endif
	uint8_t* blockRegion = regionStart + sizeof(LargeBlockAllocator);
	size_t blockRegionSize = regionSize - sizeof(LargeBlockAllocator);
	LargeBlockAllocator* allocator = new (regionStart) LargeBlockAllocator(blockRegion, blockRegionSize);
	return allocator;
}

// ----------------------------------------------------------------------------

void MemArena_Configure(const MemArenaConfig& config)
{
	MemArena_Reset();
	Arena.Initialise(config);
}

void MemArena_Reset()
{
	Arena.Reset();
	Arena = {};
}

void* MemArena_Alloc(size_t size)
{
	void* ptr = Arena.Alloc(size);

#if USE_OPTIONAL_DEBUG_LOGGING
	if ((ptr == nullptr) && (Arena.DebugFlags & MemArenaDebugFlags::PrintOnOutOfMemory))
	{
		MemArena_DebugPrint();
		if (Arena.DebugFlags & MemArenaDebugFlags::BreakOnOutOfMemory)
		{
#if _WIN32
			__debugbreak();
#endif
		}
	}
#endif
	return ptr;
}

void MemArena_Free(void* ptr)
{
	Arena.Free(ptr);
}

void* MemArena_AllocLargest(size_t* allocated)
{
	return Arena.AllocLargest(allocated);
}

void MemArena_ShrinkAlloc(void* ptr, size_t newSize)
{
	Arena.ShrinkAlloc(ptr, newSize);
}

void MemArena_UnitTest()
{
#if 0
	std::vector<MemArenaSmallBlockCount> sbas =
	{
		{ 4, 4 },
		{ 4, 8 },
		{ 4, 16 },
		{ 4, 32 },
	};

	MemArenaConfig cfg = {};
	cfg.SmallBlockCounts = sbas.data();
	cfg.NumSmallBlockCounts = sbas.size();
	cfg.LargeBlockRegionSize = 0;

	MemArena_Configure(cfg);
	Arena.Debug_Print();

	std::vector<void*> ptrs;
	ptrs.push_back(MemArena_Alloc(1));
	Arena.Debug_Print();
	ptrs.push_back(MemArena_Alloc(1));
	Arena.Debug_Print();
	ptrs.push_back(MemArena_Alloc(1));
	Arena.Debug_Print();
	ptrs.push_back(MemArena_Alloc(1));
	Arena.Debug_Print();
	ptrs.push_back(MemArena_Alloc(1));
	Arena.Debug_Print();

	MemArena_Free(ptrs[2]);
	Arena.Debug_Print();
	MemArena_Free(ptrs[1]);
	Arena.Debug_Print();
	MemArena_Free(ptrs[4]);
	Arena.Debug_Print();
	MemArena_Free(ptrs[3]);
	Arena.Debug_Print();

#elif 0
	MemArenaConfig cfg = {};
	cfg.SmallBlockCounts = nullptr;
	cfg.NumSmallBlockCounts = 0;
	cfg.LargeBlockRegionSize = 32 * 1024;

	MemArena_Configure(cfg);
	Arena.Debug_Print();

	std::vector<void*> ptrs;
	ptrs.push_back(MemArena_Alloc(256));
	Arena.Debug_Print();
	ptrs.push_back(MemArena_Alloc(256));
	Arena.Debug_Print();
	size_t allocated = 0;
	ptrs.push_back(MemArena_AllocLargest(&allocated));
	Arena.Debug_Print();

	MemArena_ShrinkAlloc(ptrs[2], 16 * 1024);
	Arena.Debug_Print();

	MemArena_ShrinkAlloc(ptrs[2], 8 * 1024);
	Arena.Debug_Print();

	MemArena_ShrinkAlloc(ptrs[1], 250);
	Arena.Debug_Print();

	for (const auto& ptr : ptrs)
	{
		MemArena_Free(ptr);
		Arena.Debug_Print();
	}
#endif
}

void MemArena_Fuzz()
{
	const int arenaTests = 100;
	const int allocationsPerArena = 1000;

	const size_t maxActiveAllocations = 64;
	const size_t maxAllocation = 512;

	const size_t maxBlocksPerSBA = 8;
	const size_t maxLBASize = 1024 * 1024;

	std::vector<MemArenaSmallBlockCount> referenceSBAs =
	{
		{ 1, 4 },
		{ 1, 8 },
		{ 1, 12 },
		{ 1, 16 },
		{ 1, 24 },
		{ 1, 32 },
		{ 1, 40 },
		{ 1, 48 },
		{ 1, 56 },
		{ 1, 64 },
		{ 1, 72 },
		{ 1, 80 },
		{ 1, 88 },
		{ 1, 96 },
		{ 1, 128 },
		{ 1, 192 },
	};

	for (int arena = 0; arena < arenaTests; arena++)
	{
		printf("Arena configuration %d...\n", arena);

		size_t numSbas = rand() % referenceSBAs.size();
		std::vector<MemArenaSmallBlockCount> sbas = referenceSBAs;
		if (numSbas == 0)
		{
			sbas.clear();
		}
		else
		{
			while (sbas.size() > numSbas)
			{
				sbas.erase(sbas.begin() + (rand() % (sbas.size() - 1)));
			}
			for (MemArenaSmallBlockCount& count : sbas)
			{
				count.NumBlocks = rand() % maxBlocksPerSBA;
			}
		}

		MemArenaConfig cfg = {};
		cfg.SmallBlockCounts = sbas.data();
		cfg.NumSmallBlockCounts = sbas.size();
		cfg.LargeBlockRegionSize = rand() % maxLBASize;

		MemArena_Configure(cfg);
		MemArena_DebugPrint();

		struct Alloc
		{
			char* Ptr = nullptr;
			size_t Size = 0;
			char Fill = '\0';
		};

		std::vector<Alloc> activeAllocations(maxActiveAllocations);
		for (int allocation = 0; allocation < allocationsPerArena; allocation++)
		{
			size_t allocIndex = rand() % activeAllocations.size();
			Alloc& alloc = activeAllocations[allocIndex];
			if (alloc.Ptr)
			{
				for (int i = 0; i < alloc.Size; i++)
				{
					assert(alloc.Ptr[i] == alloc.Fill);
				}

				if (alloc.Size == 0)
				{
					printf("[%d] Deallocating %llu\n", allocation, alloc.Size);
					MemArena_Free(alloc.Ptr);
					alloc = {};
				}
				else if (rand() % 2)
				{
					printf("[%d] Deallocating %llu\n", allocation, alloc.Size);
					MemArena_Free(alloc.Ptr);
					alloc = {};
				}
				else
				{
					size_t newSize = rand() % alloc.Size;
					printf("[%d] Shrinking %llu -> %llu\n", allocation, alloc.Size, newSize);
					MemArena_ShrinkAlloc(alloc.Ptr, newSize);
					alloc.Size = newSize;
				}
			}
			else
			{
				if (rand() % 2)
				{
					alloc.Size = rand() % maxAllocation;
					alloc.Ptr = static_cast<char*>(MemArena_Alloc(alloc.Size));
					if (alloc.Ptr)
					{
						printf("[%d] Allocated %llu\n", allocation, alloc.Size);

						alloc.Fill = rand() & 0xff;
						memset(alloc.Ptr, alloc.Fill, alloc.Size);
					}
					else
					{
						printf("[%d] Failed to allocate %llu\n", allocation, alloc.Size);
					}
				}
				else
				{
					alloc.Ptr = static_cast<char*>(MemArena_AllocLargest(&alloc.Size));
					if (alloc.Ptr)
					{
						printf("[%d] Allocated largest %llu\n", allocation, alloc.Size);
						assert(alloc.Size != 0);
						alloc.Fill = rand() & 0xff;
						memset(alloc.Ptr, alloc.Fill, alloc.Size);

						alloc.Size = rand() % alloc.Size;
						printf("[%d] Shrinking to target size %llu\n", allocation, alloc.Size);
						MemArena_ShrinkAlloc(alloc.Ptr, alloc.Size);
					}
					else
					{
						printf("[%d] Failed to allocate %llu\n", allocation, alloc.Size);
					}
				}
			}

			MemArena_DebugPrint();
		}

		for (const auto& alloc : activeAllocations)
		{
			MemArena_Free(alloc.Ptr);
			MemArena_DebugPrint();
		}
	}
}

void MemArena_DebugPrint()
{
	Arena.Debug_Print();
}
