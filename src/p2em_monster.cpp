#include "p2em_core.hpp"
#include <ctime>
#include <cstdlib>

using namespace p2em_core;

const string Monster::TAGNAME = "p2mon";
const string Monster::NAME_TAGNAME = "name";
const string Monster::LEVEL_TAGNAME = "level";
const string Monster::RARITY_TAGNAME = "rarity";
const string Monster::ALIGNMENT_TAGNAME = "alignment";
const string Monster::SIZE_TAGNAME = "size";
const string Monster::TYPES_TAGNAME = "types";
const string Monster::TYPES_TYPE_TAGNAME = "type";
const string Monster::PERCEPTION_TAGNAME = "perception";
const string Monster::LANGUAGES_TAGNAME = "languages";
const string Monster::LANGUAGES_LANG_TAGNAME = "language";
const string Monster::SKILLS_TAGNAME = "skills";
const string Monster::SKILLS_SKILL_TAGNAME = "skill";
const string Monster::ABILITIES_TAGNAME = "abilities";
const string Monster::STRENGTH_TAGNAME = "strength";
const string Monster::DEXTERITY_TAGNAME = "dexterity";
const string Monster::CONSTITUTION_TAGNAME = "constitution";
const string Monster::INTELLIGENCE_TAGNAME = "intelligence";
const string Monster::WISDOM_TAGNAME = "wisdom";
const string Monster::CHARISMA_TAGNAME = "charisma";
const string Monster::ITEMS_TAGNAME = "items";
const string Monster::ITEMS_ITEM_TAGNAME = "item";
const string Monster::INTERACTIONS_TAGNAME = "interactions";
const string Monster::ARMOR_TAGNAME = "armor";
const string Monster::SAVES_TAGNAME = "saves";
const string Monster::SAVES_FORT_TAGNAME = "fortitude";
const string Monster::SAVES_REFL_TAGNAME = "reflex";
const string Monster::SAVES_WILL_TAGNAME = "will";
const string Monster::SAVES_MOD_TAGNAME = "modifier";
const string Monster::HITPOINTS_TAGNAME = "hitpoints";
const string Monster::IMMUNITIES_TAGNAME = "immunities";
const string Monster::IMMUNITIES_IMM_TAGNAME = "immunity";
const string Monster::WEAKNESSES_TAGNAME = "weaknesses";
const string Monster::WEAKNESSES_WEAK_TAGNAME = "weakness";
const string Monster::RESISTANCES_TAGNAME = "resistances";
const string Monster::RESISTANCES_RES_TAGNAME = "resistance";
const string Monster::OFFTURN_TAGNAME = "offturn";
const string Monster::ONTURN_TAGNAME = "onturn";
const string Monster::SPEEDS_TAGNAME = "speeds";
const string Monster::SPEEDS_UNIT_TAGNAME = "units";
const string Monster::SPEEDS_SPEED_TAGNAME = "speed";
const string Monster::ACTIONS_TAGNAME = "actions";
const string Monster::SPELLS_TAGNAME = "spells";

void Monster::_init(Core& core) {
	core.add(this);
	_core = &core;

	level = NumVal("Level");
	// rarity is a trait
	alignment = AlignmentAttr("Alignment");
	// Size?
	// Types?
	perception = CondMod("Perception");
	languages = vector<string>();
	skills = vector<CondMod>();
	strength = CondMod("Strength");
	dexterity = CondMod("Dexterity");
	constitution = CondMod("Constitution");
	intelligence = CondMod("Intelligence");
	wisdom = CondMod("Wisdom");
	charisma = CondMod("Charisma");
	items = vector<string>();
	interactions = vector<Feat>();
	armor = CondMod("Armor");
	fortitude = CondMod("Fortitude");
	reflex = CondMod("Reflex");
	will = CondMod("Will");
	saves = vector<CondModOpt>();
	maxhp = CondMod("Maximum Hitpoints"); maxhp.addDependency(*this); maxhp.addCallback(this, Monster::_update_hp);
	temphp = NumVal("Temporary Hitpoints"); temphp.addDependency(*this); temphp.addCallback(this, Monster::_update_hp);
	damage = NumVal("Damage Taken"); damage.addDependency(*this); damage.addCallback(this, Monster::_update_hp);
	_hitpoints = NumVal("Hitpoints"); maxhp.addDependency(*this); maxhp.addCallback(this, Monster::_update_hp);
	immunities = vector<string>();
	resistances = vector<CondMod>();
	weaknesses = vector<CondMod>();
	offturn_feats = vector<Feat>();
	speed_units = "feet";
	speeds = vector<CondMod>();
	actions = vector<Action>();
	spelllists = vector<SpellList>();
	onturn_feats = vector<Feat>();
}

