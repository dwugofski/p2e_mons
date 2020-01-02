#pragma once

#ifndef __P2EM_XML_HPP__
#define __P2EM_XML_HPP__

#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/sax/ErrorHandler.hpp"
#include "xercesc/sax/SAXParseException.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

namespace p2em_xml {
	namespace xerc = xercesc;

	using Library = xerc::XMLPlatformUtils;
	using string = std::string;
	template<typename T_vec_t> using vector = std::vector<T_vec_t>;
	template<typename T_key_t, typename T_val_t> using umap = std::unordered_map<T_key_t, T_val_t>;

	class Exception;
	class ErrorHandler;
	class Parser;
	class Node;
	class Attribute;
	class Element;
	class Document;

	// template<typename T_ret_t> // Consider making this more generic...
	vector<string> node2strvec(const Element* srcnode, const string& tagname);

	class Exception {
	public:
		enum class Code {
			FILE_ACCEES_ERROR, // Cannot find / open a file
			INVALID_SCHEMA, // The schema file was invalid
			SCHEMA_VIOLATION, // The XML document violates the provided schema
			UNRECOGNIZED_NODE_TYPE,

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
	string ecode2str(const Exception::Code& code);
	std::ostream& operator<<(std::ostream& out, const Exception& ex);

	class ErrorHandler : public xerc::ErrorHandler {
	public:
		enum class Mode {
			CHECK_SCHEMA,
			VALIDATE_DATA,
			PARSE_DATA,
			UNSPECIFIED
		};

	private:
		int _errcount;
		string _prelude;
		Mode _mode;
		std::ostream* _out;

		string _message(const xerc::SAXParseException& err, const string& level);

	public:
		static const string ERROR_LOG_FILE;

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
		string prelude() const;
		void prelude(const string& prelude);
		Mode mode() const;
		void mode(const Mode& mode);
		std::ostream& out() const;
		void out(std::ostream& newout);
	};

	class Parser {
	private:
		xerc::XercesDOMParser _parser;
		ErrorHandler hdl;

		xerc::DOMDocument* _gParse(const string& filename, string schemafile = "", bool doschema = false, bool ischema = false);
	public:
		Parser();
		// ~Parser();

		Document* parse(const string& filename);
		Document* parse(const string& filename, const string& schemaloc);
		bool validate(const string& filename, const string& schemaloc);
	};

	class Node {
	public:
		enum class Type {
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
	private:
		xerc::DOMNode* _node;

	protected:
		Document* _srcdoc;

		static string _xercread(const XMLCh* source); // Not sure if necessary right now... adding just in case as a reminder

		xerc::DOMNode* _g_sibling(bool backwards = false, bool dofilter = false, Node::Type filter = Node::Type::ELEMENT) const;
		xerc::DOMNode* _g_child(bool backwards = false, bool dofilter = false, Node::Type filter = Node::Type::ELEMENT) const;

	public:
		Node(Document* doc, xerc::DOMNode* self);
		~Node(); // Do not release the xerces pointer, in case it is being used elsewhere

		// static Node* nodeFactory(Parser* source);

		string name() const;
		Type type() const;
		const xerc::DOMNode* xercnode() const;
		xerc::DOMNode* xercnode();

		// Functions to get parent node
		Node* parent() const;
		Document* doc() const;

		// Functions to get siblings
		Node* next_sibling() const;
		Node* next_sibling(const Type& filter) const;
		Node* prev_sibling() const;
		Node* prev_sibling(const Type& filter) const;
		vector<Node*> siblings() const;
		vector<Node*> siblings(const Type& filter) const;

	};

	// Comparison and translation for node types
	bool operator==(const Node::Type& lhs, const xerc::DOMNode::NodeType& rhs);
	bool operator==(const xerc::DOMNode::NodeType& lhs, const Node::Type& rhs);
	Node::Type xerc2t(const xerc::DOMNode::NodeType& from);
	xerc::DOMNode::NodeType t2xerc(const Node::Type& from);

	class Attribute : public Node {
	private:
		xerc::DOMAttr* _attr;

	public:
		Attribute(Document* doc, xerc::DOMAttr* self);
		// ~Attribute();

		string value() const;
		void value(const string& newval);
		Element* el() const;

		// Functions to get sibling attrs
		Attribute* next_attr() const;
		Attribute* prev_attr() const;
		vector<Attribute*> sibling_attrs() const;
	};

	class Element : public Node {
	private:
		xerc::DOMElement* _el;

	public:
		Element(Document* doc, xerc::DOMElement* self);
		// ~Element();

		string text() const;
		string tagname() const;

		// Functions to get children nodes
		Node* first_child() const;
		Node* first_child(const Type& filter) const;
		Node* last_child() const;
		Node* last_child(const Type& filter) const;
		vector<Node*> children() const;
		vector<Node*> children(const Type& filter) const;
		bool has_element(const string& tagname) const;
		Element* first_element() const;
		Element* first_element(const string& tagname) const;
		Element* last_element() const;
		Element* last_element(const string& tagname) const;
		vector<Element*> child_elements() const;
		vector<Element*> child_elements(const string& tagname) const;

		// Functions to get sibling elements
		Element* next_element() const;
		Element* prev_element() const;
		vector<Element*> sibling_elements() const;

		// Functions to add/remove children nodes


		// Functions to get attributes
		bool has_attr(const string& attrname) const;
		string get(const string& attrname) const;
		string operator[](const string& attrname) const;
		Attribute* attr(const string& attrname) const;

		// Functions to add/remove/modify attributes

	};

	class Document : public Element {
	private:
		xerc::DOMDocument* _doc;
		umap<xerc::DOMNode*, Node*> _nodes;

	public:
		Document(xerc::DOMDocument* self);
		~Document(); // Do release

		void track_node(Node* newnode);
		void untrack_node(const Node* oldnode);

		Node* grab(xerc::DOMNode* ptr);
		Attribute* grab(xerc::DOMAttr* ptr);
		Element* grab(xerc::DOMElement* ptr);
	};
}

#endif