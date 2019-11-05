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

namespace p2em_core {

	using string = std::string;
	template<typename T_unit_t> using vector = std::vector<T_unit_t>;
	template<typename T_key_t, typename T_val_t> using omap = std::map<T_key_t, T_val_t>;
	template<typename T_key_t, typename T_val_t> using umap = std::unordered_map<T_key_t, T_val_t>;
	typedef unsigned int uint;

	class Creature {
	private:
		uint _id;
		uint _name;
		uint _strength;
	public:
		Creature(uint id);
		Creature(uint id, string name);

		uint id() const;
		string name() const;

		void id(uint new_id);
		string name(string& new_name);
	};
}

#endif // !__P2EM_CORE_H__