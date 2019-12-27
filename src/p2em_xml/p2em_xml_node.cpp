#include "p2em_xml/p2em_xml.hpp"
#include <algorithm>

using namespace p2em_xml;

Node::Node(Document* doc, xerc::DOMNode* self) {
	_node = self;
	_srcdoc = doc;
	_srcdoc->track_node(this);
}

Node::~Node() {
	_srcdoc->untrack_node(this);
}

string Node::_xercread(const XMLCh* source) {
	char* msg = xerc::XMLString::transcode(source);
	string ret = msg;
	xerc::XMLString::release(&msg);
	return ret;
}

string Node::name() const {
	return _xercread(_node->getNodeName());
}

Node::Type Node::type() const {
	return xerc2t(_node->getNodeType());
}

const xerc::DOMNode* Node::xercnode() const {
	return _node;
}
xerc::DOMNode* Node::xercnode() { return const_cast<xerc::DOMNode*>(static_cast<const Node*>(this)->xercnode()); }

Node* Node::parent() const {
	xerc::DOMNode* ret = _node->getParentNode();
	return (ret == nullptr) ? nullptr : new Node(_srcdoc, ret);
}

Document* Node::doc() const {
	return _srcdoc;
}

xerc::DOMNode* Node::_g_sibling(bool backwards, bool dofilter, Node::Type filter) const {
	xerc::DOMNode* ret = (backwards) ? _node->getPreviousSibling() : _node->getNextSibling();
	xerc::DOMNode* next;
	if (dofilter) {
		while (ret != nullptr) {
			if (xerc2t(ret->getNodeType()) == filter) break;
			next = (backwards) ? ret->getPreviousSibling() : ret->getNextSibling();
			// ret->release(); // Do not release; let the document own it to prevent use-after-free
			ret = next;
		}
	}
	return ret;
}

xerc::DOMNode* Node::_g_child(bool backwards, bool dofilter, Node::Type filter) const {
	xerc::DOMNode* ret = (backwards) ? _node->getLastChild() : _node->getFirstChild();
	xerc::DOMNode* next;
	if (dofilter) {
		while (ret != nullptr) {
			if (xerc2t(ret->getNodeType()) == filter) break;
			next = (backwards) ? ret->getPreviousSibling() : ret->getNextSibling();
			// ret->release(); // Do not release; let the document own it to prevent use-after-free
			ret = next;
		}
	}
	return ret;
}

Node* Node::next_sibling() const {
	xerc::DOMNode* ret = _g_sibling();
	return (ret == nullptr) ? nullptr : _srcdoc->grab(ret);
}

Node* Node::next_sibling(const Node::Type& filter) const {
	xerc::DOMNode* ret = _g_sibling(false, true, filter);
	return (ret == nullptr) ? nullptr : _srcdoc->grab(ret);
}

Node* Node::prev_sibling() const {
	xerc::DOMNode* ret = _g_sibling(true);
	return (ret == nullptr) ? nullptr : _srcdoc->grab(ret);
}

Node* Node::prev_sibling(const Node::Type& filter) const {
	xerc::DOMNode* ret = _g_sibling(true, true, filter);
	return (ret == nullptr) ? nullptr : _srcdoc->grab(ret);
}

vector<Node*> Node::siblings() const {
	vector<Node*> ret = vector<Node*>();
	Node* newn = prev_sibling();
	while (newn != nullptr) {
		ret.push_back(newn);
		newn = newn->prev_sibling();
	}
	std::reverse(ret.begin(), ret.end());
	newn = next_sibling();
	while (newn != nullptr) {
		ret.push_back(newn);
		newn = newn->next_sibling();
	}
	return ret;
}

vector<Node*> Node::siblings(const Node::Type& filter) const {
	vector<Node*> ret = vector<Node*>();
	Node* newn = prev_sibling(filter);
	while (newn != nullptr) {
		ret.push_back(newn);
		newn = newn->prev_sibling(filter);
	}
	std::reverse(ret.begin(), ret.end());
	newn = next_sibling(filter);
	while (newn != nullptr) {
		ret.push_back(newn);
		newn = newn->next_sibling(filter);
	}
	return ret;
}

