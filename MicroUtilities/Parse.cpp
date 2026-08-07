#include "Parse.h"
#include "Parse.h"
#include "stdafx.h"
#include <assert.h>

template <typename INT_TYPE>
static INT_TYPE GetInt()
{
	bool wasMinus = false;
	while (!isdigit(PuzzleInput::PeekChar()))
	{
		assert(PuzzleInput::PeekChar() != EOF);
		wasMinus = (PuzzleInput::GetChar() == '-');
	}

	INT_TYPE result = 0;
	while (isdigit(PuzzleInput::PeekChar()))
	{
		result = (result * 10) + (INT_TYPE)(PuzzleInput::GetChar() - '0');
	}
	return wasMinus ? -result : result;
}

template <typename UINT_TYPE>
static UINT_TYPE GetUint()
{
	while (!isdigit(PuzzleInput::PeekChar()))
	{
		assert(PuzzleInput::PeekChar() != EOF);
		PuzzleInput::DropChar();
	}

	UINT_TYPE result = 0;
	while (isdigit(PuzzleInput::PeekChar()))
	{
		result = (result * 10) + (UINT_TYPE)(PuzzleInput::GetChar() - '0');
	}
	return result;
}

int8_t Parse::GetInt8()
{
	return GetInt<int8_t>();
}

int16_t Parse::GetInt16()
{
	return GetInt<int16_t>();
}

int32_t Parse::GetInt32()
{
	return GetInt<int32_t>();
}

uint8_t Parse::GetUint8()
{
	return GetUint<int8_t>();
}

uint16_t Parse::GetUint16()
{
	return GetUint<int16_t>();
}

uint32_t Parse::GetUint32()
{
	return GetUint<int32_t>();
}

template <typename INT_TYPE>
static INT_TYPE GetInt(const char* line)
{
	bool wasMinus = false;
	while (!isdigit(*line))
	{
		assert(*line != '\0');
		wasMinus = (*line++ == '-');
	}

	INT_TYPE result = 0;
	while (isdigit(*line))
	{
		result = (result * 10) + (INT_TYPE)(*line++ - '0');
	}
	return wasMinus ? -result : result;
}

template <typename INT_TYPE>
static INT_TYPE GetUint(const char* line)
{
	while (!isdigit(*line))
	{
		assert(*line != '\0');
		*line++;
	}

	INT_TYPE result = 0;
	while (isdigit(*line))
	{
		result = (result * 10) + (INT_TYPE)(*line++ - '0');
	}
	return result;
}

int16_t Parse::GetInt16(const char* line)
{
	return GetInt<int16_t>(line);
}

int32_t Parse::GetInt32(const char* line)
{
	return GetInt<int32_t>(line);
}

uint16_t Parse::GetUint16(const char* line)
{
	return GetUint<int16_t>(line);
}

uint32_t Parse::GetUint32(const char* line)
{
	return GetUint<int32_t>(line);
}

template <typename INT_TYPE>
static bool TryGetIntFromLine(INT_TYPE* out)
{
	bool wasMinus = false;
	while (!isdigit(PuzzleInput::PeekChar()))
	{
		int c = PuzzleInput::GetChar();
		if ((c == '\n') || (c == EOF))
			return false;

		wasMinus = (c == '-');
	}

	INT_TYPE result = 0;
	while (isdigit(PuzzleInput::PeekChar()))
	{
		result = (result * 10) + (INT_TYPE)(PuzzleInput::GetChar() - '0');
	}

	*out = wasMinus ? -result : result;
	return true;
}

bool Parse::TryGetInt16FromLine(int16_t* out)
{
	return TryGetIntFromLine<int16_t>(out);
}

bool Parse::TryGetInt32FromLine(int32_t* out)
{
	return TryGetIntFromLine<int32_t>(out);
}

int32_t Parse::ReadLine(char* dest, size_t bufferSize)
{
	if (bufferSize == 0)
		return -1;

	// Try to fill the buffer
	int32_t charsWritten = 0;
	for (size_t i = 0; i < bufferSize; i++)
	{
		int c = PuzzleInput::GetChar();
		if ((c == '\n') || (c == EOF))
		{
			dest[i] = '\0';
			break;
		}

		dest[i] = (char)c;
		charsWritten++;
	}

	// Fail if we couldn't fit the terminating null character in
	assert((charsWritten == bufferSize) || (dest[charsWritten] == '\0'));
	return (charsWritten < bufferSize) ? charsWritten : -1;
}

int32_t Parse::ReadNonDigits(char* dest, size_t bufferSize)
{
	if (bufferSize == 0)
		return -1;

	// Try to fill the buffer
	int32_t charsWritten = 0;
	for (size_t i = 0; i < bufferSize; i++)
	{
		if (isdigit(PuzzleInput::PeekChar()))
		{
			dest[i] = '\0';
			break;
		}

		int c = PuzzleInput::GetChar();
		if ((c == '\n') || (c == EOF))
		{
			dest[i] = '\0';
			break;
		}

		dest[i] = (char)c;
		charsWritten++;
	}

	// Fail if we couldn't fit the terminating null character in
	return (dest[charsWritten] == '\0') ? charsWritten : -1;
}

void Parse::ReadNonEmptyLine(char* dest, size_t bufferSize)
{
	int32_t lineLength = ReadLine(dest, bufferSize);
	assert(lineLength > 0);
	(void)lineLength;
}

void Parse::ReadAsVectorOfNumbers(const char* line, std::vector<int32_t>* out)
{
	while (line)
	{
		bool wasMinus = false;
		while (*line && !isdigit(*line))
		{
			wasMinus = (*line++ == '-');
		}

		if (*line == '\0')
			break;

		int32_t result = 0;
		while (isdigit(*line))
		{
			result = (result * 10) + static_cast<int32_t>(*line++ - '0');
		}
		out->push_back(wasMinus ? -result : result);
	}
}
