#include "gtest/gtest.h"

#include <cstddef>
#include <optional>
#include <iostream>

#include "common/position.h"
#include "common/enums.h"

#include "algorithm/path_tree.h"

namespace
{
    class PathTreeTest : public testing::Test
    {
        protected:
            PathTree path_tree;
            unsigned int root_index;

            void SetUp() override
            {
                root_index = path_tree.insertRoot(Position(-1,3));
            }
    };

    TEST_F(PathTreeTest, InsertRootSanity)
    {
        EXPECT_FALSE(path_tree.hasParent(root_index));

        // Check Root Position
        Position root_position = path_tree.getPosition(root_index);
        EXPECT_EQ(-1, root_position.first);
        EXPECT_EQ(3, root_position.second);
    }

    TEST_F(PathTreeTest, BadParentIndex)
    {
        // Check negative parent index
        EXPECT_THROW({
            path_tree.insertChild(-1, Direction::North, Position(5,-9), false);
        }, std::out_of_range);

        path_tree.insertChild(root_index, Direction::North, Position(5,-9), false);

        // Check out of bounds parent index
        EXPECT_THROW({
            path_tree.insertChild(2, Direction::East, Position(3,17), true);
        }, std::out_of_range);
    }

    TEST_F(PathTreeTest, BadNodeIndex)
    {
        EXPECT_THROW({
            path_tree.getParentIndex(1);
        }, std::out_of_range);

        EXPECT_THROW({
            path_tree.hasParent(1);
        }, std::out_of_range);

        EXPECT_THROW({
            path_tree.getPosition(1);
        }, std::out_of_range);

        EXPECT_THROW({
            path_tree.getDirection(1);
        }, std::out_of_range);
    }

    TEST_F(PathTreeTest, InsertChildSanity)
    {
        std::size_t children[] = {
            0, // Root index
            path_tree.insertChild(0, Direction::East, Position(3,7), true).value(),
            path_tree.insertChild(1, Direction::North, Position(0,0), true).value(),
            path_tree.insertChild(2, Direction::East, Position(-1,9), true).value(),
            path_tree.insertChild(3, Direction::West, Position(2,2564), true).value(),
            path_tree.insertChild(4, Direction::South, Position(1,-99), true).value(),
        };

        for (std::size_t i = 1; i < 6; i++)
        {
            EXPECT_TRUE(path_tree.hasParent(children[i]));
            EXPECT_EQ(children[i-1], path_tree.getParentIndex(children[i]));
        }

        EXPECT_EQ(Direction::West, path_tree.getDirection(children[4]));
        
        std::size_t another_branch[] = {
            children[0],
            children[1],
            children[2],
            path_tree.insertChild(children[2], Direction::South, Position(1,9), false).value(),
            path_tree.insertChild(6, Direction::West, Position(2,25640), false).value()
        };

        for (unsigned int i = 1; i < 5; i++)
        {
            EXPECT_TRUE(path_tree.hasParent(another_branch[i]));
            EXPECT_EQ(another_branch[i-1], path_tree.getParentIndex(another_branch[i]));
        }
    }
}
