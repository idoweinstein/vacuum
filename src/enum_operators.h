#ifndef ENUM_OPERATORS_H_
#define ENUM_OPERATORS_H_

#include <string>
#include <ostream>

#include "status.h"
#include "enums.h"

namespace std
{
    std::ostream& operator<<(std::ostream& ostream, const Step& step);

    std::ostream& operator<<(std::ostream& ostream, const Direction& direction);

    std::ostream& operator<<(std::ostream& ostream, const Status& status);
}

#endif /* ENUM_OPERATORS_H_ */
