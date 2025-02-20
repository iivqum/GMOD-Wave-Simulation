
#include <iostream>
#include <fstream>
#include <stdexcept>

#include "GarrysMod/Lua/Interface.h"
#include "GarrysMod/Lua/LuaInterface.h"
#include "bspfile.h"
#include "bsp_parser.h"

using namespace std;

LUA_FUNCTION(OpenBSP) {
	filesystem::path loc = LUA->GetString(1);
	bsp_data parser;
	// %ls is for wide character strings such as those returned from c_str()
	Msg("Attemping to load BSP %ls\n", loc.filename().c_str());
	// TODO make extention case insensitive?
	if (loc.extension() != ".bsp") {
		Warning("Warning! Passed file is not a BSP!\n");
	}

	if (parser.load(loc)) {
		bsp_octree octree(&parser);
		Vector min, max;

		octree.get_extents(&min, &max);

		LUA->PushBool(true);
		LUA->PushVector(min);
		LUA->PushVector(max);

		Msg("Parse Succesful!\n");
	}
	else {
		LUA->PushBool(false);
		LUA->PushVector(Vector());
		LUA->PushVector(Vector());

		Warning("Parse failed!\n");
	}

	return 3;
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