#include "insn_queue.h"

using namespace std;

// INITIALIZING FUNCTIONS =================================================================================================================
// Goes through entire uninitialized queue
int deque_insn_reset(std::deque<struct insn_all> *deque_insn, const int size){
	struct insn_all whole_insn;
	codeline_init2(&whole_insn);	

	// iterate over entire queue
	for(deque<struct insn_all>::iterator it = deque_insn->begin(); it != deque_insn->end(); ++it){
	 	*it = whole_insn;
	}

	return 0;
}

// FUNCTIONS TO PRINT VALUES STORED IN QUEUE ==============================================================================================

// Print all instruction lines stored in queue
int print_whole_queue(deque<struct insn_all> *deque_insn){
	int i = 1;
	for(deque<struct insn_all>::iterator it = deque_insn->begin(); it != deque_insn->end(); ++it){
	 	printf(" Nr %d in queue: %s \n", i, it->line);
		i++;
	}

	return 0;
}

// FUNCTIONS TO UPDATE QUEUE ==============================================================================================================

// Update queue by adding new element and deleting old one if necessary
int update_deque_insn(deque<struct insn_all> *deque_insn, struct insn_all *whole_insn, const int curr_instr, const int queue_size){
	if(curr_instr > queue_size){
		// Queue has reached its maximum size -> delete oldest element before adding new one
		deque_insn->pop_back();
	}

	// Add new element to queue
	deque_insn->push_front(*whole_insn);
	

	return 0;
}
