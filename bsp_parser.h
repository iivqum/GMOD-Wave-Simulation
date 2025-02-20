#pragma once

#include <filesystem>
#include <vector>

#include "bspfile.h"

using namespace std;

/*
	Simple BSP parser
*/

class bsp_data;

// A class that references a lump
template <typename lump_type>
class bsp_lump {
public:
	bsp_lump(bsp_data* bsp, size_t lump_id) : 
		data(reinterpret_cast<lump_type*>(bsp->get_lump_data(lump_id))),
		length(bsp->get_num_entries(lump_id, sizeof(lump_type))) {}

	size_t get_length(void) {return this->length;}

	lump_type* get_entry(size_t index) {
		if (index >= length) {
			return nullptr;
		}
		return &data[index];
	};
private:
	lump_type* data = nullptr;
	// How many data structures
	size_t length = 0;
};

// Stores all BSP data as generic lump blocks
class bsp_data {
private:
	// Generic data block containing lump data
	struct lump_data {
		// Length of data block
		size_t length = 0;
		// Pointer to start of data block
		void* data;
	};
	lump_data lumps[HEADER_LUMPS];
public:
	bsp_data() = default;
	bsp_data(filesystem::path loc);
	~bsp_data(void);
	// Load a BSP from file
	bool load(filesystem::path loc);
	// Cleanup resources
	void cleanup();
	// Get number of data structures inside of a lump
	size_t get_num_entries(size_t lump_id, size_t data_size);
	// Get entire lump data
	void* get_lump_data(size_t lump_id);
};

/*
	Creates an octree using the BSP data by subdividing space against
	brush faces
*/
class bsp_octree {
private:
	// Octree node
	struct node {
		node* children[8];
		// Faces contained by this node
		vector<dface_t*> faces;
		// Extents of the node
		Vector min, max;
	};
	bsp_lump<dface_t> faces;
	bsp_lump<dvertex_t> vertexes;
	bsp_lump<dedge_t> edges;
	bsp_data* bsp;
	node root;
public:
	bsp_octree(bsp_data* data) : bsp(data), faces(data, LUMP_FACES), 
		edges(data, LUMP_EDGES), vertexes(data, LUMP_VERTEXES) {
		this->get_extents(&root.min, &root.max);
	}
	~bsp_octree(void);
	void cleanup(void);
	bool build(void);
	// Get extents of all geometry
	void get_extents(Vector* min, Vector* max);
};