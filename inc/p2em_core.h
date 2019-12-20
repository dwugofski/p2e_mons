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

#define STRING_BUFFER_SIZE 256

namespace p2em_core {

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
	class Monster;
	class Object;

	struct Registry {
		virtual bool has(uint id) const = 0;
		virtual uint newid() const = 0;
	};

	class Core : public Registry {
	protected:
		umap<uint, Monster*> _monsters;
	public:
		Core();

		bool has(uint id) const override;
		uint newid() const override;
		bool has(const Monster* monster) const;
		const Monster* get(uint id) const;

		void add(Monster* monster);
		void remove(Monster* monster);
		void remove(uint id);
		Monster* get(uint id);

		const Monster* operator[](uint id) const;
		Monster* operator[](uint id);
	};

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

	class Attribute : public Updateable {
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

		virtual Attribute& operator=(const Attribute& source);
		virtual Attribute& operator=(const string& source) = 0; // Should support an empty string as "reset to default," can produce Exceptions (P2EMON_ERR_INVALID_VALUE)
	};

	template<typename T_num_t>
	class NumVal : public Attribute {
	protected:
		T_num_t _value;
	public:
		NumVal();
		NumVal(const T_num_t& value);
		NumVal(const string& name);
		NumVal(const string& name, const T_num_t& value);
		NumVal(const NumVal& source);
		NumVal(const Attribute& source);

		virtual AttrType attrType() const override;
		virtual string toString() const override;

		// Core functions for value change (do not access _value directly outside of this)
		virtual T_num_t value() const;
		virtual void value(const T_num_t& nuval);
		// --end

		virtual bool validate(const string& value) const override;

		virtual NumVal& operator=(const NumVal& source);
		virtual NumVal& operator=(const T_num_t& value);
		virtual NumVal& operator=(const string& value);

		virtual operator T_num_t() const;

		virtual bool operator==(const NumVal& comp) const;
		virtual bool operator==(const T_num_t& comp) const;
		virtual T_num_t operator+(const T_num_t& rhs);
		virtual T_num_t operator+(const NumVal& rhs);
		virtual NumVal& operator+=(const T_num_t& rhs);
		virtual NumVal& operator+=(const NumVal& rhs);
		virtual T_num_t operator-(const T_num_t& rhs);
		virtual T_num_t operator-(const NumVal& rhs);
		virtual NumVal& operator-=(const T_num_t& rhs);
		virtual NumVal& operator-=(const NumVal& rhs);
		virtual T_num_t operator*(const T_num_t& rhs);
		virtual T_num_t operator*(const NumVal& rhs);
		virtual NumVal& operator*=(const T_num_t& rhs);
		virtual NumVal& operator*=(const NumVal& rhs);
		virtual T_num_t operator/(const T_num_t& rhs);
		virtual T_num_t operator/(const NumVal& rhs);
		virtual NumVal& operator/=(const T_num_t& rhs);
		virtual NumVal& operator/=(const NumVal& rhs);
	};

	template<typename T_num_t>
	class OverrideableNumVal : public NumVal<T_num_t> {
	protected:
		bool _override;
	public:
		OverrideableNumVal();
		OverrideableNumVal(const string& name);
		OverrideableNumVal(const T_num_t& value);
		OverrideableNumVal(const string& name, const T_num_t& value);

		// Core functions for value change (do not access _value, _override directly outside of this)
		// virtual T_num_t value() const; // No reason to change from NumVal.value()
		virtual void value(const T_num_t& newval) override;
		virtual bool overridden() const;
		virtual void set_override(bool new_override);
		// --end

		virtual void toggle_override();
	};

	template<typename T_num_t>
	class ModVal : public OverrideableNumVal<T_num_t> {
	protected:
		NumVal<T_num_t>& _base;
		NumVal<T_num_t>& _offset;
		bool _ownsbase;
		bool _ownsoffset;

