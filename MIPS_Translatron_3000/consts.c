/**
 * @file consts.c
 * @brief Constants used throughout codebase.
 * @author Cameron Hannay
 */

// Corrected by Jacob Lancaster

#include "include.h"


const uint32_t UNDEFINED;

const uint32_t MAX_5_BIT = 31;
const uint32_t MAX_6_BIT = 63;
const uint32_t MAX_16_BIT = 65535;
const uint32_t MAX_26_BIT = 67108863;

const char* registers[] = {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
};

const struct instruction_definition instruction_definitions[] = {
    // Three "Golden Standard" Instructions
    // The ADD instruction shows what each part of the instruction defintion means
    {
        "ADD",                  // MIPS instruction name
        R_TYPE,                 // Type of instruction (R, I, or J)
        "000000",               // Opcode
        "100000",               // Funct Code
        { RD, RS, RT, EMPTY }   // Registers
    },                          // This would be read as: add $rd, $rs, $rt

    {
        "ADDI",
        I_TYPE,
        "001000",
        NULL,
        { RT, RS, IMM, EMPTY }
    },

    {
        "BNE",
        I_TYPE,
        "000101",
        NULL,
        { RS, RT, IMM, EMPTY }
    },

    //TODO: Fix the bugs in the other instructions to restore functionality

    // Instruction Type was I_Type, corrected to be R-Type
    // Opcode was 00100, corrected to be 000000
    {
        "AND",
        R_TYPE,
        "001000",
        "100100",
        { RD, RS, RT, EMPTY }
    },


    // Instruction Type was R_Type, corrected to be R_Type
    {
        "ANDI",
        I_TYPE,
        "001100",
        NULL,
        { RS, RT, IMM, EMPTY }
    },


    // Instruction Type was R_Type, corrected to be I_Type
    // Opcode was 000000, corrected to be 000100
    // Funct Code was 000100, corrected to be NULL
    {
        "BEQ",
        I_TYPE,
        "000100",
        NULL,
        { RS, RT, IMM, EMPTY }
    },

    // Registers were {RS, EMPTY, EMPTY, EMPTY}, corrected to { RS, RT, EMPTY, EMPTY }
    {
        "DIV",
        R_TYPE,
        "000000",
        "011010",
        { RS, RT, EMPTY, EMPTY }
    },

    // Registers were { RS, IMM, EMPTY, EMPTY }, corrected to { RT, IMM, EMPTY, EMPTY }
    {
        "LUI",
        I_TYPE,
        "001111",
        NULL,
        { RT, IMM, EMPTY, EMPTY }
    },

    // Instruction Type was R_Type, corrected to I_Type
    // Registers were { RS, IMM, RT, EMPTY }, corrected to { RT, IMM, RS, EMPTY }
    {
        "LW",
        I_TYPE,
        "100011",
        NULL,
        { RT, IMM, RS, EMPTY }
    },

    // Funct Code was 010010, corrected to be 010000
    // Registers were { RS, EMPTY, EMPTY, EMPTY }, corrected to be { RD, EMPTY, EMPTY, EMPTY }
    {
        "MFHI",
        R_TYPE,
        "000000",
        "010000",
        { RS, EMPTY, EMPTY, EMPTY }
    },

    // Funct Code was 010000, corrected to be 010010
    // Registers were { RS, EMPTY, EMPTY, EMPTY }, corrected to be { RD, EMPTY, EMPTY, EMPTY }
    {
        "MFLO",
        R_TYPE,
        "000000",
        "010010",
        { RD, EMPTY, EMPTY, EMPTY }
    },

    // Opcode was 001101, corrected to be 000000
    // Funct Code was 000000, corrected to be 100101
    {
        "OR",
        R_TYPE,
        "000000",
        "100101",
        { RD, RS, RT, EMPTY }
    },

    // Funct Code was 101011, corrected to 101010 (since it's signed)
    // Registers were { RD, RT, RS, EMPTY }, corrected to be { RD, RS, RT, EMPTY }
    {
        "SLT",
        R_TYPE,
        "000000",
        "101010",
        { RD, RT, RS, EMPTY }
    },

    // Opcode was 100010, corrected to 000000
    // Funct Code was 000000, corrected to 100010
    // Registers were { EMPTY, RS, RT, EMPTY }, corrected to be { RD, RS, RT, EMPTY }
    {
        "SUB",
        R_TYPE,
        "000000",
        "100010",
        { RD, RS, RT, EMPTY }
    },

    // Opcode was 100011, corrected to 101011
    {
        "SW",
        I_TYPE,
        "101011",
        NULL,
        { RS, IMM, RT, EMPTY }
    },
    
    {
        "MULT",
        R_TYPE,
        "000000",
        "011000",
        { RS, RT, EMPTY, EMPTY }
    },

    {
        "ORI",
        I_TYPE,
        "001101",
        NULL,
        { RT, RS, IMM, EMPTY }
    },

    {
        "SLTI",
        I_TYPE,
        "001010",
        NULL,
        { RT, RS, IMM, EMPTY }
    }
};