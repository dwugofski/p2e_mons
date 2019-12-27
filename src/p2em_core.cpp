#include "p2em_core.hpp"
#include <stdlib.h>
#include <time.h>
#include <algorithm>

#define ID_CREATE_TRYCOUNT_LIMIT	100

using namespace p2em_core;

Exception::Exception(ExceptionCode excode, const string& msg) {
	_code = excode;
	_msg = msg;
}

Exception::Exception(ExceptionCode excode) : Exception::Exception(excode, P2EMonExceptionStrings[(int)(excode)]) {} ;

ExceptionCode Exception::code() const { return _code; }
string Exception::msg() const { return _msg; }

Core::Core() {
	umap<uint, Monster*> _monsters = { {Object::NULL_ID, nullptr} };
}

bool Core::has(uint id) const {
	return (_monsters.find(id) != _monsters.end());
}

bool Core::has(const Monster* mon) const {
	return has(mon->id());
}

bool Core::has(const string& name) const {
	for (umap<uint, Monster*>::const_iterator it = _monsters.begin(); it != _monsters.end(); it++) {
		if ((*it).second->name() == name) return true;
	}
	return false;
}

uint Core::newid() const {
	uint trycount = 0;
	uint id = 0;
	bool found = true;

	srand((unsigned int)(time(0)));

	// Try to find an unused ID
	while (found) {
		if (trycount++ >= ID_CREATE_TRYCOUNT_LIMIT) throw Exception(ExceptionCode::ID_CREATION_TIMEOUT);
		id = rand();
		found = has(id);
	}

	return id;
}

Monster* Core::get(uint id) {
	if (!has(id)) throw Exception(ExceptionCode::MONSTER_NOT_FOUND);
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
		else throw Exception(ExceptionCode::DUPLICATE_MONSTER);
	}
	else {
		// Add the new monster
		_monsters.insert({ mon->id(), mon });
	}

	// Should not reach this
	throw Exception(ExceptionCode::ERROR);
}

void Core::remove(Monster* mon) {
	// Check if monster is already in record
	if (has(mon->id())) {
		// If the monster matches the record, remove it; otherwise raise an exception
		if (_monsters[mon->id()] == mon) _monsters.erase(mon->id());
		else throw Exception(ExceptionCode::MONSTER_MISMATCH);
	}
	else {
		// If the monster is not in the record, do nothing
		return;
	}

	// Should not reach this
	throw Exception(ExceptionCode::ERROR);
}

void Core::remove(uint id) {
	// If the monster is already in record, remove it. Otherwise do nothing
	if (has(id)) remove(_monsters[id]);
}

ActionCount Core::actionCount(const string& source) {
	string lower;
	std::transform(source.begin(), source.end(), lower.begin(), std::tolower);
	if (lower == "reaction") return ActionCount::REACTION;
	else if (lower == "passive") return ActionCount::PASSIVE;
	else if (lower == "free") return ActionCount::FREE;
	else if (lower == "1" || lower == "single") return ActionCount::SINGLE;
	else if (lower == "2" || lower == "double") return ActionCount::DOUBLE;
	else if (lower == "3" || lower == "triple") return ActionCount::TRIPLE;
	else throw new Exception(ExceptionCode::INVALID_VALUE, "Cannot parse '" + source + "' into an ActionCount!");
}

// --------------------------------------------------
// Feat Class ---------------------------------------
// --------------------------------------------------

Feat::Feat() : Traited() {}
Feat::Feat(const xml::Element* source) : Traited() { parse(source); }

void Feat::parse(const xml::Element* source) {
	name = source->first_element("name")->text();
	description = source->first_element("description")->text();
	action_count = Core::actionCount(source->first_element("count")->text());
	((Traited*)(this))->parse(source);
}