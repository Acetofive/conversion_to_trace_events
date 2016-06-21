#include "settings.h"

using namespace std;

// INITIALIZING FUNCTIONS =================================================================================================================

// Initializes options with default values. Can later be changed through users input
int init_tool_options(struct tool_options *options){
	memset(options, 0, sizeof(*options));
	options->filter = tool_options::none;
	options->filter_by_address = 0;
	options->single_address = 0;
	options->range_address = 0;
	options->filter_by_function = 0;

	options->print_whole_instr = 1;
	options->dont_print = 0;	// Overrides print_whole_instruction
	options->print_exec_times = 0;
	options->get_return = 0;
	options->print_queue = 0;
	options->print_caller = 0;

	return 0;
}

// Initializes/Resets curr_location back to 'main'
int reset_curr_location(char curr_loc[]){
	curr_loc[0] = 'm';
	curr_loc[1] = 'a';
	curr_loc[2] = 'i';
	curr_loc[3] = 'n';

	fill_array_null(curr_loc, 4);

	return 0;
}

// Initializes values to trace with 0
int init_values_to_trace(struct values_to_trace *val){
	memset(val, 0, sizeof(*val));
	val->addr_beg = 0;
	val->addr_end = 0;
	val->exec_times = 0;
	val->inside_range = 0;
	val->nested_calls = 0;

	reset_curr_location(val->curr_location);

	return 0;
}

// SET PRINT OPTIONS FUNCTIONS =========================================================================================================

// Set options to print_exec_times -> So far only supported for filtering by address
int set_print_exec_times(struct tool_options *options, const uint32_t set){
	if (set == 1){
		if(options->filter == tool_options::by_address || options->filter == tool_options::by_function){
			options->print_exec_times = 1;
		}
		else{
			printf(" --exec-times option not supported for chosen settings. Try setting filter by address or function first. \n");
			return -1;
		}
	}
	else{
		options->print_exec_times = 0;
	}

	return 0;
}

// Set options to print_whole_instr
int set_print_whole_instr(struct tool_options *options, const uint32_t set){
	options->print_whole_instr = set;

	return 0;
}

// Set options to dont_print
int set_dont_print(struct tool_options *options){
	options->dont_print = 1;
	options->print_whole_instr = 0;

	return 0;
}

// Sets print queue
int set_print_queue(const uint8_t set, struct tool_options *options){
	if(set == 0){
		options->print_queue = 0;
		return 0;
	}
	if(options->filter == tool_options::by_function){
		options->print_queue = 1;
	}
	else{
		printf(" Please select the filter by function before using the --print-queue option. \n");
		return -1;
	}
	
	return 0;
}

// Sets print caller
int set_print_caller(const uint8_t set, struct tool_options *options){
	if(set == 0){
		options->print_caller = 0;
		return 0;
	}
	if(options->filter == tool_options::by_function){
		options->print_caller = 1;
	}
	else{
		printf(" Please select the filter by function before using the --print-caller option. \n");
		return -1;
	}

	return 0;
}

// FUNCTIONS TO PROCESS ADRESS-BASED GENERATIONS OF EVENTS =======================================================================================

// Extract address from *char and convert it into uint64_t
int extract_single_addr(const char *ch, struct values_to_trace *val){
	val->addr_beg = strtoul(ch,0,16);

	return 0;
}

// Extract range of addresses
int extract_range_addr(const char *ch, struct values_to_trace *val, const char *ptr){
	int i = 0, j = 0, index;		
	char temp[64];

	index = (int)(ptr-ch);	

	// First address
	for(i=0; i<index; i++){
		temp[i] = *(ch+i);
	}
	fill_array_null(temp, index, 64);
	val->addr_beg = strtoul(temp, 0, 16);
			
	// Second address
	i++;
	while(*(ch+i) != 0){
		temp[j] = *(ch+i);
		i++;
		j++;
	}
	fill_array_null(temp, j, 64);
	val->addr_end = strtoul(temp, 0, 16);

	return 0;
}

// Sets options for filtering by address (If single = 0, it assumes it´s going to handle a range of addresses) and extracts address if necessary
int set_filter_by_address(int set, int single, struct tool_options *options){
	if(set == 1){
		// Set filter by address
		options->filter_by_address = 1;
		options->filter = tool_options::by_address;

		// Either by single adress or range of adresses
		if(single == 1){
			options->single_address = 1;
			options->range_address = 0;
		}
		else{
			options->single_address = 0;
			options->range_address = 1;
		}
	}
	else{
		// Reset options for filter by address (disactivates it)
		options->filter_by_address = 0;
	}

	return 0;
}

// Sets filtering options and extracts addresses to values_to_trace
int set_and_extract_filter_by_address(int single, struct tool_options *options, const char *ch, struct values_to_trace *val){
	set_filter_by_address(1, single, options);

	if(single == 1){
		extract_single_addr(ch, val);
	}
	else{
		const char *ptr;
		ptr = strchr(ch, ':');
		if(ptr == NULL) { return -1;}		
		extract_range_addr(ch, val, ptr);
	}

	return 0;
}

// FUNCTIONS TO PROCESS FUNCTION-BASED GENERATION OF EVENTS ==================================================================================
// Sets filtering options to enable function-based event generation
int set_filter_by_function(const uint8_t set, struct tool_options *options){
	if(set == 1){
		options->filter_by_function = 1;
		options->filter_by_address = 0;
		options->filter = tool_options::by_function;
	}
 	else{
		options->filter_by_function = 0;
	}

	return 0;
}

// Sets filtering options and extracts user input to val
int set_and_extract_filter_by_function(struct tool_options *options, const char *ch, struct values_to_trace *val){
	set_filter_by_function(1, options);

	const char *ptr;
	ptr = strchr(ch, ':');

	if(ptr == NULL){
		extract_single_addr(ch, val);
	}
	else{
		extract_range_addr(ch, val, ptr);
	}

	return 0;
}

// Sets get return address of traced function
int set_get_return(const uint8_t set, struct tool_options *options){
	if(set == 0){
		options->get_return = 0;
		return 0;
	}
	if(options->filter == tool_options::by_function){
		options->get_return = 1;
	}
	else{
		printf(" Please select the filter by function before using the --get-return option. \n");
		return -1;
	}

	return 0;
}

