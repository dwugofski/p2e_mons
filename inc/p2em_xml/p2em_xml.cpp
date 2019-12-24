
#include "p2em_xml/p2em_xml.hpp"
#include <iostream>
#include <exception>

using namespace p2em_xml;

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

	std::cout << "Starting the parser\n";
	try {
		Parser parser;

		std::cout << "Loading the file\n";
		Document* doc = parser.parse("../../res/p2em_xml/p2emon.template.xml", "https://github.com/dwugofski/p2e_mons p2emon.xsd");
		std::cout << "Successful parsing\n";

		std::cout << "Navigating nodes\n";
		std::cout << doc->first_child()->name() << "\n";
		std::cout << doc->first_child()->next_sibling()->name() << "\n";
		std::cout << doc->first_child(Node::Type::ELEMENT)->name() << "\n";
		std::cout << doc->first_element()->name() << "\n";
		std::cout << doc->first_element()->get("id") << "\n";
		Element* el1 = doc->first_element();
		Element* el2 = doc->first_element();
		printf("0x%016X : 0x%016X\n", el1, el2);
		printf("0x%016X : 0x%016X\n", el1->xercnode(), el2->xercnode());
	}
	catch (const Exception & e) {
		std::cout << e.message();
		return 1;
	}
	return 0;
}