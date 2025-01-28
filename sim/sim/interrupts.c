#define _CRT_SECURE_NO_WARNINGS
#include "interrupts.h"
#include <stdio.h>
#include <stdlib.h>

// Load IRQ2 events from a file
void load_irq2_events(const char *filename, IRQ2Data *irq2) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		printf("Error: Could not open IRQ2 input file: %s\n", filename);
		exit(1);
	}

	// Initialize the IRQ2Data structure
	irq2->events = NULL;
	irq2->count = 0;
	irq2->capacity = 0;
	irq2->current_index = 0;

	int event;
	while (fscanf(file, "%d", &event) == 1) {
		// Resize the array if needed
		if (irq2->count >= irq2->capacity) {
			irq2->capacity = (irq2->capacity == 0) ? 10 : irq2->capacity * 2;
			irq2->events = realloc(irq2->events, irq2->capacity * sizeof(int));
			if (!irq2->events) {
				printf("Error: Memory allocation failed while loading IRQ2 events\n");
				fclose(file);
				exit(1);
			}
		}

		// Add the event to the array
		irq2->events[irq2->count++] = event;
	}

	fclose(file);
}

// Check and trigger IRQ2 based on the current clock cycle
void check_and_trigger_irq2(IORegisters *io, IRQ2Data *irq2, int current_cycle) {
	// Check if there are remaining events
	if (irq2->current_index < irq2->count) {
		// If the current cycle matches the next event, trigger irq2
		if (current_cycle == irq2->events[irq2->current_index]) {
			io->IORegister[IRQ2STATUS] = 1; // Set irq2status to 1
			irq2->current_index++; // Move to the next event
		}
	}
}

// Free dynamically allocated memory for IRQ2
void free_irq2_data(IRQ2Data *irq2) {
	if (irq2->events != NULL) {
		free(irq2->events);
	}
	irq2->events = NULL;
	irq2->count = 0;
	irq2->capacity = 0;
	irq2->current_index = 0;
}

// Handle interrupts
void handle_interrupts(IORegisters *io, uint16_t *pc, int *in_isr) {
	// Compute the irq signal
	int irq = (io->IORegister[IRQ0ENABLE] & io->IORegister[IRQ0STATUS]) | // irq0enable & irq0status
		(io->IORegister[IRQ1ENABLE] & io->IORegister[IRQ1STATUS]) | // irq1enable & irq1status
		(io->IORegister[IRQ2ENABLE] & io->IORegister[IRQ2STATUS]);  // irq2enable & irq2status

// If irq == 1 and not already in ISR
	if (irq == 1 && *in_isr == 0) {
		// Save the current PC to irqreturn (io->IORegister[7])
		io->IORegister[IRQRETURN] = *pc;

		// Jump to the ISR address in irqhandler (io->IORegister[6])
		*pc = io->IORegister[IRQHANDLER];

		// Mark that the CPU is now inside an ISR
		*in_isr = 1;
	}
}