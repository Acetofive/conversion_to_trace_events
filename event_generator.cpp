#include "event_generator.h"

using namespace std;

// Generates Event, based on single address filtering
int process_single_address2(struct tool_options *options, struct values_to_trace *val, struct insn_all *all, uint64_t *curr_instr){
	uint64_t addr = val->addr_beg;

	if(addr == all->all_addr.addr_int){
		val->exec_times++;
		if(options->dont_print == 0){
			if(options->print_whole_instr == 1) printf(" Instr. nr %lu: %s\n", *curr_instr, all->line);
			if(options->print_exec_times == 1) printf(" Executed times: %d \n", val->exec_times);
		}

	}

	return 0;
}

// Generates Event, based on range of addresses
int process_range_address2(struct tool_options *options, struct values_to_trace *val, struct insn_all *all, uint64_t *curr_instr){
	uint64_t addr1 = val->addr_beg;
	uint64_t addr2 = val->addr_end;

	if((addr1 <= all->all_addr.addr_int) && (all->all_addr.addr_int <= addr2)){
		if(val->inside_range == 0){
			val->inside_range = 1;
			val->exec_times++;
			if(options->dont_print == 0 && options->print_exec_times == 1) printf(" Range entered for %d time.\n", val->exec_times);
		}
		if(options->dont_print == 0){
			if(options->print_whole_instr == 1) printf(" Instr. nr %lu: %s\n", *curr_instr, all->line);
		}
	}

	else{
		if(val->inside_range == 1){
			val->inside_range = 0;
			if(options->dont_print == 0 && options->print_exec_times == 1) printf(" Range exited for %d time.\n", val->exec_times);
		}
	}

	return 0;
}

// Generates Event, based on a specific function, for example its calls, returns, etc..
int process_function2(struct tool_options *options, struct values_to_trace *val, struct insn_all *all, uint64_t *curr_instr, deque<struct insn_all> *deque_insn, std::stack<string> *stack_loc){
	
	// Check whether current instr is function call
	if(val->inside_range == 0){
		if(strcmp(all->all_insn.ch, "call") == 0){
			if(all->all_dest.address == val->addr_beg){
				val->inside_range = 1;
				// Function is being called
				val->exec_times++;
				if(options->dont_print == 0){
					// Print nr of times function has been called				
					if(options->print_exec_times == 1) printf(" Function call nr: %d on instr nr %lu.\n", val->exec_times, *curr_instr);
				
					if(options->print_whole_instr == 1) printf(" %s \n", all->line);
				
					// print previous executed instructions if option print_queue is set
					if(options->print_queue == 1){ 
						printf(" Queue at the time of the call: \n");
						print_whole_queue(deque_insn);	
					}	

					// print caller function
					if(options->print_caller == 1){
						printf(" Caller function in hex: %s.\n", val->curr_location);
					}
				}
			}
			else{
				// Call function was executed, but not for function being traced --> update curr_location				
				if(*curr_instr != 1){
					strncpy(val->curr_location, all->all_dest.ch, 64);
					stack_loc->push(all->all_dest.ch);
				}
			}
		}
		if(strcmp(all->all_insn.ch, "ret") == 0 && strcmp(val->curr_location, "main") != 0){
			// Program is returning from an instruction other than main --> pop element in stack --> update curr_loc
			if(!stack_loc->empty()) stack_loc->pop();
			if(!stack_loc->empty()){
				strcpy(val->curr_location, (stack_loc->top()).c_str());
			}
		}
	}
	else{
		if(options->dont_print == 0 && options->print_whole_instr == 1) printf(" %s \n", all->line);

		// Check if current instr is return for function or call/return to another fct
		if(strcmp(all->all_insn.ch,"call") == 0) val->nested_calls++;

		if(strcmp(all->all_insn.ch, "ret") == 0) {
			if(val->nested_calls == 0){
				if(val->addr_end == 0){
					val->addr_end = all->all_addr.addr_int;
					if(options->get_return == 1) printf(" Function's return address: %lu.\n", val->addr_end);
				}
				val->inside_range = 0;
			}
			val->nested_calls--;
		}
	}

	return 0;
}

// Receives parsed instruction from main and user selected options for generation of events
int evgen_process_insn2(struct tool_options *options, struct values_to_trace *val, struct insn_all *all, uint64_t *curr_instr, deque<struct insn_all> *deque_insn, 
		stack<string> *stack_loc){
	switch(options->filter){
		case tool_options::none : 
			// For now just print whole assembly code
			if(options->dont_print == 0) printf(" Instr. nr %lu: %s\n", *curr_instr, all->line);
			break;
		
		// Address-based generation of events
		case tool_options::by_address :
			if(options->single_address == 1){
				//process_single_address(options, val, line, address, instruction, destination, source, curr_instr);
				process_single_address2(options, val, all, curr_instr);
			}
			else{
				//process_range_address(options, val, line, address, instruction, destination, source, curr_instr);
				process_range_address2(options, val, all, curr_instr);
			}
			break;
	
		// Function-based generation of events
		case tool_options::by_function :
			//process_function(options, val, line, address, instruction, destination, source, curr_instr);
			process_function2(options, val, all, curr_instr, deque_insn, stack_loc);
			break;

		default: printf(" Error: unexisting filter option selected.");
			return -1;
	}
	
	return 0;
} 
