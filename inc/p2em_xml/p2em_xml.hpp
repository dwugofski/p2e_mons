#pragma once

#ifndef __P2EM_XML_HPP__
#define __P2EM_XML_HPP__

#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/sax/ErrorHandler.hpp"
#include "xercesc/sax/SAXParseException.hpp"
#include <string>
#include <iostream>

namespace p2em_xml {
	namespace xerc = xercesc;

	using Library = xerc::XMLPlatformUtils;
	using string = std::string;

	class Exception;
	class ErrorHandler;
	class Parser;
	class Node;
	class Attribute;
	class Element;
	class Document;

	enum class NodeType {
		ELEMENT,
		ATTRIBUTE,
		TEXT,
		CDATA,
		ENTITY_REF,
		ENTITY_NODE,
		PROC_INSTR,
		COMMENT,
		DOCUMENT,
		DOCTYPE,
		DOCFRAG,
		NOTATION
	};

	class Exception {
	public:
		enum class Code {
			FILE_ACCEES_ERROR, // Cannot find / open a file
			INVALID_SCHEMA, // The schema file was invalid
			SCHEMA_VIOLATION, // The XML document violates the provided schema

			ERROR
		};

	private:
		Code _code;
		string _msg;
		
	public:
		Exception(const Code& code);
		Exception(const Code& code, const string& message);

		string message() const;
		Code code() const;
	};

	class ErrorHandler : public xerc::ErrorHandler {
	public:
		enum class Mode {
			CHECK_SCHEMA,
			VALIDATE_DATA,
			PARSE_DATA
		};

	private:
		int _errcount;
		string _prelude;
		Mode _mode;
		std::ostream& _out;

	public:
		ErrorHandler();
		ErrorHandler(std::ostream& out);
		ErrorHandler(const string& prelude);
		ErrorHandler(std::ostream& out, const string& prelude);
		ErrorHandler(const Mode& mode);
		ErrorHandler(std::ostream& out, const Mode& mode);
		ErrorHandler(const string& prelude, const Mode& mode);
		ErrorHandler(std::ostream& out, const string& prelude, const Mode& mode);

		void warning(const xerc::SAXParseException& err) override;
		void error(const xerc::SAXParseException& err) override;
		void fatalError(const xerc::SAXParseException& err) override;
		void resetErrors() override;
		int errcount() const;
		void prelude(const string& prelude);
		string prelude() const;
		void mode(const Mode& mode);
		Mode mode() const;
	};
}

#endif