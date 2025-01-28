#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h> 

// Number of registers in the SIMP processor
#define NUM_REGISTERS 16

// Register indices for special registers
#define REG_ZERO 0
#define REG_IMM1 1
#define REG_IMM2 2

// Structure to represent the registers
typedef struct {
	int32_t regs[NUM_REGISTERS]; // Array to hold the register values (32-bit signed integer).
	int imm;					  // A flag that give permission to update the imm regs.
} Registers;


// Function declaration


// Functionality: Initializes the registers. Sets R0 to 0 and clears all other registers.
void init_registers(Registers *registers);

/*
-Functionality: Gets the value of a register.
-return The value of the specified register.
-parameter1: registers - Pointer to the Registers structure.
-parameter2: reg_index - The index of the register to read (0 to 15).
*/
int32_t get_register(const Registers *registers, int reg_index);


/*
-Functionality: Sets the value of a register.
-parameter1: registers - Pointer to the Registers structure.
-parameter2: reg_index - The index of the register to write (0 to 15).
-parameter3: value - The value to set in the register.
-Note: Writing to R0, R1, or R2 has no effect (special registers).
*/
void set_register(Registers *registers, int reg_index, int32_t value);

#endif 