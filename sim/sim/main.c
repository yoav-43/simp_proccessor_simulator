#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>   
#include <stdio.h>    
#include <stdlib.h>   
#include "registers.h"   
#include "memory.h"      
#include "io.h"         
#include "disk.h" 
#include "monitor.h" 
#include "interrupts.h"  
#include "instruction_fetch.h"
#include "instruction_decode.h" 
#include "execution.h" 


// Log simulator status
void write_trace(FILE *file, uint16_t pc, const uint8_t *instruction_line, const Registers *registers) {
	// Print PC (3 hexadecimal digits)
	fprintf(file, "%03X ", pc);

	// Print the 48-bit instruction as 12 hexadecimal digits
	for (int i = 0; i < 6; i++) {
		fprintf(file, "%02X", instruction_line[i]);
	}
	fprintf(file, " "); // Add a space after the instruction

	// Print all registers R0 to R15
	for (int i = 0; i <= 15; i++) {
		if (i == 15) {
			// Print without trailing space for the last register
			fprintf(file, "%08x", registers->regs[i]);
		}
		else {
			// Print normally with a trailing space
			fprintf(file, "%08x ", registers->regs[i]);
		}
	}

	// End the line
	fprintf(file, "\n");
}

// Log leds
void log_leds(FILE *leds_file, const IORegisters *io) {
	static uint32_t previous_leds = 0; // Keep track of the previous state of the LEDs

	// Check if the LEDs register has changed
	if (io->IORegister[LEDS] != previous_leds) {
		// Write the clock cycle (decimal) and the current LED state (hexadecimal) to the file
		fprintf(leds_file, "%d %08x\n", io->IORegister[CLKS], io->IORegister[LEDS]);

		// Update the previous state to the current state
		previous_leds = io->IORegister[LEDS];
	}
}

// Log display7seg
void log_display7seg(FILE *display7seg_file, const IORegisters *io) {
	static uint32_t previous_display7seg = 0; // Keep track of the previous state of the 7-segment display

	// Check if the 7-segment display register has changed
	if (io->IORegister[DISPLAY7SEG] != previous_display7seg) {
		// Write the clock cycle (decimal) and the current 7-segment display state (hexadecimal) to the file
		fprintf(display7seg_file, "%d %08x\n", io->IORegister[CLKS], io->IORegister[DISPLAY7SEG]);

		// Update the previous state to the current state
		previous_display7seg = io->IORegister[DISPLAY7SEG];
	}
}

// The simulator fetch-decode-exe loop
void simulator_main_loop(
	Registers *registers,
	Memory *memory,
	IORegisters *io,
	IRQ2Data *irq2,
	Monitor *monitor,
	Disk *disk,
	const char *diskout_filename,
	const char *trace_filename,
	const char *hwregtrace_filename,
	const char *leds_filename,
	const char *display7seg_filename) {
	uint16_t pc = 0;        // Program counter (12-bit)
	int in_isr = 0;         // ISR state (0 = not in ISR, 1 = in ISR)
	Instruction decoded;    // Decoded instruction

	// Open trace and hardware register trace files
	FILE *trace_file = fopen(trace_filename, "w");
	if (!trace_file) {
		printf("Error: Could not open trace file: %s\n", trace_filename);
		return;
	}

	FILE *hwregtrace_file = fopen(hwregtrace_filename, "w");
	if (!hwregtrace_file) {
		printf("Error: Could not open hardware register trace file: %s\n", hwregtrace_filename);
		fclose(trace_file); // Close the already opened file
		return;
	}

	// Open LED and 7-segment display files
	FILE *leds_file = fopen(leds_filename, "w");
	if (!leds_file) {
		printf("Error: Could not open LEDs file: %s\n", leds_filename);
		fclose(trace_file);
		fclose(hwregtrace_file);
		return;
	}

	FILE *display7seg_file = fopen(display7seg_filename, "w");
	if (!display7seg_file) {
		printf("Error: Could not open 7-segment display file: %s\n", display7seg_filename);
		fclose(trace_file);
		fclose(hwregtrace_file);
		fclose(leds_file);
		return;
	}

	while (io->halt_flag) {

		// Fetch the next instruction using the 12-bit PC
		const uint8_t* instruction = fetch_instruction(memory, &pc);

		// Decode the fetched instruction
		decode_instruction(instruction, &decoded, registers);

		// Update the trace file 
		write_trace(trace_file, pc, instruction, registers);

		// Execute the decoded instruction
		execute_instruction(&decoded, registers, memory, io, &pc, &in_isr, hwregtrace_file);

		// Update the timer (option for interrupt 0)
		update_timer(io);

		// Manage disk operations (read/write tasks) (option for interrupt 1)
		handle_disk_command(memory, io, disk, diskout_filename);

		// Check and trigger IRQ2 based on the current clock cycle (option for interrupt 2)
		check_and_trigger_irq2(io, irq2, io->IORegister[CLKS]);

		// Handle interrupts if any are pending
		handle_interrupts(io, &pc, &in_isr);

		// Handle monitor commands
		if (io->IORegister[MONITORCMD] == 1) {
			write_pixel(monitor, io); // Write the pixel to the monitor
		}

		// Update the LEDs file 
		log_leds(leds_file, io);

		// Update the 7-segment file
		log_display7seg(display7seg_file, io);

		// Increment the clock register
		increment_clock(io);
	}

	// Add the remaining disk timer to the clock cycles
	io->IORegister[CLKS] += disk->timer;

	// Close all opened files
	fclose(trace_file);
	fclose(hwregtrace_file);
	fclose(leds_file);
	fclose(display7seg_file);
}

