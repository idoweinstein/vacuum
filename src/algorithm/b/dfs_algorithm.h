#ifndef DFS_ALGORITHM_H_
#define DFS_ALGORITHM_H_

#include "algorithm/base_algorithm.h"

#include <stack>

class DFSAlgorithm : public BaseAlgorithm
{
    Position current_working_position;
    std::stack<Position> position_stack;
    std::unordered_set<Position> registered_positions;

    void registerPositions(const Position& current_position);

public:
    bool getPathToNextTarget(const Position& start_position,
                             std::deque<Direction>& path) override;
};

#endif /* DFS_ALGORITHM_H_ */