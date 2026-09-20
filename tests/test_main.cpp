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

TEST(B0InstructionTest, DecR16) {
    CPU cpu;
    Memory mem;
    cpu.setBC(0xAFAF);
    mem.data[0x0000] = 0b00001011; //dec BC
    cpu.execute(8, mem);
    EXPECT_EQ(cpu.getBC(), 0xAFAF-1);
}

TEST(B0InstructionTest, AddHLR16) {
    CPU cpu;
    Memory mem;
    cpu.setBC(0xAFAF);
    cpu.setHL(0x4444);
    mem.data[0x0000] = 0b00001001; //add hl, BC
    cpu.execute(8, mem);
    EXPECT_EQ(cpu.getHL(), 0xF3F3);
}

TEST(B0InstructionTest, IncR8) {
    CPU cpu;
    Memory mem;
    cpu.B = 0x04;
    mem.data[0x0000] = 0b00000100; // inc B
    cpu.execute(4, mem);
    EXPECT_EQ(cpu.B, 0x05);

}

TEST(B0InstructionTest, DecR8) {
    CPU cpu;
    Memory mem;
    cpu.B = 0x04;
    mem.data[0x0000] = 0b00000101; // inc B
    cpu.execute(4, mem);
    EXPECT_EQ(cpu.B, 0x03);

}

TEST(B0InstructionTest, RLCA) {
    CPU cpu;
    Memory mem;
    cpu.A = 0b10101010;
    mem.data[0x0000] = 0b00000111;
    cpu.execute(4, mem);
    EXPECT_EQ(cpu.A, 0b01010101);
    EXPECT_EQ(cpu.carry, 1);
}

TEST(B0InstructionTest, RLCA_2) {
    CPU cpu;
    Memory mem;
    cpu.A = 0b00001111;
    mem.data[0x0000] = 0b00000111;
    cpu.execute(4, mem);
    EXPECT_EQ(cpu.A, 0b00011110);
    EXPECT_EQ(cpu.carry, 0);
}

TEST(B0InstructionTest, RLCA_3) {
    CPU cpu;
    Memory mem;
    cpu.A = 0b11110000;
    mem.data[0x0000] = 0b00000111;
    cpu.execute(4, mem);
    EXPECT_EQ(cpu.A, 0b11100001);
    EXPECT_EQ(cpu.carry, 1);

}

TEST(B0InstructionTest, RRCA) {
    CPU cpu;
    Memory mem;
    cpu.A = 0b10101010;
    mem.data[0x0000] = 0b00001111;
    cpu.execute(4, mem);
    EXPECT_EQ(cpu.A, 0b01010101);
    EXPECT_EQ(cpu.carry, 0);

}

TEST(B0InstructionTest, RRCA_2) {
    CPU cpu;
    Memory mem;
    cpu.A = 0b00001111;
    mem.data[0x0000] = 0b00001111;
    cpu.execute(4, mem);
    EXPECT_EQ(cpu.A, 0b10000111);
    EXPECT_EQ(cpu.carry, 1);
}

TEST(B0InstructionTest, RRCA_3) {
    CPU cpu;
    Memory mem;
    cpu.A = 0b11110000;
    mem.data[0x0000] = 0b00001111;
    cpu.execute(4, mem);
    EXPECT_EQ(cpu.A, 0b01111000);
    EXPECT_EQ(cpu.carry, 0);
}

TEST(B0InstructionTest, RLA) {
    CPU cpu;
    Memory mem;
    mem.data[0x0000] = 0b00010111;
    
    cpu.carry = 0;
    cpu.A = 0b11110000;

    cpu.execute(4, mem);
    EXPECT_EQ(cpu.A, 0b11100000);
    EXPECT_EQ(cpu.carry, 1);
}


TEST(B0InstructionTest, RLA_2) {
    CPU cpu;
    Memory mem;
    mem.data[0x0000] = 0b00010111;
    
    cpu.carry = 1;
    cpu.A = 0b11110000;

    cpu.execute(4, mem);
    EXPECT_EQ(cpu.A, 0b11100001);
    EXPECT_EQ(cpu.carry, 1);
}

TEST(B0InstructionTest, RRA) {
    CPU cpu;
    Memory mem;
    mem.data[0x0000] = 0b00011111;
    
    cpu.carry = 0;
    cpu.A = 0b11110000;

    cpu.execute(4, mem);
    EXPECT_EQ(cpu.A, 0b01111000);
    EXPECT_EQ(cpu.carry, 0);
}


TEST(B0InstructionTest, RRA_2) {
    CPU cpu;
    Memory mem;
    mem.data[0x0000] = 0b00011111;
    
    cpu.carry = 1;
    cpu.A = 0b11110000;

    cpu.execute(4, mem);
    EXPECT_EQ(cpu.A, 0b11111000);
    EXPECT_EQ(cpu.carry, 0);
}