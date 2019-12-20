#pragma once
#ifndef __P2EM_TRANSVAL_HPP__
#define __P2EM_TRANSVAL_HPP__

#include "p2em_core.h"
#include "../src/p2em_numval.hpp"

using namespace p2em_core;

template<typename T_num_t, typename T_src_t>
TransVal<T_num_t, T_src_t>::TransVal() : NumVal<T_num_t>() {
	_source = *(new NumVal<T_src_t>());
	_ownsref = true;
	_source.addDependency(*this);
}

template<typename T_num_t, typename T_src_t>
TransVal<T_num_t, T_src_t>::TransVal(const string& name) : NumVal<T_num_t>(name) {
	_source = *(new NumVal<T_src_t>());
	_ownsref = true;
	_source.addDependency(*this);
}

template<typename T_num_t, typename T_src_t>
TransVal<T_num_t, T_src_t>::TransVal(NumVal<T_src_t>& source) : NumVal<T_num_t>() {
	_source = source;
	_ownsref = false;
	_source.addDependency(*this);
}

template<typename T_num_t, typename T_src_t>
TransVal<T_num_t, T_src_t>::TransVal(const string& name, NumVal<T_src_t>& source) : NumVal<T_num_t>(name) {
	_source = source;
	_ownsref = false;
	_source.addDependency(*this);
}

template<typename T_num_t, typename T_src_t>
TransVal<T_num_t, T_src_t>::~TransVal() {
	if (_ownsref) {
		delete& _source;
		_ownsref = false;
	}
}

template<typename T_num_t, typename T_src_t>
T_num_t TransVal<T_num_t, T_src_t>::value() const {
	return (T_num_t)getSrc().value();
}

template<typename T_num_t, typename T_src_t>
void TransVal<T_num_t, T_src_t>::value(const T_num_t& value) {
	getSrc().value((T_src_t)value);
}

template<typename T_num_t, typename T_src_t>
void TransVal<T_num_t, T_src_t>::value(const T_src_t& value) {
	getSrc().value(value);
}

template<typename T_num_t, typename T_src_t>
void TransVal<T_num_t, T_src_t>::setSrc(NumVal<T_src_t>& newsrc) {
	NumVal<T_src_t> oldsrc = _source;
	if (&_source == &newsrc) return;
	_source.removeDependency(*this);
	_source = newsrc;
	newsrc.addDependency(*this);
	if (_ownsref) {
		delete& _source;
		_ownsref = false;
	}
}

template<typename T_num_t, typename T_src_t>
const NumVal<T_src_t>& TransVal<T_num_t, T_src_t>::getSrc() const {
	return _source;
}

template<typename T_num_t, typename T_src_t>
NumVal<T_src_t>& TransVal<T_num_t, T_src_t>::getSrc() {
	return const_cast<NumVal<T_num_t>&>(static_cast<const TransVal<T_num_t, T_src_t>*>(this)->getSrc());
}

template<typename T_num_t, typename T_src_t>
void TransVal<T_num_t, T_src_t>::onupdate() {
	if (this->paused()) return;
	NumVal<T_num_t>::onupdate();
}

#endif
