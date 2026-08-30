#include <gtest/gtest.h>
#include <cpu.hpp>

TEST(RegisterTest, SetBC) {
    CPU cpu;
    cpu.setBC(0xaabb);
    EXPECT_EQ(cpu.B, 0xaa);
    EXPECT_EQ(cpu.C, 0xbb);
}

TEST(RegisterTest, SetDE) {
    CPU cpu;
    cpu.setDE(0xccff);
    EXPECT_EQ(cpu.D, 0xcc);
    EXPECT_EQ(cpu.E, 0xff);
}

TEST(RegisterTest, MultipleGets) {
    CPU cpu;
    cpu.setBC(0xaabb);
    EXPECT_EQ(cpu.getBC(), 0xaabb);
    EXPECT_EQ(cpu.getBC(), 0xaabb);
    EXPECT_EQ(cpu.getBC(), 0xaabb);
    EXPECT_EQ(cpu.B, 0xaa);
    EXPECT_EQ(cpu.C, 0xbb);
}