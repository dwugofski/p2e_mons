#include "p2em_core.hpp"
#include <errno.h>
#include <limits>

using namespace p2em_core;

MinVal::MinVal() : AggVal() {  }
MinVal::MinVal(const string& name) : AggVal(name) {  }
MinVal::MinVal(NumVal& startingval) : AggVal(startingval) {  }
MinVal::MinVal(const string& name, NumVal& startingval) : AggVal(name, startingval) {  }
MinVal::MinVal(const double& baseval) : AggVal(baseval) {  }
MinVal::MinVal(const string& name, const double& baseval) : AggVal(name, baseval) {  }
MinVal::MinVal(NumVal& startingval, const double& baseval) : AggVal(startingval, baseval) {  }
MinVal::MinVal(const string& name, NumVal& startingval, const double& baseval) : AggVal(name, startingval, baseval) {  }
MinVal::MinVal(const vector<NumVal*>& startingvals) : AggVal(startingvals) {  }
MinVal::MinVal(const string& name, const vector<NumVal*>& startingvals) : AggVal(name, startingvals) {  }
MinVal::MinVal(const vector<NumVal*>& startingvals, const double& baseval) : AggVal(startingvals, baseval) {  }
MinVal::MinVal(const string& name, const vector<NumVal*>& startingvals, const double& baseval) : AggVal(name, startingvals, baseval) {  }

double MinVal::value() const {
	if (this->overridden()) return this->_value;
	double val = std::numeric_limits<double>::max();
	vector<const NumVal*> vals = this->items();
	for (typename vector<const NumVal*>::iterator it = vals.begin(); it != vals.end(); it++) {
		if ((*it)->value() < val) val = (*it)->value();
	}
	return val;
}
