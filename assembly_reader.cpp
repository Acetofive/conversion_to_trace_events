#include "assembly_reader.h"

using namespace std;


// Fills passed array with specific value. Careful with index
void fill_array_null(char array[], const int begin, const int end, char ch){
	for(int i = begin;i<end;i++){
		array[i] = ch;
	}
}

// Initialize/ Reset struct insn_address
int init_reset_addr(struct insn_address *addr){
	memset(addr, 0, sizeof(*addr));
	fill_array_null(addr->ch);
	addr->addr_int = 0;
	addr->size = 0;		

	return 0;
}

// Initialize/ Reset struct insn_instruction
int init_reset_instr(struct insn_instruction *instr){
	memset(instr, 0, sizeof(*instr));
	fill_array_null(instr->ch);
	instr->size = 0;
		
	return 0;
}

// Initialize/ Reset struct insn_destination
int init_reset_dest(struct insn_destination *dest){
	memset(dest, 0, sizeof(*dest));
	fill_array_null(dest->ch);
	dest->size[0] = 0;
	dest->size[1] = 0;
	dest->size[2] = 0;
	dest->parts = 1;
	dest->is_divided = false; 		// true when relative address is used
	dest->is_address = false;
	dest->address = 0;
	fill_array_null(dest->type, 0, 8);
	fill_array_null(dest->rel_addr, 0, 16);
	dest->nosource = false;

	return 0;
}

// Initialize/ Reset struct insn_source
int init_reset_src(struct insn_source *src){
	memset(src, 0, sizeof(*src));
	fill_array_null(src->ch);
	src->size[0] = 0;
	src->size[1] = 0;
	src->size[2] = 0;
	src->parts = 0;
	src->is_divided = false;		// true when relative address is used
	src->is_constant = false;
	src->value = 0;
	fill_array_null(src->type, 0, 8);
	fill_array_null(src->rel_addr, 0, 16);

	return 0;
}

// Initialize structs, where info about executed code line will be stored
int codeline_init2(struct insn_all *all){
	init_reset_addr(&(all->all_addr));
	init_reset_instr(&(all->all_insn));
	init_reset_dest(&(all->all_dest));
	init_reset_src(&(all->all_src));

	return 0;
}

// Reset structs, where info about executed code line will be stored -> same as codeline_init for now
int codeline_reset2(struct insn_all *all){
	init_reset_addr(&(all->all_addr));
	init_reset_instr(&(all->all_insn));
	init_reset_dest(&(all->all_dest));
	init_reset_src(&(all->all_src));

	return 0;
}

// Extract type from insn_destination->ch and store it in insn_destination->type
int extract_type(struct insn_destination *dest){
	int i = 0;
	while(dest->ch[i] != 32){
		dest->type[i] = dest->ch[i];
		i++;
	}

	fill_array_null(dest->type, i, 8);

	return 0;
}

// Extract type from insn_source->ch and store it in insn_source->type
int extract_type(struct insn_source *src){
	int i = 0;
	while(src->ch[i] != 32){
		src->type[i] = src->ch[i];
		i++;
	}

	fill_array_null(src->type, i, 8);

	return 0;
}

// Extract relative address from insn_destination->ch and stores it in insn_destination->rel_addr
int extract_reladdr(struct insn_destination *dest){
	int i = dest->size[0] + dest->size[1] + 2; // Begin of relative address
	int j = 0;

	if(dest->ch[i] != '['){ printf(" Error while extracting relative address from destination.\n"); return -1;}
	i++;

	while(dest->ch[i] != ']'){
		dest->rel_addr[j] = dest->ch[i];
		i++;
		j++;
	}

	fill_array_null(dest->rel_addr, j, 16);

	return 0;
}

