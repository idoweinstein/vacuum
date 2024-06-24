#ifndef VACUUM_POSITION_H_
#define VACUUM_POSITION_H_

#include "direction.h"

#include <functional>
#include <utility>

class Position : public std::pair<int, int> {
    public:
        Position() : std::pair<int, int>(0, 0) {}
        Position(int x, int y) : std::pair<int, int>(x, y) {} 
        static Position computePosition(Position position, Direction direction)
        {
            static std::unordered_map<Direction, std::pair<int, int>> direction_map = {
                {Direction::NORTH, std::make_pair(0, 1)},
                {Direction::SOUTH, std::make_pair(0, -1)},
                {Direction::WEST, std::make_pair(-1, 0)},
                {Direction::EAST, std::make_pair(1, 0)},
                {Direction::STAY, std::make_pair(0, 0)}
            };
            return Position(position.first + direction_map[direction].first, position.second + direction_map[direction].second);
        }
};

class UPosition : public std::pair<unsigned int, unsigned int> {
    public:
        UPosition() : std::pair<unsigned int, unsigned int>(0, 0) {}
        UPosition(unsigned int x, unsigned int y) : std::pair<unsigned int, unsigned int>(x, y) {} 
        static Position computePosition(Position position, Direction direction)
        {
            if (position.first == 0 &&  direction == Direction::NORTH) {
                throw std::range_error("Cannot move off grid");
            }

            if (position.second == 0 &&  direction == Direction::WEST) {
                throw std::range_error("Cannot move off grid");
            }

            static std::unordered_map<Direction, std::pair<int, int>> direction_map = {
                {Direction::NORTH, std::make_pair(0, 1)},
                {Direction::SOUTH, std::make_pair(0, -1)},
                {Direction::WEST, std::make_pair(-1, 0)},
                {Direction::EAST, std::make_pair(1, 0)},
                {Direction::STAY, std::make_pair(0, 0)}
            };
            return Position(position.first + direction_map[direction].first, position.second + direction_map[direction].second);
        }
};

namespace std {
  template<> struct hash<Position> {
    size_t operator()(Position const& p) const {
      return size_t(p.first ^ p.second);
    }
  };

  template<> struct hash<UPosition> {
    size_t operator()(UPosition const& p) const {
      return size_t(p.first ^ p.second);
    }
  };
};

#endif /* VACUUM_POSITION_H_ */