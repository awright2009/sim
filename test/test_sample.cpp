#include <gtest/gtest.h>

// install and compile
// sudo yum install gtest gtest-devel
// g++ test1.cpp -lgtest -lgtest_main -pthread -o test1

TEST(HelloTest, BasicAssertions)
{
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


