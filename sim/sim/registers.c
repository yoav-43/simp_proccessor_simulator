#define _CRT_SECURE_NO_WARNINGS
#include "registers.h"
#include <stdio.h> 


// Initialize the registers
void init_registers(Registers *registers) {
	for (int i = 0; i < NUM_REGISTERS; i++) {
		registers->regs[i] = 0;
	}
}

// Get the value of a register (32-bit signed integer)
int32_t get_register(const Registers *registers, int reg_index) {
	if (reg_index < 0 || reg_index >= NUM_REGISTERS) {
		printf("Error: Invalid register index %d\n", reg_index);
		return 0;
	}
	return registers->regs[reg_index];
}

// Set the value of a register
void set_register(Registers *registers, int reg_index, int32_t value) {
	if (reg_index < 0 || reg_index >= NUM_REGISTERS) {
		printf("Error: Invalid register index %d\n", reg_index);
		return;
	}

	// Handle $imm1 and $imm2 during decode
	if (registers->imm && (reg_index == REG_IMM1 || reg_index == REG_IMM2)) {
		registers->regs[reg_index] = value; // Update $imm1 or $imm2 during decode
		return;
	}

	// Prevent writing to $zero, $imm1, or $imm2 outside of decode
	if (reg_index == REG_ZERO || reg_index == REG_IMM1 || reg_index == REG_IMM2) {
		return;
	}

	// Write to general-purpose register
	registers->regs[reg_index] = value;
}