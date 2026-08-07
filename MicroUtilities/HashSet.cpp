#include "HashSet.h"

static_assert(std::input_or_output_iterator<HashSetConstIterator<size_t>>);
static_assert(std::ranges::range<HashSet<size_t>&>);
