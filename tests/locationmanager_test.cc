#include "gtest/gtest.h"

#include <vector>
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

                    for (int j = 0; j < row_num; j++)
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
            static LocationManager* location_manager;

            static void SetUpTestSuite()
            {
                
                std::vector<std::vector<bool>> wall_map;
                std::vector<std::vector<unsigned int>> dirt_map;

                initializeMaps(wall_map, dirt_map);
                Position docking_station_position = Position(3,9);

                *location_manager = LocationManager(
                    wall_map,
                    dirt_map,
                    docking_station_position
                );
            }
    };

    TEST_F(LocationManagerTest, TotalDirtCountSanity)
    {
        EXPECT_EQ(total_dirt_count, location_manager->getTotalDirtCount());
    }
}