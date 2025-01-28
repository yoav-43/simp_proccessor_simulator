#ifndef EXECUTION_H
#define EXECUTION_H

#include <stdint.h>
#include "registers.h" // To access and modify the registers
#include "memory.h"    // For memory-related instructions
#include "io.h" // To access and modify the io registers
#include "instruction_fetch.h" // For the pc handaling
#include "instruction_decode.h" // For the decoded instruction


// Function declaration

/*
-Functionality: Executes a decoded instruction.
-parameter1: decoded_instruction - Pointer to the decoded instruction.
-parameter2: registers - Pointer to the Registers structure.
-parameter3: memory - Pointer to the Memory structure.
-parameter4: IORegister - Pointer to the IO Registers structure.
-parameter5: pc - Pointer to the Program counter.
-parameter6: in_isr - Pointer to the flag the indicates if the code is in the ISR.
-parameter7: hwregtrace_file - Pointer to the the hardware registers logging file.
*/
void execute_instruction(const Instruction *decoded_instruction, Registers *registers, Memory *memory, IORegisters *IORegister, uint16_t *pc, int *in_isr, FILE *hwregtrace_file);


#endif 