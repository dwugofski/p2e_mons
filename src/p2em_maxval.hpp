#pragma once
#ifndef __P2EM_MAXVAL_HPP__
#define __P2EM_MAXVAL_HPP__

#include "p2em_core.h"
#include <errno.h>
#include <limits>
#include "../src/p2em_numval.hpp"

using namespace p2em_core;

template<typename T_num_t> MaxVal<T_num_t>::MaxVal() : AggVal<T_num_t>() {  }
template<typename T_num_t> MaxVal<T_num_t>::MaxVal(const string& name) : AggVal<T_num_t>(name) {  }
template<typename T_num_t> MaxVal<T_num_t>::MaxVal(NumVal<T_num_t>& startingval) : AggVal<T_num_t>(startingval) {  }
template<typename T_num_t> MaxVal<T_num_t>::MaxVal(const string& name, NumVal<T_num_t>& startingval) : AggVal<T_num_t>(name, startingval) {  }
template<typename T_num_t> MaxVal<T_num_t>::MaxVal(const T_num_t& baseval) : AggVal<T_num_t>(baseval) {  }
template<typename T_num_t> MaxVal<T_num_t>::MaxVal(const string& name, const T_num_t& baseval) : AggVal<T_num_t>(name, baseval) {  }
template<typename T_num_t> MaxVal<T_num_t>::MaxVal(NumVal<T_num_t>& startingval, const T_num_t& baseval) : AggVal<T_num_t>(startingval, baseval) {  }
template<typename T_num_t> MaxVal<T_num_t>::MaxVal(const string& name, NumVal<T_num_t>& startingval, const T_num_t& baseval) : AggVal<T_num_t>(name, startingval, baseval) {  }
template<typename T_num_t> MaxVal<T_num_t>::MaxVal(const vector<NumVal<T_num_t>&>& startingvals) : AggVal<T_num_t>(startingvals) {  }
template<typename T_num_t> MaxVal<T_num_t>::MaxVal(const string& name, const vector<NumVal<T_num_t>&>& startingvals) : AggVal<T_num_t>(name, startingvals) {  }
template<typename T_num_t> MaxVal<T_num_t>::MaxVal(const vector<NumVal<T_num_t>&>& startingvals, const T_num_t& baseval) : AggVal<T_num_t>(startingvals, baseval) {  }
template<typename T_num_t> MaxVal<T_num_t>::MaxVal(const string& name, const vector<NumVal<T_num_t>&>& startingvals, const T_num_t& baseval) : AggVal<T_num_t>(name, startingvals, baseval) {  }

template<typename T_num_t>
T_num_t MaxVal<T_num_t>::value() const {
	if (this->overridden()) return this->_value;
	T_num_t val = std::numeric_limits<T_num_t>::min();
	vector<const NumVal<T_num_t>*> vals = this->items();
	for (typename vector<const NumVal<T_num_t>*>::iterator it = vals.begin(); it != vals.end(); it++) {
		if ((*it)->value() > val) val = (*it)->value();
	}
	return val;
}

template<typename T_num_t>
void MaxVal<T_num_t>::onupdate() {
	if (this->paused()) return;
	OverrideableNumVal<T_num_t>::onupdate();
}

#endif
