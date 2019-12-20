#include "p2em_core.hpp"
#include <errno.h>
#include <limits>

using namespace p2em_core;

MaxVal::MaxVal() : AggVal() {  }
MaxVal::MaxVal(const string& name) : AggVal(name) {  }
MaxVal::MaxVal(NumVal& startingval) : AggVal(startingval) {  }
MaxVal::MaxVal(const string& name, NumVal& startingval) : AggVal(name, startingval) {  }
MaxVal::MaxVal(const double& baseval) : AggVal(baseval) {  }
MaxVal::MaxVal(const string& name, const double& baseval) : AggVal(name, baseval) {  }
MaxVal::MaxVal(NumVal& startingval, const double& baseval) : AggVal(startingval, baseval) {  }
MaxVal::MaxVal(const string& name, NumVal& startingval, const double& baseval) : AggVal(name, startingval, baseval) {  }
MaxVal::MaxVal(const vector<NumVal*>& startingvals) : AggVal(startingvals) {  }
MaxVal::MaxVal(const string& name, const vector<NumVal*>& startingvals) : AggVal(name, startingvals) {  }
MaxVal::MaxVal(const vector<NumVal*>& startingvals, const double& baseval) : AggVal(startingvals, baseval) {  }
MaxVal::MaxVal(const string& name, const vector<NumVal*>& startingvals, const double& baseval) : AggVal(name, startingvals, baseval) {  }

double MaxVal::value() const {
	if (this->overridden()) return this->_value;
	double val = std::numeric_limits<double>::min();
	vector<const NumVal*> vals = this->items();
	for (typename vector<const NumVal*>::iterator it = vals.begin(); it != vals.end(); it++) {
		if ((*it)->value() > val) val = (*it)->value();
	}
	return val;
}
