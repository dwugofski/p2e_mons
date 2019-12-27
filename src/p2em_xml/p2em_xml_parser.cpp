#include "p2em_xml/p2em_xml.hpp"
#include <fstream>
#include <exception>

using namespace p2em_xml;

// ---------------------------------------------
// Global Functions ----------------------------
// ---------------------------------------------

// template<typename T_ret_t> // Consider making this more generic...
vector<string> p2em_xml::node2strvec(const Element* src, const string& tagname) {
	vector<string> ret = vector<string>();
	vector<Element*> children = src->child_elements(tagname);
	for (vector<Element*>::iterator it = children.begin(); it != children.end(); it++) {
		ret.push_back((*it)->text()); // If we add operator string(const Element* source), we can get this to work generically for the string via T_ret_t((*it))
	}
	return ret; // The concern is getting classes which can use this vector easily (i.e. issues with copy constructors)
}

// ---------------------------------------------
// Parser Class --------------------------------
// ---------------------------------------------

static bool _exists(const string& filename) {
	FILE* file;
	if (fopen_s(&file, filename.c_str(), "r") == 0) {
		fclose(file);
		return true;
	}
	else return false;
}

Parser::Parser() {
}

// Parser::~Parser() {  }

// Generic parsing
xerc::DOMDocument* Parser::_gParse(const string& filename, string schemaname, bool doschema, bool ischema) {
	xerc::DOMDocument* ret = nullptr;
	if (!_exists(filename)) {
		string msg = "Could not open data file \"" + filename + "\"";
		throw Exception(Exception::Code::FILE_ACCEES_ERROR, msg);
	}/*
	if (doschema) {
		if (!_exists(schemaname)) {
			string msg = "Could not open schema file \"" + schemaname + "\"";
			throw Exception(Exception::Code::FILE_ACCEES_ERROR, msg);
		}
	}*/
	try {
		std::fstream f;
		f.open(ErrorHandler::ERROR_LOG_FILE, std::fstream::out | std::fstream::app);
		ErrorHandler errh(f, filename, ischema ? ErrorHandler::Mode::CHECK_SCHEMA : ErrorHandler::Mode::PARSE_DATA);

		_parser.setErrorHandler(&hdl);
		_parser.setValidationScheme(doschema ? xerc::XercesDOMParser::Val_Always : xerc::XercesDOMParser::Val_Never);
		_parser.setDoNamespaces(true);
		_parser.setDoSchema(doschema);

		if (doschema) {
			_parser.setExternalSchemaLocation(schemaname.c_str());
		}
		_parser.parse(filename.c_str());
		if (_parser.getErrorCount() != 0) {
			throw Exception(Exception::Code::SCHEMA_VIOLATION, "Errors found in " + filename);
		}
		ret = _parser.getDocument();
		f.close();
		return ret;
	}
	catch (const Exception & err0) { throw err0; }
	catch (const std::exception err1) {
		throw Exception(Exception::Code::ERROR, err1.what());
	}
	catch (...) {
		throw Exception(Exception::Code::ERROR);
	}
}

Document* Parser::parse(const string& filename) {
	return new Document(_gParse(filename));
}

Document* Parser::parse(const string& filename, const string& schemaloc) {
	return new Document(_gParse(filename, schemaloc, true));
}

bool Parser::validate(const string& filename, const string& schemaloc) {
	try {
		xerc::DOMDocument* schemadoc = _gParse(schemaloc, "", false, true);
		schemadoc->release();
		xerc::DOMDocument* newdoc = _gParse(filename, schemaloc, true);
		newdoc->release();
		return true;
	}
	catch (Exception& err0) {
		if (err0.code() == Exception::Code::SCHEMA_VIOLATION) return false;
		else throw err0;
	}


}