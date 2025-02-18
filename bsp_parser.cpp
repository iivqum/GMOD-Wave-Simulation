#include <iostream>
#include <fstream>
#include <stdexcept>
#include <filesystem>

#include "bsp_parser.h"
#include "bspfile.h"

using namespace std;

bsp_parser::bsp_parser(const char* loc = nullptr) {
	if (loc == nullptr) {
		return;
	}
	// TODO make desired lumps selectable
	// Dunno if copying paths around is a good thing, probably isn't
	load(loc);
}

bsp_parser::~bsp_parser() {
	this->cleanup();
}

void bsp_parser::cleanup() {
	for (int i = 0; i < HEADER_LUMPS; i++) {
		delete this->lumps[i].data;
	}
}

bool bsp_parser::load(const char* loc) {
	try {
		ifstream file;

		file.exceptions(ifstream::failbit | ifstream::badbit);
		file.open(loc, ifstream::binary);

		dheader_t header;

		file.read((char*)&header, sizeof(dheader_t));

		if (header.ident != IDBSPHEADER || header.version != MINBSPVERSION) {
			throw ifstream::failure("Invalid VBSP");
		}

		// Loop through lumps and extract data

		for (int i = 0; i < HEADER_LUMPS; i++) {
			lump_t *lump = &header.lumps[i];

			this->lumps[i].data = new char[lump->filelen];
			this->lumps[i].length = lump->filelen;

			file.seekg(lump->fileofs);
			file.read((char*)this->lumps[i].data, lump->filelen);
		}

		// file is closed when it leaves scope
	}
	catch (exception& ex) {
		// TODO error handling
		
		return false;
	}

	return true;
}

size_t bsp_parser::get_num_entries(size_t lump_id, size_t data_size) {
	if (lump_id >= HEADER_LUMPS) {
		return 0;
	}
	return this->lumps[lump_id].length / data_size;
}

void* bsp_parser::get_lump_data(size_t lump_id) {
	if (lump_id >= HEADER_LUMPS) {
		return nullptr;
	}
	return this->lumps[lump_id].data;
}