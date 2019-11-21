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

	enum P2EMonExceptionCode {
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
	class P2EMonException;
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

	class P2EMonException : public std::exception {
	private:
		string _msg;
		P2EMonExceptionCode _code;
	public:
		P2EMonException(P2EMonExceptionCode excode);
		P2EMonException(P2EMonExceptionCode excode, const string& msg);

		P2EMonExceptionCode code() const;
		string msg() const;
	};

	// Each instance of a monsterW
	class Monster {
	protected:
		Core* _core;
		uint _id;
		string _name;
		umap<string, modval> _attrs;

		void _init(Core* core, uint id);
	public:
		Monster(Core* core);
		Monster(Core* core, const string& name);
		Monster(Core* core, uint id);
		Monster(Core* core, uint id, const string& name);

		uint id() const;
		string name() const;

		void id(uint new_id);
		void name(const string& new_name);
	};
}

#endif // !__P2EM_CORE_H__