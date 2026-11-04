/**
 * @file util.c
 * @brief Utility functions
 * @author Cameron Hannay
 */

 // Commented by Jacob Lancaster
#include "include.h"

// Converts a binary number into base 10
uint32_t bin_to_num(char *binary)
{
	uint32_t num = 0;

	// For each digit of the binary number, shift the current value 1 bit
	// and set the rightmost bit to 1 if the current bit is 1
	while (*binary != '\0') {
		num <<= 1;
		if (*binary == '1')
			num |= 1;
		binary++;
	}

	return num;
}


const uint32_t ARG_AUTO 	= 1;
const uint32_t ARG_REVERSE	= 2;

// Reads command-line arguments to identify program flags and input/output filenames
uint32_t command_line_args(int argc, char *argv[], char** in_filename, char** out_filename)
{
	uint32_t result = 0;
	// Iterate through each argument
	for (int i = 1; i < argc; i++) {
		// If the argument is -a, set ARG_AUTO bit to 1 in the result
		if (!strcmp(argv[i], "-a"))
			result = result | ARG_AUTO;
		// If the argument is -r, set ARG_REVERSE bit to 1 in the result
		else if (!strcmp(argv[i], "-r"))
			result = result | ARG_REVERSE;
		// If the argument isn't a flag and the input filename hasn't been set yet,
		// assign the current argument as the input filename
		else if (*in_filename == NULL)
			*in_filename = argv[i];
		// If the argument isn't a flag and the output filename hasn't been set yet,
		// assign the current argument as the output filename
		else if (*out_filename == NULL)
			*out_filename = argv[i];
	}
	return result;
}

