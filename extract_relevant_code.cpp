#include "extract_relevant_code.h"

using namespace std;

// Functions´ Implementation ======================================================================================

// Opens files: rd has name stored in str. ofstream has predefined name
int open_files(char const read_name[], ifstream *rd, ofstream *wr){

	(*rd).open(read_name);
	(*wr).open("rel_code.txt");

	if(!(*wr).is_open()){
		cout << " Unable to open file to write to." << endl;	
		return -1;
	}

	if(!(*rd).is_open()){
		cout << " Unable to open file to read from." << endl;
		return -1;
	}

	return 0;
}

// Extracts relevant assembly code, using the previously opened files
int extract_relevant_code(ifstream *rd, ofstream *wr){
	string line, instr;
	uint32_t is_rel = 0;
	uint8_t call_stack = 0;

	// Read single line from file
	while(getline(*rd,line)){
		// Check if its an instr or an [enabled/disabled/resumed]
		if(line.at(0) == '['){
			if(is_rel == 0) { continue;}
			else{
				cout << " Tracing stopped during execution of program. Please rerun the trace again." << endl;
				return -1;
			}
		}

		// Remove first 18 elements of read string
		instr = line.substr(18, line.size()-18);
	
		if(is_rel == 0){
			// compare it to first string
			if(instr.compare(0, 29, "mov rax, qword ptr [rsp+0x18]") == 0){

				// First line is equal--> check next one
				getline(*rd,line);
				instr = line.substr(18, line.size()-18);

				//if(strcmp("call rax", instr) == 0) {
				if(instr.compare(0, 8, "call rax") == 0){
					// Entered main
					(*wr) << line << "\n";
					is_rel = 1;
				}
			}
		}
		else{
			// Check for calls
			if(instr.compare(0, 5, "call ") == 0) { call_stack++;}

			// Check for returns
			if(instr.compare(0, 4, "ret ") == 0) {
				if(call_stack > 0) {
					call_stack--;
				}
				else{
					// Final return: print line and break
					(*wr) << line << "\n";
					break;
				}
			}

			(*wr) << line << "\n";
		}
	}	
	
	return 0;
}

// Closes files
void close_files(ifstream *rd, ofstream *wr){
	(*rd).close();
	(*wr).close();
}

// Initiates whole process
int extract_process(char const read_name[]){

	ifstream read_file;
	ofstream write_file;
	
	// Open Files
	if(open_files(read_name, &read_file, &write_file) != 0) return -1;

	// Extract Relevant Code to new file
	if(extract_relevant_code(&read_file, &write_file) != 0) return -1;

	// Close files
	close_files(&read_file, &write_file);

	return 0;
}


// ================================================================================================================
