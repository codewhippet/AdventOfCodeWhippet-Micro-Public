#include "stdafx.h"

using namespace std;

namespace Puzzle19_2015_Types
{
	struct Reaction
	{
		vector<pair<string, string>> Substitutions;
		string StartingChemical;
	};

	struct AtomicStructure
	{
		set<string> Atoms;
		multimap<string, vector<string>> Substitutions;
		map<string, set<string>> FrontSets;
		map<string, set<string>> BackSets;
		vector<string> TargetMolecule;
	};

	struct CacheKey
	{
		size_t Begin;
		size_t End;
		string Element;

		auto operator<=>(const CacheKey&) const = default;
	};
}

using namespace Puzzle19_2015_Types;

static vector<pair<string_view, string_view>> ParseReactions()
{
	vector<pair<string_view, string_view>> reactions;
	reactions.reserve(64);

	char line[64];
	for (int32_t lineSize = Parse::ReadLine(line, sizeof(line)); lineSize > 0; lineSize = Parse::ReadLine(line, sizeof(line)))
	{
		char fromBuffer[32] = { 0 };
		char toBuffer[32] = { 0 };
		int scanned = sscanf(line, "%s => %s", fromBuffer, toBuffer);
		assert(scanned == 2);
		(void)scanned;

		const char* from = strdup_memarena(fromBuffer);
		const char* to = strdup_memarena(toBuffer);

		reactions.push_back({ from, to });
	}

	return reactions;
}

static void ExecuteSingleReplacement(const vector<pair<string_view, string_view>>& reactions, const string_view& chemical, vector<array<uint8_t, 16>>* derivativeHashes)
{
	const size_t newChemicalBufferSize = 1024;
	char* newChemical = MemArena_Alloc<char*>(newChemicalBufferSize);
	for (size_t currentPos = 0; currentPos < chemical.size(); currentPos++)
	{
		for (const auto& sub : reactions)
		{
			if (strncmp(&chemical[currentPos], sub.first.data(), sub.first.size()) == 0)
			{
				strncpy(newChemical, chemical.data(), currentPos);
				newChemical[currentPos] = '\0';
				strcat(newChemical, sub.second.data());
				strcat(newChemical, chemical.data() + currentPos + sub.first.size());

				array<uint8_t, 16> hash;
				uMD5::Hash(string_view(newChemical), hash.data());

				derivativeHashes->push_back(hash);
			}
		}
	}
	MemArena_Free(newChemical);
}

static int32_t FullyReduce(const vector<pair<string_view, string_view>>& reactions, const string_view& chemical)
{
	const size_t newChemicalBufferSize = 1024;
	char* chemicalBuffers[2] =
	{
		MemArena_Alloc<char*>(newChemicalBufferSize),
		MemArena_Alloc<char*>(newChemicalBufferSize),
	};

	size_t sourceBuffer = 0;
	strcpy(chemicalBuffers[sourceBuffer], chemical.data());

	int32_t steps = 0;
	while (strlen(chemicalBuffers[sourceBuffer]) > 1)
	{
		for (const auto& sub : reactions)
		{
			size_t destBuffer = 1 - sourceBuffer;
			char* found = strstr(chemicalBuffers[sourceBuffer], sub.second.data());
			if (found)
			{
				steps++;

				size_t foundAt = distance(chemicalBuffers[sourceBuffer], found);

				strncpy(chemicalBuffers[destBuffer], chemicalBuffers[sourceBuffer], foundAt);
				chemicalBuffers[destBuffer][foundAt] = '\0';
				strcat(chemicalBuffers[destBuffer], sub.first.data());
				strcat(chemicalBuffers[destBuffer], chemicalBuffers[sourceBuffer] + foundAt + sub.second.size());

				sourceBuffer = 1 - sourceBuffer;
				break;
			}
		}
	}

	MemArena_Free(chemicalBuffers[0]);
	MemArena_Free(chemicalBuffers[1]);

	return steps;
}

void Puzzle19_A_2015()
{
	array<MemArenaSmallBlockCount, 2> sbas =
	{
		MemArenaSmallBlockCount{ 128, 4 },
		MemArenaSmallBlockCount{ 64, 16 }
	};

	MemArenaConfig cfg{};
	cfg.SmallBlockCounts = sbas.data();
	cfg.NumSmallBlockCounts = sbas.size();
	cfg.LargeBlockRegionSize = 8 * 1024;

	MemArena_Configure(cfg);
	{
		vector<pair<string_view, string_view>> reactions = ParseReactions();

		char* chemical = MemArena_Alloc<char*>(1024);
		int32_t chemicalSize = Parse::ReadLine(chemical, 1024);
		assert(chemicalSize > 0);
		(void)chemicalSize;

		vector<array<uint8_t, 16>> derivativeHashes;
		derivativeHashes.reserve(1024);
		ExecuteSingleReplacement(reactions, string_view(chemical), &derivativeHashes);

		MemArena_Free(chemical);

		ranges::sort(derivativeHashes);
		const auto [newEnd, oldEnd] = ranges::unique(derivativeHashes);

		int32_t answer = (int32_t)distance(derivativeHashes.begin(), newEnd);

		PuzzleOutput::Submit(2015, 19, 1, answer);
	}
	MemArena_Reset();
}

void Puzzle19_B_2015()
{
	array<MemArenaSmallBlockCount, 2> sbas =
	{
		MemArenaSmallBlockCount{ 128, 4 },
		MemArenaSmallBlockCount{ 64, 16 }
	};

	MemArenaConfig cfg{};
	cfg.SmallBlockCounts = sbas.data();
	cfg.NumSmallBlockCounts = sbas.size();
	cfg.LargeBlockRegionSize = 8 * 1024;

	MemArena_Configure(cfg);
	{
		vector<pair<string_view, string_view>> reactions = ParseReactions();

		char* chemical = MemArena_Alloc<char*>(1024);
		int32_t chemicalSize = Parse::ReadLine(chemical, 1024);
		assert(chemicalSize > 0);
		(void)chemicalSize;

		int32_t answer = FullyReduce(reactions, string_view(chemical));

		MemArena_Free(chemical);

		PuzzleOutput::Submit(2015, 19, 2, answer);
	}
	MemArena_Reset();
}
