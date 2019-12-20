#pragma once
#ifndef __P2EM_MODVAL_HPP__
#define __P2EM_MODVAL_HPP__

#include "p2em_core.h"
#include <sstream>
#include "../src/p2em_numval.hpp"

#define MODVAL_VALUE_DELIMITER ','

using namespace p2em_core;

template<typename T_num_t>
void ModVal<T_num_t>::_link() {
	_base.addDependency(*this);
	_offset.addDependency(*this);
}

template<typename T_num_t>
vector<T_num_t> ModVal<T_num_t>::_splitstring(const string& newval) const {
	vector<T_num_t> ret = vector<T_num_t>();
	double parsed = 0.0;
	std::stringstream ss(newval);
	string token;
	while (std::getline(ss, token, MODVAL_VALUE_DELIMITER)) {
		parsed = strtod(token.c_str(), nullptr);
		if (errno == ERANGE) throw new Exception(ExceptionCode::INVALID_VALUE);
		ret.push_back((T_num_t)parsed);
	}
	return ret;
}

template<typename T_num_t>
ModVal<T_num_t>::ModVal() : OverrideableNumVal<T_num_t>(), _base(*new NumVal<T_num_t>(0)), _offset(*new NumVal<T_num_t>(0)) {
	// NumVal<T_num_t>* nbase = new NumVal<T_num_t>(0);
	// NumVal<T_num_t>* noffset = new NumVal<T_num_t>(0);
	_ownsbase = true;
	_ownsoffset = true;
	// _base = *nbase;
	// _offset = *noffset;
	// _value = 0; // Redundant, with base class
	// _override = false; // Redundant, with base class
	_link();
}

template<typename T_num_t>
ModVal<T_num_t>::ModVal(const string& name) : OverrideableNumVal<T_num_t>(name), _base(*new NumVal<T_num_t>(0)), _offset(*new NumVal<T_num_t>(0)) {
	// NumVal<T_num_t>* nbase = new NumVal<T_num_t>(0);
	// NumVal<T_num_t>* noffset = new NumVal<T_num_t>(0);
	_ownsbase = true;
	_ownsoffset = true;
	// _base = *nbase;
	// _offset = *noffset;
	// _value = 0; // Redundant, with base class
	// _override = false; // Redundant, with base class
	_link();
}

template<typename T_num_t>
ModVal<T_num_t>::ModVal(const T_num_t& value) : OverrideableNumVal<T_num_t>(value), _base(*new NumVal<T_num_t>(0)), _offset(*new NumVal<T_num_t>(0)) {
	// NumVal<T_num_t>* nbase = new NumVal<T_num_t>(0);
	// NumVal<T_num_t>* noffset = new NumVal<T_num_t>(0);
	_ownsbase = true;
	_ownsoffset = true;
	// _base = *nbase;
	// _offset = *noffset;
	// _value = value; // Redundant, with base class
	// _override = true; // Redundant, with base class
	_link();
}

template<typename T_num_t>
ModVal<T_num_t>::ModVal(const string& name, const T_num_t& value) : OverrideableNumVal<T_num_t>(name, value), _base(*new NumVal<T_num_t>(0)), _offset(*new NumVal<T_num_t>(0)) {
	// NumVal<T_num_t>* nbase = new NumVal<T_num_t>(0);
	// NumVal<T_num_t>* noffset = new NumVal<T_num_t>(0);
	_ownsbase = true;
	_ownsoffset = true;
	// _base = *nbase;
	// _offset = *noffset;
	// _value = value; // Redundant, with base class
	// _override = true; // Redundant, with base class
	_link();
}

template<typename T_num_t>
ModVal<T_num_t>::ModVal(NumVal<T_num_t>& base) : OverrideableNumVal<T_num_t>(), _offset(*new NumVal<T_num_t>(0)) {
	// NumVal<T_num_t>* nbase = new NumVal<T_num_t>(0);
	// NumVal<T_num_t>* noffset = new NumVal<T_num_t>(0);
	_ownsbase = false;
	_ownsoffset = true;
	_base = base;
	// _offset = *noffset;
	// _value = 0; // Redundant, with base class
	// _override = false; // Redundant, with base class
	_link();
}

template<typename T_num_t>
ModVal<T_num_t>::ModVal(const string& name, NumVal<T_num_t>& base) : OverrideableNumVal<T_num_t>(name), _offset(*new NumVal<T_num_t>(0)) {
	// NumVal<T_num_t>* nbase = new NumVal<T_num_t>(0);
	// NumVal<T_num_t>* noffset = new NumVal<T_num_t>(0);
	_ownsbase = false;
	_ownsoffset = true;
	_base = base;
	// _offset = *noffset;
	// _value = 0; // Redundant, with base class
	// _override = false; // Redundant, with base class
	_link();
}

template<typename T_num_t>
ModVal<T_num_t>::ModVal(NumVal<T_num_t>& base, NumVal<T_num_t>& offset) : OverrideableNumVal<T_num_t>(){
	// NumVal<T_num_t>* nbase = new NumVal<T_num_t>(0);
	// NumVal<T_num_t>* noffset = new NumVal<T_num_t>(0);
	_ownsbase = false;
	_ownsoffset = false;
	_base = base;
	_offset = offset;
	// _value = 0; // Redundant, with base class
	// _override = false; // Redundant, with base class
	_link();
}

template<typename T_num_t>
ModVal<T_num_t>::ModVal(const string& name, NumVal<T_num_t>& base, NumVal<T_num_t>& offset) : OverrideableNumVal<T_num_t>(name) {
	// NumVal<T_num_t>* nbase = new NumVal<T_num_t>(0);
	// NumVal<T_num_t>* noffset = new NumVal<T_num_t>(0);
	_ownsbase = false;
	_ownsoffset = false;
	_base = base;
	_offset = offset;
	// _value = 0; // Redundant, with base class
	// _override = false; // Redundant, with base class
	_link();
}

