#include "stdafx.h"

using namespace std;

namespace Puzzle07_2017_Types
{
	struct Identifier
	{
		char Data[8] = {};
		auto operator<=>(const Identifier&) const = default;
	};

	struct Node
	{
		int8_t WaitingFor = 0;
		int8_t ContributionsFrom = 0;
		uint16_t OurWeight = 0;
		int32_t SubtreeWeight = 0;
		int32_t ChildContributions[2] = {}; // Correct child weight should always end up in slot 0
	};
}

using namespace Puzzle07_2017_Types;

template <>
struct std::hash<Identifier>
{
	size_t operator()(const Identifier& m) const noexcept
	{
		size_t hash = 0;
		for (size_t i = 0; i < 8; i++)
		{
			hash = (hash << 16) ^ (hash >> 16) ^ std::hash<uint8_t>{}(m.Data[i]);
		}
		return hash;
	}
};

static Identifier ReadIdentifier(const char* rawId)
{
	Identifier id = {};
	for (size_t i = 0; i < 8; i++)
	{
		if (isalpha(*rawId) == false)
			break;

		id.Data[i] = *rawId++;
	}
	return id;
}

void Puzzle07_A_2017()
{
	vector<Identifier> parents;
	parents.reserve(512); // 4kb

	HashSet<Identifier> children(2 * 1024, {}); // 16kb

	char line[128];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line);
		char* arrowHead = strchr(line, '>');
		if (arrowHead == nullptr)
			continue;

		parents.push_back(ReadIdentifier(line));

		char* token = strtok(arrowHead + 2, ", ");
		while (token)
		{
			children.Insert(ReadIdentifier(token));
			token = strtok(nullptr, ", ");
		}
	}

	Identifier answer = *ranges::find_if(parents, [&](const Identifier& id) { return !children.Contains(id); });

	PuzzleOutput::Submit(2017, 7, 1, answer.Data);
}


set<string> AllPermutations;

void GenerateAllPermutations(string& str, size_t activeIndex)
{
	if (activeIndex == str.size())
	{
		AllPermutations.insert(str);
		return;
	}

	for (size_t i = activeIndex; i < str.size(); i++)
	{
		swap(str[i], str[activeIndex]);
		GenerateAllPermutations(str, activeIndex + 1);
		swap(str[i], str[activeIndex]);
	}
}

void Puzzle07_B_2017()
{
	const size_t maximumNodeCount = 1280;

	vector<Node> nodes;
	nodes.reserve(maximumNodeCount); // 16 * 1280 = 20kb

	HashMap<Identifier, size_t> idToIndex(2 * 1024, {}); // (8+4) * 2 * 1024 = 24kb
	HashMap<Identifier, Identifier> childToParent(2 * 1024, {}); // (8+8) * 2 * 1024 = 32kb

	vector<Identifier> executionQueue;
	executionQueue.reserve(maximumNodeCount); // 8 * 1280 = 10kb

	// Approx 20 + 24 + 32 + 10 = 86kb

	char line[128];
	while (PuzzleInput::NextLine())
	{
		Node newNode;

		Parse::ReadNonEmptyLine(line);

		Identifier parentId = ReadIdentifier(line);
		newNode.OurWeight = Parse::GetUint16(line);

		char* arrowHead = strchr(line, '>');
		if (arrowHead)
		{
			char* token = strtok(arrowHead + 2, ", ");
			while (token)
			{
				childToParent.Insert(ReadIdentifier(token), parentId);
				newNode.WaitingFor++;
				token = strtok(nullptr, ", ");
			}
		}
		else
		{
			executionQueue.push_back(parentId);
		}

		idToIndex.Insert(parentId, nodes.size());
		nodes.push_back(newNode);
	}

	// Work out the correct child contributions for each node
	for (size_t i = 0; i < executionQueue.size(); i++)
	{
		const Identifier& childId = executionQueue[i];
		Node& child = nodes[idToIndex.At(childId)];
		child.SubtreeWeight += child.OurWeight;

		Identifier parentId;
		if (childToParent.TryFind(childId, &parentId))
		{
			Node& parent = nodes[idToIndex.At(parentId)];

			// Cases:
			// 
			// [0 0] -a-> [a 0]
			// 
			// [a 0] -b-> [a b]
			// [a b] -b-> [b a]
			// 
			// [a 0] -a-> [a 0] do nothing
			// [a b] -a-> [a b] do nothing

			if (parent.ChildContributions[0] == 0)
			{
				parent.ChildContributions[0] = child.SubtreeWeight;
			}
			else if (parent.ChildContributions[0] != child.SubtreeWeight)
			{
				if (parent.ChildContributions[1] == 0)
				{
					parent.ChildContributions[1] = child.SubtreeWeight;
				}
				else
				{
					swap(parent.ChildContributions[0], parent.ChildContributions[1]);
				}
			}

			parent.ContributionsFrom++;
			parent.SubtreeWeight += child.SubtreeWeight;
			if (--parent.WaitingFor == 0)
			{
				executionQueue.push_back(parentId);
			}
		}
	}

	// Find the mismatched child node
	int32_t answer = 0;
	for (const auto& childId : childToParent)
	{
		const Node& child = nodes[idToIndex.At(childId.first)];

		Identifier parentId;
		if (childToParent.TryFind(childId.first, &parentId))
		{
			Node& parent = nodes[idToIndex.At(parentId)];

			bool parentIsUnbalanced = (parent.ChildContributions[1] != 0);
			bool parentHasAtLeastThreeChildren = (parent.ContributionsFrom >= 3);
			bool childIsBalanced = (child.ChildContributions[1] == 0);
			bool childIsTheUnbalancedSubtree = (child.SubtreeWeight == parent.ChildContributions[1]);

			if (parentIsUnbalanced && parentHasAtLeastThreeChildren && childIsBalanced && childIsTheUnbalancedSubtree)
			{
				// Our children are balanced, we are the mismatch. Find out from the parent what our weight should be
				int32_t weightDelta = parent.ChildContributions[0] - parent.ChildContributions[1];
				answer = child.OurWeight + weightDelta;
				break;
			}
		}
	}

	PuzzleOutput::Submit(2017, 7, 2, answer);
}
