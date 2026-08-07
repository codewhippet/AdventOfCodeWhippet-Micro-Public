#include "stdafx.h"

using namespace std;

namespace Puzzle14_2018_Types
{
	struct ReadHead
	{
		size_t StorageIndex = 0;
	};

	struct WriteHead
	{
		size_t SequenceIndex = 0;
		size_t NextStoreAt = 0;
	};

	enum : size_t
	{
		MAGIC_INDEX = 23
	};

	class ScoreStorage
	{
	public:
		ScoreStorage(int8_t* storage, size_t storageSize)
			: Storage(storage)
			, StorageEnd(storage)
			, StorageSize(storageSize)
		{
		}

		void Store(int8_t value)
		{
			if (WriteAt.SequenceIndex++ == WriteAt.NextStoreAt)
			{
				assert((StorageEnd - Storage) < StorageSize);
				*StorageEnd++ = value;
				WriteAt.NextStoreAt += (WriteAt.SequenceIndex > MAGIC_INDEX ? value : 0) + 1;
			}
		}

		int8_t Fetch(ReadHead* readAt, size_t index)
		{
			readAt->StorageIndex = (index > MAGIC_INDEX ? readAt->StorageIndex + 1 : index);
			return Storage[readAt->StorageIndex];
		}

		size_t NumScoresStored() const
		{
			return WriteAt.SequenceIndex;
		}

	private:
		int8_t* Storage;
		int8_t* StorageEnd;
		ptrdiff_t StorageSize;

		WriteHead WriteAt;
	};

	template <size_t N>
	class RecentScores
	{
	public:
		RecentScores()
		{
			// Make sure we have an impossible pattern
			ranges::fill(Ring, int8_t(10));
		}

		void Add(int8_t score)
		{
			RingIndex = (RingIndex + 1) & (N - 1);
			Ring[RingIndex] = score;
		}

		bool HasSeen(const array<int8_t, 6>& lookingFor) const
		{
			if (Ring[RingIndex] != lookingFor.back())
				return false;

			for (size_t i = 0; i < 5; i++)
			{
				if (Ring[(RingIndex + (N - 5) + i) & (N - 1)] != lookingFor[i])
				{
					return false;
				}
			}

			return true;
		}

		int64_t AsNumber() const
		{
			int64_t answer = 0;
			for (size_t i = 0; i < 10; i++)
			{
				answer *= 10;
				answer += Ring[(RingIndex + (N - 9) + i) & (N - 1)];
			}
			return answer;
		}

	private:
		array<int8_t, N> Ring;
		size_t RingIndex = 0;
	};
}

using namespace Puzzle14_2018_Types;

static int64_t FindFirstAppearanceOf(const array<int8_t, 6>& lookingFor)
{
	ScoreStorage storage(static_cast<int8_t*>(Hardware::PsramBase()), Hardware::PsramSize());
	storage.Store(3);
	storage.Store(7);

	size_t elf1 = 0;
	size_t elf2 = 1;
	ReadHead elf1ReadHead{ elf1 };
	ReadHead elf2ReadHead{ elf2 };

	RecentScores<8> haystack;

	while (true)
	{
		uint8_t e1 = storage.Fetch(&elf1ReadHead, elf1);
		uint8_t e2 = storage.Fetch(&elf2ReadHead, elf2);
		int8_t recipeSum = e1 + e2;

		if (recipeSum > 9)
		{
			storage.Store(1);
			haystack.Add(1);

			if (haystack.HasSeen(lookingFor))
			{
				return storage.NumScoresStored() - lookingFor.size();
			}
		}

		int8_t recipeUnits = recipeSum % 10;
		storage.Store(recipeUnits);
		haystack.Add(recipeUnits);

		if (haystack.HasSeen(lookingFor))
		{
			return storage.NumScoresStored() - lookingFor.size();
		}

		elf1 = (elf1 + 1 + e1) % storage.NumScoresStored();
		elf2 = (elf2 + 1 + e2) % storage.NumScoresStored();
	}

	return -1;
}

void Puzzle14_A_2018()
{
	const uint32_t improveAfter = Parse::GetUint32();

	size_t elf1 = 0;
	size_t elf2 = 1;

	vector<int8_t> data(192 * 1024);
	ScoreStorage storage(data.data(), data.size());
	storage.Store(3);
	storage.Store(7);

	ReadHead elf1Ptr{ 0 };
	ReadHead elf2Ptr{ 1 };

	RecentScores<16> recentScores;

	while (storage.NumScoresStored() < improveAfter + 10)
	{
		uint8_t e1 = storage.Fetch(&elf1Ptr, elf1);
		uint8_t e2 = storage.Fetch(&elf2Ptr, elf2);
		uint8_t recipeSum = e1 + e2;

		if (recipeSum > 9)
		{
			storage.Store(1);
			recentScores.Add(1);
		}
		uint8_t recipeSumUnits = recipeSum % 10;
		storage.Store(recipeSumUnits);
		recentScores.Add(recipeSumUnits);

		elf1 = (elf1 + 1 + e1) % storage.NumScoresStored();
		elf2 = (elf2 + 1 + e2) % storage.NumScoresStored();
	}

	int64_t answer = recentScores.AsNumber();

	PuzzleOutput::Submit(2018, 14, 1, answer);
}

void Puzzle14_B_2018()
{
	const size_t psramNeeded = 5 * 1024 * 1024;
	if (Hardware::PsramSize() >= psramNeeded)
	{
		char line[16];
		Parse::ReadNonEmptyLine(line);
		string lookingForAsString{ line };

		array<int8_t, 6> lookingFor;
		ranges::copy(lookingForAsString | views::transform([](char c) { return (int8_t)(c - '0'); }), lookingFor.begin());

		int64_t answer = FindFirstAppearanceOf(lookingFor);

		PuzzleOutput::Submit(2018, 14, 2, answer);
	}
	else
	{
		PuzzleOutput::Submit(2018, 14, 2, "[UNSUPPORTED]");
	}
}
