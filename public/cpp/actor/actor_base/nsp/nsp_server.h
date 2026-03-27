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

#include "actor/actor_base/core/ndb_modular.h"
#include "actor/actor_base/nsp/nsp.h"
#include "tools/tools/tools_thread.h"
#include "tools/tools/tools_pb_field.h"
#include "tools/tools.h"
#include "tools/type.h"

namespace ngl
{
	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	class nsp_server
	{
		// Per-actor-type field permissions merged from every registered peer.
		static operator_field						m_operator_field;
		// Peer actor id -> subscribed row scope.
		static std::map<i64_actorid, care_data>		m_care;
		// Peers that subscribe read-only to all rows.
		static std::set<i64_nodeid>					m_nodereadalls;
		// Peers that subscribe read/write to all rows.
		static std::set<i64_nodeid>					m_nodewritealls;
		// Peers with partial subscriptions.
		static std::set<i64_nodeid>					m_nodepart;
		// DB module that owns the authoritative dataset.
		static ndb_modular<ENUMDB, T, TDerived>*	m_dbmodule;
	public:
		using tnsp_server = nsp_server<ENUMDB, TDerived, T>;

		enum
		{
			esend_maxcount = 100,
		};

		// Bind the authoritative DB module and register channel handlers.
		static void init(ndb_modular<ENUMDB, T, TDerived>* adbmodule);

		// Send the current peer/subscription snapshot back to a newly registered node.
		static void channel_register_reply(i64_actorid aactorid);

		// Stream the initial row snapshot to a newly registered node.
		static void channel_channel_data(i64_actorid aactorid, const np_channel_register<T>* recv);

		// Broadcast the new node's subscription scope to existing peers.
		static void channel_dataid_sync(i64_actorid aactorid, const np_channel_register<T>* recv);

		// Handle peer registration, peer exit, and incremental writes from peers.
		static void handle(TDerived*, const message<np_channel_register<T>>& adata);

		static void handle(TDerived*, const message<np_channel_exit<T>>& adata);

		static void handle(TDerived*, const message<np_channel_data<T>>& adata);
	};

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	operator_field nsp_server<ENUMDB, TDerived, T>::m_operator_field;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	std::map<i64_actorid, care_data> nsp_server<ENUMDB, TDerived, T>::m_care;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	std::set<i64_nodeid> nsp_server<ENUMDB, TDerived, T>::m_nodereadalls;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	std::set<i64_nodeid> nsp_server<ENUMDB, TDerived, T>::m_nodewritealls;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	std::set<i64_nodeid> nsp_server<ENUMDB, TDerived, T>::m_nodepart;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	ndb_modular<ENUMDB, T, TDerived>* nsp_server<ENUMDB, TDerived, T>::m_dbmodule;

}//namespace ngl
