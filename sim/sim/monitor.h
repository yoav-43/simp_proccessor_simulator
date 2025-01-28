#ifndef MONITOR_H
#define MONITOR_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "io.h"

// Monitor constants
#define MONITOR_WIDTH  256   // Monitor width in pixels
#define MONITOR_HEIGHT 256   // Monitor height in pixels

// Monitor structure
typedef struct {
	uint8_t frame_buffer[MONITOR_HEIGHT][MONITOR_WIDTH]; // 256x256 pixels
} Monitor;

// Function declarations

/*
- Functionality: Initializes the monitor's frame buffer by setting all pixels to black (0).
- Parameter1: monitor - Pointer to the Monitor structure.
*/
void init_monitor(Monitor *monitor);

/*
- Functionality: Writes a pixel to the monitor's frame buffer.
- Parameter1: monitor - Pointer to the Monitor structure.
- Parameter2: io - Pointer to the IORegisters structure containing MONITORADDR, MONITORDATA, and MONITORCMD.
*/
void write_pixel(Monitor *monitor, IORegisters *io);

/*
- Functionality: Writes the monitor's frame buffer to a text file.
- Parameter1: monitor - Pointer to the Monitor structure.
- Parameter2: filename - Name of the output text file (e.g., "monitor.txt").
*/
void write_monitor_text(const Monitor *monitor, const char *filename);

/*
- Functionality: Writes the monitor's frame buffer to a binary YUV file.
- Parameter1: monitor - Pointer to the Monitor structure.
- Parameter2: filename - Name of the output binary file (e.g., "monitor.yuv").
*/
void write_monitor_yuv(const Monitor *monitor, const char *filename);

/*
- Functionality: Prints the contents of the monitor's frame buffer to the console in a human-readable format.
- Parameter1: monitor - Pointer to the Monitor structure.
*/
void print_monitor(const Monitor *monitor);

#endif 