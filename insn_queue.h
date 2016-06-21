#ifndef __INSN_QUEUE_H_INCLUDED__
#define __INSN_QUEUE_H_INCLUDED__

// included dependacies
// =======================================================================
#include <deque>

#include "assembly_reader.h"

// Header
// =======================================================================

// Functions
// =======================================================================
// INITIALIZING FUNCTIONS =================================================================================================================
int deque_insn_reset(std::deque<struct insn_all> *deque_insn, const int size);


// FUNCTIONS TO PRINT VALUES STORED IN QUEUE ==============================================================================================

// Print all instruction lines stored in queue
int print_whole_queue(std::deque<struct insn_all> *deque_insn);

// FUNCTIONS TO UPDATE QUEUE ==============================================================================================================

// Update queue by adding new element and deleting old one if necessary
int update_deque_insn(std::deque<struct insn_all> *deque_insn, struct insn_all *whole_insn, const int curr_instr, const int queue_size);

// =======================================================================


#endif // __INSN_QUEUE_H_INCLUDED
