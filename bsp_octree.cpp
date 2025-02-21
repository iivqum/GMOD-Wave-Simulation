#include <iostream>
#include <fstream>
#include <stdexcept>

#include "GarrysMod/Lua/Interface.h"
#include "GarrysMod/Lua/LuaInterface.h"
#include "bspfile.h"
#include "bsp_parser.h"

bsp_octree::~bsp_octree() {
	this->cleanup();
}

void bsp_octree::cleanup() {
	//delete &(this->root);
}

void bsp_octree::get_extents(Vector* min, Vector* max) {
	*min = Vector(0, 0, 0);
	*max = Vector(0, 0, 0);

	for (int i = 0; i < this->vertexes.get_length(); i++) {
		dvertex_t* vert = this->vertexes.get_entry(i);
		
		*min = min->Min(vert->point);
		*max = max->Max(vert->point);
	}
}

bool bsp_octree::build() {
	bsp_data* bsp = this->bsp;
	vector<bsp_octree_node*> split_queue;
	// Push root node

	Vector min, max;
	this->get_extents(&min, &max);
	root.set_extents(&min, &max);

	split_queue.push_back(&root);

	for (int x = 0; x < 10; x++) {
		if (split_queue.empty()) {
			return true;
		}

		bsp_octree_node* node = split_queue.back();
		// Done with the node
		split_queue.pop_back();

		node->split();

		node->get_extents(&min, &max);
		Msg("Splitting node:\n min = (%f,%f,%f), max = (%f,%f,%f)\n", min.x, min.y, min.z, max.x, max.y, max.z);

		Msg("CHILDREN:\n");

		for (int i = 0; i < 8; i++) {
			node->leafs[i]->get_extents(&min, &max);
			Msg("min = (%f,%f,%f), max = (%f,%f,%f)\n", min.x, min.y, min.z, max.x, max.y, max.z);
		}

		if (node->has_children()) {
			continue;
		}
		/*
		for (int face_id = 0; face_id < this->faces.get_length(); face_id++) {
			dface_t* face = this->faces.get_entry(face_id);

			bool split = false;

			for (int edge_id = 0; edge_id < face->numedges; edge_id++) {
				dedge_t* edge = this->edges.get_entry(edge_id + face->firstedge);

				dvertex_t* p0 = this->vertexes.get_entry(edge->v[0]);
				dvertex_t* p1 = this->vertexes.get_entry(edge->v[1]);

				if (node->lineseg_intersect(p0->point, p1->point)) {
					if (!node->split()) {
						// ERROR
						return false;
					}
					Msg("Split!\n");
					// Push children to queue
					for (int i = 0; i < 8; i++) {
						split_queue.push_back(node->leafs[i]);
					}
					split = true;
					break;
				}
			}
			if (split) {
				break;
			}
		}
		*/
	}
	return true;
}