#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN 500
#define MAX_LABELS 50
#define MAX_MEM_SIZE 4096

typedef struct {
	char label[50];
	int address;
} LabelEntry;

LabelEntry labelTable[MAX_LABELS];
int labelCount = 0;

// Function to map register names and I/O registers to numbers
int getRegisterNumber(char *reg) {
	if (strcmp(reg, "$zero") == 0) return 0;
	if (strcmp(reg, "$imm1") == 0) return 1;
	if (strcmp(reg, "$imm2") == 0) return 2;
	if (strcmp(reg, "$v0") == 0) return 3;
	if (strcmp(reg, "$a0") == 0) return 4;
	if (strcmp(reg, "$a1") == 0) return 5;
	if (strcmp(reg, "$a2") == 0) return 6;
	if (strcmp(reg, "$t0") == 0) return 7;
	if (strcmp(reg, "$t1") == 0) return 8;
	if (strcmp(reg, "$t2") == 0) return 9;
	if (strcmp(reg, "$s0") == 0) return 10;
	if (strcmp(reg, "$s1") == 0) return 11;
	if (strcmp(reg, "$s2") == 0) return 12;
	if (strcmp(reg, "$gp") == 0) return 13;
	if (strcmp(reg, "$sp") == 0) return 14;
	if (strcmp(reg, "$ra") == 0) return 15;

	// I/O registers
	if (strcmp(reg, "irq0enable") == 0) return 0;
	if (strcmp(reg, "irq1enable") == 0) return 1;
	if (strcmp(reg, "irq2enable") == 0) return 2;
	if (strcmp(reg, "irq0status") == 0) return 3;
	if (strcmp(reg, "irq1status") == 0) return 4;
	if (strcmp(reg, "irq2status") == 0) return 5;
	if (strcmp(reg, "irqhandler") == 0) return 6;
	if (strcmp(reg, "irqreturn") == 0) return 7;
	if (strcmp(reg, "clks") == 0) return 8;
	if (strcmp(reg, "leds") == 0) return 9;
	if (strcmp(reg, "display7seg") == 0) return 10;
	if (strcmp(reg, "timerenable") == 0) return 11;
	if (strcmp(reg, "timercurrent") == 0) return 12;
	if (strcmp(reg, "timermax") == 0) return 13;
	if (strcmp(reg, "diskcmd") == 0) return 14;
	if (strcmp(reg, "disksector") == 0) return 15;
	if (strcmp(reg, "diskbuffer") == 0) return 16;
	if (strcmp(reg, "diskstatus") == 0) return 17;
	if (strcmp(reg, "monitoraddr") == 0) return 20;
	if (strcmp(reg, "monitordata") == 0) return 21;
	if (strcmp(reg, "monitorcmd") == 0) return 22;

	printf("Error: Unknown register %s\n", reg);
	exit(1);
}

// Function to map opcode mnemonics to their respective numbers
int getOpcodeNumber(char *opcode) {
	if (strcmp(opcode, "add") == 0) return 0;
	if (strcmp(opcode, "sub") == 0) return 1;
	if (strcmp(opcode, "mac") == 0) return 2;
	if (strcmp(opcode, "and") == 0) return 3;
	if (strcmp(opcode, "or") == 0) return 4;
	if (strcmp(opcode, "xor") == 0) return 5;
	if (strcmp(opcode, "sll") == 0) return 6;
	if (strcmp(opcode, "sra") == 0) return 7;
	if (strcmp(opcode, "srl") == 0) return 8;
	if (strcmp(opcode, "beq") == 0) return 9;
	if (strcmp(opcode, "bne") == 0) return 10;
	if (strcmp(opcode, "blt") == 0) return 11;
	if (strcmp(opcode, "bgt") == 0) return 12;
	if (strcmp(opcode, "ble") == 0) return 13;
	if (strcmp(opcode, "bge") == 0) return 14;
	if (strcmp(opcode, "jal") == 0) return 15;
	if (strcmp(opcode, "lw") == 0) return 16;
	if (strcmp(opcode, "sw") == 0) return 17;
	if (strcmp(opcode, "reti") == 0) return 18;
	if (strcmp(opcode, "in") == 0) return 19;
	if (strcmp(opcode, "out") == 0) return 20;
	if (strcmp(opcode, "halt") == 0) return 21;

	printf("Error: Unknown opcode %s\n", opcode);
	exit(1);
}
//searches for a given label in the labelTable and returns its corresponding memory address.
int getLabelAddress(char *label) {
	for (int i = 0; i < labelCount; i++) {
		if (strcmp(labelTable[i].label, label) == 0) {
			return labelTable[i].address;
		}
	}
	printf("Error: Undefined label %s\n", label);
	exit(1);
}

