#include "p2em_core.hpp"
#include <errno.h>

using namespace p2em_core;

AggVal::AggVal() : OverrideableNumVal() {
	_values = vector<NumVal*>();
}

AggVal::AggVal(const string& name) : OverrideableNumVal(name) {
	_values = vector<NumVal*>();
}

AggVal::AggVal(NumVal& startingval) : OverrideableNumVal() {
	_values = vector<NumVal*>();
	_values.push_back(&startingval);
	startingval.addDependency(*this);
}

AggVal::AggVal(const string& name, NumVal& startingval) : OverrideableNumVal(name) {
	_values = vector<NumVal*>();
	_values.push_back(&startingval);
	startingval.addDependency(*this);
}

AggVal::AggVal(const double& baseval) : OverrideableNumVal(baseval) {
	_values = vector<NumVal*>();
}

AggVal::AggVal(const string& name, const double& baseval) : OverrideableNumVal(name, baseval) {
	_values = vector<NumVal*>();
}

AggVal::AggVal(NumVal& startingval, const double& baseval) : OverrideableNumVal(baseval) {
	_values = vector<NumVal*>();
	_values.push_back(&startingval);
	startingval.addDependency(*this);
}

AggVal::AggVal(const string& name, NumVal& startingval, const double& baseval) : OverrideableNumVal(name, baseval) {
	_values = vector<NumVal*>();
	_values.push_back(&startingval);
	startingval.addDependency(*this);
}

AggVal::AggVal(const vector<NumVal*>& startingvals) : OverrideableNumVal() {
	_values = vector<NumVal*>(startingvals);
	for (vector<NumVal*>::iterator it = _values.begin(); it != _values.end(); it++) (*it)->addDependency(*this);
}

AggVal::AggVal(const string& name, const vector<NumVal*>& startingvals) : OverrideableNumVal(name) {
	_values = vector<NumVal*>(startingvals);
	for (vector<NumVal*>::iterator it = _values.begin(); it != _values.end(); it++) (*it)->addDependency(*this);
}

AggVal::AggVal(const vector<NumVal*>& startingvals, const double& baseval) : OverrideableNumVal(baseval) {
	_values = vector<NumVal*>(startingvals);
	for (vector<NumVal*>::iterator it = _values.begin(); it != _values.end(); it++) (*it)->addDependency(*this);
}

AggVal::AggVal(const string& name, const vector<NumVal*>& startingvals, const double& baseval) : OverrideableNumVal(name, baseval) {
	_values = vector<NumVal*>(startingvals);
	for (vector<NumVal*>::iterator it = _values.begin(); it != _values.end(); it++) (*it)->addDependency(*this);
}

vector<const NumVal*> AggVal::items() const {
	vector<const NumVal*> ret = vector<const NumVal*>();
	for (typename vector<NumVal*>::const_iterator it = _values.begin(); it != _values.end(); it++) {
		ret.push_back(*it);
	}
	return ret;
}

vector<NumVal*> AggVal::items() {
	vector<const NumVal*> vals = static_cast<const AggVal*>(this)->items();
	vector<NumVal*> ret = vector<NumVal*>();
	for (typename vector<const NumVal*>::iterator it = vals.begin(); it != vals.end(); it++) {
		ret.push_back(const_cast<NumVal*>(*it));
	}
	return ret;
}

bool AggVal::hasItem(const NumVal& item) const {
	vector<const NumVal*> vals = items();
	bool found = false;
	for (typename vector<const NumVal*>::const_iterator it = vals.begin(); it != vals.end(); it++) {
		if ((*it) == &item) {
			found = true;
			break;
		}
	}
	return found;
}

void AggVal::addItem(NumVal& newitem) {
	if (hasItem(newitem)) return;
	_values.push_back(&newitem);
	newitem.addDependency(*this);
	this->update();
}

void AggVal::removeItem(NumVal& olditem) {
	if (!hasItem(olditem)) return;
	vector<NumVal*>::iterator it = _values.begin();
	while (it != _values.end()) {
		if ((*it) == &olditem) {
			it = _values.erase(it);
			break;
		}
		it++;
	}
	olditem.removeDependency(*this);
	this->update();
}

void AggVal::removeParent(Updateable& oldparent) {
	Updateable::removeParent(oldparent);
	removeItem(static_cast<NumVal&>(oldparent));
}