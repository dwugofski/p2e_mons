
#include "p2em_core.h"

using namespace p2em_core;

Exception::Exception(ExceptionCode excode, const string& msg) {
	_code = excode;
	_msg = msg;
}

Exception::Exception(ExceptionCode excode) : Exception::Exception(excode, P2EMonExceptionStrings[excode]) {} ;

ExceptionCode Exception::code() const { return _code; }
string Exception::msg() const { return _msg; }

Core::Core() {
	umap<uint, Monster*> _monsters = { {P2EMON_NULL_ID, nullptr} };
}

bool Core::has(uint id) const {
	return (_monsters.find(id) != _monsters.end());
}

bool Core::has(const Monster* mon) const {
	return has(mon->id());
}

Monster* Core::get(uint id) {
	if (!has(id)) throw Exception(P2EMON_ERR_MONSTER_NOT_FOUND);
	return _monsters[id];
}

const Monster* Core::get(uint id) const {
	return static_cast<const Monster*>(const_cast<Core*>(this)->operator[](id));
}

Monster* Core::operator[](uint id) { return get(id); }

const Monster* Core::operator[](uint id) const { return get(id); }

void Core::add(Monster* mon) {
	// Check if monster is already in record
	if (has(mon->id())) {
		// If the monster is already in the record, do nothing; otherwise raise an exception
		if (_monsters[mon->id()] == mon) return;
		else throw Exception(P2EMON_ERR_DUPLICATE_MONSTER);
	}
	else {
		// Add the new monster
		_monsters.insert({ mon->id(), mon });
	}

	// Should not reach this
	throw Exception(P2EMON_ERR);
}

void Core::remove(Monster* mon) {
	// Check if monster is already in record
	if (has(mon->id())) {
		// If the monster matches the record, remove it; otherwise raise an exception
		if (_monsters[mon->id()] == mon) _monsters.erase(mon->id());
		else throw Exception(P2EMON_ERR_MONSTER_MISMATCH);
	}
	else {
		// If the monster is not in the record, do nothing
		return;
	}

	// Should not reach this
	throw Exception(P2EMON_ERR);
}

void Core::remove(uint id) {
	// If the monster is already in record, remove it. Otherwise do nothing
	if (has(id)) remove(_monsters[id]);
}