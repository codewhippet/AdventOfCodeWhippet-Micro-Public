#pragma once
#include <vector>
#include <utility>
#include <algorithm>
#include <assert.h>

// Requirements for CUSTOMISATION base class:
// 
// size_t MaxQueueSize() const;
// 
// void InitialiseVisitedChecker();
// DijkstraControl Visit(const State& s);
// 
// void CacheNeighbours(const State& s);
// const container<std::pair<int32_t, State>>& GetNeighbours() const;
// bool CanMoveTo(const State& s);

enum class DijkstraControl
{
	Process,
	Skip,
	Found,
	Finish,
};

#if 1
template <typename CUSTOMISATION>
class Dijkstra : public CUSTOMISATION
{
public:

	using State = typename CUSTOMISATION::State;

	int32_t FindShortest(const State& start)
	{
		std::vector<std::pair<int32_t, State>> priorityQueue;
		priorityQueue.reserve(CUSTOMISATION::MaxQueueSize());

		CUSTOMISATION::InitialiseVisitedChecker();

		priorityQueue.push_back({ 0, start });
		while (priorityQueue.empty() == false)
		{
			std::pair<int32_t, State> current = priorityQueue[0];
			std::ranges::pop_heap(priorityQueue);
			priorityQueue.pop_back();

			DijkstraControl visitResult = CUSTOMISATION::Visit(current.second);
			assert(visitResult != DijkstraControl::Finish);
			if (visitResult == DijkstraControl::Found)
				return -current.first;

			if (visitResult == DijkstraControl::Skip)
				continue;

			CUSTOMISATION::CacheNeighbours(current.second);
			for (const std::pair<int32_t, State>& neighbour : CUSTOMISATION::GetNeighbours())
			{
				if (CUSTOMISATION::CanMoveTo(neighbour.second))
				{
					int32_t distance = -current.first + neighbour.first;
					priorityQueue.push_back({ -distance, neighbour.second });
					std::ranges::push_heap(priorityQueue);
				}
			}
		}

		return -1;
	}

	void FindAll(const State& start)
	{
		//**TODO
		(void)start;
		assert(false);
		return 0;
	}

private:

};
#endif
