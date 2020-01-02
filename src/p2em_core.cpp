#include "p2em_core.hpp"
#include <stdlib.h>
#include <time.h>
#include <algorithm>

#define ID_CREATE_TRYCOUNT_LIMIT	100

using namespace p2em_core;

// --------------------------------------------------
// Exception Class ----------------------------------
// --------------------------------------------------

Exception::Exception(ExceptionCode excode, const string& msg) {
	_code = excode;
	_msg = msg;
}

Exception::Exception(ExceptionCode excode) : Exception::Exception(excode, P2EMonExceptionStrings[(int)(excode)]) {} ;

ExceptionCode Exception::code() const { return _code; }
string Exception::msg() const { return _msg; }

// --------------------------------------------------
// Core Class ---------------------------------------
// --------------------------------------------------

Core::Core() {
	umap<uint, Monster*> _monsters = { {Object::NULL_ID, nullptr} };
}

bool Core::has(uint id) const {
	return (_monsters.find(id) != _monsters.end());
}

bool Core::has(const Monster* mon) const {
	return has(mon->id());
}

bool Core::has(const string& name) const {
	for (umap<uint, Monster*>::const_iterator it = _monsters.begin(); it != _monsters.end(); it++) {
		if ((*it).second->name() == name) return true;
	}
	return false;
}

uint Core::newid() const {
	uint trycount = 0;
	uint id = 0;
	bool found = true;

	srand((unsigned int)(time(0)));

	// Try to find an unused ID
	while (found) {
		if (trycount++ >= ID_CREATE_TRYCOUNT_LIMIT) throw Exception(ExceptionCode::ID_CREATION_TIMEOUT);
		id = rand();
		found = has(id);
	}

	return id;
}

Monster* Core::get(uint id) {
	if (!has(id)) throw Exception(ExceptionCode::MONSTER_NOT_FOUND);
	return _monsters[id];
}

const Monster* Core::get(uint id) const {
	return static_cast<const Monster*>(const_cast<Core*>(this)->operator[](id));
}

Monster* Core::operator[](uint id) { return get(id); }

const Monster* Core::operator[](uint id) const { return get(id); }

void Core::add(Monster* mon) {
	// Check if monster is already in record
	if (has(mon->id())) {
		// If the monster is already in the record, do nothing; otherwise raise an exception
		if (_monsters[mon->id()] == mon) return;
		else throw Exception(ExceptionCode::DUPLICATE_MONSTER);
	}
	else {
		// Add the new monster
		_monsters.insert({ mon->id(), mon });
	}

	// Should not reach this
	throw Exception(ExceptionCode::ERROR);
}

void Core::remove(Monster* mon) {
	// Check if monster is already in record
	if (has(mon->id())) {
		// If the monster matches the record, remove it; otherwise raise an exception
		if (_monsters[mon->id()] == mon) _monsters.erase(mon->id());
		else throw Exception(ExceptionCode::MONSTER_MISMATCH);
	}
	else {
		// If the monster is not in the record, do nothing
		return;
	}

	// Should not reach this
	throw Exception(ExceptionCode::ERROR);
}

void Core::remove(uint id) {
	// If the monster is already in record, remove it. Otherwise do nothing
	if (has(id)) remove(_monsters[id]);
}

ActionCount Core::actionCount(const string& source) {
	string lower;
	std::transform(source.begin(), source.end(), lower.begin(), std::tolower);
	if (lower == "reaction") return ActionCount::REACTION;
	else if (lower == "passive") return ActionCount::PASSIVE;
	else if (lower == "free") return ActionCount::FREE;
	else if (lower == "1" || lower == "single") return ActionCount::SINGLE;
	else if (lower == "2" || lower == "double") return ActionCount::DOUBLE;
	else if (lower == "3" || lower == "triple") return ActionCount::TRIPLE;
	else throw new Exception(ExceptionCode::INVALID_VALUE, "Cannot parse '" + source + "' into an ActionCount!");
}

CreatureSize Core::creatureSize(const string& source) {
	string lower;
	std::transform(source.begin(), source.end(), lower.begin(), std::tolower);
	if (lower == "tiny") return CreatureSize::TINY;
	else if (lower == "small") return CreatureSize::SMALL;
	else if (lower == "medium") return CreatureSize::MEDIUM;
	else if (lower == "large") return CreatureSize::LARGE;
	else if (lower == "huge") return CreatureSize::HUGE;
	else if (lower == "gargantuan") return CreatureSize::GARGANTUAN;
	else throw new Exception(ExceptionCode::INVALID_VALUE, "Cannot parse \"" + source + "\" into a CreatureSize!");
}

