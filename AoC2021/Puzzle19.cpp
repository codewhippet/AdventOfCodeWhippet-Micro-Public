#include "stdafx.h"

using namespace std;

namespace Puzzle19_2021_Types
{
	struct Scanner
	{
		vector<Vec3Int> Beacons;
	};
}

using namespace Puzzle19_2021_Types;

template <>
struct std::hash<pair<int32_t, int32_t>>
{
	size_t operator()(const pair<int32_t, int32_t>& p) const noexcept
	{
		uint32_t hash = 0x811c9dc5;

		hash ^= p.first;
		hash *= 0x01000193;
		hash ^= p.second;
		hash *= 0x01000193;

		return hash;
	}
};

template <>
struct std::hash<pair<uint32_t, uint32_t>>
{
	size_t operator()(const pair<uint32_t, uint32_t>& p) const noexcept
	{
		uint32_t hash = 0x811c9dc5;

		hash ^= p.first;
		hash *= 0x01000193;
		hash ^= p.second;
		hash *= 0x01000193;

		return hash;
	}
};

static vector<Scanner> ReadPuzzle()
{
	const size_t expectedMaxScanners = 40;
	const size_t expectedMaxBeacons = 28;

	vector<Scanner> scanners;
	scanners.reserve(expectedMaxScanners);
	while (PuzzleInput::NextLine())
	{
		Parse::DiscardExpected("---");
		PuzzleInput::DropLine();
		Parse::DiscardExpected("\n");

		vector<Vec3Int> beacons;
		beacons.reserve(expectedMaxBeacons);
		while (PuzzleInput::IsEmptyLine() == false)
		{
			Vec3Int beacon{ Parse::GetInt32(), Parse::GetInt32(), Parse::GetInt32() };
			Parse::DiscardExpected("\n");

			beacons.push_back(beacon);
		}

		scanners.push_back({ move(beacons) });
	}
	return scanners;
}

static vector<Mat43Int> CreateRotations()
{
	vector<Mat43Int> rotations;

	const array<Vec3Int, 6> basisVectors =
	{
		Vec3Int{  1,  0,  0 },
		Vec3Int{  0,  1,  0 },
		Vec3Int{  0,  0,  1 },

		Vec3Int{ -1,  0,  0 },
		Vec3Int{  0, -1,  0 },
		Vec3Int{  0,  0, -1 },
	};

	for (int32_t i = 0; i < static_cast<int32_t>(basisVectors.size()); i++)
	{
		for (int32_t j = 0; j < static_cast<int32_t>(basisVectors.size()); j++)
		{
			// Ignore same and parallel basis vectors
			if ((i == j) || (abs(i - j) == 3))
			{
				continue;
			}

			Mat43Int m = Matrix::MakeRotation(basisVectors[i], basisVectors[j], Cross(basisVectors[i], basisVectors[j]));
			rotations.push_back(m);
		}
	}

	return rotations;
}

static int32_t FindRotationIndex(const Vec3Int& u, const Vec3Int& v)
{
	if (v == Vec3Int{  u.X,  u.Y,  u.Z }) return  0;
	if (v == Vec3Int{  u.X, -u.Z,  u.Y }) return  1;
	if (v == Vec3Int{  u.X, -u.Y, -u.Z }) return  2;
	if (v == Vec3Int{  u.X,  u.Z, -u.Y }) return  3;
	if (v == Vec3Int{  u.Y,  u.X, -u.Z }) return  4;
	if (v == Vec3Int{  u.Z,  u.X,  u.Y }) return  5;
	if (v == Vec3Int{ -u.Y,  u.X,  u.Z }) return  6;
	if (v == Vec3Int{ -u.Z,  u.X, -u.Y }) return  7;
	if (v == Vec3Int{  u.Y,  u.Z,  u.X }) return  8;
	if (v == Vec3Int{ -u.Z,  u.Y,  u.X }) return  9;
	if (v == Vec3Int{ -u.Y, -u.Z,  u.X }) return 10;
	if (v == Vec3Int{  u.Z, -u.Y,  u.X }) return 11;
	if (v == Vec3Int{ -u.X,  u.Y, -u.Z }) return 12;
	if (v == Vec3Int{ -u.X,  u.Z,  u.Y }) return 13;
	if (v == Vec3Int{ -u.X, -u.Y,  u.Z }) return 14;
	if (v == Vec3Int{ -u.X, -u.Z, -u.Y }) return 15;
	if (v == Vec3Int{  u.Y, -u.X,  u.Z }) return 16;
	if (v == Vec3Int{ -u.Z, -u.X,  u.Y }) return 17;
	if (v == Vec3Int{ -u.Y, -u.X, -u.Z }) return 18;
	if (v == Vec3Int{  u.Z, -u.X, -u.Y }) return 19;
	if (v == Vec3Int{  u.Y, -u.Z, -u.X }) return 20;
	if (v == Vec3Int{  u.Z,  u.Y, -u.X }) return 21;
	if (v == Vec3Int{ -u.Y,  u.Z, -u.X }) return 22;
	if (v == Vec3Int{ -u.Z, -u.Y, -u.X }) return 23;

	return -1;
}