template<typename T_num_t>
ModVal<T_num_t>::ModVal(NumVal<T_num_t>& base, NumVal<T_num_t>& offset, const T_num_t& value) : OverrideableNumVal<T_num_t>(value) {
	// NumVal<T_num_t>* nbase = new NumVal<T_num_t>(0);
	// NumVal<T_num_t>* noffset = new NumVal<T_num_t>(0);
	_ownsbase = false;
	_ownsoffset = false;
	_base = base;
	_offset = offset;
	// _value = value; // Redundant, with base class
	// _override = true; // Redundant, with base class
	_link();
}

template<typename T_num_t>
ModVal<T_num_t>::ModVal(const string& name, NumVal<T_num_t>& base, NumVal<T_num_t>& offset, const T_num_t& value) : OverrideableNumVal<T_num_t>(name, value) {
	// NumVal<T_num_t>* nbase = new NumVal<T_num_t>(0);
	// NumVal<T_num_t>* noffset = new NumVal<T_num_t>(0);
	_ownsbase = false;
	_ownsoffset = false;
	_base = base;
	_offset = offset;
	// _value = value; // Redundant, with base class
	// _override = true; // Redundant, with base class
	_link();
}

template<typename T_num_t>
ModVal<T_num_t>::ModVal(const ModVal& source) : OverrideableNumVal<T_num_t>(source.name()) {
	_base = source.base();
	_offset = source.offset();
	this->_value = source.value();
	this->_override = source.overridden();
	_link();
}

template<typename T_num_t>
ModVal<T_num_t>::~ModVal() {
	// If we created these with new, we need to delete them
	if (_ownsbase) delete& _base;
	if (_ownsoffset) delete& _offset;
}

template<typename T_num_t>
string ModVal<T_num_t>::toString() const {
	char buff[STRING_BUFFER_SIZE];
	snprintf(buff, STRING_BUFFER_SIZE, "%g%c%g%c%g%c%d", (double)(value()), MODVAL_VALUE_DELIMITER, 
		(double)(base()), MODVAL_VALUE_DELIMITER, (double)(offset()), MODVAL_VALUE_DELIMITER, this->overridden() ? 1 : 0);
	return string(buff);
}

template<typename T_num_t>
T_num_t ModVal<T_num_t>::value() const {
	return ((this->overridden()) ? (this->_value) : (base() + offset()));
}

template<typename T_num_t>
T_num_t ModVal<T_num_t>::base() const {
	return _base.value();
}

template<typename T_num_t>
void ModVal<T_num_t>::base(const T_num_t& newbase) {
	if (_base != newbase) _base = newbase; // Should automatically call update()
}

template<typename T_num_t>
T_num_t ModVal<T_num_t>::offset() const {
	return _offset.value();
}

template<typename T_num_t>
void ModVal<T_num_t>::offset(const T_num_t& newoffset) {
	if(_offset != newoffset) _offset = newoffset; // Should automatically call update()
}

template<typename T_num_t>
bool ModVal<T_num_t>::validate(const string& newval) const {
	vector<T_num_t> values;
	bool override_val = this->overridden();
	try {
		values = _splitstring(newval);
	}
	catch (const Exception e) {
		if (e.code() != ExceptionCode::INVALID_VALUE) throw e;
		else return false;
	}
	if (values.size() <= 0) return false;
	return true;
}

template<typename T_num_t>
void ModVal<T_num_t>::onupdate() {
	if (this->paused()) return;
	NumVal<T_num_t>::onupdate(); // Call base class doupdate() function
}

template<typename T_num_t>
ModVal<T_num_t>& ModVal<T_num_t>::operator=(const ModVal<T_num_t>& rhs) {
	// To limit the number of repeated updates, we pause our base and offset
	// Then we update base and offset values
	// Then we update ourselves (and trigger our dependents)
	// Then we pause ourselves so we don't update in response to base/offset update
	// Then we unpause and manually update our base/offset
	// Then we unpause ourselves and continue as normal
	_base.pause();
	_offset.pause();

	base(rhs.base());
	offset(rhs.offset());
	((OverrideableNumVal<T_num_t>*)(this))->value(rhs.value());
	this->set_override(rhs.overridden());

	this->update();
	this->pause();

	_base.unpause();
	_offset.unpause();
	_base.update();
	_offset.update();

	this->unpause();
	return *this;
}

template<typename T_num_t>
ModVal<T_num_t>& ModVal<T_num_t>::operator=(const string& newval) {
	vector<T_num_t> values;
	bool override_val = this->overridden();
	values = _splitstring(newval);

	if (values.size() <= 0) throw new Exception(ExceptionCode::INVALID_VALUE);
	switch (values.size()) {
	case 0:
		throw new Exception(ExceptionCode::INVALID_VALUE);
	default: // > 4, don't really care about additional values
	case 4: // Override = 0 (false) or != 0 (true)
		override_val = (values[3] != 0); // Need to do this because setting value() may set _override = true
	case 3: // Offset
		this->pause();
		offset(values[2]);
	case 2: // Base
		this->pause();
		base(values[1]);
	case 1: // Value
		this->pause();
		((OverrideableNumVal<T_num_t>*)(this))->value(values[0]);
		this->set_override(override_val);
		this->unpause();
		this->update();
	}
	return *this;
}

template<typename T_num_t>
bool ModVal<T_num_t>::operator==(const ModVal& rhs) const {
	return value() == rhs.value();
}

#endif
