#define _CRT_SECURE_NO_WARNINGS
#include "instruction_decode.h"
#include <stdio.h> 


// Decode the instruction from instruction fetch
void decode_instruction(const uint8_t *instruction_line, Instruction *decoded_instruction, Registers *registers) {
	// Decode the opcode (bits 47:40)
	decoded_instruction->opcode = instruction_line[0];

	// Decode rd, rs, rt, and rm (4 bits each)
	decoded_instruction->rd = (instruction_line[1] >> 4) & 0x0F; // Upper 4 bits of byte 1
	decoded_instruction->rs = instruction_line[1] & 0x0F;        // Lower 4 bits of byte 1
	decoded_instruction->rt = (instruction_line[2] >> 4) & 0x0F; // Upper 4 bits of byte 2
	decoded_instruction->rm = instruction_line[2] & 0x0F;        // Lower 4 bits of byte 2

	// Turn on imm flag (for updating the $imm registers during decoding stage)
	registers->imm = 1;

	// Decode imm1 (bits 23:12) and update $imm1
	int16_t immediate1 = ((instruction_line[3] << 4) | (instruction_line[4] >> 4)) & 0x0FFF; // byte 3 and Upper 4 bits of byte 4
	set_register(registers, REG_IMM1, sign_extend(immediate1, 12));


	// Decode imm2 (bits 11:0) and update $imm2
	int16_t immediate2 = ((instruction_line[4] & 0x0F) << 8) | instruction_line[5]; // Lower 4 bits of byte 4 and byte 5
	set_register(registers, REG_IMM2, sign_extend(immediate2, 12));

	// Turn off the imm flag
	registers->imm = 0;
}

// Sign Extension Function
int32_t sign_extend(int16_t value, int bits) {
	if (value & (1 << (bits - 1))) { // Check the sign bit
		return value | ~((1 << bits) - 1); // Extend the sign bit
	}
	return value;
}

