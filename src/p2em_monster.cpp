
#include "p2em_core.h"
#include <ctime>
#include <cstdlib>
#include "../src/p2em_numval.hpp"
#include "../src/p2em_condmod.hpp"

#define ID_CREATE_TRYCOUNT_LIMIT	100

using namespace p2em_core;

void Monster::_init(Core& core) {
	core.add(this);
	_core = &core;
	/*
	CondMod<int> perception;
	vector<string> languages;
	vector<CondMod<int>> skills;
	CondMod<int> strength;
	CondMod<int> dexterity;
	CondMod<int> constitution;
	CondMod<int> intelligence;
	CondMod<int> wisdom;
	CondMod<int> charisma;
	vector<string> items;
	vector<Feat> interactions;
	CondMod<int> armor;
	CondMod<int> fortitude;
	CondMod<int> reflex;
	CondMod<int> will;
	CondMod<int> hitpoints;
	vector<string> immunities;
	vector<CondMod<int>> weaknesses;
	vector<CondMod<int>> resistances;
	vector<Feat> offturn_feats;
	vector<CondMod<int>> speeds;
	// actions and spells
	vector<Feat> onturn_feats;
	*/

	level = NumVal<int>("Level");
	alignment = AlignmentAttr("Alignment");
	perception = CondMod<int>("Perception");
	languages = vector<string>();
	skills = vector<CondMod<int>>();
	strength = CondMod<int>("Strength");
	dexterity = CondMod<int>("Dexterity");
	constitution = CondMod<int>("Constitution");
	intelligence = CondMod<int>("Intelligence");
	wisdom = CondMod<int>("Wisdom");
	charisma = CondMod<int>("Charisma");
	items = vector<string>();
	interactions = vector<Feat>();
	armor = CondMod<int>("Armor");
	fortitude = CondMod<int>("Fortitude");
	reflex = CondMod<int>("Reflex");
	will = CondMod<int>("Will");
	maxhp = CondMod<int>("Maximum Hitpoints"); maxhp.addDependency(*this); maxhp.addCallback(this, Monster::_update_hp);
	temphp = NumVal<int>("Temporary Hitpoints"); temphp.addDependency(*this); temphp.addCallback(this, Monster::_update_hp);
	damage = NumVal<int>("Damage Taken"); damage.addDependency(*this); damage.addCallback(this, Monster::_update_hp);
	_hitpoints = NumVal<int>("Hitpoints"); maxhp.addDependency(*this); maxhp.addCallback(this, Monster::_update_hp);
	immunities = vector<string>();
	resistances = vector<CondMod<int>>();
	weaknesses = vector<CondMod<int>>();
	offturn_feats = vector<Feat>();
	speeds = vector<CondMod<int>>();
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
	return _hitpoints;
}