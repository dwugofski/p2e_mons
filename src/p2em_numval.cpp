
#include "p2em_core.h"
#include <errno.h>

#define STRING_BUFFER_SIZE 100

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
	return P2E_ATTR_NUMVAL;
}

template<typename T_num_t>
string NumVal<T_num_t>::toString() const {
	char buff[STRING_BUFFER_SIZE];
	snprintf(buff, STRING_BUFFER_SIZE, "%g", value());
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
Attribute& NumVal<T_num_t>::operator=(const Attribute& rhs) {
	switch (rhs.attrType()) {
	case P2E_ATTR_NUMVAL:
	case P2E_ATTR_MODVAL:
		// Implies a type conversion, but all numeric types must translate between themselves (pretty typical)
		operator=(static_cast<const NumVal<T_num_t>&>(rhs));
		break;
	case P2E_ATTR_UINT:
	case P2E_ATTR_INT:
	case P2E_ATTR_DOUBLE:
		if (!validate(rhs.toString())) throw new Exception(P2EMON_ERR_ATTR_TYPE_MISMATCH);
		operator=(rhs.toString());
		break;
	default:
		throw new Exception(P2EMON_ERR_ATTR_TYPE_MISMATCH);
	}
	return *this;
}

template<typename T_num_t>
NumVal<T_num_t>& NumVal<T_num_t>::operator=(const string& newvalue) {
	if (!validate(newvalue)) throw new Exception(P2EMON_ERR_INVALID_VALUE);
	double converted = strtod(newvalue.c_str(), nullptr);
	if (errno == ERANGE) throw new Exception(P2EMON_ERR_INVALID_VALUE);
	value((T_num_t)newvalue);
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