// --------------------------------------------------
// Feat Class ---------------------------------------
// --------------------------------------------------

const string Feat::TAGNAME = "feat";
const string Feat::NAME_TAGNAME = "name";
const string Feat::COUNT_TAGNAME = "count";
const string Feat::DESCRIPTION_TAGNAME = "description";

Feat::Feat() : Traited() {}
Feat::Feat(const xml::Element* source) : Traited() { parse(source); }

void Feat::parse(const xml::Element* source) {
	name = source->first_element(Feat::NAME_TAGNAME)->text();
	description = source->first_element(Feat::DESCRIPTION_TAGNAME)->text();
	action_count = Core::actionCount(source->first_element(Feat::COUNT_TAGNAME)->text());
	((Traited*)(this))->parse(source);
}

// --------------------------------------------------
// DamageSpec Class ---------------------------------
// --------------------------------------------------

const string DamageSpec::TAGNAME = "damage";
const string DamageSpec::DAMAGE_TYPE_TAGNAME = "type";
const string DamageSpec::DAMAGE_DICE_TAGNAME = "dice";

DamageSpec::DamageSpec() {  }
DamageSpec::DamageSpec(const xml::Element* source) { parse(source); }

void DamageSpec::parse(const xml::Element* source) {
	if (!source->has_element(DamageSpec::DAMAGE_TYPE_TAGNAME))
		throw new Exception(ExceptionCode::INVALID_VALUE, "Damage XML node does not have a type element!");
	if (!source->has_element(DamageSpec::DAMAGE_DICE_TAGNAME))
		throw new Exception(ExceptionCode::INVALID_VALUE, "Damage XML node does not have a dice element!");

	type = source->first_element(DamageSpec::DAMAGE_TYPE_TAGNAME)->text();
	roll = source->first_element(DamageSpec::DAMAGE_DICE_TAGNAME)->text();
}

// --------------------------------------------------
// Action Class -------------------------------------
// --------------------------------------------------

const string Action::TAGNAME = "action";
const string Action::ACTION_TYPE_TAGNAME = "type";
const string Action::ACTION_COUNT_TAGNAME = "count";
const string Action::ACTION_NAME_TAGNAME = "name";
const string Action::ACTION_MODIFIER_TAGNAME = "modifier";
const string Action::ACTION_RESULTS_TAGNAME = "results";
const string Action::ACTION_RESULTS_EFFECT_TAGNAME = "effect";

Action::Action() : Traited() { modifier = 0; action_count = ActionCount::PASSIVE; }
Action::Action(const xml::Element* source) { parse(source); }

void Action::parse(const xml::Element* source) {
	if (!source->has_element(ACTION_TYPE_TAGNAME))
		throw new Exception(ExceptionCode::INVALID_VALUE, "Action XML node does not have a type element!");
	if (!source->has_element(ACTION_NAME_TAGNAME))
		throw new Exception(ExceptionCode::INVALID_VALUE, "Action XML node does not have a name element!");
	if (!source->has_element(ACTION_COUNT_TAGNAME))
		throw new Exception(ExceptionCode::INVALID_VALUE, "Action XML node does not have a count element!");
	if (!source->has_element(ACTION_RESULTS_TAGNAME))
		throw new Exception(ExceptionCode::INVALID_VALUE, "Action XML node does not have a results element!");

	type = source->first_element(ACTION_TYPE_TAGNAME)->text();
	name = source->first_element(ACTION_NAME_TAGNAME)->text();
	action_count = Core::actionCount(source->first_element(ACTION_COUNT_TAGNAME)->text());
	modifier = std::stoi(source->first_element(ACTION_MODIFIER_TAGNAME)->text());

	effects = xml::node2strvec(source->first_element(ACTION_RESULTS_TAGNAME), ACTION_RESULTS_EFFECT_TAGNAME);

	vector<xml::Element*> dmgs = source->first_element(ACTION_RESULTS_TAGNAME)->child_elements(DamageSpec::TAGNAME);
	for (vector<xml::Element*>::iterator it = dmgs.begin(); it != dmgs.end(); it++)
		damages.push_back(DamageSpec(*it));

	((Traited*)(this))->parse(source);
}

