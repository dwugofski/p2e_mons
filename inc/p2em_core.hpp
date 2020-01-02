#pragma once
/*
 * Pathfinder 2E Monster Core C++ Library
 */

#ifndef __P2EM_CORE_H__
#define __P2EM_CORE_H__

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <stdint.h>
#include <exception>
#include <tuple>

#include "p2em_xml/p2em_xml.hpp"

#define STRING_BUFFER_SIZE 256

namespace p2em_core {

	namespace xml = p2em_xml;

	using string = std::string;
	template<typename T_unit_t> using vector = std::vector<T_unit_t>;
	template<typename T_key_t, typename T_val_t> using omap = std::map<T_key_t, T_val_t>;
	template<typename T_key_t, typename T_val_t> using umap = std::unordered_map<T_key_t, T_val_t>;
	typedef unsigned int uint;

	template<typename T_valunit_t>
	struct modvalT {
		string name;
		T_valunit_t base;
		T_valunit_t mod;
	};
	using modval = modvalT<uint>;

	enum class AttrType {
		NUMERIC,
		STRING,

		UNKNOWN,
	};

	enum class ExceptionCode {
		INVALID_ID,
		INVALID_NAME,
		DUPLICATE_MONSTER,
		MONSTER_NOT_FOUND,
		MONSTER_MISMATCH,
		ATTR_TYPE_MISMATCH,
		ID_CREATION_TIMEOUT,
		CIRCULAR_DEPS,
		INVALID_VALUE,
		INVALID_DICE_STRING,
		TEMPLATE_READ_ERROR,

		ERROR
	};

	const string P2EMonExceptionStrings[static_cast<int>(ExceptionCode::ERROR) + 1] = {
		"Invalid monster name received!",
		"Invalid monster id received!",
		"Cannot create duplicate monsters!",
		"Cannot find monster!",
		"Monster id does not match recorded id!",
		"Attribute value mismatch!",
		"Could not create new monster ID!",
		"Could not add dependency: Dependency lists must not be circular!",
		"Cannot set attribute to provided value!",
		"Roll specification string does not match proper format!",
		"Error reading monster template",

		"Monster error!"
	};

	enum class Alignment {
		CHAOTIC_EVIL,
		CHAOTIC_GOOD,
		CHAOTIC_NEUTRAL,
		LAWFUL_EVIL,
		LAWFUL_GOOD,
		LAWFUL_NEUTRAL,
		NEUTRAL,
		NEUTRAL_EVIL,
		NEUTRAL_GOOD,

		UNALIGNED
	};

	enum class CreatureSize {
		TINY,
		SMALL,
		MEDIUM,
		LARGE,
		HUGE,
		GARGANTUAN
	};

	enum class ActionCount {
		FREE,
		SINGLE,
		DOUBLE,
		TRIPLE,
		REACTION,

		PASSIVE
	};

	enum class Die {
		TWO,
		THREE,
		FOUR,
		SIX,
		EIGHT,
		TEN,
		TWELVE,
		TWENTY,
		HUNDRED
	};

	enum class SpellTradition {
		ARCANE,
		DIVINE,
		OCCULT,
		PRIMAL,
	};

	enum class SpellCastingType {
		PREPARED,
		SPONTANEOUS,
		INNATE,
		RITUAL,
		FOCUS
	};

	class Core;
	class Exception;
	class Attribute;
	class Updateable;
	class Traited;
	class Object;
	class Monster;

	struct Registry {
		virtual bool has(uint id) const = 0;
		virtual bool has(const string& name) const = 0;
		virtual uint newid() const = 0;
	};

	class Core : public Registry {
	protected:
		umap<uint, Monster*> _monsters;
	public:
		Core();

		bool has(uint id) const override;
		virtual bool has(const string& name) const override;
		uint newid() const override;
		bool has(const Monster* monster) const;
		const Monster* get(uint id) const;

		void add(Monster* monster);
		void remove(Monster* monster);
		void remove(uint id);
		Monster* get(uint id);

		const Monster* operator[](uint id) const;
		Monster* operator[](uint id);

		static ActionCount actionCount(const string& source);
		static CreatureSize creatureSize(const string& source);
	};

