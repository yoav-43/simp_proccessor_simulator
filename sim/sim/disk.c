#define _CRT_SECURE_NO_WARNINGS
#include "disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_disk(const char *input_filename, const char *output_filename, Disk *disk) {
	disk->timer = 0;  // Initialize disk timer to zero

	// Open input and output files
	FILE *input_file = fopen(input_filename, "r");
	FILE *output_file = fopen(output_filename, "w");

	if (!input_file || !output_file) {
		printf("Error: Could not open disk input or output file.\n");
		if (input_file) fclose(input_file);
		if (output_file) fclose(output_file);
		return;
	}

	char line[16];  // 8 chars for hex, 1 for '\0' and extra for safety
	int word_count = 0;

	// Read words from input and write to output file
	while (fgets(line, sizeof(line), input_file)) {
		// Remove newline character, if present
		line[strcspn(line, "\r\n")] = '\0';

		// Validate the line length (must be exactly 8 hex characters)
		if (strlen(line) != 8 || strspn(line, "0123456789ABCDEFabcdef") != 8) {
			printf("Warning: Skipping invalid line: %s\n", line);
			continue;  // Skip malformed lines
		}

		// Write the valid word to the output file in uppercase
		fprintf(output_file, "%.8s\n", line);
		word_count++;

		// Stop processing if the disk capacity is reached
		if (word_count >= TOTAL_WORDS) {
			break;
		}
	}

	fclose(input_file);
	fclose(output_file);

	printf("Disk initialized successfully with %d words.\n", word_count);
}

// Functionality: Handle a read sector operation from the disk file.
void read_sector(Memory *memory, const IORegisters *io, const char *disk_filename) {
	uint32_t sector = io->IORegister[DISKSECTOR];  // Get sector number
	uint32_t buffer = io->IORegister[DISKBUFFER];  // Get memory buffer address

	FILE *disk_file = fopen(disk_filename, "r");
	if (!disk_file) {
		printf("Error: Could not open disk file for reading.\n");
		return;
	}

	// Seek to the correct sector (each line is 8 hex chars + newline + null terimantor = 10 bytes)
	if (fseek(disk_file, sector * LINES_PER_SECTOR * 10, SEEK_SET) != 0) {
		printf("Error: Failed to seek to the correct sector in the disk file.\n");
		fclose(disk_file);
		return;
	}

	char line[16];  // 8 chars for hex, 1 for '\0' and extra for safety
	int i = 0;

	while (fgets(line, sizeof(line), disk_file) && i < LINES_PER_SECTOR) {
		// Remove newline character, if present
		line[strcspn(line, "\r\n")] = '\0';

		// Validate the line length (must be exactly 8 hex characters)
		if (strlen(line) != 8 || strspn(line, "0123456789ABCDEFabcdef") != 8) {
			printf("Warning: Skipping invalid line: %s\n", line);
			continue;  // Skip malformed lines
		}

		int32_t word = (int32_t)strtol(line, NULL, 16);
		write_data(memory, buffer + i, word);  // Store the value in memory at the correct index

		i++;
	}

	fclose(disk_file);
}

// Functionality: Handle a write sector operation to the disk file.
void write_sector(const Memory *memory, const IORegisters *io, const char *output_filename) {
	uint32_t sector = io->IORegister[DISKSECTOR];
	uint32_t buffer = io->IORegister[DISKBUFFER];

	FILE *disk_file = fopen(output_filename, "r+");  // Open file for read/write
	if (!disk_file) {
		printf("Error: Could not open disk output file for writing.\n");
		return;
	}

	// Seek to the correct sector (each line is 8 hex chars + newline + null terimantor = 10 bytes)
	if (fseek(disk_file, sector * LINES_PER_SECTOR * 10, SEEK_SET) != 0) {
		printf("Error: Failed to seek to the correct sector in the disk file.\n");
		fclose(disk_file);
		return;
	}

	// Clear the sector before writing to avoid residual data
	for (int i = 0; i < LINES_PER_SECTOR; i++) {
		fprintf(disk_file, "00000000\n");
	}

	// Reset file pointer to the start of the sector
	fseek(disk_file, sector * LINES_PER_SECTOR * 10, SEEK_SET);

	for (int i = 0; i < LINES_PER_SECTOR; i++) {  // Write 128 words (each 32-bit)
		int32_t word = read_data(memory, buffer + i);

		// Ensure writing in uppercase and correct width (8 hex digits)
		if (fprintf(disk_file, "%08x\n", word) < 0) {
			printf("Error: Failed to write data to disk.\n");
			fclose(disk_file);
			return;
		}
	}

	fclose(disk_file);
}

// Functionality: Handle the disk command and update DMA/IRQ as needed.
void handle_disk_command(Memory *memory, IORegisters *io, Disk *disk, const char *output_filename) {
	if (io->IORegister[DISKSTATUS] == 1) {
		if (disk->timer > 0) {
			disk->timer--;

			if (disk->timer == 0) {
				io->IORegister[DISKCMD] = 0; // Clear diskcmd
				io->IORegister[DISKSTATUS] = 0; // Disk ready
				io->IORegister[IRQ1STATUS] = 1; // Raise IRQ1
			}
		}
		return;
	}

	if (io->IORegister[DISKCMD] != 0) {
		switch (io->IORegister[DISKCMD]) {
		case 1: // Read sector
			read_sector(memory, io, output_filename);
			break;

		case 2: // Write sector
			write_sector(memory, io, output_filename);
			break;

		default:
			break;
		}

		disk->timer = 1024; // Start 1024-cycle countdown
		io->IORegister[DISKSTATUS] = 1; // Disk is busy
	}
}
