#include "p2em_xml/p2em_xml.hpp"
#include <algorithm>

using namespace p2em_xml;

Attribute::Attribute(Document* doc, xerc::DOMAttr* self) : Node(doc, self) { _attr = self; }

string Attribute::value() const {
	return _xercread(_attr->getValue());
}

void Attribute::value(const string& newval) {
	XMLCh* xercval = xerc::XMLString::transcode(newval.c_str());
	_attr->setValue(xercval);
	// Do we release xercval?
	xerc::XMLString::release(&xercval);
}

Element* Attribute::el() const {
	xerc::DOMElement* ret = _attr->getOwnerElement();
	return (ret == nullptr) ? nullptr : new Element(_srcdoc, ret);
}

Attribute* Attribute::next_attr() const {
	xerc::DOMNode* ret = _g_sibling(false, true, Node::Type::ATTRIBUTE);
	return (ret == nullptr) ? nullptr : new Attribute(_srcdoc, (xerc::DOMAttr*)(ret));
}

Attribute* Attribute::prev_attr() const {
	xerc::DOMNode* ret = _g_sibling(true, true, Node::Type::ATTRIBUTE);
	return (ret == nullptr) ? nullptr : new Attribute(_srcdoc, (xerc::DOMAttr*)(ret));
}

vector<Attribute*> Attribute::sibling_attrs() const {
	vector<Attribute*> ret = vector<Attribute*>();
	Attribute* nattr = prev_attr();
	while (nattr != nullptr) {
		ret.push_back(nattr);
		nattr = nattr->prev_attr();
	}
	std::reverse(ret.begin(), ret.end());
	nattr = next_attr();
	while (nattr != nullptr) {
		ret.push_back(nattr);
		nattr = nattr->next_attr();
	}
	return ret;
}