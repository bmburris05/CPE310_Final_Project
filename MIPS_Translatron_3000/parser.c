/**
 * @file parser.c
 * @brief Parses user input of assembly and determines what kind of instruction it is to be converted into
 * @author Cameron Hannay
 */

// Commented by Ben Burris, 11/02/26

#include "include.h"

/**
 * @brief Takes a string and turns each character to uppercase
 * @param operand String to convert to uppercase
 */
static void uppercase(char *operand);

/**
 * @brief Takes a string and compares it with each register type to find a match
 * @param str String containing register type to look up
 * @param error Pointer to store potential error string
 * @return Index of the match in the registers array, or UNDEFINED if no match is found
 */
static uint32_t reg_lookup(char* str, char** error);

/**
 * @brief Removes whitespace from a string
 * @param str String to remove whitespace from
 */
static void remove_space(char* str);

/**
 * @brief Removes pound symbol from string
 * @param str String to remove pound symbol from
 */
static void remove_pound(char* str);


uint32_t parse_assembly(char* line, char** error)
{
    struct assm_parse_result result;    // Struct to store components of instruction
    *error = NULL;                      // Set error text to null

    size_t length = strcspn(line, " ");                             // Find spot where instruction name ends
    char* opp_name = (char*)malloc(sizeof(char) * (length + 1));    // Allocate enough space to hold the instruction name

    // Return UNDEFINED if malloc error occurs
    if (opp_name == NULL)
    {
        *error = "Memory not allocated.";
        return UNDEFINED;
    }

    // Copy instruction name string to separate variable, format it, and store in struct
    strncpy(opp_name, line, length);
    opp_name[length] = '\0';
    uppercase(opp_name);
    result.op_name = opp_name;

    // Discard instruction name by adding its size to the line's pointer value
    line += length;

    // Separate instruction's arguments using commas as a delimiter
    char* token = strtok(line, ",");
    size_t arg;                             // Number of arguments in the instruction
    for (arg = 0; token != NULL; arg++)
    {
        remove_space(token);

        // Pound sign indicates an immediate value
        if (token[0] == '#')
        {
            // Record this argument's type in result struct
            result.types[arg] = IMMEDIATE;

            remove_pound(token);
            char* end_ptr;          // Pointer to next char after the last numeric char
            
            // Use union 'trick' and strtol to get unsigned version of the immediate's value, 
            //    then store in result's argument array
	        union signedness_switch_trick signedness;
            signedness.i = (int16_t) strtol(token, &end_ptr, 10);
            result.vals[arg] = signedness.u;

            // If ptr isn't at the end (\0), there are invalid character(s) in the immediate
            if(end_ptr == token || *end_ptr != '\0')
            {
                *error = "Improperly formatted immediate.";
                free(opp_name);
                opp_name = NULL;
                return UNDEFINED;
            }
        }
        // Dollar sign indicates a register
        else if (token[0] == '$')
        {
            // Record this argument's type in result struct
            result.types[arg] = REGISTER;

            // Call lookup function to find specific register and record in struct
            result.vals[arg] = reg_lookup(token, error);

            // If an error is recorded in reg_lookup, the reg type is invalid
            if (*error != NULL) {
                free(opp_name);
                opp_name = NULL;
                return UNDEFINED;
            }
        }
        // Arguments starting with any other characters are invalid
        else
        {
            *error = "Argument isn't register or immediate (targets not yet supported).";
            free(opp_name);
            opp_name = NULL;
            return UNDEFINED; 
        }
        
        // Advance to next argument string
        token = strtok(NULL, ",");
    }

    // If the instruction had less than 4 arguments, fill the remaining ones' info with placeholders
    for (;arg < 4; arg++) {
        result.types[arg] = NONE;
        result.vals[arg] = UNDEFINED;
    }

    // Use the created result struct to convert instruction to machine code
    uint32_t new_val = convert_to_machine_code(result, error);
    
    // Final garbage collection
    free(opp_name);
    opp_name = NULL;

    return new_val;
}

static void uppercase(char* operand)
{
    size_t length = strlen(operand);

    // Increment through operand, replacing every char with its uppercase version
    for (size_t i = 0; i < length; i++)
    {
        operand[i] = toupper((unsigned char) operand[i]);
    }
}

static void remove_pound(char* str)
{
    int count = 0; // Number of non-pound characters encountered

    // Increment through each character in str
    for (int i = 0; i < strlen(str); i++)
    {
        // If str[i] isn't '#', copy it to str[count] and increment count
        // If str[i] is '#', its index is skipped, so the next non-pound character will
        //    be placed at that location, writing over the '#'
        if (str[i] != '#')
        {
            str[count++] = str[i];
        }
    }
    // Null-terminate the string at the last index
    str[count] = '\0';
}

static void remove_space(char* str)
{
    int count = 0; // Number of non-space chars encountered

    // Increment through each character in str
    for (int i = 0; i < strlen(str); i++)
    {
        // If str[i] isn't a space, copy it to str[count] and increment count
        // If str[i] is a space, its index is skipped, so the next non-space char will
        //    be placed at that location, writing over the space  
        if (str[i] != ' ')
        {
            str[count++] = str[i];
        }
    }
    // Null-terminate the string at the last index
    str[count] = '\0';
}

static uint32_t reg_lookup(char* str, char** error)
{
    // Increment through each element in the array of register types
    for (uint32_t i = 0; i < sizeof(registers) / sizeof(char*); i++)
    {
        // If a type match is found, return the index
        if (strcmp(registers[i], str) == 0)
        {
            return i;
        }
    }

    // Getting through array with no match means str is an invalid reg type
    *error = "Register not found.";
    return UNDEFINED;
}