// --------------------------------------------------
// SpellDesc Class ----------------------------------
// --------------------------------------------------

const string SpellDesc::TAGNAME = "spell";
const string SpellDesc::NAME_TAGNAME = "name";
const string SpellDesc::SLOTS_TAGNAME = "slots";
const string SpellDesc::COMMENT_TAGNAME = "comment";
const string SpellDesc::COUNT_AT_WILL = "at will";
const string SpellDesc::COMMENT_NULL = "__NULL__";
const int SpellDesc::ICOUNT_AT_WILL = -1;
const int SpellDesc::ICOUNT_NULL = -2;

SpellDesc::SpellDesc() {  }
SpellDesc::SpellDesc(const xml::Element* source) { parse(source); }

void SpellDesc::parse(const xml::Element* source) {
	if (!source->has_element(SpellDesc::NAME_TAGNAME))
		throw new Exception(ExceptionCode::INVALID_VALUE, "Spell description XML node does not have a name element!");

	name = source->first_element(SpellDesc::NAME_TAGNAME)->text();
	if (source->has_element(SpellDesc::SLOTS_TAGNAME)) {
		string slotspec = source->first_element(SpellDesc::SLOTS_TAGNAME)->text();
		std::transform(slotspec.begin(), slotspec.end(), slotspec.end(), std::tolower);
		if (slotspec == SpellDesc::COUNT_AT_WILL) slotcount = SpellDesc::ICOUNT_AT_WILL;
		else {
			try {
				int tmpcount = std::stoi(slotspec);
				if (tmpcount < 1)
					throw new Exception(ExceptionCode::INVALID_VALUE, "Spell description slot count must be a positive integer, not \"" +
						slotspec + "\"");
				slotcount = tmpcount;
			}
			catch (const std::out_of_range & oor) {
				throw new Exception(ExceptionCode::INVALID_VALUE, "Spell description slot count must be a positive integer, not \"" +
					slotspec + "\"");
			}
			catch (const std::invalid_argument & ia) {
				throw new Exception(ExceptionCode::INVALID_VALUE, "Spell description slot count must be either \"" + 
					SpellDesc::COUNT_AT_WILL + "\" or a positive integer, not \"" + slotspec + "\"");
			}
		}
	}
	else slotcount = SpellDesc::ICOUNT_NULL;

	if (source->has_element(SpellDesc::COMMENT_TAGNAME)) comment = source->first_element(SpellDesc::COMMENT_TAGNAME)->text();
	else comment = SpellDesc::COMMENT_NULL;
}

// --------------------------------------------------
// SpellLevelSummary Class --------------------------
// --------------------------------------------------

const string SpellLevelSummary::TAGNAME = "group";
const string SpellLevelSummary::LEVEL_TAGNAME = "level";
const string SpellLevelSummary::HEIGHTENING_TAGNAME = "height";
const string SpellLevelSummary::SLOTS_TAGNAME = "slots";
const string SpellLevelSummary::LEVEL_CANTRIP = "cantrip";
const string SpellLevelSummary::LEVEL_CONTINU = "cantinuous";
const string SpellLevelSummary::COUNT_AT_WILL = "at will";
const int SpellLevelSummary::ILEVEL_CANTRIP = 0;
const int SpellLevelSummary::ILEVEL_CONTINU = -1;
const int SpellLevelSummary::IHEIGHT_NULL = -1;
const int SpellLevelSummary::ICOUNT_AT_WILL = -1;
const int SpellLevelSummary::ICOUNT_NULL = -1;

SpellLevelSummary::SpellLevelSummary() {  }
SpellLevelSummary::SpellLevelSummary(const xml::Element* source) { parse(source); }

