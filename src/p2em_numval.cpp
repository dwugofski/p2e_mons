#include "p2em_core.hpp"
#include <errno.h>

using namespace p2em_core;

NumVal::NumVal() {
	_value = 0;
}

NumVal::NumVal(const string& name) : Attribute(name) {
	_value = 0;
}

NumVal::NumVal(const double& baseval) {
	_value = baseval;
}

NumVal::NumVal(const string& name, const double& baseval) : Attribute(name) {
	_value = baseval;
}

NumVal::NumVal(const NumVal& source) {
	NumVal(source.name(), source.value());
}
//
//NumVal::NumVal(const Attribute& rhs) {
//	operator=(rhs);
//}

AttrType NumVal::attrType() const {
	return AttrType::NUMERIC;
}

string NumVal::toString() const {
	char buff[STRING_BUFFER_SIZE];
	snprintf(buff, STRING_BUFFER_SIZE, "%g", (double)(value()));
	return string(buff);
}

double NumVal::value() const {
	return _value;
}

void NumVal::value(const double& newval) {
	_value = newval;
	this->update();
}

bool NumVal::validate(const string& newval) const {
	double converted = strtod(newval.c_str(), nullptr);
	return (errno == ERANGE);
}

NumVal& NumVal::operator=(const NumVal& rhs) {
	value(rhs.value());
	return *this;
}

NumVal& NumVal::operator=(const double& rhs) {
	value(rhs);
	return *this;
}

NumVal& NumVal::operator=(const string& newvalue) {
	if (!validate(newvalue)) throw new Exception(ExceptionCode::INVALID_VALUE);
	double converted = strtod(newvalue.c_str(), nullptr);
	if (errno == ERANGE) throw new Exception(ExceptionCode::INVALID_VALUE);
	value((double)converted);
	return *this;
}

NumVal::operator double() const {
	return value();
}

bool NumVal::operator==(const NumVal& rhs) const {
	return rhs.value() == value();
}

bool NumVal::operator==(const double& rhs) const {
	return rhs == value();
}

double NumVal::operator+(const double& rhs) { return value() + rhs; }
double NumVal::operator+(const NumVal& rhs) { return operator+(rhs.value()); }
NumVal& NumVal::operator+=(const double& rhs) { value(value() + rhs); return *this; }
NumVal& NumVal::operator+=(const NumVal& rhs) { return operator+=(rhs.value()); }

double NumVal::operator-(const double& rhs) { return value() - rhs; }
double NumVal::operator-(const NumVal& rhs) { return operator-(rhs.value()); }
NumVal& NumVal::operator-=(const double& rhs) { value(value() - rhs); return *this; }
NumVal& NumVal::operator-=(const NumVal& rhs) { return operator-=(rhs.value()); }

double NumVal::operator*(const double& rhs) { return value() * rhs; }
double NumVal::operator*(const NumVal& rhs) { return operator*(rhs.value()); }
NumVal& NumVal::operator*=(const double& rhs) { value(value() * rhs); return *this; }
NumVal& NumVal::operator*=(const NumVal& rhs) { return operator*=(rhs.value()); }

double NumVal::operator/(const double& rhs) { return value() * rhs; }
double NumVal::operator/(const NumVal& rhs) { return operator*(rhs.value()); }
NumVal& NumVal::operator/=(const double& rhs) { value(value() * rhs); return *this; }
NumVal& NumVal::operator/=(const NumVal& rhs) { return operator*=(rhs.value()); }

// --------------------------------------------------
// OverrideableNumVal -------------------------------
// --------------------------------------------------


OverrideableNumVal::OverrideableNumVal() : NumVal() {
	this->_override = false;
}


OverrideableNumVal::OverrideableNumVal(const string& name) : NumVal(name) {
	this->_override = false;
}


OverrideableNumVal::OverrideableNumVal(const double& baseval) : NumVal(baseval) {
	this->_override = true;
}


OverrideableNumVal::OverrideableNumVal(const string& name, const double& baseval) : NumVal(name, baseval) {
	this->_override = true;
}


void OverrideableNumVal::value(const double& newval) {
	this->_value = newval;
	this->_override = true;
	this->update();
}


bool OverrideableNumVal::overridden() const {
	return this->_override;
}


void OverrideableNumVal::set_override(bool newoverride) {
	if (newoverride != _override) {
		this->_override = newoverride;
		this->update();
	}
}


void OverrideableNumVal::toggle_override() {
	set_override(!overridden());
	this->update();
}