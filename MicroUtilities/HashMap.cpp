#include "HashMap.h"

static_assert(std::input_or_output_iterator<HashMapConstIterator<size_t, size_t>>);
static_assert(std::ranges::range<HashMap<size_t, size_t>&>);