//initial scan of the input assembly file to identify and store labels with their respective line addresses,
// which are later used for resolving symbolic references during the second pass of assembly.
void firstPass(char *filename) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		printf("Error: Cannot open file %s\n", filename);
		exit(1);
	}

	char line[MAX_LINE_LEN];
	int lineNumber = 0;
	while (fgets(line, sizeof(line), file)) {
		// Remove leading and trailing whitespace
		char *ptr = line;
		while (isspace(*ptr)) ptr++;  // Skip leading spaces/tabs
		if (*ptr == '\0' || *ptr == '#' || *ptr == '.') continue;  // Ignore empty lines or comments or .word

		// Remove inline comments
		char *commentPos = strchr(ptr, '#');
		if (commentPos) *commentPos = '\0';

		// Trim trailing whitespace
		char *end = ptr + strlen(ptr) - 1;
		while (end > ptr && isspace(*end)) *end-- = '\0';

		// Check if the line contains a label (ends with ':')
		char *token = strtok(ptr, " ,\n");
		if (token == NULL) continue;  // Skip empty lines

		size_t len = strlen(token);
		if (len > 0 && token[len - 1] == ':') {
			// Validate label: must start with a letter and contain only letters/numbers
			token[len - 1] = '\0';  // Remove trailing colon

			if (!isalpha(token[0])) {
				printf("Error: Invalid label %s (must start with a letter)\n", token);
				exit(1);
			}

			// Store label and address
			strcpy(labelTable[labelCount].label, token);
			labelTable[labelCount].address = lineNumber;
			labelCount++;

			continue;  // Do not increase lineNumber for label-only lines
		}

		lineNumber++;  // Count instructions only
	}

	fclose(file);
	printf("First pass completed. Total labels found: %d\n", labelCount);
}

