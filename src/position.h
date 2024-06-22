#ifndef VACUUM_POSITION_H_
#define VACUUM_POSITION_H_

#include <functional>
#include <utility>

using Position = std::pair<int, int>;
using UPosition = std::pair<unsigned int, unsigned int>;

namespace std {
  template<> struct hash<Position> {
    size_t operator()(Position const& p) const {
      return size_t(p.first ^ p.second);
    }
  };

  template<> struct hash<UPosition> {
    size_t operator()(Position const& p) const {
      return size_t(p.first ^ p.second);
    }
  };
};

#endif /* VACUUM_POSITION_H_ */