		void _link(); // Called by constructor functions
		vector<T_num_t> _splitstring(const string& newval) const;
	public:
		ModVal();
		ModVal(const string& name);
		ModVal(const T_num_t& value);
		ModVal(const string& name, const T_num_t& value);
		ModVal(NumVal<T_num_t>& base);
		ModVal(const string& name, NumVal<T_num_t>& base);
		ModVal(NumVal<T_num_t>& base, NumVal<T_num_t>& offset);
		ModVal(const string& name, NumVal<T_num_t>& base, NumVal<T_num_t>& offset);
		ModVal(NumVal<T_num_t>& base, NumVal<T_num_t>& offset, const T_num_t& value);
		ModVal(const string& name, NumVal<T_num_t>& base, NumVal<T_num_t>& offset, const T_num_t& value);
		ModVal(const ModVal& source);
		~ModVal();

		virtual string toString() const override;

		// Core functions for value change (do not access _value, _base, _offset, _override directly outside of this)
		virtual T_num_t value() const override;
		// virtual void value(const T_num_t& val); // Handled by OverrideableNumVal.value(const T_num_t&)
		virtual T_num_t base() const;
		virtual void base(const T_num_t& base);
		virtual T_num_t offset() const;
		virtual void offset(const T_num_t& offset);
		virtual void onupdate() override;
		// --end

		virtual bool validate(const string& value) const override;

		virtual ModVal& operator=(const ModVal& source);
		virtual ModVal& operator=(const string& source);

		virtual bool operator==(const ModVal& val2) const;
	};

	// May want to consider making this more generic...
	template<typename T_num_t>
	class AggVal : public OverrideableNumVal<T_num_t> {
	protected:
		vector<NumVal<T_num_t>*> _values;
	public:
		AggVal();
		AggVal(const string& name);
		AggVal(const T_num_t& baseval);
		AggVal(const string& name, const T_num_t& baseval);
		AggVal(NumVal<T_num_t>& startingval);
		AggVal(const string& name, NumVal<T_num_t>& startingval);
		AggVal(NumVal<T_num_t>& startingval, const T_num_t& baseval);
		AggVal(const string& name, NumVal<T_num_t>& startingval, const T_num_t& baseval);
		AggVal(const vector<NumVal<T_num_t>*>& startingvals);
		AggVal(const string& name, const vector<NumVal<T_num_t>*>& startingvals);
		AggVal(const vector<NumVal<T_num_t>*>& startingvals, const T_num_t& baseval);
		AggVal(const string& name, const vector<NumVal<T_num_t>*>& startingvals, const T_num_t& baseval);

		// Core functions for value change (do not access _value, _values directly outside of this)
		virtual vector<const NumVal<T_num_t>*> items() const;
		virtual vector<NumVal<T_num_t>*> items();
		virtual bool hasItem(const NumVal<T_num_t>& item) const;
		virtual void addItem(NumVal<T_num_t>& newitem);
		virtual void removeItem(NumVal<T_num_t>& olditem);
		virtual void onupdate() = 0;
		virtual void removeParent(Updateable& oldparent) override; // Necessary, as items (parents) need to be removed from the _values list as well
		// --end
	};

	template<typename T_num_t>
	class SumVal : public AggVal<T_num_t> {
	public:
		SumVal();
		SumVal(const string& name);
		SumVal(const T_num_t& baseval);
		SumVal(const string& name, const T_num_t& baseval);
		SumVal(NumVal<T_num_t>& startingval);
		SumVal(const string& name, NumVal<T_num_t>& startingval);
		SumVal(NumVal<T_num_t>& startingval, const T_num_t& baseval);
		SumVal(const string& name, NumVal<T_num_t>& startingval, const T_num_t& baseval);
		SumVal(const vector<NumVal<T_num_t>&>& startingvals);
		SumVal(const string& name, const vector<NumVal<T_num_t>&>& startingvals);
		SumVal(const vector<NumVal<T_num_t>&>& startingvals, const T_num_t& baseval);
		SumVal(const string& name, const vector<NumVal<T_num_t>&>& startingvals, const T_num_t& baseval);

