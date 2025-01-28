#define _CRT_SECURE_NO_WARNINGS
#include "io.h"
#include <string.h>
#include <stdio.h>

// Helper function to map register indices to names
const char *io_register_name(int reg_index) {
	switch (reg_index) {
	case 0: return "irq0enable";
	case 1: return "irq1enable";
	case 2: return "irq2enable";
	case 3: return "irq0status";
	case 4: return "irq1status";
	case 5: return "irq2status";
	case 6: return "irqhandler";
	case 7: return "irqreturn";
	case 8: return "clks";
	case 9: return "leds";
	case 10: return "display7seg";
	case 11: return "timerenable";
	case 12: return "timercurrent";
	case 13: return "timermax";
	case 14: return "diskcmd";
	case 15: return "disksector";
	case 16: return "diskbuffer";
	case 17: return "diskstatus";
	case 18: return "reserved1";
	case 19: return "reserved2";
	case 20: return "monitoraddr";
	case 21: return "monitordata";
	case 22: return "monitorcmd";
	default: return "unknown";
	}
}

// Write to the hardware trace file
void write_hardware_trace(FILE *hwregtrace_file, uint32_t cycle, const char *operation, const char *reg_name, uint32_t value) {
	if (!hwregtrace_file) {
		printf("Error: hwregtrace_file is NULL. Cannot write trace.\n");
		return;
	}

	if (!reg_name) {
		printf("Error: reg_name is NULL. Invalid register index.\n");
		return;
	}

	fprintf(hwregtrace_file, "%d %s %s %08x\n", cycle, operation, reg_name, value);
}


// Initialize all I/O registers to 0
void init_io(IORegisters *io) {
	memset(io->IORegister, 0, sizeof(io->IORegister)); // Zero out all I/O registers
	io->halt_flag = 1; // Set the halt flag to 1 (default running state)
}

// Read a value from an I/O register
uint32_t io_read(const IORegisters *io, int reg_index, FILE *hwregtrace_file) {
	if (reg_index < 0 || reg_index >= NUM_IO_REGISTERS) {
		printf("Error: Invalid I/O register index %d\n", reg_index);
		return 0;
	}

	uint32_t value = io->IORegister[reg_index];

	if (!hwregtrace_file) {
		printf("Error: hwregtrace_file is NULL.\n");
	}
	else {
		// Log the read operation
		write_hardware_trace(hwregtrace_file, io->IORegister[CLKS], "READ", io_register_name(reg_index), value);
	}

	return value;
}

// Write a value to an I/O register, respecting its bit width
void io_write(IORegisters *io, int reg_index, uint32_t value, FILE *hwregtrace_file) {
	if (reg_index < 0 || reg_index >= NUM_IO_REGISTERS) {
		printf("Error: Invalid I/O register index %d\n", reg_index);
		return;
	}

	int bit_width = IO_REGISTER_SIZES[reg_index]; // Get the bit width of the register
	if (bit_width > 0) {
		uint32_t mask = (bit_width == 32) ? 0xFFFFFFFF : (1U << bit_width) - 1;
		io->IORegister[reg_index] = value & mask; // Apply the mask to limit the value
	}

	if (!hwregtrace_file) {
		printf("Error: hwregtrace_file is NULL.\n");
	}
	else {
		// Log the read operation
		write_hardware_trace(hwregtrace_file, io->IORegister[CLKS], "WRITE", io_register_name(reg_index), value);
	}
}

// Increment the clock counter
void increment_clock(IORegisters *io) {
	// Increment the CLKS register by 1
	io->IORegister[CLKS]++;
}

// Write the total number of clock cycles to a specified file
void write_cycles(const char *filename, const IORegisters *io) {
	FILE *file = fopen(filename, "w");
	if (!file) {
		printf("Error: Could not open %s for writing.\n", filename);
		return;
	}

	// Write the value of the CLKS register as a decimal number
	fprintf(file, "%u\n", io->IORegister[CLKS]);

	fclose(file);
}

// Update the timer registers and handle timer-related interrupts
void update_timer(IORegisters *io) {
	// Check if the timer is enabled
	if (io->IORegister[TIMERENABLE] == 1) {
		// Increment the TIMERCURRENT register
		io->IORegister[TIMERCURRENT]++;

		// If TIMERCURRENT matches TIMERMAX, trigger an interrupt and reset TIMERCURRENT
		if (io->IORegister[TIMERCURRENT] == io->IORegister[TIMERMAX]) {
			io->IORegister[IRQ0STATUS] = 1;   // Set IRQ0STATUS to trigger IRQ0
			io->IORegister[TIMERCURRENT] = 0; // Reset TIMERCURRENT to 0
		}
	}
}