#include "SmallVector.h"
#include <ranges>

static_assert(std::input_or_output_iterator<SmallVectorConstIterator<size_t, 1>>);
static_assert(std::ranges::range<SmallVector<size_t, 1>&>);