// Extract relative address from insn_source->ch and store it in insn_source->rel_addr
int extract_reladdr(struct insn_source *src){
	int i = src->size[0] + src->size[1] + 2;
	int j = 0;

	if(src->ch[i] == 'f' && src->ch[i+1] == 's' && src->ch[i+2] == ':') {src->rel_addr[0] = 0; return -1;}

	if(src->ch[i] != '[') {printf(" Error while extracting relative address from source.\n"); return -1;}
	i++;
	
	while(src->ch[i] != ']'){
		src->rel_addr[j] = src->ch[i];
		i++;
		j++;
	}

	fill_array_null(src->rel_addr, j, 16);

	return 0;
}

// Complete destination struct, by using already defined values
int complete_insn_destination(struct insn_destination *dest){
	if(dest->parts > 1){
		dest->is_divided = true;
	}	
	else dest->is_divided = false;

	if(dest->ch[0] == '0' && dest->ch[1] == 'x'){
		dest->is_address = true; 
		dest->address = strtoul (dest->ch, NULL, 16);
	}
	else {dest->is_address = false; dest->address = 0;}

	if(dest->is_divided == true && dest->parts == 3){
		// extract type and relative address
		extract_type(dest);
		extract_reladdr(dest);
	}
	else{ fill_array_null(dest->type, 0, 8); fill_array_null(dest->rel_addr, 0, 16); }

	return 0;
}

// Complete source struct, by using already defined values
int complete_insn_source(struct insn_source *src){
	if(src->parts > 1){
		src->is_divided = true;
	}
	else src->is_divided = false;

	if(src->ch[0] == '0' && src->ch[1] == 'x'){
		src->is_constant = true;
		src->value = strtoul (src->ch, NULL, 16);
	}

	if(src->is_divided == true && src->parts == 3){
		// extract type and relative address
		extract_type(src);
		extract_reladdr(src);
	}
	else{ fill_array_null(src->type, 0, 8); fill_array_null(src->rel_addr, 0, 16); }

	return 0;
}

// Updates the size of a word inside the destination or source
int update_size(uint8_t size[3], const uint8_t parts, const int index){
	switch(parts){
	case 1: size[0] = index;
		size[1] = 0;
		size[2] = 0;
		break;

	case 2: size[1] = index-size[0]-1;
		size[2] = 0;
		break;

	case 3: size[2] = index-size[0]-size[1]-2;
		break;

	default: cout << "Error ocurred during extraction of destination/source from assembly line. (too many parts)\n" << endl;
		return -1;
	}
	
	return 0;
}

int get_address(const char line[256], struct insn_address *address, int *index){
	int i = 0;
	while (((line[i] > 47) && (line[i] < 58)) || ((line[i] > 96) && (line[i] < 103))) {
		address->ch[i] = line[i];
		i++;
		if (i > 63) {
			cout << "Error occured, while extracting address from assembly line.(index to big)\n" << endl;
			return -1;
		}
	}

	// update index and size of address
	address->size = i;
	*index = i+2;

	if(line[i] != 32) {
		cout << "Error occured, while extracting address from assembly line.(space didnt follow the addr)\n" << endl;
		return -1;
	}

	// Fill rest of address array with NUL value (= 0 in decimal)
	fill_array_null(address->ch,i);

	// End by converting relevant char array to uint64_t in hexadecimal base and store it in address->addr_int
	address->addr_int = strtoul (address->ch, NULL, 16);
	// Call this function to print in hexadecimal: printf("Address: %" PRIx64 " ", address->addr_int);
	// Call this function to print in decimal: printf(" %lu \n", address->addr_int);

	return 0;
}

int get_instruction(const char line[256], struct insn_instruction *instr, int *index){
	int i = *index;
	int j = 0;
	while (((line[i] > 47) && (line[i] < 58)) || ((line[i] > 96) && (line[i] < 123))) {
		instr->ch[j] = line[i];
		i++;
		j++;
		if (j > 63) {
			cout << "Error occured, while extracting instr from assembly line.(index to big)\n" << endl;
			return -1;
		}
	}

	// update index and size of instruction array
	instr->size = j;
	*index = i+1;
	if(line[i] != 32) {
		cout << "Error occured, while extracting instr from assembly line.(space didnt follow the addr)\n" << endl;
		return -1;
	}
	
	// Fill rest of instruction array with NUL value (= 0 in decimal)
	fill_array_null(instr->ch,j);

	return 0;
}

