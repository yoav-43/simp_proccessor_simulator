#ifndef IO_H
#define IO_H

#include <stdint.h>
#include <stdio.h> // For file handling

// Number of IO Registers
#define NUM_IO_REGISTERS 23

// IO Register Definitions
#define IRQ0ENABLE      0  // Enable IRQ0
#define IRQ1ENABLE      1  // Enable IRQ1
#define IRQ2ENABLE      2  // Enable IRQ2
#define IRQ0STATUS      3  // Status of IRQ0
#define IRQ1STATUS      4  // Status of IRQ1
#define IRQ2STATUS      5  // Status of IRQ2
#define IRQHANDLER      6  // Address of interrupt handler
#define IRQRETURN       7  // Address to return after interrupt
#define CLKS            8  // Clock cycle counter
#define LEDS            9  // LED state
#define DISPLAY7SEG     10 // 7-segment display
#define TIMERENABLE     11 // Enable/Disable timer
#define TIMERCURRENT    12 // Current timer value
#define TIMERMAX        13 // Timer max value
#define DISKCMD         14 // Disk command (Read/Write)
#define DISKSECTOR      15 // Disk sector
#define DISKBUFFER      16 // Disk buffer
#define DISKSTATUS      17 // Disk status
#define RESERVED1       18 // Reserved for future use
#define RESERVED2       19 // Reserved for future use
#define MONITORADDR     20 // Monitor address
#define MONITORDATA     21 // Monitor data
#define MONITORCMD      22 // Monitor command

// Define bit widths for each register
static const int IO_REGISTER_SIZES[NUM_IO_REGISTERS] = {
	1,  1,  1,  1,  1,  1,  12, 12, 32, 32, 32, 1, 32, 32, 2, 7, 12, 1, 32, 32, 16, 8, 1
};

// Structure for I/O registers
typedef struct {
	uint32_t IORegister[NUM_IO_REGISTERS]; // Array of I/O registers
	int halt_flag;                         // Halt flag for stopping the simulator
} IORegisters;

// Function Declarations

/*
- Functionality: Initializes all I/O registers to 0.
- Parameter1: io - Pointer to the I/O registers structure to initialize.
*/
void init_io(IORegisters *io);

/*
- Functionality: Reads a value from an I/O register and logs the operation.
- Parameter1: io - Pointer to the I/O registers structure.
- Parameter2: reg_index - The index of the I/O register to read (0 to 22).
- Parameter3: hwregtrace_file - File to log hardware register operations.
- Return: The value stored in the specified I/O register.
*/
uint32_t io_read(const IORegisters *io, int reg_index, FILE *hwregtrace_file);

/*
- Functionality: Writes a value to an I/O register, respecting its bit width, and logs the operation.
- Parameter1: io - Pointer to the I/O registers structure.
- Parameter2: reg_index - The index of the I/O register to write (0 to 22).
- Parameter3: value - The value to set in the register.
- Parameter4: hwregtrace_file - File to log hardware register operations.
*/
void io_write(IORegisters *io, int reg_index, uint32_t value, FILE *hwregtrace_file);

/*
- Functionality: Increments the clock counter.
- Parameter1: io - Pointer to the I/O registers structure.
*/
void increment_clock(IORegisters *io);

/*
- Functionality: Writes the total number of clock cycles to the specified file.
- Parameter1: filename - Name of the output file (e.g., "cycles.txt").
- Parameter2: io - Pointer to the I/O registers structure containing the clock register.
*/
void write_cycles(const char *filename, const IORegisters *io);

/*
- Functionality: Updates the timer registers and checks for timer-related interrupts.
- Parameter1: io - Pointer to the I/O registers structure.
*/
void update_timer(IORegisters *io);

#endif