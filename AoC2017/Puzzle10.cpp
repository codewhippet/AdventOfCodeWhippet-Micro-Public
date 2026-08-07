#include "stdafx.h"

using namespace std;

namespace Puzzle10_2017_Types
{
}

using namespace Puzzle10_2017_Types;

void Puzzle10_A_2017()
{
	char line[64];
	Parse::ReadNonEmptyLine(line);

	vector<int32_t> lengths;
	lengths.reserve(16);
	Parse::ReadAsVectorOfNumbers(line, &lengths);

	const size_t numElements = 256;
	const size_t nodeArayMask = numElements - 1;

	vector<int32_t> nodes(numElements);
	iota(nodes.begin(), nodes.end(), 0);

	int32_t currentPosition = 0;
	int32_t skipSize = 0;
	for (int32_t length : lengths)
	{
		assert(length <= (int32_t)nodes.size());

		int32_t reverseBegin = currentPosition;
		int32_t reverseEnd = currentPosition + length - 1;

		while (reverseEnd > reverseBegin)
		{
			swap(nodes[reverseBegin & nodeArayMask], nodes[reverseEnd & nodeArayMask]);

			reverseBegin++;
			reverseEnd--;
		}

		currentPosition = (currentPosition + length + skipSize) & nodeArayMask;
		skipSize++;
	}

	int32_t answer = nodes[0] * nodes[1];
	PuzzleOutput::Submit(2017, 10, 1, answer);
}

void Puzzle10_B_2017()
{
	char line[64];
	Parse::ReadNonEmptyLine(line);

	vector<int32_t> lengths;
	lengths.reserve(sizeof(line) + 5);
	for (char c : string_view{ line })
	{
		lengths.push_back(c);
	}
	for (int32_t i : { 17, 31, 73, 47, 23 })
	{
		lengths.push_back(i);
	}

	const size_t numElements = 256;
	const size_t nodeArayMask = numElements - 1;

	vector<int32_t> nodes(numElements);
	iota(nodes.begin(), nodes.end(), 0);

	int32_t currentPosition = 0;
	int32_t skipSize = 0;
	for (int32_t round = 0; round < 64; round++)
	{
		for (int32_t length : lengths)
		{
			assert(length <= (int32_t)nodes.size());

			int32_t reverseBegin = currentPosition;
			int32_t reverseEnd = currentPosition + length - 1;

			while (reverseEnd > reverseBegin)
			{
				swap(nodes[reverseBegin & nodeArayMask], nodes[reverseEnd & nodeArayMask]);

				reverseBegin++;
				reverseEnd--;
			}

			currentPosition = (currentPosition + length + skipSize) & nodeArayMask;
			skipSize++;
		}
	}

	vector<int32_t> denseHash(16);
	for (size_t i = 0; i < nodes.size(); i++)
	{
		denseHash[i / 16] ^= nodes[i];
	}

	const char hex[] = "0123456789abcdef";
	char answer[64] = {};
	for (size_t i = 0; i < 16; i++)
	{
		uint8_t c = denseHash[i] & 0xff;
		answer[(i * 2) + 0] = hex[c >> 4];
		answer[(i * 2) + 1] = hex[c & 0xf];
	}

	PuzzleOutput::Submit(2017, 10, 2, answer);
}
