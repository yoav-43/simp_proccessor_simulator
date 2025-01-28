#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h> 

// Memory sizes
#define INSTRUCTION_MEM_DEPTH 4096
#define DATA_MEM_DEPTH 4096


// Structure to represent the memory
typedef struct {
	uint8_t instructions[INSTRUCTION_MEM_DEPTH][6]; // Each instruction is 6 bytes (48 bits wide)
	int32_t data[DATA_MEM_DEPTH];                  // Data memory (32 bits wide)
} Memory;


// Function declaration


/*
-Functionality: Initializes the memory. Sets all instruction and data memory lines to 0.
-parameter1: memory - Pointer to the Memory structure to initialize.
*/
void init_memory(Memory *memory);


/*
-Functionality: Loads instruction memory from a file.
-parameter1: filename - The name of the input file containing the instruction memory.
-parameter2: memory - Pointer to the Memory structure to load into.
 */
void load_instruction_memory(const char *filename, Memory *memory);


/*
-Functionality: Loads data memory from a file.
-parameter1: filename - The name of the input file containing the data memory.
-parameter2: memory -  Pointer to the Memory structure to load into.
 */
void load_data_memory(const char *filename, Memory *memory);


/*
 -Functionality: Writes the contents of data memory to a file.
-parameter1: filename - The name of the output file to write the data memory into.
-parameter2: memory - Pointer to the Memory structure containing the data memory.
 */
void write_data_memory(const char *filename, const Memory *memory);


/*
-Functionality: Reads a 48-bit instruction from instruction memory.
-return A pointer to the 48 - bit instruction(array of 6 bytes).
-parameter1: memory - Pointer to the Memory structure.
-parameter2: address - The address to read from (0 to 4095).
*/
const uint8_t *read_instruction(const Memory *memory, int address);


/*
-Functionality: Reads a 32-bit word from data memory.
-return The 32-bit word at the specified address.
-parameter1: memory - Pointer to the Memory structure.
-parameter2: address - The address to read from (0 to 4095).
*/
int32_t read_data(const Memory *memory, int address);


/*
-Functionality: Writes a 32-bit word to data memory.
-return The 32-bit word at the specified address.
-parameter1: memory - Pointer to the Memory structure to initialize.
-parameter2: address - The address to write to (0 to 4095).
-parameter3: value - The value to write.
*/
void write_data(Memory *memory, int address, int32_t value);

/*
- Functionality: Prints the contents of the data memory.
- Parameter1: memory - Pointer to the Memory structure.
*/
void print_data_memory(const Memory *memory);

/*
- Functionality: Prints the contents of the instruction memory.
- Parameter1: memory - Pointer to the Memory structure.
*/
void print_instruction_memory(const Memory *memory);

#endif 