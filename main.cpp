#include "assembly_reader.h"
#include "extract_relevant_code.h"
#include "settings.h"
#include "event_generator.h"
#include "insn_queue.h"

#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <deque>
#include <stack>

using namespace std;

// Opens textfile
int open_textfile(ifstream *file){
	if(!(*file).is_open()){
		// Unable to open assembly code text file. End program
		cout << " Unable to open text file. Ending program.\n";
		return -1;
	}
	cout<< " File successfully opened."<<endl;

	return 0;
}

// Reads from textfile line by line
int read_textfile(ifstream *file, struct tool_options *options, struct values_to_trace *val, const int queue_size){
	struct insn_all whole_insn;
	string line;
	uint64_t curr_instr = 0; // Variable for the number of instruction being processed

	deque<struct insn_all> deque_insn (0); // Dynamic Queue which will be used to store last processed instructions in case they are later needed

	//stack<uint64_t> stack_location ; // Stack to keep track in which function program finds itself
	stack<string> stack_location ; // Stack to keep track in which function program finds itself
	

	// Initialize structs declared above
	codeline_init2(&whole_insn);

	// Read file, line by line
	while (getline(*file,line)){
		curr_instr++;
	
		// Convert line from string to char array
		strcpy(whole_insn.line, line.c_str());

		if(options->dont_print != 1 && 0) {
			printf(" %s\n", whole_insn.line);
		}	

		// Call function, which divides line into address, instruction, destination and source
		parse_instr2(&whole_insn);

		if(options->dont_print != 1 && 0) {
			printf(" %s\n", whole_insn.all_addr.ch);
			printf(" %s\n", whole_insn.all_insn.ch);
			printf(" %s\n", whole_insn.all_dest.ch);
			printf(" %s\n", whole_insn.all_src.ch);
		}
		
		update_deque_insn(&deque_insn, &whole_insn, curr_instr, queue_size);
		

		// Send current instruction to event generator
		evgen_process_insn2(options, val, &whole_insn, &curr_instr, &deque_insn, &stack_location);
	}

	return 0;
}

// Function, which outputs some helpful tips for the user
static void help(const char *name){
	printf("usage: %s [<options>]\n\n"
		"options:\n"
		" --help | -h					this text.\n"
		" --addr <address> 				only generates events for instruction executed in this address.\n"
		" --range-addr <addr_beg>:<addr_end> 		only generates events for instructions executed in this range of addresses.\n"
		" --fct | -f <addr_call>[:<addr_ret>]       	function based event generation.\n\n"	
		" --dont-print				        prints error messages only. overrides all other options\n"
		" --print-whole-instr				print whole lines of assembly code instrunctions of traced program.\n"
		" --dont-print-whole-instr			suppresses default printing of all relevant assembly code lines. \n"
		" --extr-code					extracts relevant code from decoded text file. Saves it to rel_code.txt. First step for this tool.\n"
		" --exec-times					depending on filter options: prints either nr of times traced instr. has been executed, times range has been entered/exited or nr of times fct has been called(recursive calls not supported yet).\n"
		" --get-return-addr				for filter by fct: gets functions return address. \n"
		" --print-queue					for filter by fct: prints executed insns that preceded the fct call. \n"
		" --print-caller				for filter by fct: prints caller function. \n"
		"\n"
		"Adresses must be given in hexadecimal form.\n\n",
		name);
}

int main(int argc, char *argv[]){
	ifstream assembly_file("rel_code.txt");
	int i;
	const char *prog;
	struct tool_options options;
	struct values_to_trace val;

	init_tool_options(&options);	
	init_values_to_trace(&val);
	prog = argv[0];

	// Read input from user
	for(i = 1; i<argc;){
		char *arg;
		arg = argv[i++]; 

		if(strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0){
			help(prog);
			goto out;
		}
		if(strcmp(arg, "--dont-print") == 0){
			set_dont_print(&options);

			continue;
		}
		if(strcmp(arg, "--addr") == 0){
			if(argc <= i){
				printf(" %s: --addr: missing argument.\n", prog);
				goto out;
			}

			arg = argv[i++]; // store adress in arg
			set_and_extract_filter_by_address(1, &options, arg, &val);
					
			continue;
		}
		if(strcmp(arg, "--range-addr") == 0){
			if(argc <= i){
				printf(" %s: --range-addr: missing argument. \n", prog);
				goto out;
			}

			arg = argv[i++]; 
			if(set_and_extract_filter_by_address(0, &options, arg, &val) != 0) goto err;
			
			continue;
		}
		if(strcmp(arg, "--fct") == 0 || strcmp(arg, "-f") == 0){
			if(argc <= i){	
				printf(" %s: --range-addr: missing argument. \n", prog);
				goto out;
			}

			arg = argv[i++];
			set_and_extract_filter_by_function(&options, arg, &val);

			continue;
		}
		if(strcmp(arg, "--extr-code") == 0){
			if(argc <= i){
				extract_process("assembly.txt");
			}
			else{
				arg = argv[i++]; // stores name of file to be read from in arg
				extract_process(arg);
			}
			goto out;
		}
		if(strcmp(arg, "--print-whole-instr") == 0){
			set_print_whole_instr(&options, 1);

			continue;
		}
		if(strcmp(arg, "--dont-print-whole-instr") == 0){
			set_print_whole_instr(&options, 0);

			continue;
		}
		if(strcmp(arg, "--exec-times") == 0){
			if(set_print_exec_times(&options, 1) == -1) goto err;

			continue;
		}
		if(strcmp(arg, "--get-return-addr") == 0){
			set_get_return(1, &options);

			continue;
		}
		if(strcmp(arg, "--print-queue") == 0){
			if(set_print_queue(1, &options) == -1) goto err;

			continue;
		}
		if(strcmp(arg, "--print-caller") == 0){
			if(set_print_caller(1, &options) == -1) goto err;

			continue;
		}
		
		printf(" %s: unknown option: %s.\n Call %s --help for list of options.\n", prog, arg, prog);
		goto err;

	}

	// Open text file, in order to be read from
	if(open_textfile(&assembly_file) < 0){ goto err;}

	// Read from file --> 10 is the size of the queue --> Make it flexible by user input
	read_textfile(&assembly_file, &options, &val, 10);

	// Close file
	assembly_file.close();

out:
	return 0;

err:
	return 1;
}
