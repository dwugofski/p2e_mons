#include "p2em_core.hpp"

using namespace p2em_core;

Updateable::Updateable() {
	vector<Updateable*> _depedencies = vector<Updateable*>();
	vector<Updateable*> _parents = vector<Updateable*>();
	vector<std::tuple<Updateable*, void (*)(Updateable*)>> _callbacks;
}

Updateable::Updateable(const Updateable& source) {
	vector<Updateable*> _depedencies = vector<Updateable*>();
	vector<Updateable*> _parents = vector<Updateable*>();
	vector<std::tuple<Updateable*, void (*)(Updateable*)>> _callbacks = vector<std::tuple<Updateable*, void (*)(Updateable*)>>();

	for (vector<Updateable*>::const_iterator it = source.parents().begin(); it != source.parents().end(); it++)
		addParent(*(*it));
	for (vector<std::tuple<Updateable*, void (*)(Updateable*)>>::const_iterator it = source.callbacks().begin(); it != source.callbacks().end(); it++)
		addCallback(std::get<0>(*it), std::get<1>(*it));
}

Updateable::~Updateable() {
	vector<Updateable*>::iterator it = _parents.begin();
	Updateable* oldptr;

	while (it != _parents.end()) {
		oldptr = *it;
		it = _parents.erase(it);

		oldptr->removeDependency(*this);
	}

	it = _dependencies.begin();
	while (it != _dependencies.end()) {
		oldptr = *it;
		it = _dependencies.erase(it);
		
		oldptr->removeParent(*this);
	}
}

vector<Updateable*> Updateable::dependents() const {
	return vector<Updateable*>(_dependencies);
}

vector<Updateable*> Updateable::parents() const {
	return vector<Updateable*>(_parents);
}

vector<std::tuple<Updateable*, void (*)(Updateable*)>> Updateable::callbacks() const {
	return vector<std::tuple<Updateable*, void (*)(Updateable*)>>(_callbacks);
}

bool Updateable::hasDependency(const Updateable& dep, bool recurse) const {
	for (vector<Updateable*>::const_iterator it = _dependencies.begin(); it != _dependencies.end(); it++) {
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
		throw new Exception(ExceptionCode:: CIRCULAR_DEPS);
	}

	_dependencies.push_back(&dep);
	dep.addParent(*this);
}

void Updateable::removeDependency(Updateable& dep) {
	vector<Updateable*>::iterator it, ot;
	if (!hasDependency(dep, false)) return;

	it = _dependencies.begin();
	while (it != _dependencies.end()) {
		ot = it++;
		if ((*ot) != &dep) continue;
		it = _dependencies.erase(ot);
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
		it = _parents.erase(ot);
	}
	parent.removeDependency(*this);
}

bool Updateable::hasCallback(const Updateable* src, void (*callback)(Updateable*)) const {
	for (vector<std::tuple<Updateable*, void (*)(Updateable*)>>::const_iterator it = _callbacks.begin(); it != _callbacks.end(); it++)
		if ((callback == std::get<1>(*it)) && (src == std::get<0>(*it))) return true;
	return false;
}

void Updateable::addCallback(Updateable* src, void (*callback)(Updateable*)) {
	if (hasCallback(src, callback)) return;

	_callbacks.push_back(std::make_tuple(src, callback));
}

void Updateable::removeCallback(const Updateable* src, void (*callback)(Updateable*)) {
	vector<std::tuple<Updateable*, void (*)(Updateable*)>>::iterator it, ot;
	if (!hasCallback(src, callback)) return;

	it = _callbacks.begin();
	while (it != _callbacks.end()) {
		ot = it++;
		if ((callback != std::get<1>(*ot)) || (src != std::get<0>(*ot))) continue;
		it = _callbacks.erase(ot);
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
		for (vector<std::tuple<Updateable*, void (*)(Updateable*)>>::iterator it = _callbacks.begin(); it != _callbacks.end(); it++)
			(std::get<1>(*it))(std::get<0>(*it));	// Can we just take a second to appreciate the mess that is this line? 
													// we're calling the function -- specified by get<1>(*it) -- with the pointer argument -- specified by get<0>(*it)
	}
	if (updatedeps) {
		for (vector<Updateable*>::iterator jt = _dependencies.begin(); jt != _dependencies.end(); jt++)
			(*jt)->update();
	}
	if (internalupdate) onupdate();
}

void Updateable::onupdate() {  }