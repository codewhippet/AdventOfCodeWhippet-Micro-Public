#include "stdafx.h"

using namespace std;

namespace Puzzle07_2022_Types
{
	struct Directory
	{
		int32_t LocalSize = 0;
		uint32_t Parent;
		vector<uint32_t> Children;
		int32_t TotalSize = 0;
	};

	struct GetDirectory
	{
		vector<Directory>& dirs;
		const Directory& operator()(uint32_t index) const
		{
			return dirs[index];
		}
	};

	template <typename CLASS, auto MEMBER>
	struct GetMember
	{
		auto operator()(const CLASS &ptr) const
		{
			return ptr.*MEMBER;
		}
	};

	struct LessEqual
	{
		int32_t Comparand;
		bool operator()(int32_t v) const
		{
			return v <= Comparand;
		}
	};

	struct GreaterEqual
	{
		int32_t Comparand;
		bool operator()(int32_t v) const
		{
			return v >= Comparand;
		}
	};

	struct DirectoryIterator
	{
	public:
		using value_type = uint32_t;
		using difference_type = ptrdiff_t;

		const vector<Directory>* Dirs = nullptr;
		uint32_t Current = numeric_limits<uint32_t>::max();
		vector<pair<uint32_t, int32_t>> Stack;

		DirectoryIterator() = default;
		explicit DirectoryIterator(const vector<Directory>* dirs)
			: Dirs(dirs)
			, Current(0)
		{
			const size_t expectedDirDepth = 16;
			Stack.reserve(expectedDirDepth);
			Stack.push_back({ Current, -1 });
			Next();
		}

		const value_type& operator*() const
		{
			return Current;
		}

		const value_type* operator->() const
		{
			return &Current;
		}

		DirectoryIterator& operator++()
		{
			Next();
			return *this;
		}

		DirectoryIterator operator++(int)
		{
			DirectoryIterator prev(*this);
			Next();
			return prev;
		}

		bool operator==(const DirectoryIterator& other) const
		{
			return Current == other.Current;
		}

	private:
		void Next()
		{
			assert(Stack.empty() == false);
			while (Stack.size() > 0)
			{
				pair<uint32_t, int32_t>& topOfStack = Stack.back();
				if (topOfStack.second == -1)
				{
					Current = topOfStack.first;
					topOfStack.second = 0;
					return;
				}

				const Directory& dir = (*Dirs)[topOfStack.first];
				if (topOfStack.second < dir.Children.size())
				{
					uint32_t currentChild = topOfStack.second++;
					Stack.push_back({ dir.Children[currentChild], -1 });
				}
				else
				{
					Stack.pop_back();
				}
			}

			Current = numeric_limits<uint32_t>::max();
		}
	};

	static_assert(forward_iterator<DirectoryIterator>);
}

using namespace Puzzle07_2022_Types;

static vector<Directory> ParseDirectoryStructure()
{
	const size_t expectedNumDirectories = 200;
	vector<Directory> directories;
	directories.reserve(expectedNumDirectories);

	uint32_t currentDirectory = numeric_limits<uint32_t>::max();

	array<char, 32> line;
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line.data(), line.size());
		if ((line[0] == '$') && (line[2] == 'c'))
		{
			if (line[5] == '.')
			{
				currentDirectory = directories[currentDirectory].Parent;
			}
			else
			{
				uint32_t dirIndex = static_cast<uint32_t>(directories.size());
				directories.push_back({});

				Directory& dir = directories.back();
				dir.Parent = currentDirectory;

				if (currentDirectory != numeric_limits<uint32_t>::max())
				{
					directories[currentDirectory].Children.push_back(dirIndex);
				}

				currentDirectory = dirIndex;
			}
		}
		else if (isdigit(line[0]))
		{
			int32_t fileSize = Parse::GetInt32(line.data());
			directories[currentDirectory].LocalSize += fileSize;
		}
	}

	return directories;
}

static void PopulateTotalSizes(vector<Directory>* dirs)
{
	const size_t expectedDirDepth = 16;
	vector<pair<uint32_t, int32_t>> stack;
	stack.reserve(expectedDirDepth);
	stack.push_back({ 0, 0 });

	while (stack.size() > 0)
	{
		pair<uint32_t, int32_t>& topOfStack = stack.back();
		Directory& currentDir = (*dirs)[topOfStack.first];
		if (topOfStack.second == currentDir.Children.size())
		{
			currentDir.TotalSize = currentDir.LocalSize;
			for (uint32_t child : currentDir.Children)
			{
				currentDir.TotalSize += (*dirs)[child].TotalSize;
			}
			stack.pop_back();
		}
		else
		{
			uint32_t nextChildIndex = topOfStack.second++;
			uint32_t nextChild = currentDir.Children[nextChildIndex];
			stack.push_back({ nextChild, 0 });
		}
	}
}

void Puzzle07_A_2022()
{
	vector<Directory> dirs = ParseDirectoryStructure();
	PopulateTotalSizes(&dirs);

	auto dirsLessThanTarget = ranges::subrange{ DirectoryIterator{ &dirs }, DirectoryIterator{} }
		| views::transform(GetDirectory{ dirs })
		| views::transform(GetMember<Directory, &Directory::TotalSize>{})
		| views::filter(LessEqual{ 100000 });

	int32_t answer = accumulate(dirsLessThanTarget.begin(), dirsLessThanTarget.end(), 0);

	PuzzleOutput::Submit(2022, 7, 1, answer);
}

void Puzzle07_B_2022()
{
	vector<Directory> dirs = ParseDirectoryStructure();
	PopulateTotalSizes(&dirs);

	const int32_t freeSpace = 70000000 - dirs[0].TotalSize;
	const int32_t neededSpace = 30000000;

	auto dirsThatFreeUpEnoughSpace = ranges::subrange{ DirectoryIterator{ &dirs }, DirectoryIterator{} }
		| views::transform(GetDirectory{ dirs })
		| views::transform(GetMember<Directory, &Directory::TotalSize>{})
		| views::filter(GreaterEqual{ neededSpace - freeSpace });

	int32_t answer = ranges::min(dirsThatFreeUpEnoughSpace);

	PuzzleOutput::Submit(2022, 7, 2, answer);
}
