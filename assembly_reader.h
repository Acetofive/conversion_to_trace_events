#ifndef __ASSEMBLY_READER_H_INCLUDED__
#define __ASSEMBLY_READER_H_INCLUDED__

// included dependacies
// ================================================
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

// The actual header
// ================================================

// Structs for organization of address, instruction, destination and source
struct insn_address{
	char ch[64];
	uint64_t addr_int;
	uint8_t size;
};

struct insn_instruction{
	char ch[64];
	uint8_t size;
};

struct insn_destination{
	char ch[64];
	uint8_t size[3];
	uint8_t parts;
	bool is_divided, is_address;
	uint64_t address;
	char type[8];
	char rel_addr[16];
	bool nosource;
};

struct insn_source{
	char ch[64];
	uint8_t size[3];
	uint8_t parts;
	bool is_divided, is_constant;
	uint64_t value;
	char type[8];
	char rel_addr[16];
};

// Struct which joins all other structs together, to be used in the queue for example
struct insn_all{
	char line[256];
	struct insn_address all_addr;
	struct insn_instruction all_insn;
	struct insn_destination all_dest;
	struct insn_source all_src;
};

// Functions
// ==================================================================================================================================

// Fills passed array with specific value. Careful with index
void fill_array_null(char array[], const int begin=0, const int end=64, char ch=0);

// Initialize/ Reset struct insn_address
int init_reset_addr(struct insn_address *addr);

// Initialize/ Reset struct insn_instruction
int init_reset_instr(struct insn_instruction *instr);

// Initialize/ Reset struct insn_destination
int init_reset_dest(struct insn_destination *dest);

// Initialize/ Reset struct insn_source
int init_reset_src(struct insn_source *src);

// Initialize structs, where info about executed code line will be stored
int codeline_init2(struct insn_all *all);

// Reset structs, where info about executed code line will be stored -> same as codeline_init for now
int codeline_reset2(struct insn_all *all);

// Extract type from insn_destination->ch and store it in insn_destination->type
int extract_type(struct insn_destination *dest);

// Extract type from insn_source->ch and store it in insn_source->type
int extract_type(struct insn_source *src);

// Extract relative address from insn_destination->ch and stores it in insn_destination->rel_addr
int extract_reladdr(struct insn_destination *dest);

// Extract relative address from insn_source->ch and store it in insn_source->rel_addr
int extract_reladdr(struct insn_source *src);

// Complete destination struct, by using already defined values
int complete_insn_destination(struct insn_destination *dest);

// Complete source struct, by using already defined values
int complete_insn_source(struct insn_source *src);

// Updates the size of a word inside the destination or source
int update_size(uint8_t size[3], const uint8_t parts, const int index);

int get_address(const char line[256], struct insn_address *address, int *index);

int get_instruction(const char line[256], struct insn_instruction *instr, int *index);

int get_destination(const char line[256], struct insn_destination *dest, int *index);

int get_source(const char line[256], struct insn_source *src, int *index, const bool nosource);

int parse_instr2(struct insn_all *all);



// ==================================================================================================================================

#endif // __ASSEMBLY_READER_H_INCLUDED__
