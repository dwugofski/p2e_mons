#include "p2em_xml/p2em_xml.hpp"
#include <algorithm>

using namespace p2em_xml;

Element::Element(Document* doc, xerc::DOMElement* self) : Node(doc, self) { _el = self; }

string Element::value() const {
	return _xercread(_el->getNodeValue());
}

string Element::tagname() const {
	return _xercread(_el->getTagName());
}

Node* Element::first_child() const {
	xerc::DOMNode* ret = _g_child();
	return (ret == nullptr) ? nullptr : new Node(_srcdoc, ret);
}

Node* Element::first_child(const Node::Type& filter) const {
	xerc::DOMNode* ret = _g_child(false, true, filter);
	return (ret == nullptr) ? nullptr : new Node(_srcdoc, ret);
}

Node* Element::last_child() const {
	xerc::DOMNode* ret = _g_child(true);
	return (ret == nullptr) ? nullptr : new Node(_srcdoc, ret);
}

Node* Element::last_child(const Node::Type& filter) const {
	xerc::DOMNode* ret = _g_child(true, true, filter);
	return (ret == nullptr) ? nullptr : new Node(_srcdoc, ret);
}

vector<Node*> Element::children() const {
	vector<Node*> ret = vector<Node*>();
	Node* newn = first_child();
	while (newn != nullptr) {
		ret.push_back(newn);
		newn = newn->next_sibling();
	}
	return ret;
}

vector<Node*> Element::children(const Node::Type& filter) const {
	vector<Node*> ret = vector<Node*>();
	Node* newn = first_child(filter);
	while (newn != nullptr) {
		ret.push_back(newn);
		newn = newn->next_sibling(filter);
	}
	return ret;
}

Element* Element::first_element() const {
	xerc::DOMNode* ret = _g_child(false, true, Node::Type::ELEMENT);
	return (ret == nullptr) ? nullptr : new Element(_srcdoc, (xerc::DOMElement*)(ret));
}

Element* Element::last_element() const {
	xerc::DOMNode* ret = _g_child(true, true, Node::Type::ELEMENT);
	return (ret == nullptr) ? nullptr : new Element(_srcdoc, (xerc::DOMElement*)(ret));
}

vector<Element*> Element::child_elements() const {
	vector<Element*> ret = vector<Element*>();
	Element* newn = first_element();
	while (newn != nullptr) {
		ret.push_back(newn);
		newn = newn->next_element();
	}
	return ret;
}

Element* Element::next_element() const {
	xerc::DOMNode* ret = _g_sibling(false, true, Node::Type::ELEMENT);
	return (ret == nullptr) ? nullptr : new Element(_srcdoc, (xerc::DOMElement*)(ret));
}

Element* Element::prev_element() const {
	xerc::DOMNode* ret = _g_sibling(true, true, Node::Type::ELEMENT);
	return (ret == nullptr) ? nullptr : new Element(_srcdoc, (xerc::DOMElement*)(ret));
}

vector<Element*> Element::sibling_elements() const {
	vector<Element*> ret = vector<Element*>();
	Element* newn = prev_element();
	while (newn != nullptr) {
		ret.push_back(newn);
		newn = newn->prev_element();
	}
	std::reverse(ret.begin(), ret.end());
	newn = next_element();
	while (newn != nullptr) {
		ret.push_back(newn);
		newn = newn->next_element();
	}
	return ret;
}

bool Element::has_attr(const string& name) const {
	XMLCh* xercs = xerc::XMLString::transcode(name.c_str());
	bool ret = _el->hasAttribute(xercs);
	xerc::XMLString::release(&xercs);
	return ret;
}

string Element::get(const string& attrname) const {
	XMLCh* xstr = xerc::XMLString::transcode(attrname.c_str());
	string ret = _xercread(_el->getAttribute(xstr));
	xerc::XMLString::release(&xstr);
	return ret;
}
string Element::operator[](const string& attrname) const { return get(attrname); }

Attribute* Element::attr(const string& attrname) const {
	XMLCh* xstr = xerc::XMLString::transcode(attrname.c_str());
	xerc::DOMAttr* ret = _el->getAttributeNode(xstr);
	xerc::XMLString::release(&xstr);
	return (ret == nullptr) ? nullptr : new Attribute(_srcdoc, (xerc::DOMAttr*)(ret));
}

