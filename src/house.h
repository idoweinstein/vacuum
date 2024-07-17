#ifndef HOUSE_H_
#define HOUSE_H_

#include <vector>

#include "dirt_sensor.h"
#include "wall_sensor.h"
#include "position.h"
#include "enums.h"

/**
 * @brief The House class represents the house by means of cleaning operations and house state at robot's current location.
 *
 * It keeps track of the wall map, dirt map, current position, docking station position, and total dirt count.
 */
class House : public WallsSensor, public DirtSensor
{
    static constexpr const unsigned int kDirtCleaningUnit = 1; // Units of dirt to clean when cleaning a position

    std::vector<std::vector<bool>> wall_map;                   // The map representing the walls in the environment.
    std::vector<std::vector<unsigned int>> dirt_map;           // The map representing the dirt levels in the environment.
    Position current_position;                                 // The current position of the vacuum cleaner.
    Position docking_station_position;                         // The position of the docking station.
    unsigned int total_dirt_count;                             // The total count of dirt in the environment.

    virtual void setTotalDirtCount();

    /**
     * @brief Checks if a given position is out of bounds of a given map.
     *
     * @param position The position to check.
     * @param map The map its bounds are to be used.
     * @return true if the position is out of bounds, false otherwise.
     */
    template <typename T> static bool isOutOfBounds(const std::vector<std::vector<T>>& map, const Position& position);

public:
    // Delete copy constructor and assignment operator
    House(const House&) = delete;
    House& operator=(const House&) = delete;

    /**
     * @brief Constructs a new House object.
     *
     * @param wall_map The map representing the walls in the environment.
     * @param dirt_map The map representing the dirt levels in the environment.
     * @param docking_station_position The position of the docking station.
     */
    explicit House(const std::vector<std::vector<bool>>& wall_map,
                             const std::vector<std::vector<unsigned int>>& dirt_map,
                             const Position& docking_station_position);
    /**
     * @brief Gets the total count of dirt in the environment.
     *
     * @return The total count of dirt.
     */
    virtual int getTotalDirtCount() const { return total_dirt_count; }

    /**
     * @brief Cleans the current position by reducing the dirt level (by reduction unit of 1).
     */
    virtual void cleanCurrentPosition();

    /**
     * @brief Moves the vacuum cleaner one step in the specified direction.
     *
     * @param step The step to move (a Direction or Stay / Finish).
     */
    virtual void move(Step);

    /**
     * @brief Checks if the vacuum cleaner is in the docking station.
     *
     * @return true if the vacuum cleaner is in the docking station, false otherwise.
     */
    virtual bool isInDockingStation() const { return current_position == docking_station_position; }

    /**
     * @brief Checks if the cleaning operation is finished.
     *
     * @return true if the cleaning operation is finished, false otherwise.
     */
    virtual bool isFinished() const { return total_dirt_count == 0; }

    /**
     * @brief Gets the dirt level at the current position.
     *
     * @return The dirt level at the current position.
     */
    virtual int dirtLevel() const override;

    /**
     * @brief Gets the current position of the vacuum cleaner.
     *
     * @return The current position.
     */
    virtual Position getCurrentPosition() const { return current_position; }

    /**
     * @brief Checks if there is a wall in the specified direction.
     *
     * @param direction The direction to check.
     * @return true if there is a wall, false otherwise.
     */
    virtual bool isWall(Direction) const override;
};

#endif /* HOUSE_H_ */
