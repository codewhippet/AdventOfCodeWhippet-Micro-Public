#include "stdafx.h"

using namespace std;

namespace Puzzle18_2021_Types
{
	enum Type : int32_t
	{
		Pair = -1,
		Invalid = -1024,
	};
}

using namespace Puzzle18_2021_Types;

static vector<int32_t> ParseSnailfishNumber()
{
	const size_t stableSnailfishSize = 32 + 1;
	vector<int32_t> number(stableSnailfishSize, Type::Invalid);

	int32_t current = 1;
	for (char c : Parse::ReadUntilSeen('\n'))
	{
		switch (c)
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			number[current] = c - '0';
			break;

		case '[':
			number[current] = Type::Pair;
			current *= 2;
			break;

		case ',':
			current += 1;
			break;

		case ']':
			current /= 2;
			break;
		}
	}

	return number;
}

static void GenerateVisitOrder(const vector<int32_t>& number, vector<uint32_t>* order)
{
	order->clear();

	const size_t recursionDepth = 6;
	vector<int32_t> nodeStack;
	nodeStack.reserve(recursionDepth);

	uint32_t current = 1;
	while (true)
	{
		while (number[current] == Type::Pair)
		{
			order->push_back(current);
			nodeStack.push_back(current);
			current *= 2;
		}

		order->push_back(current);

		if (nodeStack.empty())
			break;

		current = nodeStack.back();
		nodeStack.pop_back();
		current = current * 2 + 1;
	}
}

static bool TryExplode(vector<int32_t>* number, const vector<uint32_t>& visitOrder)
{
	const uint32_t unstableLayer = 4;
	for (vector<uint32_t>::const_iterator visitIt = visitOrder.begin(); visitIt != visitOrder.end(); ++visitIt)
	{
		const uint32_t current = *visitIt;
		if ((*number)[current] == Type::Pair)
		{
			const uint32_t layer = 31 - countl_zero(current);
			if (layer >= unstableLayer)
			{
				const int32_t leftChild = (*number)[current * 2];
				const int32_t rightChild = (*number)[current * 2 + 1];

				// Left
				{
					auto leftIt = find_if(make_reverse_iterator(visitIt), visitOrder.rend(),
						[&](uint32_t i)
						{
							return (*number)[i] != Type::Pair;
						});
					if (leftIt != visitOrder.rend())
					{
						(*number)[*leftIt] += leftChild;
					}
				}

				// Right
				{
					// Skip over the current pair and the next two numbers, which are the current pair's children
                    auto rightIt = find_if(visitIt + 3, visitOrder.end(),
                        [&](uint32_t i)
                        {
                            return (*number)[i] != Type::Pair;
                        });
                    if (rightIt != visitOrder.end())
                    {
                        (*number)[*rightIt] += rightChild;
                    }
				}

				(*number)[current] = 0;
				(*number)[current * 2] = Type::Invalid;
				(*number)[current * 2 + 1] = Type::Invalid;

				return true;
			}
		}
	}

	return false;
}

static bool TrySplit(vector<int32_t>* number, const vector<uint32_t>& visitOrder)
{
	for (vector<uint32_t>::const_iterator visitIt = visitOrder.begin(); visitIt != visitOrder.end(); ++visitIt)
	{
		const uint32_t current = *visitIt;
		const int32_t n = (*number)[current];
        if (n > 9)
        {
            const int32_t lo = n / 2;
            const int32_t hi = n - lo;

            (*number)[current] = Type::Pair;
            (*number)[current * 2] = lo;
            (*number)[current * 2 + 1] = hi;

            return true;
        }
	}

	return false;
}

void Reduce(vector<int32_t>* number)
{
	const size_t unstableSnailfishSize = 32 + 32 + 1;
	vector<uint32_t> visitOrder;
	visitOrder.reserve(unstableSnailfishSize);

	while (true)
	{
		GenerateVisitOrder(*number, &visitOrder);
		if (TryExplode(number, visitOrder) == false)
		{
			if (TrySplit(number, visitOrder) == false)
			{
				return;
			}
		}
	}
}

