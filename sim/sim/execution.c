#define _CRT_SECURE_NO_WARNINGS
#include "execution.h"
#include <stdio.h>
#include <stdlib.h>

// Execute the instruction from instruction decode
void execute_instruction(const Instruction *decoded_instruction, Registers *registers, Memory *memory, IORegisters *io, uint16_t *pc, int *in_isr, FILE *hwregtrace_file) {
	int32_t rs = get_register(registers, decoded_instruction->rs);
	int32_t rt = get_register(registers, decoded_instruction->rt);
	int32_t rm = get_register(registers, decoded_instruction->rm);
	int32_t rd = get_register(registers, decoded_instruction->rd);
	int32_t imm1 = get_register(registers, REG_IMM1);
	int32_t imm2 = get_register(registers, REG_IMM2);
	int32_t result = 0;

	switch (decoded_instruction->opcode) {

		// Arithmetic Instructions

	case 0: // add
		result = rs + rt + rm;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

	case 1: // sub
		result = rs - rt - rm;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

	case 2: // mac
		result = (rs * rt) + rm;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

		// Logical Instructions

	case 3: // and
		result = rs & rt & rm;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

	case 4: // or
		result = rs | rt | rm;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

	case 5: // xor
		result = rs ^ rt ^ rm;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

		// Shift Instructions

	case 6: // sll
		result = (uint32_t)rs << rt;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

	case 7: // sra
		result = rs >> rt;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

	case 8: // srl
		result = (int32_t)rs >> rt;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

		// Branch Instructions

	case 9: // beq
		if (rs == rt) {
			*pc = rm & 0x0FFF; // Use the lower 12 bits of R[rm]
		}
		else {
			increment_pc(pc);
		}
		break;

	case 10: // bne
		if (rs != rt) {
			*pc = rm & 0x0FFF;
		}
		else {
			increment_pc(pc);
		}
		break;

	case 11: // blt
		if (rs < rt) {
			*pc = rm & 0x0FFF;
		}
		else {
			increment_pc(pc);
		}
		break;

	case 12: // bgt
		if (rs > rt) {
			*pc = rm & 0x0FFF;
		}
		else {
			increment_pc(pc);
		}
		break;

	case 13: // ble
		if (rs <= rt) {
			*pc = rm & 0x0FFF;
		}
		else {
			increment_pc(pc);
		}
		break;

	case 14: // bge
		if (rs >= rt) {
			*pc = rm & 0x0FFF;
		}
		else {
			increment_pc(pc);
		}
		break;

	case 15: // jal
		result = *pc + 1;
		set_register(registers, decoded_instruction->rd, result); // Save return address
		*pc = rm & 0x0FFF; // Jump to the address in the lower 12 bits of R[rm]
		break;

		// Memory Access Instructions

	case 16: // lw
		result = read_data(memory, rs + rt) + rm;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

	case 17: // sw
		write_data(memory, rs + rt, rm + rd);
		increment_pc(pc);
		break;

		// I/O and Control Instructions

	case 18: // reti
		*pc = io->IORegister[IRQRETURN]; // Read IRQRETURN
		*in_isr = 0;                                  // Mark ISR as finished
		break;

	case 19: // in
		result = io_read(io, rs + rt, hwregtrace_file); // Read from I/O register
		set_register(registers, decoded_instruction->rd, result); // Write to destination register
		increment_pc(pc);
		break;

	case 20: // out
		io_write(io, rs + rt, rm, hwregtrace_file); // Write to I/O register
		increment_pc(pc);
		break;

	case 21: // halt
		io->halt_flag = 0;
		break;

	default:
		printf("Error: Unsupported opcode %d\n", decoded_instruction->opcode);
		break;
	}
}