bool p2em_xml::operator==(const Node::Type& lhs, const xerc::DOMNode::NodeType& rhs) {
	switch (lhs) {
	case Node::Type::ATTRIBUTE:
		return rhs == xerc::DOMNode::NodeType::ATTRIBUTE_NODE;
	case Node::Type::CDATA:
		return rhs == xerc::DOMNode::NodeType::CDATA_SECTION_NODE;
	case Node::Type::COMMENT:
		return rhs == xerc::DOMNode::NodeType::COMMENT_NODE;
	case Node::Type::DOCFRAG:
		return rhs == xerc::DOMNode::NodeType::DOCUMENT_FRAGMENT_NODE;
	case Node::Type::DOCTYPE:
		return rhs == xerc::DOMNode::NodeType::DOCUMENT_TYPE_NODE;
	case Node::Type::ELEMENT:
		return rhs == xerc::DOMNode::NodeType::ELEMENT_NODE;
	case Node::Type::ENTITY_NODE:
		return rhs == xerc::DOMNode::NodeType::ENTITY_NODE;
	case Node::Type::ENTITY_REF:
		return rhs == xerc::DOMNode::NodeType::ENTITY_REFERENCE_NODE;
	case Node::Type::PROC_INSTR:
		return rhs == xerc::DOMNode::NodeType::PROCESSING_INSTRUCTION_NODE;
	case Node::Type::NOTATION:
		return rhs == xerc::DOMNode::NodeType::NOTATION_NODE;
	case Node::Type::TEXT:
		return rhs == xerc::DOMNode::NodeType::TEXT_NODE;
	default:
		throw Exception(Exception::Code::UNRECOGNIZED_NODE_TYPE);
	}
}
bool p2em_xml::operator==(const xerc::DOMNode::NodeType& lhs, const Node::Type& rhs) { return rhs == lhs; }

Node::Type p2em_xml::xerc2t(const xerc::DOMNode::NodeType& from) {
	switch (from) {
	case xerc::DOMNode::NodeType::ATTRIBUTE_NODE:
		return Node::Type::ATTRIBUTE;
	case xerc::DOMNode::NodeType::CDATA_SECTION_NODE:
		return Node::Type::CDATA;
	case xerc::DOMNode::NodeType::COMMENT_NODE:
		return Node::Type::COMMENT;
	case xerc::DOMNode::NodeType::DOCUMENT_FRAGMENT_NODE:
		return Node::Type::DOCFRAG;
	case xerc::DOMNode::NodeType::DOCUMENT_NODE:
		return Node::Type::DOCUMENT;
	case xerc::DOMNode::NodeType::DOCUMENT_TYPE_NODE:
		return Node::Type::DOCTYPE;
	case xerc::DOMNode::NodeType::ELEMENT_NODE:
		return Node::Type::ELEMENT;
	case xerc::DOMNode::NodeType::ENTITY_NODE:
		return Node::Type::ENTITY_NODE;
	case xerc::DOMNode::NodeType::ENTITY_REFERENCE_NODE:
		return Node::Type::ENTITY_REF;
	case xerc::DOMNode::NodeType::NOTATION_NODE:
		return Node::Type::NOTATION;
	case xerc::DOMNode::NodeType::PROCESSING_INSTRUCTION_NODE:
		return Node::Type::PROC_INSTR;
	case xerc::DOMNode::NodeType::TEXT_NODE:
		return Node::Type::TEXT;
	default:
		throw Exception(Exception::Code::UNRECOGNIZED_NODE_TYPE);
	}
}

xerc::DOMNode::NodeType p2em_xml::t2xerc(const Node::Type& from) {
	switch (from) {
	case Node::Type::ATTRIBUTE:
		return xerc::DOMNode::NodeType::ATTRIBUTE_NODE;
	case Node::Type::CDATA:
		return xerc::DOMNode::NodeType::CDATA_SECTION_NODE;
	case Node::Type::COMMENT:
		return xerc::DOMNode::NodeType::COMMENT_NODE;
	case Node::Type::DOCFRAG:
		return xerc::DOMNode::NodeType::DOCUMENT_FRAGMENT_NODE;
	case Node::Type::DOCTYPE:
		return xerc::DOMNode::NodeType::DOCUMENT_TYPE_NODE;
	case Node::Type::DOCUMENT:
		return xerc::DOMNode::NodeType::DOCUMENT_NODE;
	case Node::Type::ELEMENT:
		return xerc::DOMNode::NodeType::ELEMENT_NODE;
	case Node::Type::ENTITY_NODE:
		return xerc::DOMNode::NodeType::ENTITY_NODE;
	case Node::Type::ENTITY_REF:
		return xerc::DOMNode::NodeType::ENTITY_REFERENCE_NODE;
	case Node::Type::NOTATION:
		return xerc::DOMNode::NodeType::NOTATION_NODE;
	case Node::Type::PROC_INSTR:
		return xerc::DOMNode::NodeType::PROCESSING_INSTRUCTION_NODE;
	case Node::Type::TEXT:
		return xerc::DOMNode::NodeType::TEXT_NODE;
	default:
		throw Exception(Exception::Code::UNRECOGNIZED_NODE_TYPE);
	}
}

