#include "p2em_core.hpp"
#include <ctime>
#include <cstdlib>

#define ID_CREATE_TRYCOUNT_LIMIT	100

using namespace p2em_core;

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
		name(templ->first_element("name")->text());
		level = templ->first_element("level")->text();
		addTrait(templ->first_element("rarity")->text());
		alignment = templ->first_element("alignment")->text();
		// Need to do size
		// Need to do types

		((Traited*)(this))->parse(templ); // Parse remaining traits
		perception.parse(templ->first_element("perception"));
		languages = vector<string>(xml::node2strvec(templ->first_element("languages"), "language"));

		vector<xml::Element*> skls = templ->first_element("skills")->child_elements("skill");
		for (vector<xml::Element*>::iterator it = skls.begin(); it != skls.end(); it++)
			skills.push_back(CondMod(*it));

		strength.parse(templ->first_element("abilities")->first_element("strength"));
		dexterity.parse(templ->first_element("abilities")->first_element("dexterity"));
		constitution.parse(templ->first_element("abilities")->first_element("constitution"));
		intelligence.parse(templ->first_element("abilities")->first_element("intelligence"));
		wisdom.parse(templ->first_element("abilities")->first_element("wisdom"));
		charisma.parse(templ->first_element("abilities")->first_element("charisma"));

		items = vector<string>(xml::node2strvec(templ->first_element("items"), "item"));

		vector<xml::Element*> intcs = templ->first_element("interactions")->child_elements("feat");
		for (vector<xml::Element*>::iterator it = intcs.begin(); it != intcs.end(); it++)
			interactions.push_back(Feat(*it));

		armor.parse(templ->first_element("armor"));
		fortitude.parse(templ->first_element("saves")->first_element("fortitude"));
		reflex.parse(templ->first_element("saves")->first_element("reflex"));
		will.parse(templ->first_element("saves")->first_element("will"));
		vector<xml::Element*> savemods = templ->first_element("saves")->child_elements("modifier");
		for (vector<xml::Element*>::iterator it = savemods.begin(); it != savemods.end(); it++) {
			saves.push_back(CondModOpt());
			saves[saves.size() - 1] = (*it)->text();
			fortitude.addOption(saves[saves.size() - 1], false);
			reflex.addOption(saves[saves.size() - 1], false);
			will.addOption(saves[saves.size() - 1], false);
		}
		maxhp.parse(templ->first_element("hitpoints"));
		immunities = vector<string>(xml::node2strvec(templ->first_element("immunities"), "immunity"));

		vector<xml::Element*> weaks = templ->first_element("weaknesses")->child_elements("weakness");
		for (vector<xml::Element*>::iterator it = weaks.begin(); it != weaks.end(); it++)
			weaknesses.push_back(CondMod((*it)));

		vector<xml::Element*> reses = templ->first_element("resistances")->child_elements("resistance");
		for (vector<xml::Element*>::iterator it = reses.begin(); it != reses.end(); it++)
			resistances.push_back(CondMod((*it)));

		vector<xml::Element*> oftn = templ->first_element("offturn")->child_elements("feat");
		for (vector<xml::Element*>::iterator it = oftn.begin(); it != oftn.end(); it++)
			offturn_feats.push_back(Feat(*it));

		speed_units = templ->first_element("speeds")->first_element("units")->text();

		vector<xml::Element*> spds = templ->first_element("speeds")->child_elements("speed");
		for (vector<xml::Element*>::iterator it = spds.begin(); it != spds.end(); it++)
			speeds.push_back(CondMod((*it)));

		// Do actions

		// Do spells

		vector<xml::Element*> ontn = templ->first_element("onturn")->child_elements("feat");
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