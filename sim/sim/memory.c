#define _CRT_SECURE_NO_WARNINGS
#include "memory.h"
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>


// Initialize the memory
void init_memory(Memory *memory) {
	// Set all instruction memory lines to 0
	for (int i = 0; i < INSTRUCTION_MEM_DEPTH; i++) {
		memset(memory->instructions[i], 0, sizeof(memory->instructions[i]));
	}

	// Set all data memory lines to 0
	for (int i = 0; i < DATA_MEM_DEPTH; i++) {
		memory->data[i] = 0;
	}
}

// Load the instruction memory
void load_instruction_memory(const char *filename, Memory *memory) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		printf("Error: Could not open instruction memory file: %s\n", filename);
		exit(1);
	}

	char line[13]; // Buffer for 12 hex digits + null terminator
	int address = 0;

	while (fgets(line, sizeof(line), file) && address < INSTRUCTION_MEM_DEPTH) {
		line[strcspn(line, "\r\n")] = '\0'; // Remove newline characters

		// Verify line length
		if (strlen(line) != 12) {
			continue; // Skip invalid lines
		}

		// Parse each instruction byte
		for (int i = 0; i < 6; i++) {
			char byte_str[3] = { line[i * 2], line[i * 2 + 1], '\0' };
			memory->instructions[address][i] = (uint8_t)strtoll(byte_str, NULL, 16);
		}
		address++;
	}

	fclose(file);
}


// Load the data memory
void load_data_memory(const char *filename, Memory *memory) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		printf("Error: Could not open data memory file: %s\n", filename);
		exit(1);
	}

	char line[9]; // 8 hex digits + null terminator
	int address = 0;

	while (fgets(line, sizeof(line), file) && address < DATA_MEM_DEPTH) {
		line[strcspn(line, "\r\n")] = '\0'; // Remove newline characters

		// Check for valid line length (8 hex digits)
		if (strlen(line) != 8) {
			continue; // Skip invalid lines
		}

		// Convert the valid line to a 32-bit integer and store in memory
		memory->data[address] = (int32_t)strtoll(line, NULL, 16);
		address++;
	}

	fclose(file);
}


void write_data_memory(const char *filename, const Memory *memory) {
	// Step 1: Find the last non-zero entry in the data memory
	int last_non_zero_index = -1; // Initialize to -1 to handle the case of all zeros
	for (int i = 0; i < DATA_MEM_DEPTH; i++) {
		if (memory->data[i] != 0) {
			last_non_zero_index = i; // Update the last non-zero index
		}
	}

	// Step 2: Open the file for writing
	FILE *file = fopen(filename, "w");
	if (!file) {
		printf("Error: Could not open data memory output file: %s\n", filename);
		exit(1);
	}

	// Step 3: Write to the file only up to the last non-zero index
	for (int i = 0; i <= last_non_zero_index; i++) {
		fprintf(file, "%08X\n", memory->data[i]);
	}

	// Close the file
	fclose(file);
}


// Read a 48-bit instruction from instruction memory
const uint8_t *read_instruction(const Memory *memory, int address) {
	if (address < 0 || address >= INSTRUCTION_MEM_DEPTH) {
		printf("Error: Invalid instruction memory address %d\n", address);
		return NULL;
	}
	return memory->instructions[address];
}

// Read a 32-bit word from data memory
int32_t read_data(const Memory *memory, int address) {
	if (address < 0 || address >= DATA_MEM_DEPTH) {
		printf("Error: Invalid data memory address %d\n", address);
		return 0;
	}
	return memory->data[address];
}

// Write a 32-bit word to data memory
void write_data(Memory *memory, int address, int32_t value) {
	if (address < 0 || address >= DATA_MEM_DEPTH) {
		printf("Error: Invalid data memory address %d\n", address);
		return;
	}
	memory->data[address] = value;
}

// Print the input data memory struct
void print_data_memory(const Memory *memory) {
	printf("Data Memory:\n");
	for (int i = 0; i < DATA_MEM_DEPTH; i++) {
		// Read the data at address `i` and print it in hexadecimal
		printf("Address %03X: %08X\n", i, memory->data[i]);
	}
	printf("\n");
}

// Print the input instruction memory struct
void print_instruction_memory(const Memory *memory) {
	printf("Instruction Memory:\n");
	for (int i = 0; i < INSTRUCTION_MEM_DEPTH; i++) {
		// Print the address and the 6-byte instruction in hexadecimal
		printf("Address %03X: %02X %02X %02X %02X %02X %02X\n",
			i,
			memory->instructions[i][0],
			memory->instructions[i][1],
			memory->instructions[i][2],
			memory->instructions[i][3],
			memory->instructions[i][4],
			memory->instructions[i][5]);
	}
	printf("\n");
}