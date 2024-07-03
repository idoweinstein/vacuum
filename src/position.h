#ifndef VACUUM_POSITION_H_
#define VACUUM_POSITION_H_

#include <utility>
#include <stdexcept>
#include <functional>

#include "direction.h"

class Position : public std::pair<int, int> 
{
        inline static const std::unordered_map<Direction, std::pair<int, int>> direction_map = {
            {Direction::NORTH, std::make_pair(-1, 0)},
            {Direction::SOUTH, std::make_pair(1, 0)},
            {Direction::WEST, std::make_pair(0, -1)},
            {Direction::EAST, std::make_pair(0, 1)},
            {Direction::STAY, std::make_pair(0, 0)}
        };

    public:
        Position() : std::pair<int, int>(0, 0) {}
        Position(int x, int y) : std::pair<int, int>(x, y) {}

        static Position computePosition(Position position, Direction direction)
        {
            std::pair<int, int> shift_direction = direction_map.at(direction);
            return Position(position.first + shift_direction.first, position.second + shift_direction.second);
        }
};

namespace std
{
    // Source: http://szudzik.com/ElegantPairing.pdf
    static inline unsigned int elegantPair(unsigned int a, unsigned int b) 
    {
        return a >= b ? a * a + a + b : a + b * b;
    }

    template<> struct hash<Position> 
    {
        size_t operator()(Position const& p) const 
        {
            // Map each integer to a unique unsigned integer
            unsigned int a = p.first >= 0 ? 2 * p.first : -2 * p.first - 1;
            unsigned int b = p.second >= 0 ? 2 * p.second : -2 * p.second - 1;

            return size_t(elegantPair(a, b));
        }
    };
};

#endif /* VACUUM_POSITION_H_ */
