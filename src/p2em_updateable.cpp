
#include "p2em_core.h"

using namespace p2em_core;

Updateable::Updateable() {
	vector<Updateable*> _depedencies = vector<Updateable*>();
	vector<Updateable*> _parents = vector<Updateable*>();
	vector<void (*)(Updateable*)> _callbacks;
}

Updateable::~Updateable() {
	vector<Updateable*>::iterator ot;
	vector<Updateable*>::iterator it = _parents.begin();

	while (it != _parents.end()) {
		ot = it++;
		_parents.erase(ot);

		(*ot)->removeDependency(*this);
	}

	it = _depedencies.begin();
	while (it != _depedencies.end()) {
		ot = it++;
		_depedencies.erase(ot);
		
		(*ot)->removeParent(*this);
	}
}

bool Updateable::hasDependency(const Updateable& dep, bool recurse) const {
	for (vector<Updateable*>::const_iterator it = _depedencies.begin(); it != _depedencies.end(); it++) {
		if (&(dep) == (*it)) return true;
		if (recurse) {
			if (((*it))->hasDependency(dep, true)) return true;
		}
	}
	return false;
}

void Updateable::addDependency(Updateable& dep) {
	if (hasDependency(dep, true)) return;
	if (dep.hasDependency(*this, true)) {
		throw new Exception(P2EMON_ERR_CIRCULAR_DEPS);
	}

	_depedencies.push_back(&dep);
	dep.addParent(*this);
}

void Updateable::removeDependency(Updateable& dep) {
	vector<Updateable*>::iterator it, ot;
	if (!hasDependency(dep, false)) return;

	it = _depedencies.begin();
	while (it != _depedencies.end()) {
		ot = it++;
		if ((*ot) != &dep) continue;
		_depedencies.erase(ot);
	}
	dep.removeParent(*this);
}

bool Updateable::hasParent(const Updateable& parent) const {
	for (vector<Updateable*>::const_iterator it = _parents.begin(); it != _parents.end(); it++)
		if (&parent == (*it)) return true;
	return false;
}

void Updateable::addParent(Updateable& parent) {
	if (hasParent(parent)) return;

	_parents.push_back(&parent);
	parent.addDependency(*this);
}

void Updateable::removeParent(Updateable& parent) {
	vector<Updateable*>::iterator it, ot;
	if (!hasParent(parent)) return;

	it = _parents.begin();
	while (it != _parents.end()) {
		ot = it++;
		if ((*ot) != &parent) continue;
		_parents.erase(ot);
	}
	parent.removeDependency(*this);
}

bool Updateable::hasCallback(void (*callback)(Updateable*)) const {
	for (vector<void (*)(Updateable*)>::const_iterator it = _callbacks.begin(); it != _callbacks.end(); it++)
		if (callback == *it) return true;
	return false;
}

void Updateable::addCallback(void (*callback)(Updateable*)) {
	if (hasCallback(callback)) return;

	_callbacks.push_back(callback);
}

void Updateable::removeCallback(void (*callback)(Updateable*)) {
	vector<void (*)(Updateable*)>::iterator it, ot;
	if (!hasCallback(callback)) return;

	it = _callbacks.begin();
	while (it != _callbacks.end()) {
		ot = it++;
		if (*ot != callback) continue;
		_callbacks.erase(ot);
	}
}

void Updateable::pause() {
	_paused = true;
}

void Updateable::unpause() {
	_paused = false;
}

bool Updateable::paused() {
	return _paused;
}

void Updateable::update(bool updatedeps, bool triggercallbacks, bool internalupdate) {
	if (_paused) return;
	if (triggercallbacks) {
		for (vector<void (*)(Updateable*)>::iterator it = _callbacks.begin(); it != _callbacks.end(); it++)
			(*it)(this);
	}
	if (updatedeps) {
		for (vector<Updateable*>::iterator jt = _depedencies.begin(); jt != _depedencies.end(); jt++)
			(*jt)->update();
	}
	if (internalupdate) onupdate();
}

void Updateable::onupdate() {  }