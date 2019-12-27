#include "p2em_core.hpp"
#include <algorithm>

#define P2EM_XML_TRAITLIST_TAGNAME "traits"
#define P2EM_XML_TRAIT_TAGNAME "trait"

using namespace p2em_core;

// --------------------------------------------------
// Traited ------------------------------------------
// --------------------------------------------------

Traited::Traited() {
	_traits = vector<string>();
}

Traited::Traited(const vector<string>& traits) {
	_traits = vector<string>(traits);
}

Traited::Traited(const Traited& source) {
	_traits = vector<string>(source._traits);
}

bool Traited::hasTrait(const string& Ctrait) const {
	string trait;
	std::transform(Ctrait.begin(), Ctrait.end(), trait.begin(), std::tolower);
	for (vector<string>::const_iterator it = _traits.begin(); it != _traits.end(); it++)
		if ((*it) == trait) return true;
	return false;
}

void Traited::addTrait(const string& Ctrait) {
	if (hasTrait(Ctrait)) return;
	string trait;
	std::transform(Ctrait.begin(), Ctrait.end(), trait.begin(), std::tolower);
	_traits.push_back(trait);
}

void Traited::removeTrait(const string& Ctrait) {
	if (!hasTrait(Ctrait)) return;
	string trait;
	vector<string>::const_iterator it;
	std::transform(Ctrait.begin(), Ctrait.end(), trait.begin(), std::tolower);
	for (it = _traits.begin(); it != _traits.end(); it++) {
		if (*it == trait) {
			_traits.erase(it);
			break;
		}
	}
}

void Traited::parse(const xml::Element* source) {
	if (!source->has_element(P2EM_XML_TRAITLIST_TAGNAME)) return; // If there is no traitlist, don't panic
	vector<xml::Element*> traits = source->first_element(P2EM_XML_TRAITLIST_TAGNAME)->child_elements(P2EM_XML_TRAIT_TAGNAME);
	for (vector<xml::Element*>::iterator it = traits.begin(); it != traits.end(); it++) {
		addTrait((*it)->text());
	}
}

// --------------------------------------------------
// Object -------------------------------------------
// --------------------------------------------------

const string Object::NULL_NAME_STR = "__NULL__";
const uint Object::NULL_ID = 0;

Object::Object(const Registry& reg) : Traited() {
	_attrs = umap<string, Attribute*>();
	_ownership = umap<string, bool>();
	_id = reg.newid();
	_name = Object::NULL_NAME_STR;
}

Object::Object(const Registry& reg, const string& name) : Traited() {
	_attrs = umap<string, Attribute*>();
	_ownership = umap<string, bool>();
	_id = reg.newid();
	_name = name;
}

Object::Object(const Registry& reg, const uint& id) : Traited() {
	_attrs = umap<string, Attribute*>();
	_ownership = umap<string, bool>();
	if (reg.has(id)) throw new Exception(ExceptionCode::INVALID_ID);
	else _id = id;
	_name = "";
}

Object::Object(const Registry& reg, const uint& id, const string& name) : Traited() {
	_attrs = umap<string, Attribute*>();
	_ownership = umap<string, bool>();
	if (reg.has(id)) throw new Exception(ExceptionCode::INVALID_ID);
	else _id = id;
	_name = name;
}

Object::Object(const Registry& reg, const vector<string>& traits) : Traited(traits) {
	_attrs = umap<string, Attribute*>();
	_ownership = umap<string, bool>();
	_id = reg.newid();
	_name = "";
}

Object::Object(const Registry& reg, const vector<string>& traits, const string& name) : Traited(traits) {
	_attrs = umap<string, Attribute*>();
	_ownership = umap<string, bool>();
	_id = reg.newid();
	_name = name;
}

Object::Object(const Registry& reg, const vector<string>& traits, const uint& id) : Traited(traits) {
	_attrs = umap<string, Attribute*>();
	_ownership = umap<string, bool>();
	if (reg.has(id)) throw new Exception(ExceptionCode::INVALID_ID);
	else _id = id;
	_name = "";
}