		// Core functions for value change (do not access _value, _values directly outside of this)
		virtual T_num_t value() const override;
		// virtual void value(const T_num_t& newvalue); // Call the OverrideableNumVal.value(const T_num_t&) virtual
		virtual void onupdate() override;
		// --end
	};

	template<typename T_num_t, typename T_src_t>
	class TransVal : public NumVal<T_num_t> {
	protected:
		NumVal<T_src_t>& _source;
		bool _ownsref;
	public:
		TransVal();
		TransVal(const string& name);
		TransVal(NumVal<T_src_t>& src);
		TransVal(const string& name, NumVal<T_src_t>& src);
		~TransVal();

		// Core functions for value change (do not access _source, _value directly outside of this)
		virtual T_num_t value() const override;
		virtual void value(const T_num_t& newvalue) override;
		virtual void value(const T_src_t& newvalue);
		virtual void setSrc(NumVal<T_src_t>& src);
		virtual const NumVal<T_src_t>& getSrc() const;
		virtual NumVal<T_src_t>& getSrc();
		virtual void onupdate() override;
		// --end
	};

	template<typename T_num_t>
	class MaxVal : public AggVal<T_num_t> {
	public:
		MaxVal();
		MaxVal(const string& name);
		MaxVal(const T_num_t& baseval);
		MaxVal(const string& name, const T_num_t& baseval);
		MaxVal(NumVal<T_num_t>& startingval);
		MaxVal(const string& name, NumVal<T_num_t>& startingval);
		MaxVal(NumVal<T_num_t>& startingval, const T_num_t& baseval);
		MaxVal(const string& name, NumVal<T_num_t>& startingval, const T_num_t& baseval);
		MaxVal(const vector<NumVal<T_num_t>&>& startingvals);
		MaxVal(const string& name, const vector<NumVal<T_num_t>&>& startingvals);
		MaxVal(const vector<NumVal<T_num_t>&>& startingvals, const T_num_t& baseval);
		MaxVal(const string& name, const vector<NumVal<T_num_t>&>& startingvals, const T_num_t& baseval);

		// Core functions for value change (do not access _value, _values directly outside of this)
		virtual T_num_t value() const override;
		// virtual void value(const T_num_t& newvalue); // Call the OverrideableNumVal.value(const T_num_t&) virtual
		virtual void onupdate() override;
		// --end
	};

	template<typename T_num_t>
	class MinVal : public AggVal<T_num_t> {
	public:
		MinVal();
		MinVal(const string& name);
		MinVal(const T_num_t& baseval);
		MinVal(const string& name, const T_num_t& baseval);
		MinVal(NumVal<T_num_t>& startingval);
		MinVal(const string& name, NumVal<T_num_t>& startingval);
		MinVal(NumVal<T_num_t>& startingval, const T_num_t& baseval);
		MinVal(const string& name, NumVal<T_num_t>& startingval, const T_num_t& baseval);
		MinVal(const vector<NumVal<T_num_t>&>& startingvals);
		MinVal(const string& name, const vector<NumVal<T_num_t>&>& startingvals);
		MinVal(const vector<NumVal<T_num_t>&>& startingvals, const T_num_t& baseval);
		MinVal(const string& name, const vector<NumVal<T_num_t>&>& startingvals, const T_num_t& baseval);

		// Core functions for value change (do not access _value, _values directly outside of this)
		virtual T_num_t value() const override;
		// virtual void value(const T_num_t& newvalue); // Call the OverrideableNumVal.value(const T_num_t&) virtual
		virtual void onupdate() override;
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

	template<typename T_num_t>
	struct CondModOpt : public NumVal<T_num_t> {
		string description;
		bool selected;