	// Located in p2em_core.cpp
	class Exception : public std::exception {
	private:
		string _msg;
		ExceptionCode _code;
	public:
		Exception(ExceptionCode excode);
		Exception(ExceptionCode excode, const string& msg);

		ExceptionCode code() const;
		string msg() const;
	};

	class Traited {
	protected:
		vector<string> _traits;
	public:
		Traited();
		Traited(const vector<string>& startings);
		Traited(const Traited& source);

		bool hasTrait(const string& tagnam) const;
		void addTrait(const string& newtag);
		void removeTrait(const string& oldtag);

		void parse(const xml::Element* source);
	};

	class Updateable {
	protected:
		vector<Updateable*> _dependencies;
		vector<Updateable*> _parents;
		vector<std::tuple<Updateable*, void (*)(Updateable*)>> _callbacks;
		bool _paused = false;
	public:
		Updateable();
		Updateable(const Updateable& source);
		~Updateable();

		vector<Updateable*> dependents() const;
		vector<Updateable*> parents() const;
		vector<std::tuple<Updateable*, void (*)(Updateable*)>> callbacks() const;
		bool hasDependency(const Updateable& dep, bool recurse) const;
		void addDependency(Updateable& dep);
		void removeDependency(Updateable& dep);
		bool hasParent(const Updateable& parent) const;
		void addParent(Updateable& parent);
		virtual void removeParent(Updateable& parent);
		bool hasCallback(const Updateable* src, void(*callback)(Updateable*)) const;
		void addCallback(Updateable* src, void(*callback)(Updateable*));
		void removeCallback(const Updateable* src, void(*callback)(Updateable*));
		void pause();
		void unpause();
		bool paused();
		void update(bool updatedeps=true, bool triggering=true, bool internalupdate=true);
		virtual void onupdate();
	};

	class Attribute : public Updateable, public Traited {
	protected:
		string _name;
	public:
		Attribute();
		Attribute(const string& name);
		Attribute(const Attribute& source);

		virtual string name() const;
		virtual void name(const string& name);

		virtual AttrType attrType() const = 0;

		virtual string toString() const = 0;
		virtual bool validate(const string& source) const = 0; // Determines whether a string will behave well. 

		// virtual Attribute& operator=(const Attribute& source);
		// virtual Attribute& operator=(const string& source) = 0; // Should support an empty string as "reset to default," can produce Exceptions (P2EMON_ERR_INVALID_VALUE)
	};

	class NumVal : public Attribute {
	protected:
		double _value;
	public:
		NumVal();
		NumVal(const double& value);
		NumVal(const string& name);
		NumVal(const string& name, const double& value);
		NumVal(const NumVal& source);
		// NumVal(const Attribute& source);

		virtual AttrType attrType() const override;
		virtual string toString() const override;

		// Core functions for value change (do not access _value directly outside of this)
		virtual double value() const;
		virtual void value(const double& nuval);
		// --end

		virtual bool validate(const string& value) const override;

		virtual NumVal& operator=(const NumVal& source);
		virtual NumVal& operator=(const double& value);
		virtual NumVal& operator=(const string& value);

		virtual operator double() const;

		virtual bool operator==(const NumVal& comp) const;
		virtual bool operator==(const double& comp) const;
		virtual double operator+(const double& rhs);
		virtual double operator+(const NumVal& rhs);
		virtual NumVal& operator+=(const double& rhs);
		virtual NumVal& operator+=(const NumVal& rhs);
		virtual double operator-(const double& rhs);
		virtual double operator-(const NumVal& rhs);
		virtual NumVal& operator-=(const double& rhs);
		virtual NumVal& operator-=(const NumVal& rhs);
		virtual double operator*(const double& rhs);
		virtual double operator*(const NumVal& rhs);
		virtual NumVal& operator*=(const double& rhs);
		virtual NumVal& operator*=(const NumVal& rhs);
		virtual double operator/(const double& rhs);
		virtual double operator/(const NumVal& rhs);
		virtual NumVal& operator/=(const double& rhs);
		virtual NumVal& operator/=(const NumVal& rhs);
	};

	class OverrideableNumVal : public NumVal {
	protected:
		bool _override;
	public:
		OverrideableNumVal();
		OverrideableNumVal(const string& name);
		OverrideableNumVal(const double& value);
		OverrideableNumVal(const string& name, const double& value);

