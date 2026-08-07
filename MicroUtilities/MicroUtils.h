#pragma once

#include "uArrayMap2D.h"
#include "PuzzleInput.h"
#include "PuzzleOutput.h"
#include "Parse.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Vec2Iteration.h"
#include "uMD5.h"
#include "MemArena.h"
#include "MemArenaStlAllocator.h"
#include "uStl.h"
#include "Hardware.h"
#include "Iteration.h"
#include "HashSet.h"
#include "HashMap.h"
#include "Hex.h"
#include "Counting.h"
#include "SmallVector.h"

char* strdup_memarena(const char* tempBuffer);
int32_t sprint_digits(char* dest, int32_t value);
void sprint_digits(char* dest, int32_t value, size_t numDigits);
