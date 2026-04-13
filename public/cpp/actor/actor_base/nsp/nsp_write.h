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
// File overview: Declares interfaces for nsp.

#pragma once

#include "actor/actor_base/nsp/nsp.h"

#include <ranges>

namespace ngl
{
	template <typename TDerived, typename TACTOR, typename T>
	class nsp_write
	{
		static std::once_flag											m_isregister;

		// Actor that owns this NSP writer instance.
		TDerived*														m_actor = nullptr;
		
		// User callbacks for row changes / deletes / load completion.
		nsp_callback<T>													m_call;

		// Tracks per-area NSP registration and first-sync state.
		nsp_regload														m_regload;

		// Describes which rows this writer reads and/or may modify.
		care_data														m_care;

		// Per-peer subscription info learned from the NSP server.
		std::map<i64_actorid, care_data>								m_othercare;

		// Per-actor-type field permissions used when copying synchronized rows.
		operator_field													m_operator_field;

		// Nodes that subscribe to all rows in read-only mode.
		std::set<i64_nodeid>											m_nodereadalls;

		// Nodes that subscribe to all rows in read/write mode.
		std::set<i64_nodeid>											m_nodewritealls;		

		// Local mirror of synchronized rows.
		std::map<i64_actorid, T>										m_data;

		// Nodes that must receive a later np_channel_exit<T> when this writer goes away.
		std::set<i64_actorid>											m_exit;

		// Registration retry timers owned by this writer instance.
		std::map<i16_area, int64_t>										m_regtimer;

		// Rows changed since the last incremental broadcast.
		std::set<i64_dataid>											m_changeids;

		// Rows deleted since the last incremental broadcast.
		std::set<i64_dataid>											m_delids;

		void clear_timer(i16_area aarea);

		void clear_timer();
	public:
		// Fetch the per-actor writer singleton.
		static nsp_write<TDerived, TACTOR, T>& instance(i64_actorid aactorid);

		// Create a writer that mirrors every row and may write every row.
		static nsp_write<TDerived, TACTOR, T>& instance_writeall(
			TDerived* aactor
			, const std::set<i32_fieldnumber>& areadfieldnumbers  			/* Read-onlywhichfield */
			, const std::set<i32_fieldnumber>& awritefieldnumbers			/* Read-onlywhichfield */
		);

		// Create a writer with explicit read/write row and field scopes.
		static nsp_write<TDerived, TACTOR, T>& instance_writepart(
			TDerived* aactor
			, const std::set<i32_fieldnumber>& areadfieldnumbers			/* Read-onlywhichfield */
			, const std::set<i32_fieldnumber>& awritefieldnumbers			/* / Whichfield */
			, const std::set<i64_actorid>& areadids							/* Read-onlywhichdata */
			, const std::set<i64_actorid>& awriteids						/* / Whichdata */
		);

		// Re-tag a row id with the owner actor type used by the backing NSP server.
		static i64_actorid to_actorid(i64_actorid adataid);

		// Start registration timers toward the target NSP servers.
		void init();

		// Register change / delete / load-complete callbacks.
		void set_changedatafun(const std::function<void(int64_t, const T&, bool)>& afun);

		void set_deldatafun(const std::function<void(int64_t)>& afun);

		void set_loadfinishfun(const std::function<void()>& afun);

		// Add a new row and mark it dirty for the next incremental broadcast.
		T* add(i64_dataid adataid);

		// Get one row and mark it dirty for the next incremental broadcast.
		T* get(i64_dataid adataid);

		// Delete one row and mark it for the next incremental broadcast.
		bool erase(i64_dataid adataid);

		// Read one row or the full mirrored map without marking it dirty.
		const T* getconst(i64_dataid adataid);

		// //////////////////// This this (prevent data) ////////////////////////
		// This [get_mapconst] [get]
		//std::map<i64_actorid, T>& get_map()
		//{
		//	std::transform(m_data.begin(), m_data.end(), std::inserter(m_changeids, m_changeids.end()),
		//		[](const auto& pair) { return pair.first; });
		//	return m_data;
		//}
		///////

		const std::map<i64_actorid, T>& get_mapconst();

		// Unregister from NSP servers and destroy the per-actor singleton.
		void exit();

		// Flush accumulated incremental changes to interested peers.
		void change();
		static void change(i64_actorid aactorid);

		// Handle initial/incremental data sync, registration, and field updates.
		void handle(TDerived* aactor, const message<np_channel_data<T>>& adata);

		void handle(TDerived*, const message<np_channel_check<T>>& adata);

		void handle(TDerived*, const message<np_channel_register_reply<T>>& adata);

		void handle(TDerived*, const message<np_channel_dataid_sync<T>>& adata);

		// Remove one peer from the local subscription cache.
		void handle(TDerived*, const message<np_channel_exit<T>>& adata);
	};

	template <typename TDerived, typename TACTOR, typename T>
	std::once_flag nsp_write<TDerived, TACTOR, T>::m_isregister;
}//namespace ngl
