#include "stdafx.h"
#include "Intputer.h"

using namespace std;

namespace Puzzle23_2019_Types
{
	using uIntputerPsram = uIntputer<int64_t, MemArenaStlAllocator<int64_t>>;
}

using namespace Puzzle23_2019_Types;

static pair<int64_t, int64_t> RunUntilIdle(vector<uIntputerPsram>* nics, vector<uIntputerIO<int64_t>>* network)
{
	pair<int64_t, int64_t> nat;
	size_t activeCards = 0;
	do
	{
		activeCards = 0;
		for (size_t card = 0; card < nics->size(); card++)
		{
			uIntputerPsram& nic = (*nics)[card];
			uIntputerIO<int64_t>& io = (*network)[card];

			if (io.Read.empty())
			{
				io.Read.push_back(-1);
			}

			auto exec = nic.Execute();
			assert(exec == uIntputerExecutionResult::PendingIo);
			(void)exec;

			if (io.Write.empty() == false)
			{
				activeCards++;

				assert((io.Write.size() % 3) == 0);

				for (int64_t packet = 0; packet < (int64_t)io.Write.size(); packet += 3)
				{
					int64_t packetX = io.Write[packet + 1];
					int64_t packetY = io.Write[packet + 2];
					int64_t address = io.Write[packet + 0];

					if (address == 255)
					{
						nat = { packetX, packetY };
					}
					else
					{
						(*network)[address].Read.push_back(packetX);
						(*network)[address].Read.push_back(packetY);
					}
				}

				io.Write.clear();
			}
		}

	} while (activeCards != 0);

	return nat;
}

void Puzzle23_A_2019()
{
	const size_t psramNeeded = 1 * 1024 * 1024;
	if (Hardware::PsramSize() >= psramNeeded)
	{
		const size_t firmwareSize = 2300;

		vector<int64_t> firmware;
		firmware.reserve(firmwareSize);
		while (PuzzleInput::PeekChar() != '\n')
		{
			firmware.push_back(Parse::GetInt64());
		}
		firmware.resize(firmwareSize);

		const size_t numDevices = 50;

		MemArenaConfig cfg;
		cfg.AuxiliaryRegion = Hardware::PsramBase();
		cfg.AuxiliaryRegionSize = Hardware::PsramSize();

		MemArena_Configure(cfg);
		{
			vector<uIntputerPsram> nics{ numDevices };
			vector<uIntputerIO<int64_t>> network{ numDevices };

			for (size_t i = 0; i < nics.size(); i++)
			{
				nics[i].CopyProgram(firmware);
				nics[i].SetReadWriteQueues(&network[i]);
				network[i].Read.push_back(i);
				auto exec = nics[i].Execute();
				assert(exec == uIntputerExecutionResult::PendingIo);
				(void)exec;
				assert(network[i].Write.size() == 0);
			}

			int64_t answer = RunUntilIdle(&nics, &network).second;

			PuzzleOutput::Submit(2019, 23, 1, answer);
		}
		MemArena_Reset();
	}
	else
	{
		PuzzleOutput::Unsupported(2019, 23, 2);
	}
}

void Puzzle23_B_2019()
{
	const size_t psramNeeded = 1 * 1024 * 1024;
	if (Hardware::PsramSize() >= psramNeeded)
	{
		const size_t firmwareSize = 2300;

		vector<int64_t> firmware;
		firmware.reserve(firmwareSize);
		while (PuzzleInput::PeekChar() != '\n')
		{
			firmware.push_back(Parse::GetInt64());
		}
		firmware.resize(firmwareSize);

		const size_t numDevices = 50;

		MemArenaConfig cfg;
		cfg.AuxiliaryRegion = Hardware::PsramBase();
		cfg.AuxiliaryRegionSize = Hardware::PsramSize();

		MemArena_Configure(cfg);
		{
			vector<uIntputerPsram> nics{ numDevices };
			vector<uIntputerIO<int64_t>> network{ numDevices };

			for (size_t i = 0; i < nics.size(); i++)
			{
				nics[i].CopyProgram(firmware);
				nics[i].SetReadWriteQueues(&network[i]);
				network[i].Read.push_back(i);
				auto exec = nics[i].Execute();
				assert(exec == uIntputerExecutionResult::PendingIo);
				(void)exec;
				assert(network[i].Write.size() == 0);
			}

			int64_t answer = 0;

			set<pair<int64_t, int64_t>> packets;
			while (true)
			{
				auto restartPacket = RunUntilIdle(&nics, &network);
				if (packets.insert(restartPacket).second == false)
				{
					answer = restartPacket.second;
					break;
				}

				network[0].Read.push_back(restartPacket.first);
				network[0].Read.push_back(restartPacket.second);
			}

			PuzzleOutput::Submit(2019, 23, 2, answer);
		}
		MemArena_Reset();
	}
	else
	{
		PuzzleOutput::Unsupported(2019, 23, 2);
	}
}
