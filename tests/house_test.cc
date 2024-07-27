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
                    wall_map.emplace_back();
                    wall_map[i].reserve(row_num);

                    dirt_map.emplace_back();
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
                    Step::North, Step::South, Step::West, Step::East, Step::Stay, Step::Finish
                };

                Step next_step = steps[direction_iterator % 6];
                direction_iterator++;
                return next_step;
            }

            static void SetUpTestCase()
            {
                
                std::unique_ptr<std::vector<std::vector<bool>>> wall_map =
                    std::make_unique<std::vector<std::vector<bool>>>();
                std::unique_ptr<std::vector<std::vector<unsigned int>>> dirt_map =
                    std::make_unique<std::vector<std::vector<unsigned int>>>();

                initializeMaps(*wall_map, *dirt_map);

                house = std::make_unique<House>(
                    std::move(wall_map),
                    std::move(dirt_map),
                    docking_station_position
                );
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
        EXPECT_TRUE(house->isAtDockingStation());
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

            if (Step::Finish != next_step && Step::Stay != next_step)
            {
                is_wall = house->isWall(static_cast<Direction>(next_step)); // Safe due to prior checks
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
        std::unique_ptr<std::vector<std::vector<bool>>> wall_map =
            std::make_unique<std::vector<std::vector<bool>>>();
        std::unique_ptr<std::vector<std::vector<unsigned int>>> dirt_map =
            std::make_unique<std::vector<std::vector<unsigned int>>>();

        (*wall_map).push_back({false});
        (*dirt_map).push_back({0});

        House simple_location(std::move(wall_map), std::move(dirt_map), Position(0,0));

        EXPECT_THROW({
            simple_location.move(Step::North);
        }, std::runtime_error);

        EXPECT_THROW({
            simple_location.move(Step::South);
        }, std::runtime_error);
 
        EXPECT_THROW({
            simple_location.move(Step::East);
        }, std::runtime_error);

        EXPECT_THROW({
            simple_location.move(Step::West);
        }, std::runtime_error);
    }
}
