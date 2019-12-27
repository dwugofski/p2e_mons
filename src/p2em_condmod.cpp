#include "p2em_core.hpp"

using namespace p2em_core;

// ----------------------------------------
// CondModOpt -----------------------------
// ----------------------------------------

CondModOpt::CondModOpt(const double& val, const string& desc, const bool& sel) : NumVal(desc, val) {
	description = desc;
	selected = sel;
}
CondModOpt::CondModOpt() : CondModOpt(0, "", false) {  }
CondModOpt::CondModOpt(const double& val) : CondModOpt(val, "", false) {  }
CondModOpt::CondModOpt(const string& desc) : CondModOpt(0, desc, false) {  }
CondModOpt::CondModOpt(const double& val, const string& desc) : CondModOpt(val, desc, false) {  }
CondModOpt::CondModOpt(const bool& sel) : CondModOpt(0, "", sel) {  }
CondModOpt::CondModOpt(const double& val, const bool& sel) : CondModOpt(val , "", sel) {  }
CondModOpt::CondModOpt(const string& desc, const bool& sel) : CondModOpt(0, desc, sel) {  }
CondModOpt::CondModOpt(const CondModOpt& source) : CondModOpt(source.value(), source.description, source.selected) {  }

CondModOpt& CondModOpt::operator=(const string& rhs) {
	string::size_type idx;
	bool val = std::stod(rhs, &idx);
	if (idx != 0) {
		value(val);
		idx += 1; // For the trailing space... maybe we should specifically remove it if it is a space?
	}
	description = rhs.substr(idx);
	return *this;
}

CondModOpt& CondModOpt::operator=(const bool& rhs) {
	selected = rhs;
	return *this;
}

CondModOpt& CondModOpt::operator=(const CondModOpt& rhs) {
	this->_value = rhs.value();
	description = rhs.description;
	selected = rhs.selected;
	return *this;
}

bool CondModOpt::operator==(const string& rhs) const { return description == rhs; }
bool CondModOpt::operator==(const bool& rhs) const { return selected == rhs; }
bool CondModOpt::operator==(const CondModOpt& rhs) const { 
	return ((this->_value == rhs.value()) && (description == rhs.description));
}

CondModOpt::operator string() const {
	char buffer[STRING_BUFFER_SIZE];
	snprintf(&buffer[0], STRING_BUFFER_SIZE, "%#d ", value());
	return buffer + description;
}
CondModOpt::operator bool() const { return selected; }

// ----------------------------------------
// CondMod --------------------------------
// ----------------------------------------

void CondMod::_init() {
	_options = vector<CondModOpt*>();
	_ownership = vector<bool>();
	this->removeParent(this->_offset);
	delete &(this->_offset);
	this->_ownsoffset = false;
	_agger = SumVal();
	this->_offset = _agger;
	this->addParent(_agger);
}

CondMod::CondMod() : ModVal() { _init(); }
CondMod::CondMod(const string& name) : ModVal(name) { _init(); }
CondMod::CondMod(NumVal& base) : ModVal(base) { _init(); }
CondMod::CondMod(const string& name, NumVal& base) : ModVal(name, base) { _init(); }
CondMod::CondMod(const double& val) : ModVal(val) { _init(); }
CondMod::CondMod(const string& name, const double& val) : ModVal(name, val) { _init(); }
CondMod::CondMod(NumVal& base, const double& val) : ModVal(base, val) { _init(); }
CondMod::CondMod(const string& name, NumVal& base, const double& val) : ModVal(name, base, val) { _init(); }

CondMod::CondMod(const xml::Element* source) : ModVal( /* source */) {
	_init();
	parse(source);
}

CondMod::CondMod(const CondMod& source) : ModVal(source) {
	_init();
	this->pause();
	vector<const CondModOpt*> srcopts = source.options();
	const CondModOpt* newopt;
	for (int i = 0; i < srcopts.size(); i+=1) {
		newopt = srcopts[i];
		createOption(*newopt);
	}
	this->unpause();
}

CondMod::~CondMod() {
	this->pause();
	for (uint i = size(); i > 0; i -= 1) {
		removeOption(i - 1);
	}
}

bool CondMod::hasOption(const CondModOpt& testopt) const {
	for (typename vector<CondModOpt*>::const_iterator it = _options.begin(); it != _options.end(); it++) {
		if ((*it) == &testopt) return true;
	}
	return false;
}

int CondMod::addOption(CondModOpt& newopt, bool own) {
	if (hasOption(newopt)) return indexOf(newopt);
	_options.push_back(&newopt);
	_ownership.push_back(own);
	_agger.addItem(newopt);
	return indexOf(newopt);
}

int CondMod::createOption(double val, string description, bool sel) {
	CondModOpt* newopt = new CondModOpt(val, description, sel);
	return addOption(*newopt, true);
}

int CondMod::createOption(const CondModOpt& source) {
	CondModOpt* newopt = new CondModOpt(source);
	return addOption(*newopt, true);
}

void CondMod::removeOption(const int& index) {
	if (index >= (int)(size())) return;
	typename vector<CondModOpt*>::iterator optit = _options.begin();
	typename vector<bool>::iterator ownit = _ownership.begin();
	for (int i = 0; i < index; i++) {
		optit++;
		ownit++;
	}
	CondModOpt* oldval = *optit;
	bool owned = *ownit;
	_agger.removeItem(*oldval);
	_options.erase(optit);
	_ownership.erase(ownit);
	if (owned) delete oldval;
}

void CondMod::removeOption(const CondModOpt& oldopt) {
	removeOption(indexOf(oldopt));
}

vector<const CondModOpt*> CondMod::options() const {
	vector<const CondModOpt*> ret = vector<const CondModOpt*>();
	for (typename vector<CondModOpt*>::const_iterator it = _options.begin(); it != _options.end(); it++)
		ret.push_back(*it);
	return ret;
}

vector<CondModOpt*> CondMod::options() {
	return vector<CondModOpt*>(_options);
}

int CondMod::indexOf(const CondModOpt& testval) const {
	bool found = false;
	int i = 0;
	for (typename vector<CondModOpt*>::const_iterator it = _options.begin(); it != _options.end(); it++) {
		if (*it == &testval) {
			found = true;
			break;
		}
		i += 1;
	}
	return (found) ? i : -1;
}

uint CondMod::size() const {
	return (uint)(_options.size());
}

const CondModOpt& CondMod::operator[](const int& index) const {
	vector<const CondModOpt*> opts = options();
	return *(opts[index]);
}

CondModOpt& CondMod::operator[](const int& index) {
	return const_cast<CondModOpt&>(static_cast<const CondMod*>(this)->operator[](index));
}

void CondMod::parse(const xml::Element* source) {
	if (source->has_element("name")) name(source->first_element("name")->text());
	base(std::stod(source->first_element("base")->text()));
	CondModOpt* newcmodo;
	vector<xml::Element*> modifiers = source->child_elements("modifier");
	for (vector<xml::Element*>::iterator it = modifiers.begin(); it != modifiers.end(); it++) {
		newcmodo = new CondModOpt();
		*newcmodo = (*it)->text();
		addOption(*newcmodo, true);
	}
	((Traited*)(this))->parse(source);
}