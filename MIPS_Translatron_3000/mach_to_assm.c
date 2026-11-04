/**
 * @file mach_to_assm.c
 * @brief File containing the logic to convert a machine instruction to assembly.
 * instruction
 * @author Cameron Hannay
 */

// Edited and Commented by Ben Burris, 11/03/26

#include "include.h"

/**
 * @brief Finds the index in instructon_definitions of the instruction corresponding to the
 * 			funct_code if it is R_TYPE or the op_code otherwise.
 * @param op_code The provided op_code as an integer.
 * @param funct_code The provided funct_code as an integer (nonsense if op_code isn't for
 * 						R_TYPE instruction).
 * @param error Pointer to string to hold error message if an error occurs.
 * @return The index of the instruction in instruction_definitions if error is not set.
 * 			Undefined if error is set.
 */
static size_t find_instruction_by_code(uint32_t op_code, uint32_t funct_code, char **error);


struct assm_parse_result convert_to_assembly(uint32_t inst, char **error)
{
	*error = NULL;

	// Extracting all possible argument types from binary instruction
	//	Bit shifting moves the last bit of the desired section to the LSB, then the
	//		logical AND operator is used to select only the desired bits, setting 
	//		everything to the left to 0 and leaving only the argument's value
	uint32_t rd = (inst >> 11) & MAX_5_BIT;
	uint32_t rs = (inst >> 21) & MAX_5_BIT;
	uint32_t rt = (inst >> 16) & MAX_5_BIT;
	uint32_t sa = (inst >> 6) & MAX_5_BIT;
	uint32_t imm = inst & MAX_16_BIT;
	uint32_t tar = inst & MAX_26_BIT;

	// Same extraction method used for codes
	uint32_t op_code = (inst >> 26) & MAX_6_BIT;
	uint32_t funct_code = inst & MAX_6_BIT;

	struct assm_parse_result assembly;		// struct to hold resulting assembly inst parts

	// Get instruction type from opcode matchfinding function
	size_t ins = find_instruction_by_code(op_code, funct_code, error);
	if (*error != NULL)
		return assembly;

	// Store instruction name in result struct
	assembly.op_name = instruction_definitions[ins].op_name;

	// Store argument types and values in result struct based on the argument data that
	//		is associated with this instruction's type
	// If an instruction has less than 4 arguments, the rest are given placeholder values
	for (size_t i = 0; i < 4; i++) {
		switch (instruction_definitions[ins].parts[i]) {
		case RD:
			assembly.types[i] = REGISTER;
			assembly.vals[i] = rd;
			break;
		case RS:
			assembly.types[i] = REGISTER;
			assembly.vals[i] = rs;
			break;
		case RT:
			assembly.types[i] = REGISTER;
			assembly.vals[i] = rt;
			break;
		case SA:
			assembly.types[i] = REGISTER;
			assembly.vals[i] = sa;
			break;
		case IMM:
			assembly.types[i] = IMMEDIATE;
			assembly.vals[i] = imm;
			break;
		case TAR:
			assembly.types[i] = TARGET;
			assembly.vals[i] = tar;
			break;
		case EMPTY:
			assembly.types[i] = NONE;
			assembly.vals[i] = UNDEFINED;
			break;
		}
	}

	return assembly;
}

static size_t find_instruction_by_code(uint32_t op_code, uint32_t funct_code, char **error)
{
	size_t ins;
	bool found = false;

	// Iterate through instruction opcodes to find match
	for (ins = 0; ins < sizeof(instruction_definitions)
			/ sizeof(struct instruction_definition); ins++) {
		// If no match, continue 
		if (bin_to_num(instruction_definitions[ins].op_code) != op_code)
			continue;
		// If the instruction is R type but func_code doesn't match, continue
		if (instruction_definitions[ins].type == R_TYPE) {
			if (bin_to_num(instruction_definitions[ins].funct_code) != funct_code)
				continue;
		}

		// Fall through means match was found at current ins value
		found = true;
		break;
	}

	// Exiting loop with no match means an invalid opcode
	if (found == false)
		*error = "No instruction of given op code or funct code.";

	return ins;
}
