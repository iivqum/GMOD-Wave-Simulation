
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "GarrysMod/Lua/Interface.h"
#include "bspfile.h"
#include "bsp_parser.h"

using namespace std;

LUA_FUNCTION(OpenBSP) {
	const char* path = LUA->GetString(1);

	bsp_parser parser(nullptr);

	parser.load(path);
	return 1;
}

GMOD_MODULE_OPEN() {
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	LUA->PushCFunction(OpenBSP);
	LUA->SetField(-2, "OpenBSP");
	LUA->Pop();

	return 0;
}

GMOD_MODULE_CLOSE() {
	return 0;
}