// Writes the values of registers R3�R15 to an output file.
void write_registers(const char *filename, const Registers *registers) {
	FILE *file = fopen(filename, "w");
	if (!file) {
		printf("Error: Could not open %s for writing.\n", filename);
		return;
	}

	// Write the values of registers R3�R15 (indices 3 to 15)
	for (int i = 3; i <= 15; i++) {
		fprintf(file, "%08X\n", registers->regs[i]);
	}

	fclose(file);
}

int main(int argc, char *argv[]) {
	// Ensure the correct number of arguments are passed
	if (argc != 15) {
		printf("Usage: %s imemin.txt dmemin.txt diskin.txt irq2in.txt dmemout.txt regout.txt trace.txt hwregtrace.txt cycles.txt leds.txt display7seg.txt diskout.txt monitor.txt monitor.yuv\n", argv[0]);
		return 1;
	}

	// Parse input and output file names
	const char *imemin = argv[1];      // Instruction memory input file
	const char *dmemin = argv[2];      // Data memory input file
	const char *diskin = argv[3];      // Disk content input file
	const char *irq2in = argv[4];      // IRQ2 events input file
	const char *dmemout = argv[5];     // Data memory output file
	const char *regout = argv[6];      // Registers output file
	const char *trace = argv[7];       // Instruction trace output file
	const char *hwregtrace = argv[8];  // Hardware register trace output file
	const char *cycles = argv[9];      // Clock cycle count output file
	const char *leds = argv[10];       // LED state output file
	const char *display7seg = argv[11];// 7-segment display output file
	const char *diskout = argv[12];    // Disk content output file
	const char *monitor_txt = argv[13];// Monitor text output file
	const char *monitor_yuv = argv[14];// Monitor YUV binary output file


	printf("Initializing...\n");

	// Initialize general-purpose registers
	Registers registers;
	init_registers(&registers);
	printf("Registers Initialized.\n");

	// Initialize instruction and data memory
	Memory memory;
	init_memory(&memory);
	load_instruction_memory(imemin, &memory);
	//print_instruction_memory(&memory);  // Print instruction memory for debugging
	load_data_memory(dmemin, &memory);
	//print_data_memory(&memory);  // Print data memory for debugging

	printf("Memory Loaded.\n");

	// Initialize IO registers
	IORegisters io;
	init_io(&io);
	printf("IORegisters Initialized.\n");

	// Initialize diskout
	Disk disk;
	init_disk(diskin, diskout, &disk);
	printf("Disk Loaded and Disk timer Initialized.\n");

	// Initialize monitor structure
	Monitor monitor;
	init_monitor(&monitor);
	//print_monitor(&monitor);
	printf("Monitor Initialized.\n");

	// Initialize IRQ2 events
	IRQ2Data irq2;
	load_irq2_events(irq2in, &irq2);
	printf("irq2_events Initialized.\n\n");

	// Call the main simulation loop
	printf("Starting simulation...\n");
	simulator_main_loop(&registers, &memory, &io, &irq2, &monitor, &disk, diskout, trace, hwregtrace, leds, display7seg);
	printf("Simulation completed.\n\n");

	// Write output files
	printf("Writing output files:\n");
	write_data_memory(dmemout, &memory);       // Write data memory output
	printf("Successfully wrote data memory output.\n");

	write_registers(regout, &registers);	   // Write registers output
	printf("Successfully wrote registers output.\n");

	write_monitor_text(&monitor, monitor_txt); // Write monitor text file
	printf("Successfully wrote monitor text file.\n");

	write_monitor_yuv(&monitor, monitor_yuv);  // Write monitor YUV file
	printf("Successfully wrote monitor YUV file.\n");

	write_cycles(cycles, &io);				   // Write clock cycles output
	printf("Successfully wrote clock cycles output.\n\n");

	printf("Exit program.\n");
	return 0;
}