void SpellLevelSummary::parse(const xml::Element* source) {
	if (!source->has_element(SpellLevelSummary::LEVEL_TAGNAME))
		throw new Exception(ExceptionCode::INVALID_VALUE, "Spell group XML node does not have a level element!");
	if (!source->has_element(SpellDesc::TAGNAME))
		throw new Exception(ExceptionCode::INVALID_VALUE, "Spell group XML node does not have any spell elements!");

	string levelspec = source->first_element(SpellLevelSummary::LEVEL_TAGNAME)->text();
	std::transform(levelspec.begin(), levelspec.end(), levelspec.begin(), std::tolower);
	if (levelspec == SpellLevelSummary::LEVEL_CANTRIP) level = SpellLevelSummary::ILEVEL_CANTRIP;
	else if (levelspec == SpellLevelSummary::LEVEL_CONTINU) level = SpellLevelSummary::ILEVEL_CONTINU;
	else {
		try {
			int tmplevel = std::stoi(levelspec);
			if ((tmplevel < 1) || (tmplevel > 10))
				throw new Exception(ExceptionCode::INVALID_VALUE, "Spell group level must be between 1 and 10 (inclusive), not " + levelspec);
			level = tmplevel;
		}
		catch (const std::out_of_range & oor) {
			throw new Exception(ExceptionCode::INVALID_VALUE, "Spell group level must be between 1 and 10 (inclusive), not " + levelspec);
		}
		catch (const std::invalid_argument& ia) {
			throw new Exception(ExceptionCode::INVALID_VALUE, "Spell group level \"" + levelspec + "\" is not a valid value. \
Must be \"" + SpellLevelSummary::LEVEL_CANTRIP + "\", \"" + SpellLevelSummary::LEVEL_CONTINU + "\" or an integer between 1 and 10.");
		}
	}

	if (source->has_element(SpellLevelSummary::HEIGHTENING_TAGNAME)) {
		levelspec = source->first_element(SpellLevelSummary::HEIGHTENING_TAGNAME)->text();
		try {
			int tmplevel = std::stoi(levelspec);
			if ((tmplevel < 1) || (tmplevel > 10))
				throw new Exception(ExceptionCode::INVALID_VALUE, "Spell group height must be between 1 and 10 (inclusive), not " + levelspec);
			heightening = tmplevel;
		}
		catch (const std::out_of_range & oor) {
			throw new Exception(ExceptionCode::INVALID_VALUE, "Spell group height must be between 1 and 10 (inclusive), not " + levelspec);
		}
		catch (const std::invalid_argument & ia) {
			throw new Exception(ExceptionCode::INVALID_VALUE, "Spell group height \"" + levelspec + "\" is not a valid value. \
Must be an integer between 1 and 10.");
		}
	}
	else heightening = SpellLevelSummary::IHEIGHT_NULL;

	if (source->has_element(SpellLevelSummary::SLOTS_TAGNAME)) {
		string countspec = source->first_element(SpellLevelSummary::SLOTS_TAGNAME)->text();
		std::transform(countspec.begin(), countspec.end(), countspec.begin(), std::tolower);
		if (countspec == SpellLevelSummary::COUNT_AT_WILL) slotcount = SpellLevelSummary::ICOUNT_AT_WILL;
		else {
			try {
				int tmpcount = std::stoi(countspec);
				if (tmpcount < 1)
					throw new Exception(ExceptionCode::INVALID_VALUE, "Spell group slot count must be positive integer, not "
						+ countspec);
			}
			catch (const std::out_of_range& oor) {
				throw new Exception(ExceptionCode::INVALID_VALUE, "Spell group slot count outside range of integer: " + countspec);
			}
			catch (const std::invalid_argument & ia) {
				throw new Exception(ExceptionCode::INVALID_VALUE, "Spell group slot count \"" + countspec + "\" must be either \"" +
					SpellLevelSummary::COUNT_AT_WILL + "\" or a positive integer");
			}
		}
	}
	else slotcount = SpellLevelSummary::ICOUNT_NULL;

	vector<xml::Element*> spls = source->child_elements(SpellDesc::TAGNAME);
	for (vector<xml::Element*>::iterator it = spls.begin(); it != spls.end(); it++)
		spells.push_back(SpellDesc(*it));
}

// --------------------------------------------------
// SpellList Class ----------------------------------
// --------------------------------------------------

const string SpellList::TAGNAME = "spelllist";
const string SpellList::TRADITION_TAGNAME = "tradition";
const string SpellList::TYPE_TAGNAME = "type";
const string SpellList::DC_TAGNAME = "difficulty";
const string SpellList::ATK_TAGNAME = "attack";
const int SpellList::IATK_NULL = 0;

