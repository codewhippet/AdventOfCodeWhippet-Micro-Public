#pragma once
#include <vector>
#include <string>
#include <stdint.h>

namespace Parse
{
	int8_t GetInt8();
	int16_t GetInt16();
	int32_t GetInt32();
	int64_t GetInt64();

	uint8_t GetUint8();
	uint16_t GetUint16();
	uint32_t GetUint32();

	int16_t GetInt16(const char* line);
	int32_t GetInt32(const char* line);

	uint16_t GetUint16(const char* line);
	uint32_t GetUint32(const char* line);

// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------

	void ReadAsVectorOfNumbers(const char* line, std::vector<int32_t>* out);
	void ReadAllLines(std::vector<std::string>* out, size_t lineSizeHint);

// ----------------------------------------------------------------------------

	struct ReadUntilIterator
	{
		using value_type = char;
		using difference_type = ptrdiff_t;

		value_type Current;
		value_type Terminal;

		ReadUntilIterator();
		explicit ReadUntilIterator(value_type terminal);
		ReadUntilIterator(value_type current, value_type terminal);

		ReadUntilIterator& operator++();
		ReadUntilIterator operator++(int);

		const value_type& operator*() const
		{
			return Current;
		}

		const value_type* operator->() const
		{
			return &Current;
		}

	private:
		void Next();
	};

	inline bool operator==(const ReadUntilIterator& a, const ReadUntilIterator& b)
	{
		// Minimum support for comparing against end sentinel
		return a.Current == b.Current;
	}

	inline bool operator!=(const ReadUntilIterator& a, const ReadUntilIterator& b)
	{
		return !(a == b);
	}

	struct ReadUntilRange
	{
		char Terminal = 0;

		ReadUntilRange();
		explicit ReadUntilRange(char terminal);

		ReadUntilIterator begin() const;
		ReadUntilIterator end() const;
	};

	ReadUntilRange ReadUntilSeen(char terminal);
}
