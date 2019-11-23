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
		P2E_ATTR_MODVAL,

		P2E_ATTR_UNKNOWN,
	};

	enum ExceptionCode {
		P2EMON_ERR_INVALID_ID,
		P2EMON_ERR_INVALID_NAME,
		P2EMON_ERR_DUPLICATE_MONSTER,
		P2EMON_ERR_MONSTER_NOT_FOUND,
		P2EMON_ERR_MONSTER_MISMATCH,
		P2EMON_ERR_ID_CREATION_TIMEOUT,

		P2EMON_ERR
	};

	const string P2EMonExceptionStrings[P2EMON_ERR + 1] = {
		"Invalid monster name received!",
		"Invalid monster id received!",
		"Cannot create duplicate monsters!",
		"Cannot find monster!",
		"Monster id does not match recorded id!",
		"Could not create new monster ID!",

		"Monster error!"
	};

	class Core;
	class Exception;
	class AttrGen; 
	template<typename Tval_t> class Attribute;
	class Object;
	class Duplicatable;
	class Monster;

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

	class Duplicatable {
	public:
		Duplicatable();

		virtual Duplicatable& operator=(const Duplicatable& src) const = 0;
	};

	class AttrVal : public Duplicatable {
	public:
		AttrVal();

		virtual AttrType attrType() const = 0;

		AttrVal& operator=(const AttrGen& attr);
	};

	class Updatable : public AttrVal {
	protected:
		vector<Updatable&> _depedencies;
		vector<Updatable&> _parents;
		vector<void *()> _callbacks;
	public:
		void hasDependency(const Updatable& dep) const;
		void addDependency(Updatable& dep);
		void removeDependency(Updatable& dep);
		void hasParent(const Updatable& parent) const;
		void addParent(Updatable& parent);
		void removeParent(Updatable& parent);
		void hasCallback(void(*callback)()) const;
		void addCallback(void(*callback)());
		void removeCallback(void(*callback)());
		void onupdate();
	};

	template<typename T_num_t>
	class ModVal : public Updatable {
	protected:
		T_num_t _base;
		T_num_t _offset;
	public:
		ModVal();
		ModVal(const T_num_t& base);

		ModVal& operator=(const ModVal& source);

		T_num_t val() const;

		operator T_num_t() const;

		bool operator==(const ModVal& val2) const;
		bool operator==(const T_num_t& val2) const;
	};

	class AttrGen {
	protected:
		AttrType _type;
		vector<AttrGen&> _depedencies;
		vector<AttrGen&> _parents;
		vector<void *()> _callbacks;
	public:
		AttrGen();
		AttrGen(AttrType type);
		AttrGen(AttrType type, const void* value);
		AttrGen(const AttrVal& value);
		AttrGen(const string& value);
		AttrGen(uint value);
		AttrGen(int value);
		AttrGen(double value);
		AttrGen(modval value);
		AttrGen(const AttrGen* oldattr);

		AttrType type() const;

		virtual const void* value() const = 0;
		void* value();

		virtual AttrGen& copy() const = 0;

		AttrGen& operator=(const string& value);
		AttrGen& operator=(uint value);
		AttrGen& operator=(int value);
		AttrGen& operator=(double value);
		AttrGen& operator=(modval value);
		AttrGen& operator=(const AttrVal& value);
		template<typename Tval_t>
		AttrGen& operator=(const Attribute<Tval_t>& value);

		operator string() const;
		operator uint() const;
		operator int() const;
		operator double() const;
		operator modval() const;
		template<typename Tval_t>
		operator Attribute<Tval_t>() const;

		bool operator==(const AttrGen& attr) const;

	};

	template<typename Tval_t>
	class Attribute : public AttrGen {
	protected:
		Tval_t _value;
	public:
		Attribute();
		Attribute(const Tval_t& value);

		AttrGen& copy() const;

		const void* value() const;
		const Tval_t& value() const;
		Tval_t& value();
		void value(const Tval_t& value);

		Attribute& operator=(const Attribute& attr);
		Attribute& operator=(const Tval_t& value);
		Attribute& operator=(const AttrGen& attr);

		operator Tval_t() const;

		bool operator==(const Attribute& attr) const;
		bool operator==(const Tval_t &attr) const;
	};

	class Object {
	protected:
		umap<string, AttrGen> _attrs;
	public:
		Object();
		Object(const Object& oldobj);

		bool has(const string& attr) const;

		const AttrGen& operator[](const string& name) const;
		AttrGen& operator[](const string& name);

		void add(const string& name, const AttrGen& attr);
		void add(const string& name, AttrType type, const void* value);

		void remove(const string& name);

		bool operator==(const Object& obj) const;
	};

	class AbilityScore : public Attribute<modval> {

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
		Attribute<modval> strength;
		Attribute<modval> dexterity;
		Attribute<modval> constitution;
		Attribute<modval> intelligence;
		Attribute<modval> wisdom;
		Attribute<modval> charisma;

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