#ifndef GREEDY_ALGORITHM_H_
#define GREEDY_ALGORITHM_H_

#include "algorithm/base_algorithm.h"

class GreedyAlgorithm : public BaseAlgorithm
{
public:
    bool getPathToNextTarget(const Position& start_position,
                                     std::deque<Direction>& path) override
    {
        return getPathToNearestTodo(start_position, path);
    }
};

#endif /* GREEDY_ALGORITHM_H_ */
