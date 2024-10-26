#ifndef ABSTRACT_ALGORITHM_H_
#define ABSTRACT_ALGORITHM_H_

#include <iostream>

#include "battery_meter.h"
#include "dirt_sensor.h"
#include "wall_sensor.h"
#include "enums.h"

/**
 * @brief The AbstractAlgorithm class is an abstract base class for vacuum cleaner algorithms.
 */
class AbstractAlgorithm
{
public:

    mutable std::size_t next_step_time; // Time spent on deciding the next step.
    mutable std::size_t build_tree_time; // Time spent on building the path tree.
    /**
     * @brief Virtual destructor for the AbstractAlgorithm class.
     */
    virtual ~AbstractAlgorithm() { }

    /**
     * @brief Set the maximum number of steps the algorithm can take.
     *
     * @param maxSteps The maximum number of steps.
     */
    virtual void setMaxSteps(std::size_t) = 0;

    /**
     * @brief Set the walls sensor for the algorithm.
     *
     * @param wallsSensor The walls sensor to use.
     */
    virtual void setWallsSensor(const WallsSensor&) = 0;

    /**
     * @brief Set the dirt sensor for the algorithm.
     *
     * @param dirtSensor The dirt sensor to use.
     */
    virtual void setDirtSensor(const DirtSensor&) = 0;

    /**
     * @brief Set the battery meter for the algorithm.
     *
     * @param batteryMeter The battery meter to use.
     */
    virtual void setBatteryMeter(const BatteryMeter&) = 0;

    /**
     * @brief Get the next step to take.
     *
     * @return The next step to take.
     */
    virtual Step nextStep() = 0;
};

#endif /* ABSTRACT_ALGORITHM_H_ */