		// Core functions for value change (do not access _value, _override directly outside of this)
		// virtual T_num_t value() const; // No reason to change from NumVal.value()
		virtual void value(const double& newval) override;
		virtual bool overridden() const;
		virtual void set_override(bool new_override);
		// --end

		virtual void toggle_override();
	};

	class ModVal : public OverrideableNumVal {
	protected:
		NumVal& _base;
		NumVal& _offset;
		bool _ownsbase;
		bool _ownsoffset;

		void _link(); // Called by constructor functions
		vector<double> _splitstring(const string& newval) const;
	public:
		static const char VALUE_DELIMITER;

		ModVal();
		ModVal(const string& name);
		ModVal(const double& value);
		ModVal(const string& name, const double& value);
		ModVal(NumVal& base);
		ModVal(const string& name, NumVal& base);
		ModVal(NumVal& base, const double& val);
		ModVal(const string& name, NumVal& base, const double& val);
		ModVal(NumVal& base, NumVal& offset);
		ModVal(const string& name, NumVal& base, NumVal& offset);
		ModVal(NumVal& base, NumVal& offset, const double& value);
		ModVal(const string& name, NumVal& base, NumVal& offset, const double& value);
		ModVal(const ModVal& source);
		~ModVal();

		virtual string toString() const override;

		// Core functions for value change (do not access _value, _base, _offset, _override directly outside of this)
		virtual double value() const override;
		// virtual void value(const T_num_t& val); // Handled by OverrideableNumVal.value(const T_num_t&)
		virtual double base() const;
		virtual void base(const double& base);
		virtual double offset() const;
		virtual void offset(const double& offset);
		virtual void onupdate() override;
		// --end

		virtual bool validate(const string& value) const override;

		virtual ModVal& operator=(const ModVal& source);
		virtual ModVal& operator=(const string& source);

		virtual bool operator==(const ModVal& val2) const;
	};

	// May want to consider making this more generic...
	class AggVal : public OverrideableNumVal {
	protected:
		vector<NumVal*> _values;
	public:
		AggVal();
		AggVal(const string& name);
		AggVal(const double& baseval);
		AggVal(const string& name, const double& baseval);
		AggVal(NumVal& startingval);
		AggVal(const string& name, NumVal& startingval);
		AggVal(NumVal& startingval, const double& baseval);
		AggVal(const string& name, NumVal& startingval, const double& baseval);
		AggVal(const vector<NumVal*>& startingvals);
		AggVal(const string& name, const vector<NumVal*>& startingvals);
		AggVal(const vector<NumVal*>& startingvals, const double& baseval);
		AggVal(const string& name, const vector<NumVal*>& startingvals, const double& baseval);

		// Core functions for value change (do not access _value, _values directly outside of this)
		virtual vector<const NumVal*> items() const;
		virtual vector<NumVal*> items();
		virtual bool hasItem(const NumVal& item) const;
		virtual void addItem(NumVal& newitem);
		virtual void removeItem(NumVal& olditem);
		virtual void removeParent(Updateable& oldparent) override; // Necessary, as items (parents) need to be removed from the _values list as well
		// --end
	};

	class SumVal : public AggVal {
	public:
		SumVal();
		SumVal(const string& name);
		SumVal(const double& baseval);
		SumVal(const string& name, const double& baseval);
		SumVal(NumVal& startingval);
		SumVal(const string& name, NumVal& startingval);
		SumVal(NumVal& startingval, const double& baseval);
		SumVal(const string& name, NumVal& startingval, const double& baseval);
		SumVal(const vector<NumVal*>& startingvals);
		SumVal(const string& name, const vector<NumVal*>& startingvals);
		SumVal(const vector<NumVal*>& startingvals, const double& baseval);
		SumVal(const string& name, const vector<NumVal*>& startingvals, const double& baseval);

		// Core functions for value change (do not access _value, _values directly outside of this)
		virtual double value() const override;
		// virtual void value(const T_num_t& newvalue); // Call the OverrideableNumVal.value(const T_num_t&) virtual
		// --end
	};

