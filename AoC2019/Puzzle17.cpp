#include "stdafx.h"
#include "Intputer.h"

using namespace std;

namespace Puzzle17_2019_Types
{
}

using namespace Puzzle17_2019_Types;

static uArrayMap2D ExtractMap(const vector<int32_t>& program)
{
	uArrayMap2D pipes(CreateArrayMap2DAllocator_Heap(), {}, 64, 32, '.');

	assert(program.size() == 3050);

	uIntputer<int32_t> puter;
	puter.CopyProgram(program);

	deque<int32_t> in;
	deque<int32_t> out;
	puter.SetReadWriteQueues(&in, &out);

	puter.Execute();

	Vec2Int rasterPos{};
	for (int32_t c : out)
	{
		if (c == '\n')
		{
			rasterPos.X = 0;
			rasterPos.Y++;
		}
		else
		{
			pipes(rasterPos) = static_cast<char>(c);
			rasterPos.X++;
		}
	}

	return pipes;
}

static string FindFullPath(const uArrayMap2D& pipes)
{
	// Find the robot
	Vec2Int robotPos;
	Vec2Int robotDir;
	for (const auto& p : pipes.Grid())
	{
		switch (p.second)
		{
		case '^':
			robotPos = p.first;
			robotDir = Vec2Int::Up();
			break;
		case 'v':
			robotPos = p.first;
			robotDir = Vec2Int::Down();
			break;
		case '<':
			robotPos = p.first;
			robotDir = Vec2Int::Left();
			break;
		case '>':
			robotPos = p.first;
			robotDir = Vec2Int::Right();
			break;
		}

		if (robotDir != Vec2Int{})
			break;
	}

	// Orient the robot (note: a choice between double LL or double RR would
	// knacker up simple block search strategies, so I assume the puzzles
	// don't do that)
	auto checkAhead = [&]() { return pipes(robotPos + robotDir) == '#'; };
	auto checkLeft = [&]() { return pipes(robotPos + Vec2Int::RotateAnticlockwise(robotDir)) == '#'; };
	auto checkRight = [&]() { return pipes(robotPos + Vec2Int::RotateClockwise(robotDir)) == '#'; };
	assert(checkAhead() || checkLeft() || checkRight());

	string path;
	if (checkLeft())
	{
		path += 'L';
		robotDir = Vec2Int::RotateAnticlockwise(robotDir);
	}
	else if (checkRight())
	{
		path += 'R';
		robotDir = Vec2Int::RotateClockwise(robotDir);
	}

	while (true)
	{
		for (int step = 0; step < 100; step++)
		{
			if (checkAhead() == false)
			{
				path += to_string(step);
				break;
			}
			robotPos = robotPos + robotDir;
		}

		if (checkLeft())
		{
			path += 'L';
			robotDir = Vec2Int::RotateAnticlockwise(robotDir);
		}
		else if (checkRight())
		{
			path += 'R';
			robotDir = Vec2Int::RotateClockwise(robotDir);
		}
		else
		{
			break;
		}
	}

	return path;
}

static vector<string> RemoveParticle(const vector<string>& particles, const string& particle)
{
	vector<string> updatedParticles;
	for (string section : particles)
	{
		size_t pos;
		while ((pos = section.find(particle)) != string::npos)
		{
			if (pos > 0)
			{
				updatedParticles.push_back(section.substr(0, pos));
			}
			section = section.substr(pos + particle.size());
		}
		if (!section.empty())
		{
			updatedParticles.push_back(section);
		}
	}
	return updatedParticles;
}

static int32_t EncodedLength(const string& particle)
{
	int32_t encodedLength = static_cast<int32_t>(particle.size());
	for (size_t i = 0; (i + 1) < particle.size(); i++)
	{
		encodedLength += isdigit(particle[i]) != isdigit(particle[i + 1]);
	}
	return encodedLength + 1;
}

static string EncodeMovementPiece(const string& original)
{
	string encoded;
	for (size_t i = 0; (i + 1) < original.size(); i++)
	{
		encoded += original[i];
		if (isdigit(original[i]) != isdigit(original[i + 1]))
		{
			encoded += ',';
		}
	}
	return encoded + original.back();
}

static bool Solve(const vector<string>& particles, int depth, vector<string> *answer)
{
	if (depth == 2)
	{
		bool allSame = ranges::all_of(particles | views::drop(1), [&](const string& p) { return p == particles.front(); });
		if (allSame && EncodedLength(particles.front()) <= 20)
		{
			answer->push_back(particles.front());
			return true;
		}
		return false;
	}

	for (size_t length = 10; length > 0; length--)
	{
		string candidateParticle = particles.front().substr(0, length);
		if (EncodedLength(candidateParticle) > 20)
			continue;

		answer->push_back(candidateParticle);
		vector<string> candidateParticles = RemoveParticle(particles, candidateParticle);
		if (Solve(candidateParticles, depth + 1, answer))
		{
			return true;
		}
		answer->pop_back();
	}
	return false;
}

void Puzzle17_A_2019()
{
	vector<int32_t> program;
	while (PuzzleInput::PeekChar() != '\n')
	{
		program.push_back(Parse::GetInt32());
	}
	program.resize(3050);

	uArrayMap2D pipes = ExtractMap(program);

	int32_t answer = 0;
	for (const auto& p : pipes.Grid() | views::filter([](const auto& p) { return p.second == '#'; }))
	{
		auto surroundingPipes = Vec2Int::CardinalDirections()
			| views::transform([&](const Vec2Int& dir)
				{
					return pipes(p.first + dir);
				});
		if (ranges::count(surroundingPipes, '#') == 4)
		{
			answer += p.first.X * p.first.Y;
		}
	}

	PuzzleOutput::Submit(2019, 17, 1, answer);
}

void Puzzle17_B_2019()
{
	vector<int32_t> program;
	while (PuzzleInput::PeekChar() != '\n')
	{
		program.push_back(Parse::GetInt32());
	}
	program.resize(3050);

	uArrayMap2D pipes = ExtractMap(program);
	string path = FindFullPath(pipes);

	vector<string> movementPieces;
	bool foundSolution = Solve({ path }, 0, &movementPieces);
	assert(foundSolution);
	(void)foundSolution;

	vector<size_t> movementSchedule;
	while (!path.empty())
	{
		for (size_t i = 0; i < movementPieces.size(); i++)
		{
			if (path.starts_with(movementPieces[i]))
			{
				movementSchedule.push_back(i);
				path = path.substr(movementPieces[i].size());
			}
		}
	}

	uIntputer<int32_t> puter;
	program[0] = 2;
	puter.CopyProgram(program);

	deque<int32_t> in;
	deque<int32_t> out;
	puter.SetReadWriteQueues(&in, &out);

	for (size_t functionCall : movementSchedule)
	{
		in.push_back('A' + (char)functionCall);
		in.push_back(',');
	}
	in.back() = '\n';

	for (const string& function : movementPieces)
	{
		for (char c : EncodeMovementPiece(function))
		{
			in.push_back(c);
		}
		in.push_back('\n');
	}

	in.push_back('n');
	in.push_back('\n');

	auto exec = puter.Execute();

	assert(exec == uIntputerExecutionResult::Finished);
	(void)exec;
	assert(out.empty() == false);

	int32_t answer = out.back();

	PuzzleOutput::Submit(2019, 17, 2, answer);
}
