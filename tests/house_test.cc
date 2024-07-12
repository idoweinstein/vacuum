#include "gtest/gtest.h"

#include <vector>
#include <memory>
#include <cstdlib>

#include "house.h"

namespace
{
    class HouseTest : public testing::Test
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

            inline static std::unique_ptr<House> house;

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

                house = std::unique_ptr<House>(new House(
                    wall_map,
                    dirt_map,
                    docking_station_position
                ));
            }
            
            static void TearDownTestCase()
            {
                house.reset();
            }
    };

    TEST_F(HouseTest, TotalDirtCountSanity)
    {
        EXPECT_EQ(total_dirt_count, house->getTotalDirtCount());
    }

    TEST_F(HouseTest, StartingAtDockingStation)
    {
        EXPECT_TRUE(house->isInDockingStation());
    }

    TEST_F(HouseTest, OverCleaningPosition)
    {
        while(house->dirtLevel() > 0)
        {
            house->cleanCurrentPosition();
        }

        house->cleanCurrentPosition(); // Redundant cleaning
        EXPECT_EQ(0, house->dirtLevel());
    }

    TEST_F(HouseTest, RandomNavigation)
    {
        for(int i = 0; i < 50; i++)
        {
            house->cleanCurrentPosition();

            Step next_step = getNextStep();
            bool is_wall = false;

            if (Step::FINISH != next_step && Step::STAY != next_step)
            {
                is_wall = house->isWall(static_cast<Direction>(next_step));
            }

            if(is_wall)
            {
                continue;
            }

            house->move(next_step); // Should never throw an error
        }
    }

    TEST_F(HouseTest, MoveOutOfBounds)
    {
        std::vector<std::vector<bool>> wall_map;
        std::vector<std::vector<unsigned int>> dirt_map;

        wall_map.push_back({false});
        dirt_map.push_back({0});

        House simple_location(wall_map, dirt_map, Position(0,0));

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