	class MaxVal : public AggVal {
	public:
		MaxVal();
		MaxVal(const string& name);
		MaxVal(const double& baseval);
		MaxVal(const string& name, const double& baseval);
		MaxVal(NumVal& startingval);
		MaxVal(const string& name, NumVal& startingval);
		MaxVal(NumVal& startingval, const double& baseval);
		MaxVal(const string& name, NumVal& startingval, const double& baseval);
		MaxVal(const vector<NumVal*>& startingvals);
		MaxVal(const string& name, const vector<NumVal*>& startingvals);
		MaxVal(const vector<NumVal*>& startingvals, const double& baseval);
		MaxVal(const string& name, const vector<NumVal*>& startingvals, const double& baseval);

		// Core functions for value change (do not access _value, _values directly outside of this)
		virtual double value() const override;
		// virtual void value(const T_num_t& newvalue); // Call the OverrideableNumVal.value(const T_num_t&) virtual
		// --end
	};

	class MinVal : public AggVal {
	public:
		MinVal();
		MinVal(const string& name);
		MinVal(const double& baseval);
		MinVal(const string& name, const double& baseval);
		MinVal(NumVal& startingval);
		MinVal(const string& name, NumVal& startingval);
		MinVal(NumVal& startingval, const double& baseval);
		MinVal(const string& name, NumVal& startingval, const double& baseval);
		MinVal(const vector<NumVal*>& startingvals);
		MinVal(const string& name, const vector<NumVal*>& startingvals);
		MinVal(const vector<NumVal*>& startingvals, const double& baseval);
		MinVal(const string& name, const vector<NumVal*>& startingvals, const double& baseval);

		// Core functions for value change (do not access _value, _values directly outside of this)
		virtual double value() const override;
		// virtual void value(const double& newvalue); // Call the OverrideableNumVal.value(const double&) virtual
		// --end
	};

	class StrAttr : public Attribute {
	protected:
		string _value;
	public:
		StrAttr();
		StrAttr(const string& value);
		StrAttr(const string& name, const string& value);
		StrAttr(const StrAttr& source);

		virtual AttrType attrType() const override;

		// Core functions for value change (do not access _value directly outside of this)
		virtual string value() const;
		virtual void value(const string& newval);
		// --end

		virtual string toString() const override;
		virtual bool validate(const string& source) const override; // Determines whether a string will behave well.

		virtual bool operator==(const string& rhs) const;
		virtual bool operator==(const StrAttr& rhs) const;
		virtual string operator+(const string& rhs) const;
		virtual string operator+(const StrAttr& rhs) const;
		virtual StrAttr& operator=(const string& source); // Should support an empty string as "reset to default," can produce Exceptions (P2EMON_ERR_INVALID_VALUE)
		virtual StrAttr& operator=(const StrAttr& source);
		virtual StrAttr& operator+=(const string& rhs);
		virtual StrAttr& operator+=(const StrAttr& rhs);
		virtual operator string() const;

		virtual Attribute& operator=(const Attribute& source);
	};

	class AlignmentAttr : public StrAttr {
	protected:
		Alignment _alignment;
	public:
		AlignmentAttr();
		AlignmentAttr(const string& name);
		AlignmentAttr(const Alignment& value);
		AlignmentAttr(const string& name, const Alignment& value);
		AlignmentAttr(const string& name, const string& align_str);
		AlignmentAttr(const AlignmentAttr& source);

		// Core functions for value change (do not access _value, _alignment directly outside of this)
		virtual Alignment avalue() const;
		virtual string value() const override;
		virtual void value(const string& newval) override;
		virtual void value(const Alignment& newval);
		// --end

		// virtual string toString() const; // No need to replace StrAttr.toString()
		virtual bool validate(const string& source) const override; // Determines whether a string will behave well.
	};

	struct CondModOpt : public NumVal {
		string description;
		bool selected;

		CondModOpt();
		CondModOpt(const double& val);
		CondModOpt(const string& desc);
		CondModOpt(const double& val, const string& desc);
		CondModOpt(const bool& sel);
		CondModOpt(const double& val, const bool& sel);
		CondModOpt(const string& desc, const bool& sel);
		CondModOpt(const double& val, const string& desc, const bool& sel);
		CondModOpt(const CondModOpt& source);

		virtual CondModOpt& operator=(const string& rhs);
		virtual CondModOpt& operator=(const bool& rhs);
		virtual CondModOpt& operator=(const CondModOpt& rhs);
		virtual bool operator==(const string& rhs) const;
		virtual bool operator==(const bool& rhs) const;
		virtual bool operator==(const CondModOpt& rhs) const;

