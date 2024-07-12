#include "gtest/gtest.h"

#include <vector>
#include <memory>
#include <cstdlib>

#include "locationmanager.h"

namespace
{
    class LocationManagerTest : public testing::Test
    {
            static void initializeMaps(std::vector<std::vector<bool>>& wall_map, std::vector<std::vector<unsigned int>>& dirt_map)
            {
                wall_map.reserve(column_num);
                dirt_map.reserve(column_num);

                for (int i = 0; i < 24; i++)
                {
                    wall_map.push_back({});
                    wall_map[i].reserve(row_num);

                    dirt_map.push_back({});
                    dirt_map[i].reserve(row_num);

                    for (unsigned int j = 0; j < row_num; j++)
                    {
                        bool random_bool = (bool)(rand() % 2);
                        unsigned int random_level = (unsigned int)(rand() % 10);
                        total_dirt_count += random_level;

                        wall_map[i].push_back(random_bool);
                        dirt_map[i].push_back(random_level);
                    }
                }
            }

        protected:
            inline static unsigned int column_num = 24;
            inline static unsigned int row_num = 39;
            inline static unsigned int total_dirt_count = 0;
            inline static unsigned int direction_iterator = 0;
            inline static Position docking_station_position = Position(3,9);

            inline static std::unique_ptr<LocationManager> location_manager;

            static Step getNextStep()
            {
                static Step steps[] = {
                    Step::NORTH, Step::SOUTH, Step::WEST, Step::EAST, Step::STAY, Step::FINISH
                };

                Step next_step = steps[direction_iterator % 6];
                direction_iterator++;
                return next_step;
            }

            static void SetUpTestCase()
            {
                
                std::vector<std::vector<bool>> wall_map;
                std::vector<std::vector<unsigned int>> dirt_map;

                initializeMaps(wall_map, dirt_map);

                location_manager = std::unique_ptr<LocationManager>(new LocationManager(
                    wall_map,
                    dirt_map,
                    docking_station_position
                ));
            }
            
            static void TearDownTestCase()
            {
                location_manager.reset();
            }
    };

    TEST_F(LocationManagerTest, TotalDirtCountSanity)
    {
        EXPECT_EQ(total_dirt_count, location_manager->getTotalDirtCount());
    }

    TEST_F(LocationManagerTest, StartingAtDockingStation)
    {
        EXPECT_TRUE(location_manager->isInDockingStation());
    }

    TEST_F(LocationManagerTest, OverCleaningPosition)
    {
        while(location_manager->dirtLevel() > 0)
        {
            location_manager->cleanCurrentPosition();
        }

        location_manager->cleanCurrentPosition(); // Redundant cleaning
        EXPECT_EQ(0, location_manager->dirtLevel());
    }

    TEST_F(LocationManagerTest, RandomNavigation)
    {
        for(int i = 0; i < 50; i++)
        {
            location_manager->cleanCurrentPosition();

            Step next_step = getNextStep();
            bool is_wall = false;

            if (Step::FINISH != next_step && Step::STAY != next_step)
            {
                is_wall = location_manager->isWall(static_cast<Direction>(next_step));
            }

            if(is_wall)
            {
                continue;
            }

            location_manager->move(next_step); // Should never throw an error
        }
    }

    TEST_F(LocationManagerTest, MoveOutOfBounds)
    {
        std::vector<std::vector<bool>> wall_map;
        std::vector<std::vector<unsigned int>> dirt_map;

        wall_map.push_back({false});
        dirt_map.push_back({0});

        LocationManager simple_location(wall_map, dirt_map, Position(0,0));

        EXPECT_THROW({
            simple_location.move(Step::NORTH);
        }, std::runtime_error);

        EXPECT_THROW({
            simple_location.move(Step::SOUTH);
        }, std::runtime_error);
 
        EXPECT_THROW({
            simple_location.move(Step::EAST);
        }, std::runtime_error);

        EXPECT_THROW({
            simple_location.move(Step::WEST);
        }, std::runtime_error);
    }
}