void Monster::_update_hp(Updateable* mon) {
	Monster* monster = static_cast<Monster*>(mon);
	monster->_hitpoints = monster->maxhp + monster->temphp - monster->damage;
}

Monster::Monster(Core& core) : Object(core) { _init(core); }
Monster::Monster(Core& core, const string& name) : Object(core, name) { _init(core); }
Monster::Monster(Core& core, const uint& id) : Object(core, id) { _init(core); }
Monster::Monster(Core& core, const uint& id, const string& name) : Object(core, id, name) { _init(core); }
Monster::Monster(Core& core, const vector<string>& tags) : Object(core, tags) { _init(core); }
Monster::Monster(Core& core, const vector<string>& tags, const string& name) : Object(core, tags, name) { _init(core); }
Monster::Monster(Core& core, const vector<string>& tags, const uint& id) : Object(core, tags, id) { _init(core); }
Monster::Monster(Core& core, const vector<string>& tags, const uint& id, const string& name) : Object(core, tags, id, name) { _init(core); }
Monster::Monster(Core& core, const xml::Element* templ) : Object(core) { _init(core); parse(templ); }

Monster::Monster(const Monster& source) : Object(source) { _init(*source._core); }

Core& Monster::core() const {
	return *_core;
}

Registry& Monster::reg() const {
	return core();
}

int Monster::hp() const {
	return (int)(_hitpoints);
}

