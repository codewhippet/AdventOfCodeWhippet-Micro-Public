#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle08_2018_Types
{
}

using namespace Puzzle08_2018_Types;

static int32_t MetadataSum()
{
	struct Execution
	{
		int32_t ChildCount = 0;
		int32_t MetadataCount = 0;
	};

	vector<Execution> executionStack;
	executionStack.reserve(16);
	executionStack.push_back({ -1, -1 });

	int32_t metadataSum = 0;
	while (executionStack.empty() == false)
	{
		Execution& topOfStack = executionStack.back();
		if (topOfStack.ChildCount == -1)
		{
			topOfStack.ChildCount = Parse::GetInt32();
			topOfStack.MetadataCount = Parse::GetInt32();
		}
		else if (topOfStack.ChildCount == 0)
		{
			while (topOfStack.MetadataCount--)
			{
				metadataSum += Parse::GetInt32();
			}
			executionStack.pop_back();
		}
		else
		{
			topOfStack.ChildCount--;
			executionStack.push_back({ -1, -1 });
		}
	}

	return metadataSum;
}

static int32_t RootNodeValue()
{
	struct Execution
	{
		int32_t NumChildren = 0;
		size_t StoreValueAt = 0;

		int32_t MetadataCount = 0;

		size_t ChildValueOffset = 0;
		int32_t ChildIndex = 0;
	};

	vector<int32_t> nodeValues;
	nodeValues.reserve(64);

	vector<Execution> executionStack;
	executionStack.reserve(16);

	executionStack.push_back({ -1, nodeValues.size() });
	nodeValues.push_back(0);

	while (executionStack.empty() == false)
	{
		Execution& topOfStack = executionStack.back();
		if (topOfStack.NumChildren == -1)
		{
			topOfStack.NumChildren = Parse::GetInt32();
			topOfStack.MetadataCount = Parse::GetInt32();
			topOfStack.ChildValueOffset = nodeValues.size();
			nodeValues.insert(nodeValues.end(), topOfStack.NumChildren, -1);
		}
		else if (topOfStack.NumChildren == 0)
		{
			int32_t nodeValue = 0;
			while (topOfStack.MetadataCount--)
			{
				nodeValue += Parse::GetInt32();
			}
			nodeValues[topOfStack.StoreValueAt] = nodeValue;

			executionStack.pop_back();
		}
		else if (topOfStack.ChildIndex == topOfStack.NumChildren)
		{
			int32_t nodeValue = 0;
			while (topOfStack.MetadataCount--)
			{
				int32_t childIndex = Parse::GetInt32();
				if ((childIndex > 0) && (childIndex <= topOfStack.NumChildren))
				{
					nodeValue += nodeValues[topOfStack.ChildValueOffset + childIndex - 1];
				}
			}
			nodeValues[topOfStack.StoreValueAt] = nodeValue;
			nodeValues.resize(nodeValues.size() - topOfStack.NumChildren);

			executionStack.pop_back();
		}
		else
		{
			executionStack.push_back({ -1, topOfStack.ChildValueOffset + topOfStack.ChildIndex });

			topOfStack.ChildIndex++;
		}
	}

	return nodeValues[0];
}

void Puzzle08_A_2018()
{
	int32_t answer = MetadataSum();
	PuzzleOutput::Submit(2018, 8, 1, answer);
}

void Puzzle08_B_2018()
{
	int32_t answer = RootNodeValue();
	PuzzleOutput::Submit(2018, 8, 2, answer);
}
