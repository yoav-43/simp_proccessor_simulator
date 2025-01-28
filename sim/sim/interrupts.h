#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>
#include "io.h"
#include "memory.h"

// Structure for IRQ2 interrupt events (external interrupt).
typedef struct {
	int *events;         // Pointer to dynamically allocated array for events
	int count;           // Number of events loaded
	int capacity;        // Allocated capacity of the array
	int current_index;   // Index of the next event to trigger
} IRQ2Data;


// Function declaration


/*
- Functionality: Loads IRQ2 events from an input file.
- Parameter1: filename - Path to the input file specifying IRQ2 trigger clock cycles.
- Parameter2: irq2 - Pointer to the IRQ2Data structure to populate.
*/
void load_irq2_events(const char *filename, IRQ2Data *irq2);

/*
- Functionality: Checks and triggers IRQ2 based on the current clock cycle.
- Parameter1: io - Pointer to the IORegisters structure.
- Parameter2: irq2 - Pointer to the IRQ2Data structure containing event information.
- Parameter3: current_cycle - Current clock cycle of the simulation.
*/
void check_and_trigger_irq2(IORegisters *io, IRQ2Data *irq2, int current_cycle);

/*
- Functionality: Frees the dynamically allocated memory in the IRQ2Data structure.
- Parameter1: irq2 - Pointer to the IRQ2Data structure to free.
*/
void free_irq2_data(IRQ2Data *irq2);

/*
- Functionality: Handles interrupts (IRQ0, IRQ1, IRQ2) during each clock cycle.
- Parameter1: io - Pointer to the IORegisters structure.
- Parameter2: pc - Pointer to the program counter.
- Parameter3: in_isr - Pointer to an integer flag indicating if the CPU is in an ISR (1 = true, 0 = false).
*/
void handle_interrupts(IORegisters *io, uint16_t *pc, int *in_isr);

#endif 