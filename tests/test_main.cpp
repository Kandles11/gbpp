#include <gtest/gtest.h>


int add(int a, int b) {
    return a + b;
}

TEST(MathTest, BasicAddition) {
    EXPECT_EQ(add(2, 2), 4);
    EXPECT_EQ(add(-1, 1), 0);
}