SpellTradition SpellList::s2trad(const string& _stype) {
	string stype;
	std::transform(_stype.begin(), _stype.end(), stype.begin(), std::tolower);
	if (stype == "arcane") return SpellTradition::ARCANE;
	else if (stype == "divine") return SpellTradition::DIVINE;
	else if (stype == "occult") return SpellTradition::OCCULT;
	else if (stype == "primal") return SpellTradition::PRIMAL;
	else throw new Exception(ExceptionCode::INVALID_VALUE, "Cannot translate \"" + _stype + "\" into a spell tradition");
}

string SpellList::trad2s(const SpellTradition& trad) {
	switch (trad) {
	case SpellTradition::ARCANE:
		return "arcane";
	case SpellTradition::DIVINE:
		return "divine";
	case SpellTradition::OCCULT:
		return "occult";
	case SpellTradition::PRIMAL:
		return "primal";
	default:
		throw new Exception(ExceptionCode::INVALID_VALUE, "Encountered a spell tradition that is not supported!");
	}
}

SpellCastingType SpellList::s2type(const string& _stype) {
	string stype;
	std::transform(_stype.begin(), _stype.end(), stype.begin(), std::tolower);
	if (stype == "prepared") return SpellCastingType::PREPARED;
	else if (stype == "spontaneous") return SpellCastingType::SPONTANEOUS;
	else if (stype == "innate") return SpellCastingType::INNATE;
	else if (stype == "ritual") return SpellCastingType::RITUAL;
	else if (stype == "focus") return SpellCastingType::FOCUS;
	else throw new Exception(ExceptionCode::INVALID_VALUE, "Cannot translate \"" + _stype + "\" into a spell casting type");
}

string SpellList::type2s(const SpellCastingType& sctype) {
	switch (sctype) {
	case SpellCastingType::FOCUS:
		return "focus";
	case SpellCastingType::INNATE:
		return "innate";
	case SpellCastingType::PREPARED:
		return "prepared";
	case SpellCastingType::SPONTANEOUS:
		return "spontaneous";
	case SpellCastingType::RITUAL:
		return "ritual";
	default:
		throw new Exception(ExceptionCode::INVALID_VALUE, "Encountered an unrecognized spell casting type!");
	}
}

SpellList::SpellList() {  }
SpellList::SpellList(const xml::Element* source) { parse(source); }

void SpellList::parse(const xml::Element* source) {
	if (!source->has_element(SpellList::TRADITION_TAGNAME))
		throw new Exception(ExceptionCode::INVALID_VALUE, "Spell list XML node does not have tradition element!");
	if (!source->has_element(SpellList::TYPE_TAGNAME))
		throw new Exception(ExceptionCode::INVALID_VALUE, "Spell list XML node does not have spell casting type element!");
	if (!source->has_element(SpellList::DC_TAGNAME))
		throw new Exception(ExceptionCode::INVALID_VALUE, "Spell list XML node does not have spell DC element!");
	if (!source->has_element(SpellLevelSummary::TAGNAME))
		throw new Exception(ExceptionCode::INVALID_VALUE, "Spell list XML node does not have spell elements!");

	tradition = SpellList::s2trad(source->first_element(SpellList::TRADITION_TAGNAME)->text());
	type = SpellList::s2type(source->first_element(SpellList::TYPE_TAGNAME)->text());
	string dcspec = source->first_element(SpellList::DC_TAGNAME)->text();
	try {
		int tmpdc = std::stoi(dcspec);
		if (tmpdc < 1)
			throw new Exception(ExceptionCode::INVALID_VALUE, "Spell list DC must be a positive integer, not \"" + dcspec + "\"");
		difficulty = tmpdc;
	}
	catch (const std::out_of_range & oor) {
		throw new Exception(ExceptionCode::INVALID_VALUE, "Spell list DC must be a positive integer, not \"" + dcspec + "\"");
	}
	catch (const std::invalid_argument & ia) {
		throw new Exception(ExceptionCode::INVALID_VALUE, "Spell list DC must be a positive integer, not \"" + dcspec + "\"");
	}

	if (source->has_element(SpellList::ATK_TAGNAME)) {
		has_attack = true;
		attack.parse(source->first_element(SpellList::ATK_TAGNAME));
	}
	else {
		has_attack = false;
		attack = CondMod();
		attack.base((double)(SpellList::IATK_NULL));
	}

	vector<xml::Element*> grps = source->child_elements(SpellLevelSummary::TAGNAME);
	for (vector<xml::Element*>::iterator it = grps.begin(); it != grps.end(); it++)
		levels.push_back(SpellLevelSummary(*it));
}