int get_destination(const char line[256], struct insn_destination *dest, int *index){
	int i = *index, j = 0;

	// Check if there is a destination
	if(!(((line[i] > 47) && (line[i] < 58)) || ((line[i] > 96) && (line[i] < 123)))){
		// No destination, update values accordingly
		dest->parts = 0;
		dest->size[0] = 0;
		dest->size[1] = 0;
		dest->size[2] = 0;

		fill_array_null(dest->ch);

		dest->nosource = true;
		
		return 0;
	}

	// Write destination into array
	while (((line[i] > 47) && (line[i] < 58)) || ((line[i] > 96) && (line[i] < 123)) || (line[i] == '[') || (line[i] == ']') || (line[i] == '-') || (line[i] == 32) || (line[i] == '*') || (line[i] == '+')) {
		// in case of space, check if destination is over
		if(line[i] == 32){
			// Either there´s another word in the destination or there is no source
			if(!(((line[i+1] > 47) && (line[i+1] < 58)) || ((line[i+1] > 96) && (line[i+1] < 123)) || (line[i+1] == '['))) {
				// No source
				dest->nosource = true;
				break;
			}
			
			// There is another word, update size and parts
			update_size(dest->size, dest->parts, j);
	
			(dest->parts)++;
		}

		dest->ch[j] = line[i];
		i++;
		j++;
	}
	if(line[i] == 0){ dest->nosource = true;}

	if(!((line[i] != 32) || (line[i] != ','))){
			cout << "Error ocurred during extraction of destination from assembly line. (wrong end symbol)\n" << endl;
	} 
	
	update_size(dest->size, dest->parts, j);
	*index = i+2;

	// Fill rest of destination array with nul
	fill_array_null(dest->ch,j);

	// Fill in rest of the values in the struct by calling function below
	complete_insn_destination(dest);

	return 0;
}

int get_source(const char line[256], struct insn_source *src, int *index, const bool nosource){
	int i = *index, j = 0;
	src->parts = 1;

	if(nosource){
		// No source, update values accordingly.
		init_reset_src(src);
		
		return 0;
	}

	// Write source into array
	while(((line[i] > 47) && (line[i] < 58)) || ((line[i] > 96) && (line[i] < 123)) || (line[i] == '[') || (line[i] == ']') || (line[i] == '-') || (line[i] == 32) || (line[i] == '*') || (line[i] == '+') || (line[i] == ':')) {
		
		// in case of space check if source is over
		if(line[i] == 32){
			// Either there is another word or source is over
			if(!(((line[i+1] > 47) && (line[i+1] < 58)) || ((line[i+1] > 96) && (line[i+1] < 123)) || (line[i+1] == '['))){
				// Source is over
				break;
			}
			
			// There is another word, update size and parts
			update_size(src->size, src->parts,j);
			(src->parts)++;
		}

		src->ch[j] = line[i];
		i++;
		j++;
	}

	update_size(src->size, src->parts, j);
	*index = i;

	// Fill rest of source array with null values
	fill_array_null(src->ch,j);

	// Fill in rest of the values in the struct by calling function below
	complete_insn_source(src);
	
	return 0;
}

int parse_instr2(struct insn_all *all){
	int index = 0;

	// Reset arguments, before calling functions that changes them
	codeline_reset2(all);

	get_address(all->line, &(all->all_addr), &index);

	get_instruction(all->line, &(all->all_insn), &index);

	get_destination(all->line, &(all->all_dest), &index);

	//printf("\n parts: %d, size %d  %d  %d \n", (int)destination->parts,(int)destination->size[0],(int)destination->size[1],(int)destination->size[2]);

	get_source(all->line, &(all->all_src), &index, all->all_dest.nosource);

	//printf("\n parts: %d, size %d  %d  %d \n", (int)source->parts,(int)source->size[0],(int)source->size[1],(int)source->size[2]);

	return 0;
}