// Gets the next input based on the program's current mode and state
void get_next_input(char* line, enum InteractiveState* int_state, uint32_t flags, char* in_filename, FILE** file)
{
	// Set auto and reverse mode if their corresponding flags are set
	uint32_t auto_mode = (flags & ARG_AUTO) > 0;
	uint32_t reverse_mode = (flags & ARG_REVERSE) > 0;
	
	char* str;

	// If auto mode is set and there's an input filename set, read from the file
	if (auto_mode && in_filename != NULL) {
		// Open the file if it isn't already
		if (*file == NULL)
			*file = fopen(in_filename, "r");
		
		// Read the next line from the file and store it in line
		str = fgets(line, LINE_BUFF_SIZE, *file);

		// If it's the end of the file, exit the program
		if (str == NULL && feof(*file)) {
			if (*file != NULL)
				fclose(*file);
			exit(0);
		}
		// If there was an error reading the file, print an error and exit the program
		else if (str == NULL && ferror(*file)) {
			printf("Invalid line");
			if (*file != NULL)
				fclose(*file);
			exit(1);
		}
		// If the line is empty, recursively call the function to read the next line
		else if (*line == '\n') {
			get_next_input(line, int_state, flags, in_filename, file);
		}
		// Replace the newline character with C's null terminator
		line[strcspn(line, "\n")] = 0;
		return;
	}

	// If auto mode is set but the input filename isn't set, read from the keyboard
	else if (auto_mode && in_filename == NULL) {
		// Read the next line from the keyboard and store it in str
		str = fgets(line, LINE_BUFF_SIZE, stdin);

		// If the end of keyboard input is reached, exit the program
		if (str == NULL && feof(stdin)) {
			if (stdin != NULL)
				fclose(stdin);
			exit(0);
		}
		
		// If an invalid line is read, print an error and exit the program
		else if (str == NULL && ferror(stdin)) {
			printf("Invalid line");
			if (stdin != NULL)
				fclose(stdin);
			exit(1);
		}

		// If the line is empty, recursively call the function to read the next line
		else if (*line == '\n') {
			get_next_input(line, int_state, flags, in_filename, file);
		}

		// If no newline character is found or the input couldn't be read,
		// return an error and exit the program
		if (strchr(line, '\n') == NULL || str == NULL) {
			printf("Invalid line");
			if (*file != NULL)
				fclose(*file);
			exit(1);
		}
		// Replace the newline character with C's null terminator
		line[strcspn(line, "\n")] = 0;
		return;
	}

	// If auto mode isn't set, let the user interact with the program
	else if (!auto_mode) {
		while (1) {
			char choice[10];
			// Check which state the user is currently in
			switch (*int_state) {

				// If the user is in the ROOT state, ask the user what they wish to do
				case ROOT:
					printf("\nPlease enter an option:\n\t(1) Assembly to Machine Code\n\t(2) Machine Code to Assembly\n\t(3) Quit\n\t(4) Corrupted Code Inspector\n");
					printf("\n> ");
					choice[0] = 0;

					// Set str to the input, and prompt again if it was invalid
					str = fgets(choice, sizeof(choice), stdin);
					if (strchr(choice, '\n') == NULL || str == NULL) {
						printf("Invalid input");
						*int_state = ROOT;
						continue;
					}

					// Replace the newline character of the input with a null terminator 
					choice[strcspn(choice, "\n")] = 0;

					// Set the user's state to the state corresponding to their input number
					if (!strcmp(choice, "1")) {
						*int_state = ASM_TO_MACH;
					}
					else if (!strcmp(choice, "2")) {
						*int_state = MACH_TO_ASM;
					}
					else if (!strcmp(choice, "3")) {
						// If a file is open, close the file and then exit the program
						if (*file != NULL)
							fclose(*file);
						exit(0);
					}
					else if (!strcmp(choice, "4")) {
						*int_state = DEBUG;
					}
					// Prompt again if the user's input was invalid
					else {
						printf("Invalid input");
						*int_state = ROOT;
					}
					break;
			
				// If the user is in the MACH_TO_ASM state, prompt them on what conversion they wish to make
				case MACH_TO_ASM:
					printf("\nPlease select an option:\n\t(1) Hexadecimal to Assembly\n\t(2) Binary to Assembly\n\t[3] Main Menu\n");
					printf("\n> ");
					choice[0] = 0;

					// Get the user's input and prompt again if it's invalid
					str = fgets(choice, sizeof(choice), stdin);
					if (strchr(choice, '\n') == NULL || str == NULL) {
						printf("Invalid input");
						*int_state = MACH_TO_ASM;
						continue;
					}

					// Replace the newline character of the input with a null terminator
					choice[strcspn(choice, "\n")] = 0;

					// Set the user's state to the state corresponding to their input number
					if (!strcmp(choice, "1")) {
						*int_state = HEX_TO_ASM;
					}
					else if (!strcmp(choice, "2")) {
						*int_state = BIN_TO_ASM;
					}
					else if (!strcmp(choice, "3")) {
						*int_state = ROOT;
					}
					// If the input was invalid, prompt again
					else {
						printf("Invalid input");
						*int_state = MACH_TO_ASM;
					}
					break;
				
				// If the user is in the ASM_TO_MACH state, prompt them to input assembly
				case ASM_TO_MACH:
					printf("\n Enter a line of assembly:\n> ");
					// Store the user's input in str
					str = fgets(line, LINE_BUFF_SIZE, stdin);
					// Prompt again if the input was invalid
					if (strchr(line, '\n') == NULL || str == NULL) {
						printf("Invalid input");
						continue;
					}

					// Replace the newline character of the input with a null terminator 
					line[strcspn(line, "\n")] = 0;

					// If the user entered an empty line, move to the ROOT state
					if (line[0] == 0) { 
						*int_state = ROOT;
					}
					return;
				
				// If the user is in the HEX_TO_ASM state, prompt them to input hex
				case HEX_TO_ASM:
					printf("\n Enter Hex:\n> ");
					// Store the user's input in str
					str = fgets(line, LINE_BUFF_SIZE, stdin);
					// Prompt again if the input was invalid
					if (strchr(line, '\n') == NULL || str == NULL) {
						printf("Invalid input");
						continue;
					}

					// Replace the newline character of the input with a null terminator 
					line[strcspn(line, "\n")] = 0;

					// If the user entered an empty line, move to the MACH_TO_ASM state
					if (line[0] == 0) { 
						*int_state = MACH_TO_ASM;
					}
					return;
				
				// If the user is in the BIN_TO_ASM state, prompt the user to input binary
				case BIN_TO_ASM:
					printf("\n Enter Binary:\n> ");
					// Store the user's input in str
					str = fgets(line, LINE_BUFF_SIZE, stdin);
					// Prompt again if the input is invalid
					if (strchr(line, '\n') == NULL || str == NULL) {
						printf("Invalid input");
						continue;
					}

					// Replace the newline character of the input with a null terminator 
					line[strcspn(line, "\n")] = 0;
					// If the user entered an empty line, move to the MACH_TO_ASM state
					if (line[0] == 0) { 
						*int_state = MACH_TO_ASM;
					}
					return;
				
				// If the user is in the DEBUG state, prompt the user to input broken binary
				case DEBUG:
					printf("\n Enter Broken Binary:\n> ");

					// Store the user's input in str
					str = fgets(line, LINE_BUFF_SIZE, stdin);
					// Prompt again if the input is invalid
					if (strchr(line, '\n') == NULL || str == NULL) {
						printf("Invalid input");
						continue;
					}

					// Replace the newline character of the input with a null terminator 
					line[strcspn(line, "\n")] = 0;

					// If there's nothing left to be read, return to the MACH_TO_ASM state
					if (line[0] == 0) { 
						*int_state = ROOT;
					}
					return;

			}
		}
	}

	else {
		return;
	}
}
