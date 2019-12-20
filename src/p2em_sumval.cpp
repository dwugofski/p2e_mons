#include "p2em_core.hpp"
#include <errno.h>

using namespace p2em_core;

 SumVal::SumVal() : AggVal() {  }
 SumVal::SumVal(const string& name) : AggVal(name) {  }
 SumVal::SumVal(NumVal& startingval) : AggVal(startingval) {  }
 SumVal::SumVal(const string& name, NumVal& startingval) : AggVal(name, startingval) {  }
 SumVal::SumVal(const double& baseval) : AggVal(baseval) {  }
 SumVal::SumVal(const string& name, const double& baseval) : AggVal(name, baseval) {  }
 SumVal::SumVal(NumVal& startingval, const double& baseval) : AggVal(startingval, baseval) {  }
 SumVal::SumVal(const string& name, NumVal& startingval, const double& baseval) : AggVal(name, startingval, baseval) {  }
 SumVal::SumVal(const vector<NumVal*>& startingvals) : AggVal(startingvals) {  }
 SumVal::SumVal(const string& name, const vector<NumVal*>& startingvals) : AggVal(name, startingvals) {  }
 SumVal::SumVal(const vector<NumVal*>& startingvals, const double& baseval) : AggVal(startingvals, baseval) {  }
 SumVal::SumVal(const string& name, const vector<NumVal*>& startingvals, const double& baseval) : AggVal(name, startingvals, baseval) {  }

double SumVal::value() const {
	if (this->overridden()) return this->_value;
	double val = 0;
	vector<const NumVal*> vals = this->items();
	for (typename vector<const NumVal*>::iterator it = vals.begin(); it != vals.end(); it++) {
		val += (*it)->value();
	}
	return val;
}