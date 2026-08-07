#pragma once

#include "MicroUtils.h"
#include <assert.h>

#ifdef _WIN32
#include <Windows.h>
#include <vector>
#include <fstream>
#include <stdio.h>
#endif
#if defined(PICO_ON_DEVICE)
#include <algorithm>
#include <string.h>
#endif
