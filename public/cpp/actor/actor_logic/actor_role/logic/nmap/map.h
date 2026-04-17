/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Game map that owns grids and manages entity placement and movement.

#pragma once

#include "actor/actor_logic/actor_role/logic/nmap/grids.h"
#include "actor/actor_base/core/actor.h"

namespace ngl
{
	class aoimap
	{
		grids											m_grids;		// View range
		obstacles										m_obstacles;	// Translated comment.
		int												m_tabid;		// Maptable configdata
		std::map<i64_actorid, unit_role*>				m_roleunit;		// Player
		std::map<i64_actorid, unit_monster*>			m_monster;		// Translated comment.
	public:
		aoimap();

		// Find a role unit by actor ID
		unit_role*		find_role(i64_actorid aid);

		// Find a monster unit by actor ID
		unit_monster*	find_monster(i64_actorid aid);

		// Find any unit (role or monster) by actor ID
		unit*			find_unit(i64_actorid aid);

		// Initialize the map from a table config ID
		bool init(int32_t atid);

		// Copy a unit's data into a pbdb::UNIT protobuf
		bool copy_unit(unit* aunit, pbdb::UNIT* aUNIT);

		// Get a unit's protobuf data by actor ID
		bool copy_unit(i64_actorid aid, pbdb::UNIT* aunit);

		// Enter the map at a given position
		virtual bool enter(unit* aunit, int32_t ax, int32_t ay);

		// Leave the map
		virtual void leave(unit* aunit);

		// Synchronize position info to nearby units
		void sync_position(unit* aunit, int32_t agridid);

		// Translated comment.
		bool move(unit* aunit, int32_t ax, int32_t ay);

		// Change a unit's facing direction
		void change_angle(i64_actorid aunitid, int32_t aangle);

		// Translated comment.
		void change_speed(i64_actorid aunitid, int32_t aspeed);
		
		// Update a unit's position
		void change(i64_actorid aunitid, pbdb::POSITION& aposition);

		void update(int64_t ams);
	};
}// namespace ngl