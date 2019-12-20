#include "p2em_core.hpp"
#include <stdlib.h>
#include <time.h>
#include <tuple>
#include <algorithm>

#define BUFFER_SIZE 100

using namespace p2em_core;

static Die _ntod(int num) {
	if (num < 0) num = 0 - num;
	switch(num) {
	case 2:
		return Die::TWO;
	case 3:
		return Die::THREE;
	case 4:
		return Die::FOUR;
	case 6:
		return Die::SIX;
	case 8:
		return Die::EIGHT;
	case 10:
		return Die::TEN;
	case 12:
		return Die::TWELVE;
	case 20:
		return Die::TWENTY;
	case 100:
		return Die::HUNDRED;
	default:
		throw new Exception(ExceptionCode::INVALID_VALUE, "Invalid die number!");
	}
}

static int _dton(Die die) {
	int ret = 0;
	switch (die) {
	case Die::HUNDRED:
		return 100;
	case Die::TWENTY:
		return 20;
	case Die::TWELVE:
		return 12;
	case Die::TEN:
		return 10;
	case Die::EIGHT:
		return 8;
	case Die::SIX:
		return 6;
	case Die::FOUR:
		return 4;
	case Die::THREE:
		return 3;
	case Die::TWO:
		return 2;
	default:
		throw new Exception(ExceptionCode::ERROR, "Unable to decode die!");
	}
}

static int _doroll(const Die& die, int count = 1) {
	int dienum = _dton(die);
	int ret = 0;

	srand((unsigned int)(time(NULL)));
	for (int i = 0; i < count; i += 1) {
		ret += ((rand() % dienum) + 1);
	}
	return ret;
}

static std::tuple<vector<Die>, vector<int>, int> _storoll(const string& specification) {
	// Spec string is of format "[any number of spaces][count]d<die num><possible spaces>[+/-]<possible spaces>[other roll spec, or modifier]"
	int modifier = 0;
	vector<Die> dice = vector<Die>();
	vector<int> counts = vector<int>();
	int temp = 0, index = 0, sign = 1;
	string::size_type sz;
	string spec;
	std::transform(specification.begin(), specification.end(), spec.begin(), std::tolower);
	try {
		while (index < spec.size()) {
			while ((spec[index] == ' ') && index < spec.size())
				index += 1;
			if (index == spec.size()) break;
			spec = spec.substr(index);
			index = 0;
			if (spec[0] == '+') {
				sign = 1;
				continue;
			}
			if (spec[0] == '-') {
				sign = -1;
				continue;
			}
			if (spec[0] != 'd') temp = stoi(spec, &sz);
			else temp = 1;
			if (spec[sz] == 'd') {
				counts.push_back(temp * sign);
				spec = spec.substr(sz + 1);
				temp = stoi(spec, &sz);
				dice.push_back(_ntod(temp));
				spec = spec.substr(sz);
			}
			else {
				modifier += (temp * sign);
				spec = spec.substr(sz);
			}
		}
	}
	catch (std::exception & ex) {
		throw new Exception(ExceptionCode::INVALID_DICE_STRING, P2EMonExceptionStrings[(int)(ExceptionCode::INVALID_DICE_STRING)] + ": \n" + ex.what());
	}
	return std::make_tuple(dice, counts, modifier);
}

DiceRoll::DiceRoll() {
	_dice = vector<Die>();
	_counts = vector<int>();
	_modifier = 0;
}

DiceRoll::DiceRoll(const string& spec) {
	std::tuple<vector<Die>, vector<int>, int> decoded = _storoll(spec);
	_dice = vector<Die>(std::get<0>(decoded));
	_counts = vector<int>(std::get<1>(decoded));
	_modifier = std::get<2>(decoded);
}

DiceRoll::DiceRoll(const Die& die, int count, int mod) {
	_dice = vector<Die>();
	_counts = vector<int>();
	_modifier = mod;
	_dice.push_back(die);
	_counts.push_back(count);
}

DiceRoll::DiceRoll(const DiceRoll& source) {
	_dice = vector<Die>(source._dice);
	_counts = vector<int>(source._counts);
	_modifier = source._modifier;
}

int DiceRoll::roll() const {
	int ret = 0;
	for (int i = 0; i < _dice.size(); i += 1)
		ret += _doroll(_dice[i], _counts[i]);
	ret += _modifier;
	return ret;
}

void DiceRoll::add(const Die& die, int count, int mod) {
	_modifier += mod;
	_dice.push_back(die);
	_counts.push_back(count);
}

void DiceRoll::add(const int& mod) {
	_modifier += mod;
}

void DiceRoll::add(const DiceRoll& source) {
	_modifier += source._modifier;
	_dice.insert(_dice.end(), source._dice.begin(), source._dice.end());
	_counts.insert(_counts.end(), source._counts.begin(), source._counts.end());
}

void DiceRoll::clear() {
	_dice.clear();
	_counts.clear();
	_modifier = 0;
}

void DiceRoll::add(const string& spec) {
	add(DiceRoll(spec));
}

void DiceRoll::set(const Die& die, int count, int mod) {
	clear();
	add(die, count, mod);
}

void DiceRoll::set(const string& spec) {
	clear();
	add(spec);
}

string DiceRoll::spec() const {
	char buff[BUFFER_SIZE];
	string ret = "";
	for (int i = 0; i < _dice.size(); i += 1) {
		if (i == 0) snprintf(&buff[0], BUFFER_SIZE, "%dd%d", _counts[i], _dton(_dice[i]));
		else snprintf(&buff[0], BUFFER_SIZE, "%+dd%d", _counts[i], _dton(_dice[i]));
		ret += buff;
	}
	snprintf(&buff[0], BUFFER_SIZE, "%+d", _modifier);
	ret += buff;
	return ret;
}

DiceRoll::operator string() const {
	return spec();
}

bool DiceRoll::operator==(const DiceRoll& rhs) const {
	return spec() == rhs.spec();
}

bool DiceRoll::operator==(const string& rhs) const {
	return spec() == rhs;
}

DiceRoll& DiceRoll::operator=(const DiceRoll& rhs) {
	set(rhs);
	return *this;
}

DiceRoll& DiceRoll::operator=(const string& rhs) {
	set(rhs);
	return *this;
}

DiceRoll DiceRoll::operator+(const DiceRoll& rhs) const {
	DiceRoll ret = DiceRoll(rhs);
	ret.add(*this);
	return ret;
}

DiceRoll DiceRoll::operator+(const int& mod) const {
	DiceRoll ret = DiceRoll(*this);
	ret.add(mod);
	return ret;
}

DiceRoll DiceRoll::operator+(const string& spec) const {
	DiceRoll ret = DiceRoll(*this);
	ret.add(spec);
	return ret;
}

DiceRoll& DiceRoll::operator+=(const DiceRoll& rhs) {
	add(rhs);
	return *this;
}

DiceRoll& DiceRoll::operator+=(const int& rhs) {
	add(rhs);
	return *this;
}

DiceRoll& DiceRoll::operator+=(const string& rhs) {
	add(rhs);
	return *this;
}