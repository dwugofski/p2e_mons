#pragma once
#ifndef __P2EM_NUMVAL_HPP__
#define __P2EM_NUMVAL_HPP__

#include "p2em_core.h"
#include <errno.h>

using namespace p2em_core;

template<typename T_num_t>
NumVal<T_num_t>::NumVal() {
	_value = 0;
}

template<typename T_num_t>
NumVal<T_num_t>::NumVal(const string& name) : Attribute(name) {
	_value = 0;
}

template<typename T_num_t>
NumVal<T_num_t>::NumVal(const T_num_t& baseval) {
	_value = baseval;
}

template<typename T_num_t>
NumVal<T_num_t>::NumVal(const string& name, const T_num_t& baseval) : Attribute(name) {
	_value = baseval;
}

template<typename T_num_t>
NumVal<T_num_t>::NumVal(const NumVal<T_num_t>& source) {
	NumVal(source.name(), source.value());
}

template<typename T_num_t>
NumVal<T_num_t>::NumVal(const Attribute& rhs) {
	operator=(rhs);
}


template<typename T_num_t>
AttrType NumVal<T_num_t>::attrType() const {
	return AttrType::NUMERIC;
}

template<typename T_num_t>
string NumVal<T_num_t>::toString() const {
	char buff[STRING_BUFFER_SIZE];
	snprintf(buff, STRING_BUFFER_SIZE, "%g", (double)(value()));
	return string(buff);
}

template<typename T_num_t>
T_num_t NumVal<T_num_t>::value() const {
	return _value;
}

template<typename T_num_t>
void NumVal<T_num_t>::value(const T_num_t& newval) {
	_value = newval;
	this->update();
}

template<typename T_num_t>
bool NumVal<T_num_t>::validate(const string& newval) const {
	double converted = strtod(newval.c_str(), nullptr);
	return (errno == ERANGE);
}

template<typename T_num_t>
NumVal<T_num_t>& NumVal<T_num_t>::operator=(const NumVal<T_num_t>& rhs) {
	value(rhs.value());
	return *this;
}

template<typename T_num_t>
NumVal<T_num_t>& NumVal<T_num_t>::operator=(const T_num_t& rhs) {
	value(rhs);
	return *this;
}

template<typename T_num_t>
NumVal<T_num_t>& NumVal<T_num_t>::operator=(const string& newvalue) {
	if (!validate(newvalue)) throw new Exception(ExceptionCode::INVALID_VALUE);
	double converted = strtod(newvalue.c_str(), nullptr);
	if (errno == ERANGE) throw new Exception(ExceptionCode::INVALID_VALUE);
	value((T_num_t)converted);
	return *this;
}

template<typename T_num_t>
NumVal<T_num_t>::operator T_num_t() const {
	return value();
}

template<typename T_num_t>
bool NumVal<T_num_t>::operator==(const NumVal<T_num_t>& rhs) const {
	return rhs.value() == value();
}

template<typename T_num_t>
bool NumVal<T_num_t>::operator==(const T_num_t& rhs) const {
	return rhs == value();
}

template<typename T_num_t> T_num_t NumVal<T_num_t>::operator+(const T_num_t& rhs) { return value() + rhs; }
template<typename T_num_t> T_num_t NumVal<T_num_t>::operator+(const NumVal<T_num_t>& rhs) { return operator+(rhs.value()); }
template<typename T_num_t> NumVal<T_num_t>& NumVal<T_num_t>::operator+=(const T_num_t& rhs) { value(value() + rhs); return *this; }
template<typename T_num_t> NumVal<T_num_t>& NumVal<T_num_t>::operator+=(const NumVal<T_num_t>& rhs) { return operator+=(rhs.value()); }

template<typename T_num_t> T_num_t NumVal<T_num_t>::operator-(const T_num_t& rhs) { return value() - rhs; }
template<typename T_num_t> T_num_t NumVal<T_num_t>::operator-(const NumVal<T_num_t>& rhs) { return operator-(rhs.value()); }
template<typename T_num_t> NumVal<T_num_t>& NumVal<T_num_t>::operator-=(const T_num_t& rhs) { value(value() - rhs); return *this; }
template<typename T_num_t> NumVal<T_num_t>& NumVal<T_num_t>::operator-=(const NumVal<T_num_t>& rhs) { return operator-=(rhs.value()); }

template<typename T_num_t> T_num_t NumVal<T_num_t>::operator*(const T_num_t& rhs) { return value() * rhs; }
template<typename T_num_t> T_num_t NumVal<T_num_t>::operator*(const NumVal<T_num_t>& rhs) { return operator*(rhs.value()); }
template<typename T_num_t> NumVal<T_num_t>& NumVal<T_num_t>::operator*=(const T_num_t& rhs) { value(value() * rhs); return *this; }
template<typename T_num_t> NumVal<T_num_t>& NumVal<T_num_t>::operator*=(const NumVal<T_num_t>& rhs) { return operator*=(rhs.value()); }

template<typename T_num_t> T_num_t NumVal<T_num_t>::operator/(const T_num_t& rhs) { return value() * rhs; }
template<typename T_num_t> T_num_t NumVal<T_num_t>::operator/(const NumVal<T_num_t>& rhs) { return operator*(rhs.value()); }
template<typename T_num_t> NumVal<T_num_t>& NumVal<T_num_t>::operator/=(const T_num_t& rhs) { value(value() * rhs); return *this; }
template<typename T_num_t> NumVal<T_num_t>& NumVal<T_num_t>::operator/=(const NumVal<T_num_t>& rhs) { return operator*=(rhs.value()); }

// --------------------------------------------------
// OverrideableNumVal -------------------------------
// --------------------------------------------------

template<typename T_num_t>
OverrideableNumVal<T_num_t>::OverrideableNumVal() : NumVal<T_num_t>() {
	this->_override = false;
}

template<typename T_num_t>
OverrideableNumVal<T_num_t>::OverrideableNumVal(const string& name) : NumVal<T_num_t>(name) {
	this->_override = false;
}

template<typename T_num_t>
OverrideableNumVal<T_num_t>::OverrideableNumVal(const T_num_t& baseval) : NumVal<T_num_t>(baseval) {
	this->_override = true;
}

template<typename T_num_t>
OverrideableNumVal<T_num_t>::OverrideableNumVal(const string& name, const T_num_t& baseval) : NumVal<T_num_t>(name, baseval) {
	this->_override = true;
}

template<typename T_num_t>
void OverrideableNumVal<T_num_t>::value(const T_num_t& newval) {
	this->_value = newval;
	this->_override = true;
	this->update();
}

template<typename T_num_t>
bool OverrideableNumVal<T_num_t>::overridden() const {
	return this->_override;
}

template<typename T_num_t>
void OverrideableNumVal<T_num_t>::set_override(bool newoverride) {
	if (newoverride != _override) {
		this->_override = newoverride;
		this->update();
	}
}

template<typename T_num_t>
void OverrideableNumVal<T_num_t>::toggle_override() {
	set_override(!overridden());
	this->update();
}

#endif