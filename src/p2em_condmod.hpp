#pragma once
#ifndef __P2EM_CONDMOD_HPP__
#define __P2EM_CONDMOD_HPP__

#include "p2em_core.h"
#include "../src/p2em_modval.hpp"
#include "../src/p2em_sumval.hpp"

using namespace p2em_core;

// ----------------------------------------
// CondModOpt -----------------------------
// ----------------------------------------

template<typename T_num_t>
CondModOpt<T_num_t>::CondModOpt(const T_num_t& val, const string& desc, const bool& sel) : NumVal<T_num_t>(desc, val) {
	description = desc;
	selected = sel;
}
template<typename T_num_t> CondModOpt<T_num_t>::CondModOpt() : CondModOpt(0, "", false) {  }
template<typename T_num_t> CondModOpt<T_num_t>::CondModOpt(const T_num_t& val) : CondModOpt(val, "", false) {  }
template<typename T_num_t> CondModOpt<T_num_t>::CondModOpt(const string& desc) : CondModOpt(0, desc, false) {  }
template<typename T_num_t> CondModOpt<T_num_t>::CondModOpt(const T_num_t& val, const string& desc) : CondModOpt(val, desc, false) {  }
template<typename T_num_t> CondModOpt<T_num_t>::CondModOpt(const bool& sel) : CondModOpt(0, "", sel) {  }
template<typename T_num_t> CondModOpt<T_num_t>::CondModOpt(const T_num_t& val, const bool& sel) : CondModOpt(val , "", sel) {  }
template<typename T_num_t> CondModOpt<T_num_t>::CondModOpt(const string& desc, const bool& sel) : CondModOpt(0, desc, sel) {  }
template<typename T_num_t> CondModOpt<T_num_t>::CondModOpt(const CondModOpt<T_num_t>& source) : CondModOpt(source.value(), source.description, source.selected) {  }

template<typename T_num_t>
CondModOpt<T_num_t>& CondModOpt<T_num_t>::operator=(const string& rhs) {
	description = rhs;
	return *this;
}

template<typename T_num_t>
CondModOpt<T_num_t>& CondModOpt<T_num_t>::operator=(const bool& rhs) {
	selected = rhs;
	return *this;
}

template<typename T_num_t>
CondModOpt<T_num_t>& CondModOpt<T_num_t>::operator=(const CondModOpt<T_num_t>& rhs) {
	this->_value = rhs.value();
	description = rhs.description;
	selected = rhs.selected;
	return *this;
}

template<typename T_num_t> bool CondModOpt<T_num_t>::operator==(const string& rhs) const { return description == rhs; }
template<typename T_num_t> bool CondModOpt<T_num_t>::operator==(const bool& rhs) const { return selected == rhs; }
template<typename T_num_t> bool CondModOpt<T_num_t>::operator==(const CondModOpt<T_num_t>& rhs) const { 
	return ((this->_value == rhs.value()) && (description == rhs.description));
}

template<typename T_num_t> CondModOpt<T_num_t>::operator string() const { return description; }
template<typename T_num_t> CondModOpt<T_num_t>::operator bool() const { return selected; }

// ----------------------------------------
// CondMod --------------------------------
// ----------------------------------------

template<typename T_num_t>
void CondMod<T_num_t>::_init() {
	_options = vector<CondModOpt<T_num_t>*>();
	_ownership = vector<bool>();
	this->removeParent(this->_offset);
	delete &(this->_offset);
	this->_ownsoffset = false;
	_agger = SumVal<T_num_t>();
	this->_offset = _agger;
	this->addParent(_agger);
}

template<typename T_num_t> CondMod<T_num_t>::CondMod() : ModVal<T_num_t>() { _init(); }
template<typename T_num_t> CondMod<T_num_t>::CondMod(const string& name) : ModVal<T_num_t>(name) { _init(); }
template<typename T_num_t> CondMod<T_num_t>::CondMod(NumVal<T_num_t>& base) : ModVal<T_num_t>(base) { _init(); }
template<typename T_num_t> CondMod<T_num_t>::CondMod(const string& name, NumVal<T_num_t>& base) : ModVal<T_num_t>(name, base) { _init(); }
template<typename T_num_t> CondMod<T_num_t>::CondMod(const T_num_t& val) : ModVal<T_num_t>(val) { _init(); }
template<typename T_num_t> CondMod<T_num_t>::CondMod(const string& name, const T_num_t& val) : ModVal<T_num_t>(name, val) { _init(); }
template<typename T_num_t> CondMod<T_num_t>::CondMod(NumVal<T_num_t>& base, const T_num_t& val) : ModVal<T_num_t>(base, val) { _init(); }
template<typename T_num_t> CondMod<T_num_t>::CondMod(const string& name, NumVal<T_num_t>& base, const T_num_t& val) : ModVal<T_num_t>(name, base, val) { _init(); }

