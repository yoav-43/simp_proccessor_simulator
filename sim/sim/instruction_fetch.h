#ifndef INSTRUCTION_FETCH_H
#define INSTRUCTION_FETCH_H

#include <stdint.h>
#include "memory.h"

// Maximum value for the PC (12 bits)
#define PC_MAX 4095

// Function declaration


/*
-Functionality: Fetches the 48-bit instruction from instruction memory.
-return A pointer to the 48-bit instruction (array of 6 bytes).
-parameter1: memory -  Pointer to the Memory structure.
-parameter2: pc - Pointer to the Program Counter (PC).
*/
const uint8_t *fetch_instruction(const Memory *memory, uint16_t *pc);


/*
-Functionality: Increments the Program Counter (PC) by 1.
-parameter1: pc - Pointer to the Program Counter (PC).
*/
void increment_pc(uint16_t *pc);

#endif