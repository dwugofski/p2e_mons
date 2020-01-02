#include "p2em_xml/p2em_xml.hpp"

using namespace p2em_xml;

Document::Document(xerc::DOMDocument* self) : Element(this, self->getDocumentElement()) {
	_doc = self;
	_nodes = umap<xerc::DOMNode*, Node*>();
}

Document::~Document() {
	umap<xerc::DOMNode*, Node*>::iterator it, ot;
	it = _nodes.begin();
	while (it != _nodes.end()) {
		ot = (it++);
		delete (*ot).second;
		_nodes.erase(ot);
	}
	_doc->release();
}

void Document::track_node(Node* newnode) {
	if (newnode == this) return;
	for (umap<xerc::DOMNode*, Node*>::iterator it = _nodes.begin(); it != _nodes.end(); it++) {
		if ((*it).second == newnode) return;
	}
	_nodes.insert({ newnode->xercnode(), newnode });
}

void Document::untrack_node(const Node* oldnode) {
	umap<xerc::DOMNode*, Node*>::iterator it, ot;
	it = _nodes.begin();
	while (it != _nodes.end()) {
		ot = (it++);
		if ((*ot).second == oldnode) {
			_nodes.erase(ot);
		}
	}
}

Node* Document::grab(xerc::DOMNode* xercnode) {
	umap<xerc::DOMNode*, Node*>::iterator it = _nodes.find(xercnode);
	if (it == _nodes.end()) return new Node(this, xercnode);
	else return (*it).second;
}

Attribute* Document::grab(xerc::DOMAttr* xercnode) {
	umap<xerc::DOMNode*, Node*>::iterator it = _nodes.find(xercnode);
	if (it == _nodes.end()) return new Attribute(this, xercnode);
	else return (Attribute*)((*it).second);
}

Element* Document::grab(xerc::DOMElement* xercnode) {
	umap<xerc::DOMNode*, Node*>::iterator it = _nodes.find(xercnode);
	if (it == _nodes.end()) return new Element(this, xercnode);
	else return (Element*)((*it).second);
}