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

#include "actor/actor_base/nsp/nsp_server.h"

namespace ngl
{
	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::init(ndb_modular<ENUMDB, T, TDerived>* adbmodule)
	{
		m_dbmodule = adbmodule;
		m_operator_field.init(false);

		// The NSP server listens for peer registration, peer writes, and peer exit.
		actor::register_actor_s<TDerived, np_channel_register<T>>(
			[](TDerived* aactor, const message<np_channel_register<T>>& adata)
			{
				nsp_server<ENUMDB, TDerived, T>::handle(aactor, adata);
			}
		);
		// Peer-to-peer data synchronization messages.
		actor::register_actor_s<TDerived, np_channel_data<T>>(
			[](TDerived* aactor, const message<np_channel_data<T>>& adata)
			{
				nsp_server<ENUMDB, TDerived, T>::handle(aactor, adata);
			}
		);
		// Peer disconnect notifications so subscription state can be cleaned up.
		actor::register_actor_s<TDerived, np_channel_exit<T>>(
			[](TDerived* aactor, const message<np_channel_exit<T>>& adata)
			{
				nsp_server<ENUMDB, TDerived, T>::handle(aactor, adata);
			}
		);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::channel_register_reply(i64_actorid aactorid)
	{
		auto pro = std::make_shared<np_channel_register_reply<T>>();
		pro->m_actorid = m_dbmodule->get_actor()->id_guid();
		pro->m_nodereadalls = m_nodereadalls;
		pro->m_nodewritealls = m_nodewritealls;
		for (const auto& [lnodeid, lcare] : m_care)
		{
			if (aactorid == lnodeid)
			{
				continue;
			}
			pro->m_care.emplace(lnodeid, lcare.get_core());
		}
		// Return both peer scopes and merged field rules so the new peer can participate immediately.
		pro->m_node_fieldnumbers = m_operator_field.field_numbers();
		actor::send_actor(aactorid, nguid::make(), pro);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::channel_channel_data(i64_actorid aactorid, const np_channel_register<T>* recv)
	{
		i64_actorid lnspserver = m_dbmodule->get_actor()->id_guid();
		const i16_actortype ltype = nguid::type(aactorid);
		std::function<std::shared_ptr<np_channel_data<T>>()> lmalloc = [lnspserver]()->std::shared_ptr<np_channel_data<T>>
			{
				auto pro = std::make_shared<np_channel_data<T>>();
				pro->m_actorid = lnspserver;
				pro->m_firstsynchronize = true;
				return pro;
			};
		auto pro = lmalloc();
		auto lpush = [&pro, ltype](const T& adata)
			{
				T& ldst = pro->m_data[adata.mid()];
				if (!m_operator_field.field_copy(ltype, adata, ldst, true))
				{
					ldst = adata;
				}
			};

		int32_t lindex = 0;
		if (recv->m_all)
		{
			// Full subscribers receive the whole dataset in bounded chunks.
			m_dbmodule->foreach([&pro, &lindex, &lmalloc, &lpush, aactorid](const data_modified<T>& adata)
				{
					const T& ldata = *adata.getconst();
					lpush(ldata);
					++lindex;
					if (lindex % esend_maxcount == 0)
					{
						actor::send_actor(aactorid, nguid::make(), pro);
						pro = lmalloc();
					}
				});
		}
		else
		{
			// Partial subscribers receive only their declared read/write row sets.
			auto lfun = [&pro, &lindex, &lmalloc, &lpush, aactorid](const std::set<i64_actorid>& aids)
				{
					for (i64_actorid id : aids)
					{
						data_modified<T>* lpmodifieddata = m_dbmodule->find(id);
						if (lpmodifieddata == nullptr)
						{
							continue;
						}
						const T* lpdata = lpmodifieddata->getconst();
						if (lpdata == nullptr)
						{
							continue;
						}
						lpush(*lpdata);
						++lindex;
						if (lindex % esend_maxcount == 0)
						{
							actor::send_actor(aactorid, nguid::make(), pro);
							pro = lmalloc();
						}
					}
				};

			lfun(recv->m_readids);
			lfun(recv->m_writeids);
		}

		if (lindex % esend_maxcount == 0)
		{
			// Finish with an empty tail packet when the last chunk landed exactly on the boundary.
			pro = lmalloc();
		}
		pro->m_recvfinish = true;
		actor::send_actor(aactorid, nguid::make(), pro);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::channel_dataid_sync(i64_actorid aactorid, const np_channel_register<T>* recv)
	{
		auto pro = std::make_shared<np_channel_dataid_sync<T>>();
		pro->m_actorid = aactorid;
		pro->m_read = recv->m_read;
		pro->m_all = recv->m_all;
		if (!recv->m_all)
		{
			pro->m_readpart = recv->m_readids;
			pro->m_writepart = recv->m_writeids;
		}
		pro->m_field = recv->m_field;

		std::set<i64_nodeid> lnodes;
		for (const auto& [lnodeid, _] : m_care)
		{
			if (lnodeid != aactorid)
			{
				lnodes.insert(lnodeid);
			}
		}
		// Every existing peer needs the new peer's scope so route and exit state stay complete.
		actor::send_actor(lnodes, nguid::make(), pro);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::handle(TDerived*, const message<np_channel_register<T>>& adata)
	{
		const np_channel_register<T>* recv = adata.get_data();
		i64_actorid lactorid = recv->m_actorid;

		if (m_care.find(lactorid) != m_care.end())
		{
			return;
		}

		if (m_nodewritealls.find(lactorid) != m_nodewritealls.end())
		{
			return;
		}

		if (m_nodereadalls.find(lactorid) != m_nodereadalls.end())
		{
			return;
		}

		const i16_actortype ltype = nguid::type(lactorid);
		auto [lcare_it, _] = m_care.try_emplace(lactorid);
		care_data& lcare = lcare_it->second;
		if (recv->m_read)
		{
			if (recv->m_all)
			{
				lcare.init(true);
				m_nodereadalls.insert(lactorid);
			}
			else
			{
				lcare.init(recv->m_readids);
			}
		}
		else
		{
			if (recv->m_all)
			{
				lcare.init(false);
				m_nodewritealls.insert(lactorid);
			}
			else
			{
				lcare.init(recv->m_readids, recv->m_writeids);
			}
		}
		
		// Field permissions are merged by peer actor type, not by individual peer id.
		m_operator_field.set_field(ltype, recv->m_field);
		m_nodepart.insert(lactorid);

		// Registration completes in three steps: reply with peer state, send snapshot, announce the new peer.
		channel_register_reply(lactorid);
		channel_channel_data(lactorid, recv);
		channel_dataid_sync(lactorid, recv);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::handle(TDerived*, const message<np_channel_exit<T>>& adata)
	{
		const np_channel_exit<T>* recv = adata.get_data();
		i64_actorid lactorid = recv->m_actorid;

		m_nodepart.erase(lactorid);
		m_nodewritealls.erase(lactorid);
		m_nodereadalls.erase(lactorid);
		m_care.erase(lactorid);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::handle(TDerived*, const message<np_channel_data<T>>& adata)
	{
		const np_channel_data<T>* recv = adata.get_data();
		i64_actorid lactorid = recv->m_actorid;

		nsp_handle_print<TDerived>::print("nsp_server", m_dbmodule->nactor(), recv);
		care_data* lcare = tools::findmap(m_care, lactorid);
		if (lcare == nullptr)
		{
			tools::no_core_dump();
			return;
		}

		i16_actortype ltype = nguid::type(lactorid);

		for (const auto& [ldataid, lsrc] : recv->m_data)
		{
			if (!lcare->is_write(ldataid))
			{
				// Peers may only mutate rows they explicitly registered as writable.
				tools::no_core_dump();
				return;
			}
			data_modified<T>& ldata = m_dbmodule->get(ldataid);
			m_operator_field.field_copy(ltype, lsrc, *ldata.get(), true);
		}
		for (const i64_actorid ldataid : recv->m_deldata)
		{
			if (!lcare->is_write(ldataid))
			{
				tools::no_core_dump();
				return;
			}
			m_dbmodule->erase(ldataid);
		}
	}
}//namespace ngl
