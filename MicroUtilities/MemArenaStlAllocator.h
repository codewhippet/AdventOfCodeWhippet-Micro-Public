#pragma once

#include "MemArena.h"
#include <stdint.h>

template <typename T>
struct MemArenaStlAllocator
{
	typedef T value_type;

	MemArenaStlAllocator() = default;

	template <typename U>
	MemArenaStlAllocator(const MemArenaStlAllocator<U>&) {}

	T* allocate(std::size_t n)
	{
		return MemArena_Alloc<T *>(n * sizeof(T));
	}

	void deallocate(T* ptr, std::size_t)
	{
		MemArena_Free(ptr);
	}
};

template<typename T1, typename T2>
bool operator==(const MemArenaStlAllocator<T1>&, const MemArenaStlAllocator<T2>&)
{
	return true;
}

template<typename T1, typename T2>
bool operator!=(const MemArenaStlAllocator<T1>&, const MemArenaStlAllocator<T2>&)
{
	return false;
}