static void VoteForTransform(const Vec3Int& worldSpaceA,
	const Vec3Int& worldSpaceB,
	const Vec3Int& scannerSpaceA,
	const Vec3Int& scannerSpaceB,
	const vector<Mat43Int>& rotations,
	HashMap<Vec4Int, int32_t>* transforms)
{
	const Vec3Int u = scannerSpaceB - scannerSpaceA;
	const Vec3Int v = worldSpaceB - worldSpaceA;
	const int32_t rotationIndex = FindRotationIndex(u, v);
	if (rotationIndex == -1)
		return;

	assert((u * rotations[rotationIndex]) == v);

	const Vec3Int translation = worldSpaceA - (scannerSpaceA * rotations[rotationIndex]);
	(*transforms)[{ translation.X, translation.Y, translation.Z, rotationIndex }]++;

	//{
	//	Mat43Int transform = rotations[rotationIndex];
	//	transform.m30 = translation.X;
	//	transform.m31 = translation.Y;
	//	transform.m32 = translation.Z;
	//	assert((scannerSpaceA * transform) == worldSpaceA);
	//	assert((scannerSpaceB * transform) == worldSpaceB);
	//}
}

void Puzzle19_A_2021()
{
	vector<Scanner> scanners = ReadPuzzle();

	vector<Mat43Int> rotations = CreateRotations();
	vector<Vec3Int> scannerPositions(scanners.size());

	vector<uint32_t> located;
	located.reserve(scanners.size());
	located.push_back(0);

	vector<uint32_t> todo;
	todo.reserve(scanners.size());
	ranges::copy(ranges::iota_view(uint32_t{ 1 }, static_cast<uint32_t>(scanners.size())), back_inserter(todo));

	HashSet<pair<uint32_t, uint32_t>> failedMatches(1024, { 0, 0 });
	while (todo.size() > 0)
	{
		for (int32_t locatedIndex = 0; locatedIndex < located.size(); locatedIndex++)
		{
			const uint32_t worldSpaceScannerIndex = located[locatedIndex];
			const Scanner& worldSpaceScanner = scanners[worldSpaceScannerIndex];

			// Acceleration structure for matching against this already located beacon
			HashMap<pair<int32_t, int32_t>, pair<uint32_t, uint32_t>> haystack(2 * 1024, { -1, -1 });
			for (const auto& p : AllUnorderedPairs(worldSpaceScanner.Beacons.size()))
			{
				const Vec3Int& a = worldSpaceScanner.Beacons[p.first];
				const Vec3Int& b = worldSpaceScanner.Beacons[p.second];
				const Vec3Int v = b - a;
				const pair<int32_t, int32_t> hay{ Dot(v, v), ManhattanDistance(a, b) };
				assert(hay.first > 0);
				assert(hay.second > 0);
				haystack[hay]= { static_cast<uint32_t>(p.first), static_cast<uint32_t>(p.second) };
			}
			assert(haystack.Size() == ((worldSpaceScanner.Beacons.size() * (worldSpaceScanner.Beacons.size() - 1)) / 2));

			// Check all remaining beacons in the todo list
			for (int32_t todoIndex = 0; todoIndex < todo.size(); todoIndex++)
			{
				const uint32_t localSpaceScannerIndex = todo[todoIndex];

				const pair<uint32_t, uint32_t> checking{ worldSpaceScannerIndex, localSpaceScannerIndex };
				if (failedMatches.Contains(checking))
					continue;

				HashMap<Vec4Int, int32_t> likelyTransforms(16, { numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max() });

				Scanner& localSpaceScanner = scanners[localSpaceScannerIndex];
				for (const auto& p : AllUnorderedPairs(localSpaceScanner.Beacons.size()))
				{
					const Vec3Int& localSpaceA = localSpaceScanner.Beacons[p.first];
					const Vec3Int& localSpaceB = localSpaceScanner.Beacons[p.second];
					const Vec3Int u = localSpaceB - localSpaceA;
					const pair<int32_t, int32_t> needle{ Dot(u, u), ManhattanDistance(localSpaceA, localSpaceB) };
					assert(needle.first > 0);
					assert(needle.second > 0);

					pair<uint32_t, uint32_t> haystackPair;
					if (haystack.TryFind(needle, &haystackPair))
					{
						const Vec3Int worldSpaceA = worldSpaceScanner.Beacons[haystackPair.first];
						const Vec3Int worldSpaceB = worldSpaceScanner.Beacons[haystackPair.second];
						VoteForTransform(worldSpaceA, worldSpaceB, localSpaceA, localSpaceB, rotations, &likelyTransforms);
						VoteForTransform(worldSpaceA, worldSpaceB, localSpaceB, localSpaceA, rotations, &likelyTransforms);
					}
				}

				if (likelyTransforms.Size() > 0)
				{
					// Put the beacons into world space
					Vec4Int votedTransform;
					int32_t mostVotes = 0;
					for (const auto& vote : likelyTransforms)
					{
						if (vote.second > mostVotes)
						{
							votedTransform = vote.first;
							mostVotes = vote.second;
						}
					}

					Mat43Int transform = rotations[votedTransform.W];
					transform.m30 = votedTransform.X;
					transform.m31 = votedTransform.Y;
					transform.m32 = votedTransform.Z;
					for (Vec3Int& v : localSpaceScanner.Beacons)
					{
						v = v * transform;
					}

					scannerPositions[localSpaceScannerIndex] = { votedTransform.X, votedTransform.Y, votedTransform.Z };

					located.push_back(localSpaceScannerIndex);
					todo.erase(todo.begin() + todoIndex);
					todoIndex--;
				}
				else
				{
					failedMatches.Insert(checking);
				}
			}
		}
	}

	HashSet<Vec3Int> allBeacons(1024, { numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max() });
	for (const auto& scanner : scanners)
	{
		for (const auto& beacon : scanner.Beacons)
		{
			allBeacons.Insert(beacon);
		}
	}

	int32_t answer = allBeacons.Size();
	PuzzleOutput::Submit(2021, 19, 1, answer);
}

