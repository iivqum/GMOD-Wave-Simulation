#pragma once

#include <filesystem>

#include "bspfile.h"

using namespace std;

/*
	Simple BSP parser
*/

class bsp_parser;

template <typename lump_type>
class bsp_lump {
public:
	bsp_lump(bsp_parser* parser, size_t lump_id) : data((lump_type*)parser->get_lump_data(lump_id)), 
		length(parser->get_num_entries(lump_id, sizeof(lump_type))) {}
	size_t get_length() {return this->length;}
private:
	lump_type* data = nullptr;
	// How many data structures
	size_t length = 0;
};

class bsp_parser {
public:
	// Generic data block containing lump data
	struct lump_data {
		// Length of data block
		size_t length = 0;
		// Pointer to start of data block
		void* data = nullptr;
	};
	lump_data lumps[HEADER_LUMPS];
	bsp_parser(const char* loc);
	~bsp_parser(void);
	// Load a BSP from file
	bool load(const char* loc);
	// Cleanup resources
	void cleanup();
	// Get number of data structures inside of a lump
	size_t get_num_entries(size_t lump_id, size_t data_size);
	// Get entire lump data
	void* get_lump_data(size_t lump_id);
};