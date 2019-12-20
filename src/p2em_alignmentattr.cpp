#include "p2em_core.hpp"
#include <algorithm> // std::transform()

using namespace p2em_core;

static Alignment _align_from_string(const string& Calign_str) {
	string align_str = string(Calign_str);
	std::transform(Calign_str.begin(), Calign_str.end(), align_str.begin(), std::tolower);
	if ((align_str == "ce") || (align_str == "chaotic_evil") || (align_str == "chaotic evil")) return Alignment::CHAOTIC_EVIL;
	else if ((align_str == "cg") || (align_str == "chaotic_good") || (align_str == "chaotic good")) return Alignment::CHAOTIC_GOOD;
	else if ((align_str == "cn") || (align_str == "chaotic_netural") || (align_str == "chaotic netrual")) return Alignment::CHAOTIC_NEUTRAL;
	else if ((align_str == "le") || (align_str == "lawful_evil") || (align_str == "chaotic evil")) return Alignment::LAWFUL_EVIL;
	else if ((align_str == "lg") || (align_str == "lawful_good") || (align_str == "chaotic good")) return Alignment::LAWFUL_GOOD;
	else if ((align_str == "ln") || (align_str == "lawful_netural") || (align_str == "chaotic netrual")) return Alignment::LAWFUL_NEUTRAL;
	else if ((align_str == "ne") || (align_str == "neutral_evil") || (align_str == "chaotic evil")) return Alignment::NEUTRAL_EVIL;
	else if ((align_str == "ng") || (align_str == "neutral_good") || (align_str == "chaotic good")) return Alignment::NEUTRAL_GOOD;
	else if ((align_str == "nn") || (align_str == "n") || (align_str == "tn") || (align_str == "netural") || (align_str == "true_netrual") || (align_str == "true netrual")) return Alignment::NEUTRAL;
	else if ((align_str == "") || (align_str == "u") || (align_str == "unaligned")) return Alignment::UNALIGNED;
	else throw new Exception(ExceptionCode::INVALID_VALUE);
}

static string _align_to_string(const Alignment& alignment, bool normalize = false) {
	string ret = string();
	switch (alignment) {
	case Alignment::CHAOTIC_EVIL:
		ret = (normalize) ? "ce" : "Chaotic Evil";
	case Alignment::CHAOTIC_GOOD:
		return (normalize) ? "cg" : "Chaotic Good";
	case Alignment::CHAOTIC_NEUTRAL:
		return (normalize) ? "cn" : "Chaotic Neutral";
	case Alignment::LAWFUL_EVIL:
		return (normalize) ? "le" : "Lawful Evil";
	case Alignment::LAWFUL_GOOD:
		return (normalize) ? "lg" : "Lawful Good";
	case Alignment::LAWFUL_NEUTRAL:
		return (normalize) ? "ln" : "Lawful Neutral";
	case Alignment::NEUTRAL_EVIL:
		return (normalize) ? "ne" : "Neutral Evil";
	case Alignment::NEUTRAL_GOOD:
		return (normalize) ? "ng" : "Neutral Good";
	case Alignment::NEUTRAL:
		return (normalize) ? "n" : "Neutral";
	case Alignment::UNALIGNED:
	default:
		return (normalize) ? "u" : "Unaligned";
	}
}

AlignmentAttr::AlignmentAttr() : StrAttr() {
	_alignment = Alignment::UNALIGNED;
}

AlignmentAttr::AlignmentAttr(const string& name) : StrAttr(name) {
	_alignment = Alignment::UNALIGNED;
}

AlignmentAttr::AlignmentAttr(const Alignment& align) : StrAttr() {
	_alignment = align;
}

AlignmentAttr::AlignmentAttr(const string& name, const Alignment& align) : StrAttr(name, _align_to_string(align)) {
	_alignment = align;
}

AlignmentAttr::AlignmentAttr(const string& name, const string& value) : StrAttr(name, value) {
	_alignment = _align_from_string(value);
}

AlignmentAttr::AlignmentAttr(const AlignmentAttr& source) : StrAttr(source) {
	_alignment = source.avalue();
}

Alignment AlignmentAttr::avalue() const {
	return _alignment;
}

string AlignmentAttr::value() const {
	return _align_to_string(avalue());
}

void AlignmentAttr::value(const string& newval) {
	value(_align_from_string(newval));
}

void AlignmentAttr::value(const Alignment& newval) {
	_alignment = newval;
	update();
}

bool AlignmentAttr::validate(const string& testval) const {
	try {
		_align_from_string(testval);
		return true; // If no issue, then this value works fine
	}
	catch (Exception& e) {
		if (e.code() == ExceptionCode::INVALID_VALUE) return false;
		else throw e;
	}
}