vector<int32_t> AddUnstable(const vector<int32_t>& a, const vector<int32_t>& b)
{
	const size_t unstableSnailfishSize = 32 + 32 + 1;
	vector<int32_t> c(unstableSnailfishSize, Type::Invalid);

	c[1] = Type::Pair;

	const uint32_t stableSnailfishLayers = 5;
	for (uint32_t srcLayer = 0; srcLayer < stableSnailfishLayers; srcLayer++)
	{
		const uint32_t srcLayerSize = 1 << srcLayer;
		const uint32_t srcLayerIndex = srcLayerSize;

		const uint32_t dstLayer = srcLayer + 1;
		const uint32_t dstLayerIndex = 1 << dstLayer;

		memcpy(&c[dstLayerIndex], &a[srcLayerIndex], srcLayerSize * sizeof(a[srcLayerIndex]));
		memcpy(&c[dstLayerIndex + srcLayerSize], &b[srcLayerIndex], srcLayerSize * sizeof(a[srcLayerIndex]));
	}

	return c;
}

static vector<int32_t> Add(const vector<int32_t>& a, const vector<int32_t>& b)
{
	const size_t unstableSnailfishSize = 32 + 32 + 1;
	vector<int32_t> c(unstableSnailfishSize, Type::Invalid);

	c[1] = Type::Pair;

	const uint32_t stableSnailfishLayers = 5;
	for (uint32_t srcLayer = 0; srcLayer < stableSnailfishLayers; srcLayer++)
	{
		const uint32_t srcLayerSize = 1 << srcLayer;
		const uint32_t srcLayerIndex = srcLayerSize;

		const uint32_t dstLayer = srcLayer + 1;
		const uint32_t dstLayerIndex = 1 << dstLayer;

		memcpy(&c[dstLayerIndex], &a[srcLayerIndex], srcLayerSize * sizeof(a[srcLayerIndex]));
		memcpy(&c[dstLayerIndex + srcLayerSize], &b[srcLayerIndex], srcLayerSize * sizeof(a[srcLayerIndex]));
	}

	vector<uint32_t> visitOrder;
	visitOrder.reserve(c.size());

	while (true)
	{
		GenerateVisitOrder(c, &visitOrder);
		if (TryExplode(&c, visitOrder) == false)
		{
			if (TrySplit(&c, visitOrder) == false)
			{
				return c;
			}
		}
	}

	return c;
}

static int32_t Magnitude(vector<int32_t>* number)
{
	ranges::fill(*number | views::filter([](int32_t& n) { return n < 0; }), 0);

	for (int32_t i = static_cast<int32_t>(number->size()) - 1; i > 1; i--)
	{
		const int32_t factor = (i & 1) ? 2 : 3;
		(*number)[i / 2] += factor * (*number)[i];
	}

	return (*number)[1];
}

void Puzzle18_A_2021()
{
	vector<int32_t> sum = ParseSnailfishNumber();
	while (PuzzleInput::NextLine())
	{
		sum = Add(sum, ParseSnailfishNumber());
	}

	int32_t answer = Magnitude(&sum);
	PuzzleOutput::Submit(2021, 18, 1, answer);
}

void Puzzle18_B_2021()
{
	const size_t expectedSnailfishNumbers = 100;
	vector<vector<int32_t>> snailfishNumbers;
	snailfishNumbers.reserve(expectedSnailfishNumbers);
	while (PuzzleInput::NextLine())
	{
		snailfishNumbers.push_back(ParseSnailfishNumber());
	}

	MaxValue<int32_t> largest;
	for (size_t i = 0; i < snailfishNumbers.size(); i++)
	{
		for (size_t j = 0; j < snailfishNumbers.size(); j++)
		{
			if (i == j)
				continue;

			vector<int32_t> sum = Add(snailfishNumbers[i], snailfishNumbers[j]);
			largest.Update(Magnitude(&sum));
		}
	}

	int32_t answer = largest;
	PuzzleOutput::Submit(2021, 18, 2, answer);
}
