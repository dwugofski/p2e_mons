#include "p2em_core.h"
#include <errno.h>

#define STRING_BUFFER_SIZE 100

using namespace p2em_core;

template<typename T_num_t>
AggVal<T_num_t>::AggVal() : OverrideableNumVal<T_num_t>() {
	_values = vector<NumVal<T_num_t>*>();
}

template<typename T_num_t>
AggVal<T_num_t>::AggVal(const string& name) : OverrideableNumVal<T_num_t>(name) {
	_values = vector<NumVal<T_num_t>*>();
}

template<typename T_num_t>
AggVal<T_num_t>::AggVal(NumVal<T_num_t>& startingval) : OverrideableNumVal<T_num_t>() {
	_values = vector<NumVal<T_num_t>*>(startingval);
	startingval.addDependency(*this);
}

template<typename T_num_t>
AggVal<T_num_t>::AggVal(const string& name, NumVal<T_num_t>& startingval) : OverrideableNumVal<T_num_t>(name) {
	_values = vector<NumVal<T_num_t>*>(startingval);
	startingval.addDependency(*this);
}

template<typename T_num_t>
AggVal<T_num_t>::AggVal(const T_num_t& baseval) : OverrideableNumVal<T_num_t>(baseval) {
	_values = vector<NumVal<T_num_t>*>();
}

template<typename T_num_t>
AggVal<T_num_t>::AggVal(const string& name, const T_num_t& baseval) : OverrideableNumVal<T_num_t>(name, baseval) {
	_values = vector<NumVal<T_num_t>*>();
}

template<typename T_num_t>
AggVal<T_num_t>::AggVal(NumVal<T_num_t>& startingval, const T_num_t& baseval) : OverrideableNumVal<T_num_t>(baseval) {
	_values = vector<NumVal<T_num_t>*>(startingval);
	startingval.addDependency(*this);
}

template<typename T_num_t>
AggVal<T_num_t>::AggVal(const string& name, NumVal<T_num_t>& startingval, const T_num_t& baseval) : OverrideableNumVal<T_num_t>(name, baseval) {
	_values = vector<NumVal<T_num_t>*>(startingval);
	startingval.addDependency(*this);
}

template<typename T_num_t>
AggVal<T_num_t>::AggVal(const vector<NumVal<T_num_t>*>& startingvals) : OverrideableNumVal<T_num_t>() {
	_values = vector<NumVal<T_num_t>*>(startingvals);
	for (typename vector<NumVal<T_num_t>>::iterator it = _values.begin(); it != _values.end(); it++) (*it).addDependency(*this);
}

template<typename T_num_t>
AggVal<T_num_t>::AggVal(const string& name, const vector<NumVal<T_num_t>*>& startingvals) : OverrideableNumVal<T_num_t>(name) {
	_values = vector<NumVal<T_num_t>*>(startingvals);
	for (typename vector<NumVal<T_num_t>>::iterator it = _values.begin(); it != _values.end(); it++) (*it).addDependency(*this);
}

template<typename T_num_t>
AggVal<T_num_t>::AggVal(const vector<NumVal<T_num_t>*>& startingvals, const T_num_t& baseval) : OverrideableNumVal<T_num_t>(baseval) {
	_values = vector<NumVal<T_num_t>*>(startingvals);
	for (typename vector<NumVal<T_num_t>>::iterator it = _values.begin(); it != _values.end(); it++) (*it).addDependency(*this);
}

template<typename T_num_t>
AggVal<T_num_t>::AggVal(const string& name, const vector<NumVal<T_num_t>*>& startingvals, const T_num_t& baseval) : OverrideableNumVal<T_num_t>(name, baseval) {
	_values = vector<NumVal<T_num_t>*>(startingvals);
	for (typename vector<NumVal<T_num_t>>::iterator it = _values.begin(); it != _values.end(); it++) (*it).addDependency(*this);
}

template<typename T_num_t>
vector<const NumVal<T_num_t>*> AggVal<T_num_t>::items() const {
	vector<const NumVal<T_num_t>*> ret = vector<const NumVal<T_num_t>*>();
	for (typename vector<NumVal<T_num_t>*>::iterator it = _values.begin(); it != _values.end(); it++) {
		ret.push_back(*it);
	}
	return ret;
}

template<typename T_num_t>
vector<NumVal<T_num_t>*> AggVal<T_num_t>::items() {
	vector<const NumVal<T_num_t>*> vals = static_cast<const AggVal<T_num_t>*>(this)->items();
	vector<NumVal<T_num_t>*> ret = vector<NumVal<T_num_t>*>();
	for (typename vector<NumVal<T_num_t>*>::iterator it = vals.begin(); it != vals.end(); it++) {
		ret.push_back(const_cast<NumVal<T_num_t>*>(*it));
	}
	return ret;
}

template<typename T_num_t>
bool AggVal<T_num_t>::hasItem(const NumVal<T_num_t>& item) const {
	vector<const NumVal<T_num_t>*> vals = items();
	bool found = false;
	for (typename vector<const NumVal<T_num_t>*>::iterator it = vals.begin(); it != vals.end(); it++) {
		if ((*it) == &item) {
			found = true;
			break;
		}
	}
	return found;
}

template<typename T_num_t>
void AggVal<T_num_t>::addItem(NumVal<T_num_t>& newitem) {
	if (hasItem(newitem)) return;
	_values.push_back(newitem);
	newitem.addDependency(*this);
	this->update();
}

template<typename T_num_t>
void AggVal<T_num_t>::removeItem(const NumVal<T_num_t>& olditem) {
	if (!hasItem(olditem)) return;
	for (typename vector<NumVal<T_num_t>*>::iterator it = _values.begin(); it != _values.end(); it++) {
		if ((*it) == &olditem) {
			_values.erase(it);
			break;
		}
	}
	olditem.removeDependency(*this);
	this->update();
}