void Monster::parse(const xml::Element* templ) {
	try {
		string tmpval;
		int tmpint;
		name(templ->first_element(Monster::NAME_TAGNAME)->text());
		level = templ->first_element(Monster::LEVEL_TAGNAME)->text();
		rarity = templ->first_element(Monster::RARITY_TAGNAME)->text();
		alignment = templ->first_element(Monster::ALIGNMENT_TAGNAME)->text();
		size = Core::creatureSize(templ->first_element(Monster::SIZE_TAGNAME)->text());
		types = xml::node2strvec(templ->first_element(Monster::TYPES_TAGNAME), Monster::TYPES_TYPE_TAGNAME);
		if (types.size() == 0) throw new Exception(ExceptionCode::INVALID_VALUE, "Creature must be of at least one type!");

		((Traited*)(this))->parse(templ); // Parse remaining traits
		perception.parse(templ->first_element(Monster::PERCEPTION_TAGNAME));
		languages = vector<string>(xml::node2strvec(templ->first_element(Monster::LANGUAGES_TAGNAME), Monster::LANGUAGES_LANG_TAGNAME));

		vector<xml::Element*> skls = templ->first_element(Monster::SKILLS_TAGNAME)->child_elements(Monster::SKILLS_SKILL_TAGNAME);
		for (vector<xml::Element*>::iterator it = skls.begin(); it != skls.end(); it++)
			skills.push_back(CondMod(*it));

		strength.parse(templ->first_element(Monster::ABILITIES_TAGNAME)->first_element(Monster::STRENGTH_TAGNAME));
		dexterity.parse(templ->first_element(Monster::ABILITIES_TAGNAME)->first_element(Monster::DEXTERITY_TAGNAME));
		constitution.parse(templ->first_element(Monster::ABILITIES_TAGNAME)->first_element(Monster::CONSTITUTION_TAGNAME));
		intelligence.parse(templ->first_element(Monster::ABILITIES_TAGNAME)->first_element(Monster::INTELLIGENCE_TAGNAME));
		wisdom.parse(templ->first_element(Monster::ABILITIES_TAGNAME)->first_element(Monster::WISDOM_TAGNAME));
		charisma.parse(templ->first_element(Monster::ABILITIES_TAGNAME)->first_element(Monster::CHARISMA_TAGNAME));

		items = vector<string>(xml::node2strvec(templ->first_element(Monster::ITEMS_TAGNAME), Monster::ITEMS_ITEM_TAGNAME));

		vector<xml::Element*> intcs = templ->first_element(Monster::INTERACTIONS_TAGNAME)->child_elements(Feat::TAGNAME);
		for (vector<xml::Element*>::iterator it = intcs.begin(); it != intcs.end(); it++)
			interactions.push_back(Feat(*it));

		armor.parse(templ->first_element(Monster::ARMOR_TAGNAME));
		fortitude.parse(templ->first_element(Monster::SAVES_TAGNAME)->first_element(Monster::SAVES_FORT_TAGNAME));
		reflex.parse(templ->first_element(Monster::SAVES_TAGNAME)->first_element(Monster::SAVES_REFL_TAGNAME));
		will.parse(templ->first_element(Monster::SAVES_TAGNAME)->first_element(Monster::SAVES_WILL_TAGNAME));
		vector<xml::Element*> savemods = templ->first_element(Monster::SAVES_TAGNAME)->child_elements(Monster::SAVES_MOD_TAGNAME);
		for (vector<xml::Element*>::iterator it = savemods.begin(); it != savemods.end(); it++) {
			saves.push_back(CondModOpt());
			saves[saves.size() - 1] = (*it)->text();
			fortitude.addOption(saves[saves.size() - 1], false);
			reflex.addOption(saves[saves.size() - 1], false);
			will.addOption(saves[saves.size() - 1], false);
		}
		maxhp.parse(templ->first_element(Monster::HITPOINTS_TAGNAME));
		immunities = vector<string>(xml::node2strvec(templ->first_element(Monster::IMMUNITIES_TAGNAME), Monster::IMMUNITIES_IMM_TAGNAME));

		vector<xml::Element*> weaks = templ->first_element(Monster::WEAKNESSES_TAGNAME)->child_elements(Monster::WEAKNESSES_WEAK_TAGNAME);
		for (vector<xml::Element*>::iterator it = weaks.begin(); it != weaks.end(); it++)
			weaknesses.push_back(CondMod((*it)));

		vector<xml::Element*> reses = templ->first_element(Monster::RESISTANCES_TAGNAME)->child_elements(Monster::RESISTANCES_RES_TAGNAME);
		for (vector<xml::Element*>::iterator it = reses.begin(); it != reses.end(); it++)
			resistances.push_back(CondMod((*it)));

		vector<xml::Element*> oftn = templ->first_element(Monster::OFFTURN_TAGNAME)->child_elements(Feat::TAGNAME);
		for (vector<xml::Element*>::iterator it = oftn.begin(); it != oftn.end(); it++)
			offturn_feats.push_back(Feat(*it));

		speed_units = templ->first_element(Monster::SPEEDS_TAGNAME)->first_element(Monster::SPEEDS_UNIT_TAGNAME)->text();

		vector<xml::Element*> spds = templ->first_element(Monster::SPEEDS_TAGNAME)->child_elements(Monster::SPEEDS_SPEED_TAGNAME);
		for (vector<xml::Element*>::iterator it = spds.begin(); it != spds.end(); it++)
			speeds.push_back(CondMod((*it)));

		vector<xml::Element*> acts = templ->first_element(Monster::ACTIONS_TAGNAME)->child_elements(Action::TAGNAME);
		for (vector<xml::Element*>::iterator it = acts.begin(); it != acts.end(); it++)
			actions.push_back(Action(*it));

		vector<xml::Element*> spls = templ->first_element(Monster::SPELLS_TAGNAME)->child_elements(SpellList::TAGNAME);
		for (vector<xml::Element*>::iterator it = spls.begin(); it != spls.end(); it++)
			spelllists.push_back(SpellList(*it));

		vector<xml::Element*> ontn = templ->first_element(Monster::ONTURN_TAGNAME)->child_elements(Feat::TAGNAME);
		for (vector<xml::Element*>::iterator it = ontn.begin(); it != ontn.end(); it++)
			onturn_feats.push_back(Feat(*it));


	}
	catch (const xml::Exception& err) {
		string report = string(P2EMonExceptionStrings[(int)(ExceptionCode::TEMPLATE_READ_ERROR)]);
		report += "\n";
		report += string(err.message());
		std::cout << report;
	}
}