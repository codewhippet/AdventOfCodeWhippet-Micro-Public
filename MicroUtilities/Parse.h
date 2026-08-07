#pragma once
#include <vector>
#include <stdint.h>

namespace Parse
{
	int8_t GetInt8();
	int16_t GetInt16();
	int32_t GetInt32();

	uint8_t GetUint8();
	uint16_t GetUint16();
	uint32_t GetUint32();

	int16_t GetInt16(const char* line);
	int32_t GetInt32(const char* line);

	uint16_t GetUint16(const char* line);
	uint32_t GetUint32(const char* line);

	bool TryGetInt16FromLine(int16_t* out);
	bool TryGetInt32FromLine(int32_t* out);

	int32_t ReadLine(char* dest, size_t bufferSize);
	int32_t ReadNonDigits(char* dest, size_t bufferSize);

	void ReadNonEmptyLine(char* dest, size_t bufferSize);

	template <size_t N>
	void ReadNonEmptyLine(char (&dest)[N])
	{
		ReadNonEmptyLine(dest, N);
	}

	void ReadAsVectorOfNumbers(const char* line, std::vector<int32_t>* out);
}