		virtual operator string() const;
		virtual operator bool() const;
	};

	class CondMod : public ModVal {
	private:
		void _init();
	protected:
		SumVal _agger;
		vector<CondModOpt*> _options;
		vector<bool> _ownership;
	public:
		CondMod();
		CondMod(const string& name);
		CondMod(NumVal& base);
		CondMod(const string& name, NumVal& base);
		CondMod(const double& value);
		CondMod(const string& name, const double& value);
		CondMod(NumVal& base, const double& value);
		CondMod(const string& name, NumVal& base, const double& value);
		CondMod(const xml::Element* source);
		CondMod(const CondMod& source);
		~CondMod();

		// Core functions for value change (do not access _value, _alignment directly outside of this)
		// virtual double value() const; // ModVal.value() should be good if the updating of ModVal._offset is proper
		// virtual void value(const double& value); // The ModVal.value(...) should be good
		// virtual double base() const;
		// virtual void base(const double& newbase);
		// virtual double offset() const;
		// virtual void offset(const NumVal& newoffset);
		virtual bool hasOption(const CondModOpt& testval) const;
		virtual int addOption(CondModOpt& newvalue, bool own = false);
		virtual int createOption(double val = 0, string desc = "", bool selected = false);
		virtual int createOption(const CondModOpt& source);
		virtual void removeOption(const int& index);
		virtual void removeOption(const CondModOpt& oldvalue);
		virtual vector<const CondModOpt*> options() const;
		virtual vector<CondModOpt*> options();
		virtual int indexOf(const CondModOpt& value) const;
		virtual uint size() const;
		// --end
		
		virtual const CondModOpt& operator[](const int& index) const;
		virtual CondModOpt& operator[](const int& index);

		void parse(const xml::Element* source);
	};

	// Located in p2em_core.cpp
	class Feat : public Traited {
	public:
		static const string TAGNAME;
		static const string NAME_TAGNAME;
		static const string COUNT_TAGNAME;
		static const string DESCRIPTION_TAGNAME;

		string name;
		ActionCount action_count;
		string description;

		Feat();
		Feat(const xml::Element* source);

		void parse(const xml::Element* source);
	};

	class DiceRoll {
	private:
		vector<Die> _dice;
		vector<int> _counts;
		int _modifier;
	public:
		DiceRoll();
		DiceRoll(const string& specification);
		DiceRoll(const Die& die, int count = 1, int mod = 0);
		DiceRoll(const DiceRoll& source);

		int roll() const;
		void add(const Die& dice, int count = 1, int mod = 0);
		void add(const int& modifier);
		void add(const DiceRoll& source);
		void clear();
		string spec() const;

		void add(const string& specification);
		void set(const Die& dice, int count = 1, int mod = 0);
		void set(const string& specification);

		operator string() const;

		bool operator==(const DiceRoll& rhs) const;
		bool operator==(const string& specification) const;

		DiceRoll& operator=(const DiceRoll& rhs);
		DiceRoll& operator=(const string& specification);

		DiceRoll operator+(const DiceRoll& rhs) const;
		DiceRoll operator+(const int& mod) const;
		DiceRoll operator+(const string& specification) const;

		DiceRoll& operator+=(const DiceRoll& rhs);
		DiceRoll& operator+=(const int& mod);
		DiceRoll& operator+=(const string& specification);
	};

	// Located in p2em_core.cpp
	struct DamageSpec {
		string type;
		DiceRoll roll;

		static const string TAGNAME;
		static const string DAMAGE_TYPE_TAGNAME;
		static const string DAMAGE_DICE_TAGNAME;

		DamageSpec();
		DamageSpec(const xml::Element* source);

		void parse(const xml::Element* source);
	};

	// Located in p2em_core.cpp
	struct Action : public Traited {
		string name;
		string type;
		ActionCount action_count;
		int modifier;
		vector<DamageSpec> damages;
		vector<string> effects;

		static const string TAGNAME;
		static const string ACTION_TYPE_TAGNAME;
		static const string ACTION_COUNT_TAGNAME;
		static const string ACTION_NAME_TAGNAME;
		static const string ACTION_MODIFIER_TAGNAME;
		static const string ACTION_RESULTS_TAGNAME;
		static const string ACTION_RESULTS_EFFECT_TAGNAME;

