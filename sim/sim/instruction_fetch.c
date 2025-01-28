#define _CRT_SECURE_NO_WARNINGS
#include "instruction_fetch.h"
#include <stdio.h> 


// Fetch the instruction from instruction memory
const uint8_t *fetch_instruction(const Memory *memory, uint16_t *pc) {
	if (*pc > PC_MAX || *pc < 0) {
		printf("Error: Program Counter (PC) out of range: %d\n", *pc);
		return NULL; // Return NULL for invalid PC
	}

	// Fetch and return the instruction at the current PC
	return read_instruction(memory, *pc);
}

// Increment the Program Counter (PC)
void increment_pc(uint16_t *pc) {
	if (*pc < PC_MAX) {
		(*pc)++; // Increment PC by 1
	}
	else {
		printf("Warning: Program Counter (PC) reached maximum value.\n");
	}
}