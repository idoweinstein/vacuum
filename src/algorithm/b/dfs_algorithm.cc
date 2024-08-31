#include "dfs_algorithm.h"

#include "algorithm/AlgorithmRegistration.h"

void DFSAlgorithm::registerPositions(const Position& current_position)
{
    registered_positions.insert(current_position);

    for (Direction direction : kDirections)
    {
        Position next_position = Position::computePosition(current_position, direction);

        if (isToDoPosition(next_position) && !registered_positions.contains(next_position))
        {
            position_stack.push(next_position);
            registered_positions.insert(next_position);
        }
    }
}

bool DFSAlgorithm::getPathToNextTarget(const Position& start_position,
                                       std::deque<Direction>& path)
{
    /* In case BaseAlgorithm took over and returned to station */
    if (start_position != current_working_position)
    {
        return getPathToPosition(start_position, current_working_position, path);
    }

    registerPositions(start_position);

    if (isToDoPosition(start_position))
    {
        return getPathToPosition(start_position, start_position, path);
    }

    /* Check if TODO position stack is empty */
    if (position_stack.empty())
    {
        return false;
    }

    current_working_position = position_stack.top();
    position_stack.pop();
    return getPathToPosition(start_position, current_working_position, path);
}

REGISTER_ALGORITHM(DFSAlgorithm);
