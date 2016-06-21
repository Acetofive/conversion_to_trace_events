#ifndef __SETTINGS_H_INCLUDED__
#define __SETTINGS_H_INCLUDED__

// included dependacies
// =======================================================================
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#include "assembly_reader.h"

// Header
// =======================================================================
// Tool options
struct tool_options{
	// FILTERING OPTIONS ============================================================================
	
	// Enum to store filtering options
	enum filter_type {none,	by_address, by_function} filter;

	// Filtering of instructions by their adress is on
	uint32_t filter_by_address;

	// Only generates events for this instruction address
	uint32_t single_address;

	// Only generates events for instructions inside(including) range defined by user
	uint32_t range_address;

	// Generate events based on function
	uint32_t filter_by_function;

	// PRINT OPTIONS ================================================================================

	// Print all assembly code instrunctions of traced program
	uint32_t print_whole_instr;

	// Don´t print anything regarding tracing that is not error messages. Overrides all other options
	uint32_t dont_print;

	// Print number of times instr to trace is being executed
	uint32_t print_exec_times;

	// Gets traced function´s return address. Function must be called in order for this to work
	uint32_t get_return;

	// Prints executed insns that preceded the fct call
	uint32_t print_queue;

	// Prints caller function´s address
	uint32_t print_caller;
	
};

// Value to trace, for example addresses -> Defined by user
struct values_to_trace{
	// Addresses (either one specific address or range of addresses) -> check options to determine which	
	uint64_t addr_beg;
	uint64_t addr_end;
	uint32_t inside_range; // also used for detecting when inside function being traced
	
	// Tracks number of times single address has been executed. Can also be used to count nr of times function has been called
	uint32_t exec_times;

	// Nr of nested function calls inside function being traced. Used to get return address of original function
	uint32_t nested_calls;

	// Tracks current location of programe, namely in which function the program counter finds itself
	char curr_location[64];
};

// Functions
// =======================================================================

// INITIALIZING FUNCTIONS =================================================================================================================
// Initializes options with default values. Can later be changed through users input
int init_tool_options(struct tool_options *options);

// Initializes/Resets curr_location back to 'main'
int reset_curr_location(char curr_loc[]);

// Initializes values to trace with 0
int init_values_to_trace(struct values_to_trace *val);

// SET PRINT OPTIONS FUNCTIONS ============================================================================================================
// Set options to print_exec_times -> So far only supported for filtering by address
int set_print_exec_times(struct tool_options *options, const uint32_t set);

// Set options to print_whole_instr
int set_print_whole_instr(struct tool_options *options, const uint32_t set);

// Set options to dont_print
int set_dont_print(struct tool_options *options);

// Sets print queue
int set_print_queue(const uint8_t set, struct tool_options *options);

// Sets print caller
int set_print_caller(const uint8_t set, struct tool_options *options);

// FUNCTIONS TO PROCESS ADRESS-BASED GENERATIONS OF EVENTS ================================================================================
// Extract address from *char and convert it into uint64_t
int extract_single_addr(const char *ch, struct values_to_trace *val);

// Extract range of addresses
int extract_range_addr(const char *ch, struct values_to_trace *val, const char *ptr=NULL);

// Sets options for filtering by address (If single = 0, it assumes it´s going to handle a range of addresses) and extracts address if necessary
int set_filter_by_address(int set, int single=0, struct tool_options *options=NULL);

// Sets filtering options and extracts addresses to values_to_trace
int set_and_extract_filter_by_address(int single, struct tool_options *options, const char *ch, struct values_to_trace *val);

// FUNCTIONS TO PROCESS FUNCTION-BASED GENERATION OF EVENTS ================================================================================
// Sets filtering options to enable function-based event generation
int set_filter_by_function(const uint8_t set, struct tool_options *options=NULL);

// Sets filtering options and extracts user input to val
int set_and_extract_filter_by_function(struct tool_options *options, const char *ch, struct values_to_trace *val); 

// Sets get return address of traced function
int set_get_return(const uint8_t set, struct tool_options *options);

// =======================================================================


#endif // __SETTINGS_H_INCLUDED
