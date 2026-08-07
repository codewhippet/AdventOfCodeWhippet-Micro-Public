#include "stdafx.h"
#include <assert.h>
#ifdef PICO_ON_DEVICE
#include <pico/divider.h>
#endif

char* strdup_memarena(const char* str)
{
	return strcpy(static_cast<char*>(MemArena_Alloc(strlen(str) + 1)), str);
}

int32_t sprint_digits(char* dest, int32_t value)
{
#ifdef PICO_ON_DEVICE
	char* d = dest;
	do
	{
		int32_t rem;
		value = divmod_s32s32_rem(value, 10, &rem);
		*d++ = static_cast<char>(rem + '0');

	} while (value);

	ptrdiff_t digitsWritten = d - dest;

	*d-- = '\0';
	while (d > dest)
	{
		std::swap(*d--, *dest++);
	}

	return static_cast<int>(digitsWritten);
#else
	char* d = dest;
	do
	{
		*d++ = static_cast<char>((value % 10) + '0');
		value /= 10;
	} while (value);

	ptrdiff_t digitsWritten = d - dest;

	*d-- = '\0';
	while (d > dest)
	{
		std::swap(*d--, *dest++);
	}

	return static_cast<int32_t>(digitsWritten);
#endif
}

void sprint_digits(char* dest, int32_t value, size_t numDigits)
{
#ifdef PICO_ON_DEVICE
	char* d = dest + numDigits;
	*d-- = '\0';

	do
	{
		int32_t rem;
		value = divmod_s32s32_rem(value, 10, &rem);
		*d-- = static_cast<char>(rem + '0');

	} while (value);

	assert(d == dest - 1);
#else
	char* d = dest + numDigits;
	*d-- = '\0';

	do
	{
		*d-- = static_cast<char>((value % 10) + '0');
		value /= 10;

	} while (value);

	assert(d == dest - 1);
#endif
}