Object::Object(const Registry& reg, const vector<string>& traits, const uint& id, const string& name) : Traited(traits) {
	_attrs = umap<string, Attribute*>();
	_ownership = umap<string, bool>();
	if (reg.has(id)) throw new Exception(ExceptionCode::INVALID_ID);
	else _id = id;
	_name = name;
}

Object::Object(const Object& source) : Traited(source._traits) {
	_attrs = umap<string, Attribute*>();
	_ownership = umap<string, bool>();
	// Do we copy over the attributes? We would have to do some finesse with the Attribute class to make it possible to copy() and override, returning an Attribute*
	_id = source.reg().newid();
	_name = source._name;
}

Object::~Object() {
	for (umap<string, Attribute*>::iterator it = _attrs.begin(); it != _attrs.end(); it++) {
		if (_ownership[(*it).first]) {
			delete (*it).second;
		}
	}
}

bool Object::hasAttr(const string& attrname) const {
	return _attrs.find(attrname) != _attrs.end();
}

bool Object::hasAttr(const Attribute& attr) const {
	return hasAttr(attr.name());
}

void Object::addAttr(Attribute& newattr) {
	if (hasAttr(newattr)) {
		if (_ownership[newattr.name()]) delete _attrs[newattr.name()];
		_attrs[newattr.name()] = &newattr;
	}
	else {
		_attrs.insert({ newattr.name(), &newattr });
		_ownership.insert({ newattr.name(), false });
	}
}

void Object::removeAttr(const string& oldattrname) {
	if (!hasAttr(oldattrname)) return;
	if (_ownership[oldattrname]) delete _attrs[oldattrname];
	_attrs.erase(oldattrname);
}

void Object::removeAttr(const Attribute& oldattr) {
	removeAttr(oldattr.name());
}

string Object::get(const string& name) const {
	if (!hasAttr(name)) return "";
	umap<string, Attribute*>::const_iterator it = _attrs.find(name);
	return (it->second)->toString();
}
string Object::get(const Attribute& attr) const { return get(attr.name()); }

double Object::getD(const string& name) const {
	if (!hasAttr(name)) return NAN;
	umap<string, Attribute*>::const_iterator it = _attrs.find(name);
	string value = (it->second)->toString();
	if (it->second->attrType() != AttrType::NUMERIC) throw new Exception(ExceptionCode::ATTR_TYPE_MISMATCH);
	return strtod(value.c_str(), nullptr);
}
double Object::getD(const Attribute& attr) const { return getD(attr.name()); }

int Object::getInt(const string& name) const { return (int)(getD(name)); }
int Object::getInt(const Attribute& attr) const { return getInt(attr.name()); }
uint Object::getUInt(const string& name) const { return (uint)(getD(name)); }
uint Object::getUInt(const Attribute& attr) const { return getUInt(attr.name()); }

uint Object::id() const {
	return _id;
}

string Object::name() const {
	return _name;
}

void Object::name(const string& newname) {
	// May want to do some cleaning up / validation on this string
	_name = newname;
}

const Attribute& Object::operator[](const string& name) const {
	umap<string, Attribute*>::const_iterator it = _attrs.find(name);
	return *(it->second);
}
Attribute& Object::operator[](const string& name) { return const_cast<Attribute&>(static_cast<const Object*>(this)->operator[](name)); }
const Attribute& Object::operator[](const Attribute& attr) const { return operator[](attr.name()); }
Attribute& Object::operator[](const Attribute& attr) { return operator[](attr.name()); }

bool Object::operator==(const Object& rhs) const {
	if (!_attrs.size() == rhs._attrs.size()) return false;
	else {
		for (umap<string, Attribute*>::const_iterator it = _attrs.begin(); it != _attrs.end(); it++) {
			if (!rhs.hasAttr(it->first)) return false;
			if (it->second->toString() != rhs[it->first].toString()) return false;
		}
		return true;
	}
}