#ifndef VACUUM_ABSTRACTALGORITHM_H_
#define VACUUM_ABSTRACTALGORITHM_H_

#include "batterymeter.h"
#include "dirtsensor.h"
#include "wallssensor.h"
#include "step.h"

/**
 * @brief The AbstractAlgorithm class is an abstract base class for vacuum cleaner algorithms.
 */
class AbstractAlgorithm
{
public:
    /**
     * @brief Virtual destructor for the AbstractAlgorithm class.
     */
    virtual ~AbstractAlgorithm() {}

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

#endif /* VACUUM_ABSTRACTALGORITHM_H_ */