template<typename T_num_t>
CondMod<T_num_t>::CondMod(const CondMod<T_num_t>& source) : ModVal<T_num_t>(source) {
	_init();
	this->pause();
	vector<const CondMod<T_num_t>*> srcopts = source.options();
	const CondModOpt<T_num_t>* newopt;
	for (int i = 0; i < srcopts.size(); i+=1) {
		newopt = srcopts[i];
		createOption(*newopt);
	}
	this->unpause();
}

template<typename T_num_t>
CondMod<T_num_t>::~CondMod() {
	this->pause();
	for (uint i = size(); i > 0; i -= 1) {
		removeOption(i - 1);
	}
}

template<typename T_num_t>
bool CondMod<T_num_t>::hasOption(const CondModOpt<T_num_t>& testopt) const {
	for (typename vector<CondModOpt<T_num_t>*>::const_iterator it = _options.begin(); it != _options.end(); it++) {
		if ((*it) == &testopt) return true;
	}
	return false;
}

template<typename T_num_t>
int CondMod<T_num_t>::addOption(CondModOpt<T_num_t>& newopt, bool own) {
	if (hasOption(newopt)) return indexOf(newopt);
	_options.push_back(&newopt);
	_ownership.push_back(own);
	_agger.addItem(newopt);
	return indexOf(newopt);
}

template<typename T_num_t>
int CondMod<T_num_t>::createOption(T_num_t val, string description, bool sel) {
	CondModOpt<T_num_t>* newopt = new CondModOpt<T_num_t>(val, description, sel);
	return addOption(*newopt, true);
}

template<typename T_num_t>
int CondMod<T_num_t>::createOption(const CondModOpt<T_num_t>& source) {
	CondModOpt<T_num_t>* newopt = new CondModOpt<T_num_t>(source);
	return addOption(*newopt, true);
}

template<typename T_num_t>
void CondMod<T_num_t>::removeOption(const int& index) {
	if (index >= (int)(size())) return;
	typename vector<CondModOpt<T_num_t>*>::iterator optit = _options.begin();
	typename vector<bool>::iterator ownit = _ownership.begin();
	for (int i = 0; i < index; i++) {
		optit++;
		ownit++;
	}
	CondModOpt<T_num_t>* oldval = *optit;
	bool owned = *ownit;
	_agger.removeItem(*oldval);
	_options.erase(optit);
	_ownership.erase(ownit);
	if (owned) delete oldval;
}

template<typename T_num_t>
void CondMod<T_num_t>::removeOption(const CondModOpt<T_num_t>& oldopt) {
	removeOption(indexOf(oldopt));
}

template<typename T_num_t>
vector<const CondModOpt<T_num_t>*> CondMod<T_num_t>::options() const {
	vector<const CondModOpt<T_num_t>*> ret = vector<const CondModOpt<T_num_t>*>();
	for (typename vector<CondModOpt<T_num_t>*>::const_iterator it = _options.begin(); it != _options.end(); it++)
		ret.push_back(*it);
	return ret;
}

template<typename T_num_t>
vector<CondModOpt<T_num_t>*> CondMod<T_num_t>::options() {
	return vector<CondModOpt<T_num_t>*>(_options);
}

template<typename T_num_t>
int CondMod<T_num_t>::indexOf(const CondModOpt<T_num_t>& testval) const {
	bool found = false;
	int i = 0;
	for (typename vector<CondModOpt<T_num_t>*>::const_iterator it = _options.begin(); it != _options.end(); it++) {
		if (*it == &testval) {
			found = true;
			break;
		}
		i += 1;
	}
	return (found) ? i : -1;
}

template<typename T_num_t>
uint CondMod<T_num_t>::size() const {
	return (uint)(_options.size());
}

template<typename T_num_t>
const CondModOpt<T_num_t>& CondMod<T_num_t>::operator[](const int& index) const {
	vector<const CondModOpt<T_num_t>*> opts = options();
	return *(opts[index]);
}

template<typename T_num_t>
CondModOpt<T_num_t>& CondMod<T_num_t>::operator[](const int& index) {
	return const_cast<CondModOpt<T_num_t>&>(static_cast<const CondMod<T_num_t>*>(this)->operator[](index));
}

#endif
