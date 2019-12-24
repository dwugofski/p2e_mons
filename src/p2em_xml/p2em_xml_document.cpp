#include "p2em_xml/p2em_xml.hpp"

using namespace p2em_xml;

Document::Document(xerc::DOMDocument* self) : Element(this, self->getDocumentElement()) {
	_doc = self;
}

Document::~Document() {
	_doc->release();
}