void Puzzle19_B_2021()
{
		vector<Scanner> scanners = ReadPuzzle();

	vector<Mat43Int> rotations = CreateRotations();
	vector<Vec3Int> scannerPositions(scanners.size());

	vector<uint32_t> located;
	located.reserve(scanners.size());
	located.push_back(0);

	vector<uint32_t> todo;
	todo.reserve(scanners.size());
	ranges::copy(ranges::iota_view(uint32_t{ 1 }, static_cast<uint32_t>(scanners.size())), back_inserter(todo));

	HashSet<pair<uint32_t, uint32_t>> failedMatches(1024, { 0, 0 });
	while (todo.size() > 0)
	{
		for (int32_t locatedIndex = 0; locatedIndex < located.size(); locatedIndex++)
		{
			const uint32_t worldSpaceScannerIndex = located[locatedIndex];
			const Scanner& worldSpaceScanner = scanners[worldSpaceScannerIndex];

			// Acceleration structure for matching against this already located beacon
			HashMap<pair<int32_t, int32_t>, pair<uint32_t, uint32_t>> haystack(2 * 1024, { -1, -1 });
			for (const auto& p : AllUnorderedPairs(worldSpaceScanner.Beacons.size()))
			{
				const Vec3Int& a = worldSpaceScanner.Beacons[p.first];
				const Vec3Int& b = worldSpaceScanner.Beacons[p.second];
				const Vec3Int v = b - a;
				const pair<int32_t, int32_t> hay{ Dot(v, v), ManhattanDistance(a, b) };
				assert(hay.first > 0);
				assert(hay.second > 0);
				haystack[hay]= { static_cast<uint32_t>(p.first), static_cast<uint32_t>(p.second) };
			}
			assert(haystack.Size() == ((worldSpaceScanner.Beacons.size() * (worldSpaceScanner.Beacons.size() - 1)) / 2));

			// Check all remaining beacons in the todo list
			for (int32_t todoIndex = 0; todoIndex < todo.size(); todoIndex++)
			{
				const uint32_t localSpaceScannerIndex = todo[todoIndex];

				const pair<uint32_t, uint32_t> checking{ worldSpaceScannerIndex, localSpaceScannerIndex };
				if (failedMatches.Contains(checking))
					continue;

				HashMap<Vec4Int, int32_t> likelyTransforms(16, { numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max() });

				Scanner& localSpaceScanner = scanners[localSpaceScannerIndex];
				for (const auto& p : AllUnorderedPairs(localSpaceScanner.Beacons.size()))
				{
					const Vec3Int& localSpaceA = localSpaceScanner.Beacons[p.first];
					const Vec3Int& localSpaceB = localSpaceScanner.Beacons[p.second];
					const Vec3Int u = localSpaceB - localSpaceA;
					const pair<int32_t, int32_t> needle{ Dot(u, u), ManhattanDistance(localSpaceA, localSpaceB) };
					assert(needle.first > 0);
					assert(needle.second > 0);

					pair<uint32_t, uint32_t> haystackPair;
					if (haystack.TryFind(needle, &haystackPair))
					{
						const Vec3Int worldSpaceA = worldSpaceScanner.Beacons[haystackPair.first];
						const Vec3Int worldSpaceB = worldSpaceScanner.Beacons[haystackPair.second];
						VoteForTransform(worldSpaceA, worldSpaceB, localSpaceA, localSpaceB, rotations, &likelyTransforms);
						VoteForTransform(worldSpaceA, worldSpaceB, localSpaceB, localSpaceA, rotations, &likelyTransforms);
					}
				}

				if (likelyTransforms.Size() > 0)
				{
					// Put the beacons into world space
					Vec4Int votedTransform;
					int32_t mostVotes = 0;
					for (const auto& vote : likelyTransforms)
					{
						if (vote.second > mostVotes)
						{
							votedTransform = vote.first;
							mostVotes = vote.second;
						}
					}

					Mat43Int transform = rotations[votedTransform.W];
					transform.m30 = votedTransform.X;
					transform.m31 = votedTransform.Y;
					transform.m32 = votedTransform.Z;
					for (Vec3Int& v : localSpaceScanner.Beacons)
					{
						v = v * transform;
					}

					scannerPositions[localSpaceScannerIndex] = { votedTransform.X, votedTransform.Y, votedTransform.Z };

					located.push_back(localSpaceScannerIndex);
					todo.erase(todo.begin() + todoIndex);
					todoIndex--;
				}
				else
				{
					failedMatches.Insert(checking);
				}
			}
		}
	}

	MaxValue<int32_t> manhattan;
	for (size_t i = 0; i < scanners.size(); i++)
	{
		for (size_t j = 0; j < scanners.size(); j++)
		{
			if (i == j)
				continue;

			manhattan.Update(ManhattanDistance(scannerPositions[i], scannerPositions[j]));
		}
	}

	int32_t answer = manhattan.Get();
	PuzzleOutput::Submit(2021, 19, 2, answer);
}
