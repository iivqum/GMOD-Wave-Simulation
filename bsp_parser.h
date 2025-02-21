#pragma once

#include <filesystem>
#include <vector>

#include "bspfile.h"

using namespace std;

/*
Simple BSP parser

TODO Add BVH
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
	}
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

class bsp_octree_node {
private:
	// Faces contained by this node
	vector<dface_t*> faces;
	// Extents of the node
	Vector box_min, box_max;
	// If this node has children or not
	bool is_leaf = true;
public:
	bsp_octree_node* leafs[8] = {nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr};

	bsp_octree_node() = default;
	bsp_octree_node(Vector p0, Vector p1) : box_min(p0), box_max(p1) {}
	~bsp_octree_node() {
		this->cleanup();
	}
	void cleanup(void) const {
		if (this->is_leaf) {
			return;
		}
		for (int i = 0; i < 8; i++) {
			delete leafs[i];
		}
	}
	// Check if line segment intersects this node
	bool lineseg_intersect(Vector& p0, Vector& p1) {
		float t_min = 0;
		float t_max = INFINITY;
		Vector d = p1 - p0;
		for (int i = 0; i < 3; i++) {
			float t1 = (this->box_min[i] - p0[i]) / d[i];
			float t2 = (this->box_max[i] - p0[i]) / d[i];
			t_min = fmax(t_min, fmin(t1, t2));
			t_max = fmin(t_max, fmax(t1, t2));
		}
		return t_min < t_max;
	}
	bool split(void) {
		if (!this->is_leaf) {
			// ERROR
			return false;
		}
		// TODO Optimize unnecessary initializations
		Vector dims = (this->box_max + this->box_min) * 0.5;

		this->leafs[0] = new bsp_octree_node(this->box_min, dims);
		this->leafs[1] = new bsp_octree_node(Vector(dims.x, this->box_min.y, this->box_min.z), 
			Vector(this->box_max.x, dims.y, dims.z));
		this->leafs[2] = new bsp_octree_node(Vector(this->box_min.x, this->box_min.y, dims.z), 
			Vector(dims.x, dims.y, this->box_max.z));
		this->leafs[3] = new bsp_octree_node(Vector(dims.x, this->box_min.y, dims.z), 
			Vector(this->box_max.x, dims.y, this->box_max.z));
		this->leafs[4] = new bsp_octree_node(Vector(this->box_min.x, dims.y, dims.z),
			Vector(dims.x, this->box_max.y, this->box_max.z));
		this->leafs[5] = new bsp_octree_node(dims, this->box_max);
		this->leafs[6] = new bsp_octree_node(Vector(this->box_min.x, dims.y, this->box_min.z),
			Vector(dims.x, this->box_max.y, dims.z));
		this->leafs[7] = new bsp_octree_node(Vector(dims.x, dims.y, this->box_min.z),
			Vector(this->box_max.x, this->box_max.y, dims.z));

		this->is_leaf = false;
		return true;
	}
	bool has_children(void) const {
		return !this->is_leaf;
	}
	void get_extents(Vector* p0, Vector* p1) const {
		*p0 = this->box_min;
		*p1 = this->box_max;
	}
	void set_extents(Vector* p0, Vector* p1) {
		this->box_min = *p0;
		this->box_max = *p1;
	}

};

class bsp_octree {
private:
	bsp_lump<dface_t> faces;
	bsp_lump<dvertex_t> vertexes;
	bsp_lump<dedge_t> edges;
	bsp_data* bsp;
	bsp_octree_node root;
public:
	bsp_octree(bsp_data* data) : bsp(data), faces(data, LUMP_FACES),
		edges(data, LUMP_EDGES), vertexes(data, LUMP_VERTEXES), root() {}
	~bsp_octree(void);
	void cleanup(void);
	bool build(void);
	// Get extents of all geometry
	void get_extents(Vector* min, Vector* max);
};