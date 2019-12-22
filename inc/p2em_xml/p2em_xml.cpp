
#include "p2em_xml/p2em_xml.hpp"
#include <iostream>
#include <exception>

using namespace p2em_xml;


ErrorHandler::ErrorHandler() : xerc::ErrorHandler() {
	_errcount = 0;
}

void ErrorHandler::warning(const xerc::SAXParseException& err) {
	char* msg = xerc::XMLString::transcode(err.getMessage());
	std::cout << "Warning!\n\t" << msg << "\n";
	std::cout << "\t" << err.getLineNumber()  << "\n";
}

void ErrorHandler::error(const xerc::SAXParseException& err) {
	char* msg = xerc::XMLString::transcode(err.getMessage());
	std::cout << "Error!\n\t" << msg << "\n";
	std::cout << "\t" << err.getLineNumber() << "\n";
	_errcount++;
}

void ErrorHandler::fatalError(const xerc::SAXParseException& err) {
	char* msg = xerc::XMLString::transcode(err.getMessage());
	std::cout << "FATAL ERROR!\n    " << msg << "\n";
	std::cout << err.getLineNumber() << "\n";
	throw std::exception(msg);
	_errcount++;
}

void ErrorHandler::resetErrors() {
	_errcount = 0;
}

int ErrorHandler::errcount() const { return _errcount; }

int main() {
	std::cout << "Start!\n";
	try {
		Library::Initialize();
	}
	catch (const xerc::XMLException & toCatch) {
		char* message = xerc::XMLString::transcode(toCatch.getMessage());
		std::cout << "Error during initialization! :\n" << message << "\n";
		xerc::XMLString::release(&message);
		return 1;
	}
	catch (...) {
		std::cout << "Unexpected error!\n";
		return 1;
	}
	std::cout << "Inited!\n";

	XMLCh tempstr[100];
	xerc::XMLString::transcode("LS", tempstr, 99);
	xerc::DOMImplementation* impl = xerc::DOMImplementationRegistry::getDOMImplementation(tempstr);
	//DOMLSParser* parser = ((DOMImplementationLS*)impl)->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
	MyErrorHandler hdl;
	xerc::XercesDOMParser domParser;
	domParser.setErrorHandler(&hdl);
	domParser.setValidationScheme(xerc::XercesDOMParser::Val_Always);
	domParser.setDoNamespaces(true);
	domParser.setDoSchema(true);
	//domParser.setExternalNoNamespaceSchemaLocation("E://David/Personal%20Documents/git/p2e_mons/res/p2em_xml/p2emon.xsd");

	/*if (parser->getDomConfig()->canSetParameter(XMLUni::fgDOMValidate, true)) {
		std::cout << "Enabling validation!\n";
		parser->getDomConfig()->setParameter(XMLUni::fgDOMValidate, true);
	}*/

	//const char* xmlfile = "../../res/p2em_xml/p2emon.template.xml";
	const char* xmlfile = "../../res/p2em_xml/p2emon.template.xml";
	std::string schemaloc = "../../res/p2em_xml/p2emon.xsd";
	schemaloc = "https://github.com/dwugofski/p2e_mons " + schemaloc;
	std::cout << schemaloc << "\n";
	xerc::DOMDocument* doc = nullptr;

	std::cout << "Parsing!\n";
	try {
		domParser.setExternalSchemaLocation(schemaloc.c_str());
		domParser.parse(xmlfile);
		if (domParser.getErrorCount() != 0) {
			std::cout << hdl.errcount() << " Errors found in document!\n";
		} else std::cout << "No errors found in document\n";
		doc = domParser.getDocument();
		xerc::DOMNode* child = doc->getDocumentElement()->getFirstChild();
		char* msg;
		while (child != nullptr) {
			msg = xerc::XMLString::transcode(child->getNodeName());
			std::cout << msg << "\n";
			child = child->getNextSibling();
		}
	}
	catch (const xerc::XMLException & tocatch) {
		std::cout << "Error during the thing! :\n";
		char* message = xerc::XMLString::transcode(tocatch.getMessage());
		std::cout << message << "\n";
		xerc::XMLString::release(&message);
		return 1;
	}
	catch (const xerc::DOMException & tocatch) {
		std::cout << "Error during the thing! :\n";
		char* message = xerc::XMLString::transcode(tocatch.getMessage());
		std::cout << message << "\n";
		xerc::XMLString::release(&message);
		return 1;
	}
	catch (...) {
		std::cout << "Unexpected Exception\n";
		return -1;
	}
	std::cout << "Parsed!\n";

	std::cout << "~~Done with succcess!~~\n";
	return 0;
}