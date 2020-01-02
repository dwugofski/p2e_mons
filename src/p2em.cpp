#include "p2em_core.hpp"

using namespace p2em_core;

int main(int argc, char**argv) {
	Core core;
	xml::Library::Initialize();

	xml::Parser parser;

	try {
		xml::Document* doc = parser.parse("../../res/p2em_xml/p2emon.template.xml", "https://github.com/dwugofski/p2e_mons p2emon.xsd");
		xml::Element* monel = doc->first_element(Monster::TAGNAME);
		if (monel == nullptr) {
			std::cout << "Could not find the monster element!\n";
			throw new Exception(ExceptionCode::TEMPLATE_READ_ERROR);
		}
		Monster mon = Monster(core, monel);
	}
	catch (const std::exception e) {
		std::cout << "\nError encountered!\n";
		std::cout << e.what();
	}

	return 0;
}