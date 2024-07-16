#include "gtest/gtest.h"

#include <iostream>

#include "pathtree.h"
#include "position.h"
#include "direction.h"

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
        EXPECT_EQ(false, path_tree.hasParent(root_index));

        // Check Root Position
        Position root_position = path_tree.getPosition(root_index);
        EXPECT_EQ(-1, root_position.first);
        EXPECT_EQ(3, root_position.second);

        // Check Root Depth
        EXPECT_EQ(0, path_tree.getDepth(root_index));
    }

    TEST_F(PathTreeTest, BadParentIndex)
    {
        // Check negative parent index
        EXPECT_THROW({
            path_tree.insertChild(-1, Direction::North, Position(5,-9));
        }, std::out_of_range);

        path_tree.insertChild(root_index, Direction::North, Position(5,-9));

        // Check out of bounds parent index
        EXPECT_THROW({
            path_tree.insertChild(2, Direction::East, Position(3,17));
        }, std::out_of_range);
    }

    TEST_F(PathTreeTest, BadNodeIndex)
    {
        EXPECT_THROW({
            path_tree.getParentIndex(1);
        }, std::out_of_range);

        EXPECT_THROW({
            path_tree.hasParent(-1);
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
        int children[] = {
            0, // Root index
            (int)path_tree.insertChild(0, Direction::East, Position(3,7)),
            (int)path_tree.insertChild(1, Direction::North, Position(0,0)),
            (int)path_tree.insertChild(2, Direction::East, Position(-1,9)),
            (int)path_tree.insertChild(3, Direction::West, Position(2,2564)),
            (int)path_tree.insertChild(4, Direction::East, Position(0,0)),
            (int)path_tree.insertChild(5, Direction::South, Position(1,-99))
        };

        for (unsigned int i = 1; i < 7; i++)
        {
            unsigned int parent_index = path_tree.getParentIndex(children[i]);
            EXPECT_EQ(children[i-1], parent_index);
            EXPECT_EQ(path_tree.getDepth(parent_index) + 1, path_tree.getDepth(children[i]));
        }

        EXPECT_EQ(Direction::West, path_tree.getDirection(children[4]));
        
        int another_branch[] = {
            children[0],
            children[1],
            children[2],
            (int)path_tree.insertChild(children[2], Direction::North, Position(0,0)),
            (int)path_tree.insertChild(7, Direction::South, Position(-1,9)),
            (int)path_tree.insertChild(8, Direction::West, Position(2,2564))
        };

        for (unsigned int i = 1; i < 6; i++)
        {
            unsigned int parent_index = path_tree.getParentIndex(another_branch[i]);
            EXPECT_EQ(another_branch[i-1], parent_index);
            EXPECT_EQ(path_tree.getDepth(parent_index) + 1, path_tree.getDepth(children[i]));
        }
    }
}
