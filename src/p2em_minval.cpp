#include "p2em_core.h"
#include <errno.h>
#include <limits>

#define STRING_BUFFER_SIZE 100

using namespace p2em_core;

template<typename T_num_t> MinVal<T_num_t>::MinVal() : AggVal<T_num_t>() {  }
template<typename T_num_t> MinVal<T_num_t>::MinVal(const string& name) : AggVal<T_num_t>(name) {  }
template<typename T_num_t> MinVal<T_num_t>::MinVal(NumVal<T_num_t>& startingval) : AggVal<T_num_t>(startingval) {  }
template<typename T_num_t> MinVal<T_num_t>::MinVal(const string& name, NumVal<T_num_t>& startingval) : AggVal<T_num_t>(name, startingval) {  }
template<typename T_num_t> MinVal<T_num_t>::MinVal(const T_num_t& baseval) : AggVal<T_num_t>(baseval) {  }
template<typename T_num_t> MinVal<T_num_t>::MinVal(const string& name, const T_num_t& baseval) : AggVal<T_num_t>(name, baseval) {  }
template<typename T_num_t> MinVal<T_num_t>::MinVal(NumVal<T_num_t>& startingval, const T_num_t& baseval) : AggVal<T_num_t>(startingval, baseval) {  }
template<typename T_num_t> MinVal<T_num_t>::MinVal(const string& name, NumVal<T_num_t>& startingval, const T_num_t& baseval) : AggVal<T_num_t>(name, startingval, baseval) {  }
template<typename T_num_t> MinVal<T_num_t>::MinVal(const vector<NumVal<T_num_t>&>& startingvals) : AggVal<T_num_t>(startingvals) {  }
template<typename T_num_t> MinVal<T_num_t>::MinVal(const string& name, const vector<NumVal<T_num_t>&>& startingvals) : AggVal<T_num_t>(name, startingvals) {  }
template<typename T_num_t> MinVal<T_num_t>::MinVal(const vector<NumVal<T_num_t>&>& startingvals, const T_num_t& baseval) : AggVal<T_num_t>(startingvals, baseval) {  }
template<typename T_num_t> MinVal<T_num_t>::MinVal(const string& name, const vector<NumVal<T_num_t>&>& startingvals, const T_num_t& baseval) : AggVal<T_num_t>(name, startingvals, baseval) {  }

template<typename T_num_t>
AttrType MinVal<T_num_t>::attrType() const {
	return P2E_ATTR_MINVAL;
}

template<typename T_num_t>
T_num_t MinVal<T_num_t>::value() const {
	if (this->overridden()) return this->_value;
	T_num_t val = std::numeric_limits<T_num_t>::max();
	vector<const NumVal<T_num_t>*> vals = this->items();
	for (typename vector<const NumVal<T_num_t>*>::iterator it = vals.begin(); it != vals.end(); it++) {
		if ((*it)->value() < val) val = (*it)->value();
	}
	return val;
}

template<typename T_num_t>
void MinVal<T_num_t>::onupdate() {
	if (this->paused()) return;
	OverrideableNumVal<T_num_t>::doupdate();
}