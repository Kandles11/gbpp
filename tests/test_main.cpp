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

TEST(B0InstructionTest, LdR16Imm16) {
    CPU cpu;
    Memory mem;
    mem.data[0x0000] = 0b00000001; //load BC, imm16
    mem.data[0x0001] = 0xFF;
    mem.data[0x0002] = 0xAA;
    cpu.execute(12, mem);
    EXPECT_EQ(cpu.getBC(), 0xAAFF);
    EXPECT_EQ(cpu.PC, 0x0003);
    EXPECT_EQ(cpu.B, 0xAA);
    EXPECT_EQ(cpu.C, 0xFF);
}

TEST(B0InstructionTest, LdR16memA) {
    CPU cpu; 
    Memory mem;
    mem.data[0x0000] = 0b00000010; // ld [BC], a 
    
}

TEST(B0InstructionTest, IncR16) {
    CPU cpu;
    Memory mem;
    cpu.setBC(0xAFAF);
    mem.data[0x0000] = 0b00000011; // inc BC
    cpu.execute(8, mem);
    EXPECT_EQ(cpu.getBC(), 0xAFAF+1);
}