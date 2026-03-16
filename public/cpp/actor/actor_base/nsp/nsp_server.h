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

#include "actor/actor_base/ndb_modular.h"
#include "actor/actor_base/nsp/nsp.h"
#include "tools/threadtools.h"
#include "tools/pb_field.h"
#include "tools/tools.h"
#include "tools/type.h"

namespace ngl
{
	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	class nsp_server
	{
		// # "Datafield"
		static operator_field						m_operator_field;
		// # Whichnode whichdata
		static std::map<i64_actorid, care_data>		m_care;
		// # Alldata node
		static std::set<i64_nodeid>					m_nodereadalls;
		// # Alldata node
		static std::set<i64_nodeid>					m_nodewritealls;
		// # Partialdata node
		static std::set<i64_nodeid>					m_nodepart;
		// # Data
		static ndb_modular<ENUMDB, T, TDerived>*	m_dbmodule;
	public:
		using tnsp_server = nsp_server<ENUMDB, TDerived, T>;

		enum
		{
			esend_maxcount = 100,
		};

		// # Registerhandle
		static void init(ndb_modular<ENUMDB, T, TDerived>* adbmodule);

		// # Registerresponse
		static void channel_register_reply(i64_actorid aactorid);

		// # Registerresponsedata
		static void channel_channel_data(i64_actorid aactorid, const np_channel_register<T>* recv);

		// # Registersynchronize nodeinfo
		static void channel_dataid_sync(i64_actorid aactorid, const np_channel_register<T>* recv);

		// # Noderegister
		static void handle(TDerived*, const message<np_channel_register<T>>& adata);

		// # Nodeexit
		static void handle(TDerived*, const message<np_channel_exit<T>>& adata);

		// # Node data
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