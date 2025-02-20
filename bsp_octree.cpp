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

bool bsp_octree::build() {
	bsp_data* bsp = this->bsp;
	


	return true;
}