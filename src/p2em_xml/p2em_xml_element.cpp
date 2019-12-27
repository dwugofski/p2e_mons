#include "p2em_xml/p2em_xml.hpp"
#include <algorithm>

using namespace p2em_xml;

Element::Element(Document* doc, xerc::DOMElement* self) : Node(doc, self) { _el = self; }

string Element::text() const {
	return _xercread(_el->getTextContent());
}

string Element::tagname() const {
	return _xercread(_el->getTagName());
}

Node* Element::first_child() const {
	xerc::DOMNode* ret = _g_child();
	return (ret == nullptr) ? nullptr : _srcdoc->grab(ret);
}

Node* Element::first_child(const Node::Type& filter) const {
	xerc::DOMNode* ret = _g_child(false, true, filter);
	return (ret == nullptr) ? nullptr : _srcdoc->grab(ret);
}

Node* Element::last_child() const {
	xerc::DOMNode* ret = _g_child(true);
	return (ret == nullptr) ? nullptr : _srcdoc->grab(ret);
}

Node* Element::last_child(const Node::Type& filter) const {
	xerc::DOMNode* ret = _g_child(true, true, filter);
	return (ret == nullptr) ? nullptr : _srcdoc->grab(ret);
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

bool Element::has_element(const string& tagname) const {
	XMLCh* tname = xerc::XMLString::transcode(tagname.c_str());
	xerc::DOMNodeList* nodes = _el->getElementsByTagName(tname);
	xerc::XMLString::release(&tname);
	return nodes->getLength() > 0;
}

Element* Element::first_element() const {
	xerc::DOMNode* ret = _g_child(false, true, Node::Type::ELEMENT);
	return (ret == nullptr) ? nullptr : _srcdoc->grab((xerc::DOMElement*)(ret));
}

Element* Element::first_element(const string& tagname) const {
	vector<Element*> list = child_elements(tagname);
	vector<Element*>::iterator it = list.begin();
	Element* ret = *it;
	// Should no longer be necessary as all nodes are handled through the document
	// it++; // I know, I know. But I think this is clearer
	// for (; it != list.end(); it++) {
	// 	delete* it;
	// }
	return ret;
}

Element* Element::last_element() const {
	xerc::DOMNode* ret = _g_child(true, true, Node::Type::ELEMENT);
	return (ret == nullptr) ? nullptr : _srcdoc->grab((xerc::DOMElement*)(ret));
}

Element* Element::last_element(const string& tagname) const {
	vector<Element*> list = child_elements(tagname);
	vector<Element*>::reverse_iterator it = list.rbegin(); // Doing it this way to make it more flexible with other iterable list types
	Element* ret = *it;
	// Should no longer be necessary as all nodes are handled through the document
	// it++; // I know, I know. But I think this is clearer
	// for (; it != list.rend(); it++) {
	// 	delete *it;
	// }
	return ret;
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

vector<Element*> Element::child_elements(const string& tagname) const {
	vector<Element*> ret = vector<Element*>();
	XMLCh* tname = xerc::XMLString::transcode(tagname.c_str());
	xerc::DOMNodeList* nodes = _el->getElementsByTagName(tname);
	xerc::XMLString::release(&tname);
	for (XMLSize_t i = 0; i < nodes->getLength(); i += 1) {
		ret.push_back(_srcdoc->grab((xerc::DOMElement*)(nodes->item(i))));
	}
	return ret;
}

Element* Element::next_element() const {
	xerc::DOMNode* ret = _g_sibling(false, true, Node::Type::ELEMENT);
	return (ret == nullptr) ? nullptr : _srcdoc->grab((xerc::DOMElement*)(ret));
}

Element* Element::prev_element() const {
	xerc::DOMNode* ret = _g_sibling(true, true, Node::Type::ELEMENT);
	return (ret == nullptr) ? nullptr : _srcdoc->grab((xerc::DOMElement*)(ret));
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
	return (ret == nullptr) ? nullptr : _srcdoc->grab((xerc::DOMAttr*)(ret));
}

