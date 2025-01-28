#ifndef INSTRUCTION_DECODE_H
#define INSTRUCTION_DECODE_H

#include <stdint.h>
#include "registers.h" // To access and modify registers

// Structure to represent a decoded instruction
typedef struct {
	int8_t opcode;   // 8 bits (bits 47:40)
	int8_t rd;       // 4 bits (bits 39:36)
	int8_t rs;       // 4 bits (bits 35:32)
	int8_t rt;       // 4 bits (bits 31:28)
	int8_t rm;       // 4 bits (bits 27:24)
	int16_t imm1;    // 12 bits (bits 23:12)
	int16_t imm2;    // 12 bits (bits 11:0)
} Instruction;


// Function declaration


/*
-Functionality: Decodes a 48-bit instruction into its fields.
-parameter1: instruction_line - Pointer to the 48-bit instruction (array of 6 bytes).
-parameter2: decoded_instruction - Pointer to the Instruction struct to store the decoded fields.
-parameter3: registers - Pointer to the Registers structure.
*/
void decode_instruction(const uint8_t *instruction_line, Instruction *decoded_instruction, Registers *registers);


/*
-Functionality: Perform sign extension on a value.
-return The sign-extended 32-bit value.
-parameter1: value - The value to extend.
-parameter2: bits - The number of bits in the input value.
*/
int32_t sign_extend(int16_t value, int bits);

#endif 