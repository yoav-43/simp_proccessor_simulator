#ifndef DISK_H
#define DISK_H

#include <stdint.h>
#include "memory.h"
#include "io.h"

// Disk constants
#define SECTOR_SIZE 512		// Bytes per sector
#define NUM_OF_SECTORS 128	//Total sectors
#define WORD_SIZE 4      // 4 bytes per 32-bit word
#define LINES_PER_SECTOR (SECTOR_SIZE / WORD_SIZE)  // 128 lines per sector
#define TOTAL_WORDS (NUM_OF_SECTORS * (SECTOR_SIZE / WORD_SIZE))  // Total words on disk


// Disk structure
typedef struct {
	int timer; // Timer for disk operations
} Disk;

// Function declarations

/*
- Functionality: Initializes the disk structure and copies the input disk file to the output disk file.
- parameter1: input_filename - Name of the input file (diskin.txt).
- parameter2: output_filename - Name of the output file (diskout.txt).
- parameter3: disk - Pointer to the Disk structure.
*/
void init_disk(const char *input_filename, const char *output_filename, Disk *disk);

/*
- Functionality: Handle a read sector operation from the disk file.
- parameter1: memory - Pointer to the Memory structure.
- parameter2: io - Pointer to the IORegisters structure.
- parameter3: output_filename - Name of the output disk file (diskout.txt).
*/
void read_sector(Memory *memory, const IORegisters *io, const char *output_filename);

/*
- Functionality: Handle a write sector operation to the disk file.
- parameter1: memory - Pointer to the Memory structure.
- parameter2: io - Pointer to the IORegisters structure.
- parameter3: output_filename - Name of the output disk file (diskout.txt).
*/
void write_sector(const Memory *memory, const IORegisters *io, const char *output_filename);

/*
- Functionality: Handle the disk command and update DMA/IRQ as needed.
- parameter1: memory - Pointer to the Memory structure.
- parameter2: io - Pointer to the IORegisters structure.
- parameter3: disk - Pointer to the Disk structure.
- parameter4: output_filename - Name of the output disk file (diskout.txt).
*/
void handle_disk_command(Memory *memory, IORegisters *io, Disk *disk, const char *output_filename);

#endif 