/**
 * @file test_main.cpp
 * @brief Google test main file
 * @author Iaroslav Vasilevskii (xvasil10)
 */
#include <gtest/gtest.h>

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