		Action();
		Action(const xml::Element* source);

		void parse(const xml::Element* source);
	};

	// Located in p2em_core.cpp
	struct SpellDesc {
		string name;
		string comment;
		int slotcount;

		static const string TAGNAME;
		static const string NAME_TAGNAME;
		static const string COMMENT_TAGNAME;
		static const string SLOTS_TAGNAME;
		static const string COUNT_AT_WILL;
		static const string COMMENT_NULL;
		static const int ICOUNT_AT_WILL;
		static const int ICOUNT_NULL;

		SpellDesc();
		SpellDesc(const xml::Element* source);

		void parse(const xml::Element* source);
	};

	// Located in p2em_core.cpp
	struct SpellLevelSummary {
		int level;
		int heightening;
		int slotcount;
		vector<SpellDesc> spells;

		static const string TAGNAME;
		static const string LEVEL_TAGNAME;
		static const string HEIGHTENING_TAGNAME;
		static const string SLOTS_TAGNAME;
		static const string LEVEL_CANTRIP;
		static const string LEVEL_CONTINU;
		static const string COUNT_AT_WILL;
		static const int ILEVEL_CANTRIP;
		static const int ILEVEL_CONTINU;
		static const int IHEIGHT_NULL;
		static const int ICOUNT_AT_WILL;
		static const int ICOUNT_NULL;

		SpellLevelSummary();
		SpellLevelSummary(const xml::Element* source);

		void parse(const xml::Element* source);
	};

	// Located in p2em_core.cpp
	struct SpellList {
		SpellTradition tradition;
		SpellCastingType type;
		int difficulty;
		CondMod attack;
		bool has_attack;
		vector<SpellLevelSummary> levels;

		static const string TAGNAME;
		static const string TRADITION_TAGNAME;
		static const string TYPE_TAGNAME;
		static const string DC_TAGNAME;
		static const string ATK_TAGNAME;
		static const int IATK_NULL;

		static SpellTradition s2trad(const string& strad);
		static string trad2s(const SpellTradition& trad);
		static SpellCastingType s2type(const string& stype);
		static string type2s(const SpellCastingType& sctype);

		SpellList();
		SpellList(const xml::Element* source);

		void parse(const xml::Element* source);
	};

	class Object : public Traited, public Updateable {
	private:
		umap<string, Attribute*> _attrs;
		umap<string, bool> _ownership;
	protected:
		uint _id;
		string _name;
	public:
		static const string NULL_NAME_STR;
		static const uint NULL_ID;

		Object(const Registry& reg);
		Object(const Registry& reg, const string& name);
		Object(const Registry& reg, const uint& id);
		Object(const Registry& reg, const uint& id, const string& name);
		Object(const Registry& reg, const vector<string>& traits);
		Object(const Registry& reg, const vector<string>& traits, const string& name);
		Object(const Registry& reg, const vector<string>& traits, const uint& id);
		Object(const Registry& reg, const vector<string>& traits, const uint& id, const string& name);
		Object(const Object& source); // May want to fully support, with some sort of Attribute.copy() virtual function
		~Object();

		bool hasAttr(const string& attrname) const;
		bool hasAttr(const Attribute& attr) const;
		void addAttr(Attribute& attr);
		// void addAttr(const string& attrname); // Does not make sense, with Attribute being an abstract class
		// void addAttr(Attribute& attr, const string& attrname); // Does not make sense unless we can rename/reconstruct Attributes
		void removeAttr(const Attribute& attr);
		void removeAttr(const string& attrname);

		string get(const string& attrname) const;
		string get(const Attribute& attr) const;
		int getInt(const string& attrname) const;
		int getInt(const Attribute& attr) const;
		uint getUInt(const string& attrname) const;
		uint getUInt(const Attribute& attr) const;
		double getD(const string& attrname) const;
		double getD(const Attribute& attr) const;

		virtual uint id() const;
		virtual string name() const;
		virtual void name(const string& newname);

		virtual Registry& reg() const = 0;

		virtual const Attribute& operator[](const string& attrname) const;
		virtual Attribute& operator[](const string& attrname);
		virtual const Attribute& operator[](const Attribute& attr) const;
		virtual Attribute& operator[](const Attribute& attr);

