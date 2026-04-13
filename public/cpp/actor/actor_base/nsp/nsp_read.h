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
	class nsp_read
	{
	public:
		using tnsp_read = nsp_read<TDerived, TACTOR, T>;
	private:
		static std::once_flag											m_isregister;
				
		// Actor that owns this NSP reader instance.
		TDerived*														m_actor = nullptr;

		// User callbacks for row changes / deletes / load completion.
		nsp_callback<T>													m_call;

		// Tracks per-area NSP registration and first-sync state.
		nsp_regload														m_regload;

		// Describes which rows this reader subscribes to.
		care_data														m_care;

		// Per-actor-type field permissions used when copying received rows.
		operator_field													m_operator_field;

		// Nodes that must receive a later np_channel_exit<T> when this reader goes away.
		std::set<i64_actorid>											m_exit;

		// Registration retry timers owned by this reader instance.
		std::map<i16_area, int64_t>										m_regtimer;

		// Local mirror of synchronized rows.
		std::map<i64_actorid, T>										m_data;

		void clear_timer(i16_area aarea);

		void clear_timer();
	public:
		// Fetch the per-actor reader singleton.
		static nsp_read<TDerived, TACTOR, T>& instance(i64_actorid aactorid);

		// Create a reader that subscribes to all rows.
		static nsp_read<TDerived, TACTOR, T>& instance_readall(TDerived* aactor, const std::set<i32_fieldnumber>& afieldnumbers);

		// Create a reader that subscribes only to selected rows.
		static nsp_read<TDerived, TACTOR, T>& instance_readpart(
			TDerived* aactor, const std::set<i32_fieldnumber>& afieldnumbers, const std::set<i64_actorid>& aids
		);

		// Re-tag a row id with the owner actor type used by the backing NSP server.
		static i64_actorid to_actorid(i64_actorid adataid);

		// Register change / delete / load-complete callbacks.
		void set_changedatafun(const std::function<void(int64_t, const T&, bool)>& afun);

		void set_deldatafun(const std::function<void(int64_t)>& afun);

		void set_loadfinishfun(const std::function<void()>& afun);

		// Start registration timers toward the target NSP servers.
		void init();

		// Unregister from NSP servers and destroy the per-actor singleton.
		void exit();

		// Read one mirrored row or the full mirrored map.
		const T* getconst(i64_dataid adataid);

		const std::map<i64_actorid, T>& get_mapconst();

		// Handle initial/incremental data sync, registration, and field updates.
		void handle(TDerived* aactor, const message<np_channel_data<T>>& adata);

		void handle(TDerived* aactor, const message<np_channel_check<T>>& adata);

		void handle(TDerived* aactor, const message<np_channel_register_reply<T>>& adata);

		void handle(TDerived* aactor, const message<np_channel_dataid_sync<T>>& adata);
	};

	template <typename TDerived, typename TACTOR, typename T>
	std::once_flag nsp_read<TDerived, TACTOR, T>::m_isregister;
}//namespace ngl
