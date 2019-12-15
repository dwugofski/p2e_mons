
#include "p2em_core.h"

using namespace p2em_core;

Attribute::Attribute() {
	_name = "";
}

Attribute::Attribute(const string& name) {
	_name = name;
}

string Attribute::name() const {
	return _name;
}

void Attribute::name(const string& name) {
	_name = name;
}