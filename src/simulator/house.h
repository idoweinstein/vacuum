#ifndef HOUSE_H_
#define HOUSE_H_

#include <memory>
#include <vector>

#include "common/dirt_sensor.h"
#include "common/wall_sensor.h"
#include "common/position.h"
#include "common/enums.h"

/**
 * @brief The House class represents the house by means of cleaning operations and house state at robot's current location.
 *
 * It keeps track of the wall map, dirt map, current position, docking station position, and total dirt count.
 */
class House : public WallsSensor, public DirtSensor
{
    static constexpr const unsigned int kDirtCleaningUnit = 1;        // Units of dirt to clean when cleaning a position

    std::vector<std::vector<bool>> wall_map;                          // The map representing the walls in the environment.
    std::vector<std::vector<unsigned int>> dirt_map;                  // The map representing the dirt levels in the environment.
    Position current_position;                                        // The current position of the vacuum cleaner.
    Position docking_station_position;                                // The position of the docking station.
    std::size_t total_dirt_count;                                     // The total count of dirt in the environment.
    std::size_t initial_dirt_count;                                   // The initial total count of dirt (used for scoring)

    /**
     * @brief Computes the total dirt count in the house.
     * 
     * Stores the computed total dirt in the `total_dirt_count` data member.
     */
    void computeTotalDirtCount();

    /**
     * @brief Checks if a given position is out of bounds of a given map.
     *
     * @param map The map its bounds are to be used.
     * @param position The position to check.
     * @return true if the position is out of bounds, false otherwise.
     */
    template <typename T> static bool isOutOfBounds(const std::vector<std::vector<T>>& map, const Position& position);

public:
    House() = default;

    House(const House& house) = default;
    House& operator=(const House& house) = default;

    House(House&& house) noexcept = default;
    House& operator=(House&& house) noexcept = default;

    /**
     * @brief Constructs a new House object.
     *
     * @param wall_map The map representing the walls in the environment.
     * @param dirt_map The map representing the dirt levels in the environment.
     * @param docking_station_position The position of the docking station.
     */
    House(std::vector<std::vector<bool>>&& wall_map,
          std::vector<std::vector<unsigned int>>&& dirt_map,
          const Position& docking_station_position);

    /**
     * @brief Gets the initial count of dirt in the environment (before any step was taken).
     *
     * @return The initial count of dirt.
     */
    std::size_t getInitialDirtCount() const { return initial_dirt_count; }

    /**
     * @brief Gets the total count of dirt in the environment.
     *
     * @return The total count of dirt.
     */
    std::size_t getTotalDirtCount() const { return total_dirt_count; }

    /**
     * @brief Cleans the current position by reducing the dirt level (by reduction unit of 1).
     */
    void cleanCurrentPosition();

    /**
     * @brief Moves the vacuum cleaner one step in the specified direction (in the house representation).
     *
     * @param step The step to move (a Direction or Stay / Finish).
     * @throws std::out_of_range If tried to move into a wall.
     */
    void move(Step);

    /**
     * @brief Checks if the vacuum cleaner is at the docking station.
     *
     * @return true if the vacuum cleaner is at the docking station, false otherwise.
     */
    bool isAtDockingStation() const { return current_position == docking_station_position; }

    /**
     * @brief Gets the dirt level at the current position.
     *
     * @throws std::out_of_range If tried to sample dirt level out of the house representation range.
     * @return The dirt level at the current position.
     */
    int dirtLevel() const override;

    /**
     * @brief Checks if there is a wall in the specified direction.
     *
     * @param direction The direction to check.
     * @return true if there is a wall, false otherwise.
     */
    bool isWall(Direction) const override;
};

#endif /* HOUSE_H_ */
