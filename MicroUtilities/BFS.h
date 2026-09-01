#pragma once
#include <array>
#include <vector>
#include <limits>

// Requirements for CUSTOMISATION base class:
// 
// size_t MaxQueueSize() const;
// 
// void InitialiseQueuedChecker();
// void HasQueued(const State& s);
// bool HasAlreadyQueued(const State& s);
// 
// BFSControl ProcessCurrent(const State& s, int32_t steps);
// 
// void CacheNeighbours(const State& s);
// const container<State>& GetNeighbours() const;
// bool CanMoveTo(const State& s);

enum class BFSControl
{
	Process,
	Skip,
	Finish,
};

template <typename CUSTOMISATION>
class BFS : public CUSTOMISATION
{
public:

	using State = typename CUSTOMISATION::State;

    int32_t FindShortest(const State& start)
    {
		//**TODO
		(void)start;
		assert(false);
		return 0;
    }

    void FindAll(const State& start)
    {
		std::array<std::vector<State>, 2> queues;
		queues[0].reserve(CUSTOMISATION::MaxQueueSize());
		queues[1].reserve(CUSTOMISATION::MaxQueueSize());

		CUSTOMISATION::InitialiseQueuedChecker();
		queues[0].push_back(start);
		CUSTOMISATION::HasQueued(start);

		for (int32_t steps = 0; steps < std::numeric_limits<int32_t>::max(); steps++)
		{
			const std::vector<State>& currentQueue = queues[steps & 1];
			if (currentQueue.empty())
				break;

			std::vector<State>& nextQueue = queues[1 - (steps & 1)];
			nextQueue.clear();
			for (const State& current : currentQueue)
			{
				if (CUSTOMISATION::ProcessCurrent(current, steps) == BFSControl::Skip)
					continue;

				CUSTOMISATION::CacheNeighbours(current);
				for (const State& neighbour : CUSTOMISATION::GetNeighbours())
				{
					if (!CUSTOMISATION::HasAlreadyQueued(neighbour) && CUSTOMISATION::CanMoveTo(neighbour))
					{
						nextQueue.push_back(neighbour);
						CUSTOMISATION::HasQueued(neighbour);
					}
				}
			}
		}
    }

private:

};