// Function to initialize memory and process instructions
void secondPass(char *filename, char *imemOutput, char *dmemOutput) {
	FILE *file = fopen(filename, "r");
	FILE *imemFile = fopen(imemOutput, "w");
	FILE *dmemFile = fopen(dmemOutput, "w");

	if (!file || !imemFile || !dmemFile) {
		printf("Error: Cannot open input/output files\n");
		exit(1);
	}

	char line[MAX_LINE_LEN];
	int memory[MAX_MEM_SIZE] = { 0 };  // Initialize all memory to zero
	int highestAddressWritten = -1;

	while (fgets(line, sizeof(line), file)) {
		char *ptr = line;
		while (isspace(*ptr)) ptr++;  // Trim leading spaces

		if (*ptr == '\0' || *ptr == '#') continue;  // Ignore empty lines and comments

		char *commentPos = strchr(ptr, '#');
		if (commentPos) *commentPos = '\0';

		char *end = ptr + strlen(ptr) - 1;
		while (end > ptr && isspace(*end)) *end-- = '\0';  // Trim trailing spaces

		// Handle .word directive for data memory initialization
		if (strncmp(ptr, ".word", 5) == 0) {
			ptr += 5;  // Skip ".word"
			while (isspace(*ptr)) ptr++;  // Skip spaces after ".word"

			char *addressToken = strtok(ptr, " ,\n");
			char *dataToken = strtok(NULL, " ,\n");

			if (!addressToken || !dataToken) {
				printf("Error: Invalid .word format\n");
				exit(1);
			}

			int address, data;

			// Handle decimal and hexadecimal values for address
			if (addressToken[0] == '0' && (addressToken[1] == 'x' || addressToken[1] == 'X')) {
				address = (int)strtol(addressToken, NULL, 16);
			}
			else {
				address = atoi(addressToken);
			}

			// Handle decimal and hexadecimal values for data
			if (dataToken[0] == '0' && (dataToken[1] == 'x' || dataToken[1] == 'X')) {
				data = (int)strtol(dataToken, NULL, 16);
			}
			else {
				data = atoi(dataToken);
			}

			if (address >= MAX_MEM_SIZE) {
				printf("Error: Address out of range (max: %d)\n", MAX_MEM_SIZE - 1);
				exit(1);
			}

			memory[address] = data;  // Store the value in the memory array

			if (address > highestAddressWritten) {
				highestAddressWritten = address;
			}

			continue;
		}

		// Skip label declarations
		if (strchr(ptr, ':')) continue;

		char *token = strtok(ptr, " ,\n");
		if (token == NULL) continue;

		int opcode = getOpcodeNumber(token);
		int rd = getRegisterNumber(strtok(NULL, " ,\n"));
		int rs = getRegisterNumber(strtok(NULL, " ,\n"));
		int rt = getRegisterNumber(strtok(NULL, " ,\n"));
		int rm = getRegisterNumber(strtok(NULL, " ,\n"));

		char *imm1Token = strtok(NULL, " ,\n");
		char *imm2Token = strtok(NULL, " ,\n");

		if (imm1Token == NULL || imm2Token == NULL) {
			printf("Error: Missing immediate values in instruction.\n");
			exit(1);
		}

		int imm1 = 0, imm2 = 0;

		// Handle imm1: should always be a number (decimal or hex)
		if (imm1Token[0] == '0' && (imm1Token[1] == 'x' || imm1Token[1] == 'X')) {
			imm1 = (int)strtol(imm1Token, NULL, 16);
		}
		else if (isalpha(imm1Token[0])) {
			imm1 = getLabelAddress(imm1Token);
		}
		else {
			imm1 = atoi(imm1Token);
		}

		// Handle imm2: can be a number or a label
		if (imm2Token[0] == '0' && (imm2Token[1] == 'x' || imm2Token[1] == 'X')) {
			imm2 = (int)strtol(imm2Token, NULL, 16);
		}
		else if (isalpha(imm2Token[0])) {
			imm2 = getLabelAddress(imm2Token);
		}
		else {
			imm2 = atoi(imm2Token);
		}

		// Ensure imm1 and imm2 are within 12-bit limits (0-4095)
		imm1 &= 0xFFF;
		imm2 &= 0xFFF;

		unsigned long long instruction =
			((unsigned long long)opcode << 40) |
			((unsigned long long)rd << 36) |
			((unsigned long long)rs << 32) |
			((unsigned long long)rt << 28) |
			((unsigned long long)rm << 24) |
			((signed long long)imm1 << 12) |
			(signed long long)imm2;

		fprintf(imemFile, "%012llX\n", instruction);
	}

	// Write initialized memory to dmemOutput file
	for (int i = 0; i <= highestAddressWritten; i++) {
		fprintf(dmemFile, "%08X\n", memory[i]);
	}

	fclose(file);
	fclose(imemFile);
	fclose(dmemFile);
	printf("Second pass completed. Machine code written to %s and data memory written to %s\n", imemOutput, dmemOutput);
}

int main(int argc, char *argv[]) {
	if (argc != 4) {
		printf("Usage: %s file_name.asm imemin.txt dmemin.txt \n", argv[0]);
		return 1;
	}

	firstPass(argv[1]);
	secondPass(argv[1], argv[2], argv[3]);
	printf("Assembly completed successfully!\n");
	return 0;
}