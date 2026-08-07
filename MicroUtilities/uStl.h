#pragma once

#include "MemArenaStlAllocator.h"
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>

using ustring = std::basic_string<char, std::char_traits<char>, MemArenaStlAllocator<char>>;

template <typename T>
using uvector = std::vector<T, MemArenaStlAllocator<T>>;

template <typename T>
using ulist = std::list<T, MemArenaStlAllocator<T>>;

template <typename T, typename COMPARE = std::less<T>>
using uset = std::set<T, COMPARE, MemArenaStlAllocator<T>>;

template <typename KEY, typename VALUE, typename COMPARE = std::less<KEY>>
using umap = std::map<KEY, VALUE, COMPARE, MemArenaStlAllocator<std::pair<const KEY, VALUE>>>;