		CondModOpt();
		CondModOpt(const T_num_t& val);
		CondModOpt(const string& desc);
		CondModOpt(const T_num_t& val, const string& desc);
		CondModOpt(const bool& sel);
		CondModOpt(const T_num_t& val, const bool& sel);
		CondModOpt(const string& desc, const bool& sel);
		CondModOpt(const T_num_t& val, const string& desc, const bool& sel);
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

	template<typename T_num_t>
	class CondMod : public ModVal<T_num_t> {
	private:
		void _init();
	protected:
		SumVal<T_num_t> _agger;
		vector<CondModOpt<T_num_t>*> _options;
		vector<bool> _ownership;
	public:
		CondMod();
		CondMod(const string& name);
		CondMod(NumVal<T_num_t>& base);
		CondMod(const string& name, NumVal<T_num_t>& base);
		CondMod(const T_num_t& value);
		CondMod(const string& name, const T_num_t& value);
		CondMod(NumVal<T_num_t>& base, const T_num_t& value);
		CondMod(const string& name, NumVal<T_num_t>& base, const T_num_t& value);
		CondMod(const CondMod& source);
		~CondMod();

		// Core functions for value change (do not access _value, _alignment directly outside of this)
		// virtual T_num_t value() const; // ModVal.value() should be good if the updating of ModVal._offset is proper
		// virtual void value(const T_num_t& value); // The ModVal.value(...) should be good
		// virtual T_num_t base() const;
		// virtual void base(const T_num_t& newbase);
		// virtual T_num_t offset() const;
		// virtual void offset(const NumVal<T_num_t>& newoffset);
		virtual bool hasOption(const CondModOpt<T_num_t>& testval) const;
		virtual int addOption(CondModOpt<T_num_t>& newvalue, bool own = false);
		virtual int createOption(T_num_t val = 0, string desc = "", bool selected = false);
		virtual int createOption(const CondModOpt<T_num_t>& source);
		virtual void removeOption(const int& index);
		virtual void removeOption(const CondModOpt<T_num_t>& oldvalue);
		virtual vector<const CondModOpt<T_num_t>*> options() const;
		virtual vector<CondModOpt<T_num_t>*> options();
		virtual int indexOf(const CondModOpt<T_num_t>& value) const;
		virtual uint size() const;
		// --end
		
		virtual const CondModOpt<T_num_t>& operator[](const int& index) const;
		virtual CondModOpt<T_num_t>& operator[](const int& index);
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
	};

	class Feat : public Traited {
	public:
		string name;
		ActionCount action_count;
		string description;
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

	struct DamageSpec {
		string type;
		DiceRoll roll;
	};

	struct Action : public Traited {
		string name;
		string type;
		ActionCount action_count;
		int modifier;
		vector<DamageSpec> damages;
		vector<string> effects;
	};

	struct SpellLevelSummary {
		int level;
		int heightening;
		int slotcount;
		vector<string> spells;
	};

	struct SpellList {
		SpellTradition tradition;
		SpellCastingType type;
		int difficulty;
		int attack;
		bool has_attack;
		vector<SpellLevelSummary> levels;
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

		virtual const Registry& reg() const = 0;
		virtual Registry& reg() = 0;

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
		NumVal<int> _hitpoints;

		static void _update_hp(Updateable *monster);
	protected:
		Core* _core;
	public:
		NumVal<int> level;
		AlignmentAttr alignment;
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
		CondMod<int> maxhp;
		NumVal<int> temphp;
		NumVal<int> damage;
		vector<string> immunities;
		vector<CondMod<int>> weaknesses;
		vector<CondMod<int>> resistances;
		vector<Feat> offturn_feats;
		vector<CondMod<int>> speeds;
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
		Monster(const Monster& source);

		Core& core() const;
		Registry& reg() const override;

		int hp() const;
	};
}

#endif // !__P2EM_CORE_H__