		virtual bool operator==(const Object& rhs) const;
		// virtual Object& operator=(const Object& rhs); // Need to have a reason (i.e. copy constructor) for this
	};

	// Each instance (not template / definition) of a monster
	class Monster : public Object {
	private:
		void _init(Core& core);
		NumVal _hitpoints;

		static void _update_hp(Updateable *monster);
	protected:
		Core* _core;
	public:
		static const string TAGNAME;
		static const string NAME_TAGNAME;
		static const string LEVEL_TAGNAME;
		static const string RARITY_TAGNAME;
		static const string ALIGNMENT_TAGNAME;
		static const string SIZE_TAGNAME;
		static const string TYPES_TAGNAME;
		static const string TYPES_TYPE_TAGNAME;
		static const string PERCEPTION_TAGNAME;
		static const string LANGUAGES_TAGNAME;
		static const string LANGUAGES_LANG_TAGNAME;
		static const string SKILLS_TAGNAME;
		static const string SKILLS_SKILL_TAGNAME;
		static const string ABILITIES_TAGNAME;
		static const string STRENGTH_TAGNAME;
		static const string DEXTERITY_TAGNAME;
		static const string CONSTITUTION_TAGNAME;
		static const string INTELLIGENCE_TAGNAME;
		static const string WISDOM_TAGNAME;
		static const string CHARISMA_TAGNAME;
		static const string ITEMS_TAGNAME;
		static const string ITEMS_ITEM_TAGNAME;
		static const string INTERACTIONS_TAGNAME;
		static const string ARMOR_TAGNAME;
		static const string SAVES_TAGNAME;
		static const string SAVES_FORT_TAGNAME;
		static const string SAVES_REFL_TAGNAME;
		static const string SAVES_WILL_TAGNAME;
		static const string SAVES_MOD_TAGNAME;
		static const string HITPOINTS_TAGNAME;
		static const string IMMUNITIES_TAGNAME;
		static const string IMMUNITIES_IMM_TAGNAME;
		static const string WEAKNESSES_TAGNAME;
		static const string WEAKNESSES_WEAK_TAGNAME;
		static const string RESISTANCES_TAGNAME;
		static const string RESISTANCES_RES_TAGNAME;
		static const string OFFTURN_TAGNAME;
		static const string ONTURN_TAGNAME;
		static const string SPEEDS_TAGNAME;
		static const string SPEEDS_UNIT_TAGNAME;
		static const string SPEEDS_SPEED_TAGNAME;
		static const string ACTIONS_TAGNAME;
		static const string SPELLS_TAGNAME;

		NumVal level;
		string rarity;
		AlignmentAttr alignment;
		CreatureSize size;
		vector<string> types;
		CondMod perception;
		vector<string> languages;
		vector<CondMod> skills;
		CondMod strength;
		CondMod dexterity;
		CondMod constitution;
		CondMod intelligence;
		CondMod wisdom;
		CondMod charisma;
		vector<string> items;
		vector<Feat> interactions;
		CondMod armor;
		CondMod fortitude;
		CondMod reflex;
		CondMod will;
		vector<CondModOpt> saves;
		CondMod maxhp;
		NumVal temphp;
		NumVal damage;
		vector<string> immunities;
		vector<CondMod> weaknesses;
		vector<CondMod> resistances;
		vector<Feat> offturn_feats;
		string speed_units;
		vector<CondMod> speeds;
		vector<Action> actions;
		vector<SpellList> spelllists;
		vector<Feat> onturn_feats;

		Monster(Core& core);
		Monster(Core& core, const string& name);
		Monster(Core& core, const uint& id);
		Monster(Core& core, const uint& id, const string& name);
		Monster(Core& core, const vector<string>& tags);
		Monster(Core& core, const vector<string>& tags, const string& name);
		Monster(Core& core, const vector<string>& tags, const uint& id);
		Monster(Core& core, const vector<string>& tags, const uint& id, const string& name);
		Monster(Core& core, const xml::Element* templ);
		Monster(const Monster& source);

		Core& core() const;
		Registry& reg() const override;

		int hp() const;

		void parse(const xml::Element* templ);
	};
}

#endif // !__P2EM_CORE_H__