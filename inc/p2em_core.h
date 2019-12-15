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

#define P2EMON_NULL_ID				0
#define P2EMON_NULL_NAME_STRING		"__NULL__"

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

	enum AttrType {
		P2E_ATTR_INT,
		P2E_ATTR_UINT,
		P2E_ATTR_DOUBLE,
		P2E_ATTR_STRING,
		P2E_ATTR_NUMVAL,
		P2E_ATTR_MODVAL,
		P2E_ATTR_SUMVAL,
		P2E_ATTR_MAXVAL,
		P2E_ATTR_MINVAL,
		P2E_ATTR_TRANSVAL,

		P2E_ATTR_UNKNOWN,
	};

	enum ExceptionCode {
		P2EMON_ERR_INVALID_ID,
		P2EMON_ERR_INVALID_NAME,
		P2EMON_ERR_DUPLICATE_MONSTER,
		P2EMON_ERR_MONSTER_NOT_FOUND,
		P2EMON_ERR_MONSTER_MISMATCH,
		P2EMON_ERR_ATTR_TYPE_MISMATCH,
		P2EMON_ERR_ID_CREATION_TIMEOUT,
		P2EMON_ERR_CIRCULAR_DEPS,
		P2EMON_ERR_INVALID_VALUE,

		P2EMON_ERR
	};

	const string P2EMonExceptionStrings[P2EMON_ERR + 1] = {
		"Invalid monster name received!",
		"Invalid monster id received!",
		"Cannot create duplicate monsters!",
		"Cannot find monster!",
		"Monster id does not match recorded id!",
		"Attribute value mismatch!",
		"Could not create new monster ID!",
		"Could not add dependency: Dependency lists must not be circular!",
		"Cannot set attribute to provided value!",

		"Monster error!"
	};

	class Core;
	class Exception;
	class Attribute;
	class Updateable;
	class Monster;
	class Object;

	class Core {
	protected:
		umap<uint, Monster*> _monsters;
	public:
		Core();

		bool has(uint id) const;
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
		vector<Updateable*> _depedencies;
		vector<Updateable*> _parents;
		vector<void (*)(Updateable*)> _callbacks;
		bool _paused = false;
	public:
		Updateable();
		~Updateable();

		bool hasDependency(const Updateable& dep, bool recurse) const;
		void addDependency(Updateable& dep);
		void removeDependency(Updateable& dep);
		bool hasParent(const Updateable& parent) const;
		void addParent(Updateable& parent);
		void removeParent(Updateable& parent);
		bool hasCallback(void(*callback)(Updateable*)) const;
		void addCallback(void(*callback)(Updateable*));
		void removeCallback(void(*callback)(Updateable*));
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

		virtual string name() const;
		virtual void name(const string& name);

		virtual AttrType attrType() const = 0;

		virtual string toString() const = 0;
		virtual bool validate(const string& source) const = 0; // Determines whether a string will behave well. 

		virtual Attribute& operator=(const Attribute& source) = 0;
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

		virtual AttrType attrType() const;
		virtual string toString() const;

		// Core functions for value change (do not access _value directly outside of this)
		virtual T_num_t value() const;
		virtual void value(const T_num_t& nuval);

		virtual bool validate(const string& value) const;

		virtual NumVal& operator=(const NumVal& source);
		virtual NumVal& operator=(const T_num_t& value);
		virtual Attribute& operator=(const Attribute& source);
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
		virtual void value(const T_num_t& newval);
		virtual bool overridden() const;
		virtual void set_override(bool new_override);

		virtual void toggle_override();

		virtual Attribute& operator=(const Attribute& source) = 0;
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

		virtual AttrType attrType() const;
		virtual string toString() const;

		// Core functions for value change (do not access _value, _base, _offset, _override directly outside of this)
		virtual T_num_t value() const;
		// virtual void value(const T_num_t& val); // Handled by OverrideableNumVal.value(const T_num_t&)
		virtual T_num_t base() const;
		virtual void base(const T_num_t& base);
		virtual T_num_t offset() const;
		virtual void offset(const T_num_t& offset);

		virtual bool validate(const string& value) const;
		virtual void onupdate();

		virtual ModVal& operator=(const ModVal& source);
		virtual Attribute& operator=(const Attribute& source);
		virtual ModVal& operator=(const string& source);

		// virtual operator T_num_t() const; // Do not override; ensure that _value is always correct

		virtual bool operator==(const ModVal& val2) const;
	};

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

		virtual AttrType attrType() const = 0;

		// Core functions for value change (do not access _value, _values directly outside of this)
		virtual vector<const NumVal<T_num_t>*> items() const;
		virtual vector<NumVal<T_num_t>*> items();
		virtual bool hasItem(const NumVal<T_num_t>& item) const;
		virtual void addItem(NumVal<T_num_t>& newitem);
		virtual void removeItem(const NumVal<T_num_t>& olditem);

		virtual void onupdate() = 0;

		//virtual Attribute& operator=(const Attribute& source);
	};

	template<typename T_num_t>
	class SumVal : public OverrideableNumVal<T_num_t> {
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

		virtual AttrType attrType() const;

		// Core functions for value change (do not access _value, _values directly outside of this)
		virtual T_num_t value() const;
		// virtual void value(const T_num_t& newvalue); // Call the OverrideableNumVal.value(const T_num_t&) virtual

		virtual void onupdate();

		//virtual Attribute& operator=(const Attribute& source);
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
		virtual T_num_t value() const;
		virtual void value(const T_num_t& newvalue);
		virtual void value(const T_src_t& newvalue);
		virtual void setSrc(NumVal<T_src_t>& src);
		virtual const NumVal<T_src_t>& getSrc() const;
		virtual NumVal<T_src_t>& getSrc();

		virtual void onupdate();

		//virtual Attribute& operator=(const Attribute& source);
	};

	template<typename T_num_t>
	class MaxVal : public OverrideableNumVal<T_num_t> {
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

		virtual AttrType attrType() const;

		// Core functions for value change (do not access _value, _values directly outside of this)
		virtual T_num_t value() const;
		// virtual void value(const T_num_t& newvalue); // Call the OverrideableNumVal.value(const T_num_t&) virtual

		virtual void onupdate();

		//virtual Attribute& operator=(const Attribute& source);
	};

	template<typename T_num_t>
	class MinVal : public OverrideableNumVal<T_num_t> {
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

		virtual AttrType attrType() const;

		// Core functions for value change (do not access _value, _values directly outside of this)
		virtual T_num_t value() const;
		// virtual void value(const T_num_t& newvalue); // Call the OverrideableNumVal.value(const T_num_t&) virtual

		virtual void onupdate();

		//virtual Attribute& operator=(const Attribute& source);
	};

	class Object {
	protected:
		umap<string, Attribute*> _attrs;
	};

	// Each instance of a monster
	class Monster : public Object {
	private:
		void _init(Core* core, uint id);
	protected:
		Core* _core;
		uint _id;
		string _name;
	public:

		Monster(Core* core);
		Monster(Core* core, const string& name);
		Monster(Core* core, uint id);
		Monster(Core* core, uint id, const string& name);

		uint id() const;
		void id(uint new_id);

		string name() const;
		void name(const string& new_name);
	};
}

#endif // !__P2EM_CORE_H__