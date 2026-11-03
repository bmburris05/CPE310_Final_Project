/**
 * @file gen_assm.c
 * @brief Code to generate assembly from a parse result
 * @author Cameron Hannay

 */
 // Commented by Jacob Lancaster

#include "include.h"


// Builds assembly from the internal machine format
void generate_assembly(char *buf, size_t size, struct assm_parse_result parsed, char **error)
{
	*error = NULL;

	// Creates a 2D array
	// 4 spots for arguments
	// 7 characters + a null terminator is the maximum argument length
	char args[4][8]; 
	size_t arg_count;
	union signedness_switch_trick signedness;

	// Iterate through each argument and format it for assembly output
	for (arg_count = 0; arg_count < 4 && parsed.types[arg_count] != NONE; arg_count++) {

		switch (parsed.types[arg_count]) {
		// If the argument is a register, copy its name into the argument string
		case REGISTER:
			strcpy(args[arg_count], registers[parsed.vals[arg_count]]);
			break;
		// If the argument is an immediate, convert its value to a signed 16-bit integer
		// and format it as a string with a '#' prefix
		case IMMEDIATE:
			signedness.u = (uint16_t) parsed.vals[arg_count];
			sprintf(args[arg_count], "#%d", signedness.i);
			break;
		// If the argument is a target, it's unsupported, so return an error
		case TARGET:
			*error = "Generating assembly with targets is unsupported.";
			return;
		case NONE:
			break;
		}
	}

	// The 5 possible formats the instruction could have
	char *formats[5] = {
		"%s",
		"%s %s",
		"%s %s, %s",
		"%s %s, %s, %s",
		"%s %s, %s, %s, %s"
	};

	// Put together the assembly instruction using the correct format
	int needed = snprintf(buf, size, formats[arg_count], parsed.op_name,
			args[0], args[1], args[2], args[3]);

	// Return an error if the buffer wasn't large enough
	if (needed >= size)
		*error = "Buffer too small for assembly instruction.";
}
