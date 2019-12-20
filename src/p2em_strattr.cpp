#include "p2em_core.h"

using namespace p2em_core;

StrAttr::StrAttr() : Attribute() {
	_value = "";
}

StrAttr::StrAttr(const string& value) : Attribute() {
	_value = value;
}

StrAttr::StrAttr(const string& name, const string& value) : Attribute(name) {
	_value = value;
}

StrAttr::StrAttr(const StrAttr& source) : Attribute(source) {
	if (!validate(source.value())) throw new Exception(ExceptionCode::INVALID_VALUE);
	_value = source.value();
}

AttrType StrAttr::attrType() const {
	return AttrType::STRING;
}

string StrAttr::value() const {
	return _value;
}

void StrAttr::value(const string& newval) {
	if (_value == newval) return;
	if (!validate(newval)) throw new Exception(ExceptionCode::INVALID_VALUE);
	_value = newval;
	update();
}

string StrAttr::toString() const {
	return value();
}

bool StrAttr::validate(const string& teststr) const {
	return true;
}

bool StrAttr::operator==(const string& rhs) const {
	return value() == rhs;
}

bool StrAttr::operator==(const StrAttr& rhs) const {
	return value() == rhs.value();
}

string StrAttr::operator+(const string& rhs) const {
	return value() + rhs;
}

string StrAttr::operator+(const StrAttr& rhs) const {
	return value() + rhs.value();
}

StrAttr& StrAttr::operator=(const string& rhs) {
	if (!validate(rhs)) throw new Exception(ExceptionCode::INVALID_VALUE);
	value(rhs);
	return *this;
}

StrAttr& StrAttr::operator=(const StrAttr& rhs) {
	return operator=(rhs.value());
}

StrAttr& StrAttr::operator+=(const string& rhs) {
	if (!validate(value() + rhs)) throw new Exception(ExceptionCode::INVALID_VALUE);
	value(value() + rhs);
	return *this;
}

StrAttr& StrAttr::operator+=(const StrAttr& rhs) {
	return operator+=(rhs.value());
}

StrAttr::operator string() const {
	return value();
}

Attribute& StrAttr::operator=(const Attribute& rhs) {
	return operator=(rhs.toString());
}