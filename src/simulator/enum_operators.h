#ifndef ENUM_OPERATORS_H_
#define ENUM_OPERATORS_H_

#include <string>
#include <ostream>

#include "common/enums.h"

#include "status.h"

/**
 * @brief Overload of enum class operators for logging purposes.
 */
namespace std
{
    std::ostream& operator<<(std::ostream& ostream, const Step& step);

    std::ostream& operator<<(std::ostream& ostream, const Direction& direction);

    std::ostream& operator<<(std::ostream& ostream, const Status& status);
}

#endif /* ENUM_OPERATORS_H_ */
