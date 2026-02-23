#include <cstdlib>
#include <cassert>

#include "map.h"

static const char map[] =	"1111111111111111"\
							"1000010000000001"\
							"1000010010000001"\
							"1000010011100001"\
							"1000010000100001"\
							"1000011111100001"\
							"1000000000000001"\
							"1000000000000001"\
							"1000000000000001"\
							"1000000000000001"\
							"1000000000000001"\
							"1000001111110001"\
							"1000001000010001"\
							"1000000000000001"\
							"1000000000000001"\
							"1111111111111111";

Map::Map() : w(16), h(16) {
	assert(sizeof(map) == w * h + 1);
}

int Map::get(const size_t i, const size_t j) {
	return map[i + j * w] - '0';
}

bool Map::is_empty(const size_t i, const size_t j) {
	return map[i + j * w] == '0';
}