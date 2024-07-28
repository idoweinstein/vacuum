#ifndef ROBOT_ENUMS_H_
#define ROBOT_ENUMS_H_

/**
 * @brief Different directions the robot can move to.
 */
enum class Direction { North, East, South, West };

/**
 * @brief Different steps the robot can take.
 */
enum class Step { North, East, South, West, Stay, Finish };

#endif  // ROBOT_ENUMS_H_
