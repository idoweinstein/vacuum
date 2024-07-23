#include "gtest/gtest.h"

#include <cstddef>
#include <iostream>

#include "path_tree.h"
#include "position.h"
#include "enums.h"

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
            path_tree.insertChild(-1, Direction::North, Position(5,-9), 17);
        }, std::out_of_range);

        path_tree.insertChild(root_index, Direction::North, Position(5,-9), 4);

        // Check out of bounds parent index
        EXPECT_THROW({
            path_tree.insertChild(2, Direction::East, Position(3,17), 1);
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
            (int)path_tree.insertChild(0, Direction::East, Position(3,7), 5),
            (int)path_tree.insertChild(1, Direction::North, Position(0,0), 19),
            (int)path_tree.insertChild(2, Direction::East, Position(-1,9), 3),
            (int)path_tree.insertChild(3, Direction::West, Position(2,2564), 0),
            (int)path_tree.insertChild(4, Direction::East, Position(0,0), 55),
            (int)path_tree.insertChild(5, Direction::South, Position(1,-99), 999)
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
            (int)path_tree.insertChild(children[2], Direction::North, Position(0,0), 1),
            (int)path_tree.insertChild(7, Direction::South, Position(-1,9), 2),
            (int)path_tree.insertChild(8, Direction::West, Position(2,2564), 3)
        };

        for (unsigned int i = 1; i < 6; i++)
        {
            unsigned int parent_index = path_tree.getParentIndex(another_branch[i]);
            EXPECT_EQ(another_branch[i-1], parent_index);
            EXPECT_EQ(path_tree.getDepth(parent_index) + 1, path_tree.getDepth(children[i]));
        }
    }

    TEST_F(PathTreeTest, CheckAccumulatedScore)
    {
        path_tree.insertChild(0, Direction::North, Position(1,1), 1);
        path_tree.insertChild(1, Direction::North, Position(1,1), 10);
        path_tree.insertChild(2, Direction::North, Position(1,1), 100);
        path_tree.insertChild(3, Direction::North, Position(1,1), 1000);
        path_tree.insertChild(4, Direction::North, Position(1,1), 10000);
        path_tree.insertChild(5, Direction::North, Position(1,1), 100000);

        EXPECT_EQ(111111, path_tree.getScore(6));
    }

    TEST_F(PathTreeTest, LeafIterationTest)
    {
        constexpr unsigned int kLeafScore = 1337;

        int first_branch[] = {
            0, // Root index
            (int)path_tree.insertChild(0, Direction::East, Position(3,7), 0),
            (int)path_tree.insertChild(1, Direction::North, Position(0,0), 0),
            (int)path_tree.insertChild(2, Direction::East, Position(-1,9), 0),
            (int)path_tree.insertChild(3, Direction::West, Position(2,2564), 0),
            (int)path_tree.insertChild(4, Direction::East, Position(0,0), 0),
            (int)path_tree.insertChild(5, Direction::South, Position(1,-99), kLeafScore)
        };
        path_tree.markAsPathEnd(6);

        int second_branch[] = {
            first_branch[0],
            first_branch[1],
            first_branch[2],
            (int)path_tree.insertChild(first_branch[2], Direction::North, Position(0,0), 0),
            (int)path_tree.insertChild(7, Direction::South, Position(-1,9), 0),
            (int)path_tree.insertChild(8, Direction::West, Position(2,2564), kLeafScore)
        };
        path_tree.markAsPathEnd(9);

        // Third Branch
        path_tree.insertChild(first_branch[0], Direction::South, Position(1,5), kLeafScore);
        path_tree.markAsPathEnd(10);

        // Fourth Branch
        path_tree.insertChild(second_branch[4], Direction::East, Position(1,76), 0);
        path_tree.insertChild(11, Direction::North, Position(1,1), kLeafScore);
        path_tree.markAsPathEnd(12);

        std::size_t leaf_num = 0;
        for (unsigned int leaf_index : path_tree)
        {
            leaf_num++;
            EXPECT_EQ(kLeafScore, path_tree.getScore(leaf_index));
        }

        EXPECT_EQ(4, leaf_num);
    }
}
