#include "bsp_parser.h"

bsp_octree::~bsp_octree() {
	this->cleanup();
}

void bsp_octree::cleanup() {

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

bool bsp_octree::build(int depth) {
	bsp_data* bsp = this->bsp;
	vector<node*> split_queue;
	// Push root node
	this->get_extents(&root.min, &root.max);
	split_queue.push_back(&root);

	while (!split_queue.empty()) {
		// Split whatever is in the queue
		node* n = split_queue.back();

		



		// Pop node off the end
		split_queue.pop_back();
	}

	return true;
}