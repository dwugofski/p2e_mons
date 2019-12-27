
#include "p2em_core.hpp"

using namespace p2em_core;

Attribute::Attribute() : Updateable(), Traited() {
	_name = "";
}

Attribute::Attribute(const string& name) : Updateable(), Traited() {
	_name = name;
}

Attribute::Attribute(const Attribute& source) : Updateable(source), Traited() {
	_name = source.name();
}

string Attribute::name() const {
	return _name;
}

void Attribute::name(const string& name) {
	_name = name;
}

//Attribute& Attribute::operator=(const Attribute& rhs) {
//	if (!validate(rhs.toString())) throw new Exception(ExceptionCode::INVALID_VALUE);
//	return operator=(rhs.toString());
//}