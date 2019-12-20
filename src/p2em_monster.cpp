#include "p2em_core.hpp"
#include <ctime>
#include <cstdlib>

#define ID_CREATE_TRYCOUNT_LIMIT	100

using namespace p2em_core;

void Monster::_init(Core& core) {
	core.add(this);
	_core = &core;

	level = NumVal("Level");
	alignment = AlignmentAttr("Alignment");
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
	maxhp = CondMod("Maximum Hitpoints"); maxhp.addDependency(*this); maxhp.addCallback(this, Monster::_update_hp);
	temphp = NumVal("Temporary Hitpoints"); temphp.addDependency(*this); temphp.addCallback(this, Monster::_update_hp);
	damage = NumVal("Damage Taken"); damage.addDependency(*this); damage.addCallback(this, Monster::_update_hp);
	_hitpoints = NumVal("Hitpoints"); maxhp.addDependency(*this); maxhp.addCallback(this, Monster::_update_hp);
	immunities = vector<string>();
	resistances = vector<CondMod>();
	weaknesses = vector<CondMod>();
	offturn_feats = vector<Feat>();
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