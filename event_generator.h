#ifndef __EVENT_GENERATOR_H_INCLUDED__
#define __EVENT_GENERATOR_H_INCLUDED__

// included dependacies
// =======================================================================
#include <stack>
#include <string>

#include "settings.h"
#include "assembly_reader.h"
#include "insn_queue.h"

// Header
// =======================================================================

// Functions
// =======================================================================

// Generates Event, based on single address filtering
int process_single_address2(struct tool_options *options, struct values_to_trace *val, struct insn_all *all, uint64_t *curr_instr);

// Generates Event, based on range of addresses
int process_range_address2(struct tool_options *options, struct values_to_trace *val, struct insn_all *all, uint64_t *curr_instr);

// Generates Event, based on a specific function, for example its calls, returns, etc..
int process_function2(struct tool_options *options, struct values_to_trace *val, struct insn_all *all, uint64_t *curr_instr, std::deque<struct insn_all> *deque_insn, std::stack<std::string> *stack_loc);

// Receives parsed instruction from main and user selected options for generation of events
int evgen_process_insn2(struct tool_options *options, struct values_to_trace *val, struct insn_all *all, uint64_t *curr_instr, std::deque<struct insn_all> *deque_insn, std::stack<std::string> *stack_loc);

// =======================================================================
 
#endif // __ENVENT_GENERATOR_H_INCLUDED__
