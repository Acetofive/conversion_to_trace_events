#ifndef __EXTRACT_RELEVANT_CODE_H_INCLUDED__
#define __EXTRACT_RELEVANT_CODE_H_INCLUDED__

// included dependacies
// =======================================================================
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <stdint.h>

// Header
// =======================================================================

// Functions
// =======================================================================

// Opens files: rd has name stored in str. ofstream has predefined name
int open_files(char const read_name[], std::ifstream *rd, std::ofstream *wr);

// Extracts relevant assembly code, using the previously opened files
int extract_relevant_code(std::ifstream *rd, std::ofstream *wr);

// Closes files
void close_files(std::ifstream *rd, std::ofstream *wr);

// Initiates whole process
int extract_process(char const read_name[]);

// =======================================================================

#endif // __EXTRACT_RELEVANT_CODE_H_INCLUDED__
