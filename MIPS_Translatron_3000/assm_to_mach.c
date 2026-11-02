/**
 * @file assm_to_mach.c
 * @brief File containing the logic to convert from assembly to a machine instruction
 * @author Cameron Hannay
 */

 // Comments added by Jacob Lancaster

#include "include.h"

/**
 * @brief Find the index of the instruction of the given name.
 * @param given_name The name of the op entered by the user.
 * @param error Pointer to string to hold error message if an error occurs.
 * @return Index to instruction_definitions corresponding to the given name when error isn't
 * set. Undefined when error is set.
 */
static size_t find_instruction_by_name(char *given_name, char **error);

/**
 * @brief Verify that the type of a given argument matches the type of the part of the
 * instruction it is supposed to be, and that its value does not exceed the limits for its
 * type.
 * @param part The part of the instruction the value is supposed to go to.
 * @param given_type The type of the argument.
 * @param given_val The value of the argument.
 * @param error Pointer to string to hold error message if the given argument is of the wrong
 * type or exceeds its limits.
 */
static void check_instruction_part(enum instruction_part part,
		enum assm_parse_result_type given_type, uint32_t given_val, char **error);



// Converts a parsed MIPS assembly instruction into its 32-bit machine code representation
uint32_t convert_to_machine_code(struct assm_parse_result parsed, char **error)
{
	*error = NULL;

	// Returns the index of the given instruction, or sets an error if it doesn't exist
	size_t ins = find_instruction_by_name(parsed.op_name, error);
	if (*error != NULL)
		return UNDEFINED;

	uint32_t result = 0;

	// Shifts the opcode of the instruction left by 26 bits and combines it with result
	// This is so that the opcode is in the correct place, bits 26-31
	result |= bin_to_num(instruction_definitions[ins].op_code) << 26;

	// If it's an R-Type instruction, combine the function code with the result
	// This is so that bits 0-5 are the function code. No shifting is needed
	if (instruction_definitions[ins].type == R_TYPE)
		result |= bin_to_num(instruction_definitions[ins].funct_code);

	// Iterates through each argument of the instruction
	for (size_t i = 0; i < 4; i++) {

		// Check whether the argument input is valid, and return UNDEFINED if it's invalid or missing
		check_instruction_part(instruction_definitions[ins].parts[i],
				parsed.types[i], parsed.vals[i], error);

		if (*error != NULL)
			return UNDEFINED;

		// Shift the argument of the instruction a certain number of bits depending on its type and combine it with the result
		// ie RD argument is shifted to be in bits 11-15
		switch (instruction_definitions[ins].parts[i]) {
		case RD:
			result |= parsed.vals[i] << 11;
			break;
		case RS:
			result |= parsed.vals[i] << 21;
			break;
		case RT:
			result |= parsed.vals[i] << 16;
			break;
		case SA:
			result |= parsed.vals[i] << 6;
			break;
		case IMM:
		case TAR:
			result |= parsed.vals[i];
			break;
		case EMPTY:
			break;
		}
	}

	return result;
}

// Returns the index of the given instruction, if it exists. Otherwise, returns an error
static size_t find_instruction_by_name(char *given_name, char **error)
{
	size_t ins;
	bool found = false;

	// Check and see if the given instruction exists within the list of valid instructions
	for (ins = 0; ins < sizeof(instruction_definitions)
			/ sizeof(struct instruction_definition); ins++) {

		// If instruction is found, the index of the instruction is returned
		if (strcmp(given_name, instruction_definitions[ins].op_name) == 0) {
			found = true;
			break;
		}
	}

	// If instruction wasn't found, return an error
	if (found == false)
		*error = "No instruction of given name.";

	return ins;
}

// Checks whether an instruction argument has the correct type and value
static void check_instruction_part(enum instruction_part part,
		enum assm_parse_result_type given_type, uint32_t given_val, char **error)
{
	switch (part) {
	
	// For these 4 instruction parts, checks whether a register is missing or if the provided value is invalid
	case RD:
	case RS:
	case RT:
	case SA:
		if (given_type != REGISTER) {
			*error = "Missing register.";
			return;
		}
		if (given_val > MAX_5_BIT) {
			*error = "Invalid register.";
			return;
		}
		break;

	// Checks whether an immediate value is missing or if its invalid
	case IMM:
		if (given_type != IMMEDIATE) {
			*error = "Missing immediate.";
			return;
		}
		if (given_val > MAX_16_BIT) {
			*error = "Invalid immediate.";
			return;
		}
		break;

	// Checks whether the target field is missing or if the target is invalid
	case TAR:
		if (given_type != TARGET) {
			*error = "Missing target.";
			return;
		}
		if (given_val > MAX_26_BIT) {
			*error = "Invalid target.";
			return;
		}
		break;

    // Checks whether an argument was provided when none is expected
	case EMPTY:
		if (given_type != NONE) {
			*error = "Too many arguments.";
			return;
		}
	}
}
