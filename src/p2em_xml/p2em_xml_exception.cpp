#include "p2em_xml/p2em_xml.hpp"
#include <ctime>

using namespace p2em_xml;

// --------------------------------------------------
// Exception ----------------------------------------
// --------------------------------------------------

Exception::Exception(const Exception::Code& code) {
	_code = code;
	_msg = "";
}

Exception::Exception(const Exception::Code& code, const string& message) {
	_code = code;
	_msg = message;
}

string Exception::message() const {
	return _msg;
}

Exception::Code Exception::code() const {
	return _code;
}

string p2em_xml::ecode2str(const Exception::Code& code) {
	switch (code) {
	case Exception::Code::FILE_ACCEES_ERROR:
		return "Cannot access provided file";
	case Exception::Code::INVALID_SCHEMA:
		return "Provided schema file is not a valid schema file";
	case Exception::Code::SCHEMA_VIOLATION:
		return "Provided data file violates the associated schema";
	case Exception::Code::ERROR:
	default:
		return "Unexpected error";
	}
}

std::ostream& p2em_xml::operator<<(std::ostream& out, const Exception& rhs) {
	return (out << rhs.message());
}

// --------------------------------------------------
// ErrorHandler -------------------------------------
// --------------------------------------------------

const string ErrorHandler::ERROR_LOG_FILE = "error.log";

ErrorHandler::ErrorHandler() : xerc::ErrorHandler(), _out(&std::cout) {
	_errcount = 0;
	_prelude = "-----";
	_mode = ErrorHandler::Mode::UNSPECIFIED;
}

ErrorHandler::ErrorHandler(std::ostream& out) : xerc::ErrorHandler(), _out(&out) {
	_errcount = 0;
	_prelude = "-----";
	_mode = ErrorHandler::Mode::UNSPECIFIED;
}

ErrorHandler::ErrorHandler(const string& prelude) : xerc::ErrorHandler(), _out(&std::cout) {
	_errcount = 0;
	_prelude = prelude;
	_mode = ErrorHandler::Mode::UNSPECIFIED;
}

ErrorHandler::ErrorHandler(std::ostream& out, const string& prelude) : xerc::ErrorHandler(), _out(&out) {
	_errcount = 0;
	_prelude = prelude;
	_mode = ErrorHandler::Mode::UNSPECIFIED;
}

ErrorHandler::ErrorHandler(const ErrorHandler::Mode& mode) : xerc::ErrorHandler(), _out(&std::cout) {
	_errcount = 0;
	_prelude = "-----";
	_mode = mode;
}

ErrorHandler::ErrorHandler(std::ostream& out, const ErrorHandler::Mode& mode) : xerc::ErrorHandler(), _out(&out) {
	_errcount = 0;
	_prelude = "-----";
	_mode = mode;
}

ErrorHandler::ErrorHandler(const string& prelude, const ErrorHandler::Mode& mode) : xerc::ErrorHandler(), _out(&std::cout) {
	_errcount = 0;
	_prelude = prelude;
	_mode = mode;
}

ErrorHandler::ErrorHandler(std::ostream& out, const string& prelude, const ErrorHandler::Mode& mode) : xerc::ErrorHandler(), _out(&out) {
	_errcount = 0;
	_prelude = prelude;
	_mode = mode;
}

string ErrorHandler::_message(const xerc::SAXParseException& err, const string& level) {
	char* msg = xerc::XMLString::transcode(err.getMessage());
	string ret = "";
	time_t rawtime;
	tm timeinfo;
	char buffer[80];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", &timeinfo);
	std::string tstr(buffer);

	ret += tstr + " : ";
	ret += _prelude + " : (";
	snprintf(&buffer[0], 80, "%d", err.getLineNumber());
	ret += buffer;
	ret += ", ";
	snprintf(&buffer[0], 80, "%d", err.getColumnNumber());
	ret += ")\t";
	ret += level + "\n" + msg + "\n";
	xerc::XMLString::release(&msg);
	return ret;
}

void ErrorHandler::warning(const xerc::SAXParseException& err) {
	*_out << _message(err, "Warning!");
}

void ErrorHandler::error(const xerc::SAXParseException& err) {
	*_out << _message(err, "~~ERROR!!!~~");
	_errcount += 1;
}

void ErrorHandler::fatalError(const xerc::SAXParseException& err) {
	string rep = _message(err, "~~FATAL ERROR!!!~~");
	*_out << rep;
	_errcount += 1;
	Exception::Code errcode;
	switch (_mode) {
	case ErrorHandler::Mode::CHECK_SCHEMA:
		errcode = Exception::Code::INVALID_SCHEMA;
		break;
	case ErrorHandler::Mode::VALIDATE_DATA:
		errcode = Exception::Code::SCHEMA_VIOLATION;
		break;
	default:
		errcode = Exception::Code::ERROR;
	}
	char* msg = xerc::XMLString::transcode(err.getMessage());
	string errmsg = rep + msg;
	xerc::XMLString::release(&msg);
	throw new Exception(errcode, errmsg);
}

void ErrorHandler::resetErrors() {
	_errcount = 0;
}

int ErrorHandler::errcount() const { return _errcount; }

string ErrorHandler::prelude() const { return _prelude; }
void ErrorHandler::prelude(const string& pre) { _prelude = pre; }

ErrorHandler::Mode ErrorHandler::mode() const { return _mode; }
void ErrorHandler::mode(const ErrorHandler::Mode& mode) { _mode = mode; }

std::ostream& ErrorHandler::out() const { return *_out; }
void ErrorHandler::out(std::ostream& newout) { _out = &newout; }
