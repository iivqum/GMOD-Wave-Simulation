#include <iostream>
#include <fstream>
#include <stdexcept>
#include <filesystem>

#include "bsp_parser.h"
#include "bspfile.h"

using namespace std;

bsp_parser::bsp_parser(filesystem::path loc) {
	if (loc.empty()) {
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

bool bsp_parser::load(filesystem::path loc) {
	/*
		Returns true if parse was successful
	*/
	try {
		ifstream file;

		file.exceptions(ifstream::failbit | ifstream::badbit);
		file.open(loc, ifstream::binary);

		dheader_t header;

		file.read((char*)&header, sizeof(dheader_t));

		// Can check header.version < MINBSPVERSION if desired
		if (header.ident != IDBSPHEADER) {
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
	// TODO data length is a multiple of data_size
	// And ensure it can fit
	return this->lumps[lump_id].length / data_size;
}

void* bsp_parser::get_lump_data(size_t lump_id) {
	if (lump_id >= HEADER_LUMPS) {
		return nullptr;
	}
	return this->lumps[lump_id].data;
}