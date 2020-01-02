#include "p2em_core.hpp"
#include <sstream>

using namespace p2em_core;

const char ModVal::VALUE_DELIMITER = ',';

void ModVal::_link() {
	_base->addDependency(*this);
	_offset->addDependency(*this);
}

vector<double> ModVal::_splitstring(const string& newval) const {
	vector<double> ret = vector<double>();
	double parsed = 0.0;
	std::stringstream ss(newval);
	string token;
	while (std::getline(ss, token, ModVal::VALUE_DELIMITER)) {
		parsed = strtod(token.c_str(), nullptr);
		if (errno == ERANGE) throw new Exception(ExceptionCode::INVALID_VALUE);
		ret.push_back((double)parsed);
	}
	return ret;
}

ModVal::ModVal() : OverrideableNumVal(), _base(new NumVal(0)), _offset(new NumVal(0)) {
	_ownsbase = true;
	_ownsoffset = true;
	_link();
}

ModVal::ModVal(const string& name) : OverrideableNumVal(name), _base(new NumVal(0)), _offset(new NumVal(0)) {
	_ownsbase = true;
	_ownsoffset = true;
	_link();
}

ModVal::ModVal(const double& value) : OverrideableNumVal(value), _base(new NumVal(0)), _offset(new NumVal(0)) {
	_ownsbase = true;
	_ownsoffset = true;
	_link();
}

ModVal::ModVal(const string& name, const double& value) : OverrideableNumVal(name, value), _base(new NumVal(0)), _offset(new NumVal(0)) {
	_ownsbase = true;
	_ownsoffset = true;
	_link();
}

ModVal::ModVal(NumVal& base) : OverrideableNumVal(), _base(&base), _offset(new NumVal(0)) {
	_ownsbase = false;
	_ownsoffset = true;
	_link();
}

ModVal::ModVal(const string& name, NumVal& base) : OverrideableNumVal(name), _base(&base), _offset(new NumVal(0)) {
	_ownsbase = false;
	_ownsoffset = true;
	_link();
}

ModVal::ModVal(NumVal& base, const double& val) : OverrideableNumVal(val), _base(&base), _offset(new NumVal(0)) {
	_ownsbase = false;
	_ownsoffset = true;
	_link();
}

ModVal::ModVal(const string& name, NumVal& base, const double& val) : OverrideableNumVal(name, val), _base(&base), _offset(new NumVal(0)) {
	_ownsbase = false;
	_ownsoffset = true;
	_link();
}

ModVal::ModVal(NumVal& base, NumVal& offset) : OverrideableNumVal(), _base(&base), _offset(&offset) {
	_ownsbase = false;
	_ownsoffset = false;
	_link();
}

ModVal::ModVal(const string& name, NumVal& base, NumVal& offset) : OverrideableNumVal(name), _base(&base), _offset(&offset) {
	_ownsbase = false;
	_ownsoffset = false;
	_link();
}

ModVal::ModVal(NumVal& base, NumVal& offset, const double& value) : OverrideableNumVal(value), _base(&base), _offset(&offset) {
	_ownsbase = false;
	_ownsoffset = false;
	_link();
}

ModVal::ModVal(const string& name, NumVal& base, NumVal& offset, const double& value) : OverrideableNumVal(name, value), _base(&base), _offset(&offset) {
	_ownsbase = false;
	_ownsoffset = false;
	_link();
}

ModVal::ModVal(const ModVal& source) : OverrideableNumVal(source.name()), _base(new NumVal(source.base())), _offset(new NumVal(source.offset())) {
	_ownsbase = true;
	_ownsoffset = true;
	_link();
}

ModVal::~ModVal() {
	// If we created these with new, we need to delete them
	if (_ownsbase) delete _base;
	if (_ownsoffset) delete _offset;
}

string ModVal::toString() const {
	char buff[STRING_BUFFER_SIZE];
	snprintf(buff, STRING_BUFFER_SIZE, "%g%c%g%c%g%c%d", (double)(value()), ModVal::VALUE_DELIMITER, 
		(double)(base()), ModVal::VALUE_DELIMITER, (double)(offset()), ModVal::VALUE_DELIMITER, this->overridden() ? 1 : 0);
	return string(buff);
}

double ModVal::value() const {
	return ((this->overridden()) ? (this->_value) : (base() + offset()));
}

double ModVal::base() const {
	return _base->value();
}

void ModVal::base(const double& newbase) {
	if (*_base != newbase) *_base = newbase; // Should automatically call update()
}

double ModVal::offset() const {
	return _offset->value();
}

void ModVal::offset(const double& newoffset) {
	if(*_offset != newoffset) *_offset = newoffset; // Should automatically call update()
}

bool ModVal::validate(const string& newval) const {
	vector<double> values;
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

void ModVal::onupdate() {
	if (this->paused()) return;
	NumVal::onupdate(); // Call base class doupdate() function
}

ModVal& ModVal::operator=(const ModVal& rhs) {
	// To limit the number of repeated updates, we pause our base and offset
	// Then we update base and offset values
	// Then we update ourselves (and trigger our dependents)
	// Then we pause ourselves so we don't update in response to base/offset update
	// Then we unpause and manually update our base/offset
	// Then we unpause ourselves and continue as normal
	_base->pause();
	_offset->pause();

	base(rhs.base());
	offset(rhs.offset());
	((OverrideableNumVal*)(this))->value(rhs.value());
	this->set_override(rhs.overridden());

	this->update();
	this->pause();

	_base->unpause();
	_offset->unpause();
	_base->update();
	_offset->update();

	this->unpause();
	return *this;
}

ModVal& ModVal::operator=(const string& newval) {
	vector<double> values;
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
		((OverrideableNumVal*)(this))->value(values[0]);
		this->set_override(override_val);
		this->unpause();
		this->update();
	}
	return *this;
}

bool ModVal::operator==(const ModVal& rhs) const {
	return value() == rhs.value();
}
