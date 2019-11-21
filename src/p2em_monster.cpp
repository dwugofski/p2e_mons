
#include "p2em_core.h"
#include <ctime>
#include <cstdlib>

#define ID_CREATE_TRYCOUNT_LIMIT	100

using namespace p2em_core;

void Monster::_init(Core* core, uint id) {
	std::pair<uint, Monster*> myval;

	// Set the id and name
	_id = id;
	_name = P2EMON_NULL_NAME_STRING;

	core->add(this);
	_core = core;
}

Monster::Monster(Core* core) {
	uint trycount = 0;
	uint id = 0;
	bool found = true;

	// Try to find an unused ID
	while (found) {
		if (trycount++ >= ID_CREATE_TRYCOUNT_LIMIT) throw P2EMonException(P2EMON_ERR_ID_CREATION_TIMEOUT);
		id = std::rand();
		found = core->has(id);
	}

	_init(core, id);
}

Monster::Monster(Core* core, uint id) {
	_core = core;
	_init(core, id);
}

Monster::Monster(Core* core, const string& name) : Monster::Monster(core) {
	_name = name;
}

Monster::Monster(Core* core, uint id, const string& name) : Monster::Monster(core, id) {
	_name = name;
}

uint Monster::id() const {
	return _id;
}

void Monster::id(uint id) {
	std::pair<uint, Monster*> myval;

	// If we find a monster with that ID already, do not create it
	if (_core->has(id)) throw P2EMonException(P2EMON_ERR_DUPLICATE_MONSTER);

	// Make sure the current monster exists
	if (!_core->has(_id)) throw P2EMonException(P2EMON_ERR_MONSTER_NOT_FOUND);
	if (_core->get(_id) != this) throw P2EMonException(P2EMON_ERR_MONSTER_MISMATCH);

	// Remove the current monster + id pair from the record
	_core->remove(this);

	// Set the id
	_id = id;

	// Add ID to the list
	_core->add(this);
}

string Monster::name() const {
	return _name;
}

void Monster::name(const string& newname) {
	if (newname == P2EMON_NULL_NAME_STRING) throw P2EMonException(P2EMON_ERR_INVALID_NAME);

	_name = newname;
}