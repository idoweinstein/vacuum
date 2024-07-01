#include "gtest/gtest.h"

#include "pathtree.h"
#include "position.h"
#include "direction.h"

namespace
{
    // Define a Test Fixture class (optional)
    class PathTreeTest : public testing::Test
    {
        protected:
            void SetUp() override
            {
                
            }

            void TearDown() override
            {

            }
    };

    // Whenever using Test Fixture, use TEST_F(), instead TEST()
    TEST_F(PathTreeTest /* Test Fixture */, InsertRoot /* Test Name */)
    {
        PathTree path_tree;
        unsigned int root_index = path_tree.insertRoot(Position(-1, 2));

        EXPECT_EQ(false, path_tree.hasParent(root_index));
        EXPECT_EQ(Direction::STAY, path_tree.getDirection(root_index));

        // Check Position
        Position root_position = path_tree.getPosition(root_index);
        EXPECT_EQ(-1, root_position.first);
        EXPECT_EQ(2, root_position.second);
    }
}