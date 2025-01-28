#define _CRT_SECURE_NO_WARNINGS
#include "monitor.h"
#include "io.h" 

// Initialize the monitor's frame buffer to all zeros
void init_monitor(Monitor *monitor) {
	memset(monitor->frame_buffer, 0, sizeof(monitor->frame_buffer));
}

// Write a pixel to the monitor's frame buffer
void write_pixel(Monitor *monitor, IORegisters *io) {
	// Get the address offset from the MONITORADDR register
	uint16_t offset = io->IORegister[MONITORADDR];

	// Calculate row and column based on offset
	uint16_t row = offset / MONITOR_WIDTH;
	uint16_t col = offset % MONITOR_WIDTH;

	// Validate the row and column range
	if (row >= MONITOR_HEIGHT || col >= MONITOR_WIDTH) {
		printf("Error: Invalid monitor address 0x%X\n", offset);
		return;
	}

	// Write the pixel value from MONITORDATA to the frame buffer
	monitor->frame_buffer[row][col] = (uint8_t)(io->IORegister[MONITORDATA]);

	// Reset MONITORCMD (indicates the write operation is complete)
	io->IORegister[MONITORCMD] = 0;
}

// Write the monitor's frame buffer to a text file
void write_monitor_text(const Monitor *monitor, const char *filename) {
	// Step 1: Find the last non-zero pixel in the frame buffer
	int last_non_zero_row = -1;
	int last_non_zero_col = -1;

	for (int row = 0; row < MONITOR_HEIGHT; row++) {
		for (int col = 0; col < MONITOR_WIDTH; col++) {
			if (monitor->frame_buffer[row][col] != 0) {
				last_non_zero_row = row;
				last_non_zero_col = col; // Update to the latest non-zero pixel
			}
		}
	}

	// Step 2: Open the file for writing
	FILE *file = fopen(filename, "w");
	if (!file) {
		printf("Error: Could not open %s for writing.\n", filename);
		return;
	}

	// Step 3: Write the frame buffer up to the last non-zero pixel
	for (int row = 0; row <= last_non_zero_row; row++) {
		for (int col = 0; col < MONITOR_WIDTH; col++) {
			fprintf(file, "%02X\n", monitor->frame_buffer[row][col]);

			// Stop writing if the current pixel is the last non-zero pixel
			if (row == last_non_zero_row && col == last_non_zero_col) {
				fclose(file);
				return;
			}
		}
	}

	// Close the file (this point should never be reached due to the return above)
	fclose(file);
}


// Write the monitor's frame buffer to a YUV binary file
void write_monitor_yuv(const Monitor *monitor, const char *filename) {
	FILE *file = fopen(filename, "wb");
	if (!file) {
		printf("Error: Could not open %s for writing.\n", filename);
		return;
	}

	// Write the frame buffer to the file row by row
	for (int row = 0; row < MONITOR_HEIGHT; row++) {
		fwrite(monitor->frame_buffer[row], 1, MONITOR_WIDTH, file);
	}

	fclose(file);
}

// Print the monitor's frame buffer
void print_monitor(const Monitor *monitor) {
	printf("Monitor Frame Buffer:\n");
	for (int row = 0; row < MONITOR_HEIGHT; row++) {
		for (int col = 0; col < MONITOR_WIDTH; col++) {
			printf("%02X ", monitor->frame_buffer[row][col]); // Print pixel as 2-digit hex
		}
		printf("\n"); // Move to the next line after each row
